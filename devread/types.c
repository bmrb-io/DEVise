/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
 */

#include "types.h"

Type* intAdd(Type* arg1, Type* arg2){
     return new IInt(((IInt*)arg1)->value + ((IInt*)arg2)->value);
}

Type* intEq(Type* arg1, Type* arg2){
     if(((IInt*)arg1)->value == ((IInt*)arg2)->value){
		return new IInt(1);
	}
	else{
		return new IInt(0);
	}
}

Type* intLT(Type* arg1, Type* arg2){
     if(((IInt*)arg1)->value < ((IInt*)arg2)->value){
		return new IInt(1);
	}
	else{
		return new IInt(0);
	}
}
