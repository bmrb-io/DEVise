/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of Client (and CompDate) classes.
 */

/*
  $Id$

  $Log$
  Revision 1.2  1998/02/12 17:14:54  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.2  1998/02/02 08:24:00  liping
  Added CVS header

*/

#ifndef KEYS_H
#define KEYS_H
#include "CollabTypes.h"
class CSgroupKey
{
	private:
		static 	const int seperator = 127;
		char*	_cskey;
		GroupKey *_gkp;
		void	analyseStr(char* cskey);
		void	analyse2Str(char* grpname, char* passwd);
	public:
		CSgroupKey();
		CSgroupKey(GroupKey* gkp);
		CSgroupKey(char* cskey);
		CSgroupKey(char* grpname, char* passwd);
		~CSgroupKey();
		bool isNull();
		bool containName(char* grpname);
		char* toStr();
		GroupKey* toGroupKey();
};
#endif

