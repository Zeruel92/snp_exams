#include "common.h"
#include "snp.h"
#include <sys/mman.h>
#include <sys/stat.h>        
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    
    int fd;
    struct nodo *p;
    int choise;
    int cont = 1;
    
    fd = shm_open("/esame10",O_CREAT|O_RDWR,0600);
    ftruncate(fd, sizeof(struct node *)*MACLIMIT);
    
    p =  mmap(NULL, sizeof(struct node *)*MACLIMIT, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            
    while(cont == 1){
    printf("Cosa vuoi fare?\n");
    printf("1 per contare\n");
    printf("2 per inserire\n");
    printf("3 per fare pop\n");
    scanf("%d",&choise);
    
    switch(choise){
        case 1:
            printf("Il numero di nodi è %d\n",count(p));
            break;
        case 2:
            printf("Inserisci il valore da aggiungere\n");
            scanf("%d",&choise);
            insert(p,choise,fd);
            break;
        case 3:
            printf("Pop elemento %d\n",pop(p));
            break;
        default:
            cont = 0;
            shm_unlink("/esame10");
            break;
    }
    }
    exit(0);
}

