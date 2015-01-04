1. Instrucciones para la compilacion:
	- Unicamente es necesario escribir en la consola (parado en este directorio) el comando 'make'.
	- En caso que saliera algun error de compilacion, esto se debe a que en algunas maquinas se ha observado que es posible que el makefile omita la compilacion del Logger. Para resolver eso, se puede situar en el directorio 'logger' (dentro de este mismo directorio), y ejecutar el comando:
		~$ g++ -c Logger.cpp
	  Al volver al directorio del proyecto, ejecutar el comando make.

2. Instrucciones de ejecución:
	- Luego de haber compilado como es indicado en 1. se debe correr el programa 'iniciador', que se encarga de inicializar todos los ipcs (shm, semaforos, colas), y levantar todos los procesos auxiliares.
	El programa recibe 4 parametros: 
		<CANT_DISPOSITIVOS>: 		La cantidad total de dispositivos a lanzar
		<MIN_DISP>:		 			Cantidad minima de dispositivos a lanzar en cada ciclo
		<MAX_DISP>:					Cantidad maxima de dispositivos a lanzar en cada ciclo
		<TIEMPO_SEP_SIM_MILISEC>:	Tiempo, en milisegundos, de separación entre cada ciclo a ejecutar (i.e. el sleep entre cada ciclo, en milisegundos).
	
