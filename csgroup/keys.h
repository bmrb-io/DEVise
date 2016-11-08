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
  Revision 1.3.24.1  2005/09/28 22:29:21  wenger
  Various const-ifying to make things compile better on basslet.

  Revision 1.3  1998/04/01 05:21:49  taodb
  Replaced types.h with CollabTypes.h, related source files were updated.

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
		void	analyseStr(const char* cskey);
		void	analyse2Str(const char* grpname, const char* passwd);
	public:
		CSgroupKey();
		CSgroupKey(GroupKey* gkp);
		CSgroupKey(const char* cskey);
		CSgroupKey(const char* grpname, const char* passwd);
		~CSgroupKey();
		bool isNull();
		bool containName(char* grpname);
		char* toStr();
		GroupKey* toGroupKey();
};
#endif

