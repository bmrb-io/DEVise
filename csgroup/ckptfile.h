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
  Revision 1.2  1998/02/12 17:14:44  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.2  1998/02/02 08:23:52  liping
  Added CVS header

*/

#ifndef _CKPTFILE_H
#define _CKPTFILE_H
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

class	UniqueFileName {
	public:
		char	buf [200];
		UniqueFileName(char*& fname, char* prefix)
		{
			sprintf(buf, "/tmp/%s_%ld_%ld",prefix, (long)getuid(),
				(long)getpid());
			fname = buf;
		}
};
extern char	*ServerCkptFile;
extern char *MasterCkptFile;
#endif
