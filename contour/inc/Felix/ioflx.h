#ifndef ioflx_H
#define ioflx_H

/*Configures header corresponding to filenum for bundle access in dim. Returns
the total number of vectors in that dimension*/
long BundleDimension(int dim,int filenum);

/*Gets a vector vec from the felix matrix defined by filenum and copies
it to v.  This routine works in bundle mode only*/
void GetBundleVector(float *v,long vec,int filenum);

/*Puts a vector vec from v to the felix matrix defined by filenum.  THis 
routine works in bundle mode only*/
void PutBundleVector(float *v,long vec,int filenum);	

/*Gets a vector vec from the felix matrix defined by filenum and copies
it to v.  This routine works in Cartesian mode only*/
void GetCartVector(float *v,int *pt,int filenum);

/*Puts a vector vec from v to the felix matrix defined by filenum.  THis 
routine works in Cartesian mode only*/
void PutCartVector(float *v,int *pt,int filenum);

/*Writes block number fn to FELIX matrix denoted by fn*/
void WriteBlock(int fn,int bn);

/*Returns a pointer to the block indicated by fn*/
float *GetBlock(int fn,int bn);

/*Gets a point from the felix matrix at the current cursor position*/
float GetPoint(int fn);

/*Puts a point into the felix matrix at the current cursor position*/
void PutPoint(int fn,float x);

/*Transfers data from brick to FELIX data file*/
void UnloadBrick(int fn);

/*Returns whether or not the specified pt is in the defined range of the
matrix.  This routine also updates the current dimensionality of the brick*/
int ValidVector(int *pt,int filenum);

/*Fills all of the points in the matrix with value r*/
void FillMatrix(int fn,float r);

/*Reads data from file fp and stores it in the matrix fn*/
void FileToMatrix(int fn,FILE *fp);

/*Transfers data from present cursor block to fblock.  Edge endpoints
are padded along each dimension*/
void FelixToVMatrix(int fn,int pts,float *fblock,int edge);

/*Transfers data from fblock to present cursor block.  Edge endpoints
are padded along each dimension*/
void VMatrixToFelix(int fn,int pts,float *fblock,int edge);

#endif
