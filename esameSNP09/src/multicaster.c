#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "snp.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include "common.h"

int main(int argc, char *argv[]){
    char buf[BUFSIZ];
    int n;
    	
  	char *multicastIPString = MULTICAST_IP;   
  	char *service = MULTICAST_PORT;            
  	
  	struct addrinfo hints, *res, *p;                   
  	
    n = read(STDIN_FILENO,buf,BUFSIZ);
    printf("Sending to multicast: %s\n",buf);

    if(n < 0){
        err_sys("Error reading Standard Input");
    }
    
	memset(&hints, 0, sizeof(hints)); 
  	
  	hints.ai_family = AF_UNSPEC;            
  	hints.ai_socktype = SOCK_DGRAM;         
  	hints.ai_protocol = IPPROTO_UDP;        
  	hints.ai_flags |= AI_NUMERICHOST;       


	hints.ai_flags |= AI_NUMERICSERV;       
		
  	int ecode = getaddrinfo(multicastIPString, service, &hints, &res);
    if(ecode != 0)
        err_sys("Error resolving address");
    
    int sockfd = -1;
  	
  	for (p=res; p != NULL; p=p->ai_next)
  	{
  		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
  		if (sockfd < 0)
    	{
    		perror("socket() failed: ");
    		continue;
		}
		break;
	}
	
	if (p == NULL)
	{
		freeaddrinfo(res);
		err_sys("Unable to open socket for multicast transmission");
	}
	
	n = sendto(sockfd, buf, strlen(buf), 0,  p->ai_addr, p->ai_addrlen);
        
    close(sockfd);
    
    exit(0);
}
