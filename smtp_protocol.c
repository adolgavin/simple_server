#include <stdio.h>
#include <stdlib.h>

#define STRSZ(__x__) __x__, (sizeof(__x__)-1)

#define HELO_STR "HELO\r\n"
#define RCPT_TO  "RCPT TO"

enum Phases {
    HELO,
    RCPT_TO,
};

void setup_smtp_connection( Handlers *handlers )
{
    handlers.read_handler = smtp_read_handler;
    handlers.write_handler = smtp_write_handler;
}


int smtp_read_handler( Client *c, const char *buf, size_t len )
{
    int
}

int smtp_write_handler( Client *c )
{

}
