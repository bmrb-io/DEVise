/*
  $Id$

  $Log$*/

#include <stdio.h>
#include <stdlib.h>
#include "RecRange.h"
#include "Parse.h"

class TestCallback : public RecRangeCallback {
public:
  void RecsMerged(RecId id, void *rec, void *nextRec){
    printf("merged %d,%d\n", id, id+1);
  }
};

main()
{
  /* print: enter command */
  int dummy  = 0;
  RecRange *range = new RecRange(sizeof(int));
  RecId low, hi, id;
  Boolean stat;
  RecRange::RecRangeMergeInfo flag;
  TestCallback *callback = new TestCallback;
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
	case 'l':
	  flag = RecRange::MergeLeft;
	  break;
	case 'r':
	  flag = RecRange::MergeRight;
	  break;
	case 'b':
	  flag = RecRange::MergeBoth;
	}
	range->Insert(low,hi,&dummy,&dummy,flag,callback);
      }
      break;
    case 'n':
      if (numArgs != 2){
	printf("needs 2 args\n");
      }
      else {
	id = atoi(args[1]);
	stat = range->NextUnprocessed(id,low,hi);
	if (stat)
	  printf("next range: [%d,]\n",low);
	else printf("next range: [%d,%d]\n",low, hi);
      }
      break;
    case 'h':
      printf("commands are:\n");
      printf("i low hi flag : insert new range\n");
      printf("    flag == 'l': merge left\n");
      printf("    flag == 'r': merge right\n");
      printf("    flag == 'b': merge both\n");
      printf("n id == insert num at position\n");
      printf("h == help\n");
      break;
    }
    range->Print();
  }
}
