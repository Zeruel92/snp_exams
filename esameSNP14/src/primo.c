#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#ifdef SYSV
#include <sys/shm.h>
#endif

#ifndef SYSV
#include <sys/mman.h>
#endif

#include <pthread.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	
	void *shared_memory;
	char msg[10] = "proc1";
    
#ifdef SYSV
    int shmid;
	shmid = shmget((key_t)9999, sizeof(msg), 0666 | IPC_CREAT);
	shared_memory = shmat(shmid, (void *)0, 0);
#endif
    
#ifdef POSIX
    int shmid;
	shmid = shm_open("/sharedmem",O_CREAT | O_RDWR ,0666);
	ftruncate(shmid,sizeof(msg));
	shared_memory = mmap(NULL,sizeof(msg),PROT_WRITE|PROT_READ,MAP_SHARED,shmid,0);
#endif


	pthread_mutex_t	*mtx;
	pthread_mutexattr_t mta;
	
	pthread_mutexattr_init(&mta);
	pthread_mutexattr_setpshared(&mta, PTHREAD_PROCESS_SHARED);
    
#ifdef SYSV
    printf("System V version\n");
    int id;
    void *memory;
	id = shmget((key_t)5678, sizeof(pthread_mutex_t *), 0600 | IPC_CREAT);
	memory = shmat(id, (void *)0, 0);
#endif
    
#ifdef POSIX
    printf("Posix version\n");
    int id;
    void *memory;
	id = shm_open("/mutex",O_CREAT | O_RDWR,0666);
	ftruncate(id,sizeof(pthread_mutex_t *));
	memory = mmap(NULL,sizeof(pthread_mutex_t *),PROT_WRITE|PROT_READ,MAP_SHARED,id,0);
#endif
    
	

#if defined(POSIX) || defined(SYSV)
    
    mtx = (pthread_mutex_t *)memory;
	pthread_mutex_init(mtx, &mta);	
    
	while(1){
		pthread_mutex_lock(mtx);
        printf("Processo 1 mutex lock\n");
		write(STDOUT_FILENO, shared_memory, sizeof(shared_memory));
		strcpy(shared_memory, msg);
		pthread_mutex_unlock(mtx);
        printf("Processo 1 mutex unlock\n");
		sleep(3);
	}
#endif

#if !defined(POSIX) && !defined(SYSV)
    
    printf("Parent child version\n");
    
    
    
    shared_memory = mmap(NULL, sizeof(msg), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    mtx = (pthread_mutex_t *) mmap(NULL, sizeof(pthread_mutex_t *), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    pthread_mutex_init(mtx, &mta);	
    
    pid_t pid = fork();
    
    switch(pid){
        case 0:
            sprintf(msg,"child");
            while(1){
            pthread_mutex_lock(mtx);
            printf("Child mutex lock\n");
            write(STDOUT_FILENO, shared_memory, sizeof(shared_memory));
            strcpy(shared_memory, msg);
            pthread_mutex_unlock(mtx);
            printf("child mutex unlock\n");
            sleep(3); 
            }
            break;
        default: 
            sprintf(msg,"parent");
            while(1){
            pthread_mutex_lock(mtx);
            printf("Parent mutex lock\n");
            write(STDOUT_FILENO, shared_memory, sizeof(shared_memory));
            strcpy(shared_memory, msg);
            pthread_mutex_unlock(mtx);
            printf("Parent mutex unlock\n");
            sleep(3);
            }
        break;
    }

#endif
    
return 0;
}
