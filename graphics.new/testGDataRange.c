/*
  $Id$

  $Log$*/

#include <stdio.h>
#include <stdlib.h>
#include "GDataRangeMap.h"
#include "Parse.h"

void PrintRec(GDataRangeMapRec *rec){
	printf("tId (%d,%d), gId (%d,%d)\n", rec->tLow, rec->tHigh,
		rec->gLow, rec->gHigh);
}
main(){

	/* print: enter command */
	int dummy  = 0;
	GDataRangeMap *range = new GDataRangeMap(sizeof(int),NULL);
	RecId tLow, tHigh, gLow, gHigh;
GDataRangeMapRec *rec;
	Boolean stat;
	int *first, *last;
	for (; ;){
		char buf[256];
		printf("cmd: ");
		gets(buf);
		int numArgs;
		char **args;
		int num;
		int pos;
		void *retVal;
		Parse(buf, numArgs, args);
		switch(args[0][0]){
			case 'i':
				if (numArgs != 5){
					printf("needs 5 args\n");
				}
				else {
					tLow = atoi(args[1]);
					tHigh = atoi(args[2]);
					gLow = atoi(args[3]);
					gHigh = atoi(args[4]);
					first = new int;
					*first = tLow;
					last = new int;
					*last = tHigh;
					range->InsertRange(tLow, tHigh, gLow, gHigh,first,last);
				}
				break;
			case 'l':
				if (numArgs != 2){
					printf("needs 2 args\n");
				}
				else {
					tLow = atoi(args[1]);
					rec = range->FindMaxLower(tLow);
					if (rec == NULL)
						printf("Not found\n");
					else PrintRec(rec);
				}
				break;
			case 'u':
				if (numArgs != 2){
					printf("needs 2 args\n");
				}
				else {
					tLow = atoi(args[1]);
					rec = range->FindMinUpper(tLow);
					if (rec == NULL)
						printf("Not found\n");
					else PrintRec(rec);
				}
				break;
			case 'g':
				if (numArgs != 2){
					printf("needs 2 args\n");
				}
				else {
					gLow = atoi(args[1]);
					rec = range->FindGRange(gLow);
					if (rec == NULL)
						printf("Not found\n");
					else PrintRec(rec);
				}
				break;
			case 'n':
				if (numArgs != 2){
					printf("needs 2 args\n");
				}
				else {
					tLow = atoi(args[1]);
					rec = range->FindNoOverlap(tLow);
					if (rec == NULL)
						printf("Not found\n");
					else PrintRec(rec);
				}
				break;
			case 'p':
				if (numArgs != 2){
					printf("needs 2 args\n");
				}
				else {
					tLow = atoi(args[1]);
					stat = range->NextUnprocessed(tLow, tLow, tHigh);
					if (stat)
						printf("[%d,]\n",tLow);
					else printf("[%d,%d]\n",tLow,tHigh);
				}
				break;
			case 'h':
				printf("commands are:\n");
				printf("i tlow tHi gLow gHi: insert new range\n");
				printf("l tId: find max lower bound\n");
				printf("u tId: find min upper bound\n");
				printf("g gId: Find exact\n");
				printf("n tid: Find no overlap with existing\n");
				printf("p tid: Find next to be processed\n");
				printf("h: help\n");
				break;
		}
		range->Print();
	}
}
