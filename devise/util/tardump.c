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
  Revision 1.1  1996/05/16 18:24:33  jussi
  Initial revision.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <tar.h>
#include <fcntl.h>

#if !defined(FD_READ) && !defined(FILE_READ)
#define FD_READ
#endif

#ifdef FD_READ
#include <unistd.h>
#endif

#ifndef TBLOCK
#define TBLOCK 512
#endif

#ifndef NAMSIZ
#define NAMSIZ 100
#endif

union hblock {
  char dummy[TBLOCK];
  struct header {
    char name[NAMSIZ];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char linkflag;
    char linkname[NAMSIZ];
  } dbuf;
};

#define blocksize (20 * TBLOCK)

inline void getBlock(char **buf,
#ifdef FD_READ
		     int fd
#else
		     FILE *file
#endif
		     )
{
  static char block[blocksize];
  static int nblocks = 0;
  static int curblock = 0;

  if (curblock >= nblocks) {
#ifdef FD_READ
    if (read(fd, block, blocksize) < blocksize) {
#else
    if (fread(block, blocksize, 1, file) != 1) {
#endif
      perror("read");
      exit(1);
    }
    nblocks = blocksize / TBLOCK;
    curblock = 0;
  }

  *buf = &block[curblock * TBLOCK];
  curblock++;
}

unsigned long int oct2int(char *buf)
{
  unsigned long int num = 0;
  while(*buf == ' ') buf++;
  while(*buf != ' ')
    num = 8 * num + (*buf++ - '0');
  return num;
}

int isEOT(char *buf)
{
  int i;
  for(i = 0; i < TBLOCK; i++)
    if (buf[i] != 0)
      return 0;
  return 1;
}

int isMatch(int n, char **str, char *fn)
{
  int i;
  for(i = 0; i < n; i++)
    if (!strncmp(fn, str[i], strlen(str[i])))
      return 1;
  return 0;
}

int main(int argc, char ** argv)
{
  int fnames;
  char **fns;
#ifdef FD_READ
  int file;
#else
  FILE *file;
#endif

  if (argc < 2) {
    fprintf(stderr, "Usage: %s tapedevice [filename...]\n", argv[0]);
    fprintf(stderr, "       extracts specified files from tar archive to stdout\n");
    exit(1);
  }

  fnames = argc - 2;
  fns = &argv[2];

#ifdef FD_READ
  if ((file = open(argv[1], O_RDONLY, 0)) < 0) {
#else
  if (!(file = fopen(argv[1], "r"))) {
#endif
    perror("open");
    exit(1);
  }

  while(1) {
    union hblock *hdr;
    unsigned long int size;
    unsigned long int bytes;
    int match = 0;

    char *header;
    getBlock(&header, file);
    if (isEOT(header))
      break;
    hdr = (union hblock *)header;
    match = (!fnames || isMatch(fnames, fns, hdr->dbuf.name));
    size = oct2int(hdr->dbuf.size);
    if (match)
      fprintf(stderr, "x %s, %lu bytes\n", hdr->dbuf.name, size);
    for(bytes = 0; bytes < size; bytes += TBLOCK) {
      char *data;
      getBlock(&data, file);
#if 1
      if (match)
	fwrite(data, (size - bytes < TBLOCK ? size - bytes : TBLOCK),
	       1, stdout);
#endif
    }
  }

#ifdef FD_READ
  if (close(file) < 0) {
#else
  if (fclose(file)) {
#endif
    perror("close");
    exit(1);
  }

  return 0;
}
