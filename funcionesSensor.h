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
Fichero: Interfaz de funciones para el manejo del sensor
***********************************************************************/

#ifndef __FUNCIONES_SENSOR_H__
#define __FUNCIONES_SENSOR_H__

// Función para enviar mediciones al proceso monitor a través del pipe nominal
void enviarMediciones(int tipo_sensor, int tiempo, char *archivo, char *pipe_nominal);

#endif