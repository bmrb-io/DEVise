/*Handles conversions between blocks, bricks, and vectors in a FELIX
n-dimensional data matrix*/

#include "io.h"
#include "nrutil.h"
#include "flxinit.h"
#include "flxpos.h"
#include <memory.h>
#include <string.h>
#include <malloc.h>

#define true 1
#define false 0
#define null 0

/*Forward declaration of global function*/
long BundleDimension(int dim,int filenum);
void WriteBlock(int fn,int b);

/*STATIC FUNCTIONS*/

/*Searches brick for current block.  If none found, it returns false.  If
found, it returns the index of fh->block_num which contains the block*/
static int BlockIndex(flxheader* fh,int block)
{
	int i;

	for(i=1; i<=fh->blocks_per_brick; i++)
		if ( fh->block_num[i] == block ) 
			return(i);
	return(false);
}


/*Reads block b from felix data file (fp) into dblock*/
static void ReadBlock(int fn,int idx)
{
	int b,blksize;
	FILE *fp;
	float *block;
	flxheader *fh;

	fh = GetHeader(fn);
	blksize = fh->blknpts*sizeof(float);
	if (fh->block == null)
		BundleDimension(1,fn);
	/*Get pointer to file*/
	fp = fh->fp;
	b = fh->block_num[idx];
	/*Set data pointer to beginning of block b*/
	fseek(fp,b*blksize,0);
	block = fh->brick + (idx-1)*fh->blknpts;
	/*Read data from file into dblock*/
	fread(&block[1],fh->blknpts,sizeof(float),fp);
	fh->block = block;
	fh->modify[idx] = false;
	fh->cur_modify = &(fh->modify[idx]);
}

static int GetBlock(int fn,int bn,int* flag)
{
	int idx;
	flxheader *fh;

	fh = GetHeader(fn);
	/*If bn is already contained in the brick...*/
	if ( (idx = BlockIndex(fh,bn)) != 0 )
		*flag = false;
	else
	/*Return position for block to be read into brick*/
	{
		*flag = true;
		/*Find position in brick of current block*/
		idx = fh->bpos;
		/*Advance idx to next available block in the brick array.
		The brick array is circular*/
		idx++;
		if (idx > fh->blocks_per_brick) idx = 1;
		fh->bpos = idx;
		/*Write block to file*/
		WriteBlock(fn,idx);
	}
	fh->block_num[idx] = bn;
	return(idx);
}

/*Loads a block of data into the memory pointed to by fh->block.  If the
desired block is already contained within the brick, it simply sets fh->block
to point to that location within the brick*/
static void LoadBlock(flxheader* fh,int fn,int b,int* idx)
{
	int flag;

	if (fh->cur_block == null)
		InitCursor(fn);
	*idx = GetBlock(fn,b,&flag);
	fh->block = fh->brick + (*idx - 1)*fh->blknpts;
	if (flag)
		ReadBlock(fn,*idx);
}

/*Performs transfer of data between v and the brick corresponding to the
vector vec contained in the felix matrix defined by filenum*/
static void BrickVecTransfer(float* v,int lvec,int filenum,int mwrite,int sblock)
{
	flxheader *fh;
	float *block;
	int cd,d,bpd,pbb,bs,i,j,pi,ptr,idx;

	fh = GetHeader(filenum);
	d = fh->tot_dims;
	cd = fh->cur_dim;
	bs = fh->stride[cd];         
	bpd = fh->blocks_per_dim[cd];
	pbb = fh->pts_per_block[cd];
	pi = fh->blkoff[cd];
	v = &v[1];
	for(i=1; i<=bpd; i++,sblock += bs)
	{
		LoadBlock(fh,filenum,sblock,&idx);
		fh->modify[idx] = mwrite;
		block = fh->block;
		ptr = LVecToPos(lvec,d,cd,fh->blkoff,fh->pts_per_block);
		for(j=1; j<=pbb; j++,ptr += pi)
		{
			if(mwrite)
				block[ptr] = *v++;
			else
				*v++ = block[ptr];
		}
	}
}

/*Obtains configuration for a new brick which contains vec*/
static void NewBrick(long vec,int filenum)
{
	flxheader *fh;
	int cd,vecs_per_brick,sblock;

	fh = GetHeader(filenum);
	cd = fh->cur_dim;
	if (cd == 0)
		ErrorMessage("Dimension not defined for matrix",1);	
	vecs_per_brick = fh->blknpts/fh->pts_per_block[cd];
	fh->lbrkvec = ((vec-1)/vecs_per_brick)*vecs_per_brick + 1;
	fh->hbrkvec = fh->lbrkvec + vecs_per_brick - 1;
	sblock = VecToBlock(fh->lbrkvec,filenum);
	fh->new_block = sblock;
}

/*GLOBAL FUNCTIONS*/



/*Writes block b from dblock to felix data file (fp) */
void WriteBlock(int fn,int idx)
{
	int b,blksize;
	FILE *fp;
	float *block;
	flxheader *fh;

	fh = GetHeader(fn);
	/*
	if (!fh->modify[idx]) return;
	*/
	b = fh->block_num[idx]; 
	if (b == 0) return;
	blksize = fh->blknpts*sizeof(float);
	block = fh->brick + (idx-1)*fh->blknpts;
	if (fh->block == null)
		BundleDimension(1,fn);
	/*Get pointer to data file*/
	fp = fh->fp;               
	/*Set data pointer to beginning of block b*/
	fseek(fp,b*blksize,0);
	/*Write data from block into file*/
	fwrite(&block[1],fh->blknpts,sizeof(float),fp);
	fflush(fp);
	fh->block = block;
}

/*Returns whether or not the specified pt is in the defined range of the
matrix.  This routine also updates the current dimensionality of the brick*/
int ValidVector(int* pt,int filenum)
{
	flxheader *fh;
	int i,d,cd;

	fh = GetHeader(filenum);
	cd = fh->cur_dim;
	d = fh->tot_dims;
	for(i=1; i<=d; i++)
		if (pt[i] == 0) break;
	if (i != cd)
	{
		BundleDimension(i,filenum);
		fh->cur_dim = i;
	}
	return(true);
}

/*Configures header corresponding to filenum for bundle access in dim*/
long BundleDimension(int dim,int filenum)
{
	flxheader *fh;
	int d,i,j,k,m,blksize;

	fh = GetHeader(filenum);
	blksize = fh->blknpts*sizeof(float);
	fh->cur_dim = dim;
	fh->cur_block = null;
	fh->lbrkvec = fh->hbrkvec = null;
	d = fh->tot_dims;
	fh->block = fh->brick;
	j = (dim == 1) ? 2 : 1;
	k = (dim == 1) ? 1 : 2;
	if (fh->brick == null)
		fh->brick =(float *)alloc_mem(fh->blocks_per_brick,blksize) - 1;
	if (fh->block_num == null)
		fh->block_num = ivector(1,fh->blocks_per_brick);
	if (fh->modify == null)
		fh->modify = ivector(1,fh->blocks_per_brick);
	for(i=k,fh->ptoff[j]=1,fh->brkoff[j]=1,fh->mtrxoff[j]=1,m=j; i<=d; i++) 
		if ( (i != dim) && (i !=j) )
		{
			fh->mtrxoff[i] = fh->mtrxoff[m]*fh->pts_per_dim[m];
			fh->ptoff[i] = fh->ptoff[m]*fh->pts_per_block[m];
			fh->brkoff[i] = fh->brkoff[m]*fh->blocks_per_dim[m];
			m = i;
		}
	return( CountVectors(filenum) );
}

/*Gets a point from the felix matrix at the current cursor position*/
float GetPoint(int fn)
{
	float x;
	flxheader *fh;
	int idx;

	fh = GetHeader(fn);
	if ( fh->new_block != fh->cur_block )
	{
		LoadBlock(fh,fn,fh->new_block,&idx);
		fh->cur_block = fh->new_block;
	}
	x = fh->block[fh->cur_offset];
	return(x);
}

/*Puts a point into the felix matrix at the current cursor position*/
void PutPoint(int fn,float x)
{
	flxheader *fh;
	int cur_block,idx;

	fh = GetHeader(fn);
	cur_block = fh->cur_block;
	if ( fh->new_block != cur_block )
	{
		LoadBlock(fh,fn,fh->new_block,&idx);
		fh->cur_block = fh->new_block;
	}
	/*
	fh->modify[cur_block] = true;
	*/
	fh->block[fh->cur_offset] = x;
}

/*Gets a vector vec from the felix matrix defined by filenum and copies
it to v.  This routine works in bundle mode only*/
void GetBundleVector(float* v,long vec,int filenum)
{
	flxheader *fh;
	int lv;

	fh = GetHeader(filenum);
	if(!CurrentBrick(vec,filenum)) 
	{
		NewBrick(vec,filenum);
		fh->cur_block = fh->new_block;
	}
	lv = (int) (vec - fh->lbrkvec + 1);
	BrickVecTransfer(v,lv,filenum,false,fh->cur_block);
}

/*Puts a vector vec from v to the felix matrix defined by filenum.  THis 
routine works in bundle mode only*/
void PutBundleVector(float* v,long vec,int filenum)	
{
	flxheader *fh;
	int lv;

	fh = GetHeader(filenum);
	if(!CurrentBrick(vec,filenum)) 
	{
		NewBrick(vec,filenum);
		fh->cur_block = fh->new_block;
	}
	lv = (int) (vec - fh->lbrkvec + 1);
	BrickVecTransfer(v,lv,filenum,true,fh->cur_block);
}

/*Gets a vector vec from the felix matrix defined by filenum and copies
it to v.  This routine works in Cartesian mode only*/
void GetCartVector(float* v,int* pt,int filenum)
{
	flxheader *fh;
	int lv;
	long vec;

	fh = GetHeader(filenum);
	if(!ValidVector(pt,filenum))
		ErrorMessage("Specified cartesian point is outside of range",1);
	vec = CartToVector(pt,filenum);
	if(!CurrentBrick(vec,filenum)) 
	{
		NewBrick(vec,filenum);
		fh->cur_block = fh->new_block;
	}
	lv = (int) (vec - fh->lbrkvec + 1);
	BrickVecTransfer(v,lv,filenum,false,fh->cur_block);
}

/*Puts a vector vec from v to the felix matrix defined by filenum.  THis 
routine works in Cartesian mode only*/
void PutCartVector(float* v,int* pt,int filenum)	
{
	flxheader *fh;
	int i,sblock,offset,lv;
	long vec;

	fh = GetHeader(filenum);
	if(!ValidVector(pt,filenum))
		ErrorMessage("Specified cartesian point is outside of range",1);
	vec = CartToVector(pt,filenum);
	if(!CurrentBrick(vec,filenum)) 
	{
		NewBrick(vec,filenum);
		fh->cur_block = fh->new_block;
	}
	lv = (int) (vec - fh->lbrkvec + 1);
	BrickVecTransfer(v,lv,filenum,true,fh->cur_block);
}

/*Loads fblock [0..pts-1] with the current block plus edge points in each 
dimension*/
void FelixToVMatrix(int fn,int pts,float* fblock,int edge)
{
	int i,j,d,flag,hlim[9],llim[9],npt[9],stocur[9];
	flxheader *fh;

	fh = GetHeader(fn);
	d = fh->tot_dims;
	GetCursor(fn,stocur);
	BlockLimits(fn,fh->tot_dims,fh->new_block,llim,hlim,edge);
	for(i=1; i<=d; i++)
		npt[i] = llim[i];
	for(i=0; i < pts; i++)
	{
		PutCursor(fn,npt);	
		for(j=1,flag=false; (j<=d) && (!flag); j++)
		{
			if (npt[j] < 1) flag = true;
			if (npt[j] > fh->pts_per_dim[j]) flag = true;
		}
		fblock[i] = (flag) ? 0.0 : GetPoint(fn); 
		npt[j=1]++;
		while( npt[j] > hlim[j] )
		{
			npt[j] = llim[j++];
			npt[j]++;
		}
	}
	PutCursor(fn,stocur);
}

/*Fills all of the points in the matrix with value r*/
void FillMatrix(int fn,float r)
{
	int pos;

	pos = GetAbsPos(fn);
	InitCursor(fn);
	do
	{
		PutPoint(fn,r);
	}while ( !NextCursorPos(fn) );
	PutAbsPos(fn,pos);
}

void FileToMatrix(int fn,FILE* fp)
{
	int pos,p[1];
	float r[1];

	pos = GetAbsPos(fn);
	fseek(fp,0,0);
	while( !feof(fp) )
	{
		fread(p,1,sizeof(int),fp);
		fread(r,1,sizeof(float),fp);
		PutAbsPos(fn,p[0]);
		PutPoint(fn,r[0]);
	}
	PutAbsPos(fn,pos);
}

/*Loads the current block in the matrix with fblock [0..pts-1] plus edge
points in each dimension*/
void VMatrixToFelix(int fn,int pts,float* fblock,int edge)
{
	int i,j,d,hlim[9],llim[9],npt[9],stocur[9];
	flxheader *fh;

	fh = GetHeader(fn);
	d = fh->tot_dims;
	GetCursor(fn,stocur);
	BlockLimits(fn,fh->tot_dims,fh->new_block,llim,hlim,edge);
	for(i=1; i<=d; i++)
		npt[i] = llim[i];
	memset(fblock,pts*sizeof(float),0);
	for(i=0; i < pts; i++)
	{
		PutCursor(fn,npt);
		PutPoint(fn,fblock[i]);	
		npt[j=1]++;
		while( npt[j] > hlim[j] )
		{
			npt[j] = llim[j++];
			npt[j]++;
		}
	}
	PutCursor(fn,stocur);
}

