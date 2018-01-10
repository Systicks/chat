import socket
import sys
import time
import argparse
 
puerto=6666
repeticiones=4
n=0
message=""
NBytes=64
direccion="localhost"
#Argparse nos permite gestionar los parametros de entrada
parser = argparse.ArgumentParser()
#Anadimos string para la IP y el resto de parametros que se aceptan
parser.add_argument('string')
parser.add_argument("-p", "--puerto", help="Asignar el puerto")
parser.add_argument("-r", "--repeticiones", help="Numero de repeticiones del ping")
parser.add_argument("-l", "--longitud", help="Longitud del ping")
args = parser.parse_args()
# Aqui procesamos lo que se tiene que hacer con cada argumento recibido
if args.puerto:
    puerto=int(args.puerto)
if args.repeticiones:
    repeticiones=int(args.repeticiones)
if args.longitud:
    NBytes=int(args.longitud)
if args.string:
    print args.string
    direccion=args.string
#Creamos el objeto socket con los argumentos para una conexion TCP
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#Conexion con el servidor, como parametros la ip y el puerto
s.connect((direccion, puerto)) 
#Un caracter ocupa un byte, por tanto rellenamos la cadena con tantos bytes como queramos mandar
for i in range(NBytes):
	message = message + "a"
#Imprimimos la ip destino y los bytes a enviar
print "PING " + direccion + " (" + str(s.getpeername()[0]) +") " + str(NBytes) +  " bytes of data(TCP)."
#Repetimos el while para hacer tantos ping como queramos
while repeticiones > n:
	#Tiempo de inicio
	inicio = time.time() 
	#Envio del mensaje
	s.send(message)
	#Recepcion del mensaje
	data = s.recv(NBytes)
	#Tiempo de recepcion
	fin = time.time() 
	#Imprimimos los bytes recibidos, la direccion que nos lo envia, el hostname, el numero de secuencia y el tiempo del envio y recepcion
	print str(len(data)) + " bytes from " + direccion + " (" + str(s.getpeername()[0]) +"): Seq=" + str(n+1) + " Time="+ str((fin-inicio)*1000)+ " ms"
	#detenemos el programa 1s para enviar un ping cada segundo
	time.sleep(1)
	#Actualizar contador
	n=n+1
#Cierre del socket
s.close()