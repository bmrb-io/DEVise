/*
  $Id$

  $Log$*/

/* supports page read/write operations */

#ifndef DiskFile_h
#define DiskFile_h
#include "DeviseTypes.h"

class DiskFile {
public:
	/* Create a new file. Return NULL if file already exists */
	static DiskFile *CreateFile(char *name);

	/* Open an existing file. Return NULL if file does not exist */
	static DiskFile *OpenFile(char *name);

	/* Get data for this page and put it into buffer.
	If page does not already exist, return random data */
	virtual void ReadPage(int pageNum, void *buf)=0;

	/* Write this page onto disk.*/
	virtual void WritePage(int pageNum, void *buf)=0;
};

#endif
