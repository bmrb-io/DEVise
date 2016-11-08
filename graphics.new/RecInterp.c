/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2012
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.8.12.1  2012/04/24 20:46:53  wenger
  Actual data saving now working except for some things like escaping
  commas; the code still needs some cleanup; JS GUI and help still need work.

  Revision 1.8  2008/09/23 17:56:53  wenger
  Fixed string const problems in generic and some related modules.

  Revision 1.7  2005/12/06 20:04:12  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.6.46.2  2003/04/24 17:31:16  wenger
  Fixed bug in drill-down code; axis date formats now also apply to
  drill-down coordinates; we now check for buffer overruns in
  DateString().

  Revision 1.6.46.1  2003/04/17 17:59:25  wenger
  Now compiles with no warnings with gcc 2.95, except for warnings about
  tempname and tmpnam on Linux; updated Linux and Solaris dependencies.

  Revision 1.6  1996/05/07 16:37:14  jussi
  Added _recPos variable and methods for it.

  Revision 1.5  1996/01/19 20:03:17  jussi
  Remove redundant Print().

  Revision 1.4  1995/12/28 19:46:47  jussi
  Small fixes to remove compiler warnings.

  Revision 1.3  1995/12/14 17:53:12  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.2  1995/09/05 22:15:30  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <time.h>

#include "AttrList.h"
#include "RecInterp.h"
#include "Util.h"

RecInterp::RecInterp()
{
  _attrs = NULL;
  _buf = NULL;
  _recPos = 0;
}

void RecInterp::SetBuf(void *buf)
{
  _buf = buf;
}

void RecInterp::SetAttrs(AttrList *attrs)
{
  _attrs = attrs;
}

char *RecInterp::GetString(char *attrName)
{
  AttrInfo *info;
  if (_attrs == NULL || _buf == NULL)
    return NULL;
  if ((info = _attrs->Find(attrName)) == NULL)
    return NULL;
  return ((char *)_buf) + info->offset;
}

double *RecInterp::GetFloat(char *attrName)
{
  AttrInfo *info;
  if (_attrs == NULL || _buf == NULL)
    return NULL;
  if ((info = _attrs->Find(attrName)) == NULL)
    return NULL;
  return (double *)(((char *)_buf) + info->offset);
}

int *RecInterp::GetInt(char *attrName)
{
  AttrInfo *info;
  if (_attrs == NULL || _buf == NULL)
    return NULL;
  if ((info = _attrs->Find(attrName)) == NULL)
    return NULL;
  return (int *)(((char *)_buf) + info->offset);
}

time_t *RecInterp::GetDate(char *attrName)
{
  AttrInfo *info;
  if (_attrs == NULL || _buf == NULL)
    return NULL;
  if ((info = _attrs->Find(attrName)) == NULL)
    return NULL;
  return (time_t *)(((char *)_buf) + info->offset);
}

AttrInfo *RecInterp::GetAttrInfo(const char *attrName)
{
  if (_attrs == NULL )
    return NULL;
  return _attrs->Find(attrName);
}

void RecInterp::PrintAttrHeading()
{
  if (_attrs == NULL )
    return;
  
  int num = _attrs->NumAttrs();
  for(int i = 0; i < num; i++) {
    AttrInfo *info = _attrs->Get(i);
    printf("%s: ", info->name);
  }
}

void RecInterp::PrintAttr(char *buf, int bufLen, int attrNum,
    Boolean printAttrName)
{
  if (_attrs == NULL || _buf == NULL) {
    buf[0] = '\0';
    return;
  }
  
  AttrInfo *info = _attrs->Get(attrNum);

  int formatted = 0;

  switch(info->type) {
  case IntAttr:
    {
      int *intVal = (int *)(((char *)_buf) + info->offset);
      if (printAttrName) {
        formatted = snprintf(buf, bufLen, "%s: %d", info->name, *intVal);
      } else {
        formatted = snprintf(buf, bufLen, "%d", *intVal);
      }
    }
    break;

  case FloatAttr:
    {
      float *floatVal = (float *)(((char *)_buf) + info->offset);
      if (printAttrName) {
        formatted = snprintf(buf, bufLen, "%s: %.2f", info->name, *floatVal);
      } else {
        formatted = snprintf(buf, bufLen, "%f", *floatVal);
      }
    }
    break;

  case DoubleAttr:
    {
      double *doubleVal = (double *)(((char *)_buf) + info->offset);
      if (printAttrName) {
        formatted = snprintf(buf, bufLen, "%s: %.2f", info->name, *doubleVal);
      } else {
        formatted = snprintf(buf, bufLen, "%f", *doubleVal);
      }
    }
    break;

  case StringAttr:
    {
      char *strVal = ((char *)_buf) + info->offset;
      if (printAttrName) {
        formatted = snprintf(buf, bufLen, "%s: %s", info->name, strVal);
      } else {
        formatted = snprintf(buf, bufLen, "%s", strVal);
      }
    }
    break;

  case DateAttr:
    {
      time_t *tm = (time_t *)(((char *)_buf) + info->offset);
      if (printAttrName) {
        formatted = snprintf(buf, bufLen, "%s: '%s'", info->name,
	    DateString(*tm));
      } else {
        formatted = snprintf(buf, bufLen, "'%s'", DateString(*tm));
      }
    }
    break;

  case InvalidAttr:
    {
      if (printAttrName) {
        formatted = snprintf(buf, bufLen, "%s: InvalidAttr", info->name);
      } else {
        formatted = snprintf(buf, bufLen, "InvalidAttr");
      }
    }
    break;

  default:
    DOASSERT(false, "Illegal attribute type");
    break;
  }

  checkAndTermBuf(buf, bufLen, formatted);
}
