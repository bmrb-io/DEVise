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
  Header file for DataSource, DataSourceFile, and DataSourceBuf classes.
 */

/*
  $Id$

  $Log$
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
	DataSource(char *name);
	virtual ~DataSource();

	virtual DevStatus fopen(char *type) = 0;
	virtual DevStatus fclose() = 0;
	virtual char *fgets(char *buffer, int size) = 0;
	virtual char *getName();

private:
	char *		_name;
};

class DataSourceFile : public DataSource
{
public:
	DataSourceFile(char *filename, char *name);
	virtual ~DataSourceFile();

	virtual DevStatus fopen(char *type);
	virtual DevStatus fclose();
	virtual char *fgets(char *buffer, int bufSize);

private:
	char *		_filename;
	FILE *		_file;
};

class DataSourceBuf : public DataSource
{
public:
	DataSourceBuf(char *buffer, char *name);
	virtual ~DataSourceBuf();

	virtual DevStatus fopen(char *type);
	virtual DevStatus fclose();
	virtual char *fgets(char *buffer, int bufSize);

private:
	char *		_sourceBuf;
	char *		_currentLoc;
};

#endif /* _DataSource_h_ */

/*============================================================================*/
