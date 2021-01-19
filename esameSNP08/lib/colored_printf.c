#include "snp.h"

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
    fflush(stdout);
}
