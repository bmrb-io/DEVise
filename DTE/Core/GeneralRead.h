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

/*
  $Id$

  $Log$
  Revision 1.3  1996/12/15 06:41:04  donjerko
  Added support for RTree indexes

  Revision 1.2  1996/12/05 16:05:59  wenger
  Added standard Devise file headers.

 */

#ifndef GENERAL_READ_H
#define GENERAL_READ_H

#include "types.h"

class GeneralRead {
public:
	virtual int getNumFlds() = 0;
	virtual String *getTypeIDs() = 0;
	virtual String *getAttributeNames() = 0;
	virtual Tuple  *getNext() = 0;
	virtual String*getOrderingAttrib() =0;
	virtual Stats* getStats(){

		// default stats

		return new Stats(10);
	}
	virtual void open(){
		assert(!"open not implemented on some reader");
	}
	virtual ostream& display(ostream& out){
		out << "display not implemented for this reader" << endl;
		return out;
	}
     virtual void reset(int lowRid, int highRid){
		String msg = "reset not implemented for this reader";
		THROW(new Exception(msg), );
     }
};

#endif
