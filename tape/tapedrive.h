/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  University of Wisconsin at Madison
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 20:31:57  jussi
  Added CVS header.
*/

#ifndef TAPEDRIVE_H
#define TAPEDRIVE_H

// uncomment the following #defines to enable features
//#define TAPE_DEBUG                      // print out debugging information
#define TAPE_BLOCKING                   // blocked access to tape
//#define TAPE_BLOCK_PADDING              // records do not cross blocks
//#define PARTIAL_BLOCK_ERROR             // error if partial blocks read

#ifdef TAPE_DEBUG
  #define TAPEDBG(s) s
#else
  #define TAPEDBG(s) // nothing
#endif

#include <stdio.h>
#include "machdep.h"

const int TAPE_BLOCKSIZE = 32768;       // blocking factor on tape

class TapeDrive {
public:
  // Initialize tape interface
  TapeDrive(char *name, char *mode, int fileno = -1,
	    int blockSize = TAPE_BLOCKSIZE);

  // Clean up
  virtual ~TapeDrive();

  // Print usage statistics
  virtual void printStats();

  // Return 1 if properly initialized
  virtual operator int() { return initialized; }

  // Move on tape
  virtual long seek(long offset);

  // Return current position on tape
  virtual long tell() {
    if (!bufferBlock)                   // haven't read any data yet?
      return 0;
    return (bufferBlock - 1) * blockSize + bufferOffset;
  }

  // Read a newline-terminated ASCII record from tape
  virtual int gets(void *buf, int recSize) { return read(buf, recSize, 0); }
  
  // Read a binary record from tape
  virtual int read(void *buf, int recSize) { return read(buf, recSize, 1); }
  
  // Write to tape
  virtual int write(void *buf, int recSize);
  
  // Append to tape
  virtual int append(void *buf, int recSize);

protected:
  // Read a record from tape
  int read(void *buf, int recSize, int binary);

  // Perform tape operation
  int command(short mt_op, daddr_t mt_count);

  // Get tape status
  void getStatus();

  // Fill buffer with next block from tape
  void fillBuffer();

  // Flush full or partial buffer block to tape
  void flushBuffer();

  // Go to specified block
  void gotoBlock(long block);

  // Resynchronize tape
  void resynchronize();

  // Go to beginning of file
  void gotoBeginningOfFile();

  // Go to end of file
  void gotoEndOfFile();

  // Start timer
  void startTimer() {
    gettimeofday(&startTime, 0);
  }

  // Get current timer value
  double getTimer() {
    float diff;
    gettimeofday(&stopTime, 0);
    diff = stopTime.tv_sec - startTime.tv_sec
           + ((int)((stopTime.tv_usec - startTime.tv_usec) / 1e3)) / 1e3;
    return diff;
  }

  int initialized;                      // 1 if properly initialized

  FILE *file;                           // pointer to open file
  int fileNo;                           // file number on tape

  long mt_ios[12];                      // number of I/O calls
  long mt_cnt[12];                      // number of I/O counts
  double mt_tim[12];                    // times of I/O requests

  long read_cnt;                        // number of records read
  long read_ios;                        // number of read() calls
  double read_time;                     // time spent in read()

  long write_cnt;                       // number of records written
  long write_ios;                       // number of write() calls
  double write_time;                    // time spent in write()

  struct timeval startTime;             // start time of timer
  struct timeval stopTime;              // end time of timer

  struct mtget tstat;                   // tape status
  const int blockSize;                  // tape block size

  enum { writeBuffer,
	 readBuffer } bufferType;       // write or read buffer
  int  bufferOffset;                    // current offset into buffer
  int  bufferBytes;                     // number of bytes in buffer
  long bufferBlock;                     // block number of cached block
  char *buffer;                         // block size on tape
  int  atEof;                           // 1 if tape at EOF
};

#endif
