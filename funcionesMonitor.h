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
Fichero: Interfaz de funciones para el manejo del monitor
***********************************************************************/

#ifndef __FUNCIONES_MONITOR_H__
#define __FUNCIONES_MONITOR_H__

#include <pthread.h> // Biblioteca para manejo de hilos POSIX
#include <semaphore.h> // Biblioteca estándar para manejo de semáforos
#include "comunicacion.h" // Interfaz artesanal

// Declaración variables globales
extern sem_t empty_temp, full_temp;
extern sem_t empty_ph, full_ph;
extern pthread_mutex_t mutex;
extern MedicionSensor *ph_buffer;
extern MedicionSensor *temp_buffer;
extern int ph_index;
extern int temp_index;
extern pthread_mutex_t mutexTerminado;
extern pthread_cond_t cond_terminacion;
extern int envio_terminado;
extern float PH_MIN;
extern float PH_MAX;
extern float TEMP_MIN;
extern float TEMP_MAX;

// Función para el hilo H_recolector
void *hilo_recolector(void *arg);

// Función para el hilo H-ph
void *hilo_ph(void *arg);

// Función para el hilo responsable de procesar y registrar las mediciones de temperatura en un archivo
void *hilo_temperatura(void *arg);

#endif