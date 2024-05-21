/**************************************************************
Autores: 
  - Sara Gutiérrez 
  - Leonardo Velázquez
  - Oscar Rodriguez
Fecha: 27 abril 2024
Materia: Sistemas Operativos
Pontificia Universidad Javeriana
Proyecto: Monitoreo de sensores
Tema: La interfaz provee la estructura de argumentos necesaria 
      para crear y ejecutar hilos.
***************************************************************/

#ifndef HILOS_H
#define HILOS_H

// Estructura para los argumentos de los hilos
typedef struct {
    char *file_temp; //Ruta al archivo de temperatura que el hilo procesará
    char *file_ph; //Archivo de pH a procesar por el hilo
    int tam_buffer; //// El tamaño del buffer a utilizar para almacenar datos durante el procesamiento
    char *pipe_nominal; //Nombre del pipe nominal que el hilo utilizará para la comunicación
} HiloArgs;

#endif /* HILOS_H */
