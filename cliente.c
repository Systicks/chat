#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MaxBytes 256  //Máximo número de bytes a enviar
#define PUERTO 6666   //Puerto empleado

/**Definición de funciones**/
void error(char *msg);
unsigned long ResolveName(char name[]);

int main(int argc, char *argv[])
{
	int socketfd;
	struct sockaddr_in servidor;
  char buffer[MaxBytes];
  fd_set p_leer;
  int salir=1;
  char Nombre[20];
  //Nos aseguramos de que al menos se introduzca la IP destino
  if(argc<2){
    error("Es necesario introducir al menos la ip del servidor");
  }
  /*Obtención del nombre en el chat*/
  printf("\nElige un el nombre con el que serás visto en el chat: ");
  scanf("%s", Nombre);
  setbuf(stdin, NULL);//Vaciar el buffer de entrada por problemas al leer si no se vacía
  /*Introducimos los protocolos a usar, el puerto y la Ip en la estructura sockaddr_in*/
  servidor.sin_family = AF_INET;
  servidor.sin_port = htons(PUERTO); 
  servidor.sin_addr.s_addr = ResolveName(argv[1]); //Llamada a la función de DNS que nos devuelve una ip
  bzero(&(servidor.sin_zero),8);
  /*Abrir el socket TCP*/
  if((socketfd=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){  
    error("Error al crear el socket");
  }
  /** Establecemos la conexión TCP con el servidor **/
  if(connect(socketfd, (struct sockaddr *)&servidor, sizeof(servidor)) < 0){ 
    error("Error de conexión");
  }
  /*Enviamos el nombre al servidor la primera vez*/
  if( send(socketfd, Nombre, strlen(Nombre), 0) != strlen(Nombre))
  {
    error("Error al enviar el nombre");
  }
  memset(buffer, 0, MaxBytes);//Vaciamos el buffer que vamos a emplear en la recepción
  /*Esperamos a recibir el mensaje de bienvenida del servidor y lo imprimimos por pantalla junto con la flecha que nos invita a escribir*/
  if ((read( socketfd , buffer, MaxBytes)) <0)
    error("Error en la respuesta del servidor");
  printf("\n%s\n", buffer);
  setbuf(stdout,NULL);//Buffer de salida a null porque hay problemas en los printf si no se vacía
  printf("~> ");
  /*Entramos en el bucle que se repite hasta que el usuario escribe 'quit'*/
  while(salir){
    /*Limpiamos los arrays cada vez que empiece el bucle y ponemos a 1 los descriptores que queremos que monitorice el select*/
    FD_ZERO(&p_leer);
    FD_SET(socketfd, &p_leer);
    FD_SET(STDIN_FILENO, &p_leer);
    /*Monitorizamos con select el socket con el que nos hemos conectado al servidor y el buffer de entrada*/
    if(select( socketfd +1, &p_leer , NULL , NULL , NULL) < 0){
      error("Error en select()");
    }
    /*Comprobamos si hemos salido del select porque el socket está preparado para ser leído(hemos recibido algo)*/
    if (FD_ISSET(socketfd, &p_leer)) {
      memset(buffer, 0, MaxBytes);//Limpiamos el buffer
      /*Se comprueba que no hay error y que el servidor no se ha desconectado y se lee*/
      if ((read( socketfd , buffer, MaxBytes)) <= 0){
        error("Error al leer");
      }
      /*Si hemos leído correctamente imprimimos lo que hemos recibido borrando la ~> en la línea actual y escribiéndola en la siguente línea*/
      else{
        printf("\r%s",  buffer);
        setbuf(stdout,NULL);
        printf("~> ");
      }
    }
    /*Comprobamos si hemos salido del select porque el buffer de entrada está preparado para ser leído(Hemos pulsado intro)*/
    if(FD_ISSET(STDIN_FILENO, &p_leer)){
      fgets(buffer, MaxBytes, stdin);//Leemos el buffer de entrada y lo almacenamos en 'buffer'
      printf("~> ");//Volvemos a imprimir la flecha que nos invita a escribir
      setbuf(stdin, NULL);//Vaciar el buffer de entrada por problemas al leer si no se vacía
      /*comparamos los 4 primeros bytes de la cadena buffer y salimos si se ha escrito 'quit'*/
      if(strncmp(buffer, "quit", 4)==0){
        salir=0;
      }
      else//En caso de haber escrito otra cosa se envía al servidor
        if(send(socketfd, buffer,MaxBytes, 0) < 0 ) 
          error("Error al enviar el buffer al servidor");
    }
  }
  close(socketfd);//Cerramos el socket antes de salir
  return 0;
}




unsigned long ResolveName(char name[]){
  struct hostent *host;
  //Coprobación de que existe el nombre relacionado con una IP en la estructura hostent
  if ((host = gethostbyname(name)) == NULL)
  {
    error("gethostbyname() failed");
  }
  //La función devuelve la IP en forma de unsigned long (Se puede convertir a IP con inet_ntoa())
  return * ((unsigned long *) host->h_addr_list [0] );
}

/** Empleamos esta función para imprimir los errores posibles durante la ejecución y salir del programa si se producen **/
void error(char *msg)
{
    perror(msg);
    exit(1);
}