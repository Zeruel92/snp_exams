#include "snp.h"
#include "common.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

char message[BUFSIZ];
char address[BUFSIZ];

void * thread_client(void *args);

int main(int argc, char *argv[]){
	
	char buf[BUFSIZ];
	
	if(argc!=3){
		sprintf(buf,"Usage: ./client <hostname> <message to send>\n");
		write(STDERR_FILENO,buf,strlen(buf));
	}
	
	
	int count = 0;
	struct timeval inizio,fine;
	
	pthread_t threads[10];
				
	sprintf(message,"%s",argv[2]);
	sprintf(address,"%s",argv[1]);
	
	gettimeofday(&inizio,NULL);
	
	while(count <10 ){
		pthread_create(&threads[count], NULL, &thread_client,NULL);
		count++;
	}

	count = 0;
	while(count < 10 ){
		pthread_join(threads[count], NULL);
		count++;
	}
	
	gettimeofday(&fine,NULL);
	#ifdef MACOS
	printf("Il tempo impiegato dal server per rispondere è di %d microsecondi\n",(fine.tv_usec - inizio.tv_usec));
	#endif
	#ifdef LINUX
	printf("Il tempo impiegato dal server per rispondere è di %lu microsecondi\n",(fine.tv_usec - inizio.tv_usec));
	#endif
	exit(0);
}

void * thread_client(void *args){
	
	char buf[BUFSIZ];	
	struct addrinfo hints, *result, *p;
	
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_protocol = IPPROTO_TCP; 
	hints.ai_flags |= AI_CANONNAME;
	
    getaddrinfo(address, PORT, &hints, &result);
	
	int sockfd; 
	int n=-1;
	
	for(p = result; p != NULL; p = p->ai_next){
			sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
		if ((connect(sockfd, p->ai_addr,p->ai_addrlen)) == -1){
			close(sockfd); 
			perror("connection error:"); 
			continue;
			}
		break; 
	}
	freeaddrinfo(result);
	
	send(sockfd,message,strlen(message),0);
	while (n!=0){
		n=recv(sockfd,buf,BUFSIZ,0);
		printf("%s\n",buf);
	}
	close(sockfd);	
	pthread_exit(0);
}