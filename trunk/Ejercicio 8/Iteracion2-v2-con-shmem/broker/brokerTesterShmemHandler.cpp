/* 
 * File:   brokerTesterShmemHandler.cpp
 * Author: ferno
 *
 * Created on February 12, 2015, 10:14 PM
 */

#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/msg.h>

#include "common/common.h"
#include "logger/Logger.h"

/*
 * 
 */
int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    
    // Creo la cola de memoria compartida
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_BROKER_SHMEM_HANDLER);
	int shMemQueue = msgget(key, IPC_CREAT | 0660);

    // Memoria compartida de la planilla general
    if (fork() == 0) {
        // Proceso hijo que se encarga de memoria compartida de planilla general
        // Inicializo la shmem
        TSharedMemoryPlanillaGeneral shmemPlanillaGeneral;
        shmemPlanillaGeneral.cantDispositivosSiendoTesteados = 0;
        for(int i = 0; i < MAX_DISPOSITIVOS_EN_SISTEMA; i++) {
            shmemPlanillaGeneral.idsPrivadosDispositivos[i] = false;
        }
        
        // Itero buscando requerimiento de shmem, se la envio y la espero de vuelta
        std::stringstream ss;
        while(true) {
            TSharedMemoryPlanillaGeneral req;
            ss << "Busco requerimiento de la memoria compartida de la planilla general";
            Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();
            int okRead = msgrcv(shMemQueue, &req, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), MTYPE_REQ_SHMEM_PLANILLA_GENERAL, 0);
            if(okRead == -1) {
                ss << "Error al intentar obtener un requerimiento de shmem de planilla general. Errno: " << strerror(errno);
                Logger::error(ss.str(), __FILE__);
                exit(1);
            }
            
            ss << "El tester/equipo de ID " << req.idSolicitante << " me pide la shmem de planilla general. Se la envio...";
            Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();
            
            shmemPlanillaGeneral.mtype = req.idSolicitante;
            int okSend = msgsnd(shMemQueue, &shmemPlanillaGeneral, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), 0);
            if(okSend == -1) {
                ss << "Error al intentar enviar shmem de planilla general al ID " << shmemPlanillaGeneral.mtype << ". Errno: " << strerror(errno);
                Logger::error(ss.str(), __FILE__);
                exit(1);
            }
            
            ss << "Espero devolucion de la shmem de planilla general";
            Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();
            
            okRead = msgrcv(shMemQueue, &shmemPlanillaGeneral, sizeof(TSharedMemoryPlanillaGeneral) - sizeof(long), MTYPE_DEVOLUCION_SHMEM_PLANILLA_GENERAL, 0);
            if(okRead == -1) {
                ss << "Error al esperar devolucion de la shmem de planilla general. Errno: " << strerror(errno);
                Logger::error(ss.str(), __FILE__);
                exit(1);
            }
            Logger::debug("Me devuelven la shmem de planilla general", __FILE__);
        }
    }

    // Memoria compartida de la planilla de asignacion
    if(fork() == 0) {
        // Proceso hijo que se encarga de memoria compartida de planilla asignacion
        // Inicializo la shmem
        TSharedMemoryPlanillaAsignacion shmemPlanillaAsignacion;
        for(int i = 0; i < MAX_DISPOSITIVOS_EN_SISTEMA; i++) {
            shmemPlanillaAsignacion.cantTareasEspecialesAsignadas[i].cantTareasEspecialesTotal = 0;
            shmemPlanillaAsignacion.cantTareasEspecialesAsignadas[i].cantTareasEspecialesTerminadas = 0;
            shmemPlanillaAsignacion.cantTestersEspecialesAsignados[i].cantTestersEspecialesTotal = 0;
            shmemPlanillaAsignacion.cantTestersEspecialesAsignados[i].cantTestersEspecialesTerminados = 0;
        }

        // Itero buscando requerimiento de shmem, se la envio y la espero de vuelta
        std::stringstream ss;
        while(true) {
            TSharedMemoryPlanillaAsignacion req;
            ss << "Busco requerimiento de la memoria compartida de la planilla de asignacion";
            Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();
            int okRead = msgrcv(shMemQueue, &req, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), MTYPE_REQ_SHMEM_PLANILLA_ASIGNACION, 0);
            if(okRead == -1) {
                ss << "Error al intentar obtener un requerimiento de shmem de planilla de asignacion. Errno: " << strerror(errno);
                Logger::error(ss.str(), __FILE__);
                exit(1);
            }
            
            ss << "El tester/equipo de ID " << req.idSolicitante << " me pide la shmem de planilla asignacion. Se la envio...";
            Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();
            
            shmemPlanillaAsignacion.mtype = req.idSolicitante;
            int okSend = msgsnd(shMemQueue, &shmemPlanillaAsignacion, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), 0);
            if(okSend == -1) {
                ss << "Error al intentar enviar shmem de planilla general al ID " << shmemPlanillaAsignacion.mtype << ". Errno: " << strerror(errno);
                Logger::error(ss.str(), __FILE__);
                exit(1);
            }
            
            ss << "Espero devolucion de la shmem de planilla general";
            Logger::debug(ss.str(), __FILE__); ss.str(""); ss.clear();
            
            okRead = msgrcv(shMemQueue, &shmemPlanillaAsignacion, sizeof(TSharedMemoryPlanillaAsignacion) - sizeof(long), MTYPE_DEVOLUCION_SHMEM_PLANILLA_ASIGNACION, 0);
            if(okRead == -1) {
                ss << "Error al esperar devolucion de la shmem de planilla asignacion. Errno: " << strerror(errno);
                Logger::error(ss.str(), __FILE__);
                exit(1);
            }
            
            Logger::debug("Me devuelven la shmem de planilla asignacion", __FILE__);
        }
    }

    return 0;
}

