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
  Revision 1.6  1998/06/23 22:57:11  okan
  *** empty log message ***

  Revision 1.5  1998/06/22 23:46:58  okan
  Fixed Some bugs...

  Revision 1.4  1998/06/16 16:30:51  wenger
  Added standard headers to DataReader sources.

 */

#include "Buffer.h"
#include <math.h>
#include "DateTime.h"

Buffer::Buffer(char* fileName, DRSchema* myDRSchema) {
	int i,j;
	Holder* temArr;
	_nAttr = myDRSchema->qAttr;
	_fileName = fileName;
	_in.open(_fileName);
	if (_in.fail()) {
		cerr << "DataRedaer File : " << _fileName << " not found" << endl;
		exit(1);
	}

	// Initialize EOLCheck to make faster EOL comparison
	_EOLCheck = new char[128];
	for (i = 0 ; i<128 ; i++) 
		_EOLCheck[i] = 0;
	
	if (myDRSchema->getDelimiter() != NULL) {
		_EOL = myDRSchema->getDelimiter();
		for (i = 0 ; i < _EOL->length ; i++) {
			_EOLCheck[_EOL->data[i]] = 1;
		}
	}

	_separatorCheck = new char*[myDRSchema->qAttr];
	_curDate = new DateInfo;

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
		}

		maxLens[i] = myDRSchema->tableAttr[i]->getMaxLen();
		fieldLens[i] = myDRSchema->tableAttr[i]->getFieldLen();
		quoteChars[i] = myDRSchema->tableAttr[i]->getQuote();

		if (myDRSchema->tableAttr[i]->getFieldLen() != -1) {
			switch (myDRSchema->tableAttr[i]->getType())	{
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
					valFunc[i] = &getPosTarget;
					break;
			}
		} else if (myDRSchema->tableAttr[i]->getQuote() != -1) {
			extFunc[i] = &getStringQuote;
			valFunc[i] = &getPosTarget;
		} else {
			switch (myDRSchema->tableAttr[i]->getType()) {
				case TYPE_INT:
					extFunc[i] = getIntTo;
					valFunc[i] = getIntVal;
					break;
				case TYPE_DOUBLE:
					extFunc[i] = &getDoubleTo;
					valFunc[i] = &getDoubleVal;
					break;
				case TYPE_STRING:
					extFunc[i] = &getStringTo;
					valFunc[i] = &getPosTarget;
					break;
				case TYPE_DATE:
					extFunc[i] = &getDate;
					valFunc[i] = &getDateVal;
					break;
			}
		}
	}
}

Buffer::~Buffer() {
	int i ;

	if (_EOLCheck != NULL)
		delete [] _EOLCheck;
	
	if (_separatorCheck != NULL) {
		for (i = 0 ; i < _nAttr ; i++) {
			if (_separatorCheck[i] != NULL)
				delete [] _separatorCheck[i];
		}
		delete [] _separatorCheck;
	}
	delete [] extFunc;
	delete [] valFunc;

	delete [] _months;
	delete [] _monthAbbr;

	delete [] repeatings; 
	delete [] maxLens;
	delete [] fieldLens;
	delete [] quoteChars;
}

Status Buffer::setBufferPos(int cPos) {
	char c = -1;
	_in.seekg(cPos);
	c = _in.peek();
	if (c == -1) {
		cerr << "Can not set file pointer to : " << cPos << endl;
		return FAIL;
	} else 
		return OK;
}

void Buffer::getDoubleVal(char* dest) {
// Double fields have two components : Mantissa and Exponent
// Double extractor functions stores the Mantissa value in Exponent first
// and if this function finds an exponent, moves Mantissa value from Exponent
// variable to _fRetVal

	double retVal = 0;
	double retExp = 0;
	if (_fRetVal == 0) 
		retVal = _exponent * (_sign == true ? -1.0 : 1.0);
	else {
		retVal = _fRetVal * (_sign == true ? -1.0 : 1.0);
		retExp = _exponent * (_expSign == true ? -1.0 : 1.0);
		retVal = (retVal == 0 ? 1 : retVal * pow(10.0,retExp));
	}
	*(double*)dest = retVal;
}
void Buffer::getIntVal(char* dest) {
	int retVal = 0;
	retVal = _iRetVal * (_sign == true ? -1 : 1);
	*(int*)dest = retVal;
}

void Buffer::getDateVal(char* dest) {
// creates an EncodedDTF from the temporary _curDate

	EncodedDTF* tmpDate = new EncodedDTF();

	if (_curDate->ampm == PM)
		_curDate->hour += 12;

	if (!(_curDate->adbc))
		_curDate->year *= -1;

	
	tmpDate->setDate(_curDate->year, _curDate->mon, _curDate->day);

	tmpDate->setTime(_curDate->hour, _curDate->min, _curDate->sec);
	
	tmpDate->setNanoSec(_curDate->nanosec);
	
	*(EncodedDTF*)dest = *tmpDate;
	delete tmpDate;
	
}

void Buffer::getPosTarget(char* dest) {
	dest[_posTarget] = '\0';
}

char Buffer::getChar() {
	char c = 0;
	_in.get(c);
	return c;
}

bool Buffer::isDigit(char c) {
	if ((c >= DIGITSTART) && (c <= DIGITEND))
		return true;
	return false;
}

Status Buffer::checkEOL(char curChar) {

// checks if the next character sequence in the data file is
// an EOL or not, if EOL is defined as repeating, finds several
// matches

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

Status Buffer::checkSeparator(char curChar, Attribute* myAttr) {

// checks if the next character sequence in the data file is
// a separator or not, if separator is defined as repeating, finds several
// matches

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

Status Buffer::getInt(int valLen, int& value) {

	value = 0;
	for (int i = 0; i < valLen; i++) {

		if ((_curChar = getChar()) == 0) {
			return FOUNDEOF;
		}
		if (!isDigit(_curChar))
			return OK;

		value = value*10 + (int)(_curChar - DIGITSTART);
	}
	return OK;

}

Status Buffer::checkString(char** values, int distinct, int arrayLength, int& val) {

// This function finds the given (value of current field) string in an array of strings, e.g.
// for months array, finds the index of given month in the months array
// values : given string array, distinct : how many characters do we need to
// distinguish an element in this array
// arrayLength : number of elements in the values array
// val : index of given string in the array
	
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
			ignoreRest = !(isDigit(_curChar));
			_iRetVal = (ignoreRest ? _iRetVal : _iRetVal * 10 + (_curChar - DIGITSTART));
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
// target : not used in this function, we have a single interface for
//          every extractor function
// myAttr : Attribute object assigned to current field

	double fraction = 10 , dividend = 1 , gross = 1;

// if a non-digit value find before the separator, we ignore the rest

	bool ignoreRest = false;
	int stage = 0;
	Status status;
	target++; //just to compiler avoid warning

// sign of value if the field and exponent are defined with -/+
	_sign = false;
	_expSign = false;

	_fRetVal = 0;
	_exponent = 0;

	if ((_curChar = getChar()) == 0)
		return FOUNDEOF;

	if ((_curChar == PLUS) || (_curChar == MINUS)) {
		_sign = (_curChar == MINUS) ;
		if ((_curChar = getChar()) == 0) 
			return FOUNDEOF;
	}

	while (true) {
		
		status = checkAll(_curChar,myAttr);
		if (status != OK) 
			return status;

		if (!ignoreRest) {
			if (!isDigit(_curChar)) {
				if (_curChar == DOT) {
					if ((stage != 0) && (stage != 2)) 
						ignoreRest = true;
					else {
						fraction = 1;
						dividend = 10;
						gross = 10;
						stage = 1;
					}
				} else if ((_curChar == EXPONENTS) || (_curChar == EXPONENTC)) {
					if ((stage != 1) && (stage != 0))
						ignoreRest = true;
					else {
						if ((_curChar = getChar()) == 0) 
							return FOUNDEOF;

						if ((_curChar == MINUS) || (_curChar == PLUS)) {
							_expSign = (_curChar == MINUS) ;
						} else {
							_in.putback(_curChar);
						}
						_fRetVal = _exponent;
						fraction = 10;
						_exponent = 0;
						dividend = 1;
						gross = 1;
						stage = 2;
					}
				} else 
					ignoreRest = true;
			} else {
				_exponent = (_exponent* fraction) + (((double)(_curChar - DIGITSTART)) / dividend);
				dividend *= gross;
			}
		}
		if ((_curChar = getChar()) == 0) 
			return FOUNDEOF;
	}
	cerr << "Couldn't Extract field : " << myAttr->getFieldName() << " from data file" << endl;
	return FAIL;
}

Status Buffer::getStringTo(Attribute* myAttr, char* target) {

// reads a string from data file up to defined separator
// target : destination buffer to put the string read from data file
// myAttr : Attribute object assigned to current field

	_posTarget = 0;
	int maxLen = maxLens[myAttr->whichAttr];
	int count = 0;
	Status status;

	if ((_curChar = getChar()) == 0)
		return FOUNDEOF;

	if (_curChar == ESCAPE) {
		if ((_curChar = getChar()) == 0)
			return FOUNDEOF;
	}

	while (true) {

		status = checkAll(_curChar, myAttr);
		if (status != OK) 
			return status;

		target[_posTarget] = _curChar;
		_posTarget++;

		count++;

		if ((_curChar = getChar()) == 0)
			return FOUNDEOF;

		if (count >= maxLen) {
			return FOUNDSEPARATOR;
		}

		if (_curChar == ESCAPE) {
			if ((_curChar = getChar()) == 0)
				return FOUNDEOF;

			target[_posTarget] = _curChar;
			_posTarget++;
			count++;
			
			if (count >= maxLen) {
				return FOUNDSEPARATOR;
			}

			if ((_curChar = getChar()) == 0)
				return FOUNDEOF;
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

	int count = 0;
	
// if a non-digit value find before the separator, we ignore the rest
	bool ignoreRest = 0;
	
	int fieldLen = fieldLens[myAttr->whichAttr];
	Status status;
	target++; //just to avoid compiler warning

// sign of value if the field is defined with -/+
	_sign = false;

	_iRetVal = 0;

	if ((_curChar = getChar()) == 0)
		return FOUNDEOF;
	
	if ((_curChar == PLUS) || (_curChar == MINUS)) {
	
		_sign = (_curChar == MINUS);
		count ++;
		if ((_curChar = getChar()) == 0)
			return FOUNDEOF;

	}
	
	while (true) {
		status = checkEOL(_curChar);
		if (status != OK)
			return status;

		if (!ignoreRest) {
			ignoreRest = !(isDigit(_curChar));
			_iRetVal = (ignoreRest ? _iRetVal : _iRetVal * 10 + (_curChar - DIGITSTART));
		}

		count++;

		if (count >= fieldLen) {
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
	cerr << "Couldn't Extract field : " << myAttr->getFieldName() << " from data file" << endl;
	return FAIL;
}

Status Buffer::getDoubleLen(Attribute* myAttr, char* target = NULL) {



// reads a double from data file with a given length
// this function checks e/E for exponents
// target : not used in this function, we have a single interface for
//          every extractor function
// myAttr : Attribute object assigned to current field

	double fraction = 10, dividend = 1, gross = 1;
	int count = 0;

// if a non-digit value find before the separator, we ignore the rest
	bool ignoreRest = false;
	int stage = 0;
	int fieldLen = fieldLens[myAttr->whichAttr];
	Status status;
	target++; //just to avoid warning

// sign of value if the field and exponent are defined with -/+
	_sign = false;
	_expSign = 0;
	_fRetVal = 0;
	_exponent = 0;

	if ((_curChar = getChar()) == 0)
		return FOUNDEOF;
	
	if ((_curChar == PLUS) || (_curChar == MINUS)) {
		_sign = (_curChar == MINUS);
		count++;
		if ((_curChar = getChar()) == 0)
			return FOUNDEOF;
	}

	while (true) {
		status = checkEOL(_curChar);

		if (status != OK)
			return status;

		if (!ignoreRest) {
			if (!isDigit(_curChar)) {
				if (_curChar == DOT) {
					if ((stage != 0) && (stage != 2)) 
						ignoreRest = true;
					else {
						fraction = 1;
						dividend = 10;
						gross = 10;
						stage = 1;
					}
				} else if ((_curChar == EXPONENTS) || (_curChar == EXPONENTC)) {
					if ((stage != 1) && (stage != 0)) 
						ignoreRest = true;
					else {
						if ((_curChar = getChar()) == 0)
							return FOUNDEOF;

						if ((_curChar == MINUS) || (_curChar == PLUS)) {
							_expSign = (_curChar == MINUS);
							count++;
						} else
							_in.putback(_curChar);
						
						_fRetVal = _exponent;
						_exponent = 0;
						fraction = 10;
						dividend = 1;
						gross = 1;
						stage = 2;
					}
				} else 
					ignoreRest = true;
			} else {
				_exponent = (_exponent * fraction) + (((double)(_curChar - DIGITSTART)) / dividend);
				dividend *= gross;
			}
		}

		count++;

		if (count >= fieldLen) {
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
	cerr << "Couldn't Extract field : " << myAttr->getFieldName() << " from data file" << endl;
	return FAIL;
}

Status Buffer::getStringLen(Attribute* myAttr, char* target) {

// reads a string from data file with a given length
// target : destination buffer to put the string read from data file
// myAttr : Attribute object assigned to current field

	_posTarget = 0;
	int fieldLen = fieldLens[myAttr->whichAttr];
	Status status;

	if ((_curChar = getChar()) == 0) 
		return FOUNDEOF;
	
	if (_curChar == ESCAPE) {
		if ((_curChar = getChar()) == 0)
			return FOUNDEOF;
	}

	while (true) {
		status = checkEOL(_curChar);
		if (status != OK)
			return status;

		target[_posTarget] = _curChar;
		_posTarget++ ;

		if (_posTarget > fieldLen) {
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

			if (_posTarget > fieldLen) {

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

	_posTarget = 0;
	bool ignoreRest = false;
	int maxLen = maxLens[myAttr->whichAttr];
	int count = 0;
	char quoteChar = quoteChars[myAttr->whichAttr];
	Status status;

	if ((_curChar = getChar()) == 0)
		return FOUNDEOF;
	
	if (_curChar != quoteChar) {
		cerr << "Field : " << myAttr->getFieldName() << " doesn't have quote char : " << quoteChar << endl;
		return FAIL;
	} else {
		if ((_curChar = getChar()) == 0)
			return FOUNDEOF;
	}

	while (true) {
		count ++;
		if (_curChar == quoteChar) {
			if (myAttr->getSeparator() != NULL) {
				while (true) {
					if ((_curChar = getChar()) == 0)
						return FOUNDEOF;

					status = checkAll(_curChar, myAttr);
					if (status != OK)
						return status;
				}
			} else 

				if ((_curChar = getChar()) == 0)
					return FOUNDEOF;

				status = checkEOL(_curChar);
				if (status == OK)
					return FOUNDSEPARATOR;
				else
					return status;
		}

		if (!ignoreRest) {
			target[_posTarget] = _curChar;
			_posTarget++;
		}

		if (count >= maxLen) 
			ignoreRest = true;

		if ((_curChar = getChar()) == 0)
			return FOUNDEOF;

		if (_curChar == ESCAPE) {
			if ((_curChar = getChar()) == 0)
				return FOUNDEOF;

			if (!ignoreRest) {
				target[_posTarget] = _curChar;
				_posTarget++;
			}

			count++;

			if (count >= maxLen)
				ignoreRest = true;

			if ((_curChar = getChar()) == 0)
				return FOUNDEOF;

		}
	}
	cerr << "Couldn't Extract field : " << myAttr->getFieldName() << " from data file" << endl;
	return FAIL;
}

Status Buffer::getDate(Attribute* myAttr, char* dest) {

// reads a date from data file with a date format
// target : destination buffer to put the string read from data file
// myAttr : Attribute object assigned to current field
// User may define any date component with more than one char (e.g. yyyy : year)
// To speed up extraction, user given format is converted to a different format with 
// single character for each component definition

	_curDate->day = 0;
	_curDate->mon = 0;
	_curDate->year = 0;
	_curDate->hour = 0;
	_curDate->min = 0;
	_curDate->sec = 0;
	_curDate->ampm = NONE;
	_curDate->nanosec = 0;
	_curDate->adbc = true;

	dest++; //just to avoid warning

	Status err;
	char* dFormat = myAttr->getDateFormat();
	while (*dFormat != '\0') {
		switch (*dFormat) {
			case 'd':
				err = getInt(2,_curDate->day);
				if ( err != OK)
					return err;
				break;

			case 'h':
				err = getInt(2,_curDate->hour);
				if (err != OK)
					return err;

				break;
			
			case 'm':
				err = getInt(2,_curDate->mon);
				if (err != OK)
					return err;
				break;

			case 'i':
				err = getInt(2,_curDate->min);
				if (err != OK)
					return err;
				break;

			case 'P':
				if ((_curChar = getChar()) == 0) {
					return FOUNDEOF;
				}

				if ((_curChar == 'A') || (_curChar == 'a')) {
					_curDate->ampm = AM;
				} else if ((_curChar == 'P') || (_curChar == 'p')) {
					_curDate->ampm = PM;
				} else {
					cerr << "Date Format for : " << myAttr->getFieldName() << " doesn't match the given date format : " << endl;
					return FAIL;
				}

				if ((_curChar = getChar()) == 0) {
					return FOUNDEOF;
				}
				if ((_curChar != 'M') && (_curChar != 'm')) {
					cerr << "Date Format for : " << myAttr->getFieldName() << " doesn't match the given date format : " << endl;
					return FAIL;
				}

				break;

			case 's':
				err = getInt(2,_curDate->sec);
				if (err != OK)
					return err;
				break;

			case 'f':
				err = getInt(6,_curDate->nanosec);
				if (err != OK)
					return err;
				break;
				
			case 'y':
				err = getInt(2,_curDate->year);
				if (err != OK)
					return err;
				_curDate->year += 1900;
				break;

			case 'Y':
				err = getInt(4,_curDate->year);
				if (err != OK)
					return err;
				break;

			case 'B':
				
				if ((_curChar = getChar()) == 0) {
					return FOUNDEOF;
				}

				if ((_curChar == 'B') || (_curChar == 'b')) {
					_curDate->adbc= false;
				} else if ((_curChar == 'A') || (_curChar == 'a')) {
					_curDate->adbc = true;
				} else {
					cerr << "Date Format for : " << myAttr->getFieldName() << " doesn't match the given date format : " << endl;
					return FAIL;
				}

				if ((_curChar = getChar()) == 0) {
					return FOUNDEOF;
				}
				if ((_curChar != 'D') && (_curChar != 'd') && (_curChar != 'C') && (_curChar != 'c')) {
					cerr << "Date Format for : " << myAttr->getFieldName() << " doesn't match the given date format : " << endl;
					return FAIL;
				}
			
				break;

			case 'M':
				err = checkString(_months,3,12,_curDate->mon);
				if (err != OK)
					return err;
				break;

			case 'N':
				err = checkString(_monthAbbr,3,12,_curDate->mon);
				if (err != OK)
					return err;
				break;
				
			default:
				if ((_curChar = getChar()) == 0) {
					return FOUNDEOF;
				}
				if (_curChar != *dFormat) {
					cerr << "Date Format for : " << myAttr->getFieldName() << " doesn't match the given date format : " << endl;
					return FAIL;
				}
		}
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
	Status retSt;
	retSt = extFunc[myAttr->whichAttr](myAttr,dest);
	valFunc[myAttr->whichAttr](dest);
	return retSt;
}
