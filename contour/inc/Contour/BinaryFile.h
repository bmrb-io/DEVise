/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
TITLE:   BinaryFile.h
PURPOSE: Functions related to reading/writing binary files and loading
	 data into core memory 
DATE:    10/23/97
DESIGN:  Roger A. Chylla
         email: rchylla@adacgeo.com
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/



/*-------- INCLUDES --------*/
/*-------- END INCLUDES --------*/



/*-------- NEW DATA TYPES --------*/

#ifndef BinaryFile_H
#define BinaryFile_H

/*Creates a file named filenm and write binary data to it*/
extern void vWriteBinaryFile(
	char* pcFilenm,	/*Name of file to create*/
	void *pvData,	/*Pointer to binary data*/
	int iNBytes);	/*Size of binary data*/

/*Reads a file named filenm and loads binary data from it.
  Returns pointer to binary data.  It is the responsibility
  of the calling function to free the void* returned by
  this function.  */
extern void* pvReadBinaryFile(
	char* pcFilenm,	/*Name of file to read*/
	int iNBytes);	/*Number of bytes to read*/

#endif

/*-------- END DATA TYPES --------*/
