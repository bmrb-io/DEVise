/*Handles conversions between blocks, bricks, vectors, and cartesian
coordinates in a FELIX n-dimensional data matrix*/

#ifndef flxpos_H
#define flxpos_H

/*GLOBAL FUNCTIONS*/


/*Increment cursor position by one unit.  Return true if end of matrix*/
int NextCursorPos(int fn);

/*Decrement cursor position by one unit.  Return true if start of matrix*/
int PrevCursorPos(int fn);

/*Sets cursor to absolute position defined by pt.  If pt position is outside
bounds of dim, then wrap-around occurs and function returns true*/
void PutCursor(int fn,int *pt);

/*Copies current cursor position of felix matrix into pt*/
void GetCursor(int fn, int *pt);

/*Sets cursor to beginning of matrix*/
void InitCursor(int fn);

/*Moves cursor m units along dim from present position*/
void MoveCursor(int fn,int dim,int m);

int LVecToPos(int lvec,int d,int cd,int *offst,int *ppb);


/*Checks to see if the vector specified by pt is within the bounds of
the matrix defined by filenum*/
int WithinLimits(int pt,int llim,int hlim);


/*Checks to see if block is within the current brick*/
int CurrentBrick(long vec,int filenum);

/*Returns block containing vector*/
int VecToBlock(long vn,int filenum);

/*Converts from a Cartesian coordinate pt to its global vector*/
long CartToVector(int *pt,int filenum);


/*Converts from a global vector to its Cartesian cooordinate*/
void VecToCart(long vec,int pt,int filenum);


/*Increments pt[1..d] by incr or aincr[1..d] until hlim or ahlim[1..d] is 
reached.  The starting point is llim or allim[1..d].  Returns false if
hlim is reached*/
int IncrIndex(int d,int *pt,int *allim,int *ahlim,int *aincr,int llim,int hlim,int incr);

/*Obtains the limits llim[1..d] hlim[1..d] which correspond to block
b of Felix matrix fn.  Each dimension is padded by edge endpoints*/
void BlockLimits(int fn,int d,int b,int *llim,int *hlim,int edge);

/*Obtains the absolute position of the current cursor*/
int GetAbsPos(int fn);

/*Sets the cursor to the absolute position defined by pos*/
void PutAbsPos(int fn,int pos);

void PromptBlock(int fn,int *sb);

/*Returns the absolute positions in region[1..pts]*/
int DefineRecRegion(int fn,int d,int pos,int *size,int *region);

/*Returns the absolute positions in region[1..pts]*/
int DefineSqrRegion(int fn,int d,int pos,int size,int *region);

void ReducePoint(int d,int *pt1,int *pt2);

void ExpandPoint(int d,int *pt1,int *pt2,int *p3);

#endif
