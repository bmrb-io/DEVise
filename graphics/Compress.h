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
  Revision 1.2  1995/09/05 21:12:30  jussi
  Added/updated CVS header.
*/

#ifndef Compress_h
#define Compress_h

class CompressIO {
public:
  /* beginning/ending of encoding */
  virtual void Begin() = 0;
  virtual void End() = 0;

  /* Read next line. return NULL if no more */
  virtual char *ReadLine(int &length) = 0;

  /* write line out */
  virtual void WriteLine(char *line, int length) = 0;
};

class Compression {
public:
  /* These are for whole image compression using cio */
  virtual void Compress(CompressIO *cio ) = 0;
  virtual void Decompress(CompressIO *cio) = 0;

  /* Interface w/o cio */
  virtual char *CompressLine(char *line, int count, int &outCount) = 0;
  virtual char *DecompressLine(char *line, int count, int &outCount) = 0;
};

class SimpleCompress: public Compression {
public:
  SimpleCompress();
  virtual void Compress(CompressIO *cio );
  virtual void Decompress(CompressIO *cio);
  
  virtual char *CompressLine(char *line, int count, int &outCount);
  virtual char *DecompressLine(char *line, int count, int &outCount);
};

#endif
