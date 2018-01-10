#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <sys/socket.h>

  
#define PUERTO 6666	//Puerto empleado
#define MaxBytes 256  //Máximo número de bytes a envia
#define MaxClientes 30	//Máximo número de clientes aceptados

/*Definición de funciones*/
void error(char *msg);

int main(int argc , char *argv[])
{
	fd_set p_leer;
    int socketfd, addrlen, newsockfd, i, y;
    int max_sd, socket_clientes[MaxClientes];
    struct sockaddr_in serv_addr;
    char buffer[236], buffer2[MaxBytes], Nombre[20];//Buffers para enviar y recibir datos
  	char Nombres[MaxClientes][20];//Array de nombres de usuario
    //Inicializar el array de sockets de clientes a 0
    for (i = 0; i < MaxClientes; i++) 
    {
        socket_clientes[i] = 0;
    }
    /*Abrir el socket TCP*/
	if((socketfd=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    	error("Error al crear el socket");
	//Introducimos los protocolos a usar, el puerto y las Ip admitidas(todas) en la estructura sockaddr_in
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PUERTO);
	//Asociamos el puerto elegido al socket creado mediante bind()
    if (bind(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR en binding");
    printf("Escuchando en el puerto: %d \n", PUERTO);
    //Esperamos una conexión entrante escuchando en el puerto establecido(máximo 5 a la vez)
    if (listen(socketfd, 5) < 0)
    	error("Error en listen()");
    addrlen = sizeof(serv_addr);
    /*Bucle infinito en el que el servidor escucha nuevas conexiones y reenevía los mensajes*/
    while(1) 
    {
        /*Limpiamos los arrays cada vez que empiece el bucle y ponemos a 1 los descriptores que queremos que monitorice el select*/
        FD_ZERO(&p_leer);
        FD_SET(socketfd, &p_leer);
        max_sd = socketfd;//El número máximo de socket es en principio el de socketfd
        /*Se hace el set de los sockets generados para cada cliente de manera que el select los monitorice*/
        for ( i = 0 ; i < MaxClientes ; i++) 
        {
            //Comprobamos que existe el socket
            if(socket_clientes[i] > 0)
                FD_SET( socket_clientes[i] , &p_leer);
            //Establecemos nuevamente el número máximo de socket por si ha cambiado
            if(socket_clientes[i] > max_sd)
                max_sd = socket_clientes[i];
        }
		/*Esperamos a que haya actividad en uno de los sockets*/
        if(select( max_sd + 1 , &p_leer , NULL , NULL , NULL) < 0){
            error("Error en select()");
        }
        /*Comprobamos si ha ocurrido algo en el socket principal(una petición de conexión)*/
        if (FD_ISSET(socketfd, &p_leer)) 
        {
        	/*Aceptamos la conexión y la vinculamos al nuevo socket asignado*/
            if ((newsockfd = accept(socketfd, (struct sockaddr *)&serv_addr, (socklen_t*)&addrlen))<0)
            	error("Error en accept()");
            memset(Nombre, 0, sizeof(Nombre));//Limpiamos el buffer de nombres
            /*Leemos el nombre que nos envía el cliente al establecer la conexión*/
            if (read( newsockfd , Nombre, 20) < 0)
           		error("");
           	/*Imprimimos por pantalla datos de la conexión y mandamos al cliente el mensaje de bienvenida*/
           	printf("Nueva conexión: Socket: %d, IP: %s, Puerto: %d, Nombre: %s\n" , newsockfd , inet_ntoa(serv_addr.sin_addr) , ntohs(serv_addr.sin_port), Nombre);
        	sprintf(buffer, "%s, bienvenido al servidor de chat, para salir escriba 'quit'.\n", Nombre);
            if( send(newsockfd, buffer, strlen(buffer), 0) != strlen(buffer) ) 
            {
                error("Error al enviar el mensaje inicial");
            }
            /*Añadimos el nuevo socket al array de socketsy el nombre al array de nombres*/
            for (i = 0; i < MaxClientes; i++) 
            {
                if( socket_clientes[i] == 0 )
                {
                    socket_clientes[i] = newsockfd;
                    strcpy(Nombres[i], Nombre);
                    break;
                }
            }
        }
        /*Comprobamos si ha ocurrido algo en alguno de los sockets del array*/
        for (i = 0; i < MaxClientes; i++) 
        {
            if (FD_ISSET( socket_clientes[i] , &p_leer))//Solo entramos si ese socket ha recibido algo
            {
                memset(buffer, 0, sizeof(buffer));
                /*Leemos y si se recibe 0 cerramos el socket correspondiente*/
                if (read(socket_clientes[i] , buffer, 256) == 0)
                {
                    printf("Cliente desconectado: Nombre: %s\n" , Nombres[i]);
                    //Cerramos el socket y lo ponemos a 0 en el array de sockets
                    close(socket_clientes[i]);
                    socket_clientes[i] = 0;
                }
                /*Si se ha leído bien reenviamos el mensaje a todos los clientes menos al que lo envió*/
                else
                {
                	memset(buffer2, 0, sizeof(buffer2));//Vaciamos el buffer
                	sprintf(buffer2, "%s> %s",Nombres[i], buffer);//Concatenamos el nombre con el mensaje
                	/*Se reenvía el mensaje a todos los clientes menos al que lo envió*/
                    for(y=0;y<MaxClientes; y++){
                    	if(y!=i)
                    		send(socket_clientes[y], buffer2 , MaxBytes, 0 );
                    }

                }
            }
        }
    }
    return 0;
} 

/** Empleamos esta función para imprimir los errores posibles durante la ejecución y salir del programa si se producen **/
void error(char *msg)
{
    perror(msg);
    exit(1);
}