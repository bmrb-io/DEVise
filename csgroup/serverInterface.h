#ifndef _SERVER_INTERFACE
#define _SERVER_INTERFACE

#include "dbase.h"

#ifdef __cplusplus
extern "C" {
#endif

void RPCInit(char *);
void RPCClose(void);
int Register(char *, ConnectInfo, int);
int DeRegister(char *, ConnectInfo, int);
int GroupSend(char *, char *msg, int msgsize);

#ifdef __cplusplus
}
#endif

#endif /* !_SERVER_INTERFACE */
