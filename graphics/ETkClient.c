
#include "Dali.h"
#include "ETk.h"

int
ETkClient(const char *servName, char *errmsg, int port)
{
    return DaliPatron((char *) servName, errmsg, port);
}

