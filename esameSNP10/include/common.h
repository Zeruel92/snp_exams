#ifndef _COMMON_H
#define _COMMON_H
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h>  

#define MACLIMIT 10

struct nodo{
    int n;
    /*struct nodo *next;*/
    int next;
};

int pop(struct nodo *p);
void insert(struct nodo *p,int n,int fd);
int count(struct nodo *p);

#endif
