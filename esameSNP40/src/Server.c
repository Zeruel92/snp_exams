#include <socket_header.h>
#include <snp.h>

void initLocalAddress(struct sockaddr_in *addr){
	
	memset(addr, 0, sizeof(struct sockaddr_in));
	addr->sin_family= AF_INET;	//IPv4 family

	addr->sin_addr.s_addr = htonl(INADDR_ANY);

	addr->sin_port = htons(PORT);
}

static float speed = 1.00;
static pthread_barrier_t barrier;
static int connected = 0;

static void * threadFunc(void *arg)
{
    int peerfd = (int) arg;
    char clientaddr[INET_ADDRSTRLEN] = "";
    char buf[BUFSIZE];
    ssize_t n = 0;
    //connected = 1;
    write(STDOUT_FILENO,"Thread 1 avviato\n\r",17);
    while (connected) 
	{	
		n = send(peerfd,buf,1,0);
        sleep(1.0*speed);
        sprintf(buf,"speed %f\r",1.0*speed);
        write(STDOUT_FILENO,buf,BUFSIZE);
	}
	
    close(peerfd);
    write(STDOUT_FILENO,"Thread 1 fermato\n\r",17);
    pthread_barrier_wait(&barrier);
    return (void *) 0;
}

static void * threadRecv(void *arg)
{
    
    int peerfd = (int) arg;
    char clientaddr[INET_ADDRSTRLEN] = "";
    char msg[BUFSIZE];
    char buf[BUFSIZE];
    ssize_t n = -1;
    write(STDOUT_FILENO,"Thread 2 avviato\n\r",17);

    while(connected){
        n = recv(peerfd, msg, BUFSIZE -1, 0);       	
        if(msg[0]=='u'){
            speed -= 0.05;
            sprintf(buf,"updated speed %f\r\n",speed);
            write(STDOUT_FILENO,buf,BUFSIZE);
            //printf("speed %d",speed);
			msg[0] = 'c';
        }
        else if (msg[0]=='d'){
            speed +=0.05;
         //   sprintf(buf,"speed %f\r",speed);
          //  write(STDOUT_FILENO,buf,BUFSIZE);
            msg[0] = 'c';
        }
    }

    //close(peerfd);
    write(STDOUT_FILENO,"Thread 2 fermato\n\r",17);
    pthread_barrier_wait(&barrier);
    return (void *) 0;
}

int main(int argc, char *argv[]){
		
	int sockfd = 0; /* listening socket del server TCP */
	int peerfd = 0; /* connected socket del client TCP */
	int ret = 0; 	/* valore di ritorno delle Sockets API */
	struct sockaddr_in addr; /* server local address, for bind() */
	pthread_t t1;
    pthread_t t2;
    void *res;
    int s;

	
	//step 1: open listening socket per IPv4
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket() error: ");
		return FAILURE;
	}
	
	/*
	Assegno al socket l'indirizzo IPv4 memorizzato in addr
	*/
	initLocalAddress(&addr);
	
	ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	if (ret == -1)
	{
		perror("bind() error: ");
		close(sockfd);
		return FAILURE;
	}
	

	ret = listen(sockfd, BACKLOG);
	if (ret == -1)
	{
		perror("listen() error: ");
		close(sockfd);
		return FAILURE;
	}
	
	printf("\n\tServer listening on port %d\n", (int)PORT);
	
	struct sockaddr_in peer_addr;
	socklen_t len = sizeof(peer_addr);
	
	int quit = 0; //regola il loop infinito nel server
	 //regola la gestione della connessione col client
	s = pthread_barrier_init(&barrier, NULL, 2);
	while (!quit) 
	{
		peerfd = accept(sockfd, (struct sockaddr *)&peer_addr, &len);
		if (peerfd == -1)
		{
			perror("accept() error: ");
			close(sockfd);
			return FAILURE;
		}
       // printf("Thread avviati");
        connected = 1;
        pthread_create(&t1, NULL, threadFunc, peerfd);
		pthread_create(&t2, NULL, threadRecv, peerfd);
        //printf("Thread avviati");
		s = pthread_join(t1, NULL);
        s = pthread_join(t2, NULL);
	}//wend quit
	close(sockfd);
	
return 0;
}

