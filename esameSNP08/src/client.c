#include "snp.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[]){
	
	int n;
	char buf[BUFSIZ];
	
	if(argc != 4){
		printf("Usage ./%s <hostname> <port> <message>",argv[0]);
	}
	
	struct addrinfo hints, *result, *p;
    int sockfd;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags |= AI_CANONNAME;

    getaddrinfo(argv[1], argv[2], &hints, &result);

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
    
    n = send(sockfd,argv[3],sizeof(argv[3]),0);
    if(n <0) 
    	err_sys("Error sending");
    n = recv(sockfd,buf,BUFSIZ,0);    
    if(n<0)
    	err_sys("Error receiving");
    printf("Ricevuto : \n%s\n",buf);
	exit(0);
}