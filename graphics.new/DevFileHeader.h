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
  Description of module.
 */

/*
  $Id$

  $Log$
 */

#ifndef _DevFileHeader_h_
#define _DevFileHeader_h_


#include "DeviseTypes.h"


// Note: file types are defined as strings to make Tcl interface easier.
#define FILE_TYPE_SESSION	"session"
#define FILE_TYPE_TDATA		"tdata"
#define FILE_TYPE_DATACAT	"dataCat"
#define FILE_TYPE_PSCHEMA	"physSchema"
#define FILE_TYPE_LSCHEMA	"logSchema"
#define FILE_TYPE_TEMP		"temp"
#define FILE_TYPE_WORK		"work"
#define FILE_TYPE_CACHE		"cache"
#define FILE_TYPE_CORAL		"coral"
#define FILE_TYPE_PIXMAP	"pixmap"
#define FILE_TYPE_SCHEMACAT	"schemaCat"


class DevFileHeader
{
public:
    static char *Get(char *fileType);
    //static DevStatus Write();
    //static DevStatus Read();
    //static DevStatus Parse();
    //static DevStatus Skip();
};


#endif /* _DevFileHeader_h_ */

/*============================================================================*/
