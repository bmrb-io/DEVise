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
  Header file for DataSeg (data segment) class.  This class holds info
  for data that occupies only part of a file (or another data source).
 */

/*
  $Id$

  $Log$
 */

#ifndef _DataSeg_h_
#define _DataSeg_h_


class DataSeg
{
public:
	static void Set(char *label, char *filename, long offset, long length);
	static void Get(char *&label, char *&filename, long &offset, long &length);

private:
	static char *	_label;
	static char *	_filename;
	static long		_offset;
	static long		_length;
};


#endif /* _DataSeg_h_ */

/*============================================================================*/
