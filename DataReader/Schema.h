#ifndef DATAREADER_SCHEMA_H
#define DATAREADER_SCHEMA_H

#include "sysdep.h"
#include <vector>

// Constants used for several definitions

enum types { TYPE_INT, TYPE_STRING, TYPE_DOUBLE, TYPE_DATE };

#define DIGITSTART 48
#define DIGITEND 57
#define MINUS 45 //ASCII value of '-'
#define PLUS 43 //ASCII value of '+'
#define DOT 46 //ASCII value of '.'
#define ESCAPE 92 //ASCII value of '\'
#define EXPONENTC 101 //ASCII value of 'e'
#define EXPONENTS 69 //ASCII value of 'E'

enum Status { OK, FAIL, FOUNDEOF, FOUNDEOL, NOQUOTE, FOUNDSEPARATOR };

enum Encoding { ENCODING_CSTYLE, ENCODING_QUOTED, ENCODING_URL, ENCODING_NOESCAPE };

// Struct used for defining EOL and separator
struct Holder {
	char* data; // separator or EOL string
	bool repeating; // if this repeating ?
	int length; // length of data
};

// This class defines attributes of each field
class Attribute {

private:
	Holder* _separator; // Separator for this field
	int _type; // Type of field
	int _maxLen; // Max Length of this field, used for buffer allocation
	char _quote; // Quote character
	int _fieldLen; // Length of this field if this is a fixed length field
	char _hidden; // Is this field Hidden ?
	char _encoding; // What is the encoding style if this is a string field, not implemented yet
	char* _fieldName; // Name of this field
	char* _dateFormat; // Date format
	size_t _length; // Length of field

public:

	int whichAttr; // index of attribute in the attribute array
	int offset; // Offset of this field in the destination buffer
	Attribute(char* fieldName);
	Attribute();

	~Attribute();

	Holder* getSeparator() {
		return _separator;
	}

	void setSeparator(Holder* separator) {
		_separator = separator;
	}

	int getType() {
		return _type;
	}

	void setType(int type) {

		_type = type;
	}

	int getMaxLen() {
		return _maxLen;
	}

	void setMaxLen(int maxLen) {
		_maxLen = maxLen;
	}

	int getLength() {
		return _length;
	}

	void setLength(int length) {
		_length = length;
	}

	char getQuote() {
		return _quote;
	}

	void setQuote(char quote) {
		_quote = quote;
	}

	int getFieldLen() {
		return _fieldLen;
	}

	void setFieldLen(int fieldLen) {
		_fieldLen = fieldLen;
	}

	char getHidden() {
		return _hidden;
	}

	void setHidden(char hidden) {
		_hidden = hidden;
	}

	char* getFieldName() {
		return _fieldName;
	}

	void setFieldName(char* fieldName) {
		_fieldName = fieldName;
	}

	char getEncoding() {
		return _encoding;
	}

	void setEncoding(char encoding) {
		_encoding = encoding;
	}

	void setDateFormat(char* dateFormat) {
		_dateFormat = dateFormat;
	}

	char* getDateFormat() {
		return _dateFormat;
	}
 
};

// This class holds information about schema and an array of attributes
class Schema {
private:
	Holder* _delimiter; // Record delimiter
	Holder* _separator; // Default separator
	char* _fileName; // Schema file name
	char* _nullIf; // null string, not implemented yet
	char* _schemaName; // Name of the schema
	char* _dateFormat; // Default date format

	vector< vector<char*> >* _stageKeys;
	vector<char*>* _stageSorted;

	// Keys of this data file, this is used by the upper layer
	// there may be more than one key
	vector< vector<int> >* _keys;
	
	// Sort order of this table, this is used by the upper layer
	vector<int>* _sorted;

	size_t _recSize; // size of a record

public:
	Attribute** tableAttr; // Attribute array holds info for each attribute
	size_t qAttr; // Number of attributes in this schema
	size_t tAttr; // Number of attributes - number of skip attributes

	Schema(char* fileName) {
		_fileName = fileName;
		qAttr = 0;
		_delimiter = NULL;
		_separator = NULL;
		_nullIf = NULL;
		_schemaName = NULL;
		tableAttr = NULL;
		_keys = NULL;
		_sorted = NULL;
		_stageKeys = NULL;
		_stageSorted = NULL;
		_dateFormat = NULL;
	}

	~Schema();

	void addAttribute(Attribute* newAttr); // add new attribute to schema
	void addKey(vector<char*> inVector) { // add new key to schema
		if (_stageKeys == NULL) 
			_stageKeys = new vector< vector<char*> >;
		_stageKeys->push_back(inVector);
	}

	Status setSorted(vector<char*>* inVector) { // set order of table
		if (_stageSorted != NULL) 
			return FAIL;
		_stageSorted = inVector;
		return OK;
	}

	vector< vector<int> >* getKeys() { // get keys 
		return _keys;
	}

	vector<int>* getSorted() { // get sort order
		return _sorted;
	}

	// this function is used for checking errors in the schema file
	// also makes some adjustments such as setting each attributes 
	// separator to default separator
	Status finalizeSchema();

	Holder* getDelimiter() {
		return _delimiter;
	}
			
	void setDelimiter(Holder* delimiter) {
		_delimiter = delimiter;
	}

	int getRecSize() {
		return _recSize;
	}

	void setRecSize(int recSize) {
		_recSize = recSize;
	}

	Holder* getSeparator() {
		return _separator;
	}

	void setSeparator(Holder* separator) {
		_separator = separator;
	}

	char* getFileName() {
		return _fileName;
	}

	void setFileName(char* fileName) {
		_fileName = fileName;
	}

	char* getSchemaName() {
		return _schemaName;
	}

	void setSchemaName(char* schemaName) {
		_schemaName = schemaName;
	}
	
	char* getDateFormat() {
		return _dateFormat;
	}

	void setDateFormat(char* dateFormat) {
		_dateFormat = dateFormat;
	}

	char* getNullIf() {
		return _nullIf;
	}

	void setNullIf(char* nullIf) {
		_nullIf = nullIf;
	}

	// Date Format defined by the user and used by the date extractor function are different
	// for simplicity, user can define multi-character formats for date components (eg : yyyy for year)
	// but for speeding up the extraction from data file, these are converted to single character formats
	// for each component. 
	Status normalizeDate(char*& curDate);
	
};
#endif
