/* testDictGroup.c */
#include <stdio.h>
#include "DictGroup.h"

main(){
DictGroup *g = new DictGroup();

	int i, j;
	for (i=0; i < 10; i++){
		for (j=0; j < 10; j++)
			g->Append((void *)i,(void *)j);
	}

	for (i=0; i < 10 ; i++){
		printf("%d: ", i);
		for (g->InitIterator((void *)i); g->More(); ){
			int val = (int) (g->Next());
			printf("%d ", val);
		}
		printf("\n");
		g->DoneIterator();
	}
}
