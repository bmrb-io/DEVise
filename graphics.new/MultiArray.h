/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2  1995/09/05 22:15:04  jussi
  Added CVS header.
*/

/* Multiple arrays of pointers to void */

#ifndef MultiArray_h
#define MultiArray_h

#include <stdio.h>
#include "DeviseTypes.h"
#include "VoidArray.h"
#include "Exit.h"

class MultiArray {
public:
  /* constructor */
  MultiArray(int numArrays, int arraySize);

  /* Clear so there is no element left */
  void Clear();

  /* Iterator */
  int InitIterator(int listNum) {
    CheckListNum(listNum);
    _arrays[listNum]->InitIterator();
    return 0;
  }
  Boolean MoreElements(int listNum) {
    return (_arrays[listNum]->More());
  }
  void *NextElement(int listNum) {
    return (_arrays[listNum]->Next());
  }

  /* indexing into the list */
  void *GetElement(int listNum, int pos) {
    CheckListNum(listNum);
    return _arrays[listNum]->Get(pos);
  }
  
  /* size of each array */
  int ArraySize(int arrayNum) {
    CheckListNum(arrayNum);
    return _arrays[arrayNum]->Size();
  }

  /* # of arrays */
  int NumArrays() { return _numArrays; }

  /* Move elements within lists */
  void Move(int fromList, int fromPos, int toList, int toPos) {
    CheckListNum(fromList);
    CheckListNum(toList);
    void *elem = _arrays[fromList]->Delete(fromPos);
    _arrays[toList]->Insert(elem,toPos);
  }

  /* Insert into a list */
  void Insert(int list, int pos, void *data) {
    CheckListNum(list);
    _arrays[list]->Insert(data, pos);
  }
  
  /* Delete from a list. return the deleted value.
     If pos == 0, make deletion act like a queue .
     If pos == ListSize -1, acts like a stack 
     If pos is somewhere in the middle, move the last element to
     cover its place.
     */
  void *Delete(int listNum, int pos) {
    CheckListNum(listNum);
    return _arrays[listNum]->Delete(pos);
  }
  
private:
  void CheckListNum(int listNum) {
    if (listNum < 0 || listNum >= _numArrays) {
      fprintf(stderr,"MultiArray : invalid list %d\n", listNum);
      Exit::DoExit(2);
    }
  }

  int _numArrays;
  int _maxSize;		/* max size of each array */
  VoidArray **_arrays;
};

#endif
