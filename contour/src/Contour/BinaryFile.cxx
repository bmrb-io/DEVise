/*
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
TITLE:   BinaryFile.cxx
PURPOSE: Functions related to reading/writing binary files and loading
	 data into core memory 
DATE:    10/23/97
DESIGN:  Roger A. Chylla
         email: rchylla@adacgeo.com
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/



/*-------- INCLUDES --------*/

#include "BinaryFile.h"
#include <stdio.h> 
#include <malloc.h> 
#include <stream.h> 

/*-------- END INCLUDES --------*/



/*-------- NEW DATA TYPES --------*/


/*Creates a file named filenm and write binary data to it*/
void vWriteBinaryFile(char* pcFilenm, 
	void *pvData,
	int iNBytes)	
{
	FILE* fp = fopen(pcFilenm,"w");
	if ( fp != 0 )
	{
		fwrite( pvData, iNBytes, 1, fp);
		fclose(fp);
	}
	else
		cerr << "Unable to open file named " << pcFilenm << endl;
}

/*Reads a file named filenm and loads binary data from it.
  Returns pointer to binary data.  It is the responsibility
  of the calling function to free the void* returned by
  this function.  */
void* pvReadBinaryFile(char* pcFilenm,
	int iNBytes)	
{
	void *pvData = 0;

	FILE* fp = fopen(pcFilenm,"r");
	if ( fp != 0 )
	{
		pvData = malloc(iNBytes);
		fread( pvData, iNBytes, 1, fp);
		fclose(fp);
	}
	else
		cerr << "Unable to open file named " << pcFilenm << endl;

	return pvData;
}

/*-------- END DATA TYPES --------*/
