// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Universal Data Reader Definition
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#ifndef    _UNIDATA_H
#define    _UNIDATA_H

#include  <iostream.h>
#include  <unistd.h>

#include  "RecId.h"

#include  "SlideBuf.h"
#include  "Schema.h"
#include  "udParam.h"
#include  "PerlFrag.h"

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
#ifdef  NO_FUNCNAME
  // Defense in case you're unfortunate enough to not have g++
#    define __FUNCTION__  "<Unknown Function>"
#endif

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

class udIndex;
class UniData;

typedef int (UniData::*GetRecFunc)(char *dst, off_t *off);

typedef int (*ChkCnvtFunc)(char *cnvt_cmmd);

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

// When to check converter's for validity.

enum secure_t {
    UD_CHK_SUSPICIOUS=0,    // only if suspicious (default).
    UD_CHK_ALWAYS,          // always
    UD_CHK_NEVER            // never check.
};

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

class UniData {

  friend class Attr;
  friend class Schema;

  protected:
    char     *_name;        // Name of UniData
    Schema   *_schema;      // Schema for this data.

    UD_Status _status;      // The current status of the UniData.

    int       _ownfd;       // Did we open _datafd ourself?
    int       _datafd;      // File descriptor for the data file

    char     *_in_fifo;     // For converters: the paths to the
    char     *_out_fifo;    //                 FIFOs (named pipes).

    SlideBuf *_slbuf;       // A sliding buffer.

    off_t     _data_init;   // The data starts here.

    RecId     _next_rid;    // The next rid to use.
    RecId     _last_rid;    // The highest rid used so far.

    ParamStk *_params;      // An array of parameters, one per attr
                            // or possibley multiple ones for each
                            // attr "container".

    ParamStk *_sys_params;  // An array of "system" parameters, for
                            // misc work to be done after a record
                            // is read in.  Can be 0, 1 or more 
                            // params generated for each attribute.  

    udIndex  *_index;       // An index of offsets

    GetRecFunc _getrec;     // A function that knows how to get the record. 

    secure_t   _cnvt_security;  // How secure do we want to be?

    ChkCnvtFunc _chkcnvt;   // A function that will check a converter
                            // string for validity before UniData passes
                            // it to the shell for execution.

    PerlInterpreter *_perl; // Perl interpreter, if perl is used.

      // Set everything to its initial value.
    void init();

      // Build parameters (only need index and pointer to attr).
    int build_params(Attr *attr, ParamStk *params, int& indx);

      // setup the parameters we'll need.
    int  get_params();

      // A function to figure out the converter (if any)
      // and do the setup for it.
    int  setup_converter(char *filename=NULL);

      // Prepare for the use of perl, if it's needed.
    int  setup_perl();

      // Set/Get the perl variables for this record.
    void set_vars(char *rcrd);
    void grab_vars(char *rcrd);

    // o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
    // Specific functions to handle each type of attribute
    // (Each of type AttrFunc.)
    // o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

      // This one splits on whitespace or nothing.
    int Split_White(char *dst, char *src, udParam *ud);

      // Have positions
    int Split_Position(char *dst, char *src, udParam *ud);

      // Have seperators
    int Split_Seper(char *dst, char *src, udParam *ud);

    int NullCopy(char *dst, char *src, udParam *ud);
    int BinCopy_Native(char *dst, char *src, udParam *ud);

    int TxtCopy_Int(char *dst, char *src, udParam *ud);
    int TxtCopy_Float(char *dst, char *src, udParam *ud);
    int TxtCopy_Double(char *dst, char *src, udParam *ud);
    int TxtCopy_String(char *dst, char *src, udParam *ud);
    int TxtCopy_DateTime(char *dst, char *src, udParam *ud);
    int TxtCopy_UnixTime(char *dst, char *src, udParam *ud);

    int ReaderCall(char *dst, char *src, udParam *ud);
    int ValueCall(char *dst, char *src, udParam *ud);
    int FilterCall(char *dst, char *src, udParam *ud);

    // o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

      // Private versions for specific record-locating methods.
      // (need to add more later).
    int getRec_recsze(char *buff, off_t *offset);
    int getRec_delimit(char *buff, off_t *offset);

    // o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

      // Run the system parameters, which operate and the
      // data that the normal parameters read in.
    int run_SysParams(char *dst);

    // o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

  public:
      // If schema is NULL, it's at the begining of the data.
    UniData(char *datafile, char *schemafile=NULL);
    UniData(int datafd, Schema *schema=NULL);

   ~UniData();

    void  SetName(char *name);
    char *GetName() { return _name; }

      // Don't modify this one, it's just for looking at.
    Schema *schema() { return _schema; }

      // Get the parameters
    ParamStk *GetParams() { return _params; }

      // Do we have an Rid field?
      // (get one for "free" for data srcs with fixed-length records)
    int  haveRid();

      // Indicates you want an Rid interface,
      // builds an internal index of record offsets if necessary.
      // Can switch at any time, but turning it on causes a reset()
      // to the beginning of file.
    void useRid(int uR=1);


      // Return the current status.
    UD_Status status() { return _status; }

      // Are we at EOF?
    int isEOF() { return(_status == UD_EOF); }

      // a failure condition?
    int isFail() { return(_status == UD_FAIL); }

      // or is everything fine?
    int isOk() { return(_status == UD_OK); }

      // Find the amount of room needed to store a record.
    int recSze() { return _schema->recSze(); }


      // Get a single record (the next available)
    UD_Status getRec(char *buff, off_t *offset=NULL);

      // Get a single record, with random access.
    UD_Status getRndRec(char *buff, off_t offset);

      // Get a page of records, returns the number obtained.
      // Trys to fill the buffer, but won't read
      // more than maxRecs records in any case.
    UD_Status getRecs(char *buff, int bufsze, int& numRecs,
                      off_t *offsets=NULL);

      // Moves the read pointer.  (make private?)
    int Seek(off_t offset);

      // Seek to beginning of data in this file;
      // also causes perl "global" sections to be re-run.
    int reset();

    // o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
    // Rid Interface

      // Get a single record
    UD_Status getRidRec(char *buff, RecId& rid);

      // Get a few, starting at startRid
    UD_Status getRidRecs(char *buff, int bufsze, int& numRecs,
                         RecId& startRid);

      // Translate an Rid into the offset to be used.
    UD_Status translateRid(RecId& Rid, off_t& offset);

    // o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

      // Setup a callback to be used when UniData wants to check
      // if a particular converter is safe to pass to a shell.
    void check_converters( secure_t sec_lvl,
                        ChkCnvtFunc chk= (ChkCnvtFunc) NULL) {
        _cnvt_security = sec_lvl;
        _chkcnvt = chk;
    }

      // Stream printing (prints state, for debugging)
    friend ostream& operator<< (ostream& out, const UniData& ud);
};

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#endif  // _UNIDATA_H
