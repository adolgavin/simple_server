#ifndef _SMTP_PROTOCOL_H_
#define _SMTP_PROTOCOL_H_

#include "client.h"

typedef struct {
    int phase;
    int fd; /* file descriptor for temporary mail */
} SMTP_CTX;

void setup_smtp_connection( Handlers *handlers );


#endif /* _SMTP_PROTOCOL_H_ */
