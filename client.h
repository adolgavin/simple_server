#ifndef _CLIENT_H_
#define _CLIENT_H_

typedef struct {
    int fd;
    int phase;
    char buffer[10240];
} Client;

#endif /* _CLIENT_H_ */
