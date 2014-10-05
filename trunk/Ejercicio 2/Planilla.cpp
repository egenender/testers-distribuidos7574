#include "Planilla.h"

Planilla::Planilla(int tester){
	//Creo la planilla local con el numero de tester
	//crear memoria compartida.. alguien la debe inicializar antes
}


//es un test&set, luego registra en la planilla local y ademas llama al procesarNuevoDispositivo de planilla local
bool Planilla::agregar(Dispositivo dispositivo){
	cantidad.mutex();
	if (cantidad.get() == MAXIMO_CANTIDAD_DISPOSITIVOS){
		cantidad.demux();
		return false;
	}
	cantidad.set(cantidad.get() + 1);
	cantidad.demux();
	
	planilla_local.registrar(dispositivo);
	planilla_local.procesarNuevoDispositivo();
	return true;
}
	
//llama a la planilla local, para cederle el control al TesterB si estaba esperando, y se lo pone en ocupado (para no
//poder arrebatarle el control nuevamente)
void Planilla::terminoProcesarNuevoDispositivo(){
	if (planilla_local.testerBEspera()){
		planilla_local.pasarControlTesterB();
	}
}

//llama a la planilla local, si el TesterA no esta ocupado, me cedera el control
void Planilla::procesarResultado(){
	planilla_local.procesarResultado();
}

		
//Resta 1 a la shm + elimina de la planilla local al dispositivo, y hace el cambio de control en caso de ser necesario
//(TesterA esta esperando y no hay mas Resultados a ser procesados)
void Planilla::terminoProcesarResultado(Dispositivo dispositivo){
	cantidad.mutex();
	cantidad.set(cantidad.get() - 1);
	cantidad.demux();
	
	planilla_local.desregistrar(dispositivo);
	planilla_local.analizarCambioControl();
}
