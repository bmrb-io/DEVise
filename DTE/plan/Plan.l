 /* (C) 1998- by Kevin S. Beyer */

%option c++
%option prefix="Plan"
%option yyclass="PlanParser"
%option noyywrap

%{

#include "PlanParser.h"
#include <string.h>
#include <ctype.h>
#include "Plan.tab.h"

#undef YY_DECL
#define YY_DECL int PlanParser::yylex(yystype* yylval)

%}

id		[a-zA-Z_][a-zA-Z_0-9]*
oct		[0-7]+
hex		[0-9a-fA-F]+
real            [+-]?(([0-9]+)|([0-9]*\.[0-9]+))([eE][+-]?[0-9]+)?
int		[+-]?[0-9]+
yyyy            [0-9][0-9][0-9][0-9]
mm              [0-9][0-9]
dd              [0-9][0-9]
op              op(([<>=+\-*/%])|([<>!]=))

%x COMMENT
%x STR

%%

\n		_line_nr++;
[ \t\f\r]+	;
"//".*[^\n]     ;

 /* multi-line comments */
"/*"		BEGIN COMMENT;
<COMMENT>"*/"	BEGIN INITIAL;
<COMMENT>.	;
<COMMENT>\n	_line_nr++;

 /* keywords */
run             return RUN;
quit		return QUIT;
tempfile        return TEMPFILE;
file            return FILE_t;
pipe            return PIPE;
write           return WRITE;
WritePipe       return WRITEPIPE;
LoadPipe        return LOADPIPE;
print           return PRINT;
consume         return CONSUME;
send            return SEND;
plot            return PLOT;
create          return CREATE;
rtree           return RTREE;
RadixCube       return RADIXCUBE;
ChiSquared      return CHISQUARED;
delete          return DELETE;
SelProj         return SELPROJ;
TableLookup     return TABLELOOKUP;
Sort            return SORT;
UniqueSort      return UNIQUESORT;
Union           return UNION;
RidAdder        return RIDADDER;
SimpleReader    return SIMPLEREADER;
DataReader      return DATAREADER;
HeapReader      return HEAPREADER;
RTreeLookup     return RTREELOOKUP;
NLJoin          return NLJOIN;
BNLJoin         return BNLJOIN;
MergeJoin       return MERGEJOIN;
MergeJoinDedupRight return MERGEJOINDEDUPRIGHT;
SimpleHashJoin  return SIMPLEHASHJOIN;
Agg             return AGG;
HashGroupAgg    return HASHGROUPAGG;
SortedGroupAgg  return SORTEDGROUPAGG;
Sequence        return SEQUENCE;
ShiftRelative   return SHIFT_RELATIVE;
ShiftAbsolute   return SHIFT_ABSOLUTE;
ShiftAllRelative return SHIFTALL_RELATIVE;
ShiftAllAbsolute return SHIFTALL_ABSOLUTE;
SeqAgg          return SEQAGG;
asc             return ASC;
desc            return DESC;
dedup           return DEDUP;
var             return VAR;
InnerJoin       return INNER_JOIN_TOK;
LeftOuterJoin   return LEFT_OUTER_JOIN_TOK;
RightOuterJoin  return RIGHT_OUTER_JOIN_TOK;
FullOuterJoin   return FULL_OUTER_JOIN_TOK;


 /* string literals */

\"		{ BEGIN STR; }
<STR>\\\n	;
<STR>\\n	{ _str << '\n'; }
<STR>\\t	{ _str << '\t'; }
<STR>\\v	{ _str << '\v'; }
<STR>\\b	{ _str << '\b'; }
<STR>\\r	{ _str << '\r'; }
<STR>\\f	{ _str << '\f'; }
<STR>\\a	{ _str << '\a'; }
<STR>\\\\	{ _str << '\\'; }
<STR>\\\?	{ _str << '\?'; }
<STR>\\\'	{ _str << '\''; }
<STR>\\\"	{ _str << '\"'; } 
<STR>\\{oct}	{ _str << char(strtol(yytext+1,NULL, 8)); }
<STR>\\x{hex}	{ _str << char(strtol(yytext+2,NULL,16)); }
<STR>\"		{ _str << '\0';
		  yylval->string_val = strdup(_str.str());
                  _str.seekp(0);
                  _str.freeze(0);
		  BEGIN INITIAL; 
		  return STRING_VAL;
		}
<STR>.|\n	{ _str << yytext[0]; }


 /* numeric literals */

{int}		{ yylval->int_val = atoi(yytext); return INT_VAL; }

{real}		{ yylval->double_val = atof(yytext); return DOUBLE_VAL; }


 /* boolean literals */

true            { yylval->bool_val = true; return BOOL_VAL; }

false           { yylval->bool_val = false; return BOOL_VAL; }


 /* indentifiers */

{id}		{ yylval->string_val = strdup(yytext); return ID; }
{op}		{ yylval->string_val = strdup(yytext+2); return ID; }

 /* symbols */
.               { return yytext[0]; }


%%

