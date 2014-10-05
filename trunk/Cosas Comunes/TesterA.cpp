#include "TesterA.h"
#include "Programa.h"

TesterA::TesterA(AtendedorDispositivos atendedor, int numTester){
	this.atendedor = atendedor;
	this.numTester = numTester;
	/* ver como hacer la parte de la planilla general.. deberia crearse sola, pero 
	 * Hay que ver en el orden si conviene hacerlo aca.
	 * */
}

void TesterA::atenderDispositivos(){
	while (true){ //ponerle una signal de finalizacion? o lo dejamos corriendo para siempre?
		Dispositivo dispositivo = this.atendedor.obtenerDipositivo();
		if (!this.planilla.agregar(Dispositivo)){ 
			this.atendedor.noHayLugarPara(dispositivo);
			continue;
		}
		Programa programa;
		this.atendedor.enviarPrograma(dispositivo, programa.getPrograma(), this.numTester);
		this.planilla.terminoProcesarNuevoDispositivo(); //llama a la planilla local
	}
}
