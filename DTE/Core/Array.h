#ifndef ARRAY_H
#define ARRAY_H

#ifdef __GNUG__
#pragma interface
#endif

#include <assert.h>

template<class T>
class Array {
	T* base;
public:
	unsigned length;
	Array(unsigned length) : length(length) {
		base = new T[length];
	}
	Array(const Array<T>& arg) : length(arg.length) {
		base = new T[length];
		for(unsigned i = 0; i < length; i++){
			base[i] = arg.base[i];
		}
	}
	~Array(){
		delete [] base;
	}
	Array<T>& operator=(const Array<T>& arg){
		if(this != &arg){
			length = arg.length;
			delete [] base;
			base = new T[length];
			for(unsigned i = 0; i < length; i++){
				base[i] = arg.base[i];
			}
		}
		return *this;
	}
	T& operator[](unsigned i){
		assert(i < length);
		return base[i];
	}
	const T& operator[](unsigned i) const {
		assert(i < length);
		return base[i];
	}
};

template<class T>
void destroyArray(Array<T> arg){
	for(unsigned i = 0; i < arg.length; i++){
		delete arg[i];
	}
}

#endif
