#ifndef RD_INTERNAL_H
#define RD_INTERNAL_H

#include "RDOPT.h"
#include "RDOPT_Operators.h"

#define RD_STATE_INITED 0
#define RD_STATE_OPTIMIZING 1 
#define RD_STATE_OPTDONE 10
#define RD_STATE_DEAD 20


#define RD_INFINITY ((RD_float) 1e300) /* machine-dependent */

#define RD_FREE(ptr) {if (ptr) free(ptr); (ptr) = 0;}


extern int RD_FillDerived(RD_Job * job); 
extern int RD_PrepareForErrBpp(RD_Job *job, int unum); 
extern int RD_SetErrBpp(RD_Job *job, int unum); 
extern int RD_OptimizeUnit(RD_Job *job, int unum); 
extern int RD_CombineUnits(RD_Job *job); 
extern void RD_Epilogue(RD_Job *job); 
extern int RD_InitHistogram(RD_Job *job); 
extern void RD_FreeHistogram(RD_Job *job); 
extern int RD_SetHistogram(RD_Job *job, int cnum,
   RD_float *mssq, RD_float *SigTab); 
extern int RD_LagrPrepareForErrBpp(RD_Job *job, int unum); 
extern int RD_LagrSetErrBpp(RD_Job *job, int unum); 
extern int RD_LagrSortErrBpp(RD_Job *job, int unum); 
extern void RD_LagrEpilogue(RD_Job *job); 
extern long RD_QToQentry(long q, RD_QMapper qmfunc, boolean roundup); 
extern void RD_FatalErrorInner(RD_Job *job, char *s); 
extern int RD_ReadFloatTable(DataSrc *ds, RD_float *tab, char *prefix); 
extern int RD_ReadLongTable(DataSrc *ds, long *tab, char *prefix); 
extern void RD_WriteLongTable(DataDest *dd, long *tab, char *prefix); 
extern void RD_WriteThreshTable(DataDest *dd, long *tab, char *prefix); 
extern void RD_WriteErrFBppDist(DataDest *dd, RD_float *bpptab, 
    RD_float *errtab, char *prefix); 
extern void RD_WriteErrBppDist(DataDest *dd, long bppscale,
    long *bpptab, 
    RD_float *errtab, char *prefix); 
extern boolean RD_NotEqualReal(RD_float x, RD_float y); 
extern void RD_SigToRemainingSig(RD_float *Sig,RD_float *RemainingSig); 

#define RD_FatalError(msg) \
  {RD_FatalErrorInner(job, (msg)); return 0;}
#define RD_NonFatalError(msg) \
  {(void) DD_WriteLine(&job->ErrDD, (msg)); return 0;}

extern int RD_DP_GetQForBpp(RD_Job *job, RD_float bpp, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  
extern int RD_DP_GetQForSize(RD_Job *job, long size, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  
extern int RD_DP_GetQForPsnr(RD_Job *job, RD_float psnr, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  
extern int RD_DP_GetQForSnr(RD_Job *job, RD_float snr, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  
extern int RD_DP_GetQForRmse(RD_Job *job, RD_float rmse, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  

extern int RD_LagrGetQForBpp(RD_Job *job, RD_float bpp, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  
extern int RD_LagrGetQForSize(RD_Job *job, long size, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  
extern int RD_LagrGetQForPsnr(RD_Job *job, RD_float psnr, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  
extern int RD_LagrGetQForSnr(RD_Job *job, RD_float snr, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  
extern int RD_LagrGetQForRmse(RD_Job *job, RD_float rmse, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd);  


extern boolean
RD_LagrFindRate(RD_Job *job, RD_float *bpp, RD_float *err, 
    RD_float lambdamin, RD_float lambdamax, RD_float *lambda,
    long (*anstab)[64]); 
extern void RD_DumpJobChars(RD_Job *job, DataDest *dd); 

#endif /* RD_INTERNAL_H */ 

