%{
#include "DataReaderParser.h"
#include "sysdep.h"

#include <vector>
#define drparse DataReaderParser::drparse
vector<char*>* tmpVector;
char* tmpKey;
%}

%token SCHEMA_TOKEN
%token DELIM_TOKEN
%token SEPARATOR_TOKEN
%token NULLIF_TOKEN
%token ATTR_TOKEN
%token SKIP_TOKEN
%token MAXLEN_TOKEN
%token LENGTH_TOKEN
%token QUOTE_TOKEN
%token HIDDEN_TOKEN
%token TYPE_TOKEN
%token INT_TOKEN
%token DOUBLE_TOKEN
%token STRING_TOKEN
%token DATE_TOKEN
%token DATE_FORMAT
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

beginschema: SCHEMA_TOKEN getident '{' getschema '}' ;

getident:	IDENT_TOKEN {
	if (myDRSchema->getDRSchemaName() != NULL)  {
		ostrstream tmp;
		tmp << "Schema Name : " << myDRSchema->getDRSchemaName() <<
		  " is already defined, schema name can not be entered twice!..."
		  << ends;
		drerror(tmp.str());
	}
	char* tmpSCN = new char[strlen($1)+1];
	strcpy(tmpSCN,$1);
	myDRSchema->setDRSchemaName(tmpSCN); 
} ;

getschema: getanyschema
		| getschema getanyschema
		;

getanyschema: getdelim ';'
		| getcomment ';'
		| getupsep ';'
		| getupqu ';'
		| getkey ';'
		| getsorted ';'
		| getattribute
		;

getdelim: DELIM_TOKEN '=' BRACKET_TOKEN {
	if (myDRSchema->getDelimiter() != NULL) {
		drerror("Schema Delimiter is already defined, delimiter cannot be "
		  "specified twice!...");
	}
	if ($3 == NULL) {
		drerror("DRSchema Delimiter NULL ??? ") ;
	}
	char* tmpChar = new char[strlen($3)+1];
	strcpy(tmpChar,$3);
	Holder* tmpHolder = new Holder;

    if (tmpChar[strlen(tmpChar)-1] == '+') {
		tmpHolder->repeating = true;
		tmpChar[strlen(tmpChar)-1] = '\0';
	} else {
		tmpHolder->repeating = false;
	}

	tmpHolder->data = tmpChar;
	tmpHolder->length = strlen(tmpChar);
	myDRSchema->setDelimiter(tmpHolder);
} ;

getcomment: COMMENT_TOKEN '=' STRINGVAL_TOKEN {
	if (myDRSchema->getComment() != NULL) {
		drerror("Schema Comment character is already specified, comment "
		  "character can not be specified twice!...");
	}
	if ($3 == NULL) {
		drerror("DRSchema Comment NULL ??? ") ;
	}
	char* tmpCom = new char[strlen($3)+1];
	strcpy(tmpCom,$3);
	Holder* tmpHolderC = new Holder;

	tmpHolderC->repeating = false;
	tmpHolderC->data = tmpCom;
	tmpHolderC->length = strlen(tmpCom);
	myDRSchema->setComment(tmpHolderC);
} ;

getupsep: SEPARATOR_TOKEN '=' BRACKET_TOKEN {
	if (myDRSchema->getSeparator() != NULL) {
		drerror("Schema Level Separator is already defined, Schema Level "
		  "Separator can not be specified twice!...");
	}
	char* tmpSChar = new char[strlen($3)+1];
	strcpy(tmpSChar,$3);
	Holder* tmpHolder = new Holder;

	if (tmpSChar[strlen(tmpSChar)-1]  == '+') {
		tmpHolder->repeating = true;
		tmpSChar[strlen(tmpSChar)-1] = '\0';
	} else {
		tmpHolder->repeating = false;
	}

	tmpHolder->data = tmpSChar;
	tmpHolder->length = strlen(tmpSChar);
	myDRSchema->setSeparator(tmpHolder);
} ;

getupqu: QUOTE_TOKEN '=' BRACKET_TOKEN {
	if (myDRSchema->getQuote() != -1) {
		drerror("Schema Quote is already defined, Quote can not be "
		  "specified twice!...");
	}
	char* tmpQChar = new char[strlen($3)+1];
	strcpy(tmpQChar,$3);

	myDRSchema->setQuote(tmpQChar[0]);
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

getattribute: getattrtype '{' getattributeoption '}' {
	myDRSchema->addAttribute(tmpAttr);
} ;

getattrtype: ATTR_TOKEN getattributename
		| SKIP_TOKEN {	tmpAttr = new Attribute();}
		;

getattributename: IDENT_TOKEN {
	char* tmpFN = new char[strlen($1)+1];
	strcpy(tmpFN,$1);
	if (myDRSchema->checkNameExists(tmpFN)) {
		ostrstream tmp;
		tmp << "Duplicate Attribute Name at : \'" << tmpFN <<
		  "\' , Attribute Names should be unique!..." << ends;
		drerror(tmp.str());
	}
	tmpAttr = new Attribute(tmpFN); 
} ;
							
getattributeoption: getanyattribute
		| getattributeoption getanyattribute
		;

getanyattribute: getseparator ';'
		| gettype ';'
		| getmaxlen ';'
		| getquote ';'
		| getlength ';'
		| gethidden ';'
		| getencoding ';'
		| getdateformat ';'
		;

getseparator: SEPARATOR_TOKEN '=' BRACKET_TOKEN {
	if (tmpAttr->getSeparator() != NULL)  {
		ostrstream tmp;
		tmp << "Attribute Level Separator for : " <<
		  (tmpAttr->getFieldName() == NULL ? "skip attribute" :
		  tmpAttr->getFieldName()) <<
		  " is already defined, it can not be specified twice!..." << ends ;
		drerror(tmp.str());
	}
	char* tmpS = new char[strlen($3)+1];
	strcpy(tmpS,$3);
	Holder* tmpH= new Holder;

	if (tmpS[strlen(tmpS)-1] == '+') {
		tmpH->repeating = true;
		tmpS[strlen(tmpS)-1] = '\0';
	} else {
		tmpH->repeating = false;
	}

	tmpH->data = tmpS;
	tmpH->length = strlen(tmpS);
	tmpAttr->setSeparator(tmpH);
} ;

gettype: TYPE_TOKEN '=' getwhattype ;

getwhattype: INT_TOKEN {tmpAttr->setType(TYPE_INT);}
		| STRING_TOKEN {tmpAttr->setType(TYPE_STRING);}
		| DOUBLE_TOKEN {tmpAttr->setType(TYPE_DOUBLE);}
		| DATE_TOKEN {tmpAttr->setType(TYPE_DATE);}
		;

getmaxlen: MAXLEN_TOKEN '=' INTVAL_TOKEN {
	if (tmpAttr->getMaxLen() != -1) {
		ostrstream tmp;
		tmp << "Field Max Length for : " << (tmpAttr->getFieldName() == NULL ?
		  "skip attribute": tmpAttr->getFieldName()) <<
		  " is already defined, it can not be specified twice!..." << ends;
		drerror(tmp.str());
	}
	tmpAttr->setMaxLen($3);
} ;

getquote: QUOTE_TOKEN '=' BRACKET_TOKEN {
	if (tmpAttr->getQuote() != -1) {
		ostrstream tmp;
		tmp << "Quote Character for : " << (tmpAttr->getFieldName() == NULL ?
		  "skip attribute": tmpAttr->getFieldName()) <<
		  " is already defined, it can not be specified twice!..." << ends;
		drerror(tmp.str());
	}
	char* tmpQ = new char[strlen($3)+1];
	strcpy(tmpQ,$3);
	tmpAttr->setQuote(tmpQ[0]);
	delete [] tmpQ;
} ;

getlength: LENGTH_TOKEN '=' INTVAL_TOKEN {
	if (tmpAttr->getFieldLen() != -1) {
		ostrstream tmp;
		tmp << "Field Length for : " << (tmpAttr->getFieldName() == NULL ?
		  "skip attribute": tmpAttr->getFieldName()) <<
		  " is already defined, it can not be specified twice!..." << ends;
		drerror(tmp.str());
	}
	tmpAttr->setFieldLen($3);
} ;

gethidden: HIDDEN_TOKEN '=' BRACKET_TOKEN {
	if (tmpAttr->getHidden() != -1) {
		ostrstream tmp;
		tmp << "Hidden Character for : " << (tmpAttr->getFieldName() == NULL ?
		  "skip attribute": tmpAttr->getFieldName()) <<
		  " is already defined, it can not be specified twice!..." << ends;
		drerror(tmp.str());
	}
	char* tmpHC = new char[strlen($3)+1];
	strcpy(tmpHC,$3);
	if (!((tmpHC[0] == 'Y') || (tmpHC[0] == 'y') || (tmpHC[0] == 'N') ||
	  (tmpHC[0] == 'n'))) {
		YYABORT;
	}

	tmpAttr->setHidden(tmpHC[0]); 
	delete [] tmpHC;
} ;

getencoding: ENCODING_TOKEN '=' getwhatencod ;

getwhatencod: CSTYLE_TOKEN {tmpAttr->setEncoding(ENCODING_CSTYLE);}
		| QUOTEDSTYLE_TOKEN {tmpAttr->setEncoding(ENCODING_QUOTED);}
		| URLSTYLE_TOKEN {tmpAttr->setEncoding(ENCODING_URL);}
		| NOESCAPESTYLE_TOKEN {tmpAttr->setEncoding(ENCODING_NOESCAPE);}
		;

getdateformat: DATE_FORMAT '='  STRINGVAL_TOKEN {
	char* tmpD = new char[strlen($3)+1];
	strcpy(tmpD,$3);
	tmpAttr->setDateFormat(tmpD);
} ;

%%
