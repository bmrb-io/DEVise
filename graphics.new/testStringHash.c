/* testStringHash.c */
#include <stdio.h>
#include "StringHash.h"

char *strings[10] = {
	"s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10"
};

main(){
	StringHash *hash = new StringHash();

	printf("inserting\n");
	int i;
	char buf[80];
	for (i=0; i < 100; i++){
		sprintf(buf,"proc%d",i);
		printf("(%s: %d) ", buf, hash->InsertHashTable(buf));
	}
	printf("\n");
	
	printf("getting them back\n");
	for (i=0; i < 100; i++){
		sprintf(buf,"proc%d",i);
		printf("(%s: %d) ", buf, hash->InsertHashTable(buf));
	}
	printf("\n");

	for (;;){
		printf("location: ");
		gets(buf);
		printf("%s\n",hash->GiveClosestString(atoi(buf)));
	}
}
