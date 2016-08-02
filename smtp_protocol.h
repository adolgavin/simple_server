#ifndef _SMTP_PROTOCOL_H_
#define _SMTP_PROTOCOL_H_

#include "client.h"

typedef struct {
    int phase;
};

void setup_smtp_connection( Handlers *handlers );


#endif /* _SMTP_PROTOCOL_H_ */
