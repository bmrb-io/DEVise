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

#include <stdlib.h>

#include "IntStack.h"

        // The stack can grow, but start at a reasonable size
#define    DEFAULT_SIZE    8

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
int  IntStack::push(int x)
{
    if (length >= data_size) {

        if (data_size == 0) {
            data_size = DEFAULT_SIZE;
            data = new int[data_size];
        } else {

            int len = data_size;
            data_size += data_size;
            int *tmp_data = data;
            data = new int[data_size];
            memcpy(data, tmp_data, len * sizeof(int));
            delete [] tmp_data;
        }
    }

    data[length++] = x;

    return(length-1);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
void IntStack::insert_at(int pos, int item)
{
      // Defensive programming :-)
    if (pos > length)
        return;

      // This ensures there is room available
      // (As a side-effect, it increments the length)
    push(0);

    for (int i = length-1; i > pos; i--)
        data[i] = data[i-1];

    data[pos] = item;
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//  copies contents of stack stk to this one
void IntStack::copy(IntStack& stk)
{
    delete [] data;
    length = stk.length;
    data_size = stk.data_size;
    data = new int[data_size];
    for (int i = 0; i < length; i++)
        data[i] = stk.data[i];
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
// Stream printing
ostream& operator<< (ostream& out, const IntStack& ilist)
{
    if (ilist.length)
        out << ilist.data[0];
 
    for (int j=1; j < ilist.length; j++)
        out << ", " << ilist.data[j];

    return(out);
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
