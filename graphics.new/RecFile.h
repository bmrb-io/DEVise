/*
  $Id$

  $Log$*/

/* supports file record read/write operations */

#ifndef RecFile_h
#define RecFile_h
#include "DeviseTypes.h"

class RecFile {
public:
	virtual ~RecFile() {};

	/* Create a new file. Return NULL if file already exists */
	static RecFile *CreateFile(char *name, int recSize);

	/* Open an existing file. Return NULL if file does not exist .
	trunc == true to truncate file */
	static RecFile *OpenFile(char *name, int recSize, Boolean trunc = false);

	/* Get the time file last modified. Return an integer
	specifying when file was modified. We require that
	if r1, and r2 are two different files, and r1 was modified before
	r2, then r1->GetModTime() < r2->GetModFile()*/
	virtual long GetModTime() = 0;

	virtual char *GetName()=0;

	/* Return number of records currently in the file */
	virtual int NumRecs()= 0;

	/* Get data for this record and put it into buffer.
	If record does not already exist, return random data */
	virtual void ReadRec(int recNum, int numRecs, void *buf)=0;

	/* Write this record onto disk.*/
	virtual void WriteRec(int recNum, int numRecs, void *buf)=0;
};

#endif
