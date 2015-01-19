********************************************************
COMPILAR: 
	- sender.c
	- listener.c
	- timeout.c

EJECUTAR EN EL SIGUIENTE ORDEN: 
	- listener <id>
	- sender <id>

Por cada maquina ejecutar listener o (exclusivo) sender. En total, ejecutar 1 y solo 1 sender.
Para estar mas seguros, ejecutar todos los listeners antes que el sender.

NOTA: modificar en el archivo "config" las direcciones y 
puertos necesarios respetando el formato
El archivo se tiene que llamar config, es un archivo de texto
(es lo mismo que contiene config.txt)

Es importante que la maquina que es master tenga configurada su propia IP.
*********************************************************
