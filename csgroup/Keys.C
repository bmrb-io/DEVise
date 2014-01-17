/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2010
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
  Revision 1.6.4.1  2014/01/17 21:46:00  wenger
  Fixed a bunch of possible buffer overflows.

  Revision 1.6  2008/01/24 22:08:05  wenger
  Got rid of a bunch of compile warnings.

  Revision 1.5  2005/12/06 20:01:10  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.4.18.1  2005/09/28 22:29:21  wenger
  Various const-ifying to make things compile better on basslet.

  Revision 1.4  1999/01/18 18:11:13  beyer
  fixed compile errors and warnings for egcs version 1.1.1

  Revision 1.3  1998/02/19 23:24:07  wenger
  Improved color library and got client/server test code to work
  (except for setting colors by RGB): reduced compile interdependencies,
  especially in color library; color and utils libraries install headers
  as per code reorg plans; added standard DEVise headers to all color
  manager files; moved color initialization into Display constructors;
  fixed some compile warnings throughout the code.

  Revision 1.2  1998/02/12 17:14:33  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.2  1998/02/02 08:23:49  liping
  Added CVS header

*/

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
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
		_cskey = new char[keysize +1];
		int formatted = snprintf(_cskey, keysize+1, "%s%c%s",
		  _gkp->grpName, seperator, _gkp->grpPwd);
		assert(formatted < keysize+1);//TEMP
/*TEMP
	    if (checkAndTermBuf(_cskey, keysize+1, formatted) != StatusOk) {
		    reportErrNosys("Buffer overflow!");
			exit(1);
		}
TEMP*/
	}
}
CSgroupKey::CSgroupKey(const char* grpname, const char* passwd)
{
	analyse2Str(grpname, passwd);
}

void
CSgroupKey::analyse2Str(const char*grpname, const char* passwd)
{
	char	*temp;
	int len = strlen(grpname)+strlen(passwd)+2;
	temp = new char[len];
	int formatted = snprintf(temp, len, "%s%c%s", grpname, seperator, passwd);
	assert(formatted < len);//TEMP
/*TEMP
	if (checkAndTermBuf(temp, len, formatted) != StatusOk) {
	    reportErrNosys("Buffer overflow!");
		exit(1);
	}
TEMP*/
	analyseStr(temp);
	delete temp;
}	
CSgroupKey::CSgroupKey(const char* cskey)
{
	analyseStr(cskey);
}
void
CSgroupKey::analyseStr(const char* cskey)
{
	_cskey = NULL;
	_gkp = NULL;
	if (cskey != NULL)
	{
		int keysize;
		keysize = sizeof(GroupKey);
		_cskey = strdup(cskey);
			//TEMP -- using cskeyTmp hasn't really been tested --
			// wenger, 2005-09-28.
		char *cskeyTmp = strdup(cskey);
		_gkp = new GroupKey;
		if (((int) strlen(_cskey)) < keysize)
		{
			int	i = 0;
			while ((i<keysize)&&(cskeyTmp[i++]!=seperator));
			if (i<keysize)
			{
				cskeyTmp[i-1] = 0;
				strncpy(_gkp->grpName, cskeyTmp, sizeof(_gkp->grpName));
				_gkp->grpName[sizeof(_gkp->grpName)-1] = '\0';
				strncpy(_gkp->grpPwd, cskeyTmp+i, sizeof(_gkp->grpPwd));
				_gkp->grpPwd[sizeof(_gkp->grpPwd)-1] = '\0';
				cskeyTmp[i-1] = seperator;
				delete cskeyTmp;
				return;
			}
		}
		delete cskeyTmp;
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
