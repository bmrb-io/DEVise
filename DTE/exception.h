#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <fstream.h>
#include <assert.h>
#include <sys/time.h>
#include <iomanip.h>

#include "machdep.h"

#define LOG(A) A

LOG(extern ofstream logFile);

class Exception{
	String text;
public:
	Exception(String text) : text(text){}
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
		cout << "ITimer reset; ";
		display(cout);
		cout << endl;
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

#define THROW(A,B) assert(!currExcept); currExcept = A; return B 
#define TRY(A,B) A; if(currExcept){return B;}
#define CATCH(A) if(currExcept){A; currExcept = NULL;}
#endif
