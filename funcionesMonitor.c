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
Fichero: Implementación de funciones para el manejo del monitor
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
#include "comunicacion.h" // Interfaz artesanal
#include "funcionesMonitor.h" // Interfaz artesanal

//Declaración de semáforos para supervisar el llenado y vaciado de los buffers que contienen mediciones de temperatura y pH
sem_t empty_temp;
sem_t full_temp;
sem_t empty_ph;
sem_t full_ph;

//Declaración de variables para mantener el índice actual de los buffers de mediciones de pH y temperatura
int ph_index = 0;
int temp_index = 0;

//Declaración de un mutex para asegurar la exclusión mutua al acceder a secciones críticas del código
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Declaración de un mutex para garantizar la exclusión mutua al acceder a la variable de estado de envío
pthread_mutex_t mutexTerminado = PTHREAD_MUTEX_INITIALIZER;

//Declaración de una variable de condición para notificar a los hilos cuando se completa el envío de mediciones
pthread_cond_t cond_terminacion = PTHREAD_COND_INITIALIZER;

//Declaración de variable para indicar si el envío de mediciones ha finalizado
int envio_terminado = 0;

//Establecimiento de los límites para las mediciones de pH y temperatura
float PH_MIN = 6.0;
float PH_MAX = 8.0;
float TEMP_MIN = 20.0;
float TEMP_MAX = 31.6;

//Punteros se declaran a los buffers para almacenar mediciones de pH y temperatura
MedicionSensor *ph_buffer;
MedicionSensor *temp_buffer;


// Función para el hilo H_recolector
void *hilo_recolector(void *arg) {
    // Transformar el argumento pasado a la función en un puntero hacia la estructura HiloArgs
    HiloArgs *args = (HiloArgs *)arg;

    // Reservar memoria para los buffers de mediciones de pH y temperatura
    ph_buffer = (MedicionSensor *)malloc(args->tam_buffer * sizeof(MedicionSensor));
    temp_buffer = (MedicionSensor *)malloc(args->tam_buffer * sizeof(MedicionSensor));
    if (ph_buffer == NULL || temp_buffer == NULL) {
        perror("No se pudo asignar memoria para los buffers");
        return NULL;
    }

    int fd;
    // Abrir el pipe nominal para lectura
    fd = abrirPipeNominalLectura(args->pipe_nominal);
    if (fd < 0) {
        perror("No se pudo abrir el pipe nominal");
        return NULL;
    }

    MedicionSensor medicion_buffer; // Variable para almacenar la medición
    MedicionSensor *medicion = &medicion_buffer; // Puntero a la medición

    // Leer las mediciones del pipe nominal
    while (1) {
        int texto = read(fd, medicion, sizeof(MedicionSensor));
        if (texto <= 0) {
            // El sensor no está conectado, esperando 10 segundos
            printf("\nRecolector: No hay ningún sensor conectado\n");
            sleep(10);
            close(fd);

            pthread_mutex_lock(&mutexTerminado);
            envio_terminado = 1;
            sem_post(&full_temp); 
            sem_post(&full_ph); 
            pthread_cond_broadcast(&cond_terminacion); // Notificar a todos los hilos que el envío ha terminado
            pthread_mutex_unlock(&mutexTerminado);
            destruirPipeNominal(args->pipe_nominal);
            break;
        }

        if (medicion->valor >= 0) {
            if (medicion->tipoSensor == 2) { // Medición de PH
                sem_wait(&empty_ph);
                pthread_mutex_lock(&mutex);
                if (ph_index < args->tam_buffer) {
                    // Colocar la medición en el buffer de PH
                    ph_buffer[ph_index++] = *medicion;
                } else {
                    printf("Buffer de PH lleno. Descartando medición.\n");
                }
                pthread_mutex_unlock(&mutex);
                sem_post(&full_ph);
            } else { // Medición de temperatura
                sem_wait(&empty_temp);
                pthread_mutex_lock(&mutex);
                if (temp_index < args->tam_buffer) {
                    // Colocar la medición en el buffer de temperatura
                    temp_buffer[temp_index++] = *medicion;
                } else {
                    printf("Buffer de temperatura lleno. Descartando medición.\n");
                }
                pthread_mutex_unlock(&mutex);
                sem_post(&full_temp);
            }
        } else {
            printf("\nLa medicion %f es negativa, por lo tanto erronea. Descartando medición.\n", medicion->valor);
        }
    }

    return NULL;
}

// Función para el hilo H-ph
void *hilo_ph(void *arg) {
  HiloArgs *args = (HiloArgs *)arg;

  while (1) {
    // Abrir el archivo file_ph para escritura
    FILE *file_ph = fopen(args->file_ph, "a");
    // Verificar si el envío ha terminado y salir del bucle
    pthread_mutex_lock(&mutexTerminado);
    if (envio_terminado == 1) {
      pthread_mutex_unlock(&mutexTerminado);
      fclose(file_ph);
      break;
    }
    pthread_mutex_unlock(&mutexTerminado);

    sem_wait(&full_ph);
    pthread_mutex_lock(&mutex);

    if (file_ph == NULL) {
      perror("\nError al abrir el archivo file_ph");
      exit(EXIT_FAILURE);
    }

    // Escribir las mediciones de temperatura en el archivo file_ph
    for (int i = 0; i < ph_index; i++) {
      MedicionSensor medicion = ph_buffer[i];
      time_t now = time(NULL);
      struct tm *tm_info = localtime(&now);
      char hora[9]; // Espacio para la hora en formato HH:MM:SS
      strftime(hora, sizeof(hora), "%H:%M:%S", tm_info);

      // Anexar la hora actual a la medida
      medicion.timestamp = now;

      // Escribir la medida en el archivo file_ph
      fprintf(file_ph, "%s - PH: %.2f\n", hora, medicion.valor);
      printf("\n%s - PH: %.2f\n", hora, medicion.valor);
      // Verificar si la medida está fuera de los rangos
      if (medicion.valor < PH_MIN || medicion.valor > PH_MAX) {
        printf(
            "\nAlerta: El valor del ph está fuera de los rangos aceptables.\n");
      }
    }

    // Cerrar el archivo file_ph
    fclose(file_ph);
    // printf("El hilo H-ph ha terminado de escribir la medicion en el archivo
    // file_ph.\n");

    // Resetear el índice del buffer de temp
    ph_index = 0;
    pthread_mutex_unlock(&mutex);
    sem_post(&empty_ph);
  }
  
  return NULL;
}

// Función para el hilo responsable de procesar y registrar las mediciones de temperatura en un archivo
void *hilo_temperatura(void *arg) {
  //Transformar el argumento pasado a la función en un puntero hacia la estructura HiloArgs
  HiloArgs *args = (HiloArgs *)arg;

  while (1) {
    // Abrir el archivo file_temp para escritura
    FILE *file_temp = fopen(args->file_temp, "a");
    // Verificar si el envío ha terminado y salir del bucle
    pthread_mutex_lock(&mutexTerminado);
    if (envio_terminado == 1) {
      pthread_mutex_unlock(&mutexTerminado);
      fclose(file_temp);
      break;
    }
    pthread_mutex_unlock(&mutexTerminado);
    //Esperar a que el buffer de mediciones de temperatura esté completo
    sem_wait(&full_temp);
    pthread_mutex_lock(&mutex);

    if (file_temp == NULL) {
      perror("\nError al abrir el archivo file_temp");
      exit(EXIT_FAILURE);
    }

    // Escribir las mediciones de temperatura en el archivo file_temp
    for (int i = 0; i < temp_index; i++) {
      MedicionSensor medicion = temp_buffer[i];
      time_t now = time(NULL);
      struct tm *tm_info = localtime(&now);
      char hora[9]; // Espacio para la hora en formato HH:MM:SS
      strftime(hora, sizeof(hora), "%H:%M:%S", tm_info);

      // Anexar la hora actual a la medida
      medicion.timestamp = now;

      // Escribir la medida en el archivo file_temp
      fprintf(file_temp, "%s - Temperatura: %.2f\n", hora, medicion.valor);
      printf("\n%s - Temperatura: %.2f\n", hora, medicion.valor);
      // Verificar si la medida está fuera de los rangos
      if (medicion.valor < TEMP_MIN || medicion.valor > TEMP_MAX) {
        printf("\nAlerta: El valor de la temperatura está fuera de los rangos "
               "aceptables.\n");
      }
    }

    // Cerrar el archivo file_temp
    fclose(file_temp);

    // Resetear el índice del buffer de temp
    temp_index = 0;
    pthread_mutex_unlock(&mutex);
    sem_post(&empty_temp);
  }

  return NULL;
}