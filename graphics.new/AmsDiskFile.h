/*
  $Id$

  $Log$*/

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
