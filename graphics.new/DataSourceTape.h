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
  Header file for DataSourceTape class.
 */

/*
  $Id$

  $Log$
 */

#ifndef _DataSourceTape_h_
#define _DataSourceTape_h_


#include "DataSource.h"


class TapeDrive;

class DataSourceTape : public DataSource
{
public:
	DataSourceTape(char *buffer, char *label);
	virtual ~DataSourceTape();

	virtual char *objectType() {return "DataSourceTape";};

	virtual DevStatus Open(char *mode);
	virtual DevStatus Close();

	virtual char *Fgets(char *buffer, int size);
	virtual size_t Fread(char *buf, size_t size, size_t itemCount);
	virtual size_t Read(char *buf, int byteCount);

	virtual int Seek(long offset, int from);
	virtual long Tell();

	virtual int gotoEnd();

	virtual int append(void *buf, int recSize);

	virtual void printStats();

	virtual Boolean isFile() {return false;};

	virtual Boolean isBuf() {return false;};

	virtual Boolean isTape() {return true;};

private:
	char *		_filename;
	TapeDrive *	_tapeP;
};


#endif /* _DataSourceTape_h_ */

/*============================================================================*/
