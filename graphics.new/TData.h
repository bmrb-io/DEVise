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
   $Id$

   $Log$
   Revision 1.17  1997/10/10 21:13:45  liping
   The interface between TData and BufMgr and the interface between BufMgr and
   QueryProc were changed
   The new interface carries the information of 1. LowId 2. HighId 3. AttrName
           4. Granularity in the structure "Range"

   Revision 1.16  1997/07/03 01:53:45  liping
   changed query interface to TData from RecId to double

   Revision 1.15  1997/04/21 22:57:18  guangshu
   Added function GetTableName.

   Revision 1.14  1997/01/11 20:56:20  jussi
   Added nextChunk to TData request structure.

   Revision 1.13  1996/12/18 19:34:04  jussi
   Fixed minor bugs in ReadRecAsync(). Added FlushDataPipe().

   Revision 1.12  1996/12/18 15:29:45  jussi
   Added TDataRequest constructor. Added two methods.

   Revision 1.11  1996/12/03 20:32:09  jussi
   Improved Init/Get/Done interface.

   Revision 1.10  1996/11/23 21:14:22  jussi
   Removed failing support for variable-sized records.

   Revision 1.9  1996/11/22 20:41:06  flisakow
   Made variants of the TDataAscii classes for sequential access,
   which build no indexes.

   ReadRec() method now returns a status instead of void for every
   class that has the method.

   Revision 1.8  1996/11/12 17:21:24  jussi
   Put back the 'virtual' keyword from the int RecSize() declaration.
   It had somehow disappeared, with the result that GData was never
   cached in memory! In QueryProcFull.c, tdata->RecSize() was supposed
   to return the GData recsize but instead returned zero because the
   method was not declared virtual.

   Revision 1.7  1996/10/04 17:24:15  wenger
   Moved handling of indices from TDataAscii and TDataBinary to new
   FileIndex class.

   Revision 1.6  1996/08/04 21:59:52  beyer
   Added UpdateLinks that allow one view to be told to update by another view.
   Changed TData so that all TData's have a DataSource (for UpdateLinks).
   Changed all of the subclasses of TData to conform.
   A RecFile is now a DataSource.
   Changed the stats buffers in ViewGraph to be DataSources.

   Revision 1.5  1996/07/23 20:13:07  wenger
   Preliminary version of code to save TData (schema(s) and data) to a file.

   Revision 1.4  1996/06/12 14:56:31  wenger
   Added GUI and some code for saving data to templates; added preliminary
   graphical display of TDatas; you now have the option of closing a session
   in template mode without merging the template into the main data catalog;
   removed some unnecessary interdependencies among include files; updated
   the dependencies for Sun, Solaris, and HP; removed never-accessed code in
   ParseAPI.C.

   Revision 1.3  1996/01/13 21:08:47  jussi
   Added copyright notice.

   Revision 1.2  1995/09/05 22:15:45  jussi
   Added CVS header.
   */

/* Textual data virtual base class */

#ifndef TData_h
#define TData_h

#include "DeviseTypes.h"
#include "RecId.h"
#include "DataSource.h"

// A simple Status for TData
enum TD_Status {
    TD_OK = 0,
    TD_EOF,
    TD_FAIL
};

class AttrList;

class ReleaseMemoryCallback {
  public:
    virtual void ReleaseMemory(MemMgr::PageType type,
                               char *buf, int pages) = 0;
};

class TDataRequest {
  public:
    TDataRequest() {
        nextVal = endVal = 0;
        relcb = NULL;
        iohandle = lastChunkBytes = 0;
        pipeFlushed = true;
        lastChunk = lastOrigChunk = NULL;
    }

    Boolean IsDirectIO() { return (iohandle == 0); }
    Boolean IsActiveIO() { return IsDirectIO() || !pipeFlushed; }

    Coord nextVal;                       // next record to return in GetRecs()
    Coord endVal;                        // where current GetRecs() should end
    Coord granularity;			// requested granularity
    ReleaseMemoryCallback *relcb;       // callback to release pipe memory
    int iohandle;                       // handle for data source I/O
    Boolean pipeFlushed;                // true if pipe flushed of data

    streampos_t nextChunk;              // offset of next data chunk
    char *lastChunk;                    // beginning of unused pipe data chunk
    char *lastOrigChunk;                // beginning of pipe data chunk
    int lastChunkBytes;                 // size of pipe data chunk

    char *AttrName;			// on which column is the request
};

class TData {
  public:

    // creates a new data source based upon the parameters
    TData(char* name, char* type, char* param, int recSize);

    // uses an existing data source
    TData(DataSource* data_source = NULL);

    virtual ~TData();

    /**** MetaData about TData ****/
    /* Return attribute info */
    virtual AttrList *GetAttrList() = 0;

    /* Return # of dimensions and the size of each dimension,
       or -1 if unknown */
    virtual int Dimensions(int *sizeDimension) = 0;

    /* Return record size, or -1 if variable record size */
    virtual int RecSize() { return _recSize; }

    /* Return page size of TData, or -1 if no paging structure */
    virtual int PageSize() { return -1; }

    /* Return true if TData deletes records from the beginning
       due to limited disk/memory buffer. */
    virtual Boolean HasDeletion() { return false; }

    /* Return true if TData appends records */
    virtual Boolean HasAppend() { return false; }

    /* Use this to get user defined attributes.
       We reserve attributes 0-SysAttrNum for internal use.
       A -1 is returned for none-existing attrNum*/
    virtual int UserAttr(int attrNum){ return -1; }

    // if the data source is not ok or it has changed versions,
    // then DataSourceChanged will be called.
    // returns false if there is a problem with the data source
    void CheckDataSource() {
	if( !_data->IsOk() || _data->Version() != _version ) {
	    InvalidateTData();
	}
    }

    // The data source that this tdata is defined upon has changed.
    // The tdata should clean in-memory structures like indicies, etc.
    // The derived method should also be sure to call this one which
    // cleans the query processor
    virtual void InvalidateTData();

    /* Get name */
    virtual char *GetName() { return _name; }
    virtual char *GetTableName() { return _name; }

    /* convert RecId into index */
    virtual void GetIndex(RecId id, int *&indices) = 0;

    virtual Boolean WriteIndex(int fd) { return false; }
    virtual Boolean ReadIndex(int fd) { return false; }

    /**** Getting record Id's ****/

    /* Get RecId of 1st available record, return true if available */
    virtual Boolean HeadID(RecId &recId) = 0;

    /* Get RecId of last record, return true if available */
    virtual Boolean LastID(RecId &recId) = 0;

    /**** Getting Records ****/

    typedef TDataRequest *TDHandle;

    /**************************************************************
      Init getting records.
    ***************************************************************
	NOTE: interval->NumRecs is now used by caller to say the 
	      MAXIMAL # of records it wants
    ***************************************************************/
    virtual TDHandle InitGetRecs(// double lowVal, double highVal,
				 Interval *interval,
				 int &bytesleft,
                                 Boolean asyncAllowed = false,
                                 ReleaseMemoryCallback *callback = NULL
				 ) = 0;

    /**************************************************************
      Get next batch of records, as much as fits into buffer. 
      Return false if no more.
      input:
      buf: where to start putting records.
      bufSize: size of buffer being supplied.
      output:
      startRid : starting record ID of this batch 
      numRecs: number of records.
      dataSize: # of bytes taken up by data.
      **************************************************************
	startRid and numRecs are now recorded in range
      *************************************************************/
    virtual Boolean GetRecs(TDHandle handle, void *buf, int bufSize,
                            Interval *interval, int &dataSize) = 0;

    virtual void DoneGetRecs(TDHandle handle) = 0;

    /* For writing records. Default: not implemented. */
    virtual void WriteRecs(RecId startId, int numRecs, void *buf);

    /* get the time file is modified. We only require that
       files modified later has time > files modified earlier. */
    virtual int GetModTime() = 0;

    /* Do a checkpoint */
    virtual void Checkpoint() = 0;

    /* Save the TData to a TData file. */
    DevStatus Save(char *filename);

    DataSource* GetDataSource() { return _data; }

  protected:

    char *_name;                        // name of data stream
    char *_type;                        // type of data stream
    char *_param;                       // parameters of data stream
    int _recSize;                       // size of record

    DataSource* _data;                  // data source
    int _version;                       // last _data->Version()

    char* MakeCacheFileName(char *name, char *type);
    
  private:

    DevStatus WriteHeader(int fd);
    DevStatus WriteLogSchema(int fd);
    DevStatus WritePhysSchema(int fd);
    DevStatus WriteData(int fd);

};

#endif
