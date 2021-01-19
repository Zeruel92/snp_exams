#include "common.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int pop(struct nodo *p){
    
    struct nodo *current;
    struct nodo *prev;
    
    for(current = p; current->next!=0; current = p+(sizeof(current)*current->next)){
        prev = current;
    }
    
    prev->next = 0;
    return current->n;
}

void insert(struct nodo *p, int n, int fd){
    int c;
    struct nodo *current;
    struct nodo *new;
    c = count(p);
    
#ifdef MACOS
        
    if(c < MACLIMIT){

    for(current = p; current->next != 0; current = p+(sizeof(current)*current->next)){
    }
        new = p+(sizeof(struct nodo *)*(c+1));
        current->next = c+1;
        new->n = n;
        new->next = 0;
        
    }
    
#endif
#ifdef LINUX

    if(c >= MACLIMIT){
        ftruncate(fd,(sizeof(struct nodo *)*(c+1)));
        mremap(p,sizeof(p)*c,(sizeof(struct nodo *)*(c+1)),0,NULL);
    }

    for(current = p; current->next != 0; current = p+(sizeof(current)*current->next)){
        prev = current;
    }
        
        new = p+(sizeof(struct nodo *)*(c+1));
        current->next = c+1;
        new->n = n;
        new->next = 0;
 
#endif
}

int count(struct nodo *p){
    int c = 0;
    struct nodo *current;
    for(current = p; current->next != 0; current= p+(sizeof(current)*current->next)){
        c++;
    }
    return c;
}
