#ifndef ARRAY_H
#define ARRAY_H

#ifdef __GNUG__
#pragma interface
#endif

#include <assert.h>

#ifndef __GNUG__
using namespace std;
#endif

template<class T>
class Array {
	T* base;
public:
	int length;
	Array(int length) : length(length) {
		base = new T[length];
	}
	Array(const Array<T>& arg) : length(arg.length) {
		base = new T[length];
		for(int i = 0; i < length; i++){
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
			for(int i = 0; i < length; i++){
				base[i] = arg.base[i];
			}
		}
		return *this;
	}
	T& operator[](int i){
		assert(i < length);
		return base[i];
	}
	const T& operator[](int i) const {
		assert(i < length);
		return base[i];
	}
};

template<class T>
void destroyArray(Array<T> arg){
	for(int i = 0; i < arg.length; i++){
		delete arg[i];
	}
}

#endif
