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
  Revision 1.1.2.2  1998/02/02 08:23:49  liping
  Added CVS header

*/

#include <iostream.h>
#include <string.h>
#include "keys.h"
// assume the key is composed of two strings
CSgroupKey::CSgroupKey()
{
	char nullstr[2];
	nullstr[0] = seperator-1;
	nullstr[1] = 0 ;
	analyse2Str((char*)nullstr, (char*)nullstr);
}

bool
CSgroupKey::containName(char* grpname)
{
	return !strcmp(grpname, _gkp->grpName);
}

bool
CSgroupKey::isNull()
{
	if ((strlen(_cskey)==3)&&
		(_cskey[0]== seperator-1)&&
		(_cskey[1]== seperator)&&
		(_cskey[2]== seperator-1))
		return true;
	else
		return false;
}

CSgroupKey::CSgroupKey(GroupKey *gkp)
{
	_cskey = NULL;
	_gkp = new GroupKey;
	*_gkp = *gkp;

	if (gkp != NULL)
	{
		int		keysize;
		keysize = sizeof(GroupKey);
		_cskey = new char[keysize +1](0);
		sprintf(_cskey,"%s%c%s",_gkp->grpName,seperator,_gkp->grpPwd);
	}
}
CSgroupKey::CSgroupKey(char* grpname, char* passwd)
{
	analyse2Str(grpname, passwd);
}

void
CSgroupKey::analyse2Str(char*grpname, char* passwd)
{
	char	*temp;
	temp = new char[strlen(grpname)+strlen(passwd)+2];
	sprintf(temp,"%s%c%s",grpname,seperator,passwd);
	analyseStr(temp);
	delete temp;
}	
CSgroupKey::CSgroupKey(char* cskey)
{
	analyseStr(cskey);
}
void
CSgroupKey::analyseStr(char* cskey)
{
	_cskey = NULL;
	_gkp = NULL;
	if (cskey != NULL)
	{
		int keysize;
		keysize = sizeof(GroupKey);
		_cskey = strdup(cskey);
		_gkp = new GroupKey;
		if (strlen(_cskey) < keysize)
		{
			int	i = 0;
			while ((i<keysize)&&(cskey[i++]!=seperator));
			if (i<keysize)
			{
				cskey[i-1] = 0;
				strcpy(_gkp->grpName, cskey);
				strcpy(_gkp->grpPwd, cskey+i);
				cskey[i-1] = seperator;
				return;
			}
		}
		delete _gkp;
		delete _cskey;
		_cskey = NULL;
		_gkp = NULL;
	}
}

CSgroupKey::~CSgroupKey()
{
	if (_cskey != NULL)
		delete _cskey;
	if (_gkp != NULL)
		delete _gkp;
}

GroupKey*
CSgroupKey::toGroupKey()
{
	return _gkp;
}

char*
CSgroupKey::toStr()
{
	return _cskey;
}
