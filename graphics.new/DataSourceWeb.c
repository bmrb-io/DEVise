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
  Implementation of DataSourceWeb class.

  It derives from the DataSourceFileDesc class and makes _file
  be the file pointer of the cache file instead of the fdopen()'d
  pointer to the file descriptor of the socket which is the default
  behavior of the DataSourceFileDesc class.
 */

/*
  $Id$

  $Log$*/

#define _DataSourceWeb_c_

//#define DEBUG

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "DataSourceWeb.h"
#include "Timer.h"
#include "Util.h"
#include "Web.h"
#include "DevError.h"

#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: DataSourceWeb::DataSourceWeb
 * DataSourceWeb constructor.
 */
DataSourceWeb::DataSourceWeb(char *url, char *label, char *cache) :
	DataSourceFileStream("no file", label)
{
    DO_DEBUG(printf("DataSourceWeb::DataSourceWeb(%s,%s,%s)\n",
                    url, cache, (label != NULL) ? label : "null"));

    _url = strdup(url);
    _cache = strdup(cache);
    _fd = -1;
    _cfile = NULL;
}

/*------------------------------------------------------------------------------
 * function: DataSourceWeb::~DataSourceWeb
 * DataSourceWeb destructor.
 */
DataSourceWeb::~DataSourceWeb()
{
    DO_DEBUG(printf("DataSourceWeb::~DataSourceWeb()\n"));

    if (_cfile != NULL) fclose(_cfile);
    if (_fd >= 0) close(_fd);
    delete _url;
    delete _cache;
}

/*------------------------------------------------------------------------------
 * function: DataSourceWeb::Open
 * Do an fdopen() on the file descriptor.
 */
DevStatus
DataSourceWeb::Open(char *mode)
{
    DO_DEBUG(printf("DataSourceWeb::Open()\n"));

    if (strcmp(mode, "r"))
    {
      reportError("cannot write to a Web data source", EINVAL);
      return StatusFailed;
    }

    // if file can be opened for reading, assume that the cache file
    // is complete and there is no need to fetch more data from the URL
    _file = fopen(_cache, "r");
    if (_file != NULL) {
        return StatusOk;
    }

    // attempt to create an empty cache file
    _cfile = fopen(_cache, "w");
    if (_cfile == NULL)
    {
        char errBuf[MAXPATHLEN+100];
        sprintf(errBuf, "unable to create cache file %s", _cache);
        reportError(errBuf, errno);
        return StatusFailed;
    }

    // open a second instance of the same file, for reading
    _file = fopen(_cache, "r");
    DOASSERT(_file != NULL, "Invalid file pointer");

    Timer::StopTimer();

    size_t len = 0;
    if (!strncmp(_url, "ftp://", 6))
      _fd = open_ftp(_url);
    else
      _fd = open_http(_url, &len);

    Timer::StartTimer();

    if (_fd < 0)
    {
        char errBuf[256];
        sprintf(errBuf, "unable to open URL %s", _url);
        reportError(errBuf, errno);
        return StatusFailed;
    }

    return StatusOk;
}

/*------------------------------------------------------------------------------
 * function: DataSourceWeb::IsOk
 * Return true if cache file is valid.
 */
Boolean
DataSourceWeb::IsOk()
{
    if (_file == NULL)
        return false;

    struct stat sbuf;
    if (stat(_cache, &sbuf) < 0)
        return false;

    return true;
}


/*------------------------------------------------------------------------------
 * function: DataSourceWeb::Close
 * Do a close() on the file descriptor.
 */
DevStatus
DataSourceWeb::Close()
{
    DO_DEBUG(printf("DataSourceWeb::Close()\n"));

    if (_cfile != NULL) fclose(_cfile);
    if (_fd >= 0) close(_fd);

    _fd = -1;
    _cfile = NULL;

    return DataSourceFileStream::Close();
}

/*------------------------------------------------------------------------------
 * function: DataSourceWeb::AsyncIO
 * Returns the file descriptor associated with this object.
 */
void
DataSourceWeb::AsyncIO()
{
    DO_DEBUG(printf("DataSourceWeb::AsyncIO()\n"));

    Timer::StopTimer();

    char buffer[1024];

    int len = read(_fd, buffer, sizeof buffer);
    if (len < 0) {
        reportError("cannot read from network", errno);
        printf("Closing connection to %s\n", _url);
    }
    if (len == 0) {
        printf("Data transfer from %s complete\n", _url);
    }
    if (len <= 0) {
        fclose(_cfile);
        close(_fd);
        _cfile = NULL;
        _fd = -1;
    }
    if (len > 0) {
        if (gotoEnd() < 0)
          reportError("cannot go to end of file", errno);
        else {
            if (fwrite(buffer, len, 1, _cfile) != 1)
              reportError("cannot write to cache file", errno);
        }
    }

    Timer::StartTimer();
}

/*------------------------------------------------------------------------------
 * function: DataSourceWeb::Fwrite
 * Do fwrite() on the stream associated with this object.
 */
size_t
DataSourceWeb::Fwrite(const char *buf, size_t size, size_t itemCount)
{
    DO_DEBUG(printf("DataSourceWeb::Fwrite()\n"));

    reportError("writing to Web data source not supported", EINVAL);
    return 0;
}

/*------------------------------------------------------------------------------
 * function: DataSourceWeb::Write
 * Do write() on the stream associated with this object.
 */
size_t
DataSourceWeb::Write(const char *buf, size_t byteCount)
{
    DO_DEBUG(printf("DataSourceWeb::Write()\n"));

    reportError("writing to Web data source not supported", EINVAL);
    return 0;
}

/*------------------------------------------------------------------------------
 * function: DataSourceWeb::append
 * Append the given record to the end of the file associated with this
 * object.
 */
int
DataSourceWeb::append(void *buf, int recSize)
{
    DO_DEBUG(printf("DataSourceWeb::append()\n"));
    int result = 0;

    reportError("writing to Web data source not supported", EINVAL);
    return -1;
}

/*============================================================================*/
