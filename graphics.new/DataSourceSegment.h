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
  Header file for DataSourceSegment classes (template).
 */

/*
  $Id$

  $Log$
 */

#ifndef _DataSourceSegment_
#define _DataSourceSegment_


#ifdef __GNUG__
#pragma interface
#endif


#include "DeviseTypes.h"


template<class TYPE>
class DataSourceSegment : public TYPE
{
public:
	DataSourceSegment(char *filename, char *label, long dataOffset,
		long dataLength);
	~DataSourceSegment();

	virtual char *objectType() {return "DataSourceSegment";};

	virtual DevStatus Open(char *mode);

	virtual int Seek(long offset, int from);
	virtual long Tell();

	virtual int gotoEnd();

private:
	long	_dataOffset;
	long	_dataLength;
};


#endif /* _DataSourceSegment_ */

/*============================================================================*/
