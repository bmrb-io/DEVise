#include "RDOPT.h"
#include "RDOPT_Internal.h"
#include <ctype.h>


static int ZigZagToN[64] =
{
0, 1, 8, 16, 9, 2, 3, 10, 
17, 24, 32, 25, 18, 11, 4, 5, 
12, 19, 26, 33, 40, 48, 41, 34, 
27, 20, 13, 6, 7, 14, 21, 28, 
35, 42, 49, 56, 57, 50, 43, 36, 
29, 22, 15, 23, 30, 37, 44, 51, 
58, 59, 52, 45, 38, 31, 39, 46, 
53, 60, 61, 54, 47, 55, 62, 63,
};

extern void RD_FatalErrorInner(RD_Job *job, char *s)
{
  (void) DD_WriteLine(&job->ErrDD, s); 
  RD_Free(job); 
}

static int blankline(char *line)
{
  int i, l = strlen(line);
  for (i=0;i<l;i++) {
    if (!isspace(line[i])) return 0;
  }
  return 1;
}

extern int RD_ReadFloatTable(DataSrc *ds, RD_float *tab, char *prefix)
{
  int n = 0, ret = 1;
  double temp;
  char argbuff[RD_STRLENMAX], *nextarg = 0;
  int preflen = 0;
  if (prefix) preflen = strlen(prefix); 

  while ((n < 64) && ret) {
    if (!nextarg || (sscanf(nextarg,"%lf",&temp) < 1)) {
      /* need to read more */ 
      while ((ret = DS_ReadLine(ds, argbuff, RD_STRLENMAX)) != 0) {
        if (!prefix) {
          if (!blankline(argbuff) && strncmp(argbuff,"#",1)) break; 
        } else {
	  if (!strncmp(argbuff, prefix, preflen)) break; 
	} 
      }
      if (ret) nextarg = argbuff; 
    } else {
      tab[n++] = (RD_float) temp; 
      while (isspace(*nextarg)) nextarg++;
      while ((*nextarg != '\0') && !isspace(*nextarg)) nextarg++; 
    } 
  } 
  if (n < 64) return 0;
  return 1; 
}

extern int RD_ReadLongTable(DataSrc *ds, long *tab, char *prefix)
{
  int n = 0, ret = 1;
  long temp;
  char argbuff[RD_STRLENMAX], *nextarg = 0;
  int preflen = 0;
  if (prefix) preflen = strlen(prefix); 

  while ((n < 64) && ret) {
    if (!nextarg || (sscanf(nextarg,"%ld",&temp) < 1)) {
      /* need to read more */ 
      while ((ret = DS_ReadLine(ds, argbuff, RD_STRLENMAX)) != 0) {
        if (!prefix) {
          if (!blankline(argbuff) && strncmp(argbuff,"#",1)) break; 
        } else {
	  if (!strncmp(argbuff, prefix, preflen)) break; 
	} 
      }
      if (ret) nextarg = argbuff; 
    } else {
      tab[n++] = (RD_float) temp; 
      while (isspace(*nextarg)) nextarg++;
      while ((*nextarg != '\0') && !isspace(*nextarg)) nextarg++; 
    } 
  } 
  if (n < 64) return 0;
  return 1; 
}


extern void RD_WriteLongTable(DataDest *dd, long *tab, char *prefix)
{
  int i;
  for (i=0;i<64;i+=8) {
    (void) DD_printf(dd,"%s %ld %ld %ld %ld %ld %ld %ld %ld\n",prefix,
      tab[i],   tab[i+1], tab[i+2], tab[i+3],
      tab[i+4], tab[i+5], tab[i+6], tab[i+7]);
  }
}

extern void RD_WriteThreshTable(DataDest *dd, long *tab, char *prefix)
{
  int i;
  float ftab[64];

  for (i=0;i<64;i++) ftab[i] = ((float) tab[i])/((float) 2.0); 

  DD_printf(dd,"# Table of thresholds\n");
  for (i=0;i<64;i+=8) {
      (void) DD_printf(dd,
      "%s %5.1lf %5.1lf %5.1lf %5.1lf %5.1lf %5.1lf %5.1lf %5.1lf\n",
      prefix,
      ftab[i],   ftab[i+1], ftab[i+2], ftab[i+3],
      ftab[i+4], ftab[i+5], ftab[i+6], ftab[i+7]);
  }
}

extern void RD_WriteErrFBppDist(DataDest *dd, RD_float *bpptab, 
    RD_float *errtab, char *prefix)
{
  int n, zn;

  DD_printf(dd,"# Rate-Distortion distribution\n");
  DD_printf(dd,"# ZigZagIndex CumulativeBpp   RMSE       PSNR\n");
  for (n=0;n<64;n++) {
    zn = ZigZagToN[n];
    DD_printf(dd,"%s",prefix);
    DD_printf(dd,"%2d             %7.6lf    %7.3lf %6.2lf\n",
        n, ((double) bpptab[zn]),
	sqrt((double) errtab[zn]),
	((double) 10.0*log10(((double) 65025.0)/
	  ((double) errtab[zn]))));
  }
}

extern void RD_WriteErrBppDist(DataDest *dd,
    long bppscale, long *bpptab, 
    RD_float *errtab, char *prefix)
{
  int n, zn;

  DD_printf(dd,"# Rate-Distortion distribution\n");
  DD_printf(dd,"# ZigZagIndex CumulativeBpp   RMSE       PSNR\n");
  for (n=0;n<64;n++) {
    zn = ZigZagToN[n];
    DD_printf(dd,"%s",prefix);
    DD_printf(dd,"%2d             %7.6lf    %7.3lf %6.2lf\n",
        n, ((double) bpptab[zn])/((double) bppscale),
	sqrt((double) errtab[zn]),
	((double) 10.0*log10(((double) 65025.0)/((double) errtab[zn]))));
  }
}

#define EQUALITY_DELTA ((RD_float) 1.0)

extern boolean RD_NotEqualReal(RD_float x, RD_float y)
{
  RD_float diff;
  diff = x-y;
  if (diff < 0.0) diff = 0.0 - diff;
  if (diff <= EQUALITY_DELTA) return(FALSE);
  else return(TRUE);
}

extern void RD_SigToRemainingSig(RD_float *Sig, RD_float *RemainingSig)
{
  int i;

  RemainingSig[ZigZagToN[63]] = ((RD_float) 0.0);
  for (i=62;i>=0;i--)
    RemainingSig[ZigZagToN[i]]
      = Sig[ZigZagToN[i+1]]
         + RemainingSig[ZigZagToN[i+1]];
}

extern void RD_DumpJobChars(RD_Job *job, DataDest *dd)
{
  (void) DD_printf(dd,"#");
  ImPrintImgChars(job->Im, dd); 

  DD_printf(dd,"#Number of qtables used: %d\n",job->NumTables);
  if (job->UseLagrangian) {
    DD_printf(dd,"#Optimization parameters (method: Lagrangian): \n");
    DD_printf(dd,"#\t Coarseness = %d, ThreshSpan = %d\n",
	   job->Coarseness, job->ThreshSpan);
  } else {
    DD_printf(dd,"#Optimization parameters (method: Dynamic Programming): \n");
    DD_printf(dd,"#\tMaxBpp = %lf, BppScale = %d, Coarseness = %d, ThreshSpan = %d\n",
	   (double) job->opt_method.dp.BppMax,
	   job->opt_method.dp.BppScale, job->Coarseness,
	   job->ThreshSpan);
  }
}

