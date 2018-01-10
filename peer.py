import socket, select, string, sys, json
 #funcion que escribe Tu => y limpia el buffer
def prompt() :
    sys.stdout.write('Tu => ')
    sys.stdout.flush()

#funcion para enviar mensajes a todos los peers
def broadcast (message):
    for socket in socket_list:
    	#se excluyen el sys.stdin y el socket de escucha de la lista de sockets
        if socket != s and socket != sys.stdin:
            try :
                socket.send(message)
            except :
                socket.close()
                socket_list.remove(socket)
 
#main function
if __name__ == "__main__":
     
    if(len(sys.argv) < 4) :
        print 'Por favor introduzca host del servidor, puerto del que queremos escuchar y un usuario'
        sys.exit()
     
    host = sys.argv[1]
    port = int(sys.argv[2])
    user = sys.argv[3]
    #indicamos id=0 para que el servidor sepa que es un peer no registrado
    ident = 0

    #creamos el socket tcp para escuchar y recibir conexiones
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(("", port))
    s.listen(20)

    #Creamos el socket udp y mandamos un mensaje al servidor con nuestro puerto de escucha y el id=0
    udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    ms = json.dumps([int(port), int(ident)])
    udp.sendto(ms, (host,6666))
    #obtenemos el mensaje del servidor y de el sacamos la lista de peers
    data,addr = udp.recvfrom(4096)
    listapeers = json.loads(data)
    #asignamos el id que se encuentra al final de la lista a nuestro peer y lo eliminamos de la lista
    ident = int(listapeers[-1])
    del listapeers[-1]
    #creamos el mensaje a enviar para cuando se desconecte algun peer. Ahora el id!=0, es un peer ya registrado
    ms = json.dumps([int(port), int(ident)])
    
    #creamos la lista de conexiones tcp
    conexiones = []
    numpeers = len(listapeers)/3

    j = 0
    i = 0
    #creamos todas las conexiones a los peers que obtenemos de la lista
    while i < numpeers:
    	#anyadimos un sockeet tcp a la lista de conexiones
        conexiones.append(socket.socket(socket.AF_INET, socket.SOCK_STREAM))
        try :
                #nos conectamos con ese socket a un peer de la lista
                conexiones[i].connect((listapeers[j], listapeers[j+1]))
        except :
        	#en caso de error, si el numero de peers de la lista es 1 es probable que ese se haya desconectado y nadie ha podido notificarlo
        	#cerramos el socket y enviamos un mensaje al servidor de que alguien se ha desconectado para que actualice la lista
            if numpeers == 1:
               	conexiones[i].close()
                del conexiones[i]
                udp.sendto(ms, (host,6666))
        #incrementamos i y j, i es por el numero de peers, va de uno en uno, y j es para recorrer la lista de lista peers
        #va de 3 en 3, ya que se compone de ip, puerto e id, cada tres es un nuevo peer.    	           
        j = j+3
        i = i+1
    #preparamos el terminal para que el usuario ya vea para escribir    
    prompt()
    #creamos socket_list para el select
    socket_list = [sys.stdin, s] 
    socket_list = socket_list + conexiones

    while 1:
          
        
        read_sockets, write_sockets, error_sockets = select.select(socket_list , [], [])

        for sock in read_sockets:
            
            if sock == s:
            	#se recibe una peticion por el socket de escucha, lo aceptamos y guardamos el socket en la lista de sockets
                sockfd, addr = s.accept()
                socket_list.append(sockfd)
                    
            #Se detecta que queremos escribir
            elif sock == sys.stdin :
            	#se guarda lo que escribimos en msg y se manda a todos por la funcion broadcast
                msg = sys.stdin.readline()
                #si escribimos quit cerramos todos los sockets y nos desconectamos
                if msg == "quit\n":
                	udp.close()
                	del socket_list[0]
                	for elemento in socket_list:
                		elemento.close()
                	sys.exit()
                broadcast("\r" + user + " => " + msg)
                prompt()
            #se recibe un mensaje de cualquiera de los otros sockets de la lista    
            else :
                data = sock.recv(4096)
                #mensaje de que el peer se ha desconectado, cerramos el socket y avisamos al servidor
                if not data :
                    print '\nAlguien se desconecto'
                    prompt()
                    sock.close()
                    socket_list.remove(sock)
                    udp.sendto(ms, (host,6666))
                else :
                    #escribimos el mensaje en la pantalla
                    sys.stdout.write(data)
                    prompt()
                    
    s.close()
    udp.close()