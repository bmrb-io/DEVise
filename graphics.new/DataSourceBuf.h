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
  Revision 1.3  1996/07/01 19:31:32  jussi
  Added an asynchronous I/O interface to the data source classes.
  Added a third parameter (char *param) to data sources because
  the DataSegment template requires that all data sources have the
  same constructor (DataSourceWeb requires the third parameter).

  Revision 1.2  1996/06/27 15:50:57  jussi
  Added IsOk() method which is used by TDataAscii and TDataBinary
  to determine if a file is still accessible. Also moved GetModTime()
  functionality from TDataAscii/TDataBinary to the DataSource
  classes.

  Revision 1.1  1996/05/22 17:52:01  wenger
  Extended DataSource subclasses to handle tape data; changed TDataAscii
  and TDataBinary classes to use new DataSource subclasses to hide the
  differences between tape and disk files.

 */

#ifndef _DataSourceBuf_h_
#define _DataSourceBuf_h_


#include "DataSource.h"


class DataSourceBuf : public DataSource
{
public:
	DataSourceBuf(char *buffer, char *label, char *param = 0);
	virtual ~DataSourceBuf();

	virtual char *objectType() {return "DataSourceBuf";};

	virtual DevStatus Open(char *mode);
	virtual Boolean IsOk() { return true; }
	virtual DevStatus Close();

	virtual char *Fgets(char *buffer, int size);
	virtual size_t Fread(char *buf, size_t size, size_t itemCount);
	virtual size_t Read(char *buf, int byteCount);

	virtual size_t Fwrite(const char *buf, size_t size, size_t itemCount);
	virtual size_t Write(const char *buf, size_t byteCount);

	virtual int Seek(long offset, int from);
	virtual long Tell();

	virtual int gotoEnd();

	virtual int GetModTime() {return -1;}

	virtual Boolean isFile() {return false;};

	virtual Boolean isBuf() {return true;};

	virtual Boolean isTape() {return false;};

private:
	char *		_sourceBuf;
	char *		_currentLoc;
};


#endif /* _DataSourceBuf_h_ */

/*============================================================================*/
