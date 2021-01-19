#ifndef _COMMON_H
#define _COMMON_H 

#define PORT "49999"

struct player_pos{
    int x;
    int y;
};

struct player{
    struct player_pos coord;
    int id;
    struct player *next;
};

#endif
