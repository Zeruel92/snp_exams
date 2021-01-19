#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include "snp.h"

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void * thread_f1(void *arg);
void * thread_f2(void *arg);

static int sockfd;


int main(int argc, char *argv[]){

    pthread_t t1,t2;
    
	struct addrinfo hints; 
	
	struct addrinfo *res; 
	struct addrinfo *p;
	
	memset(&hints, 0, sizeof hints);
	
	hints.ai_family = AF_INET; 	
	
	hints.ai_socktype = SOCK_STREAM;

	
	hints.ai_flags |= AI_V4MAPPED; 
	
	getaddrinfo(argv[1], argv[2], &hints, &res);
	

	for (p = res; p != NULL; p = p->ai_next) 
	{
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol); 
		if (sockfd == -1)
			continue;
		
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			printf("client connecting in the %s domain error\n", (p->ai_family == AF_INET) ? "AF_INET": "AF_INET6");
			perror("connect error:");
			continue;
		}
	break;
	}

	if (p == NULL) 
	{
		fprintf(stderr, "Client failed to connect\n");
		return -1;
	}
	
	freeaddrinfo(res); 

    pthread_create(&t1, NULL, thread_f1, NULL);
    pthread_create(&t2, NULL, thread_f2, NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    
    close(sockfd);
    

return 0;
}

void * thread_f1(void *arg){
    char buf[4096];
    char srv_reply[250];
        
    while(1){
        sleep(1);
        pthread_mutex_lock(&mtx);
        sprintf(buf, "invio thread 1\n\r");
        cPrintf(buf,RED);
        send(sockfd,buf,sizeof(buf),0);
        recv(sockfd,srv_reply,sizeof(buf),0);
        sprintf(buf,"Messagio di risposta dal server: %s\n",srv_reply);
        cPrintf(buf,RED);
        pthread_mutex_unlock(&mtx);
        pthread_cond_signal(&cond);
    }
}

void * thread_f2(void *arg){
    char buf[4096];
    char srv_reply[250];
    
    while(1){
        pthread_mutex_lock(&mtx);
        pthread_cond_wait(&cond, &mtx);
        sprintf(buf ,"invio thread 2\n\r");
        cPrintf(buf,ORANGE);
        send(sockfd,buf,sizeof(buf),0);
        recv(sockfd,srv_reply,sizeof(buf),0);
        sprintf(buf,"Messagio di risposta dal server: %s\n",srv_reply);
        cPrintf(buf,ORANGE);
        pthread_mutex_unlock(&mtx);        
    }
}
