/*Handles all input-output related functions*/

#ifndef io_H
#define io_H

#include <stdio.h>


/*Global Functions*/


void ErrorMessage(char *str,int arg);


void UserMessage(char *message,int arg);


void GetString(char *prompt,char *str);

float GetFloat(char *prompt);

double GetDouble(char *prompt);


int GetInteger(char *prompt);

char GetChar(char *prompt,int arg);

void ShowIndex(int *pt,int d);

/*Returns the length in bytes of an open file*/
long filelength(FILE *fp);

void ProcessReset(int total);

void SetFooter(char *str);

void ProcessPrompt(int incr,void (*proc)(),void *data);

void TogglePrint();
 
void GagOutput();

FILE *OpenFile(char *filenm,char *format);

void DestroyFile(char *fname);

#endif
