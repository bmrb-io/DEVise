#ifndef DTE_MEMORY_MGR_H
#define DTE_MEMORY_MGR_H

#include <deque>
#include "types.h"

class MemoryLoader {
protected:
	size_t remainSpace;
	deque<void*> pagePtrs;
	void* spacePtr;
	size_t unfreedSpace;
	deque<void*> freePagePtrs;
public:
	static size_t PAGE_SZ;
	MemoryLoader(){
		remainSpace = PAGE_SZ;
		spacePtr = (Type*) new char[PAGE_SZ];
		unfreedSpace = PAGE_SZ;
		pagePtrs.push_back(spacePtr);
	}
	virtual ~MemoryLoader(){
		deque<void*>::iterator it;
		for(it = pagePtrs.begin(); it != pagePtrs.end(); ++it){
			delete [] *it;
		}
		for(it = freePagePtrs.begin(); it != freePagePtrs.end(); ++it){
			delete [] *it;
		}
	}
	void* getNewPage(){
		if(freePagePtrs.empty()){
			return new char[PAGE_SZ];
		}
		void* retVal = freePagePtrs.back();
		freePagePtrs.pop_back();
		return retVal;
	}
	void deallocate(size_t space);
	void* allocate(size_t spaceNeed);
	virtual void reset(){
		deque<void*>::iterator it;
		for(it = pagePtrs.begin(); it != pagePtrs.end(); ++it){
			pagePtrs.pop_front();
			freePagePtrs.push_back(*it);
		}
	}
	virtual Type* load(const Type*){assert(0); return NULL;}
	virtual void drop(Type*){assert(0);}
	deque<void*>::iterator begin(){
		return pagePtrs.begin();
	}
	int size(){
		return pagePtrs.size();
	}
	int getBeginOffset(){
		return PAGE_SZ - unfreedSpace;
	}
	int getEndOffset(){
		return PAGE_SZ - remainSpace;
	}
	bool empty(){
		return unfreedSpace == remainSpace && pagePtrs.size() <= 1;
	}
	void* front(){
		return ((char*) pagePtrs.front()) + getBeginOffset(); 
	}
};

class IntLoader : public MemoryLoader {
public:
	virtual Type* load(const Type* arg){
		return (Type*) arg;
	}
	virtual void drop(Type*){}
};

class StringLoader : public MemoryLoader {
public:
	virtual Type* load(const Type* arg){
		char* retVal = (char*) allocate(strlen((char*) arg) + 1);
		strcpy(retVal, (char*) arg);
		return retVal;
	}
	virtual void drop(Type* arg){
		size_t sz = strlen((char*) arg) + 1;
		deallocate(sz);
	}
};

class InterfaceLoader : public MemoryLoader {
public:
	virtual Type* load(const Type* arg);
	virtual void drop(Type* arg){
		assert(!"not implemented");
	}
};

template <class T>
class MemoryLoaderTemplate : public MemoryLoader {
public:
	virtual Type* load(const Type* arg){
		assert(sizeof(T) % sizeof(int) == 0);
		void* space = allocate(sizeof(T));
		T* retVal = new (space) T;
		*retVal = *((T*) arg);
		return retVal;
	}
	virtual void drop(Type* arg){
		((T*) arg)->~T();
		deallocate(sizeof(T));
	}
	~MemoryLoaderTemplate(){
		deque<void*>::iterator it;
		for(it = pagePtrs.begin(); it != pagePtrs.end(); ++it){
			void* base = *it;
			for(unsigned i = 0; i < PAGE_SZ / sizeof(T); i++){
				(((T*) base) + i)->~T();
			}
		}
	}
};

MemoryLoader** newTypeLoaders(const TypeID* types, int numFlds);    // throws

class TupleLoader {
	int numFlds;
	MemoryLoader** typeLoaders;
	MemoryLoader tupleLoader;
public:
	TupleLoader(){
		typeLoaders = NULL;
	}
	virtual ~TupleLoader(){
		if(typeLoaders){
			for(int i = 0; i < numFlds; i++){
				delete typeLoaders[i];
			}
		}
		delete [] typeLoaders;
	}
	void open(int numFlds, const TypeID* typeIDs){ // throws
		this->numFlds = numFlds;
		TRY(typeLoaders = newTypeLoaders(typeIDs, numFlds), NVOID );
	}
	Tuple* insert(const ConstTuple* tuple){ // throws
		size_t spaceNeed = numFlds * sizeof(Type*);
		Tuple* retVal = (Tuple*) tupleLoader.allocate(spaceNeed);
		for(int i = 0; i < numFlds; i++){
			retVal[i] = typeLoaders[i]->load(tuple[i]);
		}
		return retVal;
	}
	virtual void reset(){
		for(int i = 0; i < numFlds; i++){
			typeLoaders[i]->reset();
		}
		tupleLoader.reset();
	}
	class iterator{
		deque<void*>::iterator pageIt;
		int numPages;
		int beginOffset;
		int endOffset;
		int numFlds;
	private:
		void reset(){
			deque<void*>::iterator tmp;
			pageIt = tmp;
			numPages = beginOffset = endOffset = numFlds = 0;
		}
		bool offsetIsOk();
	public:
		iterator() 
			: numPages(0), beginOffset(0), endOffset(0), numFlds(0) {}
		iterator(const deque<void*>::iterator& pageIt, 
			int numPages, int beginOffset, int endOffset, int numFlds);
		iterator& operator++(){
			assert(numFlds > 0);
			if(offsetIsOk()){
				beginOffset += numFlds * sizeof(void*);
				if(beginOffset == endOffset){
					reset();
				}
			}
			return *this;
		}
		Tuple* operator*(){
			assert(numFlds > 0);
			return (Tuple*) (((char*) *pageIt) + beginOffset);
		}
		bool operator==(const iterator& arg){
			return !(pageIt != arg.pageIt || numPages != arg.numPages ||
				beginOffset != arg.beginOffset || 
				endOffset != arg.endOffset || numFlds != arg.numFlds);
		}
	};
	iterator begin(){
		deque<void*>::iterator pageIt;
		pageIt = tupleLoader.begin();
		int numPages = tupleLoader.size();
		int beginOffset = tupleLoader.getBeginOffset();
		int endOffset = tupleLoader.getEndOffset();
		return iterator(pageIt, numPages, beginOffset, endOffset, numFlds);
	}
	iterator end(){
		iterator retVal;
		return retVal;
	}
	bool empty(){
		return tupleLoader.empty();
	}
	void pop_front(){
		tupleLoader.deallocate(numFlds * sizeof(void*));
	}
	const Tuple* front(){
		return (Tuple*) tupleLoader.front();
	}
};

#endif
