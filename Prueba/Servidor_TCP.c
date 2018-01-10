#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>

#define MAXBUFFER 256//Se empleará como valor máximo de recepción

void error(char *msg);

int main(int argc, char *argv[])
{
        int socketfd, newsockfd, clilen, BytesRecv, puerto = 6666;
        char buffer[MAXBUFFER];
        struct sockaddr_in serv_addr, cli_addr;
        //Comprobar que se recibe solo un parámetro que es el puerto
        if((argc<2) || (argc>2)){
            error("Es necesario introducir el puerto.\n");
        }
        puerto = atoi(argv[1]);//Establecemos el puerto con el parámetro recibido
        //Abrimos el socket TCP
        if ((socketfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            error( "Error al crear el socket") ;
        //Introducimos los protocolos a usar, el puerto y las Ip admitidas(todas) en la estructura sockaddr_in
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(puerto);
        //Asociamos el puerto elegido al socket creado mediante bind()
        if (bind(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
            error("ERROR en binding");
        /** El bucle se repetirá mientras Continuar valga 1, esto cambiará si decidimos cerrar el socket cuando nos den la opción **/
        int Continuar=1;
        do{
            //Esperamos una conexión entrante escuchando en el puerto establecido
            if (listen(socketfd, 5) < 0)
                error("Error en listen");
            //Aceptamos la conexión cuando alguien intente conectarse a nosotros
            clilen = sizeof(cli_addr);
            newsockfd = accept(socketfd, (struct sockaddr *) &cli_addr, &clilen);
            if (newsockfd < 0) 
                error("ERROR en accept");
                
            printf("Handling client %s\n", inet_ntoa(cli_addr.sin_addr));//Imprimimos la ip del cliente que se nos ha conectado para informar por pantalla
            bzero(buffer,MAXBUFFER);//Vaciamos el buffer
            /** El bucle se repetirá hasta que dejemos de recibir bytes (el cliente cierra el socket) **/
            do{
                //Esperamos a recibir información desde el socket y guardamos los bytes recibidos en BytesRecv, almacenamos lo que hemos recibido en buffer
                if ((BytesRecv = recv(newsockfd, buffer, MAXBUFFER, 0)) < 0)
                    error("Error en recv");
                //Reenviamos buffer con el mismo número de bytes que hemos recibido
                if (send(newsockfd, buffer, BytesRecv, 0) != BytesRecv)
                    error("Error en send");
                }while(BytesRecv!=0);//Saldremos cuando no recibamos nada
            /** Preguntamos al usuario si quiere mantener el socket abierto o cerrarlo **/
            char respuesta;
            printf("¿Cerrar el servidor(C) o seguir escuchando(S)?: ");
            __fpurge(stdin);//Limpiar el buffer de entrada
            respuesta=getchar();
            if(toupper(respuesta) == 'C')//Si la respuesta es cerrar se sale del while
            {
                Continuar=0;
            }
        }while(Continuar);
        printf("Cerrando socket.\n");
        close(socketfd);//Cierre del socket
    return 0; 
}
/** Empleamos esta función para imprimir los errores posibles durante la ejecución y salir del programa si se producen **/
void error(char *msg)
{
    perror(msg);
    exit(1);
}