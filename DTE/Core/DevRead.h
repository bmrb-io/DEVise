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
  Header file for DevRead (Devise Reader) class.
 */

/*
    $Id$
    $Log$
    Revision 1.3  1996/12/16 11:13:04  kmurli
    Changes to make the code work for separate TDataDQL etc..and also changes
    done to make Aggregates more robust

    Revision 1.2  1996/12/15 06:41:04  donjerko
    Added support for RTree indexes

    Revision 1.1  1996/11/23 02:00:35  donjerko
    Restructered DTE directory

    Revision 1.1  1996/11/18 18:08:46  donjerko
    Initial DTE release.

    Revision 1.1  1996/08/14 19:04:31  flisakow
      Checkin of DevRead class, a near duplicate of the AttrProj class,
      it is an interface to reading data from Devise files.

 */

#ifndef _DevRead_h_
#define _DevRead_h_


#include "DeviseTypes.h"
#include "RecId.h"

#include "types.h"
#include "Iterator.h"

class TData;

class DevRead : public Iterator
{
  public:

    DevRead() {
        _tDataP = NULL;
        _recBuf = NULL;
        _recBufSize = 0;
        _numAttr = 0;
    }

	  // schema may later be put into the dataFile
    DevRead(char *schemaFile, char *dataFile) {
        _tDataP = NULL;
        _recBuf = NULL;
        _recBufSize = 0;
        _numAttr = 0;
        Open(schemaFile,dataFile);
    }
	
   virtual ~DevRead() { Close(); }

    DevStatus Open(char *schemaFile, char *dataFile);
    DevStatus Close();
	
	virtual String *getOrderingAttrib();

	virtual int getNumFlds() { return(_numAttr); }

	  // For now, it should return "int", "string" etc.
	virtual String *getTypeIDs();

	virtual String *getAttributeNames();

	  // returns false when done
	virtual bool getNext(Tuple* next);

	virtual void reset(int lowRid, int highRid);

	virtual void setOffset(Offset offset);

	virtual Offset getOffset(){
		return Offset((int) _nxtRecId);
	}

  private:

	int computeNumFlds();

    TData         *_tDataP;
    RecId          _nxtRecId;     // The next record to examine
    RecId          _lastRecId;    // The last record to examine

    char          *_recBuf;
    int            _recBufSize;
    int            _numAttr;      // The number of attributes
};


#endif // _DevRead_h_

/*============================================================================*/
