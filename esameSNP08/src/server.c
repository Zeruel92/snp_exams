#include "snp.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>

fd_set master,sockets;
int fdmax;

void * thread_f(void *args);

int main(int argc, char *argv[]){

	int yes = 1;
    int rv;
    int listener;
	int i;
	
	char buf[BUFSIZ];
	int sockets[10];
	
	struct addrinfo hints, *ai, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	pthread_t tid;
	
	FD_ZERO(&master);
	FD_ZERO(&sockets);
	
	daemonize("server");
	
	for(i = 0; i < 10; i++){
		
		sprintf(buf,"%d",49999+i);
		
		if((rv = getaddrinfo(NULL, buf, &hints, &ai)) != 0) {
			err_sys("Server error:");
		}
	
		for(p = ai; p != NULL; p = p->ai_next) {
    		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (listener < 0) { 
				continue;
			}	
		
			setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

			if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
				close(listener);
				continue;
			}
			break;
		}
	
		freeaddrinfo(ai);
    
    	listen(listener,10);
    	FD_SET(listener,&master);
    	fdmax = listener +1;
    	printf("listening on : %d",49999+i);
    
    }
    
    pthread_create(&tid, NULL, thread_f, NULL);
	pthread_join(tid,NULL);
	exit(0);
}

void * thread_f(void *args){
	int i,n,conn;
	struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof remoteaddr;
    char buf[BUFSIZ];
    
	while(1){
		sockets = master;
		select(fdmax,&sockets,NULL,NULL,NULL);
		for(i = 0; i<fdmax; i++){
			if(FD_ISSET(i,&sockets)){
				conn = accept(i,(struct sockaddr *)&remoteaddr, &addrlen);
				n = recv(conn,buf,BUFSIZ,0);
				send(conn,buf,n,0);
				close(conn);
			}
		}
	}
	pthread_exit(0);
}