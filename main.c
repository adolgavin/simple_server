#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "network.h"
#include "client.h"

static int port = 23; /* default port for smtp */
static char *dir = "./"; /* default dir for saving emails */


static void setOptions(int argc, const char **argv)
{
}


int main(int argc, char **argv)
{
    if (argc != 1)
        setOptions(argc, argv);

    if (!initialize_network(port)) {
        fprintf(stderr, "Can't initialize network subsystem. Exit.\n");
        exit(EXIT_FAILURE);
    }

    int end_loop = 0; /* flag to end of loop */
    while( !end_loop )
    {
        int fd = -1;
        int event = get_next_event(&fd);
        switch(event) {
        case ACCEPT:
            printf("try to accept new client!\n");
            switch( accept_client() )
            {
            case ACCEPTED:
                printf("New connection is accepted!\n");
                /* need send hello */
                break;
            case ACCEPT_AGAIN:
                printf("Try to accept in next time\n");
                break;
            case ERROR:
                fprintf(stderr, "Some error was occured while accepting connection\n");
                break;
            default:
                printf("Unknown accept event. Ignore it\n");
            }
            break;
        case READ:
            break;
        case WRITE:
            break;
        case CLOSE:
            break;
        case ERROR:
            perror("Error");
            printf("Close fd=%d\n", fd);
            close(fd);
            break;
        default:
            fprintf(stderr, "Got unknown event. Ignore it\n");
            break;
        }
    }

    return 0;
}
