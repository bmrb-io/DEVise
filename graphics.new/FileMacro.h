/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.3  1997/03/28 16:09:44  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.2  1995/09/05 22:14:46  jussi
  Added CVS header.
*/

#ifndef FileMacro_h
#define FileMacro_h

#include "DevError.h"

inline void ReadChunk(int fd, char *buf, int toRead){
	while (toRead > 0){
		int len;
		len = read(fd,buf, toRead);
		if (len > 0){
			toRead -= len;
			buf +=  len;
		}
		else if (len < 0){
			if (errno != EINTR){
				perror("ReacChunk");
                reportErrNosys("Fatal error");//TEMP -- replace with better message
				Exit::DoExit(2);
			}
		}
		else{
			/* len == 0: trying to read beyond EOF */
			fprintf(stderr,"ReadChunk: read beyond EOF\n");
            reportErrNosys("Fatal error");//TEMP -- replace with better message
			Exit::DoExit(2);
		}
	}
}

/* Read chunk returning false if EOF reached (nothing read)*/
inline int ReadChunkStatus(int fd, char *buf, int toRead){
	while (toRead > 0){
		int len;
		len = read(fd,buf, toRead);
		if (len > 0){
			toRead -= len;
			buf +=  len;
		}
		else if (len < 0){
			if (errno != EINTR){
				perror("ReacChunk");
                reportErrNosys("Fatal error");//TEMP -- replace with better message
				Exit::DoExit(2);
			}
		}
		else{
			/* len == 0: trying to read beyond EOF */
			return 0;
		}
	}
	return 1;
}

inline void WriteChunk(int fd, char *buf, int toWrite){

	while (toWrite > 0){
		int len;
		len = write(fd,buf,toWrite);
		if (len >= 0){
			toWrite -= len;
			buf += len;
		}
		else {
			if (errno != EINTR){
				perror("WriteChunk:");
                reportErrNosys("Fatal error");//TEMP -- replace with better message
				Exit::DoExit(2);
			}
		}
	}
}

#endif
