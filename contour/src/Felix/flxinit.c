/*Handles allocation and management of FELIX header files*/

#include<stdlib.h> // For declaration of system().  RKW Dec. 22, 1997.
#include<string.h>
#include<malloc.h>
#include "flxinit.h"
#include "flxpos.h"
#include "ioflx.h"
#include "nrutil.h"
#include "io.h"

#define null 0
#define true 1
#define false 0

/*GLOBAL VARIABLES*/

/*Matrix of pointers to felix headers.  16 maximum*/
flxheader **headers;
/*Stack of currently available file numbers*/
int *hstk = null;
/*Current number of open felix data files*/
int headcount;


/*STATIC FUNCTIONS*/


/*Adds fh to headers array and returns current number of open files*/
static int AddHeader(flxheader* fh)
{
	int i,fn;

	/*Initialize storage for headers if it is empty*/
	if (hstk == null)
	{
		headers = (flxheader **)calloc(32,sizeof(flxheader *)) - 1;
		hstk = (int *)calloc(32,sizeof(int)) - 1;
		for(i=1; i<=32; i++)
			hstk[i] = 33 - i;
		headcount = 32;
	}
	/*Dont allow more than 32 file*/	
	if (headcount == 0)
		ErrorMessage("Too many Felix matrixes.  32 is limit",0);
	fn = hstk[headcount--];
	/*Store felix header in list and increment headcount*/
	headers[fn] = fh;
	/*Return file number for this header*/
	return(fn);
}


/*Allocates storage for header of a felix data matrix with dimension = d*/
static flxheader *CreateHeader(int d) 
{
	flxheader *fh;
	int **iptr,sd;
	float **fptr;

	sd = (d == 1) ? 2 : d;
	/*Allocate storage for fh*/
	fh = (flxheader *) alloc_mem(1,sizeof(flxheader));
	if (!fh) ErrorMessage("Error in felix header allocation",0);
	/*Allocate storage for integer arrays within felix header*/
	for(iptr = &fh->pts_per_dim; iptr <= &fh->blkoff; iptr++)
		*iptr = ivector(1,sd);
	/*Allocate storage for real arrays within felix header*/
	for(fptr = &fh->sf; fptr <= &fh->refppm; fptr++)
		*fptr = vector(1,sd);
	/*Return pointer to felix header*/
	return(fh);
}

/*Deallocates storage for header of a felix data matrix with dimension = d*/
static void DestroyHeader(flxheader* fh)
{
	int d,sd;
	int **iptr;
	float **fptr;

	/*d = dimensionality of matrix*/
	d = fh->tot_dims;
	sd = (d == 1) ? 2 : d;
	/*Deallocate storage for integer arrays within felix header*/
	for(iptr = &fh->pts_per_dim; iptr <= &fh->blkoff; iptr++)
		free_ivector(*iptr,1,sd);
	/*Deallocate storage for float arrays within felix header*/
	for(fptr = &fh->sf; fptr <= &fh->refppm; fptr++)
		free_vector(*fptr,1,sd);
	/*Deallocate storage for fh*/
	dealloc_mem(fh,1,sizeof(flxheader));
}

/*Copies information from first block of felix data to header*/
static int LoadHeader(flxheader* fh,int filenum)
{
	float *fptr,*block;
	int *iptr;
	int i,d,blknpts;

	d = fh->tot_dims;
	block = vector(1,felix_blknpts);
	/*Read header block from file*/
	fseek(fh->fp,0,0);
	fread(&block[1],felix_blknpts,sizeof(float),fh->fp);
	/*fptr points to beginning of block*/
	fptr = block;
	/*iptr points to beginning of block*/
	iptr = (int *)block;
	fh->cursor_change = true;
	/*The remaining lines copy information from dblock to fh via the
		fptr and iptr pointer*/
	fh->mxtype = iptr[2];
	fh->tot_blocks = iptr[3] - 1;
	fh->matzod = iptr[4];
	fh->compression = iptr[5];
	for(i=1,blknpts=1,fh->tpts=1; i<=d; i++)
	{
		fh->pts_per_dim[i] = iptr[20 + 1*d + i];	
		fh->pts_per_block[i] = iptr[20 + 4*d + i];
		fh->stride[i] = iptr[20 + 3*d + i];
		fh->blocks_per_dim[i] = iptr[20 + 2*d + i];	
		fh->blkoff[i] = iptr[20 + 5*d + i];
		fh->sf[i] = fptr[20 + 6*d + i];
		fh->sw[i] = fptr[20 + 7*d + i];
		fh->refpt[i] = fptr[20 + 8*d + i];
		fh->refppm[i] = fptr[20 + 9*d + i];
		blknpts *= fh->pts_per_block[i];
		fh->tpts *= fh->pts_per_dim[i];
	}
	fh->blknpts = felix_blknpts;
	free_vector(block,1,felix_blknpts);
	return(d);
}


/*Copies information from header to first block of FELIX data*/
static void LoadDBlock(flxheader* fh,float* block)
{
	float *fptr;
	int *iptr;
	int i,d;
	FILE *fp;

	/*fptr points to beginning of block*/
	fptr = block;
	/*iptr points to beginning of block*/
	iptr = (int *)block;
	fp = fh->fp;
	/*The remaining lines copy information from fh to dblock via the
		fptr and iptr pointer*/
	iptr[1] = d = fh->tot_dims;
	iptr[2] = fh->mxtype;
	iptr[3] = iptr[20 + 1*d] = fh->tot_blocks + 1;
	iptr[4] = fh->matzod;
	iptr[5] = fh->compression;
	for(i=1; i<=d; i++)
	{
		iptr[20 + 1*d + i] = fh->pts_per_dim[i];	
		iptr[20 + 4*d + i] = fh->pts_per_block[i];
		iptr[20 + 3*d + i] = fh->stride[i];
		iptr[20 + 2*d + i] = fh->blocks_per_dim[i];	
		iptr[20 + 5*d + i] = fh->blkoff[i];
		fptr[20 + 6*d + i] = fh->sf[i];
		fptr[20 + 7*d + i] = fh->sw[i];
		fptr[20 + 8*d + i] = fh->refpt[i];
		fptr[20 + 9*d + i] = fh->refppm[i];
	}
	fwrite(&(block[1]),fh->blknpts,sizeof(float),fp);
}

/*Calculates header information based upon the number of dims and the number
of points in each dimension*/
void CalcParms(flxheader *fh,int* pt,int d,int blknpts)
{
	int i,pts;

	fh->tot_dims = d;
	fh->mxtype = 0;
	fh->compression = 0;
	fh->blknpts = blknpts;
	for(i=1,pts=1; i<=d; i++)
	{
		fh->pts_per_dim[i] = 2;
		while(fh->pts_per_dim[i] < pt[i]) fh->pts_per_dim[i] *= 2; 
		fh->pts_per_block[i] = fh->pts_per_dim[i];
		pts *= (fh->pts_per_dim[i]);
	}
	fh->tot_blocks = pts/blknpts;
	if ( (pts % blknpts) != 0 ) fh->tot_blocks++;
	i=d;
	while(pts > blknpts)
	{
		do
		{
			i = (i == d) ? 1 : (i+1);
		} while ( fh->pts_per_block[i] <= 4 ); 
		fh->pts_per_block[i] /= 2;
		pts /= 2;
	}
	fh->stride[1] = fh->blkoff[1] = 1;
	for(i=1; i<=d; i++)
	{
		fh->blocks_per_dim[i] = fh->pts_per_dim[i]/fh->pts_per_block[i];
		if (i != 1)
		{
			fh->stride[i] = fh->blocks_per_dim[i-1]*fh->stride[i-1];
			fh->blkoff[i] = fh->pts_per_block[i-1]*fh->blkoff[i-1];
		}
	}
}

/*GLOBAL FUNCTIONS*/

/*Builds a real FELIX matrix file based upon the number of dims and the
number of pts in each dimension*/
void BuildMatrix(char* filenm,int d,int* pt,int blknpts)
{
	int i;
	flxheader *fh;
	float *block;
	FILE *fp;

	block = vector(1,blknpts);
	fh = CreateHeader(d);
	CalcParms(fh,pt,d,blknpts);
	fp = fopen(filenm,"wb");
	fh->fp = fp;
	LoadDBlock(fh,block);
	for(i=1; i<=blknpts; i++)
		block[i] = 0.0;
	for(i=1; i <= fh->tot_blocks; i++)
		fwrite(&(block[1]),blknpts,sizeof(float),fp);
	fclose(fp);
	free_vector(block,1,blknpts);
	DestroyHeader(fh);
}

/*Destroys matrix filenm*/
void DestroyMatrix(char* filenm)
{
	char command[80];

	strcpy(command,"rm ");	
	strcat(command,filenm);
	system(command);
}


/*Copies matrix fnm1 to fnm2*/
void CopyMatrix(char* fnm1,char* fnm2)
{
	char command[128];

	strcpy(command,"cp ");
	strcat(command,fnm1);
	strcat(command," ");
	strcat(command,fnm2);
	system(command);
}


/*Opens file and extracts and stores header information.  Returns filenumber
to calling routine*/
int OpenMatrix(char* filenm)
{
	char str[4];
	char err_message[str_len];
	FILE *fp;
	flxheader *fh;
	int i,d,filenum,bpd;

	/*Open file for updating (reading or writing)*/ 
	strcpy(str,"r+");
	/*Opens file*/ 
	fp = fopen(filenm,str);
	/*If file cannot be opened, abort*/
	if (!fp)
	{
		strcpy(err_message,"Cannot open file ");
		strcat(err_message,filenm);
		ErrorMessage(err_message,0);
	}
	/*Read dimensionality of file*/
	fread(&d,1,sizeof(int),fp);  
	/*Allocate storage for file header*/
	fh = CreateHeader(d);
	/*Add file to list of felix files*/
	filenum = AddHeader(fh);
	fh->tot_dims = d;
	fh->fp = fp;
	fh->bpos = 0;
	/*Extract information from file into felix header*/
	LoadHeader(fh,filenum);
	for(i=2,fh->blocks_per_brick=fh->blocks_per_dim[1]; i<=d; i++)
		if ( (bpd = fh->blocks_per_dim[i]) > fh->blocks_per_brick)
			fh->blocks_per_brick = bpd;
	BundleDimension(1,filenum);
	/*Return number of file*/
	return(filenum);
}

/*Returns pointer to file header as defined by filenum*/
flxheader *GetHeader(int filenum)
{
	return( headers[filenum] );
}


/*Flush any remaining data out of brick*/
void FlushMatrix(int fn)
{
	flxheader *fh;
	int i;

	fh = GetHeader(fn);
	for(i=1; i<=fh->blocks_per_brick; i++)
		WriteBlock(fn,i);
}

/*Copies open matrix from fn1 to fn2*/
void CopyOpenMatrix(int fn1,int fn2)
{
	int pos;

	InitCursor(fn1);	
	InitCursor(fn2);
	do
	{
		pos = GetAbsPos(fn1);
		PutAbsPos(fn2,pos);
		PutPoint(fn2,GetPoint(fn1));
	}while( !NextCursorPos(fn1) );	
	FlushMatrix(fn2);
}

/*Closes matrix and deallocates storage for file header*/
void CloseMatrix(int filenum)
{
	unsigned blksize;
	flxheader *fh;

	/*Find file header from file list*/
	fh = headers[filenum];
	/*Calculate size of one block of data*/
	blksize = fh->blknpts*sizeof(float);
	/*Flush any remaining data out of brick*/
	FlushMatrix(filenum);
	/*Close file*/
	fclose(fh->fp);
	hstk[++headcount] = filenum;
	/*Deallocate storage for file header*/
	DestroyHeader(fh);
	if (fh->brick != null)
		dealloc_mem(fh->brick+1,fh->blocks_per_brick,blksize);
	free_ivector(fh->block_num,1,fh->blocks_per_brick);
	free_ivector(fh->modify,1,fh->blocks_per_brick);
}

/*Returns the number of vectors in the current bundle dimension for
the felix matrix defined by filenum*/
long CountVectors(int filenum)
{
	flxheader *fh;
	int i,cd,d;
	long vecs;

	fh = headers[filenum];
	d = fh->tot_dims;
	cd = fh->cur_dim;
	for(i=1,vecs=1; i<=d; i++)
		if (i != cd)
			vecs *= fh->pts_per_dim[i];	
	return(vecs);
}

/*Closes all matrixes*/
void CloseAllMatrixes()
{
	int i;

	for(i=1; i<=headcount; i++)
		CloseMatrix(i);
}
