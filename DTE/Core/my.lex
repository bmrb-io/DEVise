%{
#include <String.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include "myopt.h"
#include "my.yacc.tab.h"

extern char* queryString;

static int global_counter_ = 0;

#define YY_INPUT(buf,result,max_size) \
    { \
    if(global_counter_){ \
	    result = YY_NULL; \
    } \
    int queryLength = strlen(queryString);\
    assert(queryLength < max_size); \
    memcpy(buf, queryString, queryLength); \
    result = queryLength; \
    global_counter_++; \
    }

char* stripquotes(char* text){
	char* retVal = strdup(text + 1);
	int len = strlen(retVal);
	int j = 0;
	for(int i = 0; i < len - 1; i++){
		if(retVal[i] == '"'){
			assert(j > 0);
			retVal[--j] = retVal[i];
		}
		else{
			retVal[j] = retVal[i];
		}
		j++;
	}
	retVal[j] = '\0';
	return retVal;
}
		
%}

Digit        [0-9]
IntLit       {Digit}+
String       [A-Za-z][A-Za-z0-9_]*
LessGreat    ">="|">"|"<="|"<"
%%
[ \t\n]+     {}
[Ss][Ee][Ll][Ee][Cc][Tt]       {return SELECT;}
[Ff][Rr][Oo][Mm]         {return FROM;}
[Ww][Hh][Ee][Rr][Ee]        {return WHERE;}
[Aa][Nn][Dd]          {return AND;}
[Oo][Rr]           {return OR;}
{String}     {yylval.string = new String(yytext); return STRING;}
{IntLit}     {yylval.integer = atoi(yytext); return INT;}
{LessGreat}  {yylval.string = new String(yytext); return LESSGREATER;}
\"([^\"]|\\\")*\" {
             yylval.string = new String(stripquotes(yytext)); 
		   return STRING_CONST;
		   }
.            {return yytext[0];}
%%
int yywrap(){
     return 1;
}
