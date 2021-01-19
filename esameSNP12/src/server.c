#include "snp.h"
#include "common.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

void * thread_client(void *args);

_Noreturn void * thread_writer(void *args);
void *get_in_addr(struct sockaddr *sa);
void add_player(struct player *p);
void delete_player(int id);

struct player *players;

fd_set write_fds,master;

int main(int argc, char *argv[]){
    
    int yes = 1;
    int rv;
    int listener;
    int newclient;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof remoteaddr;
    char remoteIP[INET6_ADDRSTRLEN];
    pthread_t tid;
       
    struct addrinfo hints, *ai, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
    
    players = NULL;
    
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

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}
		break;
	}
	
	freeaddrinfo(ai);
    
    listen(listener,10);
    
    printf("Listening for new connections\n");
    FD_ZERO(&master);
    FD_ZERO(&write_fds);
    pthread_create(&tid,NULL,&thread_writer,NULL);
    while(1){
        newclient = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
        printf("New connection from %s on socket %d\n", inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newclient);
        pthread_create(&tid,NULL,&thread_client,&newclient);

    }
    
    exit(0);
}

void * thread_writer(void *args){
    struct player *p;
    char buf[4096];
    int i;

    struct timeval timeout;
    timeout.tv_sec = 1;

    while (1){
        write_fds = master;
        usleep(100);
        select(14,NULL,&write_fds,NULL,&timeout);
        for(i =0; i<14; i++){
            if(FD_ISSET(i,&write_fds)){
                for(p = players; p != NULL; p = p->next){
                    memset(buf,0,strlen(buf));
                    sprintf(buf,"id:%d,x:%d,y:%d\n",p->id,p->coord.x,p->coord.y);
                    send(i,buf, strlen(buf),0);
                }
            }
        }
    }
}
void * thread_client(void *args){
    char buf[4096];
    int n=-1;
        
    int socket = *((int *) args);

    printf("Thread serving %d\n",socket);
    
    struct player *current;
    struct player *p;
    struct player_pos coord;
    
    
    current = (struct player *) malloc(sizeof(struct player *));
    
    coord.x = rand()%50;
    coord.y = rand()%50;
    
    current->id = socket;
    current->coord = coord;
    current->next = NULL;
    
    add_player(current);
    sprintf(buf,"your id:%d\n",socket);
    send(socket,buf,sizeof buf,0);
    FD_SET(socket,&master);
    while (n != 0){
        n = recv(socket, buf, 1, 0);
        switch(buf[0]){
            case 'w':
                printf("ricevuto w da %d\n",socket);
                coord.y --;
                if(coord.y < 0)
                    coord.y = 0;
                current->coord = coord;
                break;
            case 's':
                coord.y++;
                if(coord.y > 50)
                    coord.y = 50;
                current->coord = coord;
                break;
            case 'a':
                coord.x--;
                if(coord.x <0)
                    coord.x = 0;
                current->coord = coord;
                break;
            case 'd':
                coord.x++;
                if(coord.x > 50)
                    coord.x = 50;
                current->coord = coord;
                break;
            default:
                break;
        }
    }
    FD_CLR(socket,&master);
    delete_player(socket);
    close(socket);
    pthread_exit(0);
}

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void add_player(struct player * p){
    struct player *current;
    struct player *prev;
    if(players == NULL){
     players = p;
    }
    else{
        prev = NULL;
        for(current = players; current != NULL; current = current->next){
            prev = current;
            printf("for current %d\n",current->id);
        }
        current = p;
        prev->next = current;
        current->next = NULL;
    }
    printf("\n\n");
    for(current = players; current != NULL; current = current->next){
        printf("ID: %d, X: %d, Y: %d\n",current->id,current->coord.x,current->coord.y);
    }
}

void delete_player(int id){
    printf("Deleting %d\n",id);
    struct player *current;
    struct player *prev;
    prev = NULL; 
    for(current = players; current != NULL; current = current->next){
        if ((current->id ) == id){
            if(prev == NULL){
                players = current->next;

            }
            else{
                prev->next = current->next;
            }
            free(current);
        }
        prev = current;
    }
}
