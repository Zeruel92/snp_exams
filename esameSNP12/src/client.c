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

void sig_catch(int signo)
{
    tty_reset(STDIN_FILENO);
    printf("signal caught\n");
    exit(0);
}

void add_player(struct player *p);
void delete_player(int id);
void update(int x, int y, int id);

struct player *players;

int main(int argc, char *argv[]){

    players = NULL;
    struct player *current;
    char comodo[4096];

    struct addrinfo hints, *result, *p;
    int sockfd;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags |= AI_CANONNAME;

    getaddrinfo(argv[1], PORT, &hints, &result);

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
    char buf[BUFSIZ]= "";
    size_t n = 0;
    int id = -1;
    n = recv(sockfd,buf,BUFSIZ,0);

    sscanf(buf,"your id:%d",&id);
    printf("Ricevuto id:%d\n",id);
    if(tty_raw(STDIN_FILENO) <0)
        err_sys("Error setting terminal in raw");
    printf("Entering raw mode \n");
    int id_u,x_u,y_u;
    id_u=0;
    x_u=0;
    y_u=0;

    signal(SIGINT, sig_catch);
    fd_set read_fds,master;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO,&master);
    FD_SET(sockfd,&master);

    while (n != 0 ){
        read_fds = master;
        select(4,&read_fds,NULL,NULL,NULL);
        
        if(FD_ISSET(sockfd,&read_fds)) {
            /*printf("recv");*/
            n = recv(sockfd, buf, BUFSIZ, 0);
            if (n > 0) {
                sscanf(buf, "id:%d,x:%d,y:%d", &id_u, &x_u, &y_u);
                update(x_u,y_u,id_u);
            } else {
                perror("Error in recv:");
            }
        }
        else
        {
            n = read(STDIN_FILENO,buf,1);
            if(buf[0] == 'q')
                break;
            send(sockfd,buf,1,0);
        }
        printf("\033[2J");
        memset(buf,0,strlen(buf));
        memset(comodo,0,strlen(comodo));
        for(current = players; current != NULL; current = current->next){
            if (current->id == id) {
               sprintf(comodo,"\033[%d;%dH🕷 ", current->coord.y, current->coord.x);
                strcat(buf,comodo);
                /*printf("T");*/
            } else {
                sprintf(comodo,"\033[%d;%dH🌻 ", current->coord.y, current->coord.x);
                strcat(buf,comodo);
               /* printf("E");*/
            }
        }
        printf("%s",buf);
        fflush(stdout);
        usleep(100);
    }
    close(sockfd);
    tty_reset(STDIN_FILENO);
    exit(0);
}

void update(int x, int y, int id){
    struct player *current;
    int found = 0;
    for(current = players; current != NULL; current = current->next){
        if(current->id == id){
            current->coord.x = x;
            current->coord.y = y;
            found = 1;
        }
        /*printf("for current %d\n",current->id);*/
    }
    if (found == 0){
        current = (struct player *) malloc(sizeof (struct player *));
        current->id = id;
        current->coord.x =x;
        current->coord.y = y;
        add_player(current);
    }
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
            /*printf("for current %d\n",current->id);*/
        }
        current = p;
        prev->next = current;
        current->next = NULL;
    }
    /*printf("\n\n");
    for(current = players; current != NULL; current = current->next){
        printf("ID: %d, X: %d, Y: %d\n",current->id,current->coord.x,current->coord.y);
    }*/
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
