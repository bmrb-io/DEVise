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
  Revision 1.1.2.2  1998/02/02 08:24:03  liping
  Added CVS header

*/

/* 
** Copyright 1997 Collaborator Design Team
** 
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted,
** provided that the above copyright notice appear in all copies and that
** both that copyright notice and this permission notice appear in
** supporting documentation, and that the names of the University of
** Wisconsin and the Collaborator Design Team not be used in advertising or
** publicity pertaining to distribution of the software without specific,
** written prior permission.  The University of Wisconsin and the Collaborator
** Design Team make no representations about the suitability of this
** software for any purpose.  It is provided "as is" without express
** or implied warranty.
** 
** THE UNIVERSITY OF WISCONSIN AND THE COLLABORATOR DESIGN TEAM DISCLAIM ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE UNIVERSITY OF
** WISCONSIN OR THE COLLABORATOR DESIGN TEAM BE LIABLE FOR ANY SPECIAL, 
** INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
** NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
** WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
** 
** Author:  Dongbin Tao
** 	         University of Wisconsin, Computer Sciences Dept.
** 
*/ 
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include "parseConfig.h"

parseConfig::parseConfig(char*configfile, char csymbol)
{
	ifs = new ifstream();

	this->csymbol = csymbol;
	filename = strdup(configfile);
	listhead = NULL;

	ifs->open(filename);
	if (ifs->fail())
	{
		cerr << "Configuration file not found\n";
	}
	while (!ifs->eof())
	{
		char	buf[128];

		// skip the comment 
		ifs->getline(buf,sizeof(buf));
		if (ifs->fail())
			break;
		if (buf[0] !=csymbol)
		{
			char 	*key = strtok((char*)buf," ");
			char 	*content = strtok(NULL," ");
			if (insertName(key, content)==NULL)
			{
				cerr << "Redefination of key= "<<key<<endl;
			}
		}
	}
}
char*
parseConfig::lookupName(char*key)
{
	Node	*pt= listhead;
	char	*retval = NULL;

	while (pt != NULL)
	{
		if (strcmp(key, pt->key)==0)
		{
			retval = pt->content;
			break;
		}
		pt = pt->next;
	}
	return retval;
}
char*
parseConfig::insertName(char *key, char*content)
{
	char	*retval = NULL;

	if (lookupName(key) == NULL)
		listhead = new Node ((retval=strdup(key)),strdup(content),listhead);
	return retval;
}
parseConfig::~parseConfig()
{
	delete filename;
	while (listhead!= NULL)
	{
		Node	*pt;
		pt = listhead ;
		listhead = listhead->next;
		delete pt->key;
		delete pt->content;
		delete pt;
	}
	ifs->close();
}
