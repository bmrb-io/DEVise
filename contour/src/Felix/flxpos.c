/*Handles conversions between blocks, bricks, vectors, and cartesian
coordinates in a FELIX n-dimensional data matrix*/

#include "flxinit.h"
#include "nrutil.h"
#include "io.h"

#define true 1
#define false 0



/*Converts the current cursor position specified in Cartesian coordinates
to its position in block,offset coordinates*/
static void PtToBlockOff(flxheader *fh,int* block,int* offset)
{
	int i,l,d,pt,iblk,ioff,ppb;

	d = fh->tot_dims;
	*block = 1; *offset = 1;
	for(i=1; i<=d; i++)
	{
		l = fh->pts_per_dim[i];
		pt = fh->pt[i];
		while( pt < 1 ) pt += l;
		while( pt > l ) pt -= l;
		pt--;
		ppb = fh->pts_per_block[i];
		iblk = (pt / ppb)*fh->stride[i];
		*block +=  iblk;
		pt = (pt % ppb);
		ioff = pt*fh->blkoff[i];
		*offset += ioff;
	}
}

/*Converts from current block,offset coordinates to Cartesian coordinates*/
static void BlockOffToPt(flxheader* fh)
{
	int i,d,block,offset,bpt,ofp;

	d = fh->tot_dims;
	block = fh->new_block; offset = fh->cur_offset;
	for(i=d; i>=1; i--)
	{
		bpt = ((block-1)/fh->stride[i]);
		ofp = ((offset-1)/fh->blkoff[i]);
		block -= bpt*fh->stride[i];
		offset -= ofp*fh->blkoff[i];
		fh->pt[i] = bpt*fh->pts_per_block[i] + ofp + 1;
	}
}

/*If a cursor change is necessary, this routine calls BlockOffToPt*/
static void UpdateCursor(flxheader *fh)
{
	if (fh->cursor_change)
	{
		fh->cursor_change = false;
		BlockOffToPt(fh);
	}
}

/*GLOBAL FUNCTIONS*/

/*Copies current cursor position into pt*/
void GetCursor(int fn,int* pt)
{
	flxheader *fh;
	int i,d;

	fh = GetHeader(fn);
	UpdateCursor(fh);
	d = fh->tot_dims;
	for(i=1; i<=d; i++)
		pt[i] = fh->pt[i];
}

/*Sets cursor to absolute position defined by pt.  If pt position is outside
bounds of dim, then wrap-around occurs*/
void PutCursor(int fn,int* pt)
{
	int i,d,block,offset;
	flxheader *fh;

	fh = GetHeader(fn);
	d = fh->tot_dims;
	for(i=1; i<=d; i++)
		fh->pt[i] = pt[i];
	PtToBlockOff(fh,&block,&offset);
	fh->new_block = block;
	fh->cur_offset = offset;	
}


/*Increment cursor position by one unit.  Return true if end of matrix*/
int NextCursorPos(int fn)
{
	flxheader *fh;
	int pos,eom,blknpts;

	fh = GetHeader(fn);
	blknpts = fh->blknpts;
	pos = ((fh->new_block-1)*blknpts) + fh->cur_offset;
	if ( pos >= fh->tpts )
		eom = true;
	else 
	{
		eom = false;
		fh->new_block = (pos/blknpts) + 1;
		fh->cur_offset = (pos % blknpts) + 1;	
		fh->cursor_change = true;
	}
	return(eom);
}

/*Decrement cursor position by one unit.  Return true if start of matrix*/
int PrevCursorPos(int fn)
{
	flxheader *fh;
	int pos,som,lp,cur_block,blknpts;

	fh = GetHeader(fn);
	blknpts = fh->blknpts;
	pos = ((fh->new_block-1)*blknpts) + fh->cur_offset - 2;
	if (pos <= 0) 
	{
		som = true;
		if (pos < 0) return(som);
	}
	else som = false;
	cur_block = (pos/blknpts) + 1;
	fh->new_block = cur_block;
	fh->cur_offset = (pos % blknpts) + 1;	
	fh->cursor_change = true;
	return(som);
}

/*Sets cursor to beginning of matrix*/
void InitCursor(int fn)
{
	flxheader *fh;
	int i,d;

	fh = GetHeader(fn);
	d = fh->tot_dims;
	for(i=1; i<=d; i++)
		fh->pt[i] = 1;
	fh->cur_offset = 1;
	fh->new_block = 1;
}

/*Moves cursor m units along dim from present position*/
void MoveCursor(int fn,int dim,int m)
{
	flxheader *fh;
	int i,d,*pt;

	fh = GetHeader(fn);
	d = fh->tot_dims;
	pt = ivector(1,d);
	for(i=1; i<=d; i++)
		pt[i] = (i == dim) ? (fh->pt[i]+m) : fh->pt[i];
	PutCursor(fn,pt);
	free_ivector(pt,1,d);
}

int LVecToPos(int lvec,int d,int cd,int* offst,int* ppb)
{
	int i,pos,m,rem;

	for(i=1,pos=1; i<=d; i++)
		if (i != cd)
		{
			m = (lvec-1)/ppb[i];
			rem = (lvec-1) % ppb[i];
			pos += rem*offst[i];	
			lvec = m + 1;  
		}
	return(pos);
}


/*Checks to see if the vector specified by pt is within the bounds of
the matrix defined by filenum*/
int WithinLimits(int pt,int llim,int hlim)
{
	return( (pt >= llim) && (pt <= hlim) );
}


/*Checks to see if block is within the current brick*/
int CurrentBrick(long vec,int filenum)
{
	flxheader *fh;

	fh = GetHeader(filenum);
	return( (vec >= fh->lbrkvec) && (vec <= fh->hbrkvec) );
}

/*Returns block containing vector*/
int VecToBlock(long vn,int filenum)
{
	flxheader *fh;
	int vecs_per_block,i,d,cd,nb,block;

	fh = GetHeader(filenum);
	d = fh->tot_dims;
	cd = fh->cur_dim;
	vecs_per_block = fh->blknpts/fh->pts_per_block[cd];
	nb = (vn-1)/vecs_per_block;
	for(i=1,block=1; (i<=d)&&(nb>0); i++)
		if (i != cd)
		{
			block += (nb % fh->blocks_per_dim[i])*fh->stride[i];
			nb /= fh->blocks_per_dim[i];
		}
	return(block);
}


/*Converts from a Cartesian coordinate pt to its global vector*/
long CartToVector(int *pt,int filenum)
{
	long vec;    
	int i,d,cd,vpb,ppb,m,rem;
	flxheader *fh;

	fh = GetHeader(filenum);
	cd = fh->cur_dim;
	d = fh->tot_dims;
	vpb = fh->blknpts/fh->pts_per_block[cd];
	for(i=1,vec=1; i<=d; i++)
		if (i != cd)
		{
			ppb = fh->pts_per_block[i];
			m = (pt[i]-1) / ppb;
			rem = (pt[i]-1) % ppb;   
			vec += (long)(rem*fh->ptoff[i]);	
			vec += (long)(m*vpb*fh->brkoff[i]);
		}
	return(vec);		
}

/*Converts from a global vector to its Cartesian cooordinate*/
void VecToCart(long vec,int* pt,int filenum)
{
	int i,d,cd,vpb,m,rem,lv,nb;
	flxheader *fh;

	fh = GetHeader(filenum);
	cd = fh->cur_dim;
	d = fh->tot_dims;
	vpb = fh->blknpts/fh->pts_per_block[cd];
	nb = (int)((vec-1) / (long)vpb);
	lv = (int)((vec-1) % (long)vpb);
	for(i=d; i>=1; i--)
		if (i != cd)
		{
			m = nb / fh->brkoff[i];
			rem = nb % fh->brkoff[i];
			pt[i] = m*fh->pts_per_block[i] + 1;
			nb = rem;
			m = lv/fh->ptoff[i];
			rem = lv % fh->ptoff[i];
			pt[i] += m;
			lv = rem;
		}
	pt[cd] = 0;
}


/*Increments pt[1..d] by incr or aincr[1..d] until hlim or ahlim[1..d] is 
reached.  The starting point is llim or allim[1..d].  Returns false if
hlim is reached*/
int IncrIndex(int d,int* pt,int* allim,int* ahlim,int* aincr,int llim,int hlim,int incr)
{
	int i;

	llim = (allim == 0) ? llim : allim[1];
	hlim = (ahlim == 0) ? hlim : ahlim[1];
	incr = (aincr == 0) ? incr : aincr[1];
	pt[i=1] += incr;
	while( (pt[i] > hlim) && (i <= d) )
	{
		pt[i++] = llim;
		if (i <= d)
		{
			llim = (allim == 0) ? llim : allim[i];
			hlim = (ahlim == 0) ? hlim : ahlim[i];
			incr = (aincr == 0) ? incr : aincr[i];
			pt[i] += incr;
		}
	}
	return( i <= d );
}

/*Calculates llim[1..d] and hlim[1..d] which correspond to block b
of felix matrix fh.  The limits include edge endpoints in each dimension.
This routine modifies the current cursor position!*/
void BlockLimits(int fn,int d,int b,int* llim,int* hlim,int edge)
{
	int i;
	flxheader *fh;

	fh = GetHeader(fn);
	fh->new_block = b;
	fh->cur_offset = 1;
	BlockOffToPt(fh);
	for(i=1; i<=d; i++)
	{
		llim[i] = fh->pt[i] - edge;
		hlim[i] = fh->pt[i] + fh->pts_per_block[i] + edge - 1;
	}
}

/*Returns absolute position of current cursor position*/
int GetAbsPos(int fn)
{
	flxheader *fh;

	fh = GetHeader(fn);
	return( (fh->new_block-1)*fh->blknpts + fh->cur_offset - 1 );
}

/*Sets cursor to absolute position defined by pos*/
void PutAbsPos(int fn,int pos)
{
	flxheader *fh;

	fh = GetHeader(fn);
	fh->new_block = (pos / fh->blknpts) + 1;
	fh->cur_offset = (pos % fh->blknpts) + 1;
	BlockOffToPt(fh); 
}


int DefineRecRegion(int fn,int d,int pos,int* size,int*region)
{
	int i,j,p,pts,*pt1,*pt2,*pt3,*llim,*hlim;

	pt1 = ivector(1,d);
	pt2 = ivector(1,d);
	pt3 = ivector(1,d);
	llim = ivector(1,d);
	hlim = ivector(1,d);
	PutAbsPos(fn,pos);
	GetCursor(fn,pt1);
	for(i=1; i<=d; i++)
	{
		pt2[i] = llim[i] = -size[i]; 
		hlim[i] = size[i];
	}
	pts = 0;
	do
	{
		for(i=1; i<=d; i++)
			pt3[i] = pt1[i] + pt2[i];
		PutCursor(fn,pt3);
		p = GetAbsPos(fn);
		region[++pts] = p;
		j=1; pt2[j]++;
		while( (pt2[j] > hlim[j]) && (j <= d) )
		{
			pt2[j++] = llim[j];
			if (j <= d) pt2[j]++;
		}	
	} while (j <= d);
	PutAbsPos(fn,pos);
	free_ivector(pt1,1,d);
	free_ivector(pt2,1,d);
	free_ivector(pt3,1,d);
	free_ivector(llim,1,d);
	free_ivector(hlim,1,d);
	return(pts);
}

int DefineSqrRegion(int fn,int d,int pos,int size,int* region)
{
	int i,pts,*hlim;

	hlim = ivector(1,d);
	for(i=1; i<=d; i++)
		hlim[i] = size;
	pts = DefineRecRegion(fn,d,pos,hlim,region);
	free_ivector(hlim,1,d);
	return(pts);
}

void ReducePoint(int d,int* pt1,int* pt2)
{
	int i,j;

	for(i=1,j=0; i<=d; i++)
		if (pt1[i] != 0) pt2[++j] = pt1[i];
}

void ExpandPoint(int d,int* pt1,int* pt2,int* pt3)
{
	int i,j;

	for(i=1,j=0; i<=d; i++)
	{
		if (pt1[i] == 0)
			pt3[i] = pt2[++j];
		else 
			pt3[i] = pt1[i];
	}
}
