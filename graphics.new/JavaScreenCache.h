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
  Declaration of JavaScreenCache class (handles caching of commands and
  data for faster opening of sessions in the JavaScreen).
 */

/*
  $Id$

  $Log$
 */

#ifndef _JavaScreenCache_h_
#define _JavaScreenCache_h_

#include <stdio.h>

#include "DeviseTypes.h"

class JavaScreenCmd;

class JavaScreenCache
{
public:
    JavaScreenCache();
    ~JavaScreenCache();

    DevStatus StartPlayingBack(JavaScreenCmd *jsc, const char *sessionFile,
      int protocolMajorVersion);
    DevStatus StartRecording(const char *sessionFile,
      int protocolMajorVersion, int protocolMinorVersion);
    DevStatus StopPlayingBack();
    DevStatus StopRecording();

    Boolean IsRecording() { return _recording; }
    Boolean IsPlayingBack() { return _playingBack; }

    DevStatus GetRecordingStatus() { return _recordingStatus; }
    void AddRecordingStatus(DevStatus status) { _recordingStatus += status; }

    DevStatus SaveCommand(int argCount, const char * const *args);
    DevStatus SaveData(const char *buf, int byteCount);

private:
    DevStatus SendCmd(JavaScreenCmd *jsc, const char* cmd);
    DevStatus OpenCacheFiles(const char *sessionFile, Boolean
      writing);
    DevStatus CloseCacheFiles(Boolean deleteFiles);

private:
    Boolean     _recording;// true iff recording commands and data
    DevStatus   _recordingStatus;
    char *      _commandFileName;
    FILE *      _commandFile;
    char *      _dataFileName;
    int         _dataFile;
    int		_dataSize; // for debugging

    Boolean     _playingBack; // true iff playing back commands and data
};

#endif // _JavaScreenCache_h_
