/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1999
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
  Revision 1.11  1999/01/18 22:34:13  wenger
  Considerable changes to the DataReader:  reading is now per-field rather
  than per-character (except for dates); the "extractor" functions now do
  all the work, and the "value" functions have been eliminated; return values
  are more clear, and behaviour in "boundary conditions" is better-defined;
  fixed a number of bugs in the course of making these changes.  (The
  DataReader could still use some more cleanup.)

  Revision 1.10  1998/11/03 17:53:32  okan
  Fixed Several bugs and changed DataReader to use UtilAtof

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

typedef bool (Buffer::*eFunc)(Attribute* myAttr,char* target);

enum AmPm { AM, PM, NONE };

// Temporary Struct used for storing date values in the parsing process
struct DateInfo {
	int day;
	int mon;
	int year;
	int hour;
	int min;
	int sec;
	AmPm ampm;
	int nanosec;
	bool isad;
};

// "Flexible" (expandable) buffer.
class FlexiBuf {
public:
	FlexiBuf();
	~FlexiBuf();

	void Put(char tmpC);
	void UnPut();
	void Reset();
	const char *GetBuf() { return _buf; }
	int GetLength() { return _curIndex; } // includes terminator

private:
	void Expand();

	char *_buf;
	int _bufSize;
	int _curIndex;
};

enum BufState { BufInvalid, BufRecordStart, BufInComment, BufInField,
	BufSeparator, BufEol, BufEof };
ostream& operator<<(ostream &out, const BufState &status);


// Buffer class is used for extracting fields from data file and 
// storing them into the given buffer
// Parser (extractor) functions now store parsed values directly into
// the "final" buffers.

class Buffer {
private:
	ifstream _in; //data file
	Holder* _EOL; //End of Line string
	Holder* _comment; //Comment string
	char* _EOLCheck; //char array used for comparing current character to EOL 
	char** _separatorCheck; //same as EOLCheck, we use different arrays for each attribute
	FlexiBuf _fieldBuf;
	BufState _state;

	// temporary values
	char _curChar;

	bool* repeatings; // replicates repeating property of attributes, used for improving speed
	int* maxLens; // replicates maxLen property of attributes, used for improving speed
	int* fieldLens; // replicates fieldLen property of attributes, used for improving speed
	char* quoteChars; // replicates quoteChar property of attributes, used for improving speed

	// Reads next character from data file
	char getChar();

	// Reads the next character, dealing with escaped characters.  Returns
	// true iff the character *was* escaped.
	bool getEscapedChar(char &tmpChar, Attribute *attr);

	//Checks if the next character sequence is EOL
	bool checkEOL(char curChar);

	// Combination of EOL & Separator
	bool checkAll(char curChar, Attribute* myAttr);

    // Reads n characters from file stream and calculates the integer value
	bool getInt(int maxValLen, int &value);

	// Reads n characters from file stream and calculates the integer value
	// Also adds extra 0's for fractional seconds
	bool getFracInt(int maxValLen, int& value);
	
	// This function is used to match a given string in a given array of strings
	// I use this function to find the number of the given month
	bool checkString(char** values, int distinct, int arrayLength, int& val);

	// Reads the data for the given attribute into the field buffer, field
	// delimited by separator(s).
	int ReadFieldSep(Attribute *attr, bool append = false);

	// Reads the data for the given attribute into the field buffer, field
	// delimited by quotes.
	int ReadFieldQuote(Attribute *attr, bool append = false);

	// Reads the data for the given attribute into the field buffer, fixed
	// length field.
	int ReadFieldLen(Attribute *attr, int length, bool append = false);

	// Void function pointers used for defining a single interface to extractors
	// extFunc is the extractor function, valFunc is no longer used
	eFunc* extFunc;

	// Number of attributes
	int _nAttr;
	char** _months; // array of month names
	char** _monthAbbr; // array of abbreviated month names

public:
	Buffer(const char* fileName, DRSchema* myDRSchema, Status &status);
	~Buffer();

	// Read one record.
	bool ReadRecord(char *dest, DRSchema *schema);

	BufState GetState() { return _state; }

	// moves the pointer in a data file
	bool setBufferPos(int cPos);

private:
	// reads an integer field  from data file upto given separator
	bool getIntTo(Attribute* myAttr, char* target=NULL);

	// reads a double field  from data file upto given separator
	bool getDoubleTo(Attribute* myAttr, char* target=NULL);

	// reads a string field  from data file upto given separator
	bool getStringTo(Attribute* myAttr, char* target);

	// reads an integer field  from data file with a given length
	bool getIntLen(Attribute* myAttr, char* target=NULL);

	// reads a double field  from data file with a given length
	bool getDoubleLen(Attribute* myAttr, char* target=NULL);
	
	// reads a string field  from data file with a given length
	bool getStringLen(Attribute* myAttr, char* target);

	// reads a quoted field  string from data file
	bool getStringQuote(Attribute* myAttr, char* target);

	// reads a date field from data file
	bool getDate(Attribute* myAttr, char* dest);

	// Consume blank record(s), if any.  Note that all-whitespace lines
	// do not count as blank.
	void consumeBlank();

	// Consume commented record(s), if any.
	void consumeComment();

	// to make the interface easier, a single function is 
	// used for reading fields, this function calls proper
	// extractors using void function pointer arrays
	bool extractField(Attribute* myAttr, char* dest);

	// consume any remaining characters before the end of the current
	// field; does nothing if no field separator is defined
	void consumeField(Attribute *attr);

	// consume any remaining charaters before the end of the current
	// record; does nothing if no record delimiter is defined
	void consumeRecord();
};

#endif
