/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.6  1997/08/12 19:58:44  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.5  1997/06/21 22:48:07  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.4  1997/04/14 20:44:12  donjerko
  Removed class Path and introduced new BaseSelection class Member.

  Revision 1.3  1996/12/21 22:21:48  donjerko
  Added hierarchical namespace.

  Revision 1.2  1996/12/05 16:06:01  wenger
  Added standard Devise file headers.

 */

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

#include <fstream.h>
#include <assert.h>
#include <sys/time.h>
#include <iomanip.h>

// #include "machdep.h"

// #define LOG(A) A
#define LOG(A)

LOG(extern ofstream logFile);

class Exception{
	string text;
public:
	Exception(const string& text) : text(text){}
	void append(const string& more){
		text += string("\n") + more;
	}
	void display(ostream& out = cout){
		out << "1 " << text;
	}
};

extern Exception* currExcept;

class DefaultExceptHndl{
public:
	~DefaultExceptHndl(){
		if(currExcept){
			currExcept->display();
			cout << endl;
		}
	}
};

typedef struct timeval Timeval;

class ITimer {
	Timeval initT;
	Timeval lastT;
	bool isInit;
private:
	float subtract(Timeval bigger, Timeval smaller){
		if (smaller.tv_usec > bigger.tv_usec) {

             bigger.tv_usec += 1000000;
             bigger.tv_sec--;

          }
          int usec = bigger.tv_usec - smaller.tv_usec;
          int sec = bigger.tv_sec - smaller.tv_sec;
		return sec + usec / float(1000000);
	}
public:
	ITimer() : isInit(false) {}
	void reset(){
		assert(gettimeofday(&initT, NULL) != -1);
		lastT = initT;
		isInit = true;
	}
	ostream& display(ostream& out){
		if(!isInit){
			reset();
		}
		Timeval curT;
		assert(gettimeofday(&curT, NULL) != -1);
		float fromInit = subtract(curT, initT);
		float fromLast = subtract(curT, lastT);
		out << "time(" << fromInit << ", ";
		out << fromLast << ")"; 
		lastT = curT;
		return out;
	}
};

extern ITimer iTimer;

#define THROW(A,B)\
	if(currExcept){\
		cout << "\nUncaught exception found:\n";\
		currExcept->display();\
		cout << endl;\
	}\
	assert(!currExcept); currExcept = A; return B 

#define TRY(A,B) A; if(currExcept){return B;}
#define CHECK(A,B) if(currExcept){currExcept->append(A); return B;}
#define CATCH(A) if(currExcept){A; currExcept = NULL;}

#endif
