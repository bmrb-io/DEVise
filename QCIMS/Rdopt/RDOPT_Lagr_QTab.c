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


static long FindLambda(RD_float *llist, RD_float lambda, long num)
{
  long left, right, mid;

  if (lambda > llist[0]) return(0L);
  if (lambda <= 0.0) {
    return(num-1);
  }

  left = 0;
  right = num;
  mid = num/2;
  while (mid > 0) { 
    if ((lambda > llist[mid]) && (lambda <= llist[mid - 1]))
      return(mid);
    else if (llist[mid] > lambda) {
      left = mid;
      mid = (left + right)/2;
    }
    else {
      right = mid;
      mid = (left + right)/2;
    }
  } 
  return(0L);
}

static boolean EqualTabs(long (*tab1)[64], long (*tab2)[64],
  int numtabs)
{
  int u, n;
  for (u=0;u<numtabs;u++) {
    for (n=0;n<64;n++) {
      if (tab1[u][n] != tab2[u][n]) return(FALSE);
    }
  }
  return(TRUE);
}

#define optstr job->opt_method.lagr

extern boolean
RD_LagrFindRate(RD_Job *job, RD_float *bpp, RD_float *err, 
    RD_float lambdamin, RD_float lambdamax, RD_float *lambda,
    long (*anstab)[64])
{
  long unum, n;
  long tab1[IM_MAXPLANES][64];
  long tab2[IM_MAXPLANES][64];
  long tab3[IM_MAXPLANES][64];

  long (*lefttab)[64], (*righttab)[64], (*midtab)[64], (*temptab)[64];
  RD_float leftbpp, rightbpp, lefterr, righterr, midbpp, miderr;
  RD_float lleft, lright, lmid;
  RD_float diff, absdiff, tempdiff, tempabsdiff; 

  long where;

  lefttab = tab1;
  righttab = tab2;
  midtab = tab3;


  /* find first tables */

  lleft = lambdamax;
  leftbpp = ((RD_float) 0.0);
  lefterr = ((RD_float) 0.0);
  for (unum = 0; unum < job->NumTables; unum++) {
    for (n = 0; n < 64; n++) {
      where = FindLambda(optstr.Lambda[unum][n], lleft, optstr.IndexEntries[unum][n]);
      lefttab[unum][n] = optstr.SortedIndex[unum][n][where];
      leftbpp += optstr.Bpp[unum][n][lefttab[unum][n]];
      lefterr += optstr.Err[unum][n][lefttab[unum][n]];
    }
  }

  diff = leftbpp - (*bpp);
  if (diff < ((RD_float) 0.0)) absdiff = ((RD_float) 0.0) - diff;
  else absdiff = diff;

  lmid = lleft;
  for (unum = 0; unum < job->NumTables; unum++) {
      for (n = 0; n < 64; n++) {
        midtab[unum][n] = lefttab[unum][n];
      }
  }
  miderr = lefterr;
  midbpp = leftbpp;


  lright = lambdamin;
  rightbpp = ((RD_float) 0.0);
  righterr = ((RD_float) 0.0);
  for (unum = 0; unum < job->NumTables; unum++) {
    for (n = 0; n < 64; n++) {
      where = FindLambda(optstr.Lambda[unum][n], lright,
			       optstr.IndexEntries[unum][n]);
      righttab[unum][n] = optstr.SortedIndex[unum][n][where];
      rightbpp += optstr.Bpp[unum][n][righttab[unum][n]];
      righterr += optstr.Err[unum][n][righttab[unum][n]];
    }
  }

  tempdiff = rightbpp - (*bpp);
  if (tempdiff < ((RD_float) 0.0)) tempabsdiff = ((RD_float) 0.0) -tempdiff;
  else tempabsdiff = tempdiff;

  if (tempabsdiff < absdiff) {
    diff = tempdiff;
    absdiff = tempabsdiff;
    lmid = lright;
    miderr = righterr;
    midbpp = rightbpp;
    for (unum = 0; unum < job->NumTables; unum++) {
      for (n = 0; n < 64; n++) {
        midtab[unum][n] = righttab[unum][n];
      }
    }
  }

  while (absdiff > optstr.deltabpp) {
    if (diff > ((RD_float) 0.0)) {
      /* we are greater than target rate.. reduce lambda */
      if ((lmid - lright) < optstr.deltalambda) break;
      if (EqualTabs(midtab, righttab, job->NumTables)) break;
      lleft = lmid;
      temptab = lefttab;
      lefttab = midtab;
      midtab = temptab;
      lefterr = miderr;
      leftbpp = midbpp;
    }
    else {
      if ((lleft - lmid) < optstr.deltalambda) break;
      if (EqualTabs(midtab, lefttab, job->NumTables)) break;
      lright = lmid;
      temptab = righttab;
      righttab = midtab;
      midtab = temptab;
      righterr = miderr;
      rightbpp = midbpp;
    }


    lmid = (lleft + lright)/((RD_float) 2.0);
    midbpp = ((RD_float) 0.0);
    miderr = ((RD_float) 0.0);

    for (unum = 0; unum < job->NumTables; unum++) {
      for (n = 0; n < 64; n++) {
        where = FindLambda(optstr.Lambda[unum][n], lmid, optstr.IndexEntries[unum][n]);
        midtab[unum][n] = optstr.SortedIndex[unum][n][where];
        midbpp += optstr.Bpp[unum][n][midtab[unum][n]];
        miderr += optstr.Err[unum][n][midtab[unum][n]];
      }
    }


    diff = midbpp - (*bpp);
    if (diff < ((RD_float) 0.0)) absdiff = ((RD_float) 0.0) - diff;
    else absdiff = diff;
  }
  
  *bpp = midbpp;
  *err = miderr;
  *lambda = lmid;
  for (unum = 0; unum < job->NumTables; unum++) {
      for (n = 0; n < 64; n++) {
        anstab[unum][n] = midtab[unum][n];
      }
  }
  return(TRUE);
}


static boolean
lagrFindErr(RD_Job *job, RD_float *bpp, RD_float *err, 
    RD_float lambdamin, RD_float lambdamax, RD_float *lambda,
    long (*anstab)[64])
{
  long unum, n;
  long tab1[IM_MAXPLANES][64];
  long tab2[IM_MAXPLANES][64];
  long tab3[IM_MAXPLANES][64];

  long (*lefttab)[64], (*righttab)[64], (*midtab)[64], (*temptab)[64];
  RD_float leftbpp, rightbpp, lefterr, righterr, midbpp, miderr;
  RD_float lleft, lright, lmid;
  RD_float diff, absdiff, tempdiff, tempabsdiff; 

  long where;


#define optstr job->opt_method.lagr

  lefttab = tab1;
  righttab = tab2;
  midtab = tab3;


  /* find first tables */

  lleft = lambdamax;
  leftbpp = ((RD_float) 0.0);
  lefterr = ((RD_float) 0.0);
  for (unum = 0; unum < job->NumTables; unum++) {
    for (n = 0; n < 64; n++) {
      where = FindLambda(optstr.Lambda[unum][n], lleft, optstr.IndexEntries[unum][n]);
      lefttab[unum][n] = optstr.SortedIndex[unum][n][where];
      leftbpp += optstr.Bpp[unum][n][lefttab[unum][n]];
      lefterr += optstr.Err[unum][n][lefttab[unum][n]];
    }
  }

  diff = lefterr - (*err);
  if (diff < ((RD_float) 0.0)) absdiff = ((RD_float) 0.0) - diff;
  else absdiff = diff;

  lmid = lleft;
  for (unum = 0; unum < job->NumTables; unum++) {
      for (n = 0; n < 64; n++) {
        midtab[unum][n] = lefttab[unum][n];
      }
  }
  miderr = lefterr;
  midbpp = leftbpp;


  lright = lambdamin;
  rightbpp = ((RD_float) 0.0);
  righterr = ((RD_float) 0.0);
  for (unum = 0; unum < job->NumTables; unum++) {
    for (n = 0; n < 64; n++) {
      where = FindLambda(optstr.Lambda[unum][n], lright, optstr.IndexEntries[unum][n]);
      righttab[unum][n] = optstr.SortedIndex[unum][n][where];
      rightbpp += optstr.Bpp[unum][n][righttab[unum][n]];
      righterr += optstr.Err[unum][n][righttab[unum][n]];
    }
  }

  tempdiff = righterr - (*err);
  if (tempdiff < ((RD_float) 0.0)) tempabsdiff = ((RD_float) 0.0)- tempdiff;
  else tempabsdiff = tempdiff;


  if (tempabsdiff < absdiff) {
    diff = tempdiff;
    absdiff = tempabsdiff;
    lmid = lright;
    miderr = righterr;
    midbpp = rightbpp;
    for (unum = 0; unum < job->NumTables; unum++) {
      for (n = 0; n < 64; n++) {
        midtab[unum][n] = righttab[unum][n];
      }
    }
  }

  while (absdiff > optstr.deltamse) {
    if (diff < ((RD_float) 0.0)) {
      /* we are less than target error.. reduce lambda */
      if ((lmid - lright) < optstr.deltalambda) break;
      if (EqualTabs(midtab, righttab, job->NumTables)) break;
      lleft = lmid;
      temptab = lefttab;
      lefttab = midtab;
      midtab = temptab;
      lefterr = miderr;
      leftbpp = midbpp;
    }
    else {
      if ((lleft - lmid) < optstr.deltalambda) break;
      if (EqualTabs(midtab, lefttab, job->NumTables)) break;
      lright = lmid;
      temptab = righttab;
      righttab = midtab;
      midtab = temptab;
      righterr = miderr;
      rightbpp = midbpp;
    }


    lmid = (lleft + lright)/((RD_float) 2.0);
    midbpp = ((RD_float) 0.0);
    miderr = ((RD_float) 0.0);

    for (unum = 0; unum < job->NumTables; unum++) {
      for (n = 0; n < 64; n++) {
        where = FindLambda(optstr.Lambda[unum][n], lmid, optstr.IndexEntries[unum][n]);
        midtab[unum][n] = optstr.SortedIndex[unum][n][where];
        midbpp += optstr.Bpp[unum][n][midtab[unum][n]];
        miderr += optstr.Err[unum][n][midtab[unum][n]];
      }
    }


    diff = miderr - (*err);
    if (diff < ((RD_float) 0.0)) absdiff = ((RD_float) 0.0) - diff;
    else absdiff = diff;
  }
  
  *bpp = midbpp;
  *err = miderr;
  *lambda = lmid;
  for (unum = 0; unum < job->NumTables; unum++) {
      for (n = 0; n < 64; n++) {
        anstab[unum][n] = midtab[unum][n];
      }
  }
  return(TRUE);
}


static void lagrRecoverQandT(RD_Job *job, long (*QandT)[64],
		   long (*Q)[64], long (*T)[64],
		   RD_float (*Bpp)[64], RD_float (*Err)[64])
{
  long unum;
  long n;
  long tspanplus1 = job->ThreshSpan + 1;

  for (unum=0; unum < job->NumTables; unum++) {
    for (n=0;n<64;n++) {
      Q[unum][n] = QandT[unum][n]/tspanplus1;
      T[unum][n] = QandT[unum][n] - (Q[unum][n] * tspanplus1);

      Bpp[unum][n] = job->opt_method.lagr.Bpp[unum][n][QandT[unum][n]];
      Err[unum][n] = (job->opt_method.lagr.Err[unum][n][QandT[unum][n]]/
			job->UnitWeights[unum]);
    }
  }


  for (unum=0; unum < job->NumTables; unum++) {
    for (n=0;n<64;n++) Q[unum][n] = job->QentryToQ(Q[unum][n]);
  }

  for (unum=0; unum < job->NumTables; unum++) {
    for (n=0;n<64;n++) T[unum][n] += Q[unum][n];
  }

  for (unum=0; unum < job->NumTables; unum++) {
    /* convert Err, Bpp */
    for (n=1;n<64;n++) {
      Err[unum][ZigZagToN[n]] += Err[unum][ZigZagToN[n-1]];
      Bpp[unum][ZigZagToN[n]] += Bpp[unum][ZigZagToN[n-1]];
    }
    for (n=0;n<64;n++) {
      Err[unum][n] *= job->opt_method.lagr.ErrScale;
      Err[unum][n] += job->RemainingSignal[unum][n];
      Err[unum][n] /= job->NumPixels[unum];
    }
  }
}

static void lagrGetQforTarget(RD_Job *job, RD_float target, 
  boolean targetIsBpp, boolean PrintStats,
  long *res_size, RD_float *res_bpp,
  RD_float *res_psnr, RD_float *res_rmse,
  DataDest *dd) 
{
  long ResultSize;
  RD_float ResultSNR, ResultPSNR, TargetErr, TargetBpp;
  RD_float temp; 
  long Q[IM_MAXPLANES][64], T[IM_MAXPLANES][64],
      QandT[IM_MAXPLANES][64];
  RD_float ErrDist[IM_MAXPLANES][64], BppDist[IM_MAXPLANES][64];
  int unum; 
  RD_float lambdahigh,lambdalow,lambda;
  int i;

#define optstr job->opt_method.lagr
  
  if (targetIsBpp) { 
    TargetBpp = target - job->CorrectionBpp; 
    i = 0;
    while ((i < RD_NUM_HOOKS) && (optstr.BppHook[i] >= TargetBpp))
	i++;

    if (i == 0) {
	lambdalow = lambdahigh = optstr.LambdaMax;
    } else if (i == RD_NUM_HOOKS) {
        lambdalow = lambdahigh = ((RD_float) 0.0);
    } else {
        lambdahigh = optstr.LambdaHook[i-1];
        lambdalow = optstr.LambdaHook[i];
    }

    RD_LagrFindRate(job, &TargetBpp, &TargetErr, 
         lambdalow, lambdahigh, &lambda, QandT);
  } else { 
    /* target is mse*TotalNumPixels/ErrScale */ 
    i = 0;
    TargetErr = target; 
    while ((i < RD_NUM_HOOKS) && (optstr.ErrHook[i] <= TargetErr))
	i++;

    if (i == 0) {
	lambdalow = lambdahigh = optstr.LambdaMax;
    } else if (i == RD_NUM_HOOKS) {
        lambdalow = lambdahigh = ((RD_float) 0.0);
    } else {
        lambdahigh = optstr.LambdaHook[i-1];
        lambdalow = optstr.LambdaHook[i];
    }

    lagrFindErr(job, &TargetBpp, &TargetErr, 
         lambdalow, lambdahigh, &lambda, QandT);
  }

  (void) DD_printf(dd,"#%s\n",RD_IDSTRING);
  RD_DumpJobChars(job, dd);
  (void) DD_printf(dd,
    "#Bits per pixel for each unit and the whole image\n");
  (void) DD_printf(dd,"#    reported as num-bits/%lf\n",
    ((double) job->NumPixelsForBpp));
  (void) DD_printf(dd,"# R + (lambda*D/%lf) minimized at l = %lf\n#\n",
	optstr.ErrScale, lambda);

  if (job->NumTables==1) {
      (void) DD_printf(dd,
	"#Quantization table for color planes %d thru %d\n",
	     0,ImPlanesOfImg(job->Im)-1);
      lagrRecoverQandT(job,QandT,Q,T,BppDist,ErrDist);
      if (job->ThreshSpan > 0) {
	  RD_WriteLongTable(dd, Q[0], "");
	  RD_WriteThreshTable(dd, T[0], "#T ");
	  if (PrintStats)
	    RD_WriteErrFBppDist(dd, BppDist[0], ErrDist[0], "#D ");
      } else {
	  RD_WriteLongTable(dd, Q[0], "");
	  if (PrintStats)
	    RD_WriteErrFBppDist(dd, BppDist[0], ErrDist[0], "#D ");
      }

      temp = TargetBpp * job->NumPixelsForBpp / 8.0;
      ResultSize = RoundOff(temp);

      if (job->CorrectionBpp != (RD_float) 0) { 
        (void) DD_printf(dd,"#RdoptBpp = %lf bits per pixel\n",
	  ((double)TargetBpp));
        (void) DD_printf(dd,"#RdoptSize = %ld bytes\n", ResultSize);
        (void) DD_printf(dd,"#CorrectionToBpp = %lf bits per pixel\n",
             (double) job->CorrectionBpp); 
        TargetBpp += job->CorrectionBpp; 
        temp = TargetBpp * job->NumPixelsForBpp / 8.0;
        ResultSize = RoundOff(temp);
      }

      ResultSNR = (job->LogTotalSignalSq -
	     ((RD_float) log10( (double) (
	       (TargetErr*optstr.ErrScale)))))*10.0;
      ResultPSNR = (job->LogTotalPeakSq - 
	     ((RD_float) log10( (double) (
	       (TargetErr*optstr.ErrScale)))))*10.0;

      (void) DD_printf(dd,"#Bpp = %lf bits per pixel\n",
	((double) TargetBpp));
      (void) DD_printf(dd,"#Size = %ld bytes\n", ResultSize);
      (void) DD_printf(dd,"#SNR = %lf dB\n", ((double) ResultSNR));
      (void) DD_printf(dd,"#PSNR = %lf dB\n", ((double) ResultPSNR));
      TargetErr *= (optstr.ErrScale/job->TotalNumPixels);
      TargetErr = (RD_float) sqrt((double) TargetErr); 
      (void) DD_printf(dd,"#RMSE = %lf\n", (double) TargetErr);

  } else {

      lagrRecoverQandT(job,QandT,Q,T,BppDist,ErrDist);
      for (unum=0;unum < job->NumTables; unum++) {
        (void) DD_printf(dd,
	  "#Quantization table for color planes %d thru %d\n",
	    unum,((unum==(job->NumTables-1))?
	         (ImPlanesOfImg(job->Im)-1):unum));

        if (job->ThreshSpan > 0) {
          RD_WriteLongTable(dd, Q[unum], "");
          RD_WriteThreshTable(dd, T[unum], "#T ");
          if (PrintStats)
            RD_WriteErrFBppDist(dd, BppDist[unum],ErrDist[unum], "#D ");
        } else {
  	  RD_WriteLongTable(dd, Q[unum], "");
	  if (PrintStats)
	    RD_WriteErrFBppDist(dd, BppDist[unum], ErrDist[unum],"#D ");
        }

	temp = BppDist[unum][63] * job->NumPixelsForBpp / 8.0;
        ResultSize = RoundOff(temp);
        ResultSNR = (job->LogSignalSq[unum] -
	     ((RD_float) log10( (double) (
	       (ErrDist[unum][63]*job->NumPixels[unum])))))*10.0;
        ResultPSNR = (job->LogPeakSq[unum] - 
	     ((RD_float) log10( (double) (
	       (ErrDist[unum][63]*job->NumPixels[unum])))))*10.0;

	(void) DD_printf(dd,"#Bpp = %lf bits per pixel\n",
	    ((double) BppDist[unum][63]));
	(void) DD_printf(dd,"#Size = %ld bytes\n", ResultSize);
	(void) DD_printf(dd,"#SNR = %lf dB\n", ((double) ResultSNR));
	(void) DD_printf(dd,"#PSNR = %lf dB\n", ((double) ResultPSNR));
	(void) DD_printf(dd,"#RMSE = %lf\n", sqrt(ErrDist[unum][63])); 
	(void) DD_printf(dd,"#\n");

     } 
     (void) DD_printf(dd,"#For the entire image:\n");
     temp = TargetBpp * job->NumPixelsForBpp/ 8.0;
     ResultSize = RoundOff(temp);
     ResultSNR = (job->LogTotalSignalSq -
	     ((RD_float) log10( (double) (
	       (TargetErr*optstr.ErrScale)))))*10.0;
     ResultPSNR = (job->LogTotalPeakSq - 
	     ((RD_float) log10( (double) (
	       (TargetErr*optstr.ErrScale)))))*10.0;

     if (job->CorrectionBpp != (RD_float) 0) { 
        (void) DD_printf(dd,"#RdoptBpp = %lf bits per pixel\n",
	  ((double)TargetBpp));
        (void) DD_printf(dd,"#RdoptSize = %ld bytes\n", ResultSize);
        (void) DD_printf(dd,"#CorrectionToBpp = %lf bits per pixel\n",
             (double) job->CorrectionBpp);
        TargetBpp += job->CorrectionBpp; 
        temp = TargetBpp * job->NumPixelsForBpp / 8.0;
        ResultSize = RoundOff(temp);
     }


     (void) DD_printf(dd,"#Bpp = %lf bits per pixel\n",
       ((double) TargetBpp));
     (void) DD_printf(dd,"#Size = %ld bytes\n", ResultSize);
     (void) DD_printf(dd,"#SNR = %lf dB\n", ((double) ResultSNR));
     (void) DD_printf(dd,"#PSNR = %lf dB\n", ((double) ResultPSNR));
     TargetErr = 
       sqrt((double) (TargetErr*optstr.ErrScale/job->TotalNumPixels)); 
     (void) DD_printf(dd,"#RMSE = %lf\n", TargetErr); 
  } 

  (void) DD_printf(dd,"#END\n");
  *res_bpp = TargetBpp;
  *res_size = ResultSize;
  *res_psnr = ResultPSNR;
  *res_rmse = TargetErr; 

}


extern int RD_LagrGetQForBpp(RD_Job *job, RD_float bpp, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd)   
{
  lagrGetQforTarget(job, bpp, 1, PrintStats, 
    res_size, res_bpp, res_psnr, res_rmse, dd);
  return 1; 
} 

extern int RD_LagrGetQForSize(RD_Job *job, long size, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd)   
{
  RD_float TargetBpp = ((RD_float) (size*8.0));
  TargetBpp /= job->NumPixelsForBpp;
  lagrGetQforTarget(job, TargetBpp, 1, PrintStats, 
    res_size, res_bpp, res_psnr, res_rmse, dd);
  return 1; 
} 

extern int RD_LagrGetQForPsnr(RD_Job *job, RD_float psnr, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd)
{
  RD_float TargetErr = job->LogTotalPeakSq;
  TargetErr -= ((RD_float) psnr/10.0);
  TargetErr = ((RD_float) pow(10.0,(double) TargetErr));
  TargetErr /= optstr.ErrScale;
  lagrGetQforTarget(job, TargetErr, 0, PrintStats, 
    res_size, res_bpp, res_psnr, res_rmse, dd);
  return 1; 
} 

extern int RD_LagrGetQForSnr(RD_Job *job, RD_float snr, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd)   
{
  RD_float TargetErr = job->LogTotalSignalSq;
  TargetErr -= ((RD_float) snr/10.0);
  TargetErr = ((RD_float) pow(10.0,(double) TargetErr));
  TargetErr /= optstr.ErrScale;
  lagrGetQforTarget(job, TargetErr, 0, PrintStats, 
    res_size, res_bpp, res_psnr, res_rmse, dd);
  return 1; 
} 

extern int RD_LagrGetQForRmse(RD_Job *job, RD_float rmse, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd)   
{
  RD_float TargetErr = ((RD_float) (rmse*rmse));
  TargetErr *= (job->TotalNumPixels/optstr.ErrScale);
  lagrGetQforTarget(job, TargetErr, 0, PrintStats, 
    res_size, res_bpp, res_psnr, res_rmse, dd);
  return 1; 

}

