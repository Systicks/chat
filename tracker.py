import socket
import select
import json

puerto=6666
ListaPeers = []
aux = []
lista = []
j=0
ident = 1
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(("", puerto))

while True:
    #utilizaremos un array con multiples sockets para realizar la funcion de select del timeout
    socket_list = [s]
    read_sockets, write_sockets, error_sockets = select.select(socket_list , [], [], 0.01)  
    #siempre que pasan 0,01s sin mensajes entramos en el if
    if not(read_sockets):
    	  #en caso de que la lista aux tenga elementos sean equivalentes al numero de peers menos 1 actualiza la listaPeers 
    	  if len(aux) > 0 and len(aux)/3 == len(ListaPeers)/3 -1 :
    	  	print "Alguien se fue"
    	  	ListaPeers = aux
    	  	aux = []

    for sock in read_sockets:
        #esperamos a recibir mensajes
        if sock == s:
       		recibido,addr = s.recvfrom(4000)
       		print "Mensaje recibido de: " + str(addr)
       		#obtenemos la lista de pueto e id=0 del peer
       		data = json.loads(recibido)
       		puerto = data[0]
       		num = data[1]
       		#comprobamos el id recibido, si es !0 es que es un usuario ya existente y rellenar aux
       		if int(num) != 0 :
       			aux.append(addr[0])
       			aux.append(int(puerto))
       			aux.append(ident)
       			
       		#en caso contrario es un usuario nuevo	
       		else :
       			#guardamos al peer en una Lista temporal
       			lista = [addr[0] , int(puerto) , ident]
       			#anyadimos el id al final de la lista de  peers para darselo al peer y lo enviamos
       			ListaPeers.append(ident)
       			ms = json.dumps(ListaPeers)
       			#quitamos el id de la lista de peers
       			del ListaPeers[-1]
       			ident = ident + 1
       			s.sendto(ms, addr)
       			#anyadimos la lista temporal con el nuevo peer a la lista de peers
       			ListaPeers = ListaPeers + lista


       
s.close()