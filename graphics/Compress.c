/*
  $Id$

  $Log$*/

#include <stdio.h>
#include "Compress.h"
#include "Exit.h"

SimpleCompress::SimpleCompress() { }
const int COMPRESS_BUF_SIZE = 2048;
static unsigned char _compressBuf[COMPRESS_BUF_SIZE];

void SimpleCompress::Compress(CompressIO *cio){
	cio->Begin();
	char *line; int count;
	while ((line = cio->ReadLine(count)) != NULL){
		int outCount;
		char *outBuf = CompressLine(line, count, outCount);
		cio->WriteLine(outBuf,outCount );
	}
	cio->End();
}

void SimpleCompress::Decompress(CompressIO *cio) {
	cio->Begin();
	char *line;
	int count;
	while ((line=cio->ReadLine(count))!= NULL) {
		int outCount;
		char *outBuf = DecompressLine(line, count, outCount);
		cio->WriteLine((char *)_compressBuf, outCount);
	}
	cio->End();
}

char *SimpleCompress::CompressLine(char *line, int count, int &outCount) {
	int outIndex = 0;
	int i = 0;
	while ( i < count) {
		int j;
		for (j = i+1; j < count && line[i] == line[j] ; j++);

		/* line[i..j-1] are the same. Write out with run length
		encoding */
		int num = j-i;
		while (num > 0 ) {
			if (outIndex+1 >= COMPRESS_BUF_SIZE){
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

char *SimpleCompress::DecompressLine(char *line, int count, int &outCount) {
	if (count & 1){
		/* count should be odd */
		fprintf(stderr,"SimpleCompress::DecompressLine: error in input\n");
		Exit::DoExit(2);
	}

	int outIndex = 0;
	int i ;
	for (i=0; i < count; i += 2){
		unsigned num = (unsigned char)line[i];
		unsigned char c = (unsigned char)line[i+1];
		if (outIndex+num > COMPRESS_BUF_SIZE){
			fprintf(stderr,"SimpleCompress::Decompress:buf overflow %d\n",
				outIndex+num);
			Exit::DoExit(1);
		}
		int j;
		for (j=0; j < num; j++){
			_compressBuf[outIndex++] = c;
		}
	}
	outCount = outIndex;
	return (char *)_compressBuf;
}
