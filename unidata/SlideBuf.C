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

#include  <stdlib.h>
#include  <string.h>
#include  <errno.h>
#include  <fcntl.h>
#include  <sys/stat.h>

#include  "Config.h"

#include  "SlideBuf.h"
#include  "Comment.h"


// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
SlideBuf::SlideBuf(int datafd)
{
    _datafd = datafd;
    _data_eof = 0;

    _buf[0] = '\0';
    _buf[UD_BUFSZE] = '\0';

    _buf_init = 0;
    _buf_end  = 0;

    _buf_off  = 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
SlideBuf::~SlideBuf()
{
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// read more data into the buffer.
int SlideBuf::slurp(UD_Status& status, int init)
{
      // if !init, copy any remaining data to the front of the buffer.
    if (init) {
        _buf_init = 0;
        _buf_end  = 0;
    } else {
        int mvcnt = _buf_end - _buf_init; 
        if (mvcnt > 0)
            memmove(_buf,&(_buf[_buf_init]), mvcnt);
        _buf_off += _buf_init;
        _buf_init = 0;
        _buf_end  = mvcnt;
    }

    int n;
    if (!_data_eof)
        n = read(_datafd, &(_buf[_buf_end]), UD_BUFSZE-_buf_end); 
    else
        n = 0;

    if (n > 0) {
        _buf_end += n;
        _buf[_buf_end] = '\0';
    } else if (n < 0)
        status = UD_FAIL;
    else
        _data_eof = 1;

    if ((_buf_end == 0) && _data_eof)
        status = UD_EOF;

    return !_data_eof;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// returns the amount of (useful) data is in the buffer,
// if called with n!=0, ensures at least that much data is available.
int SlideBuf::room(UD_Status& status, int n)
{
    if (n) {
        int rm = _buf_end - _buf_init;

        if (rm < n) {
            slurp(status);
            rm = _buf_end - _buf_init;
        }

        return rm;

    } else
        return room(status,UD_BUFSLACK);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Dump the current contents out to this file descriptor
// and empty the buffer.
int  SlideBuf::dump(int wrfd)
{
    write(wrfd, &(_buf[_buf_init]), (_buf_end - _buf_init));
    empty();

    return 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Moves the read pointer.  (make private?)
int SlideBuf::Seek(UD_Status& status, off_t offset)
{
    _data_eof = 0;    // No longer valid necessarilly

    // Check if we're fortunate enough to still have
    // the data in the buffer.

    if (   (offset >= _buf_off)
        && (offset < (_buf_off + _buf_end)) ) {

        _buf_init = (offset - _buf_off);
        // NYI - Need to check if we need to slurp in some more data?

    } else {
        _buf_off = offset;
        if (lseek(_datafd,offset,SEEK_SET) == -1)
            status = UD_FAIL;
        else
            slurp(status,1);
    }

    return 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Stream printing (prints state, for debugging)
ostream& operator<< (ostream& out, const SlideBuf& sb)
{
    out << " -- SlideBuf Internals --\n";

    out << " -----------------------\n\n";

    out << "Buf Offset: " << sb._buf_off << endl;

    out << "Buf Init: " << sb._buf_init << endl;
    out << "Buf End: " << sb._buf_end << endl;

    out << "fd @ EOF: " << sb._data_eof << endl;

    out << " -----------------------\n\n";

    return out;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int SlideBuf::SkipWhite(UD_Status& status, char *white)
{
    int n, done=0, tot=0;

    if (! white)
        return 0;

    do {
        n = strspn(&(_buf[_buf_init]), white);
        _buf_init += n;
        tot += n;

        if (_buf_init == _buf_end)
            slurp(status);
        else
            done = 1;

    } while (!done && (status == UD_OK));

    return tot;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int SlideBuf::SkipCmmt(UD_Status& status, char *init, char *end)
{
    if (!end)
        end = "\n";

    int li,le;
    li = strlen(init);

    if (_buf_end - _buf_init < li)
        slurp(status);

    if (strncmp(&(_buf[_buf_init]), init, li) != 0)
        return 0;

    _buf_init += li;
    le = strlen(end);

    int done = 0;
    char *ptr = NULL;

    do {
        ptr = strchr(&(_buf[_buf_init]), *end);

        if (ptr) {
            _buf_init = ptr - _buf;

            if (_buf_end - _buf_init < le)
                slurp(status);

            if (_buf_end - _buf_init >= le)
                if (strncmp(&(_buf[_buf_init]), end, le) == 0) {
                    _buf_init += le;
                    done = 1;
                }

        } else {
            _buf_init = _buf_end;
            slurp(status);
        }

    } while (!done && (status == UD_OK));


    return 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int SlideBuf::SkipCmmts(UD_Status& status, CmmtStk *cmmts, char *white)
{
    int got_cmmt;
    off_t pos;

    do {
        got_cmmt = 0;
        pos = getoff();

        SkipWhite(status,white);

        for (int j=0; j < cmmts->nCmmts(); j++) {
            Comment *cmmt = cmmts->ith(j);
            got_cmmt |= SkipCmmt(status,cmmt->init,cmmt->end);
        }

    } while (got_cmmt);

      // Put this whitespace back (in case we're using positioning).
    Seek(status,pos);

    return 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Locate the delimiter, and position the buffer at the start of the
// next record.
int SlideBuf::SkipDelimit(UD_Status& status, char *delim)
{
    if (!delim)
        delim = "\n";

    int len = strlen(delim);
    int found = 0;

    if (len == 1) {
        do {
            char *rec_end = strchr(&(_buf[_buf_init]), *delim);

            if (rec_end) {
                _buf_init = (rec_end - _buf) + 1;
                found = 1;
            } else {
                _buf_init = _buf_end;
                slurp(status);
            }

        } while (!found);
    } else {
        do {
            char *rec_end = strstr(&(_buf[_buf_init]), delim);

            if (rec_end) {
                _buf_init = (rec_end - _buf) + len;
                found = 1;
            } else {
                _buf_init = _buf_end;
                slurp(status);
            }

        } while (!found);
    }

    return 0;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
