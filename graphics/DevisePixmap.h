#ifndef DevisePixmap_h
#define DevisePixmap_h
#include "VisualArg.h"
#include "Compress.h"

struct DevisePixmap {
VisualFilter filter; /* filter used to create this pixmap */
int dataBytes; /* # of TData bytes used to create this pixmap */
int imageBytes; /* # of bytes to store this image */
int compressedBytes; /* # of bytes in image after compression */
int width; /* width/height of image */
int height;
int bytes_per_line; /* # of bytes for each line of data */
int padding; /* amount of padding between scan lines */
unsigned char *data; /* data buffer */
};

/* Pixmap I/O */
class PixmapIO: public CompressIO {
public:
	PixmapIO(); 

	/* beginning/ending of encoding */
	virtual void Begin() ;
	virtual void End() ;

	/* Read next line. return NULL if no more */
	virtual char *ReadLine(int &length);

	/* write line out */
	virtual void WriteLine(char *line, int length);

	/* Setting/resetting params */
	void SetCompress(FILE *file, DevisePixmap *pixmap);
	void SetDecompress(FILE *file);
	DevisePixmap *GetPixmap() { return _pixmap ; }
private:
Boolean _compress;
FILE *_file;
DevisePixmap *_pixmap;
int _line; /* which line we are reading */
};

#endif
