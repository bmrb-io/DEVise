#include "RDOPT.h"
#include "RDOPT_Internal.h"



static int HistIncrCount(RD_Job *job, int n, long val, 
			  boolean waspositive)
{
  long absval, newsize, *temp;
  RD_Hist *H = job->Histogram; 
  if (waspositive) {
    if (H[n].PlusSize <= val) {
      newsize = H[n].PlusSize << 1;
      while (newsize <= val) newsize <<= 1;
      if ((temp = (long *) calloc(1,newsize*sizeof(long))) == NULL) {
	RD_FatalError("HistIncrCount: out of memory");
      }
      memcpy((char *) temp, (char *) H[n].PlusCount,
	     sizeof(long) * H[n].PlusSize);
      RD_FREE(H[n].PlusCount);
      H[n].PlusCount = temp;
      H[n].PlusSize = newsize;
    }
    H[n].PlusCount[val]++;
  } else {
    absval = 0-val;
    if (H[n].MinusSize <= absval) {
      newsize = H[n].MinusSize << 1;
      while (newsize <= absval) newsize <<= 1;
      if ((temp = (long *) calloc(1,newsize*sizeof(long))) == NULL) {
	RD_FatalError("HistIncrCount: out of memory");
      }
      memcpy((char *) temp, (char *) H[n].MinusCount,
	     sizeof(long) * H[n].MinusSize);
      RD_FREE(H[n].MinusCount);
      H[n].MinusCount = temp;
      H[n].MinusSize = newsize;
    }
    H[n].MinusCount[absval]++;
  }
  return 1; 
}

extern int RD_InitHistogram(RD_Job *job) 
{
  int n;
  RD_Hist *H = job->Histogram; 

  for (n=0;n<64;n++) {
    H[n].PlusSize = 16;
    if ((H[n].PlusCount = (long *) calloc(1,16*sizeof(long))) == NULL) {
      RD_FatalError("InitHistogram: out of memory");
    }
    H[n].MinusSize = 16;
    if ((H[n].MinusCount =(long *) calloc(1,16*sizeof(long))) == NULL) {
      RD_FatalError("InitHistogram: out of memory");
    }
  }
  return 1; 
}

extern void RD_FreeHistogram(RD_Job *job) 
{
  int n;
  RD_Hist *H = job->Histogram; 
  for (n=0;n<64;n++) {
    RD_FREE(H[n].PlusCount);
    RD_FREE(H[n].MinusCount);
  }
}


static const RD_float aanscalefactor[8] = {
            1.0, 1.387039845, 1.306562965, 1.175875602,
            1.0, 0.785694958, 0.541196100, 0.275899379
          };
static RD_float DCT_scales[64]; 
static int DCT_scales_set = 0; 

static void SetDCTScales(void) 
{
  int i, j, n=0; 
  for (i=0;i<8;i++) { 
    for (j=0;j<8;j++,n++) {
      DCT_scales[n] = aanscalefactor[i] * aanscalefactor[j] *
		     ((RD_float) 8); 
    }
  }
  DCT_scales_set = 1; 
}


static void
fdct (RD_float * data)
{
  RD_float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  RD_float tmp10, tmp11, tmp12, tmp13;
  RD_float z1, z2, z3, z4, z5, z11, z13;
  register RD_float *dataptr;
  RD_float *dctscales; 
  int n;

  /* Pass 1: process rows. */

  dataptr = data;
  for (n = 7; n >= 0; n--) {
    tmp0 = dataptr[0] + dataptr[7];
    tmp7 = dataptr[0] - dataptr[7];
    tmp1 = dataptr[1] + dataptr[6];
    tmp6 = dataptr[1] - dataptr[6];
    tmp2 = dataptr[2] + dataptr[5];
    tmp5 = dataptr[2] - dataptr[5];
    tmp3 = dataptr[3] + dataptr[4];
    tmp4 = dataptr[3] - dataptr[4];
    
    /* Even part */
    
    tmp10 = tmp0 + tmp3;	/* phase 2 */
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;
    
    dataptr[0] = tmp10 + tmp11; /* phase 3 */
    dataptr[4] = tmp10 - tmp11;
    
    z1 = (tmp12 + tmp13) * ((RD_float) 0.707106781); /* c4 */
    dataptr[2] = tmp13 + z1;	/* phase 5 */
    dataptr[6] = tmp13 - z1;
    
    /* Odd part */

    tmp10 = tmp4 + tmp5;	/* phase 2 */
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

    /* The rotator is modified from fig 4-8 to avoid extra negations. */
    z5 = (tmp10 - tmp12) * ((RD_float) 0.382683433); /* c6 */
    z2 = ((RD_float) 0.541196100) * tmp10 + z5; /* c2-c6 */
    z4 = ((RD_float) 1.306562965) * tmp12 + z5; /* c2+c6 */
    z3 = tmp11 * ((RD_float) 0.707106781); /* c4 */

    z11 = tmp7 + z3;		/* phase 5 */
    z13 = tmp7 - z3;

    dataptr[5] = z13 + z2;	/* phase 6 */
    dataptr[3] = z13 - z2;
    dataptr[1] = z11 + z4;
    dataptr[7] = z11 - z4;

    dataptr += 8;		/* advance pointer to next row */
  }

  /* Pass 2: process columns. */

  dataptr = data;
  for (n = 7; n >= 0; n--) {
    tmp0 = dataptr[8*0] + dataptr[8*7];
    tmp7 = dataptr[8*0] - dataptr[8*7];
    tmp1 = dataptr[8*1] + dataptr[8*6];
    tmp6 = dataptr[8*1] - dataptr[8*6];
    tmp2 = dataptr[8*2] + dataptr[8*5];
    tmp5 = dataptr[8*2] - dataptr[8*5];
    tmp3 = dataptr[8*3] + dataptr[8*4];
    tmp4 = dataptr[8*3] - dataptr[8*4];
    
    /* Even part */
    
    tmp10 = tmp0 + tmp3;	/* phase 2 */
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;
    
    dataptr[8*0] = tmp10 + tmp11; /* phase 3 */
    dataptr[8*4] = tmp10 - tmp11;
    
    z1 = (tmp12 + tmp13) * ((RD_float) 0.707106781); /* c4 */
    dataptr[8*2] = tmp13 + z1; /* phase 5 */
    dataptr[8*6] = tmp13 - z1;
    
    /* Odd part */

    tmp10 = tmp4 + tmp5;	/* phase 2 */
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

    /* The rotator is modified from fig 4-8 to avoid extra negations. */
    z5 = (tmp10 - tmp12) * ((RD_float) 0.382683433); /* c6 */
    z2 = ((RD_float) 0.541196100) * tmp10 + z5; /* c2-c6 */
    z4 = ((RD_float) 1.306562965) * tmp12 + z5; /* c2+c6 */
    z3 = tmp11 * ((RD_float) 0.707106781); /* c4 */

    z11 = tmp7 + z3;		/* phase 5 */
    z13 = tmp7 - z3;

    dataptr[8*5] = z13 + z2; /* phase 6 */
    dataptr[8*3] = z13 - z2;
    dataptr[8*1] = z11 + z4;
    dataptr[8*7] = z11 - z4;

    dataptr++;			/* advance pointer to next column */
  }
  
  dataptr = data; 
  dctscales = DCT_scales; 
  for (n=0;n<64;n++) {
    *dataptr++ /= *dctscales++; 
  } 
}

static int IJtoWhere(int i, int j, int height, int width)
{
  /* used for blocks at the boundaries */
  if ((i<height)&&(j<width)) return((i*width)+j);
  if (i>=height) {
    i = i - 8 + height - ((height>>3)<<3);
  }
  if (j>=width) {
    j = j - 8 + width - ((width>>3)<<3);
  }
  return((i*width)+j);
}

#define Linearize(i,j,w) (((i)*(w))+(j))


static int LastDC = 0;
static boolean LastDCwasPos = TRUE;

static int BlockStats(RD_Job *job, Pixel *image, int height, int width, 
		int where_i, int where_j, RD_float *mssq,
		RD_float *SigTab, 
		RD_float zerocorrect)
{
  RD_float coefs[64]; 
  int nc,ip,jp,lowip,lowjp,highip,highjp,loc,locincr;
  long intcoef,temp;
  boolean positive;
  boolean btemp;

  lowip = where_i<<3; highip = lowip + 8; 
  lowjp = where_j<<3; highjp = lowjp + 8; 

  if ((highip > height) || (highjp > width)) {

    nc = 0; 
    for (ip=lowip;ip<highip;ip++) {
      for (jp=lowjp;jp<highjp;jp++, nc++) {
        loc =  IJtoWhere(ip,jp,height,width);
	coefs[nc] = (RD_float) image[loc]; 
        *mssq += ((RD_float) ((unsigned long)image[loc]
			     *(unsigned long)image[loc]));
      }
    }

  } else {

    nc = 0; 
    loc =  Linearize(lowip,lowjp,width);
    locincr = width - 8; 
    for (ip=lowip;ip<highip;ip++) {
      for (jp=lowjp;jp<highjp;jp++, nc++, loc++) {
	coefs[nc] = (RD_float) image[loc]; 
        *mssq += ((RD_float) ((unsigned long)image[loc]
			     *(unsigned long)image[loc]));
      }
      loc += locincr; 
    }
  }
  
  fdct(coefs);

  for (nc=0;nc<64;nc++) SigTab[nc] += (coefs[nc]*coefs[nc]);

  coefs[0] = coefs[0] - zerocorrect;
  if (job->UseDCDPCM) { 
    btemp = TRUE;
    if (coefs[0] < 0.0) btemp = FALSE;
    temp = Discretize(coefs[0]);
    coefs[0] = coefs[0] - UnDiscretize(LastDC, LastDCwasPos);
    LastDC = temp;
    LastDCwasPos = btemp;
  } 
      
  for (nc=0;nc<64;nc++) { 
    positive = TRUE;
    if (coefs[nc] < 0.0) positive = FALSE;
    intcoef = Discretize(coefs[nc]);
    if (!HistIncrCount(job, nc,intcoef,positive)) return 0; 	
  }

  return 1; 
}

static int BlockStats2(RD_Job *job, Pixel2 *image, int height, int width, 
		int where_i, int where_j, RD_float *mssq,
		RD_float *SigTab, 
		RD_float zerocorrect)
{
  RD_float coefs[64]; 
  int nc,ip,jp,lowip,lowjp,highip,highjp,loc,locincr;
  long intcoef,temp;
  boolean positive;
  boolean btemp;

  lowip = where_i<<3; highip = lowip + 8; 
  lowjp = where_j<<3; highjp = lowjp + 8; 

  if ((highip > height) || (highjp > width)) {

    nc = 0; 
    for (ip=lowip;ip<highip;ip++) {
      for (jp=lowjp;jp<highjp;jp++, nc++) {
        loc =  IJtoWhere(ip,jp,height,width);
	coefs[nc] = (RD_float) image[loc]; 
        *mssq += ((RD_float) ((unsigned long)image[loc]
			     *(unsigned long)image[loc]));
      }
    }

  } else {

    nc = 0; 
    loc =  Linearize(lowip,lowjp,width);
    locincr = width - 8; 
    for (ip=lowip;ip<highip;ip++) {
      for (jp=lowjp;jp<highjp;jp++, nc++, loc++) {
	coefs[nc] = (RD_float) image[loc]; 
        *mssq += ((RD_float) ((unsigned long)image[loc]
			     *(unsigned long)image[loc]));
      }
      loc += locincr; 
    }
  }
  
  fdct(coefs);

  for (nc=0;nc<64;nc++) SigTab[nc] += (coefs[nc]*coefs[nc]);

  coefs[0] = coefs[0] - zerocorrect;
  if (job->UseDCDPCM) { 
    btemp = TRUE;
    if (coefs[0] < 0.0) btemp = FALSE;
    temp = Discretize(coefs[0]);
    coefs[0] = coefs[0] - UnDiscretize(LastDC, LastDCwasPos);
    LastDC = temp;
    LastDCwasPos = btemp;
  } 
      
  for (nc=0;nc<64;nc++) { 
    positive = TRUE;
    if (coefs[nc] < 0.0) positive = FALSE;
    intcoef = Discretize(coefs[nc]);
    if (!HistIncrCount(job, nc,intcoef,positive)) return 0; 	
  }

  return 1; 
}




extern int RD_SetHistogram(RD_Job *job, int cnum,
   RD_float *mssq, RD_float *SigTab) 
{
  int i,j,imax,jmax,h,w;
  RD_float zerocorrect = ((RD_float) ((job->MaxSample+1)<<2)); 

  Image *Im = job->Im; 


  if (!DCT_scales_set) SetDCTScales();

  LastDC = 0; LastDCwasPos = TRUE;

  h = ImCurrPlaneRows(Im, cnum); 
  w = ImCurrPlaneCols(Im, cnum); 
  imax = h/8;
  if ((imax * 8) != h) imax++;
  jmax = w/8;
  if ((jmax * 8) != w) jmax++;

  if (job->SampleBytes == 1) { 
    for (i=0;i<imax;i++) {
      for (j=0;j<jmax;j++) {
         if (!BlockStats(job, ImPlane(Im,cnum), h, w, i, j, mssq,
  	          SigTab, zerocorrect)) return 0; 
      }
    }      
  } else {
    for (i=0;i<imax;i++) {
      for (j=0;j<jmax;j++) {
         if (!BlockStats2(job, Im2Plane(Im,cnum), h, w, i, j, mssq,
  	          SigTab, zerocorrect)) return 0; 
      }
    }      
  }
  return 1; 
}

