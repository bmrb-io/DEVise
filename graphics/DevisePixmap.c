/*
  $Id$

  $Log$*/

#include <stdio.h>
#include "Exit.h"
#include "DevisePixmap.h"

PixmapIO::PixmapIO(){
}

void PixmapIO::SetDecompress(FILE *file){
	_compress = false;
	_file = file;
}

void PixmapIO::SetCompress(FILE *file, DevisePixmap *pixmap){
	_compress = true;
	_file = file;
	_pixmap = pixmap;
}

/* beginning/ending of encoding */
void PixmapIO::Begin() {
	if (_compress){
		/*
		printf("Begin Compress\n");
		*/
		if (fwrite(_pixmap,sizeof(*_pixmap),1,_file) != 1){
			perror("PixmapIO::WritePixmap");
			Exit::DoExit(1);
		}
		/*
		printf("compressing %d bytes\n", _pixmap->imageBytes);
		*/
	} else {
		/* decompress */
		/*
		printf("Begin Decompress\n");
		*/
		_pixmap = new DevisePixmap();
		if (fread(_pixmap,sizeof(*_pixmap),1,_file) != 1){
			perror("PixmapIO::LoadPixmap");
			Exit::DoExit(1);
		}
		unsigned char *data = (unsigned char *)
				malloc(_pixmap->imageBytes);
		if (data == NULL){
			fprintf(stderr,"PixmapIO::LoadPixmap: no memory\n");
			Exit::DoExit(1);
		}
		_pixmap->data = data;
		/*
		printf("allocating %d bytes\n", _pixmap->imageBytes);
		*/
	}
	_line = 0;
}

void PixmapIO::End(){
}

const int PIXMAPIO_BUFSIZE = 5120;
static char pixBuf[PIXMAPIO_BUFSIZE];
/* Read next line. return NULL if no more */
char *PixmapIO::ReadLine(int &length){
/*
printf("ReadLine\n");
*/
	if (_line == _pixmap->height)
		return NULL;
	char *data;
	if (_compress) {
		data = (char *)&_pixmap->data[_line*_pixmap->bytes_per_line];
		length = _pixmap->bytes_per_line;
	}
	else {
		int numBytes;
		if (fread(&numBytes,sizeof(numBytes),1,_file) != 1
			|| numBytes > PIXMAPIO_BUFSIZE ){
			perror("PixmapIO::LoadPixmap numBytes");
			Exit::DoExit(1);
		}
		if (fread(pixBuf,numBytes,1,_file) != 1){
			perror("PixmapIO::LoadPixmap read data");
			Exit::DoExit(1);
		}
		data = pixBuf;
		length = numBytes;
	}
	/*
	printf("ReadLine %d bytes\n",length);
	*/
	return data;
}

/* write line out */
void PixmapIO::WriteLine(char *line, int length){
/*
printf("PixmapIO::WriteLine len=%d\n",length);
*/
	if (_compress) {
		if (fwrite(&length,sizeof(length),1,_file) != 1){
			perror("PixmapIO::WritePixmap numBytes");
			Exit::DoExit(1);
		}
		if (fwrite(line,length,1,_file) != 1){
			perror("PixmapIO::WritePixmap data");
			Exit::DoExit(1);
		}
	}
	else {
		if (_line == _pixmap->height){
			fprintf(stderr,"PixmapIO::LoadPixmap: pixmap buf overflow\n");
			Exit::DoExit(1);
		}
		char *data = (char *)&_pixmap->data[_line*_pixmap->bytes_per_line];
		bcopy(line, data,length);
	}
	_line++;
}
