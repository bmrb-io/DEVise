#include "types.h"
#include "DateTime.h"
#include "Interface.h"
#include "MemoryMgr.h"

size_t MemoryLoader::PAGE_SZ = 1 * 1024;

void MemoryLoader::deallocate(size_t space){
	assert(space <= PAGE_SZ);
	assert(!pagePtrs.empty());
	if(unfreedSpace > space){
		unfreedSpace -= space;
	}
	else{
		void* pageToMv = pagePtrs.front();
		pagePtrs.pop_front();
		freePagePtrs.push_back(pageToMv);
		if(unfreedSpace == space){
			unfreedSpace = PAGE_SZ;
		}
		else{
			unfreedSpace = PAGE_SZ - space;
		}
	}
}

void* MemoryLoader::allocate(size_t spaceNeed){
	void* retVal;	
	if(remainSpace < spaceNeed){
		spacePtr = (Type*) getNewPage();
		pagePtrs.push_back(spacePtr);
		remainSpace = PAGE_SZ;
	}
	assert(remainSpace >= spaceNeed);
	retVal = spacePtr;
	spacePtr = (char*)(spacePtr) + spaceNeed;
	remainSpace -= spaceNeed;
	return retVal;
}

TupleLoader::iterator::iterator(const deque<void*>::iterator& pageIt, 
	int numPages, int beginOffset, int endOffset, int numFlds) : 
	pageIt(pageIt), numPages(numPages), 
	beginOffset(beginOffset), endOffset(endOffset), numFlds(numFlds)
{
	if(numPages == 0){
		reset();
	}
	else if(numPages == 1 && beginOffset >= endOffset){
		reset();
	}
	else{
		offsetIsOk();
	}
}

bool TupleLoader::iterator::offsetIsOk(){

/*
	cerr << "numPages = " << numPages << endl;
	cerr << "beginOffset = " << beginOffset << endl;
	cerr << "endOffset = " << endOffset << endl;
*/

	assert(numFlds > 0);
	if(beginOffset + numFlds * sizeof(void*) > 
		MemoryLoader::PAGE_SZ){

		beginOffset = 0;
		numPages--;
		if(numPages > 0){
			++pageIt;
		}
		else{
			reset();
		}
		return false;
	}
	return true;
}

MemoryLoader** newTypeLoaders(const TypeID* types, int numFlds){
	MemoryLoader** retVal = new MemoryLoader*[numFlds];
	for(int i = 0; i < numFlds; i++){
		if(types[i] == INT_TP){
			retVal[i] = new IntLoader();
		}
		else if(types[i] == DOUBLE_TP){
			retVal[i] = new MemoryLoaderTemplate<IDouble>;
		}
		else if(types[i] == INTERFACE_TP){
			retVal[i] = new InterfaceLoader;
		}
		else if(strncmp(types[i].c_str(), "string", 6) == 0){
			retVal[i] = new StringLoader();
		}
		else if(types[i] == DATE_TP){
			retVal[i] = new MemoryLoaderTemplate<EncodedDTF>;
		}
		else{
			cerr << "Loader not implemented for type: " << types[i] << endl;
			exit(1);
		}
	}
	return retVal;
}

Type* InterfaceLoader::load(const Type* arg){
	char* space = (char*) allocate(INITIAL_INTERFACE_SIZE);
	return ((Interface*) arg)->copyTo(space);
}

