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
#include "Csprotocols.h"
#include "ServerAPI.h"
#if !defined(SGI) && !defined(LINUX)
#include <sys/varargs.h>
#else
#include <stdarg.h>
#endif

char* ServerServerProt::DefaultStr = "NULL";
char* ServerServerProt::ControlSeperator = "?+?";
ServerServerProt::~ServerServerProt()
{
	int		i;
	if (dynamicStructure)
	{
		for (i=0; i < newargc; ++i)
			if (newargv[i] != DefaultStr)
				delete newargv[i];
		delete newargv;
	}
}

ServerServerProt::ServerServerProt(int argc, char** argv)
{
	// fill in the defaults
	int		i;
	dynamicStructure = true;
	newargv = new (char*)[argc+ getHeaderLength()](NULL);
	for (i=0; i < getHeaderLength(); ++i)
		newargv[i] = DefaultStr;

	// fill in the original commands
	for (i=0; i< argc; ++i)
		newargv[i+getHeaderLength()] =strdup(argv[i]);

	newargc = argc + getHeaderLength();
	msgsize = NetworkPrepareMsg(API_CMD, 0, newargc,newargv, &recBuffer);
}

ServerServerProt::ServerServerProt(int msize, char* msg)
{
	int tsize;

	dynamicStructure = false;
	msgsize = msize;
	recBuffer = msg;

	NetworkAnalyseHeader((const char *)msg, newargc, tsize);
	NetworkParse((const char *)(msg +
				 sizeof(NetworkHeader)), newargc, newargv);
}
int
ServerServerProt::setServerName(char* sname)
{
	if (dynamicStructure)
	{
		if (newargv[SERVER_NAME]!=DefaultStr)
			delete newargv[SERVER_NAME];

		newargv[SERVER_NAME] = strdup(sname);
		msgsize = NetworkPrepareMsg(API_CMD, 0, newargc,newargv, &recBuffer);
		return msgsize;
	}
	else
		return -1;
}

int
ServerServerProt::setClientName(char* cname)
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
		return -1;
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
			strcat(buf, cmd);
			strcat(buf, ControlSeperator);
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
		return -1;
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
			i++;
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
		listbuf = new (char) [strlen(list)+1];
		strcpy(listbuf, list);
		getNumber();
	}
}
int 
TokenList::setNumber(int num)
{
	char ch;

	ch = listbuf[3];
	sprintf(listbuf,"%-3d",num);
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
		if (listbuf == NULL)
			return -1;
		listbuf = new (char) [strlen(listbuf)+1+strlen(token)+1];
		strcpy(listbuf, oldstr);
		strcat(listbuf," ");
		strcat(listbuf,token);
		delete oldstr;
		setNumber(++listargs);
	}
	return listargs;
}

TokenList::TokenList(int args, ...)
{
	if (args >0)
	{
		char**	argv = new (char*)[args](NULL);
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
		listbuf = new (char)[4];
		sprintf(listbuf,"%-3d",0);
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
	sprintf(buf, "%-3d", args);   

	int	curpos = 2;
	for (i=0; i<args; ++i)
	{
		buf[curpos++] = ' ';
		sprintf(buf+curpos, argv[i]);
		curpos += strlen(argv[i]);
	}
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
		buf = new (char)[strlen(listbuf)];
		sprintf(buf,"{%s}",listbuf+3);
	}
	return buf;
} 
TokenList::~TokenList()
{
	delete listbuf;
}
