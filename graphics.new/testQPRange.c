/*
  $Id$

  $Log$*/

#include <stdio.h>
#include "Parse.h"
#include "QPRange.h"

class testCallback: public QPRangeCallback {
public:
    virtual void QPRangeInserted(RecId low, RecId high){
		printf("[%d,%d] inserted\n", low, high);
	}
};

void Help(){
	printf("commands:\n");
	printf("help: prints list of commands\n");
	printf("next cur: prints next unprocessed range after cur\n");
	printf("search cur: prints rane <= cur\n");
	printf("insert low high: inserts [low,high]\n");
	printf("print:  prints list\n");
}

main(){
	QPRange *range = new QPRange;
	QPRangeCallback *callback = new testCallback;
	int numArgs; char **args;
	char buf[256];
	for(;;){
		if (gets(buf) == NULL)
			break;
		Parse(buf,numArgs, args);
		if (numArgs == 1){
			if (!strcmp(args[0],"help")){
				Help();
			}
			else if (!strcmp(args[0],"print")){
				int num;
				QPRangeRec *head;
				range->GetRangeList(num, head);
				printf("List contains %d elements: \n",num);
				QPRangeRec *cur;
				for (cur = head->next; cur != head; cur = cur->next){
					printf("[%d,%d] ", cur->low, cur->high);
				}
				printf("\n");
			}
			else printf("unknown command\n");
		}
		else if (numArgs  == 2){
			if (!strcmp(args[0],"next")){
				/* prints next unprocessed */
				RecId cur = (RecId )atoi(args[1]);
				RecId low, high;
				Boolean noHigh = range->NextUnprocessed(cur, low, high);
				if (noHigh){
					printf("[%d, ]\n", low);
				}
				else printf("[%d,%d]\n", low, high);
			}
			else if (!strcmp(args[0],"search")){
				RecId cur = (RecId)atoi(args[1]);
				QPRangeRec *rec = range->Search(cur);
				if (rec == NULL){
					printf("NULL\n");
				}
				else printf("[%d,%d]\n", rec->low, rec->high);
			}
			else printf("unknown command\n");
		}
		else if (numArgs == 3){
			if (!strcmp(args[0],"insert")){
				int low = atoi(args[1]);
				int high = atoi(args[2]);
				if (low > high){
					printf("error: low %d > high %d\n",low,high);
				}
				range->Insert(low,high,callback);
			}
			else printf("unknown command\n");
		}
		else printf("unknown command\n");
	}
}
