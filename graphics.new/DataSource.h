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
  Header file for DataSource class.
 */

/*
  $Id$

  $Log$
  Revision 1.3  1996/05/07 22:28:19  jussi
  Reverted the changes made in the previous check-in because I
  found a better way to fix the problem where only the tail
  part of a schema file name is returned when a session is saved.

  Revision 1.2  1996/05/07 22:13:50  jussi
  Added virtual method getName() to DataSourceFile which returns
  the filename, not the regular name or alias. The filename is
  needed when a session file is saved.

  Revision 1.1  1996/05/07 16:04:18  wenger
  Added final version of code for reading schemas from session files;
  added an error-reporting class to improve error info.

 */

#ifndef _DataSource_h_
#define _DataSource_h_


#include "DeviseTypes.h"


class DataSource
{
public:
	DataSource(char *label);
	virtual ~DataSource();

	virtual char *objectType() {return "DataSource";};

	virtual char *getLabel();

	// These functions essentially do the same thing as the standard
	// functions of the same name.  The names are capitalized to avoid
	// conflicts with macros in some system header files that redefine
	// symbols such as ftell, etc.
	virtual DevStatus Open(char *mode) = 0;
	virtual DevStatus Close() = 0;
	virtual int Fileno();

	virtual char *Fgets(char *buffer, int size);
	virtual size_t Fread(char *buf, size_t size, size_t itemCount);
	virtual size_t Read(char *buf, int byteCount);

	virtual size_t Fwrite(const char *buf, size_t size, size_t itemCount);
	virtual size_t Write(const char *buf, size_t byteCount);

	// Returns -1 if error, 0 otherwise.
	virtual int Seek(long offset, int from) = 0;
	virtual long Tell() = 0;

	// Go to the end of the data, and return that location.
	virtual int gotoEnd() = 0;

	// Append to the end of the data.
	virtual int append(void *buf, int recSize);

	// Print statistics about this object.
	virtual void printStats();

	// Is this object a (disk) file?
	virtual Boolean isFile() = 0;

	// Is this object a buffer?
	virtual Boolean isBuf() = 0;

	// Is this object a tape?
	virtual Boolean isTape() = 0;

private:
	char *		_label;
};


#endif /* _DataSource_h_ */

/*============================================================================*/
