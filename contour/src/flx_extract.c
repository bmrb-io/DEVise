/*
Extracts a submatrix from a FELIX matrix and writes it
	to a binary file:

	INPUT (from standard input):
		1) flx_filenm<RET> (String)
		2) Dim1<TAB>Llim1<TAB>Size1<TAB>Increment1<RET>  (Integers)
		   Dim2<TAB>Llim2<TAB>Size2<TAB>Increment2<RET>  (Integers)

   		where:
			flx_filenm: name of the FELIX matrix 
			Dim1: Dimension of matrix that will be plotted on x axis
			Dim2: Dimension of matrix that will be plotted on y axis
			Llim1: Lower limit of submatrix along x axis
			Llim2: Lower limit of submatrix along y axis
			Size1: Size of submatrix along x axis
			Size2: Size of submatrix along y axis
			Increment1: Spacing along x axis
			Increment2: Spacing along y axis

	OUTPUT:
		1) Writes a binary file (bin_filenm) of floating point numbers 
		   containing the data in column,row order   
		   e.g. 1024 X 512 : Each point along x dimension would be 
			spaced 1 apart and each point along y dimension would 
			be spaced 512*sizeof(float) apart
		2) Outputs the following info to standard output:
		   a) XSize<TAB>YSize<RET>  (Integers)
		   b) bin_filenm<RET> 	    (String)
		   where:
			XSize: Size of submatrix along x axis (Size1/Increment1)
			YSize: Size of submatrix along y axis (Size2/Increment2)
			bin_filenm : Name of binary output file
*/

#include <stream.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "felix.h"
#include "nrutil.h"

void main(int argc,char *argv[])
{
	char filenm[80],tmp_filenm[80];

	/*Get name of matrix file*/
	cin >> filenm;
	
	int D1,llim1,size1,hlim1,incr1;
	int D2,llim2,size2,hlim2,incr2;

	/*Get dimensions of matrix*/
	cin >> D1 >> llim1 >> size1 >> incr1;
	cin >> D2 >> llim2 >> size2 >> incr2;

	/*Calculate upper limits along each dimension*/
	hlim1 = ( llim1 + size1 - 1);
	hlim2 = ( llim2 + size2 - 1);

	/*Open the matrix file*/
	int fn = OpenMatrix(filenm);

	/*Generate a file name and create a file to hold binary data*/
	strcpy(tmp_filenm,filenm);
	strcat(tmp_filenm,".dat");

	FILE *fp = fopen(tmp_filenm,"w");
	if ( fp == 0 )
	{
		cerr << "Unable to open file " << tmp_filenm << endl;
		exit(1);
	}

	/*Get header info from matrix*/
	flxheader* fh = GetHeader(fn);

	/*Check dimensionality*/
	int D = fh->tot_dims;
	if ( (D1 > D) || (D2 > D) )
	{
		cerr << "Invalid dimension specified" << endl;
		exit(1);
	}

	/*Check limits*/
	int max_cols = fh->pts_per_dim[D1];
	int max_rows = fh->pts_per_dim[D2];

	/*Make sure that llim and hlim are within limits of matrix*/
	llim1 = (llim1 < 1) ? 1 : llim1;
	llim2 = (llim2 < 1) ? 1 : llim2;
	hlim1 = (hlim1 > max_cols) ? max_cols : hlim1;
	hlim2 = (hlim2 > max_rows) ? max_rows : hlim2;

	/*Get number of rows and columns of binary data file*/
	int ncols = (hlim1 - llim1 + 1)/incr1;
	int nrows = (hlim2 - llim2 + 1)/incr2;

	/*Output dimensions and name of binary data file*/
	cout << ncols << '\t' << nrows << endl;
	cout << tmp_filenm << endl;

	/*Initialize the matrix cursor*/
	int* pt = ivector(1,D);
	InitCursor(fn);
	GetCursor(fn,pt);

	pt[D1] = llim1;
	pt[D2] = llim2;

	/*Loop through nrows,ncols writing the data to the
	  binary data file*/
	float fArray[1];
	for(int j=0; j<nrows; j++)
	{
		pt[D2] = (j*incr2 + llim2);
		for(int i=0; i<ncols; i++)
		{
			pt[D1] = (i*incr1 + llim1);
			PutCursor(fn,pt);
			fArray[0] = GetPoint(fn);
			fwrite(fArray,1,sizeof(float),fp);
		}
	}

	fclose(fp);
	free(tmp_filenm);
	free_ivector(pt,1,D);
}
