/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.3  1996/05/31 15:40:34  jussi
  Cleaned up a bit. Added copyright notice.

  Revision 1.2  1995/09/05 22:15:28  jussi
  Added CVS header.
*/

/* supports file record read/write operations */

#ifndef RecFile_h
#define RecFile_h

#include "DeviseTypes.h"
#include "DataSource.h"


// Should this be derived from DataSourceFileStream, or should the
// functionality of this class be put int there, or leave it alone??


class RecFile
: public DataSource
{

public:

  RecFile() : DataSource() {}

  virtual ~RecFile() {}

  /* Create a new file. Return NULL if file already exists */
  static RecFile *CreateFile(char *name, int recSize);

  /* Open an existing file. Return NULL if file does not exist .
     trunc == true to truncate file */
  static RecFile *OpenFile(char *name, int recSize, Boolean trunc = false);

  virtual char *GetName() = 0;

  /* Return number of records currently in the file */
  virtual int NumRecs() = 0;

  /* Get data for this record and put it into buffer.
     If record does not already exist, return random data */
  virtual void ReadRec(int recNum, int numRecs, void *buf) = 0;

  /* Write this record onto disk.*/
  virtual void WriteRec(int recNum, int numRecs, void *buf) = 0;

  // The following functions are included to make RecFiles complete
  // DataSources without having to specify these.  I suppose that
  // the should support them sometime...
  DevStatus Open(char*) { return StatusFailed; }
  char IsOk() { return true; }
  DevStatus Close() { return StatusFailed; }
  int Seek(long, int) { return -1; }
  long Tell() { return -1; }
  int gotoEnd() { return -1; }

};

#endif
