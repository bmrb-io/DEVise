/*
  $Id$

  $Log$*/

/* Unix RecFile implementation */

#ifndef UnixRecFile_h
#define UnixRecFile_h
#include "RecFile.h"
#include <errno.h>


class UnixRecFile: public RecFile {
public:
    /* Create a new file. Return NULL if file already exists */
	static UnixRecFile *CreateFile(char *name, int recSize);

	/* Open an existing file. Return NULL if file does not exist */
	static UnixRecFile *OpenFile(char *name, int recSize, Boolean trunc = false);

	/* Get the time file last modified. Return an integer
	specifying when file was modified. We require that
	if r1, and r2 are two different files, and r1 was modified before
	r2, then r1->GetModTime() < r2->GetModFile()*/
	virtual long GetModTime();

	virtual char *GetName();

	/* destructor */
	virtual ~UnixRecFile();

	virtual int NumRecs() { return _totalRecs; }

	/* read records */
	virtual void ReadRec(int recNum, int numRecs, void *buf);

	/* Write records */
	virtual void WriteRec(int recNum, int numRecs, void *buf);

protected:
	/* constructor, given the file descriptor */
	UnixRecFile(char *name, int fd, int recSize, int numRecs);

private:
	int _fd;	/* unix file descriptor */
	int _seekRec; /* last record we seeked to */
	int _recSize; /* size of record */
	int _totalRecs;	/* # of records */
	char *_name;
};

#endif
