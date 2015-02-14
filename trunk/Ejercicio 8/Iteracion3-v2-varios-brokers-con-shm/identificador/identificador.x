/* identificador.x: rpc encargado de la entrega de identificadores unicos en el sistema */
program IDENTIFICADORPROG {
    version IDENTIFICADORVERS {
        int GETIDDISPOSITIVO() = 1;
        int GETIDTESTERCOMUN() = 2;
        int GETIDTESTERESPECIAL() = 3;
        int DEVOLVERIDTESTERCOMUN(int) = 4;
        int DEVOLVERIDTESTERESPECIAL(int) = 5;
    } = 1;
} = 0x20000001;
