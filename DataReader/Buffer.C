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
  Revision 1.21  1999/02/01 19:18:37  wenger
  Added DataReader functions to set and get file offset.

  Revision 1.20  1999/01/29 23:30:25  beyer
  fixed memory leak and record size problem

  Revision 1.19  1999/01/18 22:34:13  wenger
  Considerable changes to the DataReader:  reading is now per-field rather
  than per-character (except for dates); the "extractor" functions now do
  all the work, and the "value" functions have been eliminated; return values
  are more clear, and behaviour in "boundary conditions" is better-defined;
  fixed a number of bugs in the course of making these changes.  (The
  DataReader could still use some more cleanup.)

  Revision 1.18  1998/11/03 17:53:31  okan
  Fixed Several bugs and changed DataReader to use UtilAtof

  Revision 1.17  1998/10/20 15:15:47  beyer
  workaround for proper eof detection (first field must be zero).

  Revision 1.16  1998/10/16 21:29:35  beyer
  fixed length integers can have leading spaces

  Revision 1.15  1998/10/12 21:24:18  wenger
  Fixed bugs 405, 406, and 408 (all DataReader problems); considerably
  increased the robustness of the DataReader (partly addresses bug 409);
  added test targets in the makefile, and corresponding data and schemas.

  Revision 1.14  1998/10/06 20:06:32  wenger
  Partially fixed bug 406 (dates sometimes work); cleaned up DataReader
  code without really changing functionality: better error handling,
  removed unused class members, added debug output, close data file (never
  closed before), various other cleanups.

  Revision 1.13  1998/10/02 17:19:57  wenger
  Fixed bug 404 (DataReader gets out-of-sync with records); made other
  cleanups and simplifications to DataReader code.

  Revision 1.12  1998/08/06 23:07:26  beyer
  bug fixes

  Revision 1.11  1998/07/02 22:39:24  beyer
  fixed comment bug

  Revision 1.10  1998/06/29 17:18:23  wenger
  Fixed bug 372 (crashed in DataReader caused by a pointer alignment problem).

  Revision 1.9  1998/06/28 21:39:20  beyer
  corrected c++ calling convention for methods

  Revision 1.8  1998/06/24 09:24:10  okan
  *** empty log message ***

  Revision 1.7  1998/06/24 01:33:20  okan
  *** empty log message ***

  Revision 1.6  1998/06/23 22:57:11  okan
  *** empty log message ***

  Revision 1.5  1998/06/22 23:46:58  okan
  Fixed Some bugs...

  Revision 1.4  1998/06/16 16:30:51  wenger
  Added standard headers to DataReader sources.

 */

#include <math.h>

#include "Buffer.h"
#include "DateTime.h"
#include "Util.h"

#define DEBUG 0
//#define DEBUG_DATE

#define DIGITSTART '0'
#define ESCAPE '\\' //ASCII value of '\'

//---------------------------------------------------------------------------
FlexiBuf::FlexiBuf()
{
	_buf = NULL;
	_bufSize = 0;
	_curIndex = 0;
}

//---------------------------------------------------------------------------
FlexiBuf::~FlexiBuf()
{
	delete [] _buf;
	_buf = NULL;
	_bufSize = 0;
}

//---------------------------------------------------------------------------
void
FlexiBuf::Put(char tmpC)
{
	if (_curIndex >= _bufSize) {
		Expand();
	}
	_buf[_curIndex++] = tmpC;
}

//---------------------------------------------------------------------------
void
FlexiBuf::UnPut()
{
	if (_curIndex > 0) _curIndex--;
}

//---------------------------------------------------------------------------
void
FlexiBuf::Reset()
{
	_curIndex = 0;

	// Make sure string is terminated.
	if (_buf != NULL) _buf[0] = '\0';
}

//---------------------------------------------------------------------------
void
FlexiBuf::Expand()
{
	int newSize = _bufSize * 2 + 128; // more or less arbitrary
	char *newBuf = new char[newSize];
	assert(newBuf);

	if (_bufSize > 0) {
		memcpy(newBuf, _buf, _bufSize);
		delete [] _buf;
	}

	_buf = newBuf;
	_bufSize = newSize;
}

//---------------------------------------------------------------------------
Buffer::Buffer(const char* fileName, DRSchema* myDRSchema, Status &status) {
#if (DEBUG >= 1)
	cout << "Buffer::Buffer(" << fileName << ", " <<
	  myDRSchema->getDRSchemaName() << ")\n";
#endif

	status = FAIL;

	int i,j;
	Holder* temArr;

	_nAttr = myDRSchema->qAttr;

	_in.open(fileName);
	if (_in.fail()) {
		cerr << "DataReader File : " << fileName << " not found" << endl;
		perror("open");
		return;
	}

	// Initialize EOLCheck to make faster EOL comparison
	_EOLCheck = new char[128];
	for (i = 0 ; i<128 ; i++) {
		_EOLCheck[i] = 0;
	}
	if (myDRSchema->getDelimiter() != NULL) {
		_EOL = myDRSchema->getDelimiter();
		for (i = 0 ; i < _EOL->length ; i++) {
			//TEMP -- array bounds error here if delimiter char is > 127
			//TEMPANS -- I designed this for unix knowing that there are only
			//           127 chars in ASCII, this can be changed easily if needed
			_EOLCheck[_EOL->data[i]] = 1;
		}
	} else {
		_EOL = NULL;
	}

	_separatorCheck = new char*[myDRSchema->qAttr];

	// Initialize month names and abbreviated month names arrays
	_months = new char*[12];
	_months[0] = "JANUARY" ;
	_months[1] = "FEBRUARY" ;
	_months[2] = "MARCH" ;
	_months[3] = "APRIL" ;
	_months[4] = "MAY" ;
	_months[5] = "JUNE" ;
	_months[6] = "JULY" ;
	_months[7] = "AUGUST" ;
	_months[8] = "SEPTEMBER" ;
	_months[9] = "OCTOBER" ;
	_months[10] = "NOVEMBER" ;
	_months[11] = "DECEMBER" ;



	_monthAbbr = new char*[12];
	_monthAbbr[0] = "JAN" ;
	_monthAbbr[1] = "FEB" ;
	_monthAbbr[2] = "MAR" ;
	_monthAbbr[3] = "APR" ;
	_monthAbbr[4] = "MAY" ;
	_monthAbbr[5] = "JUN" ;
	_monthAbbr[6] = "JUL" ;
	_monthAbbr[7] = "AUG" ;
	_monthAbbr[8] = "SEP" ;
	_monthAbbr[9] = "OCT" ;
	_monthAbbr[10] = "NOV" ;
	_monthAbbr[11] = "DEC" ;
	
	extFunc = new eFunc[myDRSchema->qAttr];

	_comment = myDRSchema->getComment();
	
	repeatings = new bool[myDRSchema->qAttr];
	maxLens = new int[myDRSchema->qAttr];
	fieldLens = new int[myDRSchema->qAttr];
	quoteChars = new char[myDRSchema->qAttr];

	// Initialize _separatorCheck to make faster separator comparison
	// Also, initialize ext and val Funcs to proper extractor functions

	for (i = 0 ; i < (int)(myDRSchema->qAttr) ; i++) {
		if (myDRSchema->tableAttr[i]->getSeparator() != NULL) {
			temArr = myDRSchema->tableAttr[i]->getSeparator();
			_separatorCheck[i] = new char[128];
			for (j = 0; j < 128 ; j++)
				_separatorCheck[i][j] = 0;
			for (j = 0; j < temArr->length; j++)
				_separatorCheck[i][temArr->data[j]] = 1;
			repeatings[i] = myDRSchema->tableAttr[i]->getSeparator()->repeating;
		} else {
			_separatorCheck[i] = NULL;
        }

		//TEMP -- why do we copy all of this stuff into here instead
		// of just referencing the schema?
		//TEMPANS -- Just to speed up the code, we can read these by calling these 
		//           functions, but it will slow down the program
		maxLens[i] = myDRSchema->tableAttr[i]->getMaxLen();
		fieldLens[i] = myDRSchema->tableAttr[i]->getFieldLen();
		quoteChars[i] = myDRSchema->tableAttr[i]->getQuote();

		if (myDRSchema->tableAttr[i]->getFieldLen() != -1) {
			switch (myDRSchema->tableAttr[i]->getType())	{
				case TYPE_INVALID:
					cerr << "InValid attribute type\n";
					break;
				case TYPE_INT:
					extFunc[i] = &getIntLen;
					break;
				case TYPE_DOUBLE:
					extFunc[i] = &getDoubleLen;
					break;
				case TYPE_STRING:
					extFunc[i] = &getStringLen;
					break;
				case TYPE_DATE:
					break;
				default:
					cerr << "Illegal attribute type\n";
					break;
			}
		} else if (myDRSchema->tableAttr[i]->getQuote() != -1) {
			extFunc[i] = &getStringQuote;
		} else {
			//TEMP -- why two switches????
			//TEMPANS -- The first one is for variable length fields and this one is for
			//           fixed length fields, these two are implemented in different ways
			switch (myDRSchema->tableAttr[i]->getType()) {
				case TYPE_INVALID:
					cerr << "InValid attribute type\n";
					break;
				case TYPE_INT:
					extFunc[i] = &getIntTo;
					break;
				case TYPE_DOUBLE:
					extFunc[i] = &getDoubleTo;
					break;
				case TYPE_STRING:
					extFunc[i] = &getStringTo;
					break;
				case TYPE_DATE:
					extFunc[i] = &getDate;
					break;
				default:
					cerr << "Illegal attribute type\n";
					break;
			}
		}
	}

	_state = BufRecordStart;

	status = OK;
}

//---------------------------------------------------------------------------
Buffer::~Buffer() {
#if (DEBUG >= 1)
	cout << "Buffer::~Buffer()\n";
#endif

	int i ;

	_in.close();

	delete [] _EOLCheck;
	_EOLCheck = NULL;
	
	if (_separatorCheck != NULL) {
		for (i = 0 ; i < _nAttr ; i++) {
			delete [] _separatorCheck[i];
			_separatorCheck[i] = NULL;
		}
		delete [] _separatorCheck;
		_separatorCheck = NULL;
	}

	delete [] extFunc;
	extFunc = NULL;

	delete [] _months;
	_months = NULL;

	delete [] _monthAbbr;
	_monthAbbr = NULL;

	delete [] repeatings; 
	repeatings = NULL;

	delete [] maxLens;
	maxLens = NULL;

	delete [] fieldLens;
	fieldLens = NULL;

	delete [] quoteChars;
	quoteChars = NULL;
}

//---------------------------------------------------------------------------
bool Buffer::setBufferPos(int cPos) {
#if (DEBUG >= 1)
	cout << "Buffer::setBufferPos(" << cPos << ")\n";
#endif

	char c = -1;
	_in.seekg(cPos);
	c = _in.peek();
	if (c == -1) {
		cerr << "Cannot set file pointer to: " << cPos << endl;
		_state = BufEof;
		return false;
	} else {
		_state = BufRecordStart;
		return true;
	}
}

//---------------------------------------------------------------------------
int Buffer::getBufferPos() {
#if (DEBUG >= 1)
	cout << "Buffer::getBufferPos()\n";
#endif

	return _in.tellg();
}

//---------------------------------------------------------------------------
bool
//TEMP -- schema arg is temporary?
Buffer::ReadRecord(char *dest, DRSchema *schema)
{
#if (DEBUG >= 1)
	cout << "Buffer::ReadRecord()\n";
#endif

	bool result = false;

	_state = BufRecordStart;

	//
	// Skip any leading blank lines.
	//
	consumeBlank();

	//
	// Skip any comment lines and subsequent blank lines.
	//
	consumeComment();

	//
	// Attempt to read the fields.
	//
	if (_state != BufEof) {
		result = true;
    	for (int attrNum = 0 ; attrNum < _nAttr; attrNum++) {
#if (DEBUG >= 2)
			cout << "Reading field " << attrNum << endl;
#endif
			char *tmpPoint = dest + schema->tableAttr[attrNum]->offset;
			if (!extractField(schema->tableAttr[attrNum], tmpPoint)) {
#if (DEBUG >= 1) //TEMP -- maybe this should always be enabled
				cerr << "Extract function failed for attribute " <<
				  schema->tableAttr[attrNum]->getFieldName() << endl;
#endif
				result = false;
			}
#if (DEBUG >= 2)
			cout << "Buffer status = " << _state << endl;
#endif
		}
	}

	//
	// Make sure we consume all of the current record and any subsequent
	// blank lines.
	//
	consumeRecord();

	//
	// Skip any trailing comments and subsequent blank lines.
	//
	consumeComment();

	return result;
}

//---------------------------------------------------------------------------
char Buffer::getChar() {
	char c = 0;
	_in.get(c);
	return c;
}

//---------------------------------------------------------------------------
// Returns true iff character was escaped.
bool
Buffer::getEscapedChar(char &tmpChar, Attribute *attr)
{
// Gets a character, dealing with escaped characters; if attr is non-NULL,
// it checks for non-escaped characters being separators, EOL, etc. and
// returns 0 if that is the case.

	bool escaped = false;
	char tmpC = getChar();
	if (tmpC == 0) {
		_state = BufEof;
	} else if (tmpC == ESCAPE) {
		escaped = true;
		tmpC = getChar();
		if (tmpC == 0) {
			_state = BufEof;
		}
	}

	if (tmpC != 0 && !escaped && attr != NULL && checkAll(tmpC, attr)) {
		// Hit separator, EOL, etc.
		tmpC = 0;
	}

	tmpChar = tmpC;
	return escaped;
}

//---------------------------------------------------------------------------
bool Buffer::checkEOL(char curChar) {

// checks if the next character sequence in the data file is
// an EOL or not, if EOL is defined as repeating, finds several
// matches

#if (DEBUG >= 3)
	cout << "Buffer::checkEOL('" << curChar << "')\n";
#endif

	// Note: this is always false if no record separator is defined.
	if (_EOLCheck[curChar] == 1) {
		if (_EOL->repeating) {

			// Consume any more immediately following record separators.
			if ((curChar = getChar()) == 0) {
				_state = BufEof;
				return true;
			}

			while (_EOLCheck[curChar] == 1) {
				if ((curChar = getChar()) == 0) {
					_state = BufEof;
					return true;
				}
			}

			_in.putback(curChar);
		}

		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
void
Buffer::consumeBlank()
{
#if (DEBUG >= 2)
	cout << "Buffer::consumeBlank()\n";
#endif

	if (_state != BufEof && _EOL != NULL) {
		char tmpC = getChar();
		if (tmpC == 0) {
			_state = BufEof;
		} else if (!checkEOL(tmpC)) {
			_in.seekg(-1, ios::cur);
		}
	}
}

//---------------------------------------------------------------------------
void
Buffer::consumeComment()
{
// This is called at the beginning of the record to check if
// record begins with comment, if so skips the record till next
// separator, otherwise backs up ifstream.  If comment chars are
// defined but record separator is not, just consumes comment
// chars.

#if (DEBUG >= 2)
	cout << "Buffer::consumeComment()\n";
#endif
	
	if (_state != BufEof && _comment != NULL) {
		while (true) {
			for (int i = 0; i < _comment->length; i++) {
				char tmpChar;
				if ((tmpChar = getChar()) == 0) {
					_state = BufEof;
					return;
				}
				if (tmpChar != _comment->data[i]) {
					// Not a comment -- put back the chars we just read.
					_in.seekg((-1)*(i+1), ios::cur);
					return;
				}
			}
			_state = BufInComment;

			consumeRecord();
			if (_state == BufEof) return;
		}
	}
}

//---------------------------------------------------------------------------
// Returns true iff we encounter a separator, EOL, or EOF.
bool
Buffer::checkAll(char curChar, Attribute* myAttr)
{
// Combination of Check EOL & Separator, added for decreasing 
// time lost in function calls

#if (DEBUG >= 3)
	cout << "Buffer::checkAll('" << curChar << "', " <<
	  myAttr->getFieldName() << ")\n";
#endif

	if (_EOLCheck[curChar] == 1) {
		if (_EOL->repeating) {
			if ((curChar = getChar()) == 0) {
				_state = BufEof;
				return true;
			}

			while (_EOLCheck[curChar] == 1) {
				if ((curChar = getChar()) == 0) {
					_state = BufEof;
				 	return true;
				}
			}

			_in.putback(curChar);
		}
		_state = BufEol;
		return true;
	}

	if (_separatorCheck[myAttr->whichAttr][curChar] == 1) {
		if (repeatings[myAttr->whichAttr]) {
			if ((curChar = getChar()) == 0) {
				_state = BufEof;
				return true;
			}
				
			while (_separatorCheck[myAttr->whichAttr][curChar] == 1) {
				if ((curChar = getChar()) == 0) {
					_state = BufEof;
					return true;
				}
			}

			_in.putback(curChar);
		}
		_state = BufSeparator;
		return true;
	}

	return false;
}

//TEMP -- this should be cleaned up
bool
Buffer::getInt(int maxValLen, int& value)
{
	value = 0;
	char tmpC;
	for (int i = 0; i < maxValLen; i++) {
		if ((tmpC = getChar()) == 0) {
			_state = BufEof;
			return false;
		}
		if (!isdigit(tmpC)) {
			// If we got to here, we presumably hit a separator character,
			// so put it back.  This allows '1/1/98', for example, to be a
			// legal date field if the format is 'm/d/y'; '01/01/98' is still
			// okay.  RKW 1998-10-09
			_in.putback(tmpC);
			return true;
		}

		value = value*10 + (int)(tmpC - DIGITSTART);
	}
	return true;
}

bool
Buffer::getFracInt(int maxValLen, int& value)
{
	value = 0;
	char tmpC;
	int i;
	for (i = 0; i < maxValLen; i++) {

		if ((tmpC = getChar()) == 0) {
			_state = BufEof;
			return false;
		}
		if (!isdigit(tmpC)) {
			// If we got to here, we presumably hit a separator character,
			// so put it back.  This allows '1/1/98', for example, to be a
			// legal date field if the format is 'm/d/y'; '01/01/98' is still
			// okay.  RKW 1998-10-09
			_in.putback(tmpC);
			break;
		}

		value = value*10 + (int)(tmpC - DIGITSTART);
	}
	value = value * (int)(pow(10 , maxValLen - i));

	return true;
}

//---------------------------------------------------------------------------
bool
Buffer::checkString(char** values, int distinct, int arrayLength, int& val)
{

// This function finds the given (value of current field) string in an array of strings, e.g.
// for months array, finds the index of given month in the months array
// values : given string array, distinct : how many characters do we need to
// distinguish an element in this array
// arrayLength : number of elements in the values array
// val : index of given string in the array
	
	//TEMP -- what if arrayLength < 12???
	//TEMPANS -- This function was designed for finding monthname, and it is only used for finding this, as 
	//           there are only 12 months, I limited array length to 12

	int howMany[12] = { 0,1,2,3,4,5,6,7,8,9,10,11 };
	int i,j;
	int where = arrayLength;
	int loc;
	int temw;

	for (i = 0 ; i < distinct; i++) {
		if ((_curChar = getChar()) == 0) {
			_state = BufEof;
			return false;
		}
		_curChar = ((int)_curChar > 90 ? (int)_curChar - 32 : _curChar);

		loc = 0;
		temw = where;
		for (j = 0; j < temw; j++) {

			if (_curChar == values[howMany[j]][i]) {
				howMany[loc] = howMany[j];
				loc ++;
			} else {
				where--;
			}
		}
	}
	if (where != 1) {
		cerr << "Couldn't find given string in the array" << endl;
		return false;
	}

	val = howMany[0]+1;
	temw = strlen(values[howMany[0]]);
	for (i = distinct; i < temw; i++) {

		if ((_curChar = getChar()) == 0) {
			_state = BufEof;
			return false;
		}
		_curChar = ((int)_curChar > 90 ? (int)_curChar - 32 : _curChar);

		if (_curChar != values[howMany[0]][i]) {
			cerr << "Couldn't find given string in the array" << endl;
			return false;
		}

	}

	return true;
}

//---------------------------------------------------------------------------
bool Buffer::getIntTo(Attribute* myAttr, char* target)
{
// reads an integer from data file up to defined separator
// target : where to put the value
// myAttr : Attribute object assigned to current field

#if (DEBUG >= 2)
	cout << "Buffer::getIntTo(" << myAttr->getFieldName() << ")\n";
#endif

	int result = ReadFieldSep(myAttr);
	int tmpInt;
	if (result > 0) {
		tmpInt = atoi(_fieldBuf.GetBuf());
	} else {
		tmpInt = 0; // or handle nulls
	}
	memcpy(target, &tmpInt, sizeof(tmpInt));

	return (result >= 0);
}

//---------------------------------------------------------------------------
bool Buffer::getDoubleTo(Attribute* myAttr, char* target) {
	
// reads a double from data file up to defined separator
// this function checks e/E for exponents
// Also, this function reads the proper field and calls UtilAtof to 
// to calculate the corresponding value
// target : where to put the value
// myAttr : Attribute object assigned to current field

#if (DEBUG >= 2)
	cout << "Buffer::getDoubleTo(" << myAttr->getFieldName() << ")\n";
#endif

	int result = ReadFieldSep(myAttr);
	double tmpDouble;
	if (result > 0) {
		tmpDouble = UtilAtof(_fieldBuf.GetBuf());
	} else {
		tmpDouble = 0.0; // or handle nulls
	}
	memcpy(target, &tmpDouble, sizeof(tmpDouble));

	return (result >= 0);
}

//---------------------------------------------------------------------------
bool Buffer::getStringTo(Attribute* myAttr, char* target) {

// reads a string from data file up to defined separator
// target : destination buffer to put the string read from data file
// myAttr : Attribute object assigned to current field

#if (DEBUG >= 2)
	cout << "Buffer::getStringTo(" << myAttr->getFieldName() << ")\n";
#endif

	int result = ReadFieldSep(myAttr);
	if (result > 0) {
		int maxStrLen = maxLens[myAttr->whichAttr]; // includes term. NULL
		// Note: memcpy() is faster than strcpy().  RKW 1999-01-08.

		if (_fieldBuf.GetLength() > maxStrLen) {
#if (DEBUG >= 1)
			cerr << "Warning: data in field " << myAttr->getFieldName() <<
			  " is longer than available buffer; string is truncated.\n";
#endif
			memcpy(target, _fieldBuf.GetBuf(), maxStrLen-1);
			target[maxStrLen-1] = 0;
		} else {
			memcpy(target, _fieldBuf.GetBuf(), _fieldBuf.GetLength());
		}
	} else {
		*target = '\0';
	}

	return (result >= 0);
}

//---------------------------------------------------------------------------
bool Buffer::getIntLen(Attribute* myAttr, char* target = NULL) {

// reads an integer from data file with a given length
// target : not used in this function, we have a single interface for
//          every extractor function
// myAttr : Attribute object assigned to current field

#if (DEBUG >= 2)
	cout << "Buffer::getIntLen(" << myAttr->getFieldName() << ")\n";
#endif

	int result = ReadFieldLen(myAttr, fieldLens[myAttr->whichAttr]);
	int tmpInt;
	if (result > 0) {
		tmpInt = atoi(_fieldBuf.GetBuf());
	} else {
		tmpInt = 0; // or handle nulls
	}
	memcpy(target, &tmpInt, sizeof(tmpInt));

	return (result >= 0);
}

//---------------------------------------------------------------------------
bool Buffer::getDoubleLen(Attribute* myAttr, char* target) {

// reads a double from data file with a given length
// this function checks e/E for exponents
// Also, this function reads the proper field and calls UtilAtof to 
// to calculate the corresponding value
// target : where to put the value
// myAttr : Attribute object assigned to current field

#if (DEBUG >= 2)
	cout << "Buffer::getDoubleLen(" << myAttr->getFieldName() << ")\n";
#endif

	int result = ReadFieldLen(myAttr, fieldLens[myAttr->whichAttr]);
	double tmpDouble;
	if (result > 0) {
		tmpDouble = UtilAtof(_fieldBuf.GetBuf());
	} else {
		tmpDouble = 0.0; // or handle nulls
	}
	memcpy(target, &tmpDouble, sizeof(tmpDouble));

	return (result >= 0);
}

//---------------------------------------------------------------------------
bool Buffer::getStringLen(Attribute* myAttr, char* target) {

// reads a string from data file with a given length
// target : destination buffer to put the string read from data file
// myAttr : Attribute object assigned to current field

#if (DEBUG >= 2)
	cout << "Buffer::getStringLen(" << myAttr->getFieldName() << ")\n";
#endif

	int result = ReadFieldLen(myAttr, fieldLens[myAttr->whichAttr]);
	if (result > 0) {
		int maxStrLen = fieldLens[myAttr->whichAttr]; // includes term. NULL
		// Note: memcpy() is faster than strcpy().  RKW 1999-01-08.

		if (_fieldBuf.GetLength() > maxStrLen) {
#if (DEBUG >= 1)
			cerr << "Warning: data in field " << myAttr->getFieldName() <<
			  " is longer than available buffer; string is truncated.\n";
#endif
			memcpy(target, _fieldBuf.GetBuf(), maxStrLen-1);
			target[maxStrLen-1] = 0;
		} else {
			memcpy(target, _fieldBuf.GetBuf(), _fieldBuf.GetLength());
		}
	} else {
		*target = '\0';
	}

	return (result >= 0);
}

//---------------------------------------------------------------------------
bool Buffer::getStringQuote(Attribute* myAttr, char* target) {

// reads a quoted string from data file with a given quote character
// target : destination buffer to put the string read from data file
// myAttr : Attribute object assigned to current field

#if (DEBUG >= 2)
	cout << "Buffer::getStringQuote(" << myAttr->getFieldName() << ")\n";
#endif

	int result = ReadFieldQuote(myAttr, false);
	if (result > 0) {
		int maxStrLen = maxLens[myAttr->whichAttr]; // includes term. NULL
		// Note: memcpy() is faster than strcpy().  RKW 1999-01-08.

		if (_fieldBuf.GetLength() > maxStrLen) {
#if (DEBUG >= 1)
			cerr << "Warning: data in field " << myAttr->getFieldName() <<
			  " is longer than available buffer; string is truncated.\n";
#endif
			memcpy(target, _fieldBuf.GetBuf(), maxStrLen-1);
			target[maxStrLen-1] = 0;
		} else {
			memcpy(target, _fieldBuf.GetBuf(), _fieldBuf.GetLength());
		}
	} else {
		*target = '\0';
	}

	return (result >= 0);
}

bool
Buffer::getDate(Attribute* myAttr, char* target)
{
// reads a date from data file with a date format
// target : destination buffer to put the string read from data file
// myAttr : Attribute object assigned to current field
// User may define any date component with more than one char (e.g. yyyy : year)
// To speed up extraction, user given format is converted to a different format with 
// single character for each component definition

#if (DEBUG >= 2)
	cout << "Buffer::getDate(" << myAttr->getFieldName() << " (" <<
	  myAttr->getDateFormat() << ") )\n";
#endif

	DateInfo curDate;

	curDate.day = 0;
	curDate.mon = 0;
	curDate.year = 0;
	curDate.hour = 0;
	curDate.min = 0;
	curDate.sec = 0;
	curDate.ampm = NONE;
	curDate.nanosec = 0;
	curDate.isad = true;

	bool tmpResult;

	char* dFormat = myAttr->getDateFormat();
	while (*dFormat != '\0') {
#if defined(DEBUG_DATE)
		cout << "*dFormat = '" << *dFormat << "'\n";
#endif
		switch (*dFormat) {
			case 'd':
				tmpResult = getInt(2, curDate.day);
#if defined(DEBUG_DATE)
				cout << "curDate.day = " << curDate.day << endl;
#endif
				if (!tmpResult) return false;
				break;

			case 'h':
				tmpResult = getInt(2, curDate.hour);
#if defined(DEBUG_DATE)
				cout << "curDate.hour = " << curDate.hour << endl;
#endif
				if (!tmpResult) return false;
				break;
			
			case 'm':
				tmpResult = getInt(2, curDate.mon);
#if defined(DEBUG_DATE)
				cout << "curDate.mon = " << curDate.mon << endl;
#endif
				if (!tmpResult) return false;
				break;

			case 'i':
				tmpResult = getInt(2, curDate.min);
#if defined(DEBUG_DATE)
				cout << "curDate.min = " << curDate.min << endl;
#endif
				if (!tmpResult) return false;
				break;

			case 'P':
				if ((_curChar = getChar()) == 0) {
					_state = BufEof;
					return false;
				}

				if ((_curChar == 'A') || (_curChar == 'a')) {
					curDate.ampm = AM;
				} else if ((_curChar == 'P') || (_curChar == 'p')) {
					curDate.ampm = PM;
				} else {
					cerr << "Date format for: " << myAttr->getFieldName() <<
					  " doesn't match the given date format: " <<
					  myAttr->getDateFormat() << endl;
					return FAIL;
				}

				if ((_curChar = getChar()) == 0) {
					_state = BufEof;
					return false;
				}
				if ((_curChar != 'M') && (_curChar != 'm')) {
					cerr << "Date format for: " << myAttr->getFieldName() <<
					  " doesn't match the given date format: " <<
					  myAttr->getDateFormat() << endl;
					return FAIL;
				}
#if defined(DEBUG_DATE)
				cout << "curDate.ampm = " << curDate.ampm << endl;
#endif

				break;

			case 's':
				tmpResult = getInt(2, curDate.sec);
#if defined(DEBUG_DATE)
				cout << "curDate.sec = " << curDate.sec << endl;
#endif
				if (!tmpResult) return false;
				break;

			case 'f':
				tmpResult = getFracInt(9, curDate.nanosec);
#if defined(DEBUG_DATE)
				cout << "curDate.nanosec = " << curDate.nanosec << endl;
#endif
				if (!tmpResult) return false;
				break;
				
			case 'y':
				tmpResult = getInt(2, curDate.year);
#if defined(DEBUG_DATE)
				cout << "curDate.year = " << curDate.year << endl;
#endif
				if (!tmpResult) return false;
				curDate.year += 1900;
				break;

			case 'Y':
				tmpResult = getInt(4, curDate.year);
#if defined(DEBUG_DATE)
				cout << "curDate.year = " << curDate.year << endl;
#endif
				if (!tmpResult) return false;
				break;

			case 'B':
				
				if ((_curChar = getChar()) == 0) {
					_state = BufEof;
					return false;
				}

				if ((_curChar == 'B') || (_curChar == 'b')) {
					curDate.isad= false;
				} else if ((_curChar == 'A') || (_curChar == 'a')) {
					curDate.isad = true;
				} else {
					cerr << "Date format for: " << myAttr->getFieldName() <<
					  " doesn't match the given date format: " <<
					  myAttr->getDateFormat() << endl;
					return FAIL;
				}

				if ((_curChar = getChar()) == 0) {
					_state = BufEof;
					return false;
				}
				if ((_curChar != 'D') && (_curChar != 'd') && (_curChar != 'C') && (_curChar != 'c')) {
					cerr << "Date format for: " << myAttr->getFieldName() <<
					  " doesn't match the given date format: " <<
					  myAttr->getDateFormat() << endl;
					return FAIL;
				}
#if defined(DEBUG_DATE)
				cout << "curDate.isad = " << curDate.isad << endl;
#endif
			
				break;

			case 'M':
				tmpResult = checkString(_months,3,12,curDate.mon);
#if defined(DEBUG_DATE)
				cout << "curDate.mon = " << curDate.mon << endl;
#endif
				if (!tmpResult) return false;
				break;

			case 'N':
				tmpResult = checkString(_monthAbbr,3,12,curDate.mon);
#if defined(DEBUG_DATE)
				cout << "curDate.mon = " << curDate.mon << endl;
#endif
				if (!tmpResult) return false;
				break;
				
			default:
				if ((_curChar = getChar()) == 0) {
					_state = BufEof;
					return false;
				}
				if (_curChar != *dFormat) {
					cerr << "Date format for: " << myAttr->getFieldName() <<
					  " doesn't match the given date format: " <<
					  myAttr->getDateFormat() << endl;
					return FAIL;
				}
		}
		dFormat++;
	}

	if (myAttr->getSeparator() != NULL) {
		if ((_curChar = getChar()) == 0) {
			_state = BufEof;
			return false;
		}
		if (!checkAll(_curChar, myAttr)) {
			cerr << "Couldn't find separator for : " << myAttr->getFieldName() << endl;
			return false;
		}
	}

	EncodedDTF tmpDate;

	if (curDate.ampm == PM) {
		curDate.hour += 12;
	}

	if (!(curDate.isad)) {
		curDate.year *= -1;
	}

	tmpDate.setDate(curDate.year, curDate.mon, curDate.day);

	tmpDate.setTime(curDate.hour, curDate.min, curDate.sec);
	
	tmpDate.setNanoSec(curDate.nanosec);

#if (DEBUG >= 2)
	cout << "  date val = " << tmpDate << "\n";
#endif

	// We MUST use memcpy() here in case dest is not aligned.
	memcpy(target, &tmpDate, sizeof(tmpDate));

	return true;
}

//---------------------------------------------------------------------------
bool Buffer::extractField(Attribute* myAttr, char* dest) {

#if (DEBUG >= 2)
	cout << "Buffer::extractField(" << myAttr->getFieldName() << ")\n";
#endif

	bool result = false;

	// Clear out the destination in case we don't find anything
	memset(dest, 0, myAttr->getLength());

	if (_state != BufEol && _state != BufEof) {
		_state = BufInField;
		result = (this->*(extFunc[myAttr->whichAttr]))(myAttr,dest);
	}

#if (DEBUG >= 2)
	cout << "  Buffer::extractField() result: " << result << endl;
#endif
	return result;
}

//---------------------------------------------------------------------------
void
Buffer::consumeField(Attribute *attr)
{
#if (DEBUG >= 2)
	cout << "Buffer::consumeField(" << attr->getFieldName() << ")\n";
#endif

	if (attr->getSeparator() != NULL && _state != BufSeparator &&
	  _state != BufEol && _state != BufEof) {
		bool done = false;
		while (!done) {
			char tmpC;
			(void)getEscapedChar(tmpC, attr);
			if (tmpC == 0) done = true;
		}
	}
}

//---------------------------------------------------------------------------
void
Buffer::consumeRecord()
{
#if (DEBUG >= 2)
	cout << "Buffer::consumeRecord()\n";
#endif

// Note: this could get confused if a record separator character is in a
// string and/or escaped.

	if (_EOL == NULL) {
		return;
	} else {
		while (_state != BufEol && _state != BufEof) {
			char tmpC = getChar();
			if (tmpC == 0) {
				_state = BufEof;
			} else if (checkEOL(tmpC)) {
				_state = BufEol;
			}
		}
		if (_state == BufEol) _state = BufRecordStart;
	}
}

//---------------------------------------------------------------------------
// Returns -1 if error, otherwise the number of chars read (can be zero).
int
Buffer::ReadFieldSep(Attribute *attr, bool append)
{
#if (DEBUG >= 2)
	cout << "Buffer::ReadFieldSep(" << attr->getFieldName() << ", " <<
	  append << ")\n";
#endif

	int result = 0;

	if (!append) {
		_fieldBuf.Reset();
	}

	bool done = (_state == BufEof || _state == BufEol);
	while (!done) {
		char tmpC;
		(void)getEscapedChar(tmpC, attr);
		if (tmpC != 0) {
			// Read a "legitimate" character.
			result++;
		} else {
			done = true;
		}

		// Put the character here even if done, so that the string always
		// gets terminated.
		_fieldBuf.Put(tmpC);
	}

#if (DEBUG >= 2)
	cout << "  Field is <" << _fieldBuf.GetBuf() << ">\n";
#endif

	return result;
}

//---------------------------------------------------------------------------
// Returns -1 if error, otherwise the number of chars read (can be zero).
int
Buffer::ReadFieldQuote(Attribute *attr, bool append)
{
#if (DEBUG >= 2)
	cout << "Buffer::ReadFieldQuote(" << attr->getFieldName() << ", " <<
	  append << ")\n";
#endif

	int result = 0;

	if (!append) {
		_fieldBuf.Reset();
	}

	char quoteChar = quoteChars[attr->whichAttr];
	bool done = (_state == BufEof || _state == BufEol);

	// Make sure we have leading quote.
	if (!done) {
		char tmpC = getChar(); // *not* escaped
		if (tmpC != quoteChar) {
			cerr << "Field : " << attr->getFieldName() <<
				" doesn't have quote char : " << quoteChar << endl;
			done = true;
			result = -1;
		}
	}

	// Read until we get to the trailing quote.
	while (!done) {
		char tmpC;
		bool escaped = getEscapedChar(tmpC, NULL);
		if (tmpC == 0) {
			// Probably hit EOF.
			done = true;
			result = -1;
		} else if (!escaped && tmpC == quoteChar) {
			tmpC = 0;
			done = true;
		} else {
			// Read a "legitimate" character.
			result++;
		}

		// Put the character here even if done, so that the string always
		// gets terminated.
		_fieldBuf.Put(tmpC);
	}

	consumeField(attr);

#if (DEBUG >= 2)
	cout << "  Field is <" << _fieldBuf.GetBuf() << ">\n";
#endif

	return result;
}

//---------------------------------------------------------------------------
// Returns -1 if error, otherwise the number of chars read (can be zero).
int
Buffer::ReadFieldLen(Attribute *attr, int length, bool append)
{
#if (DEBUG >= 2)
	cout << "Buffer::ReadFieldLen(" << attr->getFieldName() << ", " <<
	  length << ", " << append << ")\n";
#endif

	int result = 0;

	if (!append) {
		_fieldBuf.Reset();
	}

	bool done = (_state == BufEof || _state == BufEol);
	while (!done) {
		char tmpC;
		(void)getEscapedChar(tmpC, NULL);
		if (tmpC != 0) {
			// Read a "legitimate" character.
			result++;
			if (result >= length) done = true;
		} else {
			// Probably hit EOF.
			done = true;
			result = -1;
		}

		// Put the character here even if done, so that the string always
		// gets terminated.
		_fieldBuf.Put(tmpC);
	}

#if (DEBUG >= 2)
	cout << "  Field is <" << _fieldBuf.GetBuf() << ">\n";
#endif

	return result;
}

//---------------------------------------------------------------------------
ostream&
operator<<(ostream &out, const BufState &state)
{
	switch (state) {
	case BufInvalid:
		out << "BufInvalid";
		break;

	case BufRecordStart:
		out << "BufRecordStart";
		break;

	case BufInComment:
		out << "BufInComment";
		break;

	case BufInField:
		out << "BufInField";
		break;

	case BufSeparator:
		out << "BufSeparator";
		break;

	case BufEol:
		out << "BufEol";
		break;

	case BufEof:
		out << "BufEof";
		break;

	default:
		cerr << "Illegal BufState value\n";
		break;
	}

	return out;
}
