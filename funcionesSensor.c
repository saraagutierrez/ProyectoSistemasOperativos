/**********************************************************************
Autores: 
  - Sara Gutiérrez 
  - Leonardo Velázquez
  - Oscar Rodriguez
Fecha: 27 abril 2024
Materia: Sistemas Operativos
Pontificia Universidad Javeriana
Proyecto: Monitoreo de sensores
Tema: Desarrollo de un monitor de procesos para el sistema de monitoreo.
Fichero: Implementación de funciones para el manejo del sensor
***********************************************************************/

#include <stdio.h> // Biblioteca estándar de E/S
#include <stdlib.h> // Biblioteca estándar para funciones de utilidad general
#include <unistd.h> // Biblioteca estándar de llamadas al sistema y constantes POSIX
#include <fcntl.h> // Biblioteca estándar para la manipulación de archivos y descriptores de archivo
#include <string.h> // Biblioteca estándar para manipulación de cadenas de caracteres
#include <time.h> //Biblioteca para el tiempo y las fechas
#include "comunicacion.h" // Interfaz artesanal
#include "funcionesSensor.h" // Interfaz artesanal

#define BUF_SIZE 20

// Función para enviar mediciones al proceso monitor a través del pipe nominal
void enviarMediciones(int tipo_sensor, int tiempo, char *archivo, char *pipe_nominal) {
    FILE *fp;
    char buffer[BUF_SIZE];
    int fd;
    //time_t tiempo_actual;

    // Abrir el archivo de mediciones
    fp = fopen(archivo, "r");
    if (fp == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }


    // Abrir el pipe nominal para escritura
    fd = abrirPipeNominalEscritura(pipe_nominal);

    do{
      if (fd == -1) {
          fd = abrirPipeNominalEscritura(pipe_nominal);
          perror("Error al abrir el pipe nominal");
          sleep(1);
      }
    }while(fd < 0);

    // Leer las mediciones del archivo y enviarlas al proceso monitor
    while (fgets(buffer, BUF_SIZE, fp) != NULL) {

        // Verificar si la medición es válida
        float valor = atof(buffer);
        time_t current_time = time(NULL);

        MedicionSensor medicion = {tipo_sensor, valor, current_time};

      // Escribir el mensaje en la consola
      printf("\nEnviando medición al proceso monitor: Tipo sensor=%d, Valor=%f\n", medicion.tipoSensor, medicion.valor);

        // Escribir el mensaje en el pipe nominal
        if (write(fd, &medicion, sizeof(medicion)) == -1) {
            perror("Error al escribir en el pipe nominal");
            exit(EXIT_FAILURE);
        }

        // Esperar el tiempo especificado antes de la próxima medición
        sleep(tiempo);
    }

    // Cerrar el archivo y el pipe
    fclose(fp);
    close(fd);
}
