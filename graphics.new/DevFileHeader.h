/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of DevFileHeader class.
 */

/*
  $Id$

  $Log$
  Revision 1.8  2000/01/13 23:07:05  wenger
  Got DEVise to compile with new (much fussier) compiler (g++ 2.95.2).

  Revision 1.7  1999/12/06 18:41:04  wenger
  Simplified and improved command logging (includes fixing bug 537, command
  logs are now human-readable); added standard header to debug logs.

  Revision 1.6  1998/03/27 15:08:54  wenger
  Added dumping of logical session description, added GUI for dumping
  logical or physical description; cleaned up some of the command code
  a little.

  Revision 1.5  1997/07/22 15:36:36  wenger
  Added capability to dump human-readable information about all links
  and cursors.

  Revision 1.4  1997/06/25 21:25:29  wenger
  Added writeDesc (write session description) command needed by Hongyu's
  Java client.

  Revision 1.3  1996/11/03 02:41:37  kmurli
  Modified to include the query schema level. Also modified to include DQL
  processing

  Revision 1.2  1996/08/23 16:56:15  wenger
  First version that allows the use of Dali to display images (more work
  needs to be done on this); changed DevStatus to a class to make it work
  better; various minor bug fixes.

  Revision 1.1  1996/07/11 17:25:40  wenger
  Devise now writes headers to some of the files it writes;
  DataSourceSegment class allows non-fixed data length with non-zero
  offset; GUI for editing schema files can deal with comment lines;
  added targets to top-level makefiles to allow more flexibility.

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
#define FILE_TYPE_QSCHEMA	"querySchema"
#define FILE_TYPE_TEMP		"temp"
#define FILE_TYPE_WORK		"work"
#define FILE_TYPE_CACHE		"cache"
#define FILE_TYPE_CORAL		"coral"
#define FILE_TYPE_PIXMAP	"pixmap"
#define FILE_TYPE_SCHEMACAT	"schemaCat"
#define FILE_TYPE_SESSIONDESCP	"sessionDescPhys"
#define FILE_TYPE_SESSIONDESCL	"sessionDescLog"
#define FILE_TYPE_LINKDESC	"linkDesc"
#define FILE_TYPE_DEBUGLOG	"debugLog"
#define FILE_TYPE_CMDLOG	"commandLog"
#define FILE_TYPE_JSCMDCACHE	"javaScreenCmdCache"

class DevFileHeader
{
public:
    // Get the string that's the file header for the given file type.
    static char *Get(const char *fileType);

    struct Info {
        static const int bufSize = 64;
        char fileType[bufSize];
        char fileDate[bufSize];
        char deviseVersion[bufSize];
        char deviseDate[bufSize];
    };
    // Get the header info from a file.
    static DevStatus Read(FILE *file, Info &info);

    //static DevStatus Write();
    //static DevStatus Parse();
    //static DevStatus Skip();

protected:
    static DevStatus GetInfo(FILE *file, const char *prefix, char *infoBuf,
      int infoBufSize);
};


#endif /* _DevFileHeader_h_ */

/*============================================================================*/
