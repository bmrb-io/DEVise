/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2013
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.4  1999/11/30 22:28:00  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.3  1997/11/24 23:14:09  weaver
  Changes for the new ColorManager.

  Revision 1.2.6.1  1997/05/21 20:39:34  weaver
  Changes for new ColorManager

  Revision 1.2  1997/03/28 16:09:10  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef AssoArray_H
#define AssoArray_H

// Changed to non-pragma templates method. CEW 5/12/97
//#ifdef __GNUG__
//#pragma interface
//#endif

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
        reportErrNosys("Fatal error");//TEMP -- replace with better message
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
	nice_strncpy(pp->name, index, strlen(index)+1);
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
