/* RecFile.c */
#include "UnixRecFile.h"

/*******************************************************************/
RecFile *RecFile::CreateFile(char *name, int recSize){
	return UnixRecFile::CreateFile(name, recSize);
}

/*******************************************************************/
RecFile *RecFile::OpenFile(char *name, int recSize, Boolean trunc){
	return UnixRecFile::OpenFile(name, recSize, trunc);
}
