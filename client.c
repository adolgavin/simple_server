#include "client.h"

#define MAX_CLIENTS 10 /* similar to EPOLL_MAX_EVENTS */

static Client clients[MAX_CLIENTS];

void initialize_clients()
{
    for( int i = 0; i < MAX_CLIENTS; ++i )
        initialize_client(clients + i);
}

void initialize_client(int number)
{
    memset( clients + number, 0, sizeof(Client) );
    clients[number].fd = -1;
}

Client* add_new_client( int fd )
{
    int i = 0;
    for( ; i < MAX_CLIENTS; ++i )
    {
        if (clients[i].fd == -1)
            break;
    }
    if ( i == MAX_CLIENTS )
        return NULL;

    clients[i].fd = fd;
    return clients + i;
}

Client* get_client_by_fd( int fd )
{
    for( int i = 0; i < MAX_CLIENTS; ++i)
        if( clients[i].fd == fd )
            return clients + i;

    return NULL;
}

void destroy_client(Client *c)
{
    if( c->ctx && c->handlers.ctx_free )
        c->handlers.ctx_free( c->ctx );
    initialize_client(c);
}
