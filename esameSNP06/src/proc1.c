#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "snp.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


int run;
pid_t other_process = 0;

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void * thread_signal_handler(void *arg);

int main(int argc, char *argv[]){
    
    char buf[200];
    
    pthread_t tid1;
    sigset_t waitset,oldset;
    key_t shm_key;
    int shmid;
    
    struct shmid_ds memory_stat;
    void *memory_address;
    char *memory_shared;
    
    srand((unsigned int)getpid()); 
    
    sigfillset(&waitset);
    sigprocmask(SIG_BLOCK,&waitset,&oldset);
    
    shm_key = ftok("../README",1);
    
    shmid = shmget(shm_key, sizeof(char), 0600 | IPC_CREAT);
    
    memory_address = shmat(shmid, (void *)0, 0);
    
    memory_shared = (char *) memory_address;
  
    pthread_create(&tid1, NULL, thread_signal_handler, NULL);
    
    run = 1;
    
    while(run){
        if(other_process == 0){
            shmctl(shmid,IPC_STAT,&memory_stat);
            if(memory_stat.shm_cpid != memory_stat.shm_lpid){
                other_process = memory_stat.shm_lpid;
                sprintf(buf,"Other Process found with pid: %d\n",other_process);
                cPrintf(buf,RED);
            }
        }
        else{
            sleep(rand() % 10);
            
            sprintf(buf,"Current memory: %s\n",memory_shared);
            cPrintf(buf,ORANGE);
            
            pthread_mutex_lock(&mtx);
            kill(other_process,SIGUSR1);
            cPrintf("Locked sending SIGUSR1\n",RED);
        	sleep(rand() %3);
            
            *memory_shared = (char) (97+ rand() % 26);
            sprintf(buf,"after write memory: %s\n",memory_shared);
            cPrintf(buf,ORANGE);
            
            kill(other_process,SIGUSR2);
            pthread_mutex_unlock(&mtx);
            
        	cPrintf("Unlocked sending SIGUSR2\n",RED);
        }
    }

    pthread_join(tid1,NULL);
    
    cPrintf("Detaching and deleting memory\n",RED);
    shmdt(memory_address);
    
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }
    
    exit(0);
}

void * thread_signal_handler(void *arg){
    char buf[200];
    sigset_t waitset;
    int signo;
    sigfillset(&waitset);
    while(run){
        sigwait(&waitset,&signo);
        switch(signo){
            case SIGUSR1:{
                cPrintf("L'altro processo vuole scrivere prendo il lock\n",GREEN);
                pthread_mutex_lock(&mtx);
                break;
                }
            case SIGUSR2:{
                cPrintf("L'altro processo ha finito rilascio il lock\n",GREEN);
                pthread_mutex_unlock(&mtx);
                break;
                }
            case SIGINT:{
                cPrintf("Ricevuto il segnale SIGINT, termino i thread ed esco\n", GREEN);
                run = 0;   
                kill(other_process,SIGINT);
                pthread_mutex_unlock(&mtx);
                break;
                }
            default:{
                sprintf(buf,"Ricevuto il segnale %d",signo);
                cPrintf(buf,GREEN);
                break;
                }
        }
    }
    pthread_exit(0);
}
