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
  Revision 1.9  1998/10/12 21:24:19  wenger
  Fixed bugs 405, 406, and 408 (all DataReader problems); considerably
  increased the robustness of the DataReader (partly addresses bug 409);
  added test targets in the makefile, and corresponding data and schemas.

  Revision 1.8  1998/10/06 20:06:32  wenger
  Partially fixed bug 406 (dates sometimes work); cleaned up DataReader
  code without really changing functionality: better error handling,
  removed unused class members, added debug output, close data file (never
  closed before), various other cleanups.

  Revision 1.7  1998/10/02 17:19:59  wenger
  Fixed bug 404 (DataReader gets out-of-sync with records); made other
  cleanups and simplifications to DataReader code.

  Revision 1.6  1998/06/24 09:24:13  okan
  *** empty log message ***

  Revision 1.5  1998/06/16 16:30:52  wenger
  Added standard headers to DataReader sources.

 */

#ifndef DATAREADER_BUFFER_H
#define DATAREADER_BUFFER_H

#include "sysdep.h"
#include "DRSchema.h"

class Buffer;

typedef Status (Buffer::*eFunc)(Attribute* myAttr,char* target);
typedef void (Buffer::*vFunc)(char*);

enum AmPm { AM,PM,NONE};

// Temporary Struct used for storing date values in the parsing process
struct DateInfo {
	int day;
	int mon;
	int year;
	int hour;
	int min;
	int sec;
	AmPm ampm;
	int nanosec;//TEMP -- I think this is really microsec!!  RKW 1998-10-09
	bool adbc;
};

// Buffer class is used for extracting fields from data file and 
// storing them into the given buffer
// Parser functions stores parsed values into temporary private members
// and these values are read by calling getval functions after the parser 
// function. _curDate, _iRetVal, _fRetVal, _sign, _exponent and _expSign
// are used for this purpose. For Strings, we use the _posTarget to store
// the location of end of string

class Buffer {
private:
	ifstream _in; //data file
	Holder* _EOL; //End of Line string
	Holder* _comment; //Comment string
	char* _EOLCheck; //char array used for comparing current character to EOL 
	char** _separatorCheck; //same as EOLCheck, we use different arrays for each attribute
	// temporary values
	DateInfo _curDate;
	int _posTarget; // Applies only while reading a string attribute
	int _iRetVal;
	double _fRetVal;
	bool _sign;
	double _exponent;
	bool _expSign;
	char _curChar;
	bool* repeatings; // replicates repeating property of attributes, used for improving speed
	int* maxLens; // replicates maxLen property of attributes, used for improving speed
	int* fieldLens; // replicates fieldLen property of attributes, used for improving speed
	char* quoteChars; // replicates quoteChar property of attributes, used for improving speed
	
	char getChar(); // Reads next character from data file
	Status checkEOL(char curChar); //Checks if the next character sequence is EOL
	Status checkSeparator(char curChar, Attribute* myAttr); // Checks if the next character sequence is a Separator
	Status checkAll(char curChar, Attribute* myAttr); // Combination of EOL & Separator
	
	// Reads n characters from file stream and calculates the integer value
	Status getInt(int maxValLen, int& value);
	
	// Reads n characters from file stream and calculates the integer value
	// Also adds extra 0's for fractional seconds
	Status getFracInt(int maxValLen, int& value);

	// This function is used to match a given string in a given array of strings
	// I use this function to find the number of the given month
	Status checkString(char** values, int distinct, int arrayLength, int& val);

	// Void function pointers used for defining a single interface to extractors
	// extFunc is the extractor function, valFunc is used for reading the proper
	// value from temporary private members
	eFunc* extFunc;
	vFunc* valFunc;

	// Number of attributes
	int _nAttr;
	char** _months; // array of month names
	char** _monthAbbr; // array of abbreviated month names
	char* _tmpB; // temporary Buffer for double types
	char* _tmpBStart;
	bool _dataInValid;

public:
	Buffer(const char* fileName, DRSchema* myDRSchema, Status &status); // constructor
	~Buffer();

	// read temporary private members and calculates the final result
	void getDoubleVal(char* dest);
	void getIntVal(char* dest);
	void getDateVal(char* dest);
	void getStringVal(char* dest); // terminates string

	Status setBufferPos(int cPos); // moves the pointer in a data file

	// reads an integer field  from data file upto given separator
	Status getIntTo(Attribute* myAttr, char* target);

	// reads a double field  from data file upto given separator
	Status getDoubleTo(Attribute* myAttr, char* target=NULL);

	// reads a string field  from data file upto given separator
	Status getStringTo(Attribute* myAttr, char* target);

	// reads an integer field  from data file with a given length
	Status getIntLen(Attribute* myAttr, char* target=NULL);

	// reads a double field  from data file with a given length
	Status getDoubleLen(Attribute* myAttr, char* target=NULL);
	
	// reads a string field  from data file with a given length
	Status getStringLen(Attribute* myAttr, char* target);

	// reads a quoted field  string from data file
	Status getStringQuote(Attribute* myAttr, char* target);

	// reads a date field from data file
	Status getDate(Attribute* myAttr, char* dest);

	Status checkComment(); // Check if this record begins with a comment

	// to make the interface easier, a single function is 
	// used for reading fields, this function calls proper
	// extractors using void function pointer arrays
	Status extractField(Attribute* myAttr, char* dest);

	// consume any remaining characters before the end of the current
	// field; does nothing if no field separator is defined
	Status consumeField(Attribute *attr);

	// consume any remaining charaters before the end of the current
	// record; does nothing if no record delimiter is defined
	Status consumeRecord(Status tmpS);
	void unSetInValid() { _dataInValid = true;}
	bool getInValid() { return _dataInValid;}
};

#endif
