// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   Universal Data Reader Function Types and Parameters
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#include  "udParam.h"

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
udParam::udParam()
{
    attr      = NULL;
    attrfunc  = (AttrFunc) NULL;

    flat_indx = 0;
    dst_off   = 0;

    buf_pos   = 0;
    subparam  = NULL;

    use_slide = 1;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
udParam::~udParam()
{
    if (subparam)
        subparam->empty(1);
    delete subparam;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
udParam *udParam::dup()
{
    udParam *that = new udParam();

    that->attr     = attr;
    that->attrfunc = attrfunc;

    that->flat_indx = flat_indx;
    that->dst_off   = dst_off;
    that->buf_pos   = buf_pos;

    that->use_slide = use_slide;

    // Don't copy subparam, this is only for "leaf" parameters.

    return that;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Stream printing (prints state, for debugging)
ostream& operator<< (ostream& out, const udParam& param)
{
    out << "Param ";

    if (param.attr)
        out << param.attr->name();
    else
        out << "NULL";

    if (param.subparam)
        out << " with subparams " << *(param.subparam) << endl;
    else
        out << " with no subparams.\n";

    out << "flat index is " << param.flat_indx << endl;
    out << "dest offset is " << param.dst_off << endl;
    out << "buf_pos is " << param.buf_pos << endl;

    return out;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int  ParamStk::push(udParam *x)
{
    if (length >= data_size) {

        int len = data_size;
        data_size += data_size;
        udParam **tmp_data = data;
        data = new udParam* [data_size];
        memcpy(data, tmp_data, len * sizeof(udParam*));
        delete [] tmp_data;
    }

    data[length++] = x;

    return(length-1);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Stream printing
ostream& operator<< (ostream& out, const ParamStk& plist)
{
    for (int j=0; j < plist.length; j++)
        out << ", " << *(plist.data[j]);

    return(out);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
