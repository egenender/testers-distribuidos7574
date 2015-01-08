struct par_broker_tester{
	int id_broker;
	int id_tester;
};

program REGISTER {
	version REGISTER_V1 {
		int GET_ID_DISPOSITIVO() = 1;
		int GET_ID_TESTER(int) = 2;
		int REGISTRAR_TESTER_ACTIVO(par_broker_tester) = 3;
		int DEVOLVER_ID_DISPOSITIVO(int) = 4;
		int DEVOLVER_ID_TESTER(int) = 5;
		int BROKER_DE_TESTER(int) = 6;
	} = 1;
} = 0x2fffffff;
