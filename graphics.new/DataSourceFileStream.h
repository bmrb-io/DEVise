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
  Header file for DataSourceFileStream class.
 */

/*
  $Id$

  $Log$
  Revision 1.2  1996/06/27 15:50:59  jussi
  Added IsOk() method which is used by TDataAscii and TDataBinary
  to determine if a file is still accessible. Also moved GetModTime()
  functionality from TDataAscii/TDataBinary to the DataSource
  classes.

  Revision 1.1  1996/05/22 17:52:04  wenger
  Extended DataSource subclasses to handle tape data; changed TDataAscii
  and TDataBinary classes to use new DataSource subclasses to hide the
  differences between tape and disk files.

 */

#ifndef _DataSourceFileStream_h_
#define _DataSourceFileStream_h_


#include "DataSource.h"


class DataSourceFileStream : public DataSource
{
public:
	DataSourceFileStream(char *filename, char *label, char *param = 0);
	virtual ~DataSourceFileStream();

	virtual char *objectType() {return "DataSourceFileStream";};

	virtual DevStatus Open(char *mode);
	virtual Boolean IsOk();
	virtual DevStatus Close();

	virtual char *Fgets(char *buffer, int size);
	virtual size_t Fread(char *buf, size_t size, size_t itemCount);
	virtual size_t Read(char *buf, int byteCount);

	virtual size_t Fwrite(const char *buf, size_t size, size_t itemCount);
	virtual size_t Write(const char *buf, size_t byteCount);

	virtual int Seek(long offset, int from);
	virtual long Tell();

	virtual int gotoEnd();

	virtual int append(void *buf, int recSize);

	virtual int GetModTime();

	virtual Boolean isFile() {return true;};

	virtual Boolean isBuf() {return false;};

	virtual Boolean isTape() {return false;};

protected:
	char *		_filename;
	FILE *		_file;
};


#endif /* _DataSourceFileStream_h_ */

/*============================================================================*/
