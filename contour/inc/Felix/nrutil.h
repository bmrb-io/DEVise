#ifndef nrutil_H
#define nrutil_H


#ifndef COMPLEX_DEF
typedef struct
{
	float r;
	float i;
} complex;			/*Defines a complex number*/
#endif

#define COMPLEX_DEF

/*Definitions file for all vector allocation functions*/

void RecordBytes();

void ListBytes();

/*Allocate storage for nobj of size bytes*/
void *alloc_mem(int nobj,unsigned bytes);

/*Dellocate storage for nobj of size bytes*/
void dealloc_mem(void *obj,int nobj,unsigned bytes);

/*Returns number of bytes that have been allocated via alloc_mem and
not deallocated via dealloc_mem*/
int BytesAlloc();

/*Allocate storage for a nd long array of real numbers*/
float *vector(int nl,int nh);

/*Deallocate storage for vector*/
void free_vector(float *v,int nl,int nh);

/*Allocate storage for a nd long array of real numbers*/
complex *cvector(int nl,int nh);

/*Deallocate storage for vector*/
void free_cvector(complex *cv,int nl,int nh);

/*Allocate storage for a nd long array of integers*/
int *ivector(int nl, int nh);

/*Deallocate storage for ivector*/
void free_ivector(int *v,int nl,int nh);

/*Allocate storage for a nd long array of integers*/
short *svector(int nl, int nh);

/*Deallocate storage for ivector*/
void free_svector(short *sv,int nl,int nh);

/*Allocate storage for a nd long array of chars*/
char *bvector(int nl, int nh);

/*Deallocate storage for chars*/
void free_bvector(char *bv,int nl,int nh);

/*Allocate storage for a nd long array of double precision numbers*/
double *dvector(int nl, int nh);

/*Deallocate storage for dvector*/
void free_dvector(double *dv,int nl,int nh);

#endif
