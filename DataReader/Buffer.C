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

//#define DEBUG
//#define DEBUG_DATE

Buffer::Buffer(const char* fileName, DRSchema* myDRSchema, Status &status) {
#if defined(DEBUG)
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
	valFunc = new vFunc[myDRSchema->qAttr];

	_comment = myDRSchema->getComment();
	
	repeatings = new bool[myDRSchema->qAttr];
	maxLens = new int[myDRSchema->qAttr];
	fieldLens = new int[myDRSchema->qAttr];
	quoteChars = new char[myDRSchema->qAttr];
	_tmpB = new char[40];
	_tmpBStart = _tmpB;


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
					valFunc[i] = &getIntVal;
					break;
				case TYPE_DOUBLE:
					extFunc[i] = &getDoubleLen;
					valFunc[i] = &getDoubleVal;
					break;
				case TYPE_STRING:
					extFunc[i] = &getStringLen;
					valFunc[i] = &getStringVal;
					break;
				case TYPE_DATE:
					break;
				default:
					cerr << "Illegal attribute type\n";
					break;
			}
		} else if (myDRSchema->tableAttr[i]->getQuote() != -1) {
			extFunc[i] = &getStringQuote;
			valFunc[i] = &getStringVal;
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
					valFunc[i] = &getIntVal;
					break;
				case TYPE_DOUBLE:
					extFunc[i] = &getDoubleTo;
					valFunc[i] = &getDoubleVal;
					break;
				case TYPE_STRING:
					extFunc[i] = &getStringTo;
					valFunc[i] = &getStringVal;
					break;
				case TYPE_DATE:
					extFunc[i] = &getDate;
					valFunc[i] = &getDateVal;
					break;
				default:
					cerr << "Illegal attribute type\n";
					break;
			}
		}
	}

	status = OK;
}

Buffer::~Buffer() {
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

	delete [] valFunc;
	valFunc = NULL;

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

Status Buffer::setBufferPos(int cPos) {
	char c = -1;
	_in.seekg(cPos);
	c = _in.peek();
	if (c == -1) {
		cerr << "Can not set file pointer to : " << cPos << endl;
		return FAIL;
	} else {
		return OK;
	}
}

void Buffer::getDoubleVal(char* dest) {
// Returns value of _fRetVal, which is calculated by UtilAtof()

#if defined(DEBUG)
	cout << "Buffer::getDoubleVal(" << retVal << ")\n";
#endif

	// We MUST use memcpy() here in case dest is not aligned.
	memcpy(dest, &_fRetVal, sizeof(_fRetVal));
}

void Buffer::getIntVal(char* dest) {
	int retVal = 0;
	retVal = _iRetVal * (_sign == true ? -1 : 1);

#if defined(DEBUG)
	cout << "Buffer::getIntVal(" << retVal << ")\n";
#endif

	// We MUST use memcpy() here in case dest is not aligned.
	memcpy(dest, &retVal, sizeof(retVal));
}

void Buffer::getDateVal(char* dest) {
// creates an EncodedDTF from the temporary _curDate

	EncodedDTF tmpDate;

	if (_curDate.ampm == PM)
		_curDate.hour += 12;

	if (!(_curDate.adbc))
		_curDate.year *= -1;

	tmpDate.setDate(_curDate.year, _curDate.mon, _curDate.day);

	tmpDate.setTime(_curDate.hour, _curDate.min, _curDate.sec);
	
	tmpDate.setNanoSec(_curDate.nanosec);

#if defined(DEBUG)
	cout << "Buffer::getDateVal(" << tmpDate << ")\n";
#endif

	// We MUST use memcpy() here in case dest is not aligned.
	memcpy(dest, &tmpDate, sizeof(tmpDate));
}

void Buffer::getStringVal(char* dest) {
	dest[_posTarget] = '\0';
#if defined(DEBUG)
	cout << "Buffer::getStringVal(" << dest << ")\n";
#endif
	_posTarget = 0;
}

char Buffer::getChar() {
	char c = 0;
	_in.get(c);
	return c;
}

Status Buffer::checkEOL(char curChar) {

// checks if the next character sequence in the data file is
// an EOL or not, if EOL is defined as repeating, finds several
// matches

#if defined(DEBUG)
	cout << "Buffer::checkEOL('" << curChar << "')\n";
#endif

	if (_EOLCheck[curChar] == 1) {
		if (_EOL->repeating) {
			if ((curChar = getChar()) == 0) 
				return FOUNDEOF;

			while (_EOLCheck[curChar] == 1) {
				if ((curChar = getChar()) == 0)
				 	return FOUNDEOF;
			}

			_in.putback(curChar);
		}
		return FOUNDEOL;
	}
	return OK;
}

Status Buffer::checkComment() {

#if defined(DEBUG)
	cout << "Buffer::checkComment()\n";
#endif

// This is called at the beginning of the record to check if
// record begins with comment, if so skips the record till next
// delimiter, otherwise backs up ifstream and returns OK

	char tmpChar;
	Status tSt;

	if (_comment == NULL) {
		return OK;
	}

	while (true) {
		for (int i = 0; i < _comment->length; i++) {
			if ((tmpChar = getChar()) == 0) {
				return FOUNDEOF;
			}
			if (tmpChar != _comment->data[i]) {
				_in.seekg((-1)*(i+1), ios::cur);
				return OK;
			}
		}

		while (true) {

			if ((tmpChar = getChar()) == 0) {
				return FOUNDEOF;
			}

			tSt = checkEOL(tmpChar);

			if (tSt == FOUNDEOF) {
				return tSt;
			} else if (tSt == FOUNDEOL) {
				break;
			}

		}
	}

	// Should never get here.
	return FAIL;
}

Status Buffer::checkSeparator(char curChar, Attribute* myAttr) {

// checks if the next character sequence in the data file is
// a separator or not, if separator is defined as repeating, finds several
// matches

#if defined(DEBUG)
	cout << "Buffer::checkSeparator('" << curChar << "', " <<
	  myAttr->getFieldName() << ")\n";
#endif

    if (_separatorCheck[myAttr->whichAttr][curChar] == 1) {
        if (repeatings[myAttr->whichAttr]) {
            if ((curChar = getChar()) == 0)
                return FOUNDEOF;

            while (_separatorCheck[myAttr->whichAttr][curChar] == 1) {
                if ((curChar = getChar()) == 0)
                    return FOUNDEOF;
            }

            _in.putback(curChar);
        }
        return FOUNDSEPARATOR;
    }
    return OK;
}
				
Status Buffer::checkAll(char curChar, Attribute* myAttr) {

// Combination of Check EOL & Separator, added for decreasing 
// time lost in function calls

#if defined(DEBUG)
	cout << "Buffer::checkAll('" << curChar << "', " <<
	  myAttr->getFieldName() << ")\n";
#endif

	if (_EOLCheck[curChar] == 1) {
		if (_EOL->repeating) {
			if ((curChar = getChar()) == 0) 
				return FOUNDEOF;

			while (_EOLCheck[curChar] == 1) {
				if ((curChar = getChar()) == 0)
				 	return FOUNDEOF;
			}

			_in.putback(curChar);
		}
		return FOUNDEOL;
	}

	if (_separatorCheck[myAttr->whichAttr][curChar] == 1) {
		if (repeatings[myAttr->whichAttr]) {
			if ((curChar = getChar()) == 0)
				return FOUNDEOF;
				
			while (_separatorCheck[myAttr->whichAttr][curChar] == 1) {
				if ((curChar = getChar()) == 0) 
					return FOUNDEOF;
			}

			_in.putback(curChar);
		}
		return FOUNDSEPARATOR;
	}
	return OK;
}

Status Buffer::getInt(int maxValLen, int& value) {

	value = 0;
	for (int i = 0; i < maxValLen; i++) {

		if ((_curChar = getChar()) == 0) {
			return FOUNDEOF;
		}
		if (!isdigit(_curChar)) {
			// If we got to here, we presumably hit a separator character,
			// so put it back.  This allows '1/1/98', for example, to be a
			// legal date field if the format is 'm/d/y'; '01/01/98' is still
			// okay.  RKW 1998-10-09
			_in.putback(_curChar);
			return OK;
		}

		value = value*10 + (int)(_curChar - DIGITSTART);
	}
	return OK;

}

Status Buffer::getFracInt(int maxValLen, int& value) {

	value = 0;
	int i;
	for (i = 0; i < maxValLen; i++) {

		if ((_curChar = getChar()) == 0) {
			return FOUNDEOF;
		}
		if (!isdigit(_curChar)) {
			// If we got to here, we presumably hit a separator character,
			// so put it back.  This allows '1/1/98', for example, to be a
			// legal date field if the format is 'm/d/y'; '01/01/98' is still
			// okay.  RKW 1998-10-09
			_in.putback(_curChar);
			break;
		}

		value = value*10 + (int)(_curChar - DIGITSTART);
	}
	value = value * (int)(pow(10 , 6 - i));
	return OK;
}

Status Buffer::checkString(char** values, int distinct, int arrayLength, int& val) {

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
			return FOUNDEOF;
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
		return FAIL;
	}

	val = howMany[0]+1;
	temw = strlen(values[howMany[0]]);
	for (i = distinct; i < temw; i++) {

		if ((_curChar = getChar()) == 0) {
			return FOUNDEOF;
		}
		_curChar = ((int)_curChar > 90 ? (int)_curChar - 32 : _curChar);

		if (_curChar != values[howMany[0]][i]) {
			cerr << "Couldn't find given string in the array" << endl;
			return FAIL;
		}

	}
	return OK;
}

Status Buffer::getIntTo(Attribute* myAttr, char* target) {

// reads an integer from data file up to defined separator
// target : not used in this function, we have a single interface for
//          every extractor function
// myAttr : Attribute object assigned to current field

// if a non-digit value find before the separator, we ignore the rest
	bool ignoreRest = false;
	Status status;
	target++; //just to avoid compiler warning

// sign of value if the field is defined with -/+
	_sign = false;
	_iRetVal = 0;

	if ((_curChar = getChar()) == 0) {
		return FOUNDEOF;
	}

	if ((_curChar == PLUS) || (_curChar == MINUS)) {
		_sign = (_curChar == MINUS);
		if ((_curChar = getChar()) == 0) {
			return FOUNDEOF;
		}
	}

	while (true) {
		status = checkAll(_curChar, myAttr);
		if (status != OK)
			return status;

		if (!ignoreRest) {
			ignoreRest = !(isdigit(_curChar));
			_iRetVal = (ignoreRest ? _iRetVal : _iRetVal * 10 + (_curChar - DIGITSTART));
			_dataInValid = false;
		}

		if ((_curChar = getChar()) == 0) {
			return FOUNDEOF;
		}
	}
	cerr << "Couldn't Extract field : " << myAttr->getFieldName() << " from data file" << endl;
	return FAIL;
}

Status Buffer::getDoubleTo(Attribute* myAttr, char* target = NULL) {
	
// reads a double from data file up to defined separator
// this function checks e/E for exponents
// Also, this function reads the proper field and calls UtilAtof to 
// to calculate the corresponding value
// target : not used in this function, we have a single interface for
//          every extractor function
// myAttr : Attribute object assigned to current field


	Status status;
	target++; //just to compiler avoid warning

// Buffer for UtilAtof(), first reset pointer to beginning of buffer
	_tmpB = _tmpBStart;
	*_tmpB = '\0'; // in case field is not valid

	while (true) {
		if ((*_tmpB = getChar()) == 0) {
			status = FOUNDEOF;
			break;
		}

		status = checkAll(*_tmpB,myAttr);
		if (status != OK) 
			break;
		_dataInValid = false;
		_tmpB++;
	}
	_tmpB++;
	*_tmpB = '\0'; // mark end of record in case _tmpB contains results of previous
	_fRetVal = UtilAtof(_tmpBStart);
	return status;
}

Status Buffer::getStringTo(Attribute* myAttr, char* target) {

// reads a string from data file up to defined separator
// target : destination buffer to put the string read from data file
// myAttr : Attribute object assigned to current field

#if defined(DEBUG)
	cout << "Buffer::getStringTo(" << myAttr->getFieldName() << ")\n";
#endif

	_posTarget = 0;
	int maxLen = maxLens[myAttr->whichAttr];
	Status status;

	while (true) {
		if ((_curChar = getChar()) == 0) {
			return FOUNDEOF;
		}

		if (_curChar == ESCAPE) {
			if ((_curChar = getChar()) == 0) {
				return FOUNDEOF;
			}
		} else {
			status = checkAll(_curChar, myAttr);
			if (status != OK) {
				return status;
			}
		}

		if (_posTarget < maxLen) {
			target[_posTarget] = _curChar;
			_posTarget++;
			_dataInValid = false;
		}
	}
	cerr << "Couldn't Extract field : " << myAttr->getFieldName() << " from data file" << endl;
	return FAIL;
}

Status Buffer::getIntLen(Attribute* myAttr, char* target = NULL) {

// reads an integer from data file with a given length
// target : not used in this function, we have a single interface for
//          every extractor function
// myAttr : Attribute object assigned to current field

  _sign = false;
  _iRetVal = 0;   // kb: because of goofy stop condition in DataRead.c
  int fieldLen = fieldLens[myAttr->whichAttr];
  char buf[1024];
  assert( fieldLen <= (int)sizeof(buf) );
  for(int i = 0 ; i < fieldLen ; i++) {
    if( (_curChar = getChar()) == 0 ) {
      return FOUNDEOF;
    }
    buf[i] = _curChar;
  }
  buf[fieldLen] = 0;
  _iRetVal = atoi(buf);
  return FOUNDSEPARATOR;

#if defined(OLDVERSION)
//kb: this version does not handle leading spaces

	int count = 0;
	
// if a non-digit value find before the separator, we ignore the rest
	bool ignoreRest = 0;
	
	int fieldLen = fieldLens[myAttr->whichAttr];
	Status status;
	target++; //just to avoid compiler warning

// sign of value if the field is defined with -/+
	_sign = false;

	_iRetVal = 0;

	if ((_curChar = getChar()) == 0) {
		return FOUNDEOF;
	}
	
	if ((_curChar == PLUS) || (_curChar == MINUS)) {
	
		_sign = (_curChar == MINUS);
		count ++;
		if ((_curChar = getChar()) == 0) {
			return FOUNDEOF;
        }
	}
	
	while (true) {
		status = checkEOL(_curChar);
		if (status != OK) {
			return status;
        }

		if (!ignoreRest) {
			ignoreRest = !(isdigit(_curChar));
			_iRetVal = (ignoreRest ? _iRetVal : _iRetVal * 10 + (_curChar - DIGITSTART));
			_dataInValid = false;
		}

		count++;

		if (count >= fieldLen) {
			if ((_curChar = getChar()) == 0) {
				return FOUNDEOF;
			}
			status = checkEOL(_curChar);
			if (status != OK) {
				return status;
			} else {
				_in.putback(_curChar);
			}

			return FOUNDSEPARATOR;
		}

		if ((_curChar = getChar()) == 0) {
			return FOUNDEOF;
		}
	}
	cerr << "Couldn't Extract field : " << myAttr->getFieldName() << " from data file" << endl;
	return FAIL;
#endif
}

Status Buffer::getDoubleLen(Attribute* myAttr, char* target = NULL) {

// reads a double from data file with a given length
// this function checks e/E for exponents
// Also, this function reads the proper field and calls UtilAtof to 
// to calculate the corresponding value
// target : not used in this function, we have a single interface for
//          every extractor function
// myAttr : Attribute object assigned to current field

	int count = 0;

	int fieldLen = fieldLens[myAttr->whichAttr];
	Status status;
	target++; //just to avoid warning

//  _tmpB is the buffer area for UtilAtof(), first reset pointer to beginning
	_tmpB = _tmpBStart;
	*_tmpB = '\0'; // mark beginning in case we can't retrieve any value


	while (true) {
		
		if ((*_tmpB = getChar()) == 0) {
			status = FOUNDEOF;
			break;
		}

		status = checkEOL(_curChar);

		if (status != OK)
			break;

		count++;
		_dataInValid = false;

		if (count >= fieldLen) { 

			if ((_curChar = getChar()) == 0) {
				status = FOUNDEOF;
				break;
			}

			status = checkEOL(_curChar);
			if (status == OK) {
				_in.putback(_curChar);
				status = FOUNDSEPARATOR;
			}
			break;
		}
		_tmpB++;
	}
	_tmpB++;
	*_tmpB = '\0';
	_fRetVal = UtilAtof(_tmpBStart);
	return status;
}

Status Buffer::getStringLen(Attribute* myAttr, char* target) {

// reads a string from data file with a given length
// target : destination buffer to put the string read from data file
// myAttr : Attribute object assigned to current field

#if defined(DEBUG)
	cout << "Buffer::getStringLen(" << myAttr->getFieldName() << ")\n";
#endif

	_posTarget = 0;
	int fieldLen = fieldLens[myAttr->whichAttr];
	Status status;

	if ((_curChar = getChar()) == 0) 
		return FOUNDEOF;
	
	if (_curChar == ESCAPE) {
		if ((_curChar = getChar()) == 0)
			return FOUNDEOF;
	}

	target[0] = '\0'; //kb: should get null at return
	while (true) {
		status = checkEOL(_curChar);
		if (status != OK)
			return status;

		target[_posTarget] = _curChar;
		_posTarget++ ;
		_dataInValid = false;
		target[_posTarget] = '\0'; //kb: should get null at return

		if (_posTarget >= fieldLen) {
				if ((_curChar = getChar()) == 0) 
					return FOUNDEOF;
				status = checkEOL(_curChar);
				if (status != OK)
					return status;
				else
					_in.putback(_curChar);

			return FOUNDSEPARATOR;
		}

		if ((_curChar = getChar()) == 0)
			return FOUNDEOF;
		
		if (_curChar == ESCAPE) {
			if ((_curChar = getChar()) == 0)
				return FOUNDEOF;

			target[_posTarget] = _curChar;
			_posTarget++;
                        target[_posTarget] = '\0'; //kb: should get null at return

			if (_posTarget >= fieldLen) {

				if ((_curChar = getChar()) == 0) 
					return FOUNDEOF;
				status = checkEOL(_curChar);
				if (status != OK)
					return status;
				else
					_in.putback(_curChar);
			
				return FOUNDSEPARATOR;
			}

			if ((_curChar = getChar()) == 0)
				return FOUNDEOF;
		}
	}
	cerr << "Couldn't Extract field : " << myAttr->getFieldName() << " from data file" << endl;
	return FAIL;
}

Status Buffer::getStringQuote(Attribute* myAttr, char* target) {

// reads a quoted string from data file with a given quote character
// target : destination buffer to put the string read from data file
// myAttr : Attribute object assigned to current field

#if defined(DEBUG)
	cout << "Buffer::getStringQuote(" << myAttr->getFieldName() << ")\n";
#endif

	_posTarget = 0;
	int maxLen = maxLens[myAttr->whichAttr];
	int count = 0;
	char quoteChar = quoteChars[myAttr->whichAttr];

	while (true) {
		if ((_curChar = getChar()) == 0) {
			return FOUNDEOF;
		}

		if (count == 0) {
			
			// Check for quote character and skip it.
			if (_curChar != quoteChar) {
				cerr << "Field : " << myAttr->getFieldName() <<
				  " doesn't have quote char : " << quoteChar << endl;
				(void) consumeField(myAttr);
				return FAIL;
			}
			_dataInValid = false;
		} else {
			// Check for escape character and skip it if it's there.
			if (_curChar == ESCAPE) {
				if ((_curChar = getChar()) == 0) {
					return FOUNDEOF;
				}
			} else if (_curChar == quoteChar) {
				// Consume characters up to next field or record separator.
				return consumeField(myAttr);
			}

			if (_posTarget < maxLen) {
				target[_posTarget] = _curChar;
				_posTarget++;
			}
		}
		count++;
	}

	cerr << "Couldn't Extract field : " << myAttr->getFieldName() <<
	  " from data file" << endl;
	return FAIL;
}

Status Buffer::getDate(Attribute* myAttr, char* dest) {

// reads a date from data file with a date format
// target : destination buffer to put the string read from data file
// myAttr : Attribute object assigned to current field
// User may define any date component with more than one char (e.g. yyyy : year)
// To speed up extraction, user given format is converted to a different format with 
// single character for each component definition

#if defined(DEBUG)
	cout << "Buffer::getDate(" << myAttr->getFieldName() << " (" <<
	  myAttr->getDateFormat() << ") )\n";
#endif

	_curDate.day = 0;
	_curDate.mon = 0;
	_curDate.year = 0;
	_curDate.hour = 0;
	_curDate.min = 0;
	_curDate.sec = 0;
	_curDate.ampm = NONE;
	_curDate.nanosec = 0;
	_curDate.adbc = true;

	dest++; //just to avoid warning

	Status err;
	char* dFormat = myAttr->getDateFormat();
	while (*dFormat != '\0') {
#if defined(DEBUG_DATE)
		cout << "*dFormat = '" << *dFormat << "'\n";
#endif
		switch (*dFormat) {
			case 'd':
				err = getInt(2,_curDate.day);
#if defined(DEBUG_DATE)
				cout << "_curDate.day = " << _curDate.day << endl;
#endif
				if ( err != OK)
					return err;
				break;

			case 'h':
				err = getInt(2,_curDate.hour);
#if defined(DEBUG_DATE)
				cout << "_curDate.hour = " << _curDate.hour << endl;
#endif
				if (err != OK)
					return err;

				break;
			
			case 'm':
				err = getInt(2,_curDate.mon);
#if defined(DEBUG_DATE)
				cout << "_curDate.mon = " << _curDate.mon << endl;
#endif
				if (err != OK)
					return err;
				break;

			case 'i':
				err = getInt(2,_curDate.min);
#if defined(DEBUG_DATE)
				cout << "_curDate.min = " << _curDate.min << endl;
#endif
				if (err != OK)
					return err;
				break;

			case 'P':
				if ((_curChar = getChar()) == 0) {
					return FOUNDEOF;
				}

				if ((_curChar == 'A') || (_curChar == 'a')) {
					_curDate.ampm = AM;
				} else if ((_curChar == 'P') || (_curChar == 'p')) {
					_curDate.ampm = PM;
				} else {
					cerr << "Date format for: " << myAttr->getFieldName() <<
					  " doesn't match the given date format: " <<
					  myAttr->getDateFormat() << endl;
					return FAIL;
				}

				if ((_curChar = getChar()) == 0) {
					return FOUNDEOF;
				}
				if ((_curChar != 'M') && (_curChar != 'm')) {
					cerr << "Date format for: " << myAttr->getFieldName() <<
					  " doesn't match the given date format: " <<
					  myAttr->getDateFormat() << endl;
					return FAIL;
				}
#if defined(DEBUG_DATE)
				cout << "_curDate.ampm = " << _curDate.ampm << endl;
#endif

				break;

			case 's':
				err = getInt(2,_curDate.sec);
#if defined(DEBUG_DATE)
				cout << "_curDate.sec = " << _curDate.sec << endl;
#endif
				if (err != OK)
					return err;
				break;

			case 'f':
				//TEMP -- does this really mean nanosec or microsec????
				// RKW 1998-10-09
				// TEMPANS -- microsec
				err = getFracInt(6,_curDate.nanosec);
#if defined(DEBUG_DATE)
				cout << "_curDate.nanosec = " << _curDate.nanosec << endl;
#endif
				if (err != OK)
					return err;
				break;
				
			case 'y':
				err = getInt(2,_curDate.year);
#if defined(DEBUG_DATE)
				cout << "_curDate.year = " << _curDate.year << endl;
#endif
				if (err != OK)
					return err;
				_curDate.year += 1900;
				break;

			case 'Y':
				err = getInt(4,_curDate.year);
#if defined(DEBUG_DATE)
				cout << "_curDate.year = " << _curDate.year << endl;
#endif
				if (err != OK)
					return err;
				break;

			case 'B':
				
				if ((_curChar = getChar()) == 0) {
					return FOUNDEOF;
				}

				if ((_curChar == 'B') || (_curChar == 'b')) {
					_curDate.adbc= false;
				} else if ((_curChar == 'A') || (_curChar == 'a')) {
					_curDate.adbc = true;
				} else {
					cerr << "Date format for: " << myAttr->getFieldName() <<
					  " doesn't match the given date format: " <<
					  myAttr->getDateFormat() << endl;
					return FAIL;
				}

				if ((_curChar = getChar()) == 0) {
					return FOUNDEOF;
				}
				if ((_curChar != 'D') && (_curChar != 'd') && (_curChar != 'C') && (_curChar != 'c')) {
					cerr << "Date format for: " << myAttr->getFieldName() <<
					  " doesn't match the given date format: " <<
					  myAttr->getDateFormat() << endl;
					return FAIL;
				}
#if defined(DEBUG_DATE)
				cout << "_curDate.adbc = " << _curDate.adbc << endl;
#endif
			
				break;

			case 'M':
				err = checkString(_months,3,12,_curDate.mon);
#if defined(DEBUG_DATE)
				cout << "_curDate.mon = " << _curDate.mon << endl;
#endif
				if (err != OK)
					return err;
				break;

			case 'N':
				err = checkString(_monthAbbr,3,12,_curDate.mon);
#if defined(DEBUG_DATE)
				cout << "_curDate.mon = " << _curDate.mon << endl;
#endif
				if (err != OK)
					return err;
				break;
				
			default:
				if ((_curChar = getChar()) == 0) {
					return FOUNDEOF;
				}
				if (_curChar != *dFormat) {
					cerr << "Date format for: " << myAttr->getFieldName() <<
					  " doesn't match the given date format: " <<
					  myAttr->getDateFormat() << endl;
					return FAIL;
				}
		}
		_dataInValid = false;
		dFormat++;
	}

	if (myAttr->getSeparator() != NULL) {
		if ((_curChar = getChar()) == 0) {
			return FOUNDEOF;
		}
		if ((err = checkEOL(_curChar)) != OK)
			return err;

		err = checkSeparator(_curChar,myAttr);
		if (err != OK)
			return err;
		else {
			cerr << "Couldn't find separator for : " << myAttr->getFieldName() << endl;
			return FAIL;
		}
	}
	return OK;

}

Status Buffer::extractField(Attribute* myAttr, char* dest) {

#if defined(DEBUG)
	cout << "Buffer::extractField(" << myAttr->getFieldName() << ")\n";
#endif

	Status retSt;

	// Clear out the destination in case we don't find anything
	memset(dest, 0, myAttr->getLength());

	retSt = (this->*(extFunc[myAttr->whichAttr]))(myAttr,dest);
// kb: this should only happen if extract was successful
	(this->*(valFunc[myAttr->whichAttr]))(dest);

#if defined(DEBUG)
	cout << "  Buffer::extractField() retSt: " << retSt << endl;
#endif
	return retSt;
}

Status
Buffer::consumeField(Attribute *attr)
{
#if defined(DEBUG)
	cout << "Buffer::consumeField()\n";
#endif

	if (attr->getSeparator() == NULL) {
		return OK;
	} else {
		char curChar;
		while (true) {
			curChar = getChar();
			Status status;
			if (curChar == 0) {
				return FOUNDEOF;
			} else if ((status = checkAll(curChar, attr)) != OK) {
				return status;
			}
		}
	}
}

Status Buffer::consumeRecord(Status tmpS) {
#if defined(DEBUG)
	cout << "Buffer::consumeRecord()\n";
#endif

// Note: this could get confused if a record separator character is in a
// string and/or escaped.

	if (_EOL == NULL) {
		return tmpS;
	} else {
		Status status;
		while ((status = checkEOL(getChar())) == OK) {}
		return status;
	}
}
