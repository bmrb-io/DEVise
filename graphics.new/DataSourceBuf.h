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
  Header file for DataSourceBuf class.
 */

/*
  $Id$

  $Log$
 */

#ifndef _DataSourceBuf_h_
#define _DataSourceBuf_h_


#include "DataSource.h"


class DataSourceBuf : public DataSource
{
public:
	DataSourceBuf(char *buffer, char *label);
	virtual ~DataSourceBuf();

	virtual char *objectType() {return "DataSourceBuf";};

	virtual DevStatus Open(char *mode);
	virtual DevStatus Close();

	virtual char *Fgets(char *buffer, int size);
	virtual size_t Fread(char *buf, size_t size, size_t itemCount);

	virtual int Seek(long offset, int from);
	virtual long Tell();

	virtual int gotoEnd();

	virtual Boolean isFile() {return false;};

	virtual Boolean isBuf() {return true;};

	virtual Boolean isTape() {return false;};

private:
	char *		_sourceBuf;
	char *		_currentLoc;
};


#endif /* _DataSourceBuf_h_ */

/*============================================================================*/
