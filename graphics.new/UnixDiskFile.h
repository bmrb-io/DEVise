/* UnixDiskFile.h: Unix DiskFile implementation. */
#ifndef UnixDiskFile_h
#define UnixDiskFile_h
#include "DiskFile.h"
#include <errno.h>


class UnixDiskFile: public DiskFile {
public:
    /* Create a new file. Return NULL if file already exists */
	static UnixDiskFile *CreateFile(char *name);

	/* Open an existing file. Return NULL if file does not exist */
	static UnixDiskFile *OpenFile(char *name);


	/* destructor */
	~UnixDiskFile();

	/* Get data for this page and put it into buffer */
	virtual void ReadPage(int pageNum, void *buf);

	/* Write page */
	virtual void WritePage(int pageNum, void *buf);

protected:
	/* constructor, given the file descriptor */
	UnixDiskFile(int fd);

private:
	int _fd;	/* unix file descriptor */
	int _seekPage; /* last page we seeked to */
};

#endif
