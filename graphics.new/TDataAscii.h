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
  $Id$

  $Log$
  Revision 1.28  2000/01/11 22:28:33  wenger
  TData indices are now saved when they are built, rather than only when a
  session is saved; other improvements to indexing; indexing info added
  to debug logs; moved duplicate TDataAscii and TDataBinary code up into
  TData class.

  Revision 1.27  1998/10/13 19:40:45  wenger
  Added SetAttrs() function to TData and its subclasses to allow Liping to
  push projection down to the DTE.

  Revision 1.26  1997/12/23 23:35:39  liping
  Changed internal structure of BufMgrFull and classes it called
  The buffer manager is now able to accept queries on any attribute from the
          Query Processor
  The buffer manager is also able to issue queries on various attributes to DTE
  Instead of keeping an in memory list for each T/GData, the buffer manager keeps
          a list for each (T/GData, AttrName, Granularity) combination
  The class Range was replaced by Interval

  Revision 1.25  1997/10/10 21:13:46  liping
  The interface between TData and BufMgr and the interface between BufMgr and
  QueryProc were changed
  The new interface carries the information of 1. LowId 2. HighId 3. AttrName
          4. Granularity in the structure "Range"

  Revision 1.24  1997/07/03 01:53:47  liping
  changed query interface to TData from RecId to double

  Revision 1.23  1997/01/11 20:56:05  jussi
  Removed references to _currPos.

  Revision 1.22  1997/01/09 18:48:27  jussi
  Added controlling of live data update frequency.

  Revision 1.21  1996/12/18 19:34:06  jussi
  Fixed minor bugs in ReadRecAsync(). Added FlushDataPipe().

  Revision 1.20  1996/12/03 20:32:58  jussi
  Updated to reflect new TData interface. Added support for concurrent I/O.

  Revision 1.19  1996/11/23 21:14:23  jussi
  Removed failing support for variable-sized records.

  Revision 1.18  1996/11/22 20:41:09  flisakow
  Made variants of the TDataAscii classes for sequential access,
  which build no indexes.

  ReadRec() method now returns a status instead of void for every
  class that has the method.

  Revision 1.17  1996/10/04 17:24:16  wenger
  Moved handling of indices from TDataAscii and TDataBinary to new
  FileIndex class.

  Revision 1.16  1996/08/04 21:59:53  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

  Revision 1.15  1996/07/12 19:39:43  jussi
  Removed _file member variable as it's not needed.

  Revision 1.14  1996/07/03 23:13:51  jussi
  Added call to _data->Close() in destructor. Renamed
  _fileOkay to _fileOpen which is more accurate.

  Revision 1.13  1996/07/01 19:28:06  jussi
  Added support for typed data sources (WWW and UNIXFILE). Renamed
  'cache' references to 'index' (cache file is really an index).
  Added support for asynchronous interface to data sources.

  Revision 1.12  1996/06/27 18:12:39  wenger
  Re-integrated most of the attribute projection code (most importantly,
  all of the TData code) into the main code base (reduced the number of
  modules used only in attribute projection).

  Revision 1.11  1996/06/27 15:49:32  jussi
  TDataAscii and TDataBinary now recognize when a file has been deleted,
  shrunk, or has increased in size. The query processor is asked to
  re-issue relevant queries when such events occur.

  Revision 1.10  1996/06/12 14:56:33  wenger
  Added GUI and some code for saving data to templates; added preliminary
  graphical display of TDatas; you now have the option of closing a session
  in template mode without merging the template into the main data catalog;
  removed some unnecessary interdependencies among include files; updated
  the dependencies for Sun, Solaris, and HP; removed never-accessed code in
  ParseAPI.C.

  Revision 1.9  1996/05/22 17:52:15  wenger
  Extended DataSource subclasses to handle tape data; changed TDataAscii
  and TDataBinary classes to use new DataSource subclasses to hide the
  differences between tape and disk files.

  Revision 1.8  1996/05/07 16:43:01  jussi
  Cache file name now based on file alias (TData name). Added parameter
  to the Decode() function call.

  Revision 1.7  1996/03/26 21:18:44  jussi
  Merged with TDataTape. Added magic number to cache file.

  Revision 1.6  1996/01/25 20:22:58  jussi
  Improved support for data files that grow while visualization
  is being performed.

  Revision 1.5  1995/12/28 19:59:35  jussi
  Small fixes to remove compiler warnings.

  Revision 1.4  1995/11/24 21:35:24  jussi
  Added _currPos member.

  Revision 1.3  1995/11/22 17:52:02  jussi
  Added copyright notice and cleaned up the code. Added some
  optimizations a la TDataTape.h.

  Revision 1.2  1995/09/05 22:15:50  jussi
  Added CVS header.
*/

/* Textual data virtual base class */

#ifndef TDataAscii_h
#define TDataAscii_h

#include <sys/types.h>

#include "TData.h"
#include "RecId.h"
#include "DataSource.h"

class TDataAscii: public TData {
public:
	TDataAscii(char *name, char *type, char *param, int recSize);

	virtual ~TDataAscii();

	/**** MetaData about TDataAscii ****/

	// Get list of attributes
	//TEMP -- could this be moved up into TData??
	virtual AttrList *GetAttrList() { return NULL; }
	virtual Boolean SetAttrs(const AttrList &attrs) { return false; }

	// Return true if TDataTape appends records
	virtual Boolean HasAppend() { return true; }

	/* writing a record. For TDataAscii, the new record
	is appended to the file (startRid not examined), numRecs ==1, 
	and buf points to a string to be written to disk. */
	virtual void WriteRecs(RecId startId, int numRecs, void *buf);

	/* Write a line into the file, but don't make it into a record */
	void WriteLine(void *line);

protected:
	/* Decode a record and put data into buffer. Return false if
	this line is not valid. */
	virtual Boolean Decode(void *recordBuf, int recPos, char *line) = 0;

private:
	/* From DispatcherCallback */
	char *DispatchedName() { return "TDataAscii"; }

	/* Build index */
	virtual void BuildIndex();

	TD_Status ReadRec(RecId id, int numRecs, void *buf);
	TD_Status ReadRecAsync(TDataRequest *req, RecId id,
                               int numRecs, void *buf);
};

#endif
