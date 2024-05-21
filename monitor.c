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
Fichero: Programa principal para la gestion del monitor
***********************************************************************/

#include <stdio.h> // Biblioteca estándar de E/S
#include <stdlib.h> // Biblioteca estándar para funciones de utilidad general
#include <unistd.h> // Biblioteca estándar de llamadas al sistema y constantes POSIX
#include <fcntl.h> // Biblioteca estándar para la manipulación de archivos y descriptores de archivo
#include <string.h> // Biblioteca estándar para manipulación de cadenas de caracteres
#include <sys/types.h> // Biblioteca de tipos y estructuras estándar para sistemas UNIX 
#include <sys/stat.h> // Biblioteca estándar para manipular la información de archivos y directorios.
#include <stdatomic.h> //Biblioteca que ofrece tipos y funciones para operaciones atómicas en variables
#include "hilos.h" // Interfaz artesanal
#include "funcionesMonitor.h" // Interfaz artesanal

// Función principal del monitor
int main(int argc, char *argv[]) {
    int tam_buffer = 0;
    char *file_temp = NULL;
    char *file_ph = NULL;
    char *pipe_nominal = NULL;

    // Verificar los argumentos de línea de comandos
    if (argc != 9) {
        fprintf(stderr, "Uso: %s -b <tam_buffer> -t <file_temp> -h <file_ph> -p <pipe_nominal>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parsear los argumentos de línea de comandos
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-b") == 0) {
            tam_buffer = atoi(argv[i+1]);
        } else if (strcmp(argv[i], "-t") == 0) {
            // Verificar la extensión del archivo de temperatura
            if (strstr(argv[i+1], ".txt") == NULL) {
                fprintf(stderr, "El archivo de temperatura debe tener la extensión .txt\n");
                exit(EXIT_FAILURE);
            }
            file_temp = argv[i+1];
        } else if (strcmp(argv[i], "-h") == 0) {
            // Verificar la extensión del archivo de pH
            if (strstr(argv[i+1], ".txt") == NULL) {
                fprintf(stderr, "El archivo de pH debe tener la extensión .txt\n");
                exit(EXIT_FAILURE);
            }
            file_ph = argv[i+1];
        } else if (strcmp(argv[i], "-p") == 0) {
            pipe_nominal = argv[i+1];
        } else {
            fprintf(stderr, "Argumento desconocido: %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    // Crear el pipe nominal si no existe
    crearPipeNominal(pipe_nominal);

    // Crear el semáforo
    sem_init(&empty_ph, 0, tam_buffer);
    sem_init(&full_ph, 0, 0);
    sem_init(&empty_temp, 0, tam_buffer);
    sem_init(&full_temp, 0, 0);

    // Crear e inicializar los hilos
    pthread_t h_recolector, h_ph, h_temperatura;

    HiloArgs args = {file_temp, file_ph, tam_buffer, pipe_nominal};

    pthread_create(&h_recolector, NULL, hilo_recolector, (void *)&args);
    pthread_create(&h_ph, NULL, hilo_ph, (void *)&args);
    pthread_create(&h_temperatura, NULL, hilo_temperatura, (void *)&args);

    // Esperar a que los hilos terminen
    pthread_join(h_recolector, NULL);
    pthread_join(h_ph, NULL);
    pthread_join(h_temperatura, NULL);

    sem_destroy(&empty_ph);
    sem_destroy(&full_ph);
    sem_destroy(&empty_temp);
    sem_destroy(&full_temp);

    printf("\nFinalización del procesamiento de medidas\n");

    return 0;
}
