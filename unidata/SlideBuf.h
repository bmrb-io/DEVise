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

#ifndef    _SLIDEBUF_H
#define    _SLIDEBUF_H

#include  <iostream.h>
#include  <unistd.h>

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
#ifdef  NO_FUNCNAME
  // Defense in case you're unfortunate enough to not have g++
#    define __FUNCTION__  "<Unknown Function>"
#endif

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

            // Any single field must fit in this buffer (as text).
#define   UD_BUFSZE   16284

            // Get more stuff in the buffer when we get this low?
#define   UD_BUFSLACK   256

            // We might need a big piece soon.
#define   UD_BUFCHUNK  2048

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

enum UD_Status {
    UD_OK = 0,
    UD_EOF,
    UD_FAIL
};


class CmmtStk;

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

class SlideBuf {

  friend class UniData;

  protected:

    int       _datafd;      // File descriptor for the data file
    int       _data_eof;    // File descriptor at EOF?

    char      _buf[UD_BUFSZE+1]; // A buffer that is used while
                                 // reading the record. (plus sentinel)

    off_t     _buf_off;     // The offset corresponding to buff[0].

    int       _buf_init;    // The start of the buffer. 
    int       _buf_end;     // The end of the buffer. 

    // o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

  public:
    SlideBuf(int datafd = -1);
   ~SlideBuf();

    // o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
    // Small functions to skip over
    // whitespace and comments.
    // o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

    int SkipWhite(UD_Status& status, char *white);

        // end defaults to '\n'
    int SkipCmmt(UD_Status& status, char *init, char *end=NULL);
    int SkipCmmts(UD_Status& status, CmmtStk *cmmts, char *white=NULL);

    int SkipDelimit(UD_Status& status, char *delim);

    // o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o


    int  get_init() { return _buf_init; }
    void set_init(int init) { _buf_init = init; }
    void set_init(off_t init) { _buf_init = init - _buf_off; }

      // set _buf_init to point here (within _buf!)
    void set_init(char *bptr) {
                                if (_buf_init || _buf_end)
                                  _buf_init = bptr - _buf;
                              }

      // empty out the buffer
    void empty() { _buf_init = _buf_end; }

      // Dump the current contents out to this file descriptor
      // and empty the buffer.
    int  dump(int wrfd);

      // returns the file offset corresponding to the current _buf_init
    off_t getoff() { return( _buf_off + _buf_init ); }

      // returns the file offset corresponding to the char*
    off_t getoff(char *bptr) { return( _buf_off + (bptr - _buf)); }
    void  setoff(off_t off) { _buf_off = off; }

    char *getcur(int pos=0) { 
		return &(_buf[_buf_init + pos]); 
	}

      // returns a pointer to the pos position, when myzero is 
      // considered to be the "zero" position.
    char *getpos(off_t myzero, int pos=0) {
        return &(_buf[ (myzero - _buf_off) + pos ]);
    }

    void setzero(off_t& zero) {
        zero = _buf_off + _buf_init;
    }

    void setzero(off_t& zero, off_t myzero, int pos=0) {
        zero = myzero + pos;
    }

      // read more data into the buffer.
      // Use init to discard the buffer's current contents.
    int slurp(UD_Status& status, int init=0);

      // returns the amount of (useful) data is in the buffer,
      // if called with n!=0, ensures at least that much data is available.
    int room(UD_Status& status, int n=0);

      // Moves the read pointer.
    int Seek(UD_Status& status, off_t offset);

      // Stream printing (prints state, for debugging)
    friend ostream& operator<< (ostream& out, const SlideBuf& sb);
};

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#endif  // _SLIDEBUF_H
