#include <stdlib.h>
#include "common.h"
#include "snp.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/wait.h>


int main(int argc, char *argv[]){
    
    if(argc != 3){
        printf("Usage: ./client <hostname or ip server> <line to request>\n");
        exit(-1);
    }
    
    char buf[BUFSIZ];
        
    struct addrinfo hints, *result, *p;
    int sockfd;
    
    int n;
    
    int pipefd[2];
    pid_t pid;
    
    pipe(pipefd);
        
    pid = fork();
    
    switch(pid){
        case -1:
            err_sys("Error during fork()");
            break;
        case 0: /*child*/
            if(close(pipefd[1]) == -1)
                err_sys("Error closing pipfd 1");
            if (pipefd[0] != STDIN_FILENO) {              /* Defensive check */
            if (dup2(pipefd[0], STDIN_FILENO) == -1)
                err_sys("Error on dup2");
            }
            if(execl("./multicaster", "multicaster", (char *) NULL) == -1)
                err_sys("Exec failed");
            break;
        default: break;
    }
    
    if( close(pipefd[0]) == -1)
        err_sys("Error closing pipefd 0");
    
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
            err_sys("connection error:");
            continue;
        }
        break;
    }      
    
    freeaddrinfo(result);
    

    n = send(sockfd,argv[2],strlen(argv[2]),0);
    
    while(n !=0 ){
        n = recv(sockfd,buf,BUFSIZ,0);
        if(n >0 ){
            write(pipefd[1],buf,n);
        }
    }
    
    close(pipefd[1]);
    wait(&pid);
    close(sockfd);
    
    exit(0);
}
