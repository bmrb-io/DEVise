// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o


#include  "SlideRead.h"
#include  "SlideBuf.h"

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

// This simple function uses a SlideBuf to provide a read interface
// for flex.

SlideBuf *yyslbuf = NULL;

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

int SlideRead(char *buf, int max_size)
{
    int result = 0;
    UD_Status stat;

    if (max_size >= UD_BUFSZE) {
        result = yyslbuf->room(stat,UD_BUFSLACK);
        memcpy(buf,yyslbuf->getcur(),result);
        yyslbuf->empty();
    } else {
        result = yyslbuf->room(stat,max_size);
        if (result >= max_size)
            result = max_size;
        memcpy(buf,yyslbuf->getcur(),result);
        yyslbuf->set_init(yyslbuf->get_init() + result);
    }

    return result;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
