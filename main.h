/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.h
 * Author: jpmazate
 *
 * Created on 29 de septiembre de 2020, 15:51
 */

#ifndef MAIN_H
#define MAIN_H

#define  TAMANIO_MENSAJE    256
#define  MAXIMO_MENSAJES    15
#define  PUERTO_1   1
#define  PUERTO_2   2
#define MENSAJES_RECIBIDOS 1 
#define MENSAJES_ENVIADOS  2
#define CADENA_SALIDA_CHAT "~p"
#define  PARTICIPANTE_1 "PARTICIPANTE 1"
#define  PARTICIPANTE_2 "PARTICIPANTE 2"
#define  ASTERICOS "***************************************"

typedef char stringg [256];

void mostrarMensajes(int tipo);
void agregarMensaje(char msg[], int tipo);
void subirPosicionMensajes(int tipo);
void mostrarMenu();
void inicializarPuerto(int puerto);
void writePuertoSerial(char mensaje[]);
void *leerPuerto(void *ptr);
void *inicioChat(void *ptr);
void cerrarPuerto();
void closePuertoSerial() ;
void mostrarCredenciales();

#endif /* MAIN_H */

