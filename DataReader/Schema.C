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
 */

#include "DRSchema.h"
#include "sysdep.h"
#include "DateTime.h"

Attribute::Attribute(char* fieldName) {
	_fieldName = fieldName;

	_type = -1;
	_maxLen = -1;
	_quote = -1;
	_encoding = -1;
	_fieldLen = -1;
	_hidden = -1;
	_separator = NULL;
	_dateFormat = NULL;
}

Attribute::Attribute() {
	_type = -1;
	_maxLen = -1;
	_quote = -1;
	_encoding = -1;
	_fieldLen = -1;
	_hidden = -1;
	_separator = NULL;
	_fieldName = NULL; 
	_dateFormat = NULL;
}

Attribute::~Attribute() {

	if (_fieldName != NULL)
		delete [] _fieldName;
	if (_dateFormat != NULL)
		delete [] _dateFormat;

	if (_separator != NULL) {
		if (_separator->data != NULL)
			delete [] _separator->data;
	}

}

DRSchema::~DRSchema() {
	int i;
	tAttr = 0;

	if (tableAttr != NULL) {
		for (i = 0; i < (int)(qAttr); i++) {
			if ((tableAttr[i]->getSeparator() != _separator) && (tableAttr[i]->getSeparator() != NULL)) {
				if (tableAttr[i]->getSeparator()->data != NULL)
					delete [] tableAttr[i]->getSeparator()->data;
				delete tableAttr[i]->getSeparator();
			}
			delete tableAttr[i];
		}
	}
	delete [] tableAttr;
	if (_delimiter != NULL) {
		if (_delimiter->data != NULL) 
			delete [] _delimiter->data;
		delete _delimiter;
	}

	if (_separator != NULL) {
		if (_separator->data != NULL)
			delete [] _separator->data;
		delete _separator;
	}
	if (_nullIf != NULL)
		delete [] _nullIf;
	delete [] _schemaName;
	if (_sorted != NULL) 
		delete _sorted;
	if (_keys != NULL)
		delete _keys;
}

void DRSchema::addAttribute(Attribute* newAttr) {
	int i=0;
	if (qAttr == 0) {
		tableAttr = new Attribute*[1];
		tableAttr[0] = newAttr;
		qAttr = 1;
	} else {
		Attribute** tmpAttr;
		tmpAttr = new Attribute*[qAttr+1];
		for (i = 0 ; i < (int)(qAttr); i++)
			tmpAttr[i] = tableAttr[i];
		tmpAttr[qAttr] = newAttr;
		delete [] tableAttr;
		tableAttr = tmpAttr;
		qAttr++;
	}
}

Status DRSchema::finalizeDRSchema() {
	int i,j,k;
	bool compoundFound = true;
	int curOff = 0;
	int fLen = 0;
	_recSize = 0;

	// normalize default date format
	if (_dateFormat != NULL) {
		if (normalizeDate(_dateFormat) != OK) {
			cerr << "Default Date format is not correct for : " << tableAttr[i]->getFieldName() << " !" << endl;
			return FAIL;
		}
	}

	if (_delimiter == NULL) { //default delimiter = [\n]+
		char* tmpC = new char[2];
		tmpC[0] = '\n';
		tmpC[1] = '\0';
		_delimiter = new Holder();
		_delimiter->length = 1;
		_delimiter->data = tmpC;
		_delimiter->repeating = true;
	}
	

	if (_separator == NULL) { //default separator = [\t ]+
		char* tmpC = new char[2];
		tmpC[0] = '\t';
		tmpC[1] = ' ';
		tmpC[2] = '\0';
		_separator = new Holder();
		_separator->length = 2;
		_separator->data = tmpC;
		_separator->repeating = true;
	}
	
	for (i = 0; i < (int)(qAttr); i++) {
		
		switch (tableAttr[i]->getType()) {
			case TYPE_INT:
				fLen = sizeof(int);
				break;
			case TYPE_STRING:
				if (tableAttr[i]->getMaxLen() > 0)
					fLen = tableAttr[i]->getMaxLen() + 1;
				else
					fLen = tableAttr[i]->getFieldLen() + 1;
				break;
			case TYPE_DATE:
				fLen = sizeof(EncodedDTF);
				break;
			case TYPE_DOUBLE:
				fLen = sizeof(double);
				break;
		}

		if (tableAttr[i]->getFieldName() != NULL) {
			tAttr++;
		}

		// calculate record length, length of fields and offsets of fields in
		// the destination buffer
		tableAttr[i]->offset = curOff;
		curOff += fLen;
		tableAttr[i]->setLength(fLen);
		_recSize += fLen;
				
		if ((tableAttr[i]->getType() != TYPE_STRING) && (tableAttr[i]->getEncoding() != -1)) {
			cerr << "Parse Error : Only String fields can have Encoding -- Attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if ((tableAttr[i]->getQuote() != -1) && (tableAttr[i]->getFieldLen() != -1)) {
			cerr << "Parse Error :  Attribute can't have both Quote and Length -- Attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if ((tableAttr[i]->getSeparator() != NULL) && (tableAttr[i]->getFieldLen() != -1)) {
			cerr << "Parse Error :  Attribute can't have both Separator and Length -- Attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if ((tableAttr[i]->getMaxLen() != -1) && (tableAttr[i]->getFieldLen() != -1)) {
			cerr << "Parse Error :  Attribute can't have both Max Length and Length -- Attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if ((tableAttr[i]->getType() != TYPE_STRING) && (tableAttr[i]->getMaxLen() != -1)) {
			cerr << "Parse Error :  Only String Fields can have Max Length -- Attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if ((tableAttr[i]->getType() == TYPE_STRING) && (tableAttr[i]->getMaxLen() == -1 ) && (tableAttr[i]->getFieldLen() == -1)) {
			cerr << "Parse Error :  MaxLen or Field Length should be defined for String Fields -- Attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if ((tableAttr[i]->getType() != TYPE_STRING) && (tableAttr[i]->getQuote() != -1)) {
			cerr << "Parse Error :  Only String Fields can have Quote -- Attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if (tableAttr[i]->getType() == -1) {
			cerr << "Parse Error :  Type should be specified for Attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if ((tableAttr[i]->getQuote() == -1) && (tableAttr[i]->getType() == TYPE_STRING) && (tableAttr[i]->getFieldLen() == -1)) {
			tableAttr[i]->setQuote(_quote);
		}
		
		if ((tableAttr[i]->getSeparator() == NULL) && (tableAttr[i]->getQuote() == -1) && (tableAttr[i]->getFieldLen() == -1)) {
			tableAttr[i]->setSeparator(_separator);
		}

		if (tableAttr[i]->getType() == TYPE_DATE) {

			// normalize date format
			char* tmpDF = tableAttr[i]->getDateFormat();
			if (tmpDF == NULL) {
				tableAttr[i]->setDateFormat(_dateFormat);
			} else {
				if (normalizeDate(tmpDF) != OK) {
					cerr << "Default Date format is not correct for : " << tableAttr[i]->getFieldName() << " !" << endl;
					return FAIL;
				} else 
					tableAttr[i]->setDateFormat(tmpDF);
			}

		}

		if (_separator != NULL) {
			if (tableAttr[i]->getSeparator() == NULL) {
				if ((tableAttr[i]->getQuote() == -1) && (tableAttr[i]->getFieldLen() == -1))
					tableAttr[i]->setSeparator(_separator);
			}
		} else {
			if ((tableAttr[i]->getQuote() == -1) && (tableAttr[i]->getFieldLen() == -1) && (tableAttr[i]->getSeparator() == NULL)) {
				cerr << "Parse Error : Separator should be specified for Attribute : " << tableAttr[i]->getFieldName() << endl ;
				return FAIL;
			}
		}

		tableAttr[i]->whichAttr = i;

		// will take care of Keys and sorted
	}
	vector<int>* tmpVector;
	
	if (_stageKeys != NULL) {
		_keys = new vector< vector<int> >;
		
		for (i = 0; i < (int)(_stageKeys->size()); i++) {
		
			tmpVector = new vector<int>;
			for (j = 0; j <  (int)((*_stageKeys)[i].size()) ; j++) {
				compoundFound = false;

				for (k = 0 ; k < (int)(qAttr); k++) {

					if (strcmp((*_stageKeys)[i][j] , tableAttr[k]->getFieldName()) == 0) {
						tmpVector->push_back(k);
						compoundFound = true;
						break;
					}
				}
				if (!compoundFound) {
                	cerr << "Parse Error : Key not found for : " << (*_stageKeys)[i][j] << endl ;
				    return FAIL;
				}
			}
			_keys->push_back(*tmpVector);
			delete tmpVector;
		}
		delete _stageKeys;
	}

	if (_stageSorted != NULL) {
		_sorted = new vector<int>;
		for (j = 0 ; j < (int)(_stageSorted->size()) ; j++) {
			compoundFound = false;

			for (k = 0; k < (int)(qAttr) ; k++) {
				if (strcmp((*_stageSorted)[j], tableAttr[k]->getFieldName()) == 0) {
					_sorted->push_back(k);
					compoundFound = true;
					break;
				}
			}
			if (!compoundFound) {
                cerr << "Parse Error : Sorted not found for : " << (*_stageSorted)[j] << endl ;
				return FAIL;
			}
		}
		delete _stageSorted;
	}

	return OK;
}

Status DRSchema::normalizeDate(char*& curDate) {
	char* tmp = curDate;
	char* boundary = curDate + strlen(curDate) - 1;
	ostringstream tmpString;

	while (*tmp != '\0') {
		switch (*tmp) {
			case 'B':
				tmpString << *tmp;
				tmp++;
				break;

			case 'D':
				tmp++;
				if (tmp > boundary) {
					cerr << "Parse Error! Date Format is incomplete !..." << endl ;
					return FAIL;
				}

				if (*tmp != 'D') {
					cerr << "Parse Error! Date Format is incorrect, it should be : DD" << endl;
					return FAIL;
				}
				tmpString << 'd' ;
				tmp++ ;
				break;

			case 'f':
				tmp++;
				if (tmp > boundary) {
					cerr << "Parse Error! Date Format is incomplete !..." << endl ;
					return FAIL;
				}
				
				if (*tmp != 'f') {
					cerr << "Parse Error! Date Format is incorrect, it should be : ff" << endl;
					return FAIL;
				}
				tmpString << 'f' ;
				tmp++;
				break;

			case 'h':
				tmp++;
				if (tmp > boundary) {
					cerr << "Parse Error! Date Format is incomplete !..." << endl ;
					return FAIL;
				}
				
				if (*tmp != 'h') {
					cerr << "Parse Error! Date Format is incorrect, it should be : hh" << endl;
					return FAIL;
				}
				tmpString << 'h' ;
				tmp++;
				break;

			case 'm':
				tmp++;
				if (tmp > boundary) {
					cerr << "Parse Error! Date Format is incomplete !..." << endl ;
					return FAIL;
				}
				
				if (*tmp != 'm') {
					cerr << "Parse Error! Date Format is incorrect, it should be : mm" << endl;
					return FAIL;
				}
				tmpString << 'i' ;
				tmp++;
				break;
				
			case 'M':
				tmp++;
				if (tmp > boundary) {
					cerr << "Parse Error! Date Format is incomplete !..." << endl ;
					return FAIL;
				}
				
				if (*tmp == 'M') {
					tmpString << 'm' ;
					tmp++;
				} else if (*tmp == 'O') {
					tmp++;
					if (tmp > boundary) {
						cerr << "Parse Error! Date Format is incomplete !..." << endl ;
						return FAIL;
					}
				
					if (*tmp != 'N') {
						cerr << "Parse Error! Date Format is incorrect" << endl;
						return FAIL;
					}
					tmp++;
					if (tmp > boundary) {
						tmpString << 'N' ;
						break;
					}
				
					if (*tmp == 'T') {
						tmp++;
						if (tmp > boundary) {
							cerr << "Parse Error! Date Format is incomplete !..." << endl ;
							return FAIL;
						}
				
						if (*tmp != 'H') {
							cerr << "Parse Error! Date Format is incorrect" << endl;
							return FAIL;
						}
						tmpString << 'M';
						tmp++;
					} else {
						tmpString << 'N';
					}
				} else {
					cerr << "Parse Error! Date Format is incorrect" << endl;
					return FAIL;
				}
				break;

			case 'P':
				tmpString << 'P';
				tmp++;
				break;

			case 's':
				tmp++;
				if (tmp > boundary) {
					cerr << "Parse Error! Date Format is incomplete !..." << endl ;
					return FAIL;
				}
				
				if (*tmp != 's') {
					cerr << "Parse Error! Date Format is incorrect, it should be : ss" << endl;
					return FAIL;
				}
				tmpString << 's' ;
				tmp++;
				break;

			case 'Y':
				tmp++;
				if (tmp > boundary) {
					cerr << "Parse Error! Date Format is incomplete !..." << endl ;
					return FAIL;
				}
				
				if (*tmp != 'Y') {
					cerr << "Parse Error! Date Format is incorrect, it should be : YY or YYYY" << endl;
					return FAIL;
				}
				tmp++;
				if (tmp > boundary) {
					tmpString << 'y';
					break;
				}
				
				if (*tmp != 'Y') {
					tmpString << 'y';
				} else {
					if (*tmp != 'Y') {
						cerr << "Parse Error! Date Format is incorrect, it should be : YY or YYYY" << endl;
						return FAIL;
					}
					tmp++;
					if (tmp > boundary) {
						cerr << "Parse Error! Date Format is incomplete !..." << endl ;
						return FAIL;
					}
				
					if (*tmp != 'Y') {
						cerr << "Parse Error! Date Format is incorrect, it should be : YY or YYYY" << endl;
						return FAIL;
					}
					tmp++;
					tmpString << 'Y';
					break;
				}

			default:
				tmp++;
				tmpString << *(tmp-1); // anything, skip this
		}
	}
	tmpString << '\0';
	char* tmp1 = curDate;
	curDate = new char[strlen(tmpString.str())+1];
	strcpy(curDate,tmpString.str());
	delete [] tmp1;
	return OK;
}
