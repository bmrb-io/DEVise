#include "RDOPT.h"
#include "RDOPT_Internal.h"

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



static long ScanRowForErr(RD_float *row, RD_float err,
    long least, long most)
{
  long i;
  for (i=least;i<=most;i++) {
    if (row[i] < err) return(i);
  }
  return(most);
}

static long ScanRowForBpp(RD_float *row, long bpp,
    long least, long most)
{
  long i;
  if (bpp < least) return(least);
  if (bpp > most) return(most);
  i = bpp; 
  while (i>=least) {
    if (row[i] < RD_INFINITY) return(i);
    i--;
  }
  /* will not come here */ 
  return(-1);
}

static void GetStartingPoints(long *ToCombine[], long *StartingPt,
			      long Loc, int NumUnits)
{
  int i; 
  long  NextLoc;
  NextLoc = Loc;
  for (i=NumUnits-1;i>=1;i--) {
    StartingPt[i] = ToCombine[i][NextLoc];
    NextLoc -= StartingPt[i];
  }
  StartingPt[0] = NextLoc;
}


static void RecoverQ(long Loc, int unum, RD_Job *job,
  long *Q, long *Bpp, RD_float *Err)
{
  /* ThreshSpan was 0 */
  long n, NextLoc;

  NextLoc = Loc;

  for (n=0;n<64;n++) {
    Q[n] = (long) job->opt_method.dp.QChoice[unum][n][NextLoc];

    Bpp[n] = job->opt_method.dp.Bpp[unum][n][Q[n]];

    if (job->opt_method.dp.ErrEncodesRow[unum][n]) {
      Err[n] = job->opt_method.dp.Err[unum][n][Bpp[n] -
		      job->opt_method.dp.BppOffsetInEncoding[unum][n]];
    } else {
      Err[n] = job->opt_method.dp.Err[unum][n][Q[n]];
    }
    Err[n] /= job->UnitWeights[unum];


    NextLoc -= Bpp[n];
  }

  /* convert Err and Bpp to cumulative zigzag values */
  for (n=1;n<64;n++) {
    Err[ZigZagToN[n]] += Err[ZigZagToN[n-1]];
    Bpp[ZigZagToN[n]] += Bpp[ZigZagToN[n-1]];
  }
  for (n=0;n<64;n++) {
    Err[n] += job->RemainingSignal[unum][n];
    Err[n] /= job->NumPixels[unum];
  }

  for (n=0;n<64;n++) Q[n] = job->QentryToQ(Q[n]);

}

static void RecoverQandT(long Loc, int unum,
  RD_Job *job, long *Q, long *T, long *Bpp, RD_float *Err)
{
  long n, NextLoc, indx;
  NextLoc = Loc;
  for (n=0;n<64;n++) {
    Q[n] = (long) job->opt_method.dp.QChoice[unum][n][NextLoc];
    T[n] = (long) job->opt_method.dp.TChoice[unum][n][NextLoc];
    indx = Q[n]*(job->ThreshSpan+1) + T[n];

    Bpp[n] = job->opt_method.dp.Bpp[unum][n][indx];

    NextLoc -= Bpp[n];

    if (job->opt_method.dp.ErrEncodesRow[unum][n]) {
      Err[n] = job->opt_method.dp.Err[unum][n][Bpp[n] -
		      job->opt_method.dp.BppOffsetInEncoding[unum][n]];
    } else {
      Err[n] = job->opt_method.dp.Err[unum][n][indx];
    }
    Err[n] /= job->UnitWeights[unum];
  }

  for (n=0;n<64;n++) Q[n] = job->QentryToQ(Q[n]);

  for (n=0;n<64;n++) T[n] += Q[n];

  /* convert Err */
  for (n=1;n<64;n++) {
    Err[ZigZagToN[n]] += Err[ZigZagToN[n-1]];
    Bpp[ZigZagToN[n]] += Bpp[ZigZagToN[n-1]];
  }
  for (n=0;n<64;n++) {
    Err[n] += job->RemainingSignal[unum][n];
    Err[n] /= job->NumPixels[unum];
  }

}

static int RD_GetQForTarget(RD_Job *job, RD_float target,
  boolean targetIsBpp, boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd) 
{
  long IntTargetBpp, ResultSize;
  RD_float ResultBpp, ResultSNR, ResultPSNR, RealTargetBpp;
  RD_float ResultErr, temp; 
  long Q[64], T[64], BppDist[64];
  RD_float ErrDist[64];

  int unum;
  long StartingPt[IM_MAXPLANES], Loc;
  
  if (targetIsBpp) { 
    /* target is bpp */ 
    RealTargetBpp = target - job->CorrectionBpp; 
    RealTargetBpp *= ((RD_float) job->opt_method.dp.BppScale);
    IntTargetBpp = RoundOff(RealTargetBpp);
    if (job->NumTables==1) {
       Loc = ScanRowForBpp(job->opt_method.dp.LastRow[0],IntTargetBpp,
	      job->opt_method.dp.LeastLast[0],
	      job->opt_method.dp.MostLast[0]);
       if (Loc < 0) return 0; 
    } else { 
       Loc = ScanRowForBpp(job->opt_method.dp.CombinedLastRow,
	     IntTargetBpp, job->opt_method.dp.CombinedLeast,
	     job->opt_method.dp.CombinedMost);
       if (Loc < 0) return 0; 
       GetStartingPoints(job->opt_method.dp.ToCombine, StartingPt,
			      Loc, job->NumTables);
    }
  } else {
    /* target is log10(Distortion) */ 
    if (job->NumTables==1) {
       Loc = ScanRowForErr(job->opt_method.dp.LastRow[0],target, 
	      job->opt_method.dp.LeastLast[0],
	      job->opt_method.dp.MostLast[0]);
       if (Loc < 0) return 0; 
    } else { 
       Loc = ScanRowForErr(job->opt_method.dp.CombinedLastRow,
	     target, job->opt_method.dp.CombinedLeast,
	     job->opt_method.dp.CombinedMost);
       if (Loc < 0) return 0; 
       GetStartingPoints(job->opt_method.dp.ToCombine, StartingPt,
			      Loc, job->NumTables);
    }
  }

  /******** extract Qtable(s) **********/

  (void) DD_printf(dd,"#%s\n",RD_IDSTRING);
  RD_DumpJobChars(job, dd);
  (void) DD_printf(dd,"#Bits per pixel for each unit and the whole image\n");
  (void) DD_printf(dd,"#    reported as num-bits/%lf\n",
    ((double) job->NumPixelsForBpp));

  if (job->NumTables==1) {
    (void) DD_printf(dd,"#Quantization table for color planes %d thru %d\n",
	    0, ImPlanesOfImg(job->Im)-1);

    if (job->ThreshSpan > 0) {
	  RecoverQandT(Loc, 0, job, Q, T, BppDist, ErrDist);
	  RD_WriteLongTable(dd, Q, "");
	  RD_WriteThreshTable(dd, T, "#T ");
	  if (PrintStats)
	    RD_WriteErrBppDist(dd, job->opt_method.dp.BppScale,
	      BppDist, ErrDist, "#D ");
    } else {
          RecoverQ(Loc, 0, job, Q, BppDist, ErrDist);
	  RD_WriteLongTable(dd, Q, "");
	  if (PrintStats)
	    RD_WriteErrBppDist(dd, job->opt_method.dp.BppScale,
	      BppDist, ErrDist, "#D ");
    }

    ResultErr = job->opt_method.dp.LastRow[0][Loc];
    ResultBpp = ((RD_float) Loc)/
		((RD_float) job->opt_method.dp.BppScale);
    temp = ResultBpp * job->NumPixelsForBpp / 8.0;
    ResultSize = RoundOff(temp);

    if (job->CorrectionBpp != (RD_float) 0) { 
      (void) DD_printf(dd,"#RdoptBpp = %lf bits per pixel\n",
	((double)ResultBpp));
      (void) DD_printf(dd,"#RdoptSize = %ld bytes\n", ResultSize);
      (void) DD_printf(dd,"#CorrectionToBpp = %lf bits per pixel\n",
         (double) job->CorrectionBpp); 
      ResultBpp += job->CorrectionBpp; 
      temp = ResultBpp * job->NumPixelsForBpp / 8.0;
      ResultSize = RoundOff(temp);
    }

    ResultSNR = (job->LogTotalSignalSq - ResultErr)*10.0;
    ResultPSNR = (job->LogTotalPeakSq - ResultErr)*10.0;

    (void) DD_printf(dd,"#Bpp = %lf bits per pixel\n", ((double) ResultBpp));
    (void) DD_printf(dd,"#Size = %ld bytes\n", ResultSize);
    (void) DD_printf(dd,"#SNR = %lf dB\n", ((double) ResultSNR));
    (void) DD_printf(dd,"#PSNR = %lf dB\n", ((double) ResultPSNR));
    ResultErr -= ((RD_float) log10((double) job->TotalNumPixels));
    ResultErr = (RD_float) sqrt(pow(10.0,((double) ResultErr)));
    (void) DD_printf(dd,"#RMSE = %lf\n", ResultErr); 

  } else {

    for (unum=0;unum < job->NumTables; unum++) {
      (void) DD_printf(dd,"#Quantization table for color planes %d thru %d\n",
         unum,((unum==(job->NumTables-1))?
           (ImPlanesOfImg(job->Im)-1):unum));

      if (job->ThreshSpan > 0) {
        RecoverQandT(StartingPt[unum], unum, job, Q, T,
	   BppDist, ErrDist);
        RD_WriteLongTable(dd, Q, "");
        RD_WriteThreshTable(dd, T, "#T ");
        if (PrintStats)
          RD_WriteErrBppDist(dd, job->opt_method.dp.BppScale,
            BppDist, ErrDist, "#D ");
      } else {
        RecoverQ(StartingPt[unum], unum, job, Q, BppDist, ErrDist);
        RD_WriteLongTable(dd, Q, "");
        if (PrintStats)
          RD_WriteErrBppDist(dd, job->opt_method.dp.BppScale,
	      BppDist, ErrDist, "#D ");
        }
        ResultErr = job->opt_method.dp.LastRow[unum][StartingPt[unum]];
        ResultBpp = ((RD_float) StartingPt[unum])/
	  ((RD_float) job->opt_method.dp.BppScale);
        temp = ResultBpp * job->NumPixelsForBpp / 8.0;
	ResultSize = RoundOff(temp);
	ResultSNR = (job->LogSignalSq[unum] - ResultErr)*10.0;
	ResultPSNR = (job->LogPeakSq[unum] - ResultErr)*10.0;

	(void) DD_printf(dd,"#Bpp = %lf bits per pixel\n",
	  ((double) ResultBpp));
	(void) DD_printf(dd,"#Size = %ld bytes\n", ResultSize);
	(void) DD_printf(dd,"#SNR = %lf dB\n", ((double) ResultSNR));
	(void) DD_printf(dd,"#PSNR = %lf dB\n", ((double) ResultPSNR));
	ResultErr -= ((RD_float) log10((double) job->NumPixels[unum]));
	(void) DD_printf(dd,"#RMSE = %lf\n",
	  sqrt(pow(10.0,((double) ResultErr))));
	(void) DD_printf(dd,"#\n");

      }
      (void) DD_printf(dd,"#For the entire image:\n");
      ResultErr = job->opt_method.dp.CombinedLastRow[Loc];
      ResultBpp = ((RD_float) Loc)/
	((RD_float) job->opt_method.dp.BppScale);
      temp = ResultBpp * job->NumPixelsForBpp/ 8.0;
      ResultSize = RoundOff(temp);
      if (job->CorrectionBpp != (RD_float) 0) { 
          (void) DD_printf(dd,"#RdoptBpp = %lf bits per pixel\n",
	    ((double) ResultBpp));
          (void) DD_printf(dd,"#RdoptSize = %ld bytes\n", ResultSize);
          (void) DD_printf(dd,"#CorrectionToBpp = %lf bits per pixel\n",
             (double) job->CorrectionBpp); 
          ResultBpp += job->CorrectionBpp; 
          temp = ResultBpp * job->NumPixelsForBpp / 8.0;
          ResultSize = RoundOff(temp);
      }
      ResultSNR = (job->LogTotalSignalSq - ResultErr)*10.0;
      ResultPSNR = (job->LogTotalPeakSq - ResultErr)*10.0;

      (void) DD_printf(dd,"#Bpp = %lf bits per pixel\n",
	((double) ResultBpp));
      (void) DD_printf(dd,"#Size = %ld bytes\n", ResultSize);
      (void) DD_printf(dd,"#SNR = %lf dB\n", ((double) ResultSNR));
      (void) DD_printf(dd,"#PSNR = %lf dB\n", ((double) ResultPSNR));
      ResultErr -= ((RD_float) log10((double) job->TotalNumPixels));
      ResultErr = (RD_float) sqrt(pow(10.0,((double) ResultErr)));
      (void) DD_printf(dd,"#RMSE = %lf\n", ResultErr); 
    }

    (void) DD_printf(dd,"#END\n");

    *res_size = ResultSize;
    *res_bpp = ResultBpp;
    *res_psnr = ResultPSNR;
    *res_rmse = ResultErr;
    return 1; 

}

extern int RD_DP_GetQForBpp(RD_Job *job, RD_float bpp, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd) 
{
  return RD_GetQForTarget(job, bpp, 1, PrintStats, res_size,
    res_bpp, res_psnr, res_rmse, dd); 
} 

extern int RD_DP_GetQForSize(RD_Job *job, long size, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd) 
{
  RD_float RealTargetBpp = ((RD_float) (size*8.0));
  RealTargetBpp /= job->NumPixelsForBpp;
  return RD_GetQForTarget(job, RealTargetBpp, 1, PrintStats, res_size,
    res_bpp, res_psnr, res_rmse, dd); 
} 

extern int RD_DP_GetQForPsnr(RD_Job *job, RD_float psnr, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd) 
{
  RD_float TargetErr = job->LogTotalPeakSq;
  TargetErr -= ((RD_float) psnr/10.0);
  return RD_GetQForTarget(job, TargetErr, 0, PrintStats, res_size,
    res_bpp, res_psnr, res_rmse, dd); 
} 

extern int RD_DP_GetQForSnr(RD_Job *job, RD_float snr, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd) 
{ 
  RD_float TargetErr = job->LogTotalSignalSq;
  TargetErr -= ((RD_float) snr/10.0);
  return RD_GetQForTarget(job, TargetErr, 0, PrintStats, res_size,
    res_bpp, res_psnr, res_rmse, dd); 
} 

extern int RD_DP_GetQForRmse(RD_Job *job, RD_float rmse, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd) 
{ 
  RD_float TargetErr = ((RD_float) log10((double) rmse))*2.0;
  TargetErr += ((RD_float) log10((double) job->TotalNumPixels));
  return RD_GetQForTarget(job, TargetErr, 0, PrintStats, res_size,
    res_bpp, res_psnr, res_rmse, dd); 
} 

