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

#ifndef GdataStat_h
#define GdataStat_h

#include <stdio.h>

#include "HashTable.h"
#include "BasicStats.h"

const int  NUM_BUCK = 199;

class GdataStat {
  public:
      GdataStat() : _hashTable(NUM_BUCK, hashFun) {}

      void Clear() {
	   _hashTable.clear();
      }

      void Insert(int index, BasicStats *bs){
	  _hashTable.insert(index, bs);
      }

      Boolean Lookup(int index, BasicStats *&bs) {
	  BasicStats *stat;
	  if(_hashTable.lookup(index, stat) == 0) {
	      bs = stat;
	      return true;
	   } 

	   return false;
       }

       Boolean Remove(int index) {
	   if(_hashTable.remove(index) >= 0) 
	       return true;
	   printf("Warning: Statistics for index %d not found", index);
	   return false;
       }


  protected:
     static int hashFun(int &index, int numOfBucks) { return index%numOfBucks; }

     HashTable<int, BasicStats *> _hashTable;
};

#endif
