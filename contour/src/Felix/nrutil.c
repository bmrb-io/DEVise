/*Definitions file for all vector allocation functions*/

#include <malloc.h> 
#include "io.h"
#include "nrutil.h"

#define null 0

/*Global variables*/
int nb = 0;
int nrec = 16;
int pvnb = 0;
int *vnb = null;

/*Allocates memory by calling alloc_mem and keeps track of requested bytes*/
void *alloc_mem(int nobj,unsigned bytes)

{
	void *obj;

	nb += nobj*bytes;
	obj = calloc(nobj,bytes);
	if (!obj) ErrorMessage("Unable to allocate memory in alloc_mem",0);
	return(obj);
}

/*Deallocates memory keeps track of total number of bytes requested from heap*/
void dealloc_mem(void* obj,int nobj,unsigned bytes)

{
	nb -= nobj*bytes;
	free(obj);
}


/*Returns the number of bytes that have been allocated via alloc_mem*/
int BytesAlloc()

{
	return(nb);
}


/*Record number of bytes allocated by alloc_mem*/
void RecordBytes()

{
	if (pvnb == 0)
	{
		pvnb = 1;
		vnb = (int *)calloc(nrec,sizeof(int)) - 1;
		if(!vnb) ErrorMessage("Memory error in RecordBytes",0);
	}
	if (pvnb == nrec)
	{
		nrec *= 2;
		vnb = (int *)realloc(vnb+1,nrec*sizeof(int)) - 1;
		if(!vnb) ErrorMessage("Memory error in RecordBytes",0);
	}
	vnb[pvnb++] = nb;
}

/*List all byte records*/
void ListBytes()

{
	int i;

	for(i=1; i<pvnb; i++)
		UserMessage("%d bytes\n",vnb[i]);
	nrec = 16;
	pvnb = 0;
	free(vnb+1);	
}

/*Allocates space for a 1 dimensional vector of elements of sizeof(double)*/
double *lvector(int nl,int nh)
{
	double *dv;

	dv = (double *)alloc_mem(nh-nl+1,sizeof(double));
	if (!dv) ErrorMessage("allocation failure in double vector",0);
	return(dv - nl);
}

/*Allocates space for a 1 dimensional vector of elements of sizeof(complex)*/
double *dvector(int nl,int nh)

{
	double *dv;

	dv = (double *)alloc_mem(nh-nl+1,sizeof(double));
	if (!dv) ErrorMessage("allocation failure in double vector",0);
	return( dv - nl );
}


/*Deallocates space for 1 dimensional vector of elements of sizeof(double)*/
void free_dvector(double* dv,int nl,int nh)
{
	dealloc_mem((dv+nl),(nh-nl+1),sizeof(double));
}

/*Allocates space for a 1 dimensional vector of elements of sizeof(complex)*/
complex *cvector(int nl,int nh)
{
	complex *cv;

	cv = (complex *)alloc_mem(nh-nl+1,sizeof(complex));
	if (!cv) ErrorMessage("allocation failure in complex vector",0);
	return( cv - nl );
}

/*Deallocates space for 1 dimensional vector of elements of sizeof(double)*/
void free_cvector(complex* cv,int nl,int nh)
{
	dealloc_mem((cv+nl),(nh-nl+1),sizeof(complex));
}

/*Allocates space for a 1 dimensional vector of elements of sizeof(float)*/
float *vector(int nl,int nh)
{
	float *v;

	v = (float *)alloc_mem(nh-nl+1,sizeof(float));
	if (!v) ErrorMessage("allocation failure in real vector",0);
	return( v - nl );
}

/*Deallocates space for 1 dimensional vector of elements of sizeof(float)*/
void free_vector(float* v,int nl,int nh)
{
	dealloc_mem((v+nl),(nh-nl+1),sizeof(float));
}

/*Allocates space for a 1 dimensional vector of elements of sizeof(int)*/
int *ivector(int nl,int nh)
{
	int *iv;

	iv = (int *)alloc_mem(nh-nl+1,sizeof(int));
	if (!iv) ErrorMessage("allocation failure in integer vector",0);
	return( iv - nl );
}

/*Deallocates space for 1 dimensional vector of elements of sizeof(int)*/
void free_ivector(int* iv,int nl,int nh)
{
	dealloc_mem((iv+nl),(nh-nl+1),sizeof(int));
}

/*Allocates space for a 1 dimensional vector of elements of sizeof(short)*/
short *svector(int nl,int nh)
{
	short *sv;

	sv = (short *)alloc_mem(nh-nl+1,sizeof(short));
	if (!sv) ErrorMessage("allocation failure in short integer vector",0);
	return( sv - nl );
}

/*Deallocates space for 1 dimensional vector of elements of sizeof(short)*/
void free_svector(short* sv,int nl,int nh)
{
	dealloc_mem((sv+nl),(nh-nl+1),sizeof(short));
}

/*Allocates space for a 1 dimensional vector of elements of one-byte*/
char *bvector(int nl,int nh)
{
	char *bv;

	bv = (char *)alloc_mem(nh-nl+1,sizeof(char));
	if (!bv) ErrorMessage("allocation failure in char integer vector",0);
	return( bv - nl );
}

/*Deallocates space for 1 dimensional vector of elements of sizeof(char)*/
void free_bvector(char* bv,int nl,int nh)
{
	dealloc_mem((bv+nl),(nh-nl+1),sizeof(char));
}

