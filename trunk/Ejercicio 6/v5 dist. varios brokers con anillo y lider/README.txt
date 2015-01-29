1. Instrucciones para la instalación:
	- Únicamente es necesario escribir en una terminal (parado en este directorio) el comando 'make setup', y luego ejecutar dicho programa (./setup). 
	Seguir las instrucciones de instalacion (que modifican ciertas constantes de los archivos fuente). Las direcciones de los brokers indicados en la instalación
	deben seguir el mismo orden para todas las máquinas.
2. Instrucciones para la compilacion:
	- Unicamente es necesario escribir en la consola (parado en este directorio) el comando 'make', o 'make all'.
	- En caso que saliera algun error de compilacion, esto se debe a que en algunas maquinas se ha observado que es posible que el makefile omita la compilacion del Logger. Para resolver eso, se puede situar en el directorio 'logger' (dentro de este mismo directorio), y ejecutar el comando:
		~$ g++ -c Logger.cpp
	Al volver al directorio del proyecto, ejecutar el comando make.
	
Los pasos 1 y 2 se pueden realizar en secuencia, utilizando la opción 'full_install' del makefile (i.e. 'make full_install'), que primero compila el instalador, luego lo ejectura,
realiza la instalación completa, elimina el instalador y compila el proyecto.

3. Instrucciones de ejecución:
	- Luego de haber compilado como es indicado en 1. se debe correr el programa 'iniciador', que se encarga de inicializar todos los ipcs (shm, semaforos, colas).
	El 'iniciador' recibe parámetros en función de la tarea a realizar.
	- Si el equipo será utilizado para lanzar dispositivos, testers comunes, testers especiales, o el servidor RPC, no es necesario pasarle ningún parámetro.
	- Si el equipo será utilizado como broker, pero como un slave broker (en el caso inicial), es necesario pasarle el parámetro: '-sb'.
	- Si el equipo será utilizado como broker, pero como un master broker (en el caso inicial), es necesario pasarle el parámetro '-mb'.
	
	Para el caso de los brokers, luego de haberse ejecutado se pasará a la etapa de armado del anillo, luego de lo cual se esperará poco menos de un minuto para lanzar el resto de los procesos
	del broker. Tal espera se debe a tener que esperar por el cierre de los canales de comunicaciones (i.e. tiempo de timeout de tcp) para que en caso de error, se pueda volver a formar el anillo en cualquier momento,
	y no caer en un caso de 'Address already in use'.
	
4. Ejecutar Dispositivos y Testers:
	- Para ejecutar un dispositivo, simplemente escribir en la consola (situado en el directorio del proyecto) './Dispositivo <Direccion>'. El parámetro de dirección es opcional, e indica 
	la dirección del broker al cual debe conectarse. En caso de no indicarse ninguno, se realizará de forma aleatoria. En caso de reintentar la conexión (porque luego de mucho tiempo, el dispositivo no
	fue atendido), se reintará con el mismo parámetro, o con ninguno si así fue invocado la primera vez.
	- Para ejecutar un tester común o especial, simplemente escribir en la consola (situado en el directorio del proyecto) './TesterComun <Direccion>' o './TesterEspecial <Direccion>' (segun corresponda). El parámetro de dirección es opcional, e indica
	la dirección del broker al cual debe conectarse. En caso de no indicarse ninguno, se realizará de forma por defecto.

5. Lanzamiento de varios dispositivos:
	- Para poder lanzar varios dispositivos (simulación de lanzada), debe ejecutarse el programa 'LanzaDisp' ('./LanzaDisp <Cantidad> <min_lanzada> <max_lanzada> <tiempo_entre_lanzadas>).
	Los parámetros indican:
		- Cantidad: Cantidad de dispositivos (totales) a ser lanzados en la simulación.
		- Min_lanzada: Cantidad mínima de dispositivos a ser lanzados en un lanzamiento (si eventualmente, faltaran menos dispositivos por ser lanzados que esta cantidad, se lanza la cantidad faltante)
		- Max_lanzada: cantidad máxima de dispositivos a ser lanzados en un lanzamiento.
		- Tiempo entre lanzadas: tiempo de espera entre que se lanza una tanda de dispositivos y otra.
	