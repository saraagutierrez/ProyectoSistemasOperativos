# ProyectoSistemasOperativos

## Compilacion
#### Shell 1
$ gcc  sensor.c funcionesSensor.c comunicacion.c -o sensor
#### Shell 2
$ gcc  monitor.c funcionesMonitor.c comunicacion.c -o monitor

## Ejecucion con 2 sensores  
#### Shell 1
$ ./monitor -b 10 -t file-temp.txt -h file-ph.txt -p pipe1 
#### Shell 2
$ /sensor -s 1 -t 3 -f datos_temperatura.txt -p pipe1
#### Shell 3
$ ./sensor -s 2 -t 3 -f datos_ph.txt -p pipe1

## Con Makefile 
#### Shell 1
$ make run_monitor
#### Shell 2
$ make run_sensor1
#### Shell 3
$ make run_sensor2
