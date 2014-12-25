program REGISTER {
	version REGISTER_V1 {
		int GET_ID_DISPOSITIVO() = 1;
		int GET_ID_TESTER(int) = 2;
		int REGISTRAR_TESTER_ACTIVO(int) = 3;
		int DEVOLVER_ID_DISPOSITIVO(int) = 4;
		int DEVOLVER_ID_TESTER(int) = 5;
	} = 1;
} = 0x2fffffff;
