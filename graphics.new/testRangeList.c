/*
  $Id$

  $Log$*/

#include <stdio.h>
#include <stdlib.h>
#include "RangeList.h"
#include "Parse.h"

PrintDisposed(int num, RangeInfo **disposed)
{
  printf("%d disposed\n", num);
  for (int i=0; i < num; i++){
    printf("(%d,%d)\n",disposed[i]->low, disposed[i]->high);
  }
}

main()
{
  /* print: enter command */
  int dummy  = 0;
  RangeList *rangeList = new RangeList();
  RecId low, hi, id;
  Boolean stat;
  RangeList::RangeListMergeInfo flag;
  int numDisposed;
  RangeInfo **disposed;
  for (;;) {
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
      if (numArgs != 4){
	printf("needs 4 args\n");
      }
      else {
	low = atoi(args[1]);
	hi = atoi(args[2]);
	switch(args[3][0]){
	case 'i':
	  flag = RangeList::MergeIgnore;
	  break;
	case 'l':
	  flag = RangeList::MergeLeft;
	  break;
	case 'r':
	  flag = RangeList::MergeRight;
	  break;
	case 'b':
	  flag = RangeList::MergeBoth;
	}
	RangeInfo *range = new RangeInfo;
	range->low = low;
					range->high = hi;
	range->buf = range->data = &buf[low];
	range->bufSize = range->dataSize = hi-low+1;
	
	rangeList->Insert(range,flag,numDisposed, disposed);
	PrintDisposed(numDisposed,disposed);
      }
      break;
    case 'd':
      low = atoi(args[1]);
      RangeInfo *rangeInfo = rangeList->Search(low);
      if (rangeInfo == NULL)
	printf("can't find %d\n", low);
      else rangeList->Delete(rangeInfo);
      break;
    case 'h':
      printf("commands are:\n");
				printf("i low hi flag : insert new range\n");
      printf("    flag == 'i': merge ignore\n");
      printf("    flag == 'l': merge left\n");
      printf("    flag == 'r': merge right\n");
      printf("    flag == 'b': merge both\n");
      printf("h == help\n");
      printf("d low: delete range\n");
      break;
    }
    rangeList->Print();
  }
}
