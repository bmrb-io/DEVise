/*Interface file for flxinit.c*/

#include <stdio.h>

#ifndef flxinit_H
#define flxinit_H

#define felix_blknpts 4096
#define str_len 64

typedef struct
{
	/*File pointer to felix file*/
	FILE *fp;
	/*Pointer to data storage area during reading and writing*/
	float *brick;
	/*Pointer to current block*/
	float *block;
	/*Number of data points in one data block*/
	int blknpts;
	/*Current bundle dimension: 0 if not in bundle mode*/
	int cur_dim;
	/*Pointer to current block pos to be filled*/
	int bpos;
	/*Current block containing cursor*/
	int cur_block;
	/*Next block to be read from matrix*/
	int new_block;
	/*Flag indicating change in cursor position*/	
	int cursor_change;
	/*Current offset within block containing cursor*/
	int cur_offset;
	/*Indicates whether current block has been written to*/
	int *cur_modify;
	/*Array of current block numbers within brick*/
	int *block_num;
	/*Array of flags indicating which blocks have been modified*/
	int *modify;
	/*Number of current blocks within brick*/
	int blocks_per_brick;
	/*Lowest vector of current brick*/
	long lbrkvec;
	/*Highest vector of current brick*/
	long hbrkvec;
	/*Total number of dimensions in file*/
	int tot_dims;
	/*Matrix type: real or complex*/
	int mxtype;
	/*Total number of blocks in matrix*/
	int tot_blocks;
	/*Total number of points in matrix*/
	int tpts;
	/*Who knows?*/
	int matzod;
	/*Is matrix compressed?*/
	int compression;
	/*Number of data points along each dimension*/
	int *pts_per_dim;
	/*Current cursor position in Cartesian coordinates*/
	int *pt;
	/*Starting position of region in Cartesian coordinates*/
	int *fregpt;
	/*Last position of region in Cartesian coordinates*/
	int *lregpt;
	/*Number of blocks along each dimension*/
	int *blocks_per_dim;
	/*Number of words between two consecutive data points along each 
		dimension of a block*/
	int *stride;
	/*Number of points along each dimension of a block*/
	int *pts_per_block;
	/*Number of points between two consecutive data points along a
		dimension of a matrix*/
	int *mtrxoff;
	/*Number of points between two consecutive points along a dimension
		of a block according to the current bundle dimension*/
	int *ptoff;
	/*Number of blocks between two consecutive blocks along a dimension
		of a brick according to the current bundle dimension*/
	int *brkoff;
	/*point offset along each dimension of a block*/
	int *blkoff;
	/*Spectrometer frequency for each dimension*/
	float *sf;
	/*Sweep width for each dimension*/
	float *sw;
	/*Reference point for each dimension*/
	float *refpt;
	/*ppm at reference point for each dimension*/
	float *refppm;
} flxheader;


/*Opens file and extracts and stores header information.  Returns filenumber
to calling routine*/
int OpenMatrix(char *filenm);

/*Flushes remaining data out of matrix*/
void FlushMatrix(int fn);

/*Closes matrix and deallocates storage for file header*/
void CloseMatrix(int filenum);

/*Returns pointer to file header corresponding to filenum*/
flxheader *GetHeader(int filenum);

/*Returns the number of vectors in the current bundle dimension for
the felix matrix defined by filenum*/
long CountVectors(int filenum);

/*Copies fnm1 to fnm2*/
void CopyMatrix(char *fnm1,char *fnm2);

/*Copies fn1 to fn2*/
void CopyOpenMatrix(int fn1,int fn2);

/*Closes all currently open matrixes*/
void CloseAllMatrixes();

/*Sets bundle dimension for matrix corresponding to filenum */
long BundleDimension(int dim,int filenum);

#endif
