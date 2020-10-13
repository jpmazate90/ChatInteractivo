

#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cstring>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include "main.h"

#include <thread>


using namespace std;

/*
 * jpmazate
 */
// nombre del participante
stringg nombreParticipante;
//lista de los mensajes
stringg mensajesEnviados[MAXIMO_MENSAJES];
stringg mensajesRecibidos[MAXIMO_MENSAJES];
// cantidad de mensajes enviados
int cantidadMensajesEnviados = 0;
int cantidadMensajesRecibidos = 0;
int serial_port;
//bandera para que sigua el chat
bool seguirChat = true;

int main(int argc, char** argv) {
    int datoPuerto;
    bool banderaInicio = true;
    cout << ASTERICOS << endl;
    cout << "BIENVENIDO AL CHAT VIRTUAL" << endl;
    cout << "POR FAVOR BRINDA EL PUERTO POR EL QUE QUIERES MANDAR DATOS (1 o 2): ";
    cin>>datoPuerto;
    inicializarPuerto(datoPuerto);// inicializara el puerto

    thread hiloLeerPuerto(leerPuerto, &serial_port);
    thread hiloChatPrincipal(inicioChat, &banderaInicio);
    hiloChatPrincipal.join();// crea hilo de escribir
    hiloLeerPuerto.join();// crea hilo de leer
    closePuertoSerial();
    return 0;
}

void *inicioChat(void *ptr) { // MUESTRA EL INICIO DEL CHAT
    bool bandera = (void *) ptr;
    while (seguirChat) { // ESTARA CONSTANTEMENTE ESPERANDO QUE SE ESCRIBA UN MENSAJE
        char msg[256];
        cout << "Escribe por favor:" << endl;
        cin.getline(msg, TAMANIO_MENSAJE); // OBTIENE LA LINEA O EL MENSAJE
        if (strcmp(msg, CADENA_SALIDA_CHAT) == 0) { // COMPARA SI NO ES LA CADENA DE SALIDA DEL CHAT
            cout << "SALIENDO DEL CHAT" << endl;
            stringg salidaMensaje;
            sprintf(salidaMensaje, "%s HA SALIDO DEL CHAT.\0", nombreParticipante); // SI SI LO ES MANDA MENSAJE AL OTRO LADO DEL PUERTO QUE SE DESCONECTO
            agregarMensaje(salidaMensaje, MENSAJES_ENVIADOS); 
            closePuertoSerial();// CIERRA EL PUERTO SERIAL
            seguirChat = false;// AVISA QUE HAY QUE SALIR DE LA APLICACION
        } else {
            agregarMensaje(msg, MENSAJES_ENVIADOS); // SI NO, UNICAMENTE MANDA EL MENSAJE CORRESPONDIENTE
            mostrarMenu(); // MUESTRA EL MENU
        }
    }

}

void mostrarMenu() { // muestra los mensajes recibidos y enviados
    cout << ASTERICOS << endl;
    mostrarMensajes(MENSAJES_RECIBIDOS);
    cout << ASTERICOS << endl;
    mostrarMensajes(MENSAJES_ENVIADOS);
    cout << ASTERICOS << endl;
}

void agregarMensaje(char msg[], int tipo) {

    if (tipo == MENSAJES_ENVIADOS) {

        if (cantidadMensajesEnviados >= MAXIMO_MENSAJES) { // verifica si ya llego al maximo de mensajes enviados disponibles
            subirPosicionMensajes(tipo); // si ya llego movera en 1 posicion todos los mensajes para que entre el nuevo
            sprintf(mensajesEnviados[cantidadMensajesEnviados - 1], "%s\0", msg); // escribe en la ultima posicion EL MENSAJE ESCRITO

        } else { // si no unicamente lo agrega al arreglo
            sprintf(mensajesEnviados[cantidadMensajesEnviados], "%s\0", msg);
            cantidadMensajesEnviados++;
        }

        writePuertoSerial(msg); // escribira en el puerto serial

    } else if (tipo == MENSAJES_RECIBIDOS) { // verifica si ya llego al maximo de mensajes recibidos disponibles
        if (cantidadMensajesRecibidos >= MAXIMO_MENSAJES) {
            subirPosicionMensajes(tipo); // actualiza el arreglo y los mueve en una posicion
            sprintf(mensajesRecibidos[cantidadMensajesRecibidos - 1], "%s\0", msg); // escribe en la ultima posicion EL MENSAJE RECIBIDO

        } else {
            sprintf(mensajesRecibidos[cantidadMensajesRecibidos], "%s\0", msg); // escribe en la ultima posicion del arreglo
            cantidadMensajesRecibidos++;
        }

    }
}

void subirPosicionMensajes(int tipo) {
    if (tipo == MENSAJES_ENVIADOS) {
        for (int i = 0; i < MAXIMO_MENSAJES; i++) { // aumenta los mensajes enviados en una posicion del arreglo
            if (i == MAXIMO_MENSAJES - 1) {

            } else {
                sprintf(mensajesEnviados[i], "%s\0", mensajesEnviados[i + 1]);
            }
        } 
    } else if (tipo == MENSAJES_RECIBIDOS) {// aumenta los mensajes recibidos en una posicion del arreglo
        for (int i = 0; i < MAXIMO_MENSAJES; i++) {
            if (i == MAXIMO_MENSAJES - 1) { // si es el ultimo posicion no hace nada

            } else {
                sprintf(mensajesRecibidos[i], "%s\0", mensajesRecibidos[i + 1]);
            }
        }
    }

}

void mostrarMensajes(int tipo) { // recorre los arreglos para mostrar los mensajes enviados o recibidos
    if (tipo == MENSAJES_ENVIADOS) {
        cout << "MENSAJES ENVIADOS: " << cantidadMensajesEnviados << endl;
        for (int i = 0; i < cantidadMensajesEnviados; i++) {
            cout << "Mensaje: " << mensajesEnviados[i] << endl;
        }
    } else if (tipo == MENSAJES_RECIBIDOS) {
        cout << "MENSAJES RECIBIDOS: " << cantidadMensajesRecibidos << endl;
        for (int i = 0; i < cantidadMensajesRecibidos; i++) {
            cout << "Mensaje: " << mensajesRecibidos[i] << endl;
        }
    }
}

void mostrarCredenciales() { // muestra mensaje interactivo en la pantalla
    cout << ASTERICOS << endl;
    cout << " CREDENCIALES Y REGLAS" << endl;
    cout << "1.- MUY BIEN, TU NOMBRE ASIGNADO ES: " << nombreParticipante << endl;
    cout << "2.- PARA ENVIAR UN MENSAJE UNICAMENTE NECESITAS ESCRIBIR EL MENSAJE Y LUEGO PRESIONAR ENTER " << endl;
    cout << "3.- PARA SALIR DE LA APLICACION DEBES ENVIAR UN MENSAJE CON LOS SIGUIENTES CARACTERES: " << CADENA_SALIDA_CHAT << endl;
    cout << ASTERICOS << endl;

}

void inicializarPuerto(int puerto) {

    if (puerto == PUERTO_1) {
        serial_port = open("/home/jpmazate/puertosSeriales/interface1", O_RDWR); // escoge el puerto a utilizar
        strcpy(nombreParticipante, PARTICIPANTE_1);
    }
    if (puerto == PUERTO_2) {
        serial_port = open("/home/jpmazate/puertosSeriales/interface2", O_RDWR); // si escoge el 2, usaria la interfaz 2
        strcpy(nombreParticipante, PARTICIPANTE_2);
    }
    struct termios tty;

    if (tcgetattr(serial_port, &tty) != 0) {
        cout << "Error " << errno << "from tcgetattr: " << strerror(errno) << endl;
    }

    mostrarCredenciales();
// variables para que funcione el puerto serial
    tty.c_cflag &= ~PARENB; 
    tty.c_cflag &= ~CSTOPB; 
    tty.c_cflag |= CS8; 
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL; 
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; 
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG; 
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); 
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    tty.c_oflag &= ~OPOST; 
    tty.c_oflag &= ~ONLCR; 
    tty.c_cc[VTIME] = 10;
    tty.c_cc[VMIN] = 0;
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) { // si existe error lo mostrara
        cout << "Error " << errno << " de tcsetattr: " << strerror(errno) << endl;
    }
}

void writePuertoSerial(char mensaje[]) { // escribira en el puerto serial
    write(serial_port, mensaje, strlen(mensaje));
}

void closePuertoSerial() { // cerrara el puerto serial
    close(serial_port);
}

void *leerPuerto(void *ptr) {// hilo que estara leyendo datos del puerto serial
    while (seguirChat) {
        char read_buf [256];
        memset(&read_buf, '\0', sizeof (read_buf));

        int num_bytes = read(serial_port, &read_buf, sizeof (read_buf));

        if (num_bytes < 0) { // si es menor a 0 es que existe un error con el puerto serial en la lectura
            cout << "ERROR LEYENDO POR EL PUERTO SERIAL: " << strerror(errno) << endl;
        }

        if (num_bytes > 0) { // si los bytes son mayores a 0, es que se recibio un mensaje
            cout << ASTERICOS << endl;
            cout << "SE ACABA DE RECIBIR UN MENSAJE " << endl;
            cout << ASTERICOS << endl;
            agregarMensaje(read_buf, MENSAJES_RECIBIDOS);
            mostrarMenu();
        }
    }
}
