/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2008
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
  Revision 1.11.4.1  2014/01/17 21:45:59  wenger
  Fixed a bunch of possible buffer overflows.

  Revision 1.11  2008/10/13 19:45:34  wenger
  More const-ifying, especially Control- and csgroup-related.

  Revision 1.10  2005/12/06 20:01:10  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.9.22.2  2005/09/28 17:14:21  wenger
  Fixed a bunch of possible buffer overflows (sprintfs and
  strcats) in DeviseCommand.C and Dispatcher.c; changed a bunch
  of fprintfs() to reportErr*() so the messages go into the
  debug log; various const-ifying of function arguments.

  Revision 1.9.22.1  2003/12/19 18:07:06  wenger
  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.

  Revision 1.9.40.1  2003/12/17 00:17:41  wenger
  Merged gcc3_br_1 thru gcc3_br_2 to redhat9_br (just fixed conflicts,
  didn't actually get it to work).

  Revision 1.9.38.1  2003/12/16 16:07:56  wenger
  Got DEVise to compile with gcc 3.2.3 (with lots of deprecated-header
  warnings).  It runs on RedHat 7.2, but not on Solaris 2.8 (some kind
  of dynamic library problem).

  Revision 1.9  1998/08/21 22:16:03  wenger
  Got DEVise 1.5.4 to compile on SPARC/SunOS (sundance) -- to make statically-
  linked DEVise for distribution.

  Revision 1.8  1998/08/17 21:03:38  wenger
  Changed deviseb and devisec to work properly with current devised
  (clients don't expect to be sent slot number at startup).

  Revision 1.7  1998/07/29 14:19:40  wenger
  Mods to compile DEVise on Alpha/OSF again (partially successful); mods to
  allow static linking on Linux.

  Revision 1.6  1998/05/02 08:38:56  taodb
  Added command logging and playing support
  Added communication support for JAVA Screen

  Revision 1.5  1998/03/11 18:25:07  wenger
  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
  reducing include dependencies between csgroup code and the rest of
  the code, and within the csgroup code.  (Note: running collaboration
  doesn't work yet.)

  Revision 1.4  1998/02/27 20:47:46  wenger
  More SGI compile fixes.

  Revision 1.3  1998/02/26 18:54:06  wenger
  Got everything to compile on haha -- still have a link problem, though.

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
#if !defined(SUN)
#include <netdb.h>
#endif

//#define USE_START_PROTOCOL // Whether to use startup protocol as defined
							 // by API.txt (server sends slot number to
							 // client, etc.).

#if !defined(LINUX) && !defined(OSF) && !defined(SOLARIS)
extern "C" {
	void bzero(void *s, size_t n);
	int gethostname(char *name, int namelen);
	struct hostent *gethostbyname(const char *name);   
};
#endif

#define SLOTNUMSIZE			6			// size of the maximum slot number
#define IMG_OOB				1			// size of pseudo out-of-band msg
										// for stopping sending image
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
	static const char* DefaultStr;
	static const char* ControlSeperator; 
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
		ServerServerProt( int argc, const char* const * argv);
		ServerServerProt(int msize, char* msg);

		int setServerName(const char* sname);
		int setClientName(const char* cname);
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
		int		buflen;
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
