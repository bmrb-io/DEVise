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
  Revision 1.17  1998/03/17 17:19:07  donjerko
  Added new namespace management through relation ids.

  Revision 1.16  1997/12/04 04:05:18  donjerko
  *** empty log message ***

  Revision 1.15  1997/11/24 12:45:06  okan
  Added record operationts to ODBC

  Revision 1.14  1997/11/24 05:25:37  okan
  *** empty log message ***

  Revision 1.12  1997/11/12 23:17:39  donjerko
  Improved error checking.

  Revision 1.11  1997/11/05 00:19:44  donjerko
  Separated typechecking from optimization.

  Revision 1.10  1997/10/02 02:27:30  donjerko
  Implementing moving aggregates.

  Revision 1.9  1997/09/05 22:20:15  donjerko
  Made changes for port to NT.

  Revision 1.8  1997/08/25 15:28:13  donjerko
  Added minmax table

  Revision 1.7  1997/08/21 21:04:30  donjerko
  Implemented view materialization

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

#ifndef DTE_EXCEPTION_H
#define DTE_EXCEPTION_H

#include <string>

//#include <fstream.h>   erased for sysdep.h
#include <assert.h>
//#include <sys/time.h>   //erased for sysdep.h
//#include <iomanip.h>   erased for sysdep.h
#include "sysdep.h"
// #include "machdep.h"

#ifndef __GNUG__
using namespace std;
#endif

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
	const string& toString(){
		return text;
	}
};

extern Exception* currExcept;

class DefaultExceptHndl{
public:
	~DefaultExceptHndl(){
		if(currExcept){
			cerr << currExcept->toString();
			cerr << endl;
		}
	}
};


class ITimer {
	timeval initT;
	timeval lastT;
	bool isInit;
private:
	float subtract(timeval bigger, timeval smaller){
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
//		assert(gettimeofday(&initT, NULL) != -1);
		lastT = initT;
		isInit = true;
	}
	ostream& display(ostream& out){
		if(!isInit){
			reset();
		}
		timeval curT;
//		assert(gettimeofday(&curT, NULL) != -1);
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
		cout << currExcept->toString();\
		cout << endl;\
	}\
	assert(!currExcept); currExcept = A; return B 

#define CON_THROW(A)\
	if(currExcept){\
		cout << "\nUncaught exception found:\n";\
		cout << currExcept->toString();\
		cout << endl;\
	}\
	assert(!currExcept); currExcept = A; goto CON_END;

#define TRY(A,B) A; if(currExcept){return B;}
#define CON_TRY(A) A; if(currExcept){goto CON_END;}
#define CHECK(A,B,C) A; if(currExcept){currExcept->append(B); return C;}
#define CATCH(A) if(currExcept){A; delete currExcept; currExcept = NULL;}
#define EXIT(A) A; if(currExcept){cerr << currExcept->toString() << endl; currExcept = 0; exit(1);}

#endif
