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
 */

#ifndef _DataSourceFileStream_h_
#define _DataSourceFileStream_h_


#include "DataSource.h"


class DataSourceFileStream : public DataSource
{
public:
	DataSourceFileStream(char *filename, char *label);
	virtual ~DataSourceFileStream();

	virtual char *objectType() {return "DataSourceFileStream";};

	virtual DevStatus Open(char *mode);
	virtual DevStatus Close();
	virtual int Fileno();

	virtual char *Fgets(char *buffer, int size);
	virtual size_t Fread(char *buf, size_t size, size_t itemCount);
	virtual size_t Read(char *buf, int byteCount);

	virtual size_t Fwrite(const char *buf, size_t size, size_t itemCount);
	virtual size_t Write(const char *buf, size_t byteCount);

	virtual int Seek(long offset, int from);
	virtual long Tell();

	virtual int gotoEnd();

	virtual int append(void *buf, int recSize);

	virtual Boolean isFile() {return true;};

	virtual Boolean isBuf() {return false;};

	virtual Boolean isTape() {return false;};

private:
	char *		_filename;
	FILE *		_file;
};


#endif /* _DataSourceFileStream_h_ */

/*============================================================================*/
