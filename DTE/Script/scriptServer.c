#ifdef DTE_AS_SERVER
	#include <stdlib.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <string>
#endif

#include <string.h>
#include <iostream.h>
#include <memory.h>
#include<assert.h>
#include<math.h>

#ifdef DTE_AS_SERVER
	#include "common.h"
#endif

#include "RTreeCommon.h"
#include "Engine.h"
#include "types.h"
#include "exception.h"
#include "ncsa.h"
#include "SockStream.h"

#ifdef SERVER_DEBUG
        #define DBSERVER(a) {cout << __FILE__ << ':' << __LINE__ << ' ' << a << endl;}
#else
        #define DBSERVER(a) {}
#endif

const int DETAIL = 1;

#ifndef DTE_AS_SERVER
	#define out cout
#endif

int main(int argc, char** argv)
{
  
  char* queryString = NULL;

#ifdef DTE_AS_SERVER
  int request_sock, new_sock;
  struct sockaddr_in server, remote;
  fd_set rmask;
  //static struct timeval timeout = { 1, 0 };
  int addrlen, nfound, bytesread, ugly=0, cl;
  char *inbuf, *outbuf;
  
  if ((request_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
      cerr << "*** socket ***";
      exit(1);
    }
  
   memset((void *) &server, 0, sizeof (server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = INADDR_ANY;
   server.sin_port = htons((unsigned short)DTEserverport);

   if (bind(request_sock, (struct sockaddr *)&server, sizeof server) < 0)
     {
       cerr << "*** bind ***";
       exit(1);
     }

   if (listen(request_sock, SOMAXCONN) < 0)
     {
       cerr << "*** listen ***";
       exit(1);
     }

   for (;;)
     {
       addrlen = sizeof(remote);
       DBSERVER("--- BEFORE ACCEPT\n");
       new_sock = accept(request_sock, (struct sockaddr *)&remote, &addrlen);
       if (new_sock < 0)
         {
           cerr << "*** accept ***";
           exit(1);
         }
       DBSERVER("--- AFTER ACCEPT\n");

       bytesread = read(new_sock, &cl, sizeof (int));
	DBSERVER(cl)
	FILE* inputStream = fdopen(new_sock, "r");
#else

     printf("Content-type: text/plain%c%c",10,10);
	FILE* inputStream = stdin;
	char* cont_len = getenv("CONTENT_LENGTH");
	int cl = 0;
	if(cont_len){
		cl = atoi(cont_len);
	}

#endif //ifdef DTE_AS_SERVER

	int entryCnt = 0;
	if(cl > 0){
		entryCnt = post_query(cl, inputStream);
	}

#ifdef DTE_AS_SERVER
       Cor_sockbuf *accepted;
       accepted=new Cor_sockbuf((int) new_sock);
       ostream out(accepted);
#endif

       if(entryCnt == 0)
	 {
	#ifdef DTE_AS_SERVER
	  out << "1 entryCnt=0\n";
	  delete accepted;
               continue;
	#else
		return (0);
	#endif
	 }
       bool execute = false;
       bool profile = false;
	  bool typecheck = false;
       char* shipping = NULL;
       char* table = NULL;
        #ifdef DTE_AS_SERVER 
	ugly=0;
	#endif
       for(int i = 0; i < entryCnt; i++)
	 {
	   string option = entries[i].name;
	   if(option == "execute")
	     {
	       execute = true;
	     }
	   else if(option == "typecheck")
	     {
	       typecheck = true;
	     }
	   else if(option == "profile")
	     {
	       profile = true;
	     }
	   else if(option == "query")
	     {
	       queryString = entries[i].val;
	       // int queryLength = strlen(queryString);
	       // queryString[queryLength - 1] = ';';
	     }
	   else if(option == "shipping"){
	     shipping = entries[i].val;
	   }
	   else if(option == "table"){
	     table = entries[i].val;
	   }
	   else
	     {
		#ifdef DTE_AS_SERVER
	       out << "1 unknown option: " << option << endl;
	       ugly=1;
	       delete accepted;		
	       continue;
		#else
			out << "1 unknown option: " << option << endl;
                        return 0;
		#endif
	   }
	 }

	#ifdef DTE_AS_SERVER
       if (ugly)
	 continue;
	#endif
       if(execute || typecheck)
	 {
	 }	  
       else if(profile)
	 {
	   // Change the tables in from clause to the samples.
	     // do the post processing.
	     #ifdef DTE_AS_SERVER
	   out << "1 profile not implemented\n";
	   delete accepted;
	   continue;
		#else
		 out << "1 profile not implemented\n";
                return 0;
		#endif
	 }
       else
	 {
	   if(!shipping || !table)
	     {
		out << "1 shipping table not specified\n";
	     }
	   // create and load local table;
		#ifdef DTE_AS_SERVER
	   out << "1 shipping not implemented\n";
	   delete accepted;
	   continue;
		#else
		out << "1 shipping not implemented\n";
                return 0;
		#endif
	 }

        string query(queryString);
  
       Engine engine(query);
	DBSERVER("before TRY")
	#ifdef DTE_AS_SERVER
	engine.optimize();
	if (currExcept){
		out << "1 Parse error in: ";
		out << queryString;
		delete accepted;
		currExcept=0;
		continue;
		}
	DBSERVER("after TRY")
	#else
	engine.optimize();
	string err = "In query: " + query;
	CHECK(err, 0);

	#endif
     int numFlds = engine.getNumFlds();
	#ifdef DTE_AS_SERVER
	if (!(numFlds>0)) 
	  {
		out << "1 numFlds<=0\n";
		delete accepted;
		continue;
	  }
	#else
	assert(numFlds > 0);
	#endif

     const TypeID* types = engine.getTypeIDs();
     const string* attrs = engine.getAttributeNames();

	if(typecheck){
		ISchema schema(numFlds, types, attrs);	
		out << "0 OK\n";
		out << schema << endl;
		return 0;
	}

     WritePtr* writePtrs = newWritePtrs(types, numFlds);

	engine.initialize();
	err = "In query: " + query;
	CHECK(err, 0);
	
     const Tuple* tup;

	out << "0 OK\n";

     while((tup = engine.getNext())){
		for(int i = 0; i < numFlds; i++){
			writePtrs[i](out, tup[i]);
			out << '\t';
          }
          out << endl;
     }
	engine.finalize();

	#ifdef DTE_AS_SERVER
	delete accepted;
	#endif
}
