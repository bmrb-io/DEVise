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
  Revision 1.23  1998/10/13 19:40:46  wenger
  Added SetAttrs() function to TData and its subclasses to allow Liping to
  push projection down to the DTE.

  Revision 1.22  1997/12/23 23:35:40  liping
  Changed internal structure of BufMgrFull and classes it called
  The buffer manager is now able to accept queries on any attribute from the
          Query Processor
  The buffer manager is also able to issue queries on various attributes to DTE
  Instead of keeping an in memory list for each T/GData, the buffer manager keeps
          a list for each (T/GData, AttrName, Granularity) combination
  The class Range was replaced by Interval

  Revision 1.21  1997/10/10 21:13:48  liping
  The interface between TData and BufMgr and the interface between BufMgr and
  QueryProc were changed
  The new interface carries the information of 1. LowId 2. HighId 3. AttrName
          4. Granularity in the structure "Range"

  Revision 1.20  1997/07/03 01:53:48  liping
  changed query interface to TData from RecId to double

  Revision 1.19  1997/01/11 20:56:06  jussi
  Removed references to _currPos.

  Revision 1.18  1997/01/09 18:51:59  jussi
  Added controlling of live data update frequency.

  Revision 1.17  1996/12/18 19:34:07  jussi
  Fixed minor bugs in ReadRecAsync(). Added FlushDataPipe().

  Revision 1.16  1996/12/18 15:30:45  jussi
  Added support for concurrent I/O.

  Revision 1.15  1996/12/03 20:31:36  jussi
  Updated to reflect new TData interface.

  Revision 1.14  1996/11/23 21:14:24  jussi
  Removed failing support for variable-sized records.

  Revision 1.13  1996/11/22 20:41:11  flisakow
  Made variants of the TDataAscii classes for sequential access,
  which build no indexes.

  ReadRec() method now returns a status instead of void for every
  class that has the method.

  Revision 1.12  1996/10/04 17:24:17  wenger
  Moved handling of indices from TDataAscii and TDataBinary to new
  FileIndex class.

  Revision 1.11  1996/08/04 21:59:54  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

  Revision 1.10  1996/07/03 23:13:47  jussi
  Added call to _data->Close() in destructor. Renamed
  _fileOkay to _fileOpen which is more accurate.

  Revision 1.9  1996/07/01 19:28:09  jussi
  Added support for typed data sources (WWW and UNIXFILE). Renamed
  'cache' references to 'index' (cache file is really an index).
  Added support for asynchronous interface to data sources.

  Revision 1.8  1996/06/27 18:12:42  wenger
  Re-integrated most of the attribute projection code (most importantly,
  all of the TData code) into the main code base (reduced the number of
  modules used only in attribute projection).

  Revision 1.7  1996/06/27 15:49:34  jussi
  TDataAscii and TDataBinary now recognize when a file has been deleted,
  shrunk, or has increased in size. The query processor is asked to
  re-issue relevant queries when such events occur.

  Revision 1.6  1996/06/12 14:56:35  wenger
  Added GUI and some code for saving data to templates; added preliminary
  graphical display of TDatas; you now have the option of closing a session
  in template mode without merging the template into the main data catalog;
  removed some unnecessary interdependencies among include files; updated
  the dependencies for Sun, Solaris, and HP; removed never-accessed code in
  ParseAPI.C.

  Revision 1.5  1996/05/22 17:52:18  wenger
  Extended DataSource subclasses to handle tape data; changed TDataAscii
  and TDataBinary classes to use new DataSource subclasses to hide the
  differences between tape and disk files.

  Revision 1.4  1996/05/07 16:44:20  jussi
  Cache file name now based on file alias (TData name). Added recPos
  parameter to Decode() function call. Added support for a simple
  index which is needed when streams are split into multiple
  sub-streams (via matching values defined in the schema).

  Revision 1.3  1996/05/05 03:08:29  jussi
  Added support for composite attributes. Also added tape drive
  support.

  Revision 1.2  1996/01/25 20:22:52  jussi
  Improved support for data files that grow while visualization
  is being performed.

  Revision 1.1  1996/01/23 20:54:50  jussi
  Initial revision.
*/

/* Textual data virtual base class */

#ifndef TDataBinary_h
#define TDataBinary_h

#include <stdio.h>

#include "DeviseTypes.h"
#include "TData.h"
#include "RecId.h"
#include "DataSource.h"

class TDataBinary: public TData {
public:
  TDataBinary(char *name, char *type, char *param,
              int recSize, int physRecSize);

  virtual ~TDataBinary();

  /**** MetaData about TDataBinary ****/
  virtual AttrList *GetAttrList(){ return NULL;}

  virtual Boolean SetAttrs(const AttrList &attrs) { return false; }

  // Return record size, or -1 if variable record size
  virtual int RecSize() { return _recSize; }

  // Return page size of TDataBinary, or -1 if no paging structure
    virtual int PageSize() { return -1; }

  // Return true if TDataBinary deletes records from the beginning
  // due to limited disk/memory buffer.
  virtual Boolean HasDeletion() { return false; }

  // Return true if TDataBinary appends records
  virtual Boolean HasAppend() { return true; }

  // Use this to get user defined attributes.
  // We reserve attributes 0-SysAttrNum for internal use.
  // A -1 is returned for none-existing attrNum
  virtual int UserAttr(int attrNum) { return -1; }

  /* writing a record. For TDataBinary, the new record
     is appended to the file (startRid not examined), numRecs ==1, 
     and buf points to a string to be written to disk. */
  virtual void WriteRecs(RecId startId, int numRecs, void *buf);

  /* Write a line into the file, but don't make it into a record */
  void WriteLine(void *line);

protected:
  /* Copy record into buffer. Return false if invalid record. */
  virtual Boolean Decode(void *recordBuf, int recPos, char *line) = 0;

private:
  /* From DispatcherCallback */
  char *DispatchedName() { return "TDataBinary"; }

  /* Build index */
  virtual void BuildIndex();

  TD_Status ReadRec(RecId id, int numRecs, void *buf);
  TD_Status ReadRecAsync(TDataRequest *req, RecId id,
                         int numRecs, void *buf);

  char *_file;                    // name of (cache) file
  int _physRecSize;               // physical record size
};

#endif
