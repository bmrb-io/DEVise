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
  $Id$

  $Log$
  Revision 1.2  1995/09/05 22:14:04  jussi
  Added CVS header.
*/

/* supports page read/write operations */

#ifndef AmsDiskFile_h
#define AmsDiskFile_h
#include "DiskFile.h"


class AmsDiskFile: public DiskFile {
public:

	/* Get data for this page and put it into buffer.
	If page does not already exist, return random data */
	virtual void ReadPage(int pageNum, void *buf);

	/* Write this page onto disk.*/
	virtual void WritePage(int pageNum, void *buf);
};

#endif
