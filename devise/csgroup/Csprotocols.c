/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2013
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
  Revision 1.10.4.1  2014/01/17 21:45:59  wenger
  Fixed a bunch of possible buffer overflows.

  Revision 1.10  2008/10/13 19:45:34  wenger
  More const-ifying, especially Control- and csgroup-related.

  Revision 1.9  2005/12/06 20:01:10  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.8.8.2  2005/09/28 17:14:19  wenger
  Fixed a bunch of possible buffer overflows (sprintfs and
  strcats) in DeviseCommand.C and Dispatcher.c; changed a bunch
  of fprintfs() to reportErr*() so the messages go into the
  debug log; various const-ifying of function arguments.

  Revision 1.8.8.1  2005/09/06 21:20:00  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.8  2001/10/12 18:30:32  wenger
  Fixed problems with network header reading/writing code (old code
  assumed that the NetworkHeader struct would never contain any extra
  space for alignment, other problems).

  Revision 1.7  1999/01/18 18:11:13  beyer
  fixed compile errors and warnings for egcs version 1.1.1

  Revision 1.6  1998/07/29 14:19:40  wenger
  Mods to compile DEVise on Alpha/OSF again (partially successful); mods to
  allow static linking on Linux.

  Revision 1.5  1998/03/11 18:25:07  wenger
  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
  reducing include dependencies between csgroup code and the rest of
  the code, and within the csgroup code.  (Note: running collaboration
  doesn't work yet.)

  Revision 1.4  1998/02/26 20:35:09  taodb
  Removed ParaseAPI() interface, and added CommandObject interface

  Revision 1.3  1998/02/26 18:54:06  wenger
  Got everything to compile on haha -- still have a link problem, though.

  Revision 1.2  1998/02/12 17:14:32  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.3  1998/02/02 08:23:48  liping
  Added CVS header

*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Csprotocols.h"
#include "ServerAPI.h"
#include "devise_varargs.h"

const char* ServerServerProt::DefaultStr = "NULL";
const char* ServerServerProt::ControlSeperator = "?+?";
ServerServerProt::~ServerServerProt()
{
	int		i;
	if (dynamicStructure)
	{
		for (i=0; i < newargc; ++i)
		{
			delete newargv[i];
		}
		delete newargv;
	}
}

ServerServerProt::ServerServerProt(int argc, const char* const * argv)
{
	// fill in the defaults
	int		i;
	dynamicStructure = true;
	newargv = new char*[argc+ getHeaderLength()];
	for (i=0; i < getHeaderLength(); ++i)
	{
		newargv[i] = strdup(DefaultStr);
	}

	// fill in the original commands
	for (i=0; i< argc; ++i)
	{
		newargv[i+getHeaderLength()] = strdup(argv[i]);
	}

	newargc = argc + getHeaderLength();
	msgsize = NetworkPrepareMsg(API_CMD, 0, newargc,newargv, &recBuffer);
}

ServerServerProt::ServerServerProt(int msize, char* msg)
{
	int tsize;

	dynamicStructure = false;
	msgsize = msize;
	recBuffer = msg;

	NetworkAnalyzeHeader(msg, newargc, tsize);
	NetworkParse((msg + NetworkHeaderSize), newargc, newargv);
}
int
ServerServerProt::setServerName(const char* sname)
{
	if (dynamicStructure)
	{
		if (newargv[SERVER_NAME]!=DefaultStr) {
			delete newargv[SERVER_NAME];
		}
		
		newargv[SERVER_NAME] = strdup(sname);
		msgsize = NetworkPrepareMsg(API_CMD, 0, newargc,newargv, &recBuffer);
		return msgsize;
	}
	else
	{
		return -1;
	}
}

int
ServerServerProt::setClientName(const char* cname)
{
	if (dynamicStructure)
	{
		if (newargv[CLIENT_NAME]!=DefaultStr)
			delete newargv[CLIENT_NAME];
		newargv[CLIENT_NAME] = strdup(cname);
		msgsize = NetworkPrepareMsg(API_CMD, 0, newargc,newargv, &recBuffer);
		return msgsize;
	}
	else 
	{
		return -1;
	}
}

int
ServerServerProt::setControl(int args, ...)
{
	int		i;
	char	buf[1000];

	if (dynamicStructure)
	{
		buf[0] = 0;
		va_list	pvar;
		va_start(pvar, args);
		for (i=0; i< args; ++i)
		{
			char	*cmd;
			cmd = va_arg(pvar, char*);
            int formatted = snprintf(buf, sizeof(buf), "%s%s", cmd,
			  ControlSeperator);
            assert(formatted < (int)sizeof(buf));//TEMP
/*TEMP
	        if (checkAndTermBuf2(buf, formatted) != StatusOk) {
	            reportErrNosys("Buffer overflow!");
		        exit(1);
	        }
TEMP*/
		}
		va_end(pvar);
	
		if (newargv[CONTROL]!=DefaultStr)
		{
			delete newargv[CONTROL];
		}
		newargv[CONTROL] = strdup(buf);
	
		msgsize = NetworkPrepareMsg(API_CMD, 0, newargc,newargv, &recBuffer);
		return msgsize;
	}
	else
	{
		return -1;
	}
}


int
ServerServerProt::getArguments(int& argc, char**&argv)
{
	argc = newargc - getHeaderLength();
	if (argc <0)
	{
		fprintf(stderr, "too few parameters in the server-server message\n");
		return -1;		
	}
	argv = &newargv[getHeaderLength()];
	return argc;
}

char* 
ServerServerProt::getServerName()
{
	char*	sname;
	sname = newargv[SERVER_NAME];
	return sname;
}
char* 
ServerServerProt::getClientName()
{
	char*	cname;
	cname = newargv[CLIENT_NAME];
	return cname;
}

int   
ServerServerProt::getControl()
{
	char*	buf;
	int		i = 0;
	int		prev = 0;
	int		retval = 0;


	buf = newargv[CONTROL];
	while (buf[i])
	{
		if (!strcmp(buf+i, ControlSeperator))
		{
			char	ch;

			ch = buf[i];
			buf[i] = 0;
			if (!strcmp(buf+prev, SS_SSC_Cmd))
				retval |= SSC_CMD;
			else
			if (!strcmp(buf+prev, SS_CSS_Cmd))
				retval |= CSS_CMD;

			buf[i] = ch;
			i += strlen(ControlSeperator);
			prev = i;
		}
		else
		{
			i++;
		}
	}
	return retval;
}

int
ServerServerProt::getMsg(char** recBufferp)
{
	*recBufferp = recBuffer;
	return msgsize;
}


TokenList::TokenList(char* list)
{
	if (list != NULL)
	{
		buflen = MAX(strlen(list)+1, 4);
		listbuf = new char[buflen];
		strncpy(listbuf, list, buflen);
		listbuf[buflen-1] = '\0'; // ensure termination
		getNumber();
	}
}
int 
TokenList::setNumber(int num)
{
	char ch;

	ch = listbuf[3];
	int formatted = snprintf(listbuf, buflen, "%-3d", num);
    assert(formatted < buflen);//TEMP
/*TEMP
	if (checkAndTermBuf(listbuf, buflen, formatted) != StatusOk) {
	    reportErrNosys("Buffer overflow!");
		exit(1);
	}
TEMP*/
	listbuf[3] = ch;
	return num;
}

int 
TokenList::getNumber()
{
	char ch;
	ch = listbuf[3];
	listbuf[3] = 0;
	listargs = atoi(listbuf);
	listbuf[3] = ch;
	return listargs;
}
int
TokenList::appendToken(char* token)
{
	if (token !=NULL)
	{
		char*	oldstr = listbuf;
		if (listbuf == NULL) {
			return -1;
		}
		buflen = MAX(strlen(listbuf)+1+strlen(token)+1, 4);
		listbuf = new char[buflen];
        int formatted = snprintf(listbuf, buflen, "%s %s", oldstr, token);
        assert(formatted < buflen);//TEMP
/*TEMP
	    if (checkAndTermBuf(listbuf, buflen, formatted) != StatusOk) {
	        reportErrNosys("Buffer overflow!");
		    exit(1);
	    }
TEMP*/

		delete oldstr;
		setNumber(++listargs);
	}
	return listargs;
}

TokenList::TokenList(int args, ...)
{
	if (args >0)
	{
		char**	argv = new char*[args];
		va_list	pvar;
		int	i;
		va_start(pvar,args);
		for (i =0; i < args; ++i)
		{
			argv[i ] = va_arg(pvar, char*);
		}
		va_end(pvar);
		ProcessList(args, argv);
	}
	else
	{
		listargs = 0;
		buflen = 4;
		listbuf = new char[buflen];
		int formatted = snprintf(listbuf, buflen, "%-3d", 0);
        assert(formatted < buflen);//TEMP
/*TEMP
	    if (checkAndTermBuf(listbuf, buflen, formatted) != StatusOk) {
	        reportErrNosys("Buffer overflow!");
		    exit(1);
	    }
TEMP*/
	}
}

TokenList::TokenList(int args, char** argv)
{
	ProcessList(args, argv);
}
void
TokenList::ProcessList(int args, char** argv)
{
	int		i;
	int		len = 3;
	char	*buf;


	for (i=0; i< args; ++i)
	{
		len += strlen(argv[i]) +1;
	}
	buf = new char [len];

	// print all the tokens into one string
	int formatted = snprintf(buf, len, "%-3d", args);   
    assert(formatted < len);//TEMP
/*TEMP
	if (checkAndTermBuf(buf, len, formatted) != StatusOk) {
	    reportErrNosys("Buffer overflow!");
		exit(1);
	}
TEMP*/

	int	curpos = 2;
	for (i=0; i<args; ++i)
	{
		buf[curpos++] = ' ';
		int formatted = snprintf(buf+curpos, len-curpos, argv[i]);
        assert(formatted < len-curpos);//TEMP
/*TEMP
	    if (checkAndTermBuf(buf, len-curpos, formatted) != StatusOk) {
	        reportErrNosys("Buffer overflow!");
		    exit(1);
	    }
TEMP*/
		curpos += strlen(argv[i]);
	}
	buflen = len;
	listbuf = buf;
	listargs = args;
}
char* 
TokenList::getList()
{
	return listbuf;
}
char*
TokenList::toTclList(char*& buf)
{
	if (listargs ==0)
	{
		buf = strdup("{}");
	}
	else
	{
		int len = strlen(listbuf);
		buf = new char[len];
		int formatted = snprintf(buf, len, "{%s}", listbuf+3);
        assert(formatted < len);//TEMP
/*TEMP
	    if (checkAndTermBuf(buf, len, formatted) != StatusOk) {
	        reportErrNosys("Buffer overflow!");
		    exit(1);
	    }
TEMP*/
	}
	return buf;
} 
TokenList::~TokenList()
{
	delete listbuf;
}
