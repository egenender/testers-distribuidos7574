#include <iostream>
#include <sys/msg.h>
#include <sys/shm.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include "../common/common.h"
#include "../ipc/Semaphore.h"
#include "../logger/Logger.h"
#include <string.h>

void copiarResultado(resultado_t* destino, resultado_t* origen){
	for (int i = 0; i < CANT_RESULTADOS; i++){
		destino[i].idDispositivo = origen[i].idDispositivo;
		destino[i].resultadosPendientes = origen[i].resultadosPendientes;
		destino[i].resultadosGraves = origen[i].resultadosGraves;
	}
}

void rearmar_anillo(int sig){
	Logger::error("Se cayo el anillo", __FILE__);
}

int main (int argc, char** argv){
	Logger::initialize(logFileName.c_str(), Logger::LOG_NOTICE);
	Logger::notice("Creo los ipcs necesarias", __FILE__);
	
	int broker_id = atoi(argv[1]);
	key_t key;
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_ENVIO_MENSAJES_TESTERS);
	int cola_hacia_testers = msgget(key, 0660 );
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_SHM_TESTERS);
	int cola_shm_testers = msgget(key, 0660 );
	
	key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_DEVOLUCION_SHM);
	int cola_requerimiento_shm = msgget(key, 0660);
	
	Semaphore puede_buscar(SEM_SHM_TESTERS_REQUERIMIENTO);
	puede_buscar.getSem();
	  
	key = ftok(ipcFileName.c_str(), SHM_TESTER_QUIERE_SHM);
    int shmtester = shmget(key, sizeof(int) , 0660);
    int* tester_shm = (int*)shmat(shmtester, NULL, 0);   
	
	Semaphore sem_tester_shm(SEM_MUTEX_TESTER_QUIERE_SHM);
	sem_tester_shm.getSem();
	
	key = ftok(ipcFileName.c_str(), SHM_NEXT_BROKER);
    int shmnext = shmget(key, sizeof(int) , 0660);
    int* next = (int*)shmat(shmnext, NULL, 0);   
	
	Semaphore sem_next(SEM_MUTEX_NEXT_BROKER);
	sem_next.getSem();
	
	key = ftok(ipcFileName.c_str(), SHM_VERSION);
    int shmversion = shmget(key, sizeof(uint64_t) , 0660 | IPC_CREAT);
    uint64_t* version_id = (uint64_t*)shmat(shmversion, NULL, 0);   
	
	signal(SIGUSR1, rearmar_anillo);
	
    Logger::notice("Termino la obtencion de ipcs", __FILE__);
    
	/* Fin Setup */
	TMessageAtendedor msg;
	
	while (true){
		std::stringstream ss;
		ss << "Espero por la shm para " << broker_id;
		Logger::debug(ss.str(), __FILE__);
		ss.str("");
		
		pid_t padre = getpid();
		pid_t timer = fork();
		if (timer == 0){
			sleep(TIEMPO_ESPERA_ANILLO);
			kill(padre, SIGUSR1);
			exit(0);
		}
		int ok_read = msgrcv(cola_shm_testers, &msg, sizeof(TMessageAtendedor) - sizeof(long), broker_id, 0);
		if (ok_read == -1){
			exit(0);
		}
		kill(timer, SIGINT);
		wait(NULL);
		Logger::debug("Obtengo shm. Reviso si hay un pedido de shm", __FILE__);
				
		sem_tester_shm.p();
		int id_tester_espera = *tester_shm;
		*tester_shm = 0;
		sem_tester_shm.v();
		
		if (id_tester_espera != 0){
			puede_buscar.v();//Permito al par de requerimiento que busque
				
			ss << "Tengo que enviarle al tester/broker " << id_tester_espera << " la shm";
			Logger::debug(ss.str(), __FILE__);
			ss.str("");
			
			TMessageAtendedor msg_envio;
			msg_envio.mtype = id_tester_espera;
			msg_envio.mtype_envio = id_tester_espera;
			msg_envio.tester = id_tester_espera;
			msg_envio.finalizar_conexion = 0;
			
			if (id_tester_espera < MAX_DISPOSITIVOS_EN_SISTEMA){ // ->Es un tester!
				//copiarResultado(msg_envio.resultados, msg.resultados);
				memcpy(msg_envio.resultados, msg.resultados, sizeof(resultado_t) * CANT_RESULTADOS);
			} else { // -> es un sub broker
				memcpy(&msg_envio.tabla, &msg.tabla, sizeof(tabla_testers_disponibles_t));
			}
			
			int ret = msgsnd(cola_hacia_testers, &msg_envio, sizeof(TMessageAtendedor) - sizeof(long), 0);
			if(ret == -1) {
				exit(1);
			}
			ss << "Envie shm a tester/broker " << id_tester_espera << ". Espero que me la devuelva";
			Logger::debug(ss.str(), __FILE__);
			ss.str("");
			
			ok_read = msgrcv(cola_requerimiento_shm, &msg_envio, sizeof(TMessageAtendedor) - sizeof(long), MTYPE_DEVOLUCION_SHM_TESTERS, 0);
			if (ok_read == -1){
				exit(0);
			}
			ss << "El tester/broker " << id_tester_espera << " me devolvio la shm";
			Logger::debug(ss.str(), __FILE__);
			ss.str("");
			
			if (id_tester_espera < MAX_DISPOSITIVOS_EN_SISTEMA){ // ->Es un tester!
				//copiarResultado(msg_envio.resultados, msg.resultados);
				memcpy(msg.resultados, msg_envio.resultados,sizeof(resultado_t) * CANT_RESULTADOS);
			} else { // -> es un sub broker
				memcpy(&msg.tabla, &msg_envio.tabla, sizeof(tabla_testers_disponibles_t));
			}
		} 
		
		msg.version++;
		*version_id = msg.version;
		
		sem_next.p();
		int siguiente = *next;
		sem_next.v();
		
		ss << "Paso la shm al siguiente broker: " << siguiente << ". SHM-version: " << msg.version;	
		Logger::debug (ss.str(), __FILE__);
		
		msg.mtype = siguiente;
		msg.mtype_envio = siguiente;
				
		int ret = msgsnd(cola_shm_testers, &msg, sizeof(TMessageAtendedor) - sizeof(long), 0);
		if (ret == -1) exit(0);
		
		
		if ( siguiente == broker_id ){
			usleep(1000); //Esperamos para el caso que hay un solo broker, para que no termine consumiendo el procesador
		}
		
	}
}

