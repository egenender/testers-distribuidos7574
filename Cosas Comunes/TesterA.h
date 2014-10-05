#ifndef TESTER_A_H
#define TESTER_A_H

class TesterA{
	private:
		Planilla planilla;
		AtendedorDispositivos atendedor;
		int numTester;
		
	public:
		TesterA(AtendedorDispositivos, int numTester);
		
		void atenderDispositivos();
}

#endif 
