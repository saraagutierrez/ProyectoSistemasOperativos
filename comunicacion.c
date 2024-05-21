/***************************************************************
Autores: 
  - Sara Gutiérrez 
  - Leonardo Velázquez
  - Oscar Rodriguez
Fecha: 27 abril 2024
Materia: Sistemas Operativos
Pontificia Universidad Javeriana
Proyecto: Monitoreo de sensores
Tema: Desarrollo de un monitor de procesos para el sistema de monitoreo.
Fichero: Implementación de funciones para gestionar pipes nominales.
****************************************************************/

#include <stdio.h> // Biblioteca estándar de E/S
#include <stdlib.h> // Biblioteca estándar para funciones de utilidad general
#include <unistd.h> // Biblioteca estándar de llamadas al sistema y constantes POSIX
#include <fcntl.h> // Biblioteca estándar para la manipulación de archivos y descriptores de archivo
#include <string.h> // Biblioteca estándar para manipulación de cadenas de caracteres
#include <sys/types.h> // Biblioteca de tipos y estructuras estándar para sistemas UNIX 
#include <sys/stat.h> // Biblioteca estándar para manipular la información de archivos y directorios.
#include <semaphore.h> // Biblioteca estándar para manejo de semáforos
#include "comunicacion.h" // Interfaz artesanal

// Función para crear un pipe nominal
void crearPipeNominal(char *nombre_pipe) {
  //Uso de la función mkfifo para generar un canal con nombre utilizando el nombre especificado
    if (mkfifo(nombre_pipe, 0666) == -1) {
        perror("Error al crear el pipe nominal");
        exit(EXIT_FAILURE);
    }
}

// Función para destruir un pipe nominal
void destruirPipeNominal(char *nombre_pipe) {
  //Uso de la función unlink para eliminar un pipe dado su nombre
    if (unlink(nombre_pipe) == -1) {
        perror("Error al destruir el pipe nominal");
        exit(EXIT_FAILURE);
    }
}

// Función para abrir un pipe nominal para lectura
int abrirPipeNominalLectura(char *nombre_pipe) {
    int fd = open(nombre_pipe, O_RDONLY); //Abre un pipe nominal en modo lectura, la función open devuelve un descriptor de archivo (fd) 
    if (fd == -1) {
        perror("Error al abrir el pipe nominal para lectura");
        exit(EXIT_FAILURE);
    }
    return fd; // Devuelve el descriptor de archivo abierto
}

// Función para abrir un pipe nominal para escritura
int abrirPipeNominalEscritura(char *nombre_pipe) {
    int fd = open(nombre_pipe, O_WRONLY); //Abre un pipe nominal en modo escritura, la función open retorna un descriptor de archivo (fd) que se usa para escribir en el pipe
    if (fd == -1) {
        //perror("Error al abrir el pipe nominal para escritura");
        //exit(EXIT_FAILURE);
    }
    return fd; // Devuelve el descriptor de archivo abierto
}
