/*
  $Id$

  $Log$*/

#ifndef Parse_h
#define Parse_h

/* parse funcion with ' ', and \t as separators */
extern void Parse(char *, int &, char **&);

/* parse function that takes an array of characters as separators.
isSepaator == false to parse whitespace.
	== true to parse separators*/
extern void Parse(char *line,int &num, char **&args, char *separators,
	int numSeparators, int isSeparator = 0);

/* Parse either an integer, or date format, and make a double.
Return true if parsed OK.
date format is like: Thu Jan 26 17:27:42 CST 1995
*/
int ParseFloatDate(char *line, double &val);
#endif
