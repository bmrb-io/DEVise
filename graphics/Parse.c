/*
  $Id$

  $Log$*/

#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <libc.h>
#include "Config.h"
#include "Exit.h"

const int MAXARGS = DEVISE_MAX_ATTRS;

static char *args[MAXARGS];

inline int IsBlank(char c, char *blanks, int numBlanks){
	int i;
	for (i=0; i < numBlanks; i++){
		if (c == blanks[i])
			return 1;
	}
	return 0;
}

inline char *SkipBlanks(char *str, char *blanks, int numBlanks){
	while (IsBlank(*str, blanks, numBlanks))  str++;
	return str;
}


/* Parse an a blank separated line into fields and return the 
# of fields parsed. */
void Parse(char *str, int &numArgs, char **&returnArgs, char *blanks,
	int numBlanks, int isSeparator){

	returnArgs = args;
	numArgs = 0;
	if (!isSeparator){
		/* process line with whitespace */
		str = SkipBlanks(str, blanks, numBlanks);
		while (*str != '\0'){
			/* If we got here, *str is not a blank. skip non-blank 
			to find next blank */
			char *start = str;
			if (*str == '\'' ){
				/* reading a literal */
				str++;
				start = str;
				while (*str != '\0' && *str != '\'')
					str++;
				if (*str == '\0')
					break;
			}
			else while (*str != '\0' && !IsBlank(*str, blanks, numBlanks)) 
				str++;
			/* make an arg */
			if (*str != '\0'){
				*str = '\0';
				str++;
			}

			if (numArgs >= MAXARGS-1){
				fprintf(stderr,"parse: no more args\n");
				Exit::DoExit(1);
			}
			args[numArgs++] = start;

			str = SkipBlanks(str, blanks, numBlanks);
		}
	} else {
		/* process line with separators */
		char *start;
		do {
			start = str;
			while (*str != '\0' && !IsBlank(*str, blanks, numBlanks)){
				str++;
			}
			/* from start to str is the field */
			char *end = str;
			if (*str != '\0')
				str++;
			*end = '\0';

			if (numArgs >= MAXARGS-1){
				fprintf(stderr,"parse: no more args\n");
				Exit::DoExit(1);
			}
			args[numArgs++] = start;
		} while (*str != '\0');
	}
}

void Parse(char *str, int &numArgs, char **&returnArgs){
	char blanks[2] = {' ', '\t'};
	Parse(str,numArgs, returnArgs, blanks, 2, 0);
}

static char *monthNames[12] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL",
	"AUG", "SEP", "OCT", "NOV", "DEC" };

/* get month */
static int GetMonth(char *month){
static int monthHint = -1;
	if (monthHint >= 0 && strcmp(monthNames[monthHint],month) == 0){
		return monthHint;
	} else {
		int i;
		for (i=0; i < 12; i++){
			if (strcmp(monthNames[i],month) == 0) {
				monthHint = i;
				return i;
			}
		}
		/* not found */
		fprintf(stderr,"unknown month %s\n", month);
		Exit::DoExit(2);
	}
}


int ParseFloatDate(char *input, double &val){
	/*
	printf("ParseFloatDate %s\n", input);
	*/
	char line[256];
	strcpy(line, input);
	char *ptr = line;
	for (; *ptr != '\0'; ptr++){
		if (isalpha(*ptr)) *ptr  = toupper(*ptr);
	}
	/*
	printf("after converting to upper: %s\n", line);
	*/

	int numArgs;
	char **args;
	Parse(line,numArgs, args);
	/*
	printf("gotd %d args\n", numArgs);
	*/
	struct tm timestr;
	if (numArgs == 1){
		val = atof(args[0]);
		/*
		printf("val = %f\n", val);
		*/
		return 1;
	}
	else if (numArgs == 4) {
		if (strlen(args[0]) != 3)
			goto error;
		if (strlen(args[3]) != 8)
			goto error;

		timestr.tm_mon = GetMonth(args[0]);
		timestr.tm_mday = atoi(args[1]);
		timestr.tm_year = atoi(args[2])- 1900;
		args[3][2] = '\0';
		args[3][5] = '\0';
		timestr.tm_hour = atoi(args[3]);
		timestr.tm_min = atoi(&args[3][3]);
		timestr.tm_sec = atoi(&args[3][6]);

		val = (double)mktime(&timestr);
		/*
		printf("val = %f\n", val);
		*/
		return 1;
	}
error:
	val = 0.0;
	/*
	printf("error val = 0\n" );
	*/
	return 0;
}
