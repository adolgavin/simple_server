#ifndef _NETWORK_H_
#define _NETWORK_H_

enum NETWORK_EVENTS {
    ACCEPT,
    READ,
    WRITE,
    CLOSE,
    ERROR
};

enum ACCEPT_EVENTS {
    ACCEPTED,
    ACCEPT_AGAIN,
    ACCEPT_ERROR
};

int initialize_network(int port);

#endif /* _NETWORK_H_ */
