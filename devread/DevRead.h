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
 */

#ifndef _DevRead_h_
#define _DevRead_h_


#include "DeviseTypes.h"
#include "RecId.h"

#include "types.h"
#include "marshal.h"  // for Tuple class

class TData;

class DevRead
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

   ~DevRead() { Close(); }

    DevStatus Open(char *schemaFile, char *dataFile);
    DevStatus Close();

	int getNumFlds() { return(_numAttr); }

	  // For now, it should return "IInt", "IString" etc.
	String *getTypeIDs();

	String *getAttributeNames();

	  // returns NULL when done
	Tuple  *getNext();

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
