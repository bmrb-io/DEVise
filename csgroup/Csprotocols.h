/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of Client (and CompDate) classes.
 */

/*
  $Id$

  $Log$
  Revision 1.2  1998/02/12 17:14:33  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.3  1998/02/02 08:23:48  liping
  Added CVS header

*/

#ifndef _Csprotocols_H
#define _Csprotocols_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#if !defined(SGI)
#include <sys/varargs.h>
#else
#include <varargs.h>
#endif

extern "C" {
	void bzero(void *s, size_t n);
	int gethostname(char *name, int namelen);
	struct hostent *gethostbyname(const char *name);   
};
// Client->Server group control
#define CS_Creat_Req       "creat_req"  // blocking & timeout to fail
#define CS_Join_Req        "join_req"   // blocking & timeout to fail
#define CS_Lead_Req        "lead_req"   // blcoking & timeout to fail
#define CS_Relinquish_Not  "listen_not" // blocking and timeout to fail
#define CS_Leave_Req       "leave_req"  // blocking & timeout to fail
#define CS_Init_Req        "init_req"   // first message
#define CS_Query_Req        "query_req"   // first message
											
// Server->Client group control
#define SC_Relinquish_Req  "relinq_req" // non-blocking & once
#define SC_Grabbed_Not     "grabbed_not" // non-blocking & once

// Server->Collaborator 
#define SERVERTAG		"IS_SERVER"		// passwd for control channel

// Server<->Server 
#define	SS_SSC_Cmd			"ssc"		// server-server-client commands
#define	SS_CSS_Cmd			"css"		// client-server-server commands

// implement server-server protocols
class ServerServerProt					
{
	static char* DefaultStr;
	static char* ControlSeperator; 
	private:
		char	** newargv;				// message in argv formats	
		int		newargc;				// number of arguments

		char	*recBuffer;				// prepared message
		int		msgsize;				// size of the message
		bool	dynamicStructure;		

		enum { 
			SERVER_NAME = 0,
			CLIENT_NAME,
			CONTROL,
			SS_Header_Offset
		};
	public:
		enum {
			SSC_CMD = 0x01,
			CSS_CMD = 0x02
		};
		ServerServerProt( int argc, char** argv);
		ServerServerProt(int msize, char* msg);

		int setServerName(char* sname);
		int setClientName(char* cname);
		int setControl(int argc, ...);

		char* getServerName();
		char* getClientName();
		int	  getControl();

		int	getArguments(int& argc, char**&argv);
		int getMsg(char** recBufferp);
		~ServerServerProt();
		static int getHeaderLength()
		{
			return SS_Header_Offset;
		}
};

// implementing 
class TokenList
{
	private:
		char*	listbuf;
		int		listargs;
	public:	
		TokenList (char* list);
		TokenList(int args, char** argv);
		TokenList(int args, ...);
		void ProcessList(int args, char** argv);
		~TokenList();
		char* getList();
		char* toTclList(char*& buf);
		int	 appendToken(char* token);

	private:
		int setNumber(int num);
		int getNumber();
};
#endif
