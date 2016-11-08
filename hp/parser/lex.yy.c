# include "stdio.h"
#ifdef __cplusplus
   extern "C" {
     extern int yyreject();
     extern int yywrap();
     extern int yylook();
     extern void main();
     extern int yyback(int *, int);
     extern int yyinput();
     extern void yyoutput(int);
     extern void yyunput(int);
     extern int yylex();
   }
#endif	/* __cplusplus */
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 200
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng;
int yylenguc;
extern unsigned char yytextarr[];
extern unsigned char yytext[];
int yyposix_point=0;
int yynls16=0;
int yynls_wchar=0;
char *yylocale = "/\001:C;\002:C;\003:C;\004:C;:C;:C;:C;/";
int yymorfg;
extern unsigned char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	int yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2.16.1  1998/01/09 16:34:23  wenger
  Updated copyright date and version number; minor mods to compile for
  hp and sun; fixed problem with _batchMode flag getting improperly
  reset in the ControlPanel class (prevented using pixmaps instead of
  X windows).

  Revision 1.8  1995/12/14 19:07:59  jussi
  Changed return code of yywrap().

  Revision 1.7  1995/12/14 18:40:27  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.6  1995/11/25 19:35:37  jussi
  Added copyright notice and CVS header. Cleaned up the code a bit.
*/

#include "tokens.h"
# define YYNEWLINE 10
yylex(){
   int nstr; extern int yyprevious;
   while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
   if(yywrap()) return(0); break;
case 1:
		return(TOKEN_X);
break;
case 2:
		return(TOKEN_Y);
break;
case 3:
		return(TOKEN_COLOR);
break;
case 4:
		return(TOKEN_SHAPE);
break;
case 5:
	return(TOKEN_ORIENTATION);
break;
case 6:
	return(TOKEN_SHAPEATTR);
break;
case 7:
		return(TOKEN_LEFT_BRACKET);
break;
case 8:
		return(TOKEN_RIGHT_BRACKET);
break;
case 9:
	return(TOKEN_MAPPING);
break;
case 10:
	return(TOKEN_DYNAMIC);
break;
case 11:
	return(TOKEN_STATIC);
break;
case 12:
		return(TOKEN_SEMI_COLON);
break;
case 13:
		return(TOKEN_COMMA);
break;
case 14:
		return(TOKEN_LEFT_BRACE);
break;
case 15:
		return(TOKEN_RIGHT_BRACE);
break;
case 16:
	return(TOKEN_INT);
break;
case 17:
		return(TOKEN_INT);
break;
case 18:
return(TOKEN_IDENT);
break;
case 19:
;
break;
case 20:
;
break;
case 21:
;
break;
case 22:
syntax_error();
break;
case -1:
break;
default:
   fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */

static void __yy__unused() { main(); }

/* user subroutines */

void syntax_error()
{
  printf("syntax error, buffer contains %s\n", yytext);
  exit(2);
}

/*******************************************************************
Read text until a matching '}' is found
********************************************************************/

char *GetText()
{
  char buf[2048];
  int level = 1;
  int done = 0;
  int index = 0;
  char *text;

  while (!done) {
    char c = yyinput();
    if (c == 0)
      syntax_error();
    else if (c == '{') {
      level++;
      buf[index++] = c;
    }
    else if (c == '}') {
      if (--level == 0) {
	unput(c);
	done = 1;
      } else {
	buf[index++] = c;
      }
    } else {
      buf[index++] = c;
    }
  }
  buf[index++] = '\0';
  
  if (!(text = (char *)malloc(index))) {
    printf("no memory\n");
    exit(2);
  }

  memcpy(text, buf, index);

  return text;
}

/********************************************************************
Return the current text
*********************************************************************/

char *GetTokenText()
{
  char *text;

  if (!(text = (char *)malloc(yyleng + 1))) {
    printf("no memory\n");
    exit(2);
  }
  strcpy(text, yytext);
  return text;
}

int yywrap()
{
  return 1;
}
int yyvstop[] = {
0,

22,
0,

20,
22,
0,

21,
0,

19,
22,
0,

22,
0,

13,
22,
0,

17,
22,
0,

12,
22,
0,

22,
0,

7,
22,
0,

8,
22,
0,

22,
0,

22,
0,

22,
0,

22,
0,

22,
0,

1,
22,
0,

2,
22,
0,

14,
22,
0,

15,
22,
0,

16,
0,

17,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

3,
18,
0,

18,
0,

18,
0,

18,
0,

4,
18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

11,
18,
0,

10,
18,
0,

9,
18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

18,
0,

6,
18,
0,

18,
0,

5,
18,
0,
0};
# define YYTYPE unsigned char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	2,4,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,6,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
1,7,	1,8,	2,6,	0,0,	
0,0,	1,9,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	2,8,	0,0,	
1,10,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,11,	48,53,	
0,0,	0,0,	0,0,	0,0,	
0,0,	2,10,	7,23,	7,23,	
7,23,	7,23,	7,23,	7,23,	
7,23,	7,23,	7,23,	7,23,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
1,12,	0,0,	1,13,	0,0,	
0,0,	0,0,	16,28,	30,36,	
1,14,	1,15,	31,37,	33,39,	
35,41,	2,12,	18,30,	2,13,	
29,35,	26,32,	1,16,	27,33,	
1,17,	2,14,	2,15,	14,26,	
1,18,	17,29,	18,31,	28,34,	
32,38,	1,19,	1,20,	2,16,	
1,21,	2,17,	1,22,	15,27,	
34,40,	2,18,	36,42,	37,43,	
38,44,	39,45,	2,19,	2,20,	
40,46,	2,21,	41,47,	2,22,	
9,24,	9,24,	9,24,	9,24,	
9,24,	9,24,	9,24,	9,24,	
9,24,	9,24,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
42,48,	43,49,	45,50,	46,51,	
47,52,	49,54,	50,55,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	51,56,	52,57,	53,58,	
57,59,	11,25,	58,60,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	59,61,	60,62,	61,63,	
63,64,	0,0,	0,0,	0,0,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
-1,	0,		0,	
-14,	yysvec+1,	0,	
0,	0,		yyvstop+1,
0,	0,		yyvstop+3,
0,	0,		yyvstop+6,
0,	0,		yyvstop+8,
26,	0,		yyvstop+11,
0,	0,		yyvstop+13,
92,	0,		yyvstop+16,
0,	0,		yyvstop+19,
102,	0,		yyvstop+22,
0,	0,		yyvstop+24,
0,	0,		yyvstop+27,
4,	yysvec+11,	yyvstop+30,
6,	yysvec+11,	yyvstop+32,
1,	yysvec+11,	yyvstop+34,
3,	yysvec+11,	yyvstop+36,
2,	yysvec+11,	yyvstop+38,
0,	yysvec+11,	yyvstop+40,
0,	yysvec+11,	yyvstop+43,
0,	0,		yyvstop+46,
0,	0,		yyvstop+49,
0,	yysvec+7,	yyvstop+52,
0,	yysvec+9,	yyvstop+54,
0,	yysvec+11,	yyvstop+56,
1,	yysvec+11,	yyvstop+58,
1,	yysvec+11,	yyvstop+60,
7,	yysvec+11,	yyvstop+62,
3,	yysvec+11,	yyvstop+64,
2,	yysvec+11,	yyvstop+66,
5,	yysvec+11,	yyvstop+68,
9,	yysvec+11,	yyvstop+70,
6,	yysvec+11,	yyvstop+72,
16,	yysvec+11,	yyvstop+74,
3,	yysvec+11,	yyvstop+76,
18,	yysvec+11,	yyvstop+78,
15,	yysvec+11,	yyvstop+80,
18,	yysvec+11,	yyvstop+82,
24,	yysvec+11,	yyvstop+84,
31,	yysvec+11,	yyvstop+86,
28,	yysvec+11,	yyvstop+88,
59,	yysvec+11,	yyvstop+90,
56,	yysvec+11,	yyvstop+92,
0,	yysvec+11,	yyvstop+94,
57,	yysvec+11,	yyvstop+97,
53,	yysvec+11,	yyvstop+99,
48,	yysvec+11,	yyvstop+101,
2,	yysvec+11,	yyvstop+103,
66,	yysvec+11,	yyvstop+106,
67,	yysvec+11,	yyvstop+108,
90,	yysvec+11,	yyvstop+110,
97,	yysvec+11,	yyvstop+112,
79,	yysvec+11,	yyvstop+114,
0,	yysvec+11,	yyvstop+116,
0,	yysvec+11,	yyvstop+119,
0,	yysvec+11,	yyvstop+122,
80,	yysvec+11,	yyvstop+125,
82,	yysvec+11,	yyvstop+127,
120,	yysvec+11,	yyvstop+129,
112,	yysvec+11,	yyvstop+131,
116,	yysvec+11,	yyvstop+133,
0,	yysvec+11,	yyvstop+135,
118,	yysvec+11,	yyvstop+138,
0,	yysvec+11,	yyvstop+140,
0,	0,	0};
struct yywork *yytop = yycrank+228;
struct yysvf *yybgin = yysvec+1;
unsigned char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,'+' ,01  ,'+' ,01  ,01  ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,'A' ,
01  ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
0};
unsigned char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/* @(#) $Revision$      */
int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
 
#ifdef YYNLS16_WCHAR
unsigned char yytextuc[YYLMAX * sizeof(wchar_t)];
# ifdef YY_PCT_POINT /* for %pointer */
wchar_t yytextarr[YYLMAX];
wchar_t *yytext;
# else               /* %array */
wchar_t yytextarr[1];
wchar_t yytext[YYLMAX];
# endif
#else
unsigned char yytextuc;
# ifdef YY_PCT_POINT /* for %pointer */
unsigned char yytextarr[YYLMAX];
unsigned char *yytext;
# else               /* %array */
unsigned char yytextarr[1];
unsigned char yytext[YYLMAX];
# endif
#endif

struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
unsigned char yysbuf[YYLMAX];
unsigned char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
/*	char *yylastch;
 * ***** nls8 ***** */
	unsigned char *yylastch, sec;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
#ifdef YYNLS16_WCHAR
		yylastch = yytextuc;
#else
		yylastch = yytext;
#endif
	else {
		yymorfg=0;
#ifdef YYNLS16_WCHAR
		yylastch = yytextuc+yylenguc;
#else
		yylastch = yytext+yyleng;
#endif
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = &yycrank[yystate->yystoff];
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == 0)break;
				}
			*yylastch++ = yych = input();
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt = &yycrank[yystate->yystoff]) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
#ifdef YYNLS16_WCHAR
				yylenguc = yylastch-yytextuc+1;
				yytextuc[yylenguc] = 0;
#else
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
#endif
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
#ifdef YYNLS16_WCHAR
					sprint(yytextuc);
#else
					sprint(yytext);
#endif
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
#ifdef YYNLS16_WCHAR
		if (yytextuc[0] == 0  /* && feof(yyin) */)
#else
		if (yytext[0] == 0  /* && feof(yyin) */)
#endif
			{
			yysptr=yysbuf;
			return(0);
			}
#ifdef YYNLS16_WCHAR
		yyprevious = yytextuc[0] = input();
#else
		yyprevious = yytext[0] = input();
#endif
		if (yyprevious>0) {
			output(yyprevious);
#ifdef YYNLS16
                        if (yynls16)
#ifdef YYNLS16_WCHAR
                        	if (FIRSTof2(yytextuc[0]))
#else
                        	if (FIRSTof2(yytext[0]))
#endif
     					if (SECof2(sec = input()))
#ifdef YYNLS16_WCHAR
 						output(yyprevious=yytextuc[0]=sec);
#else
 						output(yyprevious=yytext[0]=sec);
#endif
					else 
						unput(sec);
#endif
                }
#ifdef YYNLS16_WCHAR
		yylastch=yytextuc;
#else
		yylastch=yytext;
#endif
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}

# ifdef __cplusplus
yyback(int *p, int m)
# else
yyback(p, m)
	int *p;
# endif
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	
	}

#ifdef __cplusplus
void yyoutput(int c)
#else
yyoutput(c)
  int c;
# endif
{
	output(c);
}

#ifdef __cplusplus
void yyunput(int c)
#else
yyunput(c)
   int c;
#endif
{
	unput(c);
}
