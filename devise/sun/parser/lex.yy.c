# include "stdio.h"
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX BUFSIZ
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
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
  Revision 1.9.12.1  1997/10/30 20:18:31  wenger
  Got DEVise to compile, link, and run on sun and linux; compiles but
  doesn't link on hp (can't find memory mapping functions).  This includes
  moving the unidata config.h files to separate copies in the unidata
  directory for each architecture, since the config.h's are architecture-
  dependent.

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
# define YYTYPE char
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
0,0,	0,0,	1,11,	0,0,	
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
47,52,	48,53,	49,54,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	50,55,	51,56,	52,57,	
53,58,	11,25,	57,59,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	11,25,	11,25,	11,25,	
11,25,	58,60,	59,61,	60,62,	
61,63,	63,64,	0,0,	0,0,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-14,	yysvec+1,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+0,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+6,
yycrank+0,	0,		yyvstop+8,
yycrank+26,	0,		yyvstop+11,
yycrank+0,	0,		yyvstop+13,
yycrank+92,	0,		yyvstop+16,
yycrank+0,	0,		yyvstop+19,
yycrank+102,	0,		yyvstop+22,
yycrank+0,	0,		yyvstop+24,
yycrank+0,	0,		yyvstop+27,
yycrank+4,	yysvec+11,	yyvstop+30,
yycrank+6,	yysvec+11,	yyvstop+32,
yycrank+1,	yysvec+11,	yyvstop+34,
yycrank+3,	yysvec+11,	yyvstop+36,
yycrank+2,	yysvec+11,	yyvstop+38,
yycrank+0,	yysvec+11,	yyvstop+40,
yycrank+0,	yysvec+11,	yyvstop+43,
yycrank+0,	0,		yyvstop+46,
yycrank+0,	0,		yyvstop+49,
yycrank+0,	yysvec+7,	yyvstop+52,
yycrank+0,	yysvec+9,	yyvstop+54,
yycrank+0,	yysvec+11,	yyvstop+56,
yycrank+1,	yysvec+11,	yyvstop+58,
yycrank+1,	yysvec+11,	yyvstop+60,
yycrank+7,	yysvec+11,	yyvstop+62,
yycrank+3,	yysvec+11,	yyvstop+64,
yycrank+2,	yysvec+11,	yyvstop+66,
yycrank+5,	yysvec+11,	yyvstop+68,
yycrank+9,	yysvec+11,	yyvstop+70,
yycrank+6,	yysvec+11,	yyvstop+72,
yycrank+16,	yysvec+11,	yyvstop+74,
yycrank+3,	yysvec+11,	yyvstop+76,
yycrank+18,	yysvec+11,	yyvstop+78,
yycrank+15,	yysvec+11,	yyvstop+80,
yycrank+18,	yysvec+11,	yyvstop+82,
yycrank+24,	yysvec+11,	yyvstop+84,
yycrank+31,	yysvec+11,	yyvstop+86,
yycrank+28,	yysvec+11,	yyvstop+88,
yycrank+59,	yysvec+11,	yyvstop+90,
yycrank+56,	yysvec+11,	yyvstop+92,
yycrank+0,	yysvec+11,	yyvstop+94,
yycrank+57,	yysvec+11,	yyvstop+97,
yycrank+53,	yysvec+11,	yyvstop+99,
yycrank+48,	yysvec+11,	yyvstop+101,
yycrank+100,	yysvec+11,	yyvstop+103,
yycrank+67,	yysvec+11,	yyvstop+106,
yycrank+94,	yysvec+11,	yyvstop+108,
yycrank+91,	yysvec+11,	yyvstop+110,
yycrank+98,	yysvec+11,	yyvstop+112,
yycrank+80,	yysvec+11,	yyvstop+114,
yycrank+0,	yysvec+11,	yyvstop+116,
yycrank+0,	yysvec+11,	yyvstop+119,
yycrank+0,	yysvec+11,	yyvstop+122,
yycrank+82,	yysvec+11,	yyvstop+125,
yycrank+109,	yysvec+11,	yyvstop+127,
yycrank+121,	yysvec+11,	yyvstop+129,
yycrank+113,	yysvec+11,	yyvstop+131,
yycrank+117,	yysvec+11,	yyvstop+133,
yycrank+0,	yysvec+11,	yyvstop+135,
yycrank+119,	yysvec+11,	yyvstop+138,
yycrank+0,	yysvec+11,	yyvstop+140,
0,	0,	0};
struct yywork *yytop = yycrank+229;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
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
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
#ifndef lint
static	char ncform_sccsid[] = "@(#)ncform 1.6 88/02/08 SMI"; /* from S5R2 1.2 */
#endif

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
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
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
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
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
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
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
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
yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
