#include "network.h"
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define EPOLL_MAX_EVENTS 10

static int epollfd = -1;
static struct epoll_event events[EPOLL_MAX_EVENTS];
static int sockfd = -1;

static int set_non_blocking(int fd)
{
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return 0;
    }

    flags |= O_NONBLOCK;
    if (fcntl( sockfd, F_SETFL, &flags ) == -1) {
        perror("fcntl");
        return 0;
    }
    return 1;
}

int initialize_network(int port)
{
    if (epollfd != -1  && sockfd != -1) {
        printf("network subsystem already inited");
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 0;
    }

    struct sockaddr_in s;
    memset( &s, 0, sizeof(s) );
    s.sin_family = AF_INET;
    s.sin_addr.s_addr = INADDR_ANY;
    s.sin_port = htons(port);
    if (bind( sockfd, (struct sockaddr*) &s, sizeof(s)) == -1) {
        perror("bind");
        return 0;
    }

    if( !set_non_blocking(sockfd) )
        return 0;

    if( listen( sockfd, SOMAXCONN ) == -1 ) {
        perror("listen");
        return 0;
    }

    /* add our socket to epoll */
    epollfd = epoll_create(EPOLL_MAX_EVENTS);
    if (epollfd == -1) {
        perror("epoll_create");
        return 0;
    }

    struct epoll_event ev;
    memset( &ev, 0, sizeof(ev) );
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = sockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
        perror("epoll_ctl");
        return 0;
    }

    return 1;
}

int get_next_event(int *fd)
{
    int nfds = epoll_wait(epollfd, events, EPOLL_MAX_EVENTS, -1);
    if (nfds == -1) {
        perror("epoll_wait");
        return ERROR;
    }

    for( int i = 0; i < nfds; ++i ) {
        if( (events[i].events & EPOLLERR) ||
            (events[i].events & EPOLLHUP) ||
            (!(events[i].events & EPOLLIN ))
          ) {
            *fd = events[i].data.fd;
            return ERROR;
        }

        if( sockfd == events[i].data.fd ) {
            /* accept event */
            return ACCEPT;
        }
    }
}

int accept_client()
{
    struct sockaddr addr_in;
    socklen_t len = sizeof(addr_in);
    int infd = -1;
    struct epoll_event ev;

    infd = accept(sockfd, &addr_in, &len);
    if (infd == -1) {
        if( errno == EAGAIN ||
            errno == EWOULDBLOCK
          ) {
            return ACCEPT_AGAIN;
        } else {
            perror("accept");
            return ACCEPT_ERROR;
        }
    }

    if (!set_non_blocking( infd ))
        return ACCEPT_ERROR;

    ev.data.fd = infd;
    ev.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, infd, &ev) == -1) {
        perror("epoll_ctl");
        return ACCEPT_ERROR;
    }

    return ACCEPTED;
}

