#include "snp.h"
#include "common.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[]){
	char buf[BUFSIZ];
	int n;
	int yes = 1;
	int rv;
	int listener;
	int newclient;
	struct sockaddr_storage remoteaddr;
	pid_t pid;

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
   		pid = fork();
   		
   		switch(pid){
   			case 0: /* child process */
   				close(listener);
   				n = recv(newclient,buf,BUFSIZ,0);
   				if(n > 0){
   					/*printf("ricevuto dal client: %s\n",buf);*/
   					send(newclient,buf,strlen(buf),0);
   					close(newclient);
   				}
   				exit(0);
   				break;
   			case -1:
   				err_sys("Fork failed");
   			default:
   				close(newclient);
   			break;
   		}
   	}
   
}