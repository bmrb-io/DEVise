#ifndef _DBASE_H
#define _DBASE_H

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ndbm.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sockaddr_in ConnectInfo; 

struct DbEntry {
	ConnectInfo ServerAddr;
	int FileDesc;
};
typedef struct DbEntry DbEntry;

#define SIZEOFDBENTRY sizeof(DbEntry)

struct ServerNode {
	DbEntry ServerAddr;
	struct ServerNode *next;
};

typedef struct ServerNode ServerNode;
typedef ServerNode * ServerList;

extern DBM *GroupDB;
extern char GroupDBFile[];

void GroupDBInit(void);
void GroupDBClose(void);
ServerList FetchServers(char *);
ServerList FetchMaster(char *);
int AddGroup(char *, DbEntry);
int AppendServer(char *, DbEntry);
int DeleteServer(char *, DbEntry);
int DeleteGroup(char *, DbEntry);

char *Pack(ServerList, int *);
ServerList UnPack(datum);
ServerList Delete(ServerList, DbEntry);
void FreeList(ServerList);

#ifdef __cplusplus
}
#endif

#endif /* !_DBASE_H */
