#ifndef StringHash_H
#define StringHash_H

const char NULLCHAR=  '\0';
const int HashTableSize=  2110;
const int MaxCharCode=    127;


class StringHash{
public:
	StringHash();
	int InsertHashTable (char* name); 
	char* GiveString (int location); 

	/* Return next closest string, or NULL if none */
	char *GiveClosestString(int location);

	void TestString();
	void DumpStringSpace();
private:
	void WriteName(int name);
	void InsertStringSpace(char ch);
	int Hash(char *name);
	int EqualStrings(char *s1, char *s2);

int debug;
int stringSpaceInit;
int stringSpaceMAX;

int hashTable[HashTableSize];

char* stringSpace;

int stringSpaceSize;
int nextFreeStringSpace;
};

#endif

