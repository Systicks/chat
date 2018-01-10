import socket
import argparse
 
puerto=6666
#Argparse nos permite gestionar los parametros de entrada
parser = argparse.ArgumentParser()
#Pedimos el puerto aunque por defecto es el 6666
parser.add_argument("-p", "--puerto", help="Asignar el puerto")
args = parser.parse_args()
#Guardamos el puerto introducido en nuestro parametro
if args.puerto:
    puerto=int(args.puerto)
#Creamos el objeto socket con los argumentos para una conexion TCP
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#Con bind() indicamos en qeu puerto escuchar, el primer parametro se deja vacio para aceptar conexiones de cualquier ip
s.bind(("", puerto))
#Esperamos conexiones entrantes y establecemos 1 conexion a la vez como maximo
s.listen(1)
#sc servira para recibir las conexiones y en addr se almacenan la ip y el puerto desde la que se nos conectan
sc, addr = s.accept()
#Imprimimos la ip que estamos manejando
print "Conexion con: " + str(addr[0]) 

while True:
    #Esperamos a recibir datos (un maximo de 256 bytes) y los almacenamos en recibido
    recibido = sc.recv(256)
    #Devolvemos el mensaje al cliente
    sc.send(recibido)
    #Salir del while cuando no se reciban datos
    if len(recibido)<=0:
        break

#Cerramos la instancia del socket cliente y servidor
print "Cerrando sockets."
sc.close()
s.close()

 
