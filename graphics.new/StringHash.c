/* StringHash.c */
#include <stdio.h>
#include "StringHash.h"


StringHash::StringHash(){
	debug = 0;
	stringSpaceInit = 10000;
	stringSpaceMAX  = 1000000;


	stringSpace = 0 ;

	stringSpaceSize = 0;
	nextFreeStringSpace = 1;
}

void StringHash::DumpStringSpace(){
    int charCount = 0;
    for (int i = 0 ; i < nextFreeStringSpace;i++){
	if ( stringSpace[i]  != NULLCHAR) {
	    printf("%c",stringSpace[i]);
	    }
	else
	    if  (charCount > 60) {
	        printf("\n");
	        charCount = 0;
	        }
	    else { printf(" | "); }
	charCount++;
    };
    printf("\n");
};

void StringHash::WriteName(int name){
    int i = 0;
    while ((stringSpace[name] != NULLCHAR) && i < 30 ) {
        printf("%c",stringSpace[name]);
        name++;
        i++;
        };
    };
  
void StringHash::InsertStringSpace (char ch){
    char*   newSpace = 0 ;
    char*   oldSpace = 0 ;
    char*   tempSpace = 0 ;

	if (nextFreeStringSpace >= stringSpaceSize) {
		newSpace = new char[stringSpaceSize+stringSpaceInit];
		tempSpace = newSpace;
		for(int index = 0 ; index < stringSpaceSize ; index++){
			tempSpace[index] = stringSpace[index];
		}
		if (stringSpace) {
			oldSpace = stringSpace;
			delete(oldSpace);
			stringSpace = tempSpace;
			stringSpaceSize = stringSpaceSize + stringSpaceInit;
		} else {
			stringSpace = tempSpace;
			stringSpaceSize = stringSpaceSize + stringSpaceInit;
			/*
			InsertHashTable("NoName");
			*/
		}
		if (debug)
			printf("NEW SPACE %d %d %d %d\n",
				stringSpace,tempSpace,stringSpaceSize,nextFreeStringSpace); 
    }
	stringSpace[nextFreeStringSpace] = ch; 
	nextFreeStringSpace++;
};

int StringHash::Hash(char* name ){
  int  remainder = 0;
  while (*name != NULLCHAR) {
      remainder = (remainder * MaxCharCode + *name ) % (HashTableSize+1);
      name++;
      }
  if (debug)
      printf("Hash value for %s is %d \n",name,remainder);
  return  remainder;
};

int StringHash::EqualStrings(char* string1, char* string2 ) {
    if (debug)
        printf("Equal??? value for %s  %s \n",string1,string2);
    while ((*string1 == *string2) && *string1 != NULLCHAR) { 
        string1++;
        string2++;
	};
    if (*string1 == *string2)
        return  1;
    else
        return  0;
};

int StringHash::InsertHashTable (char* name) { 
	int  hashValue, bucket,location ; 
	int  found = 0; 
	int  empty = 0;
	if (debug)
		printf("Insert string %s \n",name);
	hashValue = Hash(name);
	bucket    = hashValue;
	do {
		if (hashTable[bucket] == 0 ) {
			empty = 1;
		}
		else { 
			if ( EqualStrings(name, stringSpace+hashTable[bucket]) ) {
				found = 1;
			}
			else {
				if ( hashValue == 0 ) {
					bucket = (bucket+1) % (HashTableSize+1);
				}
				else {
					bucket = (bucket + hashValue) % (HashTableSize + 1);
				};
				if (debug)
					printf("New Hash value for %s is %d \n",name,bucket);
	
				if ( bucket == hashValue ) {
					printf("ERROR!!!! Can not find a bucket for string\n");
				};
			};
		}
	} while ( !found && !empty);

    if ( found ) { 
        location = hashTable[bucket];
	}
    else {
        location = nextFreeStringSpace;
        hashTable[bucket] = location; 
        for (int index = 0 ; name[index] != NULLCHAR ; index++) {
            InsertStringSpace(name[index]);
        };
        InsertStringSpace(NULLCHAR);
    };
    if (debug)
        printf("Location for %s is %d \n",name,location);
    return  location;
};

void StringHash::TestString() {

    char* name = new char[35]; 
    int  i;
    int location = InsertHashTable("Miron"); 
        WriteName(location) ; printf(" - %d \n",location);
    location = InsertHashTable("iron");
        WriteName(location) ; printf(" - %d \n",location);
    for ( i = 0; i < 1000; i++){
        sprintf(name,"bufferbuffer buffer buffer %d",i);
        location = InsertHashTable(name); WriteName(location) ; 
	printf(" - %d \n",location);
	}
};

char* StringHash::GiveString(int location) {
    return &stringSpace[location];
};

char *StringHash::GiveClosestString(int location){
	if (location <= 0)
		return NULL;
	if (location >= nextFreeStringSpace)
		return NULL;

	/* search backwards to find the previous string */
	if (location > 0 && stringSpace[location] == NULLCHAR)
		location--;
	while(location >0 && stringSpace[location] != NULLCHAR)
		location--;
	return &stringSpace[location+1];
}
