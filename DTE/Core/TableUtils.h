#ifndef TABLE_UTILS_H
#define TABLE_UTILS_H

#include "types.h"

class Iterator;

Iterator* createIteratorFor(
     const ISchema& schema, istream* in, const string& tableStr);

class TableMap {
	int bitmap;
public:
	class Iterator {
		int refMap;
		int currMap;
		int counter;
	public:
		Iterator();
		Iterator(int bitmap, int counter = 0);
		inline Iterator& operator++();
		inline TableMap operator*();
		inline bool operator==(const Iterator& it);
		inline TableMap getComplement();
	};

	inline TableMap(int bitmap = 0);
	void initialize(int numTabs);
	Iterator begin();
	Iterator end();
	int count() const; // returns the number of 1s
	inline bool isSinglet() const;
	int getBitMap() const {
		return bitmap;
	}
	bool contains(TableMap x) const {
		return (bitmap == (bitmap | x.bitmap));
	}
	TableMap operator|(TableMap x) const {
		return TableMap(bitmap | x.bitmap);
	}
	friend ostream& operator<<(ostream&, const TableMap&);
	string toString() const;
	int index() const;  // returns the position of the first 1, counting from the least sig. bit position.
};

inline TableMap::TableMap(int bitmap) : bitmap(bitmap) {}

inline TableMap::Iterator& TableMap::Iterator::operator++(){
     counter++;
	currMap = refMap & (currMap - refMap);
	return *this;
}

inline bool TableMap::Iterator::operator==(const TableMap::Iterator& it){
	return counter == it.counter;
}

inline TableMap TableMap::Iterator::operator*(){
	return TableMap(currMap);
}

inline TableMap TableMap::Iterator::getComplement(){
	return TableMap(refMap ^ currMap);
}

inline bool TableMap::isSinglet() const {
	return count() == 1;
}

#endif
