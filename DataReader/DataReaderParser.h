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
  Revision 1.5  1998/06/16 16:30:53  wenger
  Added standard headers to DataReader sources.

 */

#ifndef _DataReader_PARSER_H_
#define _DataReader_PARSER_H_

#include "sysdep.h"
#include <assert.h>
#include "DRSchema.h"

extern int drdebug;
#define YYSTYPE DataReaderParser::yystype

#if !defined(yyFlexLexer)
#define yyFlexLexer DataReaderFlexLexer
#include <FlexLexer.h>
#endif

class DataReaderParser
: protected DataReaderFlexLexer
{
  public:

    union yystype {
		int	tok_val;
		int	int_val;
		double  real_val;
		char* string_val;
    };

    DataReaderParser(istream* ins = 0, ostream* outs = 0) 
    : DataReaderFlexLexer(ins, outs) { 
		drdebug = 0;
	}

    ~DataReaderParser() {}

    int parse() {
      line_nr = 1;
      return drparse();
    }
	DRSchema* myDRSchema;
	void setDRSchema(DRSchema* tmpDRSchema) { myDRSchema = tmpDRSchema;}
	Attribute* tmpAttr;

  protected:

    yystype yylval;

    int line_nr;

    // This is kind of a primitive way of doing this, but it works -- the
    // old code was not fully resetting the string each time a new string
    // started, so if the previous string was longer the next string got
    // garbage on the end.
    char stringBuf[1024];
    int stringIndex;

    int yylex();

    int drparse();

    void drerror(char* msg) {
        cerr << line_nr << ": " << msg << " at " << YYText() << '\n';
    }

  private:

    DataReaderParser(const DataReaderParser& other);
    DataReaderParser& operator=(const DataReaderParser& other);
};

#endif
