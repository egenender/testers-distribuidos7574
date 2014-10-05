#ifndef PLANILLA_H_
#define PLANILLA_H_
#include <stdbool.h>

class Planilla{
	private:
		static const unsigned int MAXIMO_CANTIDAD_DISPOSITIVOS = 100;
	
		unsigned int cantidad//Memoria compartida de un int, entre todos los procesos---> TODO: PONERLO COMO SHMEMORY
		PlanillaCoordinacion planilla_local; //Memoria compartida entre un par de Procesos TesterA y TesterB
		
	public:
		Planilla(int NumTester);
		
		//es un test&set, luego registra en la planilla local y ademas llama al procesarNuevoDispositivo de planilla local
		bool agregar(Dispositivo dispositivo);
	
		//llama a la planilla local, para cederle el control al TesterB si estaba esperando, y se lo pone en ocupado (para no
		//poder arrebatarle el control nuevamente)
		void terminoProcesarNuevoDispositivo();		
		
		//llama a la planilla local, si el TesterA no esta ocupado, me cedera el control
		void procesarResultado(); 
		
		
		//Resta 1 a la shm + elimina de la planilla local al dispositivo, y hace el cambio de control en caso de ser necesario
		//(TesterA esta esperando y no hay mas Resultados a ser procesados)
		void terminoProcesarResultado(Dispositivo dispositivo);
}

#endif
