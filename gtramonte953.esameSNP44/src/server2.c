#include "snp.h"
#include "common.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

void * thread_f(void *args);

int main(int argc, char *argv[]){

	int yes = 1;
	int rv;
	int listener;
	int newclient;
	struct sockaddr_storage remoteaddr;
	pthread_t tid;
	
	socklen_t addrlen = sizeof remoteaddr; 
	
	struct addrinfo hints, *ai, *p; 
	memset(&hints, 0, sizeof hints); hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
   
	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) { 
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv)); 
		exit(1);
	}
	for(p = ai; p != NULL; p = p->ai_next) {
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) { 
		continue;
		}
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) { close(listener);
			continue;
		}
		break; 
	}
	
	freeaddrinfo(ai);
   	listen(listener,10);
   	
   	while(1){
   		newclient = accept(listener,(struct sockaddr *)&remoteaddr, &addrlen);
   		
   		pthread_create(&tid,NULL,&thread_f,&newclient);
		
   	}
   
}


void * thread_f(void *args){
	char buf[BUFSIZ];
	int n;
	int socket  = *((int *)args);
	
	n = recv(socket,buf,BUFSIZ,0);
   	if(n > 0){
   		send(socket,buf,strlen(buf),0);
   		close(socket);
	}
	pthread_exit(0);
}