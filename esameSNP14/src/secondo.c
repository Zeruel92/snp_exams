#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#ifdef SYSV
#include <sys/shm.h>
#endif

#ifdef POSIX
#include <sys/mman.h>
#endif

#include <pthread.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	
	void *shared_memory;
	char	msg[10] = "proc2";
	
#ifdef SYSV
    int shmid;
	shmid = shmget((key_t)9999, sizeof(msg), 0666 | IPC_CREAT);
	shared_memory = shmat(shmid, (void *)0, 0);
#endif
    
#ifdef POSIX
    int shmid;
	shmid = shm_open("/sharedmem",O_CREAT |O_RDWR ,0666);
	shared_memory = mmap(NULL,sizeof(msg),PROT_WRITE|PROT_READ,MAP_SHARED,shmid,0);
#endif
	
	void *memory;
	pthread_mutex_t	*mtx;
    
#ifdef SYSV
    int id;
	id = shmget((key_t)5678, sizeof(pthread_mutex_t *), 0600 | IPC_CREAT);
	memory = shmat(id, (void *)0, 0);
#endif 
    
#ifdef POSIX
    int id;
	id = shm_open("/mutex",O_CREAT | O_RDWR,0666);
	memory = mmap(NULL,sizeof(pthread_mutex_t *),PROT_WRITE|PROT_READ,MAP_SHARED,id,0);
#endif
    
	mtx = (pthread_mutex_t *)memory;
		
	while(1){
		pthread_mutex_lock(mtx);
        printf("Processo 2 Mutex lock\n");
		write(STDOUT_FILENO, shared_memory, sizeof(shared_memory));
		strcpy(shared_memory, msg);
		pthread_mutex_unlock(mtx);
        printf("Processo 2 mutex unlock\n");
		sleep(3);
	}
    
return 0;
}
