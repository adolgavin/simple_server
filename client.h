#ifndef _CLIENT_H_
#define _CLIENT_H_

typedef struct {
    void (*read_handler)();
    void (*write_handler)();
    void (*ctx_free)();
} Handlers;

typedef struct {
    int fd; /* file descriptor network socket */
    char buffer[10240];
    void (*ntwk_read)();
    void (*ntwk_write)();
    Handlers handlers;
    void *ctx;
} Client;

void initialize_clients();
void initialize_client(Client *);
Client* add_new_client(int fd);
Client* get_client_by_fd(int fd);
void destroy_client(Client *);
#endif /* _CLIENT_H_ */
