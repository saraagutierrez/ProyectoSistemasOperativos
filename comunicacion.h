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
Fichero: Interfaz que provee una estructura y funciones para la 
          comunicación entre procesos
************************************************************************/

#ifndef COMUNICACION_H
#define COMUNICACION_H

#include <time.h> //Biblioteca para el tiempo y las fechas

// La estructura representa una medición de sensor.
typedef struct {
    int tipoSensor; // tipo sensor
    float valor; // valor medida
    time_t timestamp; // marca de tiempo de la medición
} MedicionSensor;

// Función para crear un pipe nominal
void crearPipeNominal(char *nombre_pipe);

// Función para destruir un pipe nominal
void destruirPipeNominal(char *nombre_pipe);

// Función para abrir un pipe nominal para lectura
int abrirPipeNominalLectura(char *nombre_pipe);

// Función para abrir un pipe nominal para escritura
int abrirPipeNominalEscritura(char *nombre_pipe);

#endif /* COMUNICACION_H */
