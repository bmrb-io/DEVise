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
