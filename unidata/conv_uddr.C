/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  UniData to DataReader converter.  Uses the UniData library to parse
  a UniData schema, and then writes it out in DataReader 2.0 format.
 */

/*
  $Id$

  $Log$
 */

#include <iostream.h>
#include <fstream.h>
#include <string>
#include <strstream.h>

#include "UniData.h"

#define DEBUG 0

enum ConvState { cSInvalid, cSOk, cSPartFail, cSFail };

inline ConvState
max(ConvState s1, ConvState s2)
{
	if ((int)s1 > (int)s2) {
		return (ConvState)s1;
	} else {
		return (ConvState)s2;
	}
}

inline bool
semiok(ConvState s)
{
	return (s == cSOk) || (s == cSPartFail);
}

//-----------------------------------------------------------------------------
ConvState
OutputDateFormat(char *dateFormat, ostrstream &attrStr)
{
	ConvState result = cSOk;

	attrStr << "  date_format = \"";

	char *curChar = dateFormat;

	while (*curChar != '\0') {
		if (*curChar == '%') {
			curChar++;
			if (*curChar == '\0') {
				cerr << "Improper termination of date_format string " <<
				  dateFormat << endl;
				result = cSFail;
				break;
			} else {
				switch (*curChar) {
				case 'b': // abbreviated month name, eg, "Feb"
				case 'B': // full month name, eg, "February"
				case 'd': // Day of month as integer (0-31)
				case 'H': // Hour as integer (00-23)
				case 'm': // Month as integer (01-12)
				case 'M': // Minute as integer (00-59)
				case 'P': // AM/PM
				case 'S': // second as integer (00-61)
				case 'y': // year as integer w/o century (00-99)
				case 'Y': // year as integer w/ century, eg, 1999
				case '%': // literal %
					attrStr << '%' << *curChar;
					break;

				default:
					cerr << "Unsupported date format code: %" << *curChar <<
					  endl;
					attrStr << "<<<error: %" << *curChar << ">>>";
					result = max(result, cSPartFail);
					break;
				}
			}
		} else if (*curChar == '"') {
			attrStr << "\\\"";
		} else {
			attrStr << *curChar;
		}
		curChar++;
	}
	attrStr << "\"";

	return result;
}

//-----------------------------------------------------------------------------
ConvState
OutputAttr(Attr *attr0, Attr *attr, ofstream &scOut)
{
	ConvState result = cSOk;

	//
	// Check for things we can't handle in the DataReader.
	//
	if (attr->func_of()) {
		cerr << "Attribute " << attr->flat_name() <<
		  " is a function of other attributes; this feature is not "
		  "supported by the DataReader.\n";
		result = cSFail;
	}

	if (attr->uses_perl() || attr->format() || attr->value() ||
	  attr->filter() || attr->reader()) {
		cerr << "Attribute " << attr->flat_name() <<
		  " uses Perl; this feature is not supported by the DataReader.\n";
		result = cSFail;
	}

	if (attr->traits()->is_set(Unique_Trait)) {
		cerr << "Attribute " << attr->flat_name() <<
		  " has unsupported trait unique\n";
	}
	if (attr->traits()->is_set(Consecutive_Trait)) {
		cerr << "Attribute " << attr->flat_name() <<
		  " has unsupported trait consecutive\n";
	}
	if (attr->traits()->is_set(Null_Allowed_Trait)) {
		cerr << "Attribute " << attr->flat_name() <<
		  " has unsupported trait null_allowed\n";
	}

	ostrstream attrStr;

	//
	// Output attribute type.
	//
	if (semiok(result)) {
		switch (attr->type()) {
		case Int_Attr:
			attrStr << "int ";
			break;

		case Float_Attr: // DataReader currently doesn't have float,
						 // so make it double.
		case Double_Attr:
			attrStr << "double ";
			break;

		case String_Attr:
			attrStr << "string ";
			break;

		case UnixTime_Attr:
		case DateTime_Attr:
			attrStr << "date ";
			break;

		default:
			cerr << "Unknown or invalid attribute type for attribute " <<
		  	  attr->flat_name() << endl;
			result = cSFail;
			break;
		}
	}

	//
	// Output attribute name.
	//
	if (semiok(result)) {
		attrStr << attr->flat_name();
	}

	//
	// Output attribute options.
	//
	if (semiok(result)) {
		if (attr->have_lpos() && attr->have_rpos()) {
			attrStr << "  length = " << attr->rpos() - attr->lpos() + 1;
		}

		if (attr->maxlen() > 0) {
			attrStr << "  maxlen = " << attr->maxlen();
		}

		if (!attr->quote_is_default()) {
			attrStr << "  quote = [" << attr->quote() << "]";
		}

		if (attr->seperator() && strlen(attr->seperator()) > 0
		  && (!attr0->seperator() ||
		  strcmp(attr->seperator(), attr0->seperator()))) {
			attrStr << "  field_separator = [" << attr->seperator() << "]";
		}
		if (attr->whitespace() && strlen(attr->whitespace()) > 0
		  && (!attr0->whitespace() ||
		  strcmp(attr->whitespace(), attr0->whitespace()))) {
			attrStr << "  field_separator = [" << attr->whitespace() << "]+";
		}

		if (attr->date_format()) {
			OutputDateFormat(attr->date_format(), attrStr);
		}

		if (attr->traits()->is_set(Sorted_Trait)) {
			attrStr << "  <<<error: sorted>>>";
			cerr << "Attribute " << attr->flat_name() <<
			  " is declared sorted.\n";
			cerr << "This is not currently supported by the conversion "
			  "program.\n";
			cerr << "Edit the output schema manually.\n";
			result = max(result, cSPartFail);
		}
	}


	if (semiok(result)) {
		attrStr << ends;
		scOut << attrStr.str() << ";\n";
	}

	return result;
}

//-----------------------------------------------------------------------------
ConvState
OutputSchema(Schema *schema, ofstream &scOut)
{
	ConvState result = cSOk;


	//
	// Check for things we can't handle in the DataReader.
	//
	if (schema->getType() != Schema::TEXT) {
		cerr << "Schema is not of type text; DataReader cannot handle "
		  "non-text data\n";
		result = cSFail;
	} else if (schema->getCharSet() != Schema::ASCII) {
		cerr << "Schema has non-ASCII character set; DataReader cannot "
		  "handle non-ASCII data\n";
		result = cSFail;
	} else if (schema->converter()) {
		cerr << "Schema has a converter; not supported by DataReader\n";
		result = cSFail;
	}

	int reclen;
	if (schema->getRecLen(reclen) != Schema::VAR_LEN) {
		cerr << "Schema uses fixed-length record specification not "
		  "supported by DataReader\n";
		result = max(result, cSPartFail);
	}

	Attr *attr0 = schema->attr0();
	if (attr0->uses_perl() || attr0->format() || attr0->value() ||
	  attr0->filter() || attr0->reader()) {
		cerr << "Schema uses Perl; this feature is not supported by "
		  "the DataReader.\n";
		result = cSFail;
	}

	//
	// DataReader header.
	//
	scOut << "datareader 2.0;\n\n";

	//
	// Schema-level options.
	//
	CmmtStk *comments = schema->comments();
	if (comments->nCmmts() > 1) {
		cerr << "Schema has multiple comments; only the first will be used\n";
		result = max(result, cSPartFail);
	}
	if (comments->nCmmts() > 0) {
		Comment *comment = comments->ith(0);
		if (strcmp(comment->end, "\n")) {
			cerr << "Schema has multi-part comment not supported by the "
			  "DataReader\n";
			result = max(result, cSPartFail);
		} else {
			scOut << "comment = \"" << comment->init << "\";\n";
		}
	}

	if (attr0->delimiter() && strlen(attr0->delimiter()) > 0) {
		// Note: assuming repeating here.
		scOut << "record_separator = [" << attr0->delimiter() << "]+\n";
	}

	if (attr0->seperator() && strlen(attr0->seperator()) > 0) {
		scOut << "field_separator = [" << attr0->seperator() << "];\n";
	}

	if (attr0->whitespace() && strlen(attr0->whitespace()) > 0) {
		scOut << "field_separator = [" << attr0->whitespace() << "]+;\n";
	}

	if (!attr0->quote_is_default()) {
		scOut << "quote = [" << attr0->quote() << "];\n";
	}

	scOut << endl;

	//
	// Attributes.
	//
	if (semiok(result)) {
		for (int attrNum = 0; attrNum < schema->NumFlatAttrs(); attrNum++) {
			Attr *attr = schema->GetFlatAttr(attrNum);
			result = max(result, OutputAttr(attr0, attr, scOut));
		}
	}

	return result;
}

//-----------------------------------------------------------------------------
ConvState
ConvertSchema(char *udFile, char *drFile)
{
#if (DEBUG >= 1)
	cout << "Converting UniData schema " << udFile <<
	  " to DataReader schema " << drFile << endl;
#endif

	ConvState result = cSOk;

	// Note: nothing happens to the dataFile except that it gets opened --
	// just picking the schema file because we know it exists.
	char *dataFile = udFile;

	UniData ud(dataFile, udFile);
	if (!ud.isOk()) {
		cerr << "Error creating UniData object for schema " <<
		  udFile << endl;
		result = cSFail;
	} else {
		Schema *schema = ud.schema();

		if (!schema->valid()) {
			cerr << "Invalid UniData schema object for schema " <<
			  udFile << endl;
			result = cSFail;
		} else {
			ofstream scOut(drFile);
			if (!scOut) {
				cerr << "Unable to open/create DataReader schema file " <<
			  	drFile << endl;
				result = cSFail;
			} else {
				result = OutputSchema(schema, scOut);
			}
		}
	}

	return result;
}

//-----------------------------------------------------------------------------
// Note: I'm only allowing conversion of one schema at a time in this
// program because UniData sometimes core dumps if it can't parse a
// schema.  RKW 1999-02-04.
int
main(int argc, char **argv)
{
	int result = 0;

	if (argc != 3) {
		cout << "UniData to DataReader schema converter\n";
		cout << "Usage: conv_uddr <unidata schema file> "
		  "<datareader schema file>\n";
	} else {
		ConvState tmpResult = ConvertSchema(argv[1], argv[2]);
		if (tmpResult == cSOk) {
			result = 0;
		} else if (tmpResult == cSPartFail) {
			result = 1;
		} else {
			result = 2;
		}
	}

	return result;
}

/*============================================================================*/
