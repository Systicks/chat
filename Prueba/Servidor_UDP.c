#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXBUFFER 256//Se empleará como valor máximo de recepción

void error(char *msg);

int main(int argc, char *argv[]){
	int socketfd, clilen, BytesRecv, puerto = 6666;
	struct sockaddr_in servidor, cliente; 
	char buffer[MAXBUFFER];

	//Comprobar que se recibe solo un parámetro que es el puerto
 	if((argc<2) || (argc>2)){
        error("Es necesario introducir el puerto.\n");
    }
    puerto = atoi(argv[1]);//Establecemos el puerto con el parámetro recibido
    //Abrimos el socket UDP
	if ((socketfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		error( "Error al crear el socket") ;
	//Introducimos los protocolos a usar, el puerto y las Ip admitidas(todas) en la estructura sockaddr_in
	memset(&servidor, 0, sizeof(servidor));
	servidor.sin_family = AF_INET;
	servidor.sin_addr.s_addr = htonl(INADDR_ANY);
	servidor.sin_port = htons(puerto);
	//Asociamos el puerto elegido al socket creado mediante bind()
	if (bind(socketfd, (struct sockaddr *) &servidor, sizeof(servidor)) < 0)
		error("Error en bind");
	/** El bucle se repetirá hasta que no se reciba nada **/
	do{
		clilen = sizeof(cliente);
		//Esperamos a recibir información por el puerto en el que estamos escuchando, guardamos en buffer la información y en BytesRecv la cantidad recibida
		if ((BytesRecv = recvfrom(socketfd, buffer, MAXBUFFER, 0, (struct sockaddr *) &cliente, &clilen)) < 0)
			error("Error en recvfrom") ;
		printf("Handling client %s\n", inet_ntoa(cliente.sin_addr));//Imprimimos la ip del cliente que se nos ha conectado para informar por pantalla
		//Reenviamos lo que hemos guardado en buffer con el mismo número de bytes recibidos
		if (sendto(socketfd, buffer, BytesRecv, 0,(struct sockaddr *) &cliente, sizeof(cliente)) != BytesRecv)
			error("Error en sendto");
	}while(BytesRecv!=0);
	close(socketfd);//Cerramos el socket
	return(0);
}
/** Empleamos esta función para imprimir los errores posibles durante la ejecución y salir del programa si se producen **/
void error(char *msg)
{
    perror(msg);
    exit(1);
}