/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2000
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of JavaScreenCache class.
 */

/*
  $Id$

  $Log$
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "JavaScreenCache.h"
#include "JavaScreenCmd.h"
#include "DevError.h"
#include "Util.h"
#include "Init.h"
#include "Display.h"
#include "ArgList.h"
#include "DebugLog.h"
#include "DevFileHeader.h"

//#define DEBUG
//#define DEBUG_LOG

// Buffer for debug logging.
static char logBuf[MAXPATHLEN * 2];

static const int _cacheMajorVersion = 1;
static const int _cacheMinorVersion = 0;

static const char *_dispSizePrefix = "display size: ";
static const char *_protocolVersionPrefix = "protocol version: ";
static const char *_cacheVersionPrefix = "cache version: ";
static const char *_cmdPrefix = "command: ";

//====================================================================
// Constructor.
JavaScreenCache::JavaScreenCache()
{
    _recording = false;
    _recordingStatus = StatusInvalid;
    _commandFileName = NULL;
    _commandFile = NULL;
    _dataFileName = NULL;
    _dataFile = -1;
    _playingBack = false;
}

//====================================================================
// Destructor.
JavaScreenCache::~JavaScreenCache()
{
    if (_recording) {
        StopRecording();
    } else if (_playingBack) {
        StopPlayingBack();
    }
}

//====================================================================
// Get the line beginning with the given prefix, if any, from the file,
// and return the part of the line after the prefix.  Returns NULL if
// the given line is not found or if there is an error.  If multiple
// lines start with the given prefix, the information in the first one
// is returned.
static char *
GetLineValue(FILE *fp, const char *prefix, char buf[], int bufSize)
{
char *result = NULL;

int prefixLen = strlen(prefix);

    rewind(fp);
    Boolean done = false;
    while (!done) {
        fgets_result tmpResult = nice_fgets(buf, bufSize, fp);
        if (tmpResult == fgets_ok) {
            if (!strncmp(buf, prefix, prefixLen)) {
                result = &buf[prefixLen];
                done = true;
            }
        } else if (tmpResult == fgets_eof) {
            done = true;
        } else {
	    reportErrNosys("Error in nice_fgets()");
            done = true;
        }
    }

    return result;
}

//====================================================================
DevStatus
JavaScreenCache::StartPlayingBack(JavaScreenCmd *jsc, const char *sessionFile,
  int protocolMajorVersion)
{
#if defined (DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCache::StartPlayingBack(%s)\n", sessionFile);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

    DevStatus result(StatusOk);

    result += OpenCacheFiles(sessionFile, false);

    if (result.IsComplete()) {
        //
        // Make sure command and data cache files are newer than session
        // file.
        //
        struct stat sessionStat, commandStat, dataStat;
        if ((stat(sessionFile, &sessionStat) != 0) ||
          (fstat(fileno(_commandFile), &commandStat) != 0) ||
          (fstat(_dataFile, &dataStat) != 0)) {
            reportErrSys("Error getting file status");
            result += StatusFailed;
        } else {
            if (sessionStat.st_mtime > commandStat.st_mtime) {
                reportErrNosys("Command cache file is older than session file");
                result += StatusCancel;
            }

            if (sessionStat.st_mtime > dataStat.st_mtime) {
                reportErrNosys("Data cache file is older than session file");
                result += StatusCancel;
            }
        }

        //TEMP -- Maybe check internal date of command cache file.
    }

    if (result.IsComplete()) {

        //
        // Check file header (make sure this really is a command cache file).
        //
        DevFileHeader::Info info;
        result += DevFileHeader::Read(_commandFile, info);

        if (result.IsComplete()) {
            if (strcmp(info.fileType, FILE_TYPE_JSCMDCACHE)) {
                char errBuf[MAXPATHLEN + 128];
                sprintf(errBuf,
                  "File %s is not a JavaScreen command cache file",
                 _commandFileName);
                reportErrNosys(errBuf);
                  result += StatusFailed;
            }
        }
    }

    if (result.IsComplete()) {
        const int bufSize = 512;
        char buf[bufSize];

        //
        // Check display size for match.
        //
        char *sizeVal = GetLineValue(_commandFile, _dispSizePrefix, buf,
          bufSize);
        if (sizeVal) {
            int cacheWidth, cacheHeight;
            sscanf(sizeVal, "%dx%d", &cacheWidth, &cacheHeight);
            int curWidth =
              DeviseDisplay::DefaultDisplay()->DesiredScreenWidth();
            int curHeight =
              DeviseDisplay::DefaultDisplay()->DesiredScreenHeight();
            if (cacheWidth != curWidth || cacheHeight != curHeight) {
                char errBuf[1024];
                sprintf(errBuf, "Current display size %dx%d does not match cache file display size of %dx%d; cached data not used",
                  curWidth, curHeight, cacheWidth, cacheHeight);
                reportErrNosys(errBuf);
                result += StatusFailed;
            }
        } else {
            reportErrNosys("Can't find display size in cache file");
            result += StatusFailed;
        }

        //
        // Check protocol version for match.
        //
        char *protVerVal = GetLineValue(_commandFile, _protocolVersionPrefix,
          buf, bufSize);
        if (protVerVal) {
            int majorVer, minorVer;
            sscanf(protVerVal, "%d.%d", &majorVer, &minorVer);
            if (majorVer != protocolMajorVersion) {
                reportErrNosys("Cache file protocol version mismatch");
                result += StatusFailed;
            }
        } else {
              reportErrNosys("Can't find protocol version in cache file");
                result += StatusFailed;
        }

        //
        // Check cache version for match.
        //
        char *cacheVerVal = GetLineValue(_commandFile, _cacheVersionPrefix,
          buf, bufSize);
        if (cacheVerVal) {
            int majorVer, minorVer;
            sscanf(cacheVerVal, "%d.%d", &majorVer, &minorVer);
            if (majorVer != _cacheMajorVersion) {
                reportErrNosys("Cache file cache version mismatch");
                result += StatusFailed;
            }
        } else {
            reportErrNosys("Can't find cache version in cache file");
            result += StatusFailed;
        }
   }

    if (result.IsComplete()) {
        // Play back the command file.
        int prefixLen = strlen(_cmdPrefix);
        const int cmdBufSize = 512;
        char cmdBuf[cmdBufSize];
        while (nice_fgets(cmdBuf, cmdBufSize, _commandFile) != fgets_eof) {
            if (!strncmp(cmdBuf, _cmdPrefix, prefixLen)) {
                result += SendCmd(jsc, &cmdBuf[prefixLen]);
            }
        }
    }

    if (result.IsComplete()) {
        // Play back the data (image and GData) file.
        int tmp;
	//TEMP -- there may be some problems here if we actually send
	// checksum values to the JavaScreen.
        //TEMP -- maybe don't use SendWindowData here
        if (jsc->SendWindowData(_dataFileName, false, tmp) !=
	  JavaScreenCmd::DONE) {
	    reportErrNosys("Error sending cached data");
	    result += StatusFailed;
	} else {
            // This must be set *after* sending the data, otherwise
            // SendWindowData() won't do anything.
           _playingBack = true;
       }
    }

    //TEMP -- if we send JAVAC_Done here, what happens if user does
    // something while devised is still really opening the session?
    // It seems to work okay.  RKW 2000-06-02.

    if (!_playingBack) {
       result += CloseCacheFiles(false);
    }

    return result;
}

//====================================================================
DevStatus
JavaScreenCache::SendCmd(JavaScreenCmd *jsc, const char* cmd)
{
    DevStatus result(StatusOk);

    ArgList args;
    args.ParseString(cmd);

#if defined(DEBUG)
    printf("Sending command: ");
    PrintArgs(stdout, args.GetCount(), args.GetArgs(), true);
#endif

    //TEMP -- eliminate cast
    if (jsc->ReturnVal(args.GetCount(), (char **)args.GetArgs()) < 0) {
        result += StatusFailed;
    }

    return result;
}

//====================================================================
DevStatus
JavaScreenCache::StartRecording(const char *sessionFile,
  int protocolMajorVersion, int protocolMinorVersion)
{
#if defined (DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCache::StartRecording(%s)\n", sessionFile);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

    DevStatus result(StatusOk);

    if (_recording) {
        reportErrNosys("Can't start recording when already recording");
        result = StatusFailed;
    }

    if (result.IsComplete()) {
        result += OpenCacheFiles(sessionFile, true);
    }

    if (result.IsComplete()) {
        _recording = true;
        _recordingStatus = StatusOk;

        // Write file header.
        const char *header = DevFileHeader::Get(FILE_TYPE_JSCMDCACHE);
        fprintf(_commandFile, "%s\n", header);

        // Save display size.
        int width = DeviseDisplay::DefaultDisplay()->DesiredScreenWidth();
        int height = DeviseDisplay::DefaultDisplay()->DesiredScreenHeight();
        fprintf(_commandFile, "%s%dx%d\n", _dispSizePrefix, width, height);

        // Save protocol version.
        fprintf(_commandFile, "%s%d.%d\n", _protocolVersionPrefix,
        protocolMajorVersion, protocolMinorVersion);

        // Save cache version.
        fprintf(_commandFile, "%s%d.%d\n\n", _cacheVersionPrefix,
          _cacheMajorVersion, _cacheMinorVersion);

        //TEMP -- maybe save session file name here.

	//TEMP -- maybe save data cache file name here.
    } else {
        if (_commandFile) {
            (void)fclose(_commandFile);
            _commandFile = NULL;
        }
    }

    _dataSize = 0;

    return result;
}

//====================================================================
DevStatus
JavaScreenCache::OpenCacheFiles(const char *sessionFile, Boolean writing)
{
    DevStatus result(StatusOk);

    _commandFile = NULL;
    _dataFile = -1;

    if (result.IsComplete()) {
        char fileBuf[MAXPATHLEN];
        sprintf(fileBuf, "%s.commands", sessionFile);
        _commandFileName = CopyString(fileBuf);
        _commandFile = fopen(_commandFileName, writing ? "w" : "r");
        if (!_commandFile) {
            sprintf(logBuf, "Can't open command file %s", _commandFileName);
            reportErrSys(logBuf);
            result += StatusFailed;
        } else if (writing) {
	    ftruncate(fileno(_commandFile), 0);
	}
    }

    if (result.IsComplete()) {
        char fileBuf[MAXPATHLEN];
        sprintf(fileBuf, "%s.data", sessionFile);
        _dataFileName = CopyString(fileBuf);
        _dataFile = open(fileBuf, writing ? O_WRONLY | O_CREAT : O_RDONLY,
          0644);
        if (_dataFile < 0) {
            sprintf(logBuf, "Can't open data file %s", _dataFileName);
            reportErrSys(logBuf);
            result += StatusFailed;
        } else if (writing) {
	    ftruncate(_dataFile, 0);
	}
    }

    if (!result.IsComplete()) {
        result += CloseCacheFiles(false);
    }

    return result;
}

//====================================================================
DevStatus
JavaScreenCache::SaveCommand(int argCount, const char * const *args)
{
    DevStatus result(StatusOk);

    // TEMP -- could check fprintf return values here (< 0 if error)
    fprintf(_commandFile, "%s", _cmdPrefix);
    for (int aNum = 0; aNum < argCount; aNum++) {
        fprintf(_commandFile, "{%s} ", args[aNum]);
    }
    fprintf(_commandFile, "\n");

    return result;

}

//====================================================================
DevStatus
JavaScreenCache::SaveData(const char *buf, int byteCount)
{
#if defined (DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCache::SaveData(0x%p, %d)\n", buf, byteCount);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

    DevStatus result(StatusOk);

    int bytesWritten = write(_dataFile, buf, byteCount);
    if (bytesWritten < 0) {
        reportErrSys("write data file");
	result += StatusFailed;
    } else if (bytesWritten != byteCount) {
        // Note: we may want to re-try the write here.  RKW
        // 2000-03-28.
        char errBuf[2048];
        sprintf(errBuf, "Expected to write %d bytes; wrote %d",
          byteCount, bytesWritten);
        reportErrSys(errBuf);
	result += StatusFailed;
    }

    _recordingStatus += result;
    if (result.IsComplete())_dataSize += byteCount;

#if defined (DEBUG_LOG)
    sprintf(logBuf, "  Total data size: %d; last byte: 0x%x", _dataSize,
      buf[byteCount-1]);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

    return result;
}

//====================================================================
DevStatus
JavaScreenCache::StopPlayingBack()
{
#if defined (DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCache::StopPlayingBack()\n");
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

    DevStatus result(StatusOk);

    if (!_playingBack) {
	reportErrNosys("Not currently playing back");
	result += StatusFailed;
    }

    if (result.IsComplete()) {
	// TEMP -- check actual number of commands against number of
	// commands to be saved in command file.

        result += CloseCacheFiles(false);
    }

    _playingBack = false;

    return result;
}

//====================================================================
DevStatus
JavaScreenCache::StopRecording()
{
#if defined (DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCache::StopRecording()\n");
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

    DevStatus result(StatusOk);

    if (!_recording) {
	reportErrNosys("Not currently recording");
	result += StatusFailed;
    }

    if (result.IsComplete()) {
	//TEMP -- save the number of commands as the last line in the
	// file, so we can find out if any lines got removed.

        result += CloseCacheFiles(!_recordingStatus.IsComplete());
    }

    _recording = false;

    return result;
}

//====================================================================
DevStatus
JavaScreenCache::CloseCacheFiles(Boolean deleteFiles)
{
    DevStatus result(StatusOk);

    if (_commandFile) {
        if (fclose(_commandFile) != 0) {
            reportErrSys("Error closing command file");
            result = StatusFailed;
        }
        _commandFile = NULL;
    }

    if (deleteFiles && _commandFileName) {
        if (unlink(_commandFileName) < 0) {
            sprintf(logBuf, "Error unlinking command file %s",
              _commandFileName);
            reportErrSys(logBuf);
        result += StatusWarn;
        }
    }
    FreeString(_commandFileName);
    _commandFileName = NULL;

    if (_dataFile != -1) {
        if (close(_dataFile) < 0) {
            reportErrSys("Error closing data file");
            result = StatusFailed;
        }
        _dataFile = -1;
    }

    if (deleteFiles && _dataFileName) {
        if (unlink(_dataFileName) < 0) {
            sprintf(logBuf, "Error unlinking data file %s", _dataFileName);
            reportErrSys(logBuf);
            result += StatusWarn;
        }
    }
    FreeString(_dataFileName);
    _dataFileName = NULL;

    return result;
}
