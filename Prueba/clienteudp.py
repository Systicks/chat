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
#Anadimos string para la IP y el resto de parametos que se aceptan
parser.add_argument('string')
parser.add_argument("-p", "--puerto", help="Asignar puerto")
parser.add_argument("-r", "--repeticiones", help="Numero de repeticiones del ping")
parser.add_argument("-l", "--longitud", help="Longitud del ping")
args = parser.parse_args()
# Aqui procesamos lo que se hace con cada argumento
if args.puerto:
       puerto=int(args.puerto)
if args.repeticiones:
       repeticiones=int(args.repeticiones)
if args.longitud:
       NBytes=int(args.longitud)
if args.string:
       direccion=args.string
#creamos el objeto socket con los argumentos para udp
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#un caracter ocupa un byte, por tanto rellenamos la cadena con tantos bytes como qeramos mandar
for i in range(NBytes):
       message = message + "a"
#imprimimos la ip destino y los bytes a enviar
print "PING " + direccion + str(NBytes) + " bytes of data(UDP)."
#repetimos el while para hacer tantos ping como queramos
while repeticiones > n:
       #Tiempo de inicio
       inicio = time.time()
       #Envio del mensaje
       s.sendto(message,(direccion,puerto))
       #Recepcion del mensaje
       data,addr=s.recvfrom(NBytes)
       #Tiempo de recepcion
       fin = time.time()
       #imprimimos los byes recibidos, la direccion que nos los envia, el hostname, el numero de secuencia y el tiempo de envio y recepcion
       print " " + str(len(data)) + " bytes from " + direccion + " " + str(addr[0]) +":  Seq=" + str(n+1) + " Time="+ str((fin-inicio)*1000)+ " ms"
       #detenemos el progama 1s para envia un ping cada segundo
       time.sleep(1)
       #Actualizar contador
       n=n+1
s.close()
