/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Revision 1.6  1998/10/02 17:20:00  wenger
  Fixed bug 404 (DataReader gets out-of-sync with records); made other
  cleanups and simplifications to DataReader code.

  Revision 1.5  1998/06/22 23:46:59  okan
  Fixed Some bugs...

  Revision 1.4  1998/06/16 16:30:52  wenger
  Added standard headers to DataReader sources.

 */

#ifndef DATAREADER_DATAREADER_H
#define DATAREADER_DATAREADER_H

//TEMP -- should probably hide some of this
#include "DRSchema.h"

class Buffer;

// This class defines the DataReader interface for external programs
class DataReader {
private:
	Buffer* myBuffer; // Buffer Object used for extracting fields
	Status _uStat; // Status of DataReader

public:
	DataReader(const char* dataFile, const char* schemaFile);
	~DataReader();

	Status getRecord(char* dest); // Function to read next record
	Status getRndRec(char* dest,int fileOffset); // Function to read a random record
	bool isOk() {return _uStat == OK;} // Check the Status of Reader
	friend ostream& operator<<(ostream &out, const DataReader &dr);

	DRSchema* myDRSchema; // DRSchema Object associated with this Reader
};

#endif
