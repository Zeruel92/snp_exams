#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#define RED 1
#define ORANGE 2
#define GREEN 3

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void * thread_f1(void *arg);
void * thread_f2(void *arg);
void * thread_f3(void *arg);

void cPrintf(char *,int);

char buf[1024];
int run;

int main(int argc, char *argv[]){
    
    pthread_t t1,t2,t3;
    sigset_t waitset,oldset;
    
    run = 1;
    
    sigfillset(&waitset);
    sigprocmask(SIG_BLOCK,&waitset,&oldset);
    
    
    pthread_create(&t1, NULL, thread_f1, NULL);
    pthread_create(&t2, NULL, thread_f2, NULL);
    pthread_create(&t3, NULL, thread_f3, NULL);
    
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    
    exit(0);
}

void * thread_f1(void *arg){
    int fd,n;
    cPrintf("Avvio Thread 1\n",RED);
    fd = open("esameSNP29.txt",O_RDONLY);
    
    while(run){
        sleep(1);
        cPrintf("Leggo\n",RED);
        pthread_mutex_lock(&mtx);
        n = read(fd,buf,1024);
        pthread_mutex_unlock(&mtx);
        cPrintf("Invio il signal al cond var\n",RED);
        pthread_cond_signal(&cond);
    }
    
    cPrintf("Chiudo il file descriptor ed esco\n",RED);
    
    close(fd);
    pthread_exit(0);    
}

void * thread_f2(void *arg){
    int fd;
    
    cPrintf("Avvio Thread 2\n",ORANGE);
    
    fd = open("file_output.txt", O_CREAT | O_WRONLY | O_APPEND, 0600);
    
    
    while(run){
        pthread_mutex_lock(&mtx);
        cPrintf("Mi metto in attesa sulla cond var\n",ORANGE);
        pthread_cond_wait(&cond, &mtx);
        cPrintf("Ricevuto il segnale dalla condition variable scrivo\n",ORANGE);
        write(fd,buf,1024);
        pthread_mutex_unlock(&mtx);
    }
    
    cPrintf("Chiudo il file descriptor ed esco\n",ORANGE);
    
    close(fd);
    pthread_exit(0); 
}

void * thread_f3(void *arg){
    sigset_t waitset;
    int signo;
    sigfillset(&waitset);
    while(run){
        sigwait(&waitset,&signo);
        if(signo == SIGINT){
            cPrintf("Ricevuto il segnale SIGINT, termino i thread ed esco\n", GREEN);
            run = 0;            
        }
    }
    
    pthread_exit(0);
}

void cPrintf(char *str,int color){
    switch(color){
        case RED:
            printf("\033[\1;0;31m%s\033[\1C\033[1;0m\r",str);
            break;
        case GREEN:
            printf("\033[0;32m%s\033[\1C\033[1;0m\r",str);
            break;
        case ORANGE:
            printf("\033[0;33m%s\033[\1C\033[1;0m\r",str);
            break;
        default:
            printf("%s",str);
            break;
    }
}
