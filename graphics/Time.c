/* Time.c */
#include "Time.h"

struct timeval Time::_beginning;

/*********************************************************/
void Time::Init(){
	struct timezone zone;
	gettimeofday(&_beginning, &zone);

}

/***********************************************************/
long Time::Now(){
	struct timezone zone;
	struct timeval now;
	gettimeofday(&now,&zone);

	 return (now.tv_sec - _beginning.tv_sec)*1000+
		  (now.tv_usec - _beginning.tv_usec)/1000;
}



