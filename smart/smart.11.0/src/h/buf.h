#ifndef BUFH
#define BUFH
/*        $Header$*/

/* structure used for passing around text (buf) which possibly includes
   NULLs.  see buf_util.c for add_buf(). */
typedef struct {
    int size;                      
    int end;
    char *buf;
} SM_BUF;

#endif /* BUFH */
