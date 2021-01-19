#include <socket_header.h>
#include <snp.h>

static int total = 0;
static pthread_barrier_t barrier;
static int connected;

static void * threadFunc(void *arg)
{
    int sockfd = (int) arg;
    char msg[BUFSIZE] = "";
	int counter = 0;
	ssize_t n = 0;
	int openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    int filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;      /* rw-rw-rw- */
    int outputFd = open("client_file_out.txt", openFlags, filePerms);
	int i = 0;
    while(connected){
		n = recv(sockfd, msg, BUFSIZE -1, 0);
		if (n == -1) {
			perror("recv() error: ");
			close(sockfd);
			connected =0;
		} 
		if (n > 0) {
			total += n;
			write(outputFd,msg,n);
		}
    }
	//4-way teardown
	close(sockfd);
	close(outputFd);
    return (void *) 0;
}

static void * threadSpeed(void *arg){
	char buf[BUFSIZE];
	while(connected){
		sleep(1);
		sprintf(buf,"speed %d byte/s\r",total);
        write(STDOUT_FILENO,buf,BUFSIZE);
        total = 0;
	}
	pthread_barrier_wait(&barrier);
	return (void *) 0;
}

int main(int argc, char *argv[]){
	
	int res = 0; //valore di ritorno delle Sockets API
	char buf[BUFSIZE];
	int sockfd = 0; //connection socket: servirà per la comunicazione col server
	pthread_t t1;
	pthread_t t2;
    void *tres;
    int s;
    
	struct sockaddr_in server; //IPv4 server address, senza hostname resolution 
	socklen_t len = sizeof(server); //dimensione della struttura di indirizzo
	
	memset(&server, 0, sizeof(server)); //azzero la struttura dati
	server.sin_family = AF_INET; //specifico l'Address FAmily IPv4
	server.sin_port = htons(PORT);
	
	if (argc == 1)
	{
		printf("\tTCP Client app connecting to localhost TCP server...\n");
		server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	} 
	else 
	{
		printf("\tTCP Client app connecting to TCP server at '%s'\n", argv[1]);
		
		/* Utilizzo inet_pton() per convertire l'indirizzo dotted decimal */
		res = inet_pton(AF_INET, argv[1], &(server.sin_addr));
		if (res == 1){
			printf("Memorizzato l'indirizzo IPv4 del server\n");
		}
		else if (res == -1)
		{
			perror("Errore inet_pton: ");
			return FAILURE;
		}
		else if (res == 0)
		{
			printf("The input value is not a valid IPv4 dotted-decimal string\n");
			return FAILURE;
		}
	}
	
	//connessione al server
	/* open socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket error: ");
		return FAILURE;
	}
	
	//avvio il 3WHS
	res = connect(sockfd, (struct sockaddr *)&server, len);
	if (res != 0)
	{
		perror("connect() error: ");
		close(sockfd);
		return FAILURE;
	}
	connected =1;
	s = pthread_barrier_init(&barrier, NULL, 2);
	pthread_create(&t1, NULL, threadFunc, sockfd);
	pthread_create(&t2, NULL, threadSpeed, NULL);
	
	while(connected){
        read(STDIN_FILENO,buf,1);
        if((buf[0]=='u')||(buf[0]=='d'))
            //printf("------");
            send(sockfd,buf,1,0);
    }
    s = pthread_join(t1, &tres);
    s = pthread_join(t2, &tres);
return 0;
}
