#ifndef AssoArray_H
#define AssoArray_H

#ifdef __GNUG__
#pragma interface
#endif

#include <string.h>
#include <stdio.h>

template <class valType>
class AssoArray {
	
	struct pair {
	     char *name;
	     valType val;
	};

	pair *vec;
	int max;
	int free;

	void init();

  public:
	AssoArray(int size);
	~AssoArray();
	valType &operator[](const char *index);
	void print_all();
	int size() const {return free;}
	void clear() { init(); }
	void remove(valType index);
	valType first();
	valType last();
};

template <class valType>
AssoArray<valType>::AssoArray(int size) {
	max = (size < 12) ? 12 : size;
	free = 0;
	vec = new pair[max];
	if(!vec) {
		fprintf(stderr, "Insufficient memory for AssoArray\n");
		exit(1);
	}
}

template <class valType>
AssoArray<valType>::~AssoArray(){
	for(int i=0; i<max; i++) {
		delete vec[i];
	}
}

template <class valType>
void AssoArray<valType>::init() {
	for(int i=0; i<max; i++) {
		delete vec[i];
	}
	free = 0;
}

template <class valType>
valType &AssoArray<valType>::operator[](const char *index){
	register pair *pp;

	for(pp = &vec[free-1]; free>0 && pp >= vec; pp--)
		if(!strcmp(index, pp->name)) return pp->val;

	if (free == max) {
		pair *nvec = new pair[max*2];
		for(int i=0; i<max; i++) nvec[i] = vec[i];
		delete [] vec;
		vec = nvec;
		max = 2*max;
	}

	pp = &vec[free++];
	pp->name = new char[strlen(index)+1];
	strcpy(pp->name, index);
	pp->val = 0;
	return pp->val;
}

template <class valType>
void AssoArray<valType>::print_all(){
	for(int i=0; i<free; i++)
	  printf("vec[%s] = %g\n", vec[i].name, vec[i].name);
}

template <class valType>
void AssoArray<valType>::remove(valType index){
}

template <class valType>
valType AssoArray<valType>::first(){
	DOASSERT(free>0, "Invalid request, Nothing in the array");
	return vec[0].val;
}

template <class valType>
valType AssoArray<valType>::last(){
	DOASSERT(free>0, "Invalid request, Nothing in the array");
	return vec[free-1].val;
}

#endif
