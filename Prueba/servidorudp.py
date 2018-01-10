import socket
import argparse

puerto=6666
#argparse nos permite gestionar los parametos de entrada
parser = argparse.ArgumentParser()
#Pedimos el puerto aunque por defecto es el 6666
parser.add_argument("-p", "--puerto", help="Asignar el puerto")
args = parser.parse_args()
#Guardamos ek puerto introducido
if args.puerto:
       puerto=int(args.puerto)
#Creamos el objeto socket con los argumentos de la conexion UDP
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#Con bind() indicamos que puerto escuchar, el primer parametro se deja vacio para aceptar conexiones de cualquier ip
s.bind(("", puerto))
while True:
       #Esperamos a recibir datos (un maximo de 256bytes) y los almacenamos en recibido
       recibido,addr = s.recvfrom(256)
       #imprimimos de donde nos llegan datos
       print "Mensaje recibido de: " + str(addr[0])
       s.sendto(recibido, addr)
       if len(recibido)<=0:
              break
s.close()
