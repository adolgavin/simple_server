#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sts/types.h>
#include <errno.h>
#include <time.h>

#define STRSZ(__x__) __x__, (sizeof(__x__)-1)

#define HELO_STR "HELO"
#define RCPT_TO_STR  "RCPT TO"
#define MAIL_FROM_STR  "MAIL FROM"
#define DATA_STR "DATA"
#define QUIT_STR "QUIT"
#define INVALID_STR NULL

#define END_OF_DATA "\r\n.\r\n"

enum Phases {
    HELO,
    RCPT_TO,
    MAIL_FROM,
    DATA,
    QUIT,
    INVALID
};

typedef struct {
    const char *str;
    int (*func)();
} phase_func;

static const phase_func phase_func_tbl[] = {
    { HELO_STR, helo_handler },
    { RCPT_TO_STR, rcpt_to_handler },
    { MAIL_FROM_STR, mail_from_handler },
    { DATA_STR, data_handler },
    { QUIT_STR, quit_handler },
    { NULL, invalid_handler }
};

static const char connect_words[] = "220 my.very.cool.smtp.server is glad to see you!\r\n";

static void smtp_ctx_free( void *_ctx );
static int rcpt_to_handler( SMTP_CTX *ctx, const char *buf, int buf_len );
static int data_handler( SMTP_CTX *ctx, const char *buf, int buf_len );


void setup_smtp_connection( Handlers *handlers )
{
    handlers.read_handler = smtp_read_handler;
    handlers.write_handler = smtp_write_handler;
    handlers.ctx_free = smtp_ctx_free;
}


int smtp_read_handler( Client *c, const char *buf, size_t len )
{
    if( !strstr( buf, "\r\n\r\n" ) || !strstr( buf, "\n\n" ) )
        return 0;

    for( int i = 0; phase_func_tbl[i].str != NULL; ++i )
    {
        if( !strcasecmp( buf, phase_func_tbl[i].str ) )
            break;
    }

    if( !phase_func_tbl[i].str )
    {
        fprintf(stderr, "Unknown SMTP command \"%s\"\n", buf );
        return -1;
    }

    phase_func_tbl[i].func( c->ctx.smtp, buf + strlen(phase_func_tbl[i].str), len - strlen(phase_func_tbl[i].str) );


}

void send_hello_words( SMTP_CTX *ctx )
{
}

int smtp_write_handler( Client *c )
{

}

/* returns file descriptor to the temp file.
   Or -1 if error.
 */
static int rcpt_to_handler( SMTP_CTX *ctx, const char *buf, int buf_len )
{/*{{{*/
    char user[256] = { 0 };
    int len = buf_len;
    /* fill buf to user name */
    while( !isalnum( *buf ) && len ) { ++buf; --len; }

    const char *end_buf = buf;
    while( ( isalnum( *end_buf ) || *end_buf == '@' || *end_buf == '.' ) && len ) { ++end_buf; --len; }

    if( end_buf - buf > sizeof(user)-1)
        strcpy( user, buf, sizeof(user)-1 );
    else
        strcpy( user, buf, end_buf - buf );
    
    char full_path[1024];
    sprintf(full_path, "./%s", user);

    if( mkdir( full_path, S_IRWXU ) == -1 && errno != EEXIST )
    {
        perror("rcpt_to_handler");
        send_answer( "550 Unknown user\r\n" );
        return -1;
    }

    sprintf( full_path + strlen(full_path), "/%010dXXXXXX", time() );

    ctx->fd = mkstemp( full_path );
    if( ctx->fd == -1 )
    {
        perror("rcpt_to_handler");
        send_answer( "550 Unknown user\r\n" );
        return -1;
    }

    return buf_len;
}/*}}}*/

static int data_handler( SMTP_CTX *ctx, const char *buf, int buf_len )
{
    if( ctx->fd == -1 )
    {
        fprintf(stderr, "temporary file with email is closed!\n" );
        return -1;
    }
    if( write( ctx->fd, buf, buf_len ) != buf_len )
    {
        /* TODO: something bad */
    }

    if( !strcmp( buf + buf_len - STRSZ(END_OF_DATA), END_OF_DATA ) )
    {
        printf( "end of data. close file\n" );
        close(ctx->fd);
        ctx->fd = -1;
    }
    return buf_len;
}

void send_answer( SMTP_CTX *ctx, const char *msg )
{
}


static void smtp_ctx_free( void *_ctx )
{
    SMTP_CTX *ctx = (SMTP_CTX *) _ctx;
    if( ctx->fd != -1 )
        close( ctx->fd );
    free( ctx );
}
