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
  Revision 1.11  1998/10/12 21:24:21  wenger
  Fixed bugs 405, 406, and 408 (all DataReader problems); considerably
  increased the robustness of the DataReader (partly addresses bug 409);
  added test targets in the makefile, and corresponding data and schemas.

  Revision 1.10  1998/10/06 20:06:34  wenger
  Partially fixed bug 406 (dates sometimes work); cleaned up DataReader
  code without really changing functionality: better error handling,
  removed unused class members, added debug output, close data file (never
  closed before), various other cleanups.

  Revision 1.9  1998/10/02 17:20:01  wenger
  Fixed bug 404 (DataReader gets out-of-sync with records); made other
  cleanups and simplifications to DataReader code.

  Revision 1.8  1998/06/24 09:24:14  okan
  *** empty log message ***

  Revision 1.7  1998/06/22 23:46:59  okan
  Fixed Some bugs...

  Revision 1.6  1998/06/16 16:30:54  wenger
  Added standard headers to DataReader sources.

 */

#include "DRSchema.h"
#include "sysdep.h"
#include "DateTime.h"

ostream&
operator<<(ostream &out, const AttrType &attrType)
{
	switch (attrType) {
	case TYPE_INVALID:
		out << "Invalid";
		break;

	case TYPE_INT:
		out << "int";
		break;

	case TYPE_STRING:
		out << "string";
		break;

	case TYPE_DOUBLE:
		out << "double";
		break;

	case TYPE_DATE:
		out << "date";
		break;

	default:
		out << "illegal attribute type";
		break;
	}

	return out;
}

ostream&
operator<<(ostream &out, const Status &status)
{
	switch (status) {
	case OK:
		out << "OK";
		break;

	case FAIL:
		out << "FAIL";
		break;

	case FOUNDEOF:
		out << "FOUNDEOF";
		break;

	case FOUNDEOL:
		out << "FOUNDEOL";
		break;

	case NOQUOTE:
		out << "NOQUOTE";
		break;

	case FOUNDSEPARATOR:
		out << "FOUNDSEPARATOR";
		break;

	case FOUNDCOMMENT:
		out << "FOUNDCOMMENT";
		break;

	default:
		cerr << "Illegal status value\n";
		break;
	}

	return out;
}

Attribute::Attribute(char* fieldName) {
	_fieldName = fieldName;

	_type = TYPE_INVALID;
	_maxLen = -1;
	_quote = -1;
	_encoding = -1;
	_fieldLen = -1;
	_hidden = -1;
	_separator = NULL;
	_dateFormat = NULL;
}

Attribute::~Attribute() {
#if defined(DEBUG)
	cout << "Attribute::~Attribute(" << _fieldName << ")\n";
#endif

	delete [] _fieldName;
	_fieldName = NULL;
	delete [] _dateFormat;
	_dateFormat = NULL;

	// Note: _separator gets deleted, if necessary, in DRSchema destructor.
}

ostream&
operator<<(ostream &out, const Attribute &attr)
{
	out << "Attribute " << attr._fieldName << ":\n";
	out << "  Type: " << attr._type << endl;

	out << "  Field separator: ";
	if (attr._separator != NULL) {
		out << *attr._separator << endl;
	} else {
		out << "NULL\n";
	}

	out << "  Max. length: " << attr._maxLen << endl;
	out << "  Quote: {" << attr._quote << "} (" << (int)attr._quote << ")\n";
	out << "  Field length: " << attr._fieldLen << endl;
	out << "  Hidden: {" << attr._hidden << "} (" << (int)attr._hidden <<
	  ")\n";

	out << "  Date format: ";
	if (attr._dateFormat != NULL) {
		out << attr._dateFormat << endl;
	} else {
		out << "NULL\n";
	}

	out << "  Length: " << attr._length << endl;
	return out;
}


DRSchema::~DRSchema() {
#if defined(DEBUG)
	cout << "DRSchema::~DRSchema(" << _schemaName << ")\n";
#endif

	int i;

	delete _separator;
	_separator = NULL;

	if (tableAttr != NULL) {
		for (i = 0; i < (int)(qAttr); i++) {
			if (tableAttr[i]->getSeparator() != _separator) {
				delete tableAttr[i]->getSeparator();
				tableAttr[i]->setSeparator(NULL);
			}
			delete tableAttr[i];
			tableAttr[i] = NULL;
		}
	}
	delete [] tableAttr;
	tableAttr = NULL;

	delete _delimiter;
	_delimiter = NULL;

	delete [] _nullIf;
	_nullIf = NULL;

	delete [] _schemaName;
	_schemaName = NULL;

	delete _sorted;
	_sorted = NULL;

	delete _keys;
	_keys = NULL;
}

void DRSchema::addAttribute(Attribute* newAttr) {
	Attribute** tmpAttr = new Attribute*[qAttr+1];
	if (qAttr > 0) {
		for (int attrNum = 0 ; attrNum < (int)(qAttr); attrNum++) {
			tmpAttr[attrNum] = tableAttr[attrNum];
		}
		delete [] tableAttr;
	}
	tmpAttr[qAttr] = newAttr;
	tableAttr = tmpAttr;
	qAttr++;
}

Status DRSchema::finalizeDRSchema() {
	int curOff = 0;
	int i,j,k;
	bool compoundFound = true;
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
			case TYPE_INVALID:
				cerr << "Invalid attribute type\n";
				break;
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
			default:
				cerr << "Illegal attribute type\n";
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
			cerr << "Parse error : Only string fields can have encoding -- attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if ((tableAttr[i]->getQuote() != -1) && (tableAttr[i]->getFieldLen() != -1)) {
			cerr << "Parse error :  Attribute can't have both quote and length -- attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if ((tableAttr[i]->getSeparator() != NULL) && (tableAttr[i]->getFieldLen() != -1)) {
			cerr << "Parse error :  Attribute can't have both separator and length -- attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if ((tableAttr[i]->getMaxLen() != -1) && (tableAttr[i]->getFieldLen() != -1)) {
			cerr << "Parse error :  Attribute can't have both maxlen and length -- attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if ((tableAttr[i]->getType() != TYPE_STRING) && (tableAttr[i]->getMaxLen() != -1)) {
			cerr << "Parse error :  Only string fields can have maxlen -- attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if ((tableAttr[i]->getType() == TYPE_STRING) && (tableAttr[i]->getMaxLen() == -1 ) && (tableAttr[i]->getFieldLen() == -1)) {
			cerr << "Parse Error :  maxlen or length must be defined for string fields -- attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if ((tableAttr[i]->getType() != TYPE_STRING) && (tableAttr[i]->getQuote() != -1)) {
			cerr << "Parse error :  Only string fields can have quote -- attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if (tableAttr[i]->getType() == TYPE_INVALID) {
			cerr << "Parse error :  Type must be specified for attribute : " << tableAttr[i]->getFieldName() << endl;
			return FAIL;
		}

		if ((tableAttr[i]->getQuote() == -1) && (tableAttr[i]->getType() == TYPE_STRING) && (tableAttr[i]->getFieldLen() == -1)) {
			tableAttr[i]->setQuote(_quote);
		}
		
		if ((tableAttr[i]->getSeparator() == NULL) &&
		  (tableAttr[i]->getQuote() == -1) &&
		  (tableAttr[i]->getFieldLen() == -1)) {

			// Attribute object must 'own' the separator object
			// and its string.
			Holder *tmpSep = new Holder;
			tmpSep->data = new char[strlen(_separator->data)+1];
			strcpy(tmpSep->data, _separator->data);
			tmpSep->repeating = _separator->repeating;
			tmpSep->length = _separator->length;
			tableAttr[i]->setSeparator(tmpSep);
		}

		if (tableAttr[i]->getType() == TYPE_DATE) {

			// normalize date format
			char* tmpDF = tableAttr[i]->getDateFormat();
			if (tmpDF == NULL) {
				// Attribute object must 'own' the date format string.
				char *tmpFormat = new char[strlen(_dateFormat)+1];
				strcpy(tmpFormat, _dateFormat);
				tableAttr[i]->setDateFormat(tmpFormat);
			} else {
				if (normalizeDate(tmpDF) != OK) {
					cerr << "Default date format is not correct for : " << tableAttr[i]->getFieldName() << " !" << endl;
					return FAIL;
				} else 
					tableAttr[i]->setDateFormat(tmpDF);
			}

		}

		// TEMP -- why do we do the separator twice?  (see code a few lines
		// above this).  RKW 1998-10-13.
		if (_separator != NULL) {
			if (tableAttr[i]->getSeparator() == NULL) {
				if ((tableAttr[i]->getQuote() == -1) &&
				  (tableAttr[i]->getFieldLen() == -1)) {
					// Attribute object must 'own' the separator object
					// and its string.
					Holder *tmpSep = new Holder;
					tmpSep->data = new char[strlen(_separator->data)+1];
					strcpy(tmpSep->data, _separator->data);
					tmpSep->repeating = _separator->repeating;
					tmpSep->length = _separator->length;
					tableAttr[i]->setSeparator(tmpSep);
				}
			}
		} else {
			if ((tableAttr[i]->getQuote() == -1) && (tableAttr[i]->getFieldLen() == -1) && (tableAttr[i]->getSeparator() == NULL)) {
				cerr << "Parse Error : Separator must be specified for Attribute : " << tableAttr[i]->getFieldName() << endl ;
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
#if defined(DEBUG)
    cout << "DRSchema::normalizeDate("<< curDate << ")\n";
#endif

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

ostream&
operator<<(ostream &out, DRSchema &schema)
{
	out << "Schema " << schema._schemaName << ":\n";

	out << "  Record separator: ";
	if (schema._delimiter != NULL) {
		out << *schema._delimiter << endl;
	} else {
		out << "NULL\n";
	}

	out << "  Field separator: ";
	if (schema._separator != NULL) {
		out << *schema._separator << endl;
	} else {
		out << "NULL\n";
	}

	out << "  Comment: ";
	if (schema._comment != NULL) {
		out << *schema._comment << endl;
	} else {
		out << "NULL\n";
	}

	out << "  Quote: {" << schema._quote << "} (" << (int)schema._quote <<
	  ")\n";
	out << "  Null string: {" << schema._nullIf << "} (" <<
	  (int)schema._nullIf << ")\n";

	out << "  Date format: ";
	if (schema._dateFormat != NULL) {
		out << schema._dateFormat << endl;
	} else {
		out << "NULL\n";
	}

	out << "  Record length: " << schema._recSize << endl;

	for (int attrNum = 0; attrNum < (int)schema.qAttr; attrNum++) {
		out << *schema.tableAttr[attrNum];
	}

	return out;
}
