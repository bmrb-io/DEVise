#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>

#include "./h/network.h"

#define  MAXQUERYLENGTH 80
#define  SMART_SOCKET   9325

struct smart_msg_buf
{
 long mtype;
 int  recv_portnum;
};

int main (int argc, char **argv)
{
  int sock,newsock;

  sock=CreateServerTCPSocket(SMART_SOCKET);

  while(1)
  {
      printf("Waiting for connection \n");
      IsMessageAvailableOnSocket(sock,0);

      printf("Accepting  connection \n");
      newsock = AcceptConnectOnTCPSocket(sock);
      printf("Accepted   connection \n");

           if (fork()==0) 
           {
             char *tmp1, *tmp2, *tmp3;

              close(sock);
              printf("\n forking the smart process \n");
              tmp1 = (char *)malloc(50);
              tmp2 = (char *)malloc(10);
              tmp3 = (char *)malloc(80);
              strcpy(tmp1,"/p/devise/sanj/smart/smart.11.0/src/bin/smart");
              argv[0]  = tmp1;
              strcpy(tmp2,"inter");
              argv[1]  = tmp2;
              strcpy(tmp3,"/p/devise/sanj/smart/smart.11.0/src/test_adi/indexed/spec");
              argv[2]  = tmp3;
              fflush(stdout);
              execvp(argv[0],argv);
              exit(0);

           }
           else  /*
                    get a portnumber from the smart-process and 
                    return that to the client
                   */
           {
              key_t key;
              int msqid;
              char *msg_to_client;

              struct smart_msg_buf pmb;
              key = ftok("/tmp/smart.portnum", 'b');
              msqid = msgget(key, 0666 | IPC_CREAT);
              /* get him off the queue! */       
              msgrcv(msqid, &pmb, sizeof(pmb), 1, 0);
              msgctl(msqid, IPC_RMID, NULL);
              printf("the portnumber received = %d\n",pmb.recv_portnum);
              /* send this portnumber to the client */
              
              msg_to_client = (char *)malloc(12);
              bzero(msg_to_client,12);
              sprintf(msg_to_client,"%d",pmb.recv_portnum);

              printf(" server on port %s \n",msg_to_client);
              WriteToTCPSocket(newsock,msg_to_client,12); 
              printf("sending port %d to client \n",pmb.recv_portnum);
              free(msg_to_client);
              close(newsock);

           }
  }
}




