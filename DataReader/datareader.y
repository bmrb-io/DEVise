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
%token <string_val> BRACKET_TOKEN
%token <string_val> STRING_TOKEN
%token <int_val> INTVAL_TOKEN
%token <string_val> IDENT_TOKEN
%token <string_val> CONFIRM_TOKEN
%token ';'
%token '='
%token '{'
%token '}'
%token '('
%token ')'

%%

beginschema: SCHEMA_TOKEN getident '{' getschema '}' ;

getident:	IDENT_TOKEN { if (mySchema->getSchemaName() != NULL) 
								drerror("Schema name can't be entered twice!");
						  char* tmpSCN = new char[strlen($1)+1];
						  strcpy(tmpSCN,$1);
						  mySchema->setSchemaName(tmpSCN); 
						} ;

getschema: getanyschema
           | getschema getanyschema
		   ;

getanyschema: getdelim ';'
              | getupsep ';'
			  | getkey ';'
			  | getsorted ';'
			  | getattribute
			  ;
getdelim: DELIM_TOKEN '=' BRACKET_TOKEN {	if (mySchema->getDelimiter() != NULL)
												drerror("Schema Delimiter can't be entered twice !...");
											if ($3 == NULL)
												drerror("Schema Delimiter NULL ??? ") ;
											char* tmpChar = new char[strlen($3)+1];
											strcpy(tmpChar,$3);
                                            Holder* tmpHolder = new Holder;

                                            if (tmpChar[strlen(tmpChar)-1] == '+') {
                                                tmpHolder->repeating = true;
                                                tmpChar[strlen(tmpChar)-1] = '\0';
                                            } else
                                                tmpHolder->repeating = false;

                                            tmpHolder->data = tmpChar;
                                            tmpHolder->length = strlen(tmpChar);
                                            mySchema->setDelimiter(tmpHolder);
                                        }
			;
getupsep: SEPARATOR_TOKEN '=' BRACKET_TOKEN {
											if (mySchema->getSeparator() != NULL) 
												drerror("Schema Separator can't be entered twice!...");
											char* tmpSChar = new char[strlen($3)+1];
											strcpy(tmpSChar,$3);
											Holder* tmpHolder = new Holder;

											if (tmpSChar[strlen(tmpSChar)-1]  == '+') {
												tmpHolder->repeating = true;
												tmpSChar[strlen(tmpSChar)-1] = '\0';
											} else 
												tmpHolder->repeating = false;

											tmpHolder->data = tmpSChar;
											tmpHolder->length = strlen(tmpSChar);
											mySchema->setSeparator(tmpHolder);
										}
			;

getkey: KEY_TOKEN '=' '(' { tmpVector = new vector<char*> ;}
			 getkeys ')'   { mySchema->addKey(*tmpVector);
						delete tmpVector;
					  }
		 ;

getsorted: SORTED_TOKEN '=' '(' { tmpVector = new vector<char*> ; }
						getkeys ')' { if (mySchema->setSorted(tmpVector) == FAIL) {
											delete tmpVector;
											drerror("Sorted can't be entered twice !...");
									  }
								    }
		;

getkeys: getkeyparts
         | getkeys ',' getkeyparts
		 ;

getkeyparts: IDENT_TOKEN {  tmpKey = new char[strlen($1)+1];
							strcpy(tmpKey,$1);
							tmpVector->push_back(tmpKey);
						 }
		 ;

getattribute: getattrtype '{' getattributeoption '}' {
														mySchema->addAttribute(tmpAttr);
													 }
			  ;

getattrtype: ATTR_TOKEN getattributename
			 | SKIP_TOKEN {	tmpAttr = new Attribute();}
			 ;

getattributename: IDENT_TOKEN { char* tmpFN = new char[strlen($1)+1];
								strcpy(tmpFN,$1);
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
														drerror("Attribute Separator can't be defined twice !...");
													}
													char* tmpS = new char[strlen($3)+1];
													strcpy(tmpS,$3);
													Holder* tmpH= new Holder;

													if (tmpS[strlen(tmpS)-1] == '+') {
														tmpH->repeating = true;
														tmpS[strlen(tmpS)-1] = '\0';
													} else
														tmpH->repeating = false;

													tmpH->data = tmpS;
													tmpH->length = strlen(tmpS);
													tmpAttr->setSeparator(tmpH);
												}
			;
gettype: TYPE_TOKEN '=' getwhattype ;

getwhattype: INT_TOKEN {tmpAttr->setType(TYPE_INT);}
			 | STRING_TOKEN {tmpAttr->setType(TYPE_STRING);}
			 | DOUBLE_TOKEN {tmpAttr->setType(TYPE_DOUBLE);}
			 | DATE_TOKEN {tmpAttr->setType(TYPE_DATE);}
			 ;
getmaxlen: MAXLEN_TOKEN '=' INTVAL_TOKEN {	if (tmpAttr->getMaxLen() != -1)
                                                drerror("Field Max Length can't be defined twice !...");
											tmpAttr->setMaxLen($3); } ;

getquote: QUOTE_TOKEN '=' BRACKET_TOKEN { 	if (tmpAttr->getQuote() != -1) 
												drerror("Attribute Quote can't be defined twice !...");
											char* tmpQ = new char[strlen($3)+1];
											strcpy(tmpQ,$3);
											tmpAttr->setQuote(tmpQ[0]);
											delete [] tmpQ;
										}
		  ;
getlength: LENGTH_TOKEN '=' INTVAL_TOKEN {	if (tmpAttr->getFieldLen() != -1)
												drerror("Field Length can't be defined twice !...");
											tmpAttr->setFieldLen($3); } ;

gethidden: HIDDEN_TOKEN '=' BRACKET_TOKEN { if (tmpAttr->getHidden() != -1)
                                                drerror("Attribute Quote can't be defined twice !...");
											char* tmpHC = new char[strlen($3)+1];
											strcpy(tmpHC,$3);
											if (!((tmpHC[0] == 'Y') || (tmpHC[0] == 'y') || (tmpHC[0] == 'N') || (tmpHC[0] == 'n')))
												YYABORT;
											
											tmpAttr->setHidden(tmpHC[0]); 
											delete [] tmpHC;
										}
			;
getencoding: ENCODING_TOKEN '=' getwhatencod ;

getwhatencod: CSTYLE_TOKEN {tmpAttr->setEncoding(ENCODING_CSTYLE);}
			  | QUOTEDSTYLE_TOKEN {tmpAttr->setEncoding(ENCODING_QUOTED);}
			  | URLSTYLE_TOKEN {tmpAttr->setEncoding(ENCODING_URL);}
			  | NOESCAPESTYLE_TOKEN {tmpAttr->setEncoding(ENCODING_NOESCAPE);}
			  ;

getdateformat: DATE_FORMAT '='  STRING_TOKEN { char* tmpD = new char[strlen($3)+1];
				   							   strcpy(tmpD,$3);
											   tmpAttr->setDateFormat(tmpD);
			   								 }
			;
%%
