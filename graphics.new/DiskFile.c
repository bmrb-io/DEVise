/*
  $Id$

  $Log$*/

#ifdef USE_AMSDISKFILE
#include <stdio.h>
#include "AmsDiskFile.h"
#else
#include "UnixDiskFile.h"
#endif

/*******************************************************************/
DiskFile *DiskFile::CreateFile(char *name){
#ifdef USE_AMSDISKFILE
	return NULL;
#else
	return UnixDiskFile::CreateFile(name);
#endif
}

/*******************************************************************/
DiskFile *DiskFile::OpenFile(char *name){
#ifdef USE_AMSDISKFILE
	return new AmsDiskFile();
#else
	return UnixDiskFile::OpenFile(name);
#endif
}
