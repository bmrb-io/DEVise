/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2  1995/09/05 21:12:29  jussi
  Added/updated CVS header.
*/

#include <stdio.h>
#include "Compress.h"
#include "Exit.h"

static const int COMPRESS_BUF_SIZE = 2048;
static unsigned char _compressBuf[COMPRESS_BUF_SIZE];

SimpleCompress::SimpleCompress()
{
}

void SimpleCompress::Compress(CompressIO *cio)
{
  char *line;
  int count;

  cio->Begin();
  while ((line = cio->ReadLine(count)) != NULL){
    int outCount;
    char *outBuf = CompressLine(line, count, outCount);
    cio->WriteLine(outBuf, outCount);
  }
  cio->End();
}

void SimpleCompress::Decompress(CompressIO *cio)
{
  char *line;
  int count;

  cio->Begin();
  while ((line = cio->ReadLine(count))!= NULL) {
    int outCount;
    char *outBuf = DecompressLine(line, count, outCount);
    cio->WriteLine((char *)_compressBuf, outCount);
  }
  cio->End();
}

char *SimpleCompress::CompressLine(char *line, int count, int &outCount)
{
  int outIndex = 0;
  int i = 0;

  while ( i < count) {
    for(int j = i+1; j < count && line[i] == line[j] ; j++);
    /* line[i..j-1] are the same. Write out with run length
       encoding */
    int num = j - i;
    while (num > 0 ) {
      if (outIndex + 1 >= COMPRESS_BUF_SIZE) {
	fprintf(stderr,"SimpleCompress::Compress:buf overflow\n");
	Exit::DoExit(1);
      }
      int amount = num;
      if (amount > 255) amount = 255;
      _compressBuf[outIndex++] = (unsigned char)amount;
      _compressBuf[outIndex++] = (unsigned char)line[i];
      num -= amount;
    }
    i = j;
  }

  outCount = outIndex;
  return (char *)_compressBuf;
}

char *SimpleCompress::DecompressLine(char *line, int count, int &outCount)
{
  if (count & 1) {
    /* count should be odd */
    fprintf(stderr, "SimpleCompress::DecompressLine: error in input\n");
    Exit::DoExit(2);
  }
  
  int outIndex = 0;
  for(int i = 0; i < count; i += 2) {
    unsigned int num = (unsigned char)line[i];
    unsigned char c = (unsigned char)line[i + 1];
    if (outIndex + num > COMPRESS_BUF_SIZE){
      fprintf(stderr, "SimpleCompress::Decompress:buf overflow %d\n",
	      outIndex + num);
      Exit::DoExit(1);
    }
    for(int j = 0; j < num; j++) {
      _compressBuf[outIndex++] = c;
    }
  }

  outCount = outIndex;
  return (char *)_compressBuf;
}
