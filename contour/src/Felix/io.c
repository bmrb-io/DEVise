/*Handles all input-output related functions*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nrutil.h"

#define true 1
#define false 0
#define null 0

/*Global Variables*/
int count,pcount,chunk,finish;
int prn = true;
int gabort = false;
char footer[128];

/*Static Functions*/

/*Returns the length in bytes of an open file*/
long filelength(FILE* fp)
{
	long l1,l2,stol;

	stol = ftell(fp);
	fseek(fp,0,0);
	l1 = ftell(fp);
	fseek(fp,0,2);
	l2 = ftell(fp);
	fseek(fp,stol,0);
	return(l2 - l1 + 1);
}

/*Global Functions*/


void ErrorMessage(char* str,int arg)
{
	printf(str,arg);
	printf("Execution has been aborted\n");
	exit(1);
}

void UserMessage(char* message,int arg)
{
	if (gabort) return;
	if (!prn) return;
	printf(message,arg);
	fflush(stdout);
}

void OutMessage(char* message)
{
	if (!prn) return;
	printf(message);
	fflush(stdout);
}

void GetString(char* prompt,char* str)
{
	printf("%s\n",prompt);
	scanf("%s",str);
}


double GetDouble(char* prompt)
{
	char numb[64];
	double d;

	printf("%s\n",prompt);
	scanf("%s",numb);
	d = atof(numb);
	return(d);
}

float GetFloat(char* prompt)
{
	double d;

	d = GetDouble(prompt);
	return((float)d);
}

int GetInteger(char* prompt)
{
	char numb[64];
	int j;

	printf("%s\n",prompt);
	scanf("%s",numb);
	j = atoi(numb);
	return(j);
}

char GetChar(char* prompt,int arg)
{
	char ch;

	printf("%s %d\n",prompt,arg);
	ch = getchar();
	return(ch);
}

void ShowIndex(int *pt,int d)
{
	int i;

	if (!prn) return;	
	for(i=1; i<=d; i++)
	{
		if( i == 1)
			printf("\n%d",pt[1]);
		else
			printf("\t%d",pt[i]);
	}
	fflush(stdout);
}

void ProcessReset(int total)
{
	count = 0;
	pcount = 0;
	chunk = (int)( 0.001*(float)total );
	finish = total;
}

void SetFooter(char* str)
{
	strcpy(footer,str);
}


void ProcessPrompt(int incr,void (*proc)(void*),void* data)
{
	float frac;

	if (gabort) return;
	if (!prn) return;
	count += incr;
	pcount += incr;
	if ( (pcount >= chunk) || (count == finish) )
	{
		frac = 100.0*(float)count/(float)finish;
		printf("\r%s%5.1f%c",footer,frac,'%');
		if( proc != 0)
			(*proc)(data);
		pcount = 0;
	}
	fflush(stdout);
}

void TogglePrint()

{
	prn = !prn;
}

void GagOutput()

{
	gabort = true;
}

FILE *OpenFile(char* filenm,char* format)
{
	FILE *fp = null;

	fp = fopen(filenm,format);
	if (!fp) 
	{
		printf ("Cannot open file %s\n",filenm);
		abort();
	}
	return(fp);
}

void DestroyFile(char* fname)
{
	char command[120];

	strcpy(command,"rm ");
	strcat(command,fname);
	system(command);
}
