CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -lpthread

all: sensor monitor

sensor: sensor.c funcionesSensor.c comunicacion.c
	$(CC) $(CFLAGS) $^ -o $@

monitor: monitor.c funcionesMonitor.c comunicacion.c
	$(CC) $(CFLAGS) $^ -o $@

run: run_monitor run_sensor1 run_sensor2 

run_monitor: monitor
	@echo "Ejecutando monitor en segundo plano..."
	@./monitor -b 10 -t file-temp.txt -h file-ph.txt -p pipe1 

run_sensor1: sensor
	@echo "Ejecutando sensor en segundo plano..."
	@./sensor -s 1 -t 3 -f datos_temperatura.txt -p pipe1

run_sensor2: sensor
	@echo "Ejecutando sensor en segundo plano..."
	@./sensor -s 2 -t 3 -f datos_ph.txt -p pipe1

clean:
	@rm -f sensor monitor 
