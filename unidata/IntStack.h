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

#ifndef _INTSTACK_H_
#define _INTSTACK_H_

#include <iostream.h>
#include <stdlib.h>

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

class IntStack {

  public:
    int *data;
    int  length;
    int  data_size;

    IntStack() {
                 data = NULL;
                 length = 0;
                 data_size = 0;
               }

    IntStack(int sze) {
                        data = new int[sze];
                        length = 0;
                        data_size = sze;
                      }

   ~IntStack() { delete [] data; }

    int  count() { return(length); }
    int  is_empty() { return(length == 0); }

      // push() returns the position
    int  push(int item);

      // Warning, insert_at is not particularly efficient
    void insert_at(int pos, int item);

    void swap(int pos1, int pos2) {
        int tmp    = data[pos1];
        data[pos1] = data[pos2];
        data[pos2] = tmp;
    }

    int  get_ith(int i) {
        if (i < length)
            return(data[i]);
        return(0);
    }

    void set_ith(int i, int val) {
        if (i < length)
            data[i] = val;
    }

      // Delete an element based on its position, this function must
      // maintain the "stack" concept, sliding the other values down.
    void del_ith(int i) {
        for (; i < length-1; i++) 
            data[i] = data[i+1];
        length--;
    }

        // Pull out the last element
    int pop() {
        if (length > 0) {
            length--;
            return(data[length]);
        } else {
              // Not the best, need to be careful not to exceed
            return(0);
        }
    }

        // Peek at the last element
    int top() {
        if (length > 0)
            return(data[length-1]);
        return(0);
    }

    void copy(IntStack&);

      // Empty out the stack
    void empty() {
        length = 0;
    }

    int contains(int elemnt) {
        for (int j=0; j < length; j++)
            if (data[j] == elemnt)
                return(1);
        return(0);
    }

      // Find the first occurance.
    int find(int elemnt) {
        for (int j=0; j < length; j++)
            if (data[j] == elemnt)
                return(j);
        return(-1);
    }

      // Delete an element based on its value, this function must
      // maintain the "stack" concept, sliding the other values down.
      // The return value indicates if the value was found.      
    int  del_val(int val) {
        int did_it = 0;

        for (int j=0; j < length; j++)
            if (data[j] == val) {
                did_it = 1;
                for (; j < length-1; j++)
                    data[j] = data[j+1];
                length--;
                break;
            } 

        return(did_it);
    }

    int  set_val(int oldval, int newval) {
        int did_it = 0;

        for (int j=0; j < length; j++)
            if (data[j] == oldval) {
                did_it = 1;
                data[j] = newval;
            } 

        return(did_it);
    }

      // Stream printing
    friend ostream& operator<< (ostream& out, const IntStack& ilist);
};

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

#endif // _INTSTACK_H_
