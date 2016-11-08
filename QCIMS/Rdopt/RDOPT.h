
#ifndef RDOPT_H
#define RDOPT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef IMAGE_H_INCLUDED
#include "Image.h"
#endif


#define RD_IDSTRING "RDOPTv3.0" 


typedef unsigned char RD_Qentry;
#define RD_QENTRY_MAX 255

typedef long (*RD_QMapper)(long qe); 

#define RD_NUM_HOOKS 20 /* # of lagrangian hooks */ 

#define RD_STRLENMAX 400

#ifndef boolean
#define boolean int
#endif
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define RD_float double

/*********************************
type definitions
**********************************/

/******* histograms **********/
struct RD_HistStruct {
  /*
    PlusCount[v] will store the number of times the DCT
    coefficient (for which this struct is storing the
    counts) takes a postive value that gets Discretized
    to the integer v. (For definition of "Discretization"
    see opers.h)
    MinusCount[v] will store the number of times the DCT
    coefficient takes a negative value that gets Discretized
    to the integer -v.
    The array bounds will be 0..PlusSize and 0..MinusSize,
    respectively.
  */

  long PlusSize;
  long MinusSize;
  long *PlusCount;
  long *MinusCount;
};
typedef struct RD_HistStruct RD_Hist;



/************** OptimizationJob ***********/
struct RD_JobStruct {
  int State; 
  Image *Im;
  long MaxSample;
  int SampleBytes; 

  int Coarseness; 
  RD_QMapper QentryToQ;

  DataDest ErrDD; 
  int NumTables;
                 /* Number of quantization tables to be found.
		    If NumTables < ImPlanesOfImg(Im), then
		    the last table will be used for all the
		    color planes numbered (NumTables - 1)
		    thru (ImPlanesOfImg(Im) - 1). We call
		    each set of planes being quantized together
		    a "Unit" 
                 */

  int VerboseLevel;
  
  int ThreshSpan;
    /* for qentry=q, thresholds upto
     q/2 + ThreshSpan/2 will be tried */

  boolean UseLagrangian;
  
  RD_float CorrectionBpp;

  union { 
    struct { 
      /*** fields to be used for dynamic programming  ***/
  
      RD_float BppMax;
      long BppScale;
      long TableRowSize;
      long LeastLast[IM_MAXPLANES]; /* lower and */
      long MostLast[IM_MAXPLANES];  /* upper bounds on the extent of LastRow */
      RD_float *LastRow[IM_MAXPLANES]; /* last row of Dyn Prog table
  				     for each unit */
      RD_float *CombinedLastRow;
      long CombinedLeast;
      long CombinedMost;
      long *ToCombine[IM_MAXPLANES]; /* ToCombine[i][bits] will give
  				    the starting point in LastRow[i] */
  
      RD_Qentry *QChoice[IM_MAXPLANES][64];
      RD_Qentry *TChoice[IM_MAXPLANES][64];

      RD_float *Err[IM_MAXPLANES][64]; 
      long *Bpp[IM_MAXPLANES][64]; /* BPP for each unit needs
  				     to be retained */
	/* explain the complexity of encoded use of Err and Bpp some day */
      boolean ErrEncodesRow[IM_MAXPLANES][64];
      RD_Qentry *QforBpp[64];
      RD_Qentry *TforBpp[64];

      long BppOffsetInEncoding[IM_MAXPLANES][64];
      long BppMaxInEncoding[IM_MAXPLANES][64];


    }  dp;

    struct {
      /***** fields for Lagrangian method  ***/
      RD_float ErrScale; /* each error will be divided by this */
      RD_float LambdaMax;
      RD_float BppMax;
      RD_float BppMin;
      RD_float ErrMax;
      RD_float ErrMin;
      RD_float *Err[IM_MAXPLANES][64];
      RD_float *Bpp[IM_MAXPLANES][64];
      long    *SortedIndex[IM_MAXPLANES][64];
      long     IndexEntries[IM_MAXPLANES][64];
      RD_float *Lambda[IM_MAXPLANES][64];

      RD_float BppHook[RD_NUM_HOOKS];
      RD_float LambdaHook[RD_NUM_HOOKS];
      RD_float ErrHook[RD_NUM_HOOKS];

      RD_float deltalambda;
      RD_float deltabpp;
      RD_float deltamse; 

    }  lagr;


  } opt_method;

  boolean UnitWeightsExist; 
  RD_float UnitWeights[IM_MAXPLANES];

  boolean WeightedCoefs[IM_MAXPLANES];
  RD_float CoefWeights[IM_MAXPLANES][64];

  RD_float bppWeight[IM_MAXPLANES]; /* bpp = \sum_{i=0..NumTables-1}
					       bpp-for-ith-unit *
					       bppWeight[i] */
  RD_float RemainingSignal[IM_MAXPLANES][64];
  RD_float LogSignalSq[IM_MAXPLANES];
  RD_float LogTotalSignalSq;
  RD_float LogPeakSq[IM_MAXPLANES];
  RD_float LogTotalPeakSq;
  RD_float NumPixels[IM_MAXPLANES];
  RD_float NumBlocks[IM_MAXPLANES];
  RD_float TotalNumPixels;
  RD_float NumPixelsForBpp;
  

  RD_Hist Histogram[64];

  boolean MinTableExists[IM_MAXPLANES]; 
  long MinTable[IM_MAXPLANES][64];
  boolean MaxTableExists[IM_MAXPLANES]; 
  long MaxTable[IM_MAXPLANES][64];
  long DCclamp;
  boolean UseDCDPCM; 


};

typedef struct RD_JobStruct RD_Job;


/********************** prototypes ************************/

extern int RD_Init(RD_Job *job, Image *Im); 
extern int RD_SetThreshSpan(RD_Job *job, long thspan); 
extern void RD_SetVerboseLevel(RD_Job *job, int verb); 
extern int RD_SetDCDPCM(RD_Job *job, boolean on); 
extern int RD_SetOptMethod(RD_Job *job, char * method); 
extern int RD_SetMaxSample(RD_Job *job, long maxs); 
extern int RD_SetQMapper(RD_Job *job, RD_QMapper qmfunc); 
extern int RD_SetBppMax(RD_Job *job, RD_float bppmax); 
extern int RD_SetBppScale(RD_Job *job, long bppscale); 
extern int RD_SetUnitWeights(RD_Job *job, int nweights, RD_float *wts); 
extern int RD_SetDCclamp(RD_Job *job, long dcc); 
extern int RD_SetMinTables(RD_Job *job, int startunit,
                 void *src, long srcbytes, int srckind); 
extern int RD_SetMaxTables(RD_Job *job, int startunit,
                 void *src, long srcbytes, int srckind); 
extern int RD_SetCoefWeights(RD_Job *job, int startunit,
                 void *src, long srcbytes, int srckind); 
extern int RD_SetNumTables(RD_Job *job, int numtables); 
extern int RD_SetCoarseness(RD_Job *job, int coarseness); 
extern int RD_Optimize(RD_Job *job); 

extern long RD_QMapperId(long qe); 
extern long RD_QMapper8fast1(long qe); 
extern long RD_QMapper8fast2(long qe); 
extern long RD_QMapper8fast3(long qe); 
extern long RD_QMapper8fast4(long qe); 
extern long RD_QMapper8fast5(long qe); 
extern long RD_QMapper8fast6(long qe); 
extern long RD_QMapper8fast7(long qe); 
extern long RD_QMapper8fastest(long qe); 
extern long RD_QMapper10(long qe); 
extern long RD_QMapper10fast1(long qe); 
extern long RD_QMapper10fast2(long qe); 
extern long RD_QMapper10fast3(long qe); 
extern long RD_QMapper10fast4(long qe); 
extern long RD_QMapper10fastest(long qe); 
extern long RD_QMapper12(long qe); 
extern long RD_QMapper12fastest(long qe); 
extern long RD_QMapper16(long qe); 
extern long RD_QMapper16fastest(long qe); 

extern int RD_ApplyCorrection(RD_Job *job, RD_float add_bpp);

extern int RD_LagrSetDeltaBpp(RD_Job *job, RD_float deltabpp); 
extern int RD_LagrSetDeltaMse(RD_Job *job, RD_float deltamse); 
extern int RD_LagrSetDeltaLambda(RD_Job *job, RD_float deltalambda); 

extern int RD_GetQForBpp(RD_Job *job, RD_float bpp, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  
extern int RD_GetQForSize(RD_Job *job, long size, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  
extern int RD_GetQForPsnr(RD_Job *job, RD_float psnr, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  
extern int RD_GetQForSnr(RD_Job *job, RD_float snr, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  
extern int RD_GetQForRmse(RD_Job *job, RD_float rmse, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  

extern int RD_GetFirstPoint(RD_Job *job, 
  long *res_size, RD_float *res_bpp,
  RD_float *res_psnr, RD_float *res_rmse);  
extern int RD_GetLastPoint(RD_Job *job, 
  long *res_size, RD_float *res_bpp,
  RD_float *res_psnr, RD_float *res_rmse);  

extern int RD_BppPsnrCurve(RD_Job *job, 
   RD_float minbpp, RD_float maxbpp, int npoints, 
   boolean PrintStats, boolean printbpps,  
   DataDest *dd, char *qfile_prefix, int do_compress,  
   void *ssrc, long ssrcbytes, int ssrckind);

extern int RD_FillQclicCurve(RD_Job *job, 
   RD_float minbpp, RD_float maxbpp, int npoints, 
   char *qfile_prefix, int do_compress,
   void *ssrc, long ssrcbytes, int ssrckind);

extern void RD_Free(RD_Job *job);

extern int RD_SetErrDest(RD_Job *job, void *dest, int destkind); 

#endif /* RDOPT_H */

