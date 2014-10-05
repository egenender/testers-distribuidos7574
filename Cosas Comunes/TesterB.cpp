#include "TesterB.h"
#include "Resultado.h"


TesterB::TesterB(AtendedorDispositivos atendedor, Despachante despachante, PlanillaCoordinacion coordinacion, int numTester){
	this.atendedor = atendedor;
	this.despachante = despachante;
	this.numTester = numTester;
	/* ver como hacer la parte de la planilla general.. deberia crearse sola, pero 
	 * Hay que ver en el orden si conviene hacerlo aca.
	 * */
}

void TesterB::atenderDispositivos(){
	while (true) { //ver idem TesterA
		Dispositivo dispositivo = this.atendedor.obtenerDispositivoTesteado(this.numTester);
		this.planilla.procesarResultado(); //llama a planilla_local
		Resultado resultado(dispositivo.obtenerResultadosTesteo());
		if (resultado.esGrave()){
			atendedor.enviarOrdenApagado(dispositivo, this.numTester);
			OrdenReparacion orden(resultado);
			despachante.enviarOrdenReparacion(orden, this.numTester);
		}else{
			atendedor.enviarOrdenReinicio(dispositivo);
		}
		this.planilla.terminoProcesarResultado(dispositivo);//llama a planilla local para ver cuantos le quedan asignados
	}
}
