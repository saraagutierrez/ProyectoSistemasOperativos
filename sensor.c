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
Fichero: Programa principal para la gestion del sensor
***********************************************************************/

#include <stdio.h> // Biblioteca estándar de E/S
#include <stdlib.h> // Biblioteca estándar para funciones de utilidad general
#include <unistd.h> // Biblioteca estándar de llamadas al sistema y constantes POSIX
#include <fcntl.h> // Biblioteca estándar para la manipulación de archivos y descriptores de archivo
#include <string.h> // Biblioteca estándar para manipulación de cadenas de caracteres
#include <time.h> //Biblioteca para el tiempo y las fechas
#include "comunicacion.h" // Interfaz artesanal
#include "funcionesSensor.h" // Interfaz artesanal

// Función principal del sensor
int main(int argc, char *argv[]) {
    int tipo_sensor = 0;  // Inicializa con un valor predeterminado
    int tiempo = 0;       // Inicializa con un valor predeterminado
    char *archivo = NULL; // Inicializa con NULL
    char *pipe_nominal = NULL; // Inicializa con NULL

    // Verificar los argumentos de línea de comandos
    if (argc != 9) {
        fprintf(stderr, "Uso: %s -s <tipo_sensor> -t <tiempo> -f <archivo> -p <pipe_nominal>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parsear los argumentos de línea de comandos
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-s") == 0) {
            tipo_sensor = atoi(argv[i+1]);
            if (tipo_sensor != 1 && tipo_sensor != 2) {
                fprintf(stderr, "El tipo de sensor debe ser 1(Temp) o 2(Ph)\n");
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[i], "-t") == 0) {
            tiempo = atoi(argv[i+1]);
        } else if (strcmp(argv[i], "-f") == 0) {
            // Verificar la extensión del archivo
            if (strstr(argv[i+1], ".txt") == NULL) {
                fprintf(stderr, "El archivo debe tener la extensión .txt\n");
                exit(EXIT_FAILURE);
            }
            archivo = argv[i+1];
        } else if (strcmp(argv[i], "-p") == 0) {
            pipe_nominal = argv[i+1];
        } else {
            fprintf(stderr, "Argumento desconocido: %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    // Enviar mediciones al proceso monitor
    enviarMediciones(tipo_sensor, tiempo, archivo, pipe_nominal);

    return 0;
}
