#ifndef TESTER_B_H
#define TESTER_B_H

class TesterB{
	private:
		Planilla planilla;
		int numTester;
		AtendedorDispositivos atendedor;
		Despachante despachante;
		
	public:
		TesterB(AtendedorDispositivos, Despachante, int numTester);
		
		void atenderDispositivos();
	
}
#endif
