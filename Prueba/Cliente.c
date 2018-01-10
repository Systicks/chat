#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <time.h>

/** Definición de funciones **/
void error(char *msg);
unsigned long ResolveName(char name[]);
void Ping_TCP(int socketfd, int NBytes, char *argv[], struct sockaddr_in servidor, int Repeticiones);
void Ping_UDP(int socketfd, struct sockaddr_in fromAddr, int NBytes, char *argv[], struct sockaddr_in servidor, int fromSize, int Repeticiones);
void inicializar(int argc, char *argv[]);
void compararparametros(char *argv[], int i);
/** Parámetros globales que servirán para cambiar los atributos que recibimos como parámetros de entrada **/
int NBytes=64, Repeticiones=4, TipoConex=1, puerto = 6666;

int main(int argc, char *argv[])
{
	int socketfd, fromSize;
	struct sockaddr_in servidor, fromAddr;
  //Llamada a inicializar, que cambiará el valor de los parámetros introducidos por línea de órdenes
  inicializar(argc, argv);
  //Nos aseguramos de que al menos se introduzca la IP destino
  if(argc<2){
    error("Es necesario introducir al menos la ip");
  }
  //Introducimos los protocolos a usar, el puerto y la Ip en la estructura sockaddr_in
  servidor.sin_family = AF_INET;
  servidor.sin_port = htons(puerto); 
  servidor.sin_addr.s_addr = ResolveName(argv[1]); //Llamada a la función de DNS que nos devuelve una ip
  bzero(&(servidor.sin_zero),8);

  //A partir del parámetro TipoConex establecido por la función inicializar establecemos una conexión TCP o UDP
  if(TipoConex)
    Ping_TCP(socketfd, NBytes, argv, servidor, Repeticiones);
  else
    Ping_UDP(socketfd, fromAddr, NBytes, argv, servidor, fromSize, Repeticiones);

  return 0;
}

void compararparametros(char* argv[], int i)
{
  if(!strcmp(argv[i], "-tcp"))
  {
    TipoConex = 1;
    return;
  }
  else if(!strcmp(argv[i], "-udp"))
  {
    TipoConex = 0;
    return;
  }
  else if(!strcmp(argv[i], "-l"))
  {
    NBytes = atoi(argv[i+1]);
    return;
  }
  else if(!strcmp(argv[i], "-r"))
  {
    Repeticiones = atoi(argv[i+1]);
    return;
  }
    else if(!strcmp(argv[i], "-p"))
  {
    puerto = atoi(argv[i+1]);
    return;
  }
}

void inicializar(int argc, char* argv[])
{
  int i;
  if(argc == 1)
    {return;}
  
  for(i=0; i<argc; i++)
  {
        compararparametros(argv, i);
  }
}

/** Función que realiza unn ping UDP **/
void Ping_UDP(int socketfd, struct sockaddr_in fromAddr, int NBytes, char *argv[], struct sockaddr_in servidor, int fromSize, int Repeticiones){
  char buffer[NBytes];//Buffer que nos servirá para enviar los mensajes
  int i;
  for(i=0; i<NBytes;i++){//Rellenar el buffer con la cantidad establecida, por defecto 64 bytes(cada char ocupa 1 byte)
    buffer[i]='a';
  }
  /*Abrir el socket UDP*/
  if ((socketfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
      error( "Error al crear el socket.\n") ;

  printf("PING %s (%s) %d bytes of data(UDP).\n", argv[1], inet_ntoa(servidor.sin_addr), sizeof(buffer)*sizeof(char));//Imprimimos el tamaño que vamos a enviar, IP, etc
  /** El while se ejecutará el número de veces establecidas, por defecto 4 (se realizan 4 pings)**/
  int n=0;
  while(n < Repeticiones){
    clock_t start = clock();//Guardamos el tiempo de inicio del ping
    /*Envío del buffer mediante sendto()*/
    if (sendto(socketfd, buffer, NBytes, 0, (struct sockaddr *) &servidor, sizeof(servidor)) != NBytes)
        error("Error en sendto.\n");
    /* Recibir la respuesta con recvfrom, establecemos como máximo los bytes que hemos enviado(NBytes)*/
    fromSize = sizeof(fromAddr);
    if ((recvfrom(socketfd, buffer, NBytes, 0, (struct sockaddr *) &fromAddr, &fromSize)) != NBytes)
     error("Error en recvfrom.\n") ;
    /*Comprobar que no se reciben paquetes UDP de una fuente que no sea el servidor*/
    if (servidor.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
    {
      error("Se ha recibido un paquete de una fuente desconocida.\n");
    }
    //Imprimimos una cadena informando del tamaño recibido, la ip, el nº de secuencia y el tiempo trascurrido(comparamos el inicio y se lo restamos al tiempo actual)
    printf("%d bytes from  %s (%s): Seq=%d Time=%d ms.\n", sizeof(buffer)*sizeof(char), inet_ntoa(servidor.sin_addr), argv[1], n+1, ((int)clock() - start));
    n++;//Contador de repeticiones
    sleep(1);//Paramos la ejecución 1s
  }
  close(socketfd);//Una vez se acaba la función se cierra el socket
}

/** Función que realiza una conexión TCP **/
void Ping_TCP(int socketfd, int NBytes, char *argv[], struct sockaddr_in servidor, int Repeticiones){
  char buffer[NBytes];//Buffer que nos servirá para enviar los mensajes
  int i;
  for(i=0; i<NBytes;i++){//Rellenar el buffer con la cantidad establecida, por defecto 64 bytes(cada char ocupa 1 byte)
    buffer[i]='a';
  }
  /*Abrir el socket TCP*/
  if((socketfd=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){  
    error("Error al crear el socket");
  }
  /** Establecemos la conexión TCP con el servidor **/
  if(connect(socketfd, (struct sockaddr *)&servidor, sizeof(servidor)) < 0){ 
    error("Error de conexión");
  }
  printf("PING %s (%s) %d bytes of data(TCP).\n", argv[1], inet_ntoa(servidor.sin_addr), sizeof(buffer)*sizeof(char));//Imprimimos el tamaño que vamos a enviar, IP, etc
  /** El while se ejecutará el número de veces establecidas, por defecto 4 (se realizan 4 pings)**/
  int n=0;
  while(n < Repeticiones){
    clock_t start = clock();//Guardamos el tiempo de inicio del ping
    /*Envío del buffer mediante send()*/
    if (send(socketfd, buffer,NBytes, 0) < 0 ) 
      error("ERROR en el send");
    /* Recibir la respuesta con recv, establecemos como máximo los bytes que hemos enviado(NBytes)*/
    if ((recv(socketfd, buffer, NBytes, 0)) < 0)
        error("ERROR leyendo del socket");
    //Imprimimos una cadena informando del tamaño recibido, la ip, el nº de secuencia y el tiempo trascurrido(comparamos el inicio y se lo restamos al tiempo actual)
    printf("%d bytes from  %s (%s): Seq=%d Time=%d ms.\n", sizeof(buffer)*sizeof(char), inet_ntoa(servidor.sin_addr), argv[1], n+1, ((int)clock() - start));
    n++;//Actualizar contador
    sleep(1);//Paramos la ejecución 1s
  }
  close(socketfd);//Una vez se acaba el envío se cierra el socket
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