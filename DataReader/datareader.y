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
  $Id$

  $Log$
  Revision 1.12  1999/02/04 20:04:28  wenger
  Implemented simplified DataReader schema format; field separators
  now propagate to quoted string attributes; fixed some bugs in some
  special cases; updated documentation (still needs some work).

 */

%{
#include "DataReaderParser.h"
#include "sysdep.h"

#include <vector>
#define drparse DataReaderParser::drparse
vector<char*>* tmpVector;
char* tmpKey;
Holder * StrToHolder(char *str, bool allowRepeating);
%}

%token SCHEMA_TOKEN
%token VERSION_TOKEN
%token REC_SEP_TOKEN
%token FLD_SEP_TOKEN
%token NULLIF_TOKEN
%token MAXLEN_TOKEN
%token LENGTH_TOKEN
%token QUOTE_TOKEN
%token HIDDEN_TOKEN
%token INT_TOKEN
%token DOUBLE_TOKEN
%token STRING_TOKEN
%token DATE_TOKEN
%token DATE_FORMAT_TOKEN
%token KEY_TOKEN
%token SORTED_TOKEN
%token ENCODING_TOKEN
%token CSTYLE_TOKEN
%token QUOTEDSTYLE_TOKEN
%token URLSTYLE_TOKEN
%token NOESCAPESTYLE_TOKEN
%token COMMENT_TOKEN
%token <string_val> BRACKET_TOKEN
%token <string_val> STRINGVAL_TOKEN
%token <int_val> INTVAL_TOKEN
%token <string_val> IDENT_TOKEN
%token ';'
%token '='
%token '{'
%token '}'
%token '('
%token ')'

%%

beginschema: SCHEMA_TOKEN VERSION_TOKEN ';' getschema ;

getschema: getschemapart
		| getschema getschemapart
		;

getschemapart: getrecsep ';'
		| getcomment ';'
		| getupsep ';'
		| getupqu ';'
		| getupdateform ';'
		| getkey ';'
		| getsorted ';'
		| getattribute
		;

getrecsep: REC_SEP_TOKEN '=' BRACKET_TOKEN {
	if (myDRSchema->getDelimiter() != NULL) {
		drerror("Schema record separator is already defined, record "
		  "separator cannot be specified twice!...");
	} else {
		myDRSchema->setDelimiter(StrToHolder($3, true));
	}
} ;

getcomment: COMMENT_TOKEN '=' STRINGVAL_TOKEN {
	if (myDRSchema->getComment() != NULL) {
		drerror("Schema Comment character is already specified, comment "
		  "character can not be specified twice !...");
	} else {
		myDRSchema->setComment(StrToHolder($3, false));
	}
} ;

getupsep: FLD_SEP_TOKEN '=' BRACKET_TOKEN {
	if (myDRSchema->getSeparator() != NULL) {
		drerror("Schema level field separator is already defined, schema "
		  "level field separator can not be specified twice!...");
	} else {
		myDRSchema->setSeparator(StrToHolder($3, true));
	}
} ;

getupqu: QUOTE_TOKEN '=' BRACKET_TOKEN {
	if (myDRSchema->getQuote() != -1) {
		drerror("Schema Quote is already defined, Quote can not be "
		  "specified twice!...");
	} else {
		myDRSchema->setQuote($3[0]);
	}
} ;

getupdateform: DATE_FORMAT_TOKEN '='  STRINGVAL_TOKEN {
	char* tmpD = new char[strlen($3)+1];
	strcpy(tmpD,$3);
	myDRSchema->setDateFormat(tmpD);
} ;

getkey: KEY_TOKEN '=' '(' {
	tmpVector = new vector<char*> ;
} getkeys ')'   {
	myDRSchema->addKey(*tmpVector);
	delete tmpVector;
} ;

getsorted: SORTED_TOKEN '=' '(' {
	tmpVector = new vector<char*> ;
} getkeys ')' {
	if (myDRSchema->setSorted(tmpVector) == FAIL) {
		delete tmpVector;
		drerror("Sort order is already defined, it can not be specified "
		  "twice!...");
	}
} ;

getkeys: getkeyparts
		| getkeys ',' getkeyparts
		;

getkeyparts: IDENT_TOKEN {
	tmpKey = new char[strlen($1)+1];
	strcpy(tmpKey,$1);
	tmpVector->push_back(tmpKey);
} ;

getattribute: get_attr_type get_attr_name get_attr_maxlen
		get_attr_options ';' {
	myDRSchema->addAttribute(tmpAttr);
} ;

get_attr_type: INT_TOKEN {
	tmpAttr = new Attribute();
	tmpAttr->setType(TYPE_INT);
} | STRING_TOKEN {
	tmpAttr = new Attribute();
	tmpAttr->setType(TYPE_STRING);
} | DOUBLE_TOKEN {
	tmpAttr = new Attribute();
	tmpAttr->setType(TYPE_DOUBLE);
} | DATE_TOKEN {
	tmpAttr = new Attribute();
	tmpAttr->setType(TYPE_DATE);
} ;

get_attr_name: /* empty -- implies a "skip" attribute */
		| IDENT_TOKEN {
	if (myDRSchema->checkNameExists($1)) {
		ostrstream tmp;
		tmp << "Duplicate attribute name \'" << $1 <<
		  "\'; attribute names should be unique!..." << ends;
		drerror(tmp.str());
	} else {
		char* tmpFN = new char[strlen($1)+1];
		strcpy(tmpFN,$1);
		tmpAttr->setFieldName(tmpFN);
	}
} ;

get_attr_maxlen: /* empty */
		| BRACKET_TOKEN {
	if (tmpAttr->getMaxLen() != -1) {
		ostrstream tmp;
		tmp << "Field Max Length for : " << (tmpAttr->getFieldName() == NULL ?
		  "skip attribute": tmpAttr->getFieldName()) <<
		  " is already defined, it can not be specified twice!..." << ends;
		drerror(tmp.str());
	} else {
		tmpAttr->setMaxLen(atoi($1));
	}
} ;
							
get_attr_options: /* empty */
		| get_all_options
		;

get_all_options: get_one_option
		| get_all_options get_one_option
		;

get_one_option: getseparator
		| getmaxlen
		| getquote
		| getlength
		| gethidden
		| getencoding
		| getdateformat
		;

getseparator: FLD_SEP_TOKEN '=' BRACKET_TOKEN {
	if (tmpAttr->getSeparator() != NULL)  {
		ostrstream tmp;
		tmp << "Attribute level field separator for : " <<
		  (tmpAttr->getFieldName() == NULL ? "skip attribute":
		  tmpAttr->getFieldName()) <<
		  " is already defined, it can not be specified twice !..." << ends ;
		drerror(tmp.str());
	} else {
		tmpAttr->setSeparator(StrToHolder($3, true));
	}
} ;

getmaxlen: MAXLEN_TOKEN '=' INTVAL_TOKEN {
	if (tmpAttr->getMaxLen() != -1) {
		ostrstream tmp;
		tmp << "Field Max Length for : " << (tmpAttr->getFieldName() == NULL ?
		  "skip attribute": tmpAttr->getFieldName()) <<
		  " is already defined, it can not be specified twice!..." << ends;
		drerror(tmp.str());
	} else {
		tmpAttr->setMaxLen($3);
	}
} ;

getquote: QUOTE_TOKEN '=' BRACKET_TOKEN {
	if (tmpAttr->getQuote() != -1) {
		ostrstream tmp;
		tmp << "Quote Character for : " << (tmpAttr->getFieldName() == NULL ?
		  "skip attribute": tmpAttr->getFieldName()) <<
		  " is already defined, it can not be specified twice!..." << ends;
		drerror(tmp.str());
	} else {
		tmpAttr->setQuote($3[0]);
	}
} ;

getlength: LENGTH_TOKEN '=' INTVAL_TOKEN {
	if (tmpAttr->getFieldLen() != -1) {
		ostrstream tmp;
		tmp << "Field Length for : " << (tmpAttr->getFieldName() == NULL ?
		  "skip attribute": tmpAttr->getFieldName()) <<
		  " is already defined, it can not be specified twice!..." << ends;
		drerror(tmp.str());
	} else {
		tmpAttr->setFieldLen($3);
	}
} ;

gethidden: HIDDEN_TOKEN '=' BRACKET_TOKEN {
	if (tmpAttr->getHidden() != -1) {
		ostrstream tmp;
		tmp << "Hidden Character for : " << (tmpAttr->getFieldName() == NULL ?
		  "skip attribute": tmpAttr->getFieldName()) <<
		  " is already defined, it can not be specified twice!..." << ends;
		drerror(tmp.str());
	} else {
		char* tmpHC = new char[strlen($3)+1];
		strcpy(tmpHC,$3);
		if (!((tmpHC[0] == 'Y') || (tmpHC[0] == 'y') || (tmpHC[0] == 'N') ||
	  	(tmpHC[0] == 'n'))) {
			YYABORT;
		}

		tmpAttr->setHidden(tmpHC[0]); 
		delete [] tmpHC;
	}
} ;

getencoding: ENCODING_TOKEN '=' getwhatencod ;

getwhatencod: CSTYLE_TOKEN {tmpAttr->setEncoding(ENCODING_CSTYLE);}
		| QUOTEDSTYLE_TOKEN {tmpAttr->setEncoding(ENCODING_QUOTED);}
		| URLSTYLE_TOKEN {tmpAttr->setEncoding(ENCODING_URL);}
		| NOESCAPESTYLE_TOKEN {tmpAttr->setEncoding(ENCODING_NOESCAPE);}
		;

getdateformat: DATE_FORMAT_TOKEN '='  STRINGVAL_TOKEN {
	char* tmpD = new char[strlen($3)+1];
	strcpy(tmpD,$3);
	tmpAttr->setDateFormat(tmpD);
} ;

%%

Holder *
StrToHolder(char *str, bool allowRepeating)
{
	char* tmpChar = new char[strlen(str)+1];
	strcpy(tmpChar, str);
	Holder* tmpHolder = new Holder;

	if (allowRepeating && tmpChar[strlen(tmpChar)-1] == '+') {
		tmpHolder->repeating = true;
		tmpChar[strlen(tmpChar)-1] = '\0';
	} else {
		tmpHolder->repeating = false;
	}

	tmpHolder->data = tmpChar;
	tmpHolder->length = strlen(tmpChar);

	return tmpHolder;
}
