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
#include <stdlib.h>

#include "HashTable.h"
#include "BasicStats.h"

const int  NUM_BUCK = 199;

class GdataStat {
  public:
      GdataStat() : _hashTable(NUM_BUCK, hashFun) {}

      void Clear() {
	   _hashTable.clear();
      }

      void Insert(double index, BasicStats *bs){
	  _hashTable.insert(index, bs);
      }

      Boolean Lookup(double index, BasicStats *&bs) {
	  BasicStats *stat;
	  if(_hashTable.lookup(index, stat) == 0) {
	      bs = stat;
	      return true;
	   } 

	   return false;
       }

       Boolean Remove(double index) {
	   if(_hashTable.remove(index) >= 0) 
	       return true;
	   printf("Warning: Statistics for index %f not found", index);
	   return false;
       }


  protected:
     static int hashFun(double &index, int numOfBucks) { return abs((int)index)%numOfBucks; }

//     HashTable<int, BasicStats *> _hashTable;
     HashTable<double, BasicStats *> _hashTable;
};

#endif
