#include <stdlib.h>
#include "common.h"
#include "snp.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/mman.h>


void *get_in_addr(struct sockaddr *sa);

int main(int argc, char *argv[]){
    
    int yes = 1;
    int rv;
    int listener;
    
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof remoteaddr;
    
    int fd;
    char *file_mapping;
    char *str;
    struct stat statbuf;
    int n;
    char buf[BUFSIZ];
    char *sendbuff;
    int lines;
    int i;
    
    fd = open("../README", O_RDONLY);
    if(fd < 0)
        err_sys("Errore aprendo il file:");
    
    if(fstat(fd,&statbuf) < 0)
        err_sys("Impossible to stat file descriptor\n");

    file_mapping =(char *) mmap(NULL,statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    str = (char *) malloc(statbuf.st_size);
    close(fd);
    
    struct addrinfo hints, *ai, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
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

                if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
                        close(listener);
                        continue;
                }
                break;
        }

        freeaddrinfo(ai);
    
    listen(listener,10);

    while(1){
        fd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
        printf("accepted new connection\n");
        memset(buf,0,BUFSIZ);
        n = recv(fd,buf,BUFSIZ,0);
        printf("received request from client %d\n",atoi(buf));
        if( n > 0){
            lines = atoi(buf);
            memcpy(str,file_mapping,strlen(file_mapping));
            sendbuff = strtok(str, "\n");
            if(lines > 1){
                for( i = 1;i<lines;i++){
                    sendbuff = strtok(NULL, "\n");
                    if(sendbuff == NULL){
                        sendbuff = malloc(sizeof(char));
                        strcpy(sendbuff,"\0");
                        break;
                    }
                }
            }
            send(fd,sendbuff,strlen(sendbuff),0);
            close(fd);
        }
        else{
            printf("An error occurred during connection\n");
            close(fd);
        }
            
    }
    
    exit(0);
}
