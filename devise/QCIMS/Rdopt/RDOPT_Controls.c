
#include "RDOPT.h"
#include "RDOPT_Internal.h"




extern int RD_Init(RD_Job *job, Image *Im)
{
  memset(job, 0, sizeof(RD_Job)); 

  if ((ImStateOfImg(Im) < ImStateReadSome) || 
      (ImStateOfImg(Im) >= ImStateDead) ||
      ((job->SampleBytes = ImAllSampleBytesEqual(Im)) == 0) ||
      (job->SampleBytes > 2) )
    return 0; 


  job->Im = Im; 

  job->MaxSample = (((long) 1)  << (8*job->SampleBytes)) - 1; 

  job->NumTables = ImPlanesOfImg(Im); 

  job->UseLagrangian = TRUE; 

  DD_Init(&job->ErrDD);
  (void) DD_OpenStream(&job->ErrDD, stderr); 

  job->State = RD_STATE_INITED; 

  return 1; 

}

extern int RD_FillDerived(RD_Job * job)
{ 
  long i, h, v, n; 
  RD_float ftemp; 

  if (!job->QentryToQ) {
    if (job->MaxSample < 256L) {
      if (job->Coarseness < 1) RD_SetQMapper(job, RD_QMapperId);
      else if (job->Coarseness==1) RD_SetQMapper(job, RD_QMapper8fast1); 
      else if (job->Coarseness==2) RD_SetQMapper(job, RD_QMapper8fast2); 
      else if (job->Coarseness==3) RD_SetQMapper(job, RD_QMapper8fast3); 
      else if (job->Coarseness==4) RD_SetQMapper(job, RD_QMapper8fast4); 
      else if (job->Coarseness==5) RD_SetQMapper(job, RD_QMapper8fast5); 
      else if (job->Coarseness==6) RD_SetQMapper(job, RD_QMapper8fast6); 
      else if (job->Coarseness==7) RD_SetQMapper(job, RD_QMapper8fast7); 
      else RD_SetQMapper(job, RD_QMapper8fastest); 
    } else if (job->MaxSample < 1024L) { 
      if (job->Coarseness < 1) RD_SetQMapper(job, RD_QMapper10);
      else if (job->Coarseness==1) RD_SetQMapper(job, RD_QMapper10fast1); 
      else if (job->Coarseness==2) RD_SetQMapper(job, RD_QMapper10fast2); 
      else if (job->Coarseness==3) RD_SetQMapper(job, RD_QMapper10fast3); 
      else if (job->Coarseness==4) RD_SetQMapper(job, RD_QMapper10fast4); 
      else RD_SetQMapper(job, RD_QMapper10fastest); 
    } else if (job->MaxSample < 4096L) { 
      if (job->Coarseness < 1) RD_SetQMapper(job, RD_QMapper12);
      else RD_SetQMapper(job, RD_QMapper12fastest); 
    } else { 
      if (job->Coarseness < 1) RD_SetQMapper(job, RD_QMapper16);
      else RD_SetQMapper(job, RD_QMapper16fastest); 
    } 
  } 

  
  for (i=0;i<job->NumTables;i++) {
    if (!job->MinTableExists[i]) {
      for (n=0;n<64;n++) {
	job->MinTable[i][n] = (RD_Qentry) 1;
      }
    }
    if (!job->MaxTableExists[i]) {
      for (n=0;n<64;n++) {
	job->MaxTable[i][n] = (RD_Qentry) RD_QENTRY_MAX; 
      }
    }
  } 

  if (job->DCclamp > 0) {
    for (i=0;i<job->NumTables;i++)
      if (job->MaxTable[i][0] > job->DCclamp)
        job->MaxTable[i][0] = job->DCclamp; 
  } 
  for (i=0;i<job->NumTables;i++) {
    for (n=0;n<64;n++) {
	if (job->MinTable[i][n] > job->MaxTable[i][n]) 
	    job->MinTable[i][n] = job->MaxTable[i][n];  
	job->MinTable[i][n] = (long) RD_QToQentry(job->MinTable[i][n], 
			 job->QentryToQ, FALSE); /* round down */ 
	job->MaxTable[i][n] = (long) RD_QToQentry(job->MaxTable[i][n], 
			 job->QentryToQ, TRUE);  /* round up */ 
    } 
  }


  for (i=0;i<ImPlanesOfImg(job->Im);i++) {
    h = ImCurrPlaneCols(job->Im,i)/8; 
    if ((h*8) < ImCurrPlaneCols(job->Im,i)) h++; 
    v = ImCurrPlaneRows(job->Im,i)/8; 
    if ((v*8) < ImCurrPlaneRows(job->Im,i)) v++; 
    job->NumBlocks[i] = ((RD_float) (((long) h)*v));
  }

  job->NumPixelsForBpp =
      job->NumBlocks[0]*((RD_float) 64.0);

  for (i=job->NumTables;i<ImPlanesOfImg(job->Im);i++) {
    job->NumBlocks[job->NumTables-1] += job->NumBlocks[i];
  }

  ftemp = ((RD_float) log10(((double) job->MaxSample)
				       * job->MaxSample)); 
  job->TotalNumPixels = 0.0;
  for (i=0; i<job->NumTables; i++) {
    job->NumPixels[i] = job->NumBlocks[i] * 64.0;
    job->LogPeakSq[i] = ftemp +
		((RD_float) log10((double) job->NumPixels[i]));
    job->TotalNumPixels += job->NumPixels[i];
  }
  job->LogTotalPeakSq = ftemp +
		   ((RD_float) log10((double) job->TotalNumPixels));


  for (i=0; i<job->NumTables; i++) {
    job->bppWeight[i] = job->NumPixels[i]/job->NumPixelsForBpp;
  } 

  if (!job->UnitWeightsExist) {
    for (i=0;i<job->NumTables;i++)
      job->UnitWeights[i] = ((RD_float) 1.0);
  } else {
    /* normalize weights */ 
    ftemp = ((RD_float) 0.0);
    for (i=0;i<job->NumTables;i++) {
      ftemp += job->UnitWeights[i]; /* each weight already checked
				       to be > 0 */ 
    }
    ftemp /= ((RD_float) job->NumTables);
    for (i=0;i<job->NumTables;i++) {
      job->UnitWeights[i] /= ftemp;
    }
  } 

  if (job->UseLagrangian) {   
    job->opt_method.lagr.ErrScale = job->NumPixelsForBpp;
  } else {
    if (job->opt_method.dp.BppMax <= 0) 
      job->opt_method.dp.BppMax = 1.5;
    if (job->opt_method.dp.BppScale <= 0) 
      job->opt_method.dp.BppScale = 5000;
    job->opt_method.dp.TableRowSize =
      RoundOff(job->opt_method.dp.BppMax *
		((RD_float) job->opt_method.dp.BppScale)) + 2;
  } 

  for (i=0;i<job->NumTables;i++) {
    if (job->WeightedCoefs[i]) {
      /* normalize weights: all weights already >= zero */ 
      ftemp = ((RD_float) 0.0);
      for (n=0;n<64;n++) ftemp += job->CoefWeights[i][n];
      ftemp /= ((RD_float) 64.0);
      if (ftemp == (RD_float) 0) {
	job->CoefWeights[i][0] = (RD_float) 1.0;
      } else { 
        for (n=0;n<64;n++) job->CoefWeights[i][n] /= ftemp;
      } 
    }
  }

  return 1;

}

extern int RD_SetThreshSpan(RD_Job *job, long thspan)
{
  if (job->State != RD_STATE_INITED)
    RD_NonFatalError("SetThreshSpan: can be called after Init only"); 
  if (thspan < 0) thspan = 0;
  if (thspan > RD_QENTRY_MAX) thspan = RD_QENTRY_MAX;
  job->ThreshSpan = thspan; 
  return 1; 
}

extern int RD_SetCoarseness(RD_Job *job, int coarseness)
{
  if (job->State != RD_STATE_INITED)
    RD_NonFatalError("SetCoarseness: can be called after Init only"); 
  if (coarseness < 0) coarseness = 0;
  job->Coarseness = coarseness; 
  return 1; 
}

extern void RD_SetVerboseLevel(RD_Job *job, int verb)
{
  job->VerboseLevel = verb; 
}

extern int RD_SetDCDPCM(RD_Job *job, boolean on)
{
  if (job->State != RD_STATE_INITED)
    RD_NonFatalError("SetDCDPCM: can be called after Init only"); 
  job->UseDCDPCM = on; 
  return 1; 
}

extern int RD_SetOptMethod(RD_Job *job, char * method)
{
  if (job->State != RD_STATE_INITED)
    RD_NonFatalError("SetOptMethod: can be called after Init only"); 
  if (!method || !strncasecmp(method,"lagrangian",1)) {
    job->UseLagrangian = TRUE; 
  } else {
    job->UseLagrangian = FALSE; 
  }
  return 1; 
}

extern int RD_SetMaxSample(RD_Job *job, long maxs)
{
  if (job->State != RD_STATE_INITED)
    RD_NonFatalError("SetMaxSample: can be called after Init only"); 
  if (maxs <= 0)
    RD_NonFatalError("SetMaxSample: needs positive value");
  job->MaxSample = maxs; 
  return 1; 
}

extern int RD_SetQMapper(RD_Job *job, RD_QMapper qmfunc)
{
  if (job->State != RD_STATE_INITED)
    RD_NonFatalError("SetQMapper: can be called after Init only"); 
  job->QentryToQ = qmfunc;
  return 1; 
}

extern int RD_SetBppMax(RD_Job *job, RD_float bppmax)
{
  if (job->State != RD_STATE_INITED)
    RD_NonFatalError("SetBppMax: can be called after Init only"); 
  job->opt_method.dp.BppMax = bppmax; 
  return 1; 
}

extern int RD_SetBppScale(RD_Job *job, long bppscale)
{ 
  if (job->State != RD_STATE_INITED)
    RD_NonFatalError("SetBppScale: can be called after Init only"); 
  job->opt_method.dp.BppScale = bppscale; 
  return 1; 
}

extern int RD_SetUnitWeights(RD_Job *job, int nweights, RD_float *wts)
{
  int i; 
  if (job->State != RD_STATE_INITED)
    RD_NonFatalError("SetUnitWeights: can be called after Init only"); 
  if (nweights > ImPlanesOfImg(job->Im))
    nweights = ImPlanesOfImg(job->Im);
  for (i=0;i<nweights;i++) {
    if ((job->UnitWeights[i] = wts[i]) < ((RD_float) 0))
      RD_NonFatalError("SetUnitWeights: needs non-negative weights"); 
  }
  for (i=nweights;i<ImPlanesOfImg(job->Im);i++) {
    job->UnitWeights[i] = wts[nweights-1]; 
  }
  job->UnitWeightsExist = TRUE; 
  return 1; 

}

extern int RD_SetDCclamp(RD_Job *job, long dcc)
{
  if (job->State != RD_STATE_INITED)
    RD_NonFatalError("SetDCclamp: can be called after Init only"); 
  if (dcc <= 0) 
    RD_NonFatalError("SetDCclamp: needs positive value");
  job->DCclamp = dcc;
  return 1; 
}

extern int RD_SetMinTables(RD_Job *job, int startunit,
   void *src, long srcbytes, int srckind)
{
  DataSrc dsrc, *ds = &dsrc; 
  long tab[64];
  int i, n, ret = 0; 
  if (job->State != RD_STATE_INITED)
    RD_NonFatalError("SetMinTables: can be called after Init only"); 
  DS_Init(ds, 0); 
  if ((srckind == DS_KIND_FILE) || (srckind == DS_KIND_WEB))
    ret = DS_OpenFile(ds, (char *) src, srcbytes); 
  else if (srckind == DS_KIND_MEMORY) 
    ret = DS_OpenMemory(ds, (char *) src, srcbytes); 
  else if (src && (srckind == DS_KIND_FD) ) 
    ret = DS_OpenFd(ds, *((int *) src), srcbytes); 
  else if (srckind == DS_KIND_STREAM) 
    ret = DS_OpenStream(ds, (FILE *) src, srcbytes); 
  if (!ret) 
    RD_NonFatalError("SetMinTables: error opening source"); 

  i = startunit;
  while ((i < ImPlanesOfImg(job->Im)) && RD_ReadLongTable(ds, tab, 0)) {
    for (n=0;n<64;n++) {
      if ((job->MinTable[i][n] = tab[n]) <= 0)
	job->MinTable[i][n] = 1; 
    } 
    job->MinTableExists[i] = TRUE; 
    i++; 
  }

  DS_Close(ds);
  return 1; 
}

extern int RD_SetMaxTables(RD_Job *job, int startunit,
   void *src, long srcbytes, int srckind)
{
  DataSrc dsrc, *ds = &dsrc; 
  long tab[64];
  int i, n, ret = 0; 
  if (job->State != RD_STATE_INITED)
    RD_NonFatalError("SetMaxTables: can be called after Init only"); 
  DS_Init(ds, 0); 
  if ((srckind == DS_KIND_FILE) || (srckind == DS_KIND_WEB))
    ret = DS_OpenFile(ds, (char *) src, srcbytes); 
  else if (srckind == DS_KIND_MEMORY) 
    ret = DS_OpenMemory(ds, (char *) src, srcbytes); 
  else if (src && (srckind == DS_KIND_FD) ) 
    ret = DS_OpenFd(ds, *((int *) src), srcbytes); 
  else if (srckind == DS_KIND_STREAM) 
    ret = DS_OpenStream(ds, (FILE *) src, srcbytes); 
  if (!ret) 
    RD_NonFatalError("SetMaxTables: error opening source"); 

  i = startunit;
  while ((i < ImPlanesOfImg(job->Im)) && RD_ReadLongTable(ds, tab, 0)) {
    for (n=0;n<64;n++) {
      if ((job->MaxTable[i][n] = tab[n]) <= 0)
	job->MaxTable[i][n] = (long) RD_QENTRY_MAX; 
    } 
    job->MaxTableExists[i] = TRUE; 
    i++; 
  }

  DS_Close(ds);
  return 1; 
}

extern int RD_SetCoefWeights(RD_Job *job, int startunit,
   void *src, long srcbytes, int srckind)
{
  DataSrc dsrc, *ds = &dsrc; 
  RD_float tab[64];
  int i, n, ret = 0; 
  if (job->State != RD_STATE_INITED)
    RD_NonFatalError("SetCoefWeights: can be called after Init only"); 
  DS_Init(ds, 0); 
  if ((srckind == DS_KIND_FILE) || (srckind == DS_KIND_WEB))
    ret = DS_OpenFile(ds, (char *) src, srcbytes); 
  else if (srckind == DS_KIND_MEMORY) 
    ret = DS_OpenMemory(ds, (char *) src, srcbytes); 
  else if (src && (srckind == DS_KIND_FD) ) 
    ret = DS_OpenFd(ds, *((int *) src), srcbytes); 
  else if (srckind == DS_KIND_STREAM) 
    ret = DS_OpenStream(ds, (FILE *) src, srcbytes); 
  if (!ret) 
    RD_NonFatalError("SetCoefWeights: error opening source"); 

  i = startunit;
  while ((i < ImPlanesOfImg(job->Im)) && RD_ReadFloatTable(ds,tab,0)) {
    for (n=0;n<64;n++) {
      if ((job->CoefWeights[i][n] = tab[n]) < (RD_float) 0)
	job->CoefWeights[i][n] = (RD_float) 0; 
    } 
    job->WeightedCoefs[i] = TRUE; 
    i++; 
  }

  DS_Close(ds);
  return 1; 
}

extern int RD_SetNumTables(RD_Job *job, int numtables)
{
  if (job->State != RD_STATE_INITED)
    RD_NonFatalError("SetNumTables: can be called after Init only"); 
  if (numtables <= 0)  
    RD_NonFatalError("SetNumTables: needs positive value");
  if (numtables > ImPlanesOfImg(job->Im))  
    numtables = ImPlanesOfImg(job->Im); 
  job->NumTables = numtables; 
  return 1; 
}

extern int RD_Optimize(RD_Job *job)
{
  int UnitNum, EndPlaneNum, n, i;
  RD_float Stemp[64];
  

  if (job->State != RD_STATE_INITED) 
    RD_NonFatalError("Optimize: can be called after Init only"); 

  if (!RD_FillDerived(job))
    RD_FatalError("Optimize: FillDerived failed");

  job->State = RD_STATE_OPTIMIZING; 

  for (UnitNum=0; UnitNum < job->NumTables; UnitNum++) {

    EndPlaneNum = (UnitNum == (job->NumTables-1)) ? 
		    ImPlanesOfImg(job->Im) - 1 : UnitNum;

    if (!RD_InitHistogram(job)) return 0; 
    job->LogSignalSq[UnitNum] = 0.0;
    for (n=0;n<64;n++) Stemp[n] = ((RD_float) 0.0);

    if (job->VerboseLevel)  
      (void) DD_printf(&job->ErrDD,"Unit %d: color planes %d through %d\n",
		       UnitNum,UnitNum,EndPlaneNum); 

    for (i=UnitNum; i<=EndPlaneNum; i++) {
      if (job->VerboseLevel)  
        (void) DD_printf(&job->ErrDD,
	  "\tComputing statistics for image color plane #%d\n", i);
      if (!RD_SetHistogram(job, i, &job->LogSignalSq[UnitNum], Stemp)) 
	  return 0; 
    } 

    if (job->WeightedCoefs[UnitNum]) {
      for (i=0;i<64;i++)
      Stemp[i] *= job->CoefWeights[UnitNum][i];
    } 

    RD_SigToRemainingSig(Stemp, job->RemainingSignal[UnitNum]);
    
    for (n=0;n<64;n++) {
      while ((job->Histogram[n].PlusSize > 1) &&
	      !job->Histogram[n].PlusCount[job->Histogram[n].PlusSize-1])
	job->Histogram[n].PlusSize--;
      while ((job->Histogram[n].MinusSize > 1) &&
	      !job->Histogram[n].MinusCount[job->Histogram[n].MinusSize-1])
	job->Histogram[n].MinusSize--;
    }


    /*********** fill ERR and BPP **********************/

    if (!job->UseLagrangian) {

      if (!RD_PrepareForErrBpp(job,UnitNum)) return 0; 

      if (job->VerboseLevel)  
        (void) DD_printf(&job->ErrDD, "\tFilling errors and entropies\n");
      if (!RD_SetErrBpp(job,UnitNum)) return 0; 

      RD_FreeHistogram(job);

      if (job->VerboseLevel)  
        (void) DD_printf(&job->ErrDD, "\tRunning DP optimization algorithm\n"); 
      if (!RD_OptimizeUnit(job,UnitNum)) return 0; 

    } else {

      if (!RD_LagrPrepareForErrBpp(job,UnitNum)) return 0; 

      if (job->VerboseLevel)  
        (void) DD_printf(&job->ErrDD, "\tFilling errors and entropies\n");
      if (!RD_LagrSetErrBpp(job,UnitNum)) return 0;

      RD_FreeHistogram(job);

      if (job->VerboseLevel)  
        (void) DD_printf(&job->ErrDD, "\tSorting R-D tables\n");
      if (!RD_LagrSortErrBpp(job,UnitNum)) return 0; 

    }
  }

  if (!job->UseLagrangian) {
    /*********************** combine units *****************/
    if (job->NumTables > 1) {
      if (job->VerboseLevel)  
        (void) DD_printf(&job->ErrDD, "Combining information from all units\n");
      if (!RD_CombineUnits(job)) return 0; 
    } 
    RD_Epilogue(job);
  } else {
    RD_LagrEpilogue(job);
  } 
  job->State = RD_STATE_OPTDONE; 
  return 1; 
}

extern int RD_GetQForBpp(RD_Job *job, RD_float bpp, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd)   
{ 
  DataDest ddest; 
  if (job->State != RD_STATE_OPTDONE) return 0; 
  if (!dd) {
    dd = &ddest;
    DD_Init(dd);
  } 
  if (job->UseLagrangian) { 
    return RD_LagrGetQForBpp(job, bpp, PrintStats,
      res_size, res_bpp, res_psnr, res_rmse, dd); 
  } else {
    return RD_DP_GetQForBpp(job, bpp, PrintStats,
      res_size, res_bpp, res_psnr, res_rmse, dd); 
  } 
} 

extern int RD_GetQForSize(RD_Job *job, long size, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd)   
{ 
  DataDest ddest; 
  if (job->State != RD_STATE_OPTDONE) return 0; 
  if (!dd) {
    dd = &ddest;
    DD_Init(dd);
  } 
  if (job->UseLagrangian) { 
    return RD_LagrGetQForSize(job, size, PrintStats,
      res_size, res_bpp, res_psnr, res_rmse, dd); 
  } else {
    return RD_DP_GetQForSize(job, size, PrintStats,
      res_size, res_bpp, res_psnr, res_rmse, dd); 
  } 
} 

extern int RD_GetQForPsnr(RD_Job *job, RD_float psnr, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd)   
{ 
  DataDest ddest; 
  if (job->State != RD_STATE_OPTDONE) return 0; 
  if (!dd) {
    dd = &ddest;
    DD_Init(dd);
  } 
  if (job->UseLagrangian) { 
    return RD_LagrGetQForPsnr(job, psnr, PrintStats,
      res_size, res_bpp, res_psnr, res_rmse, dd); 
  } else {
    return RD_DP_GetQForPsnr(job, psnr, PrintStats,
      res_size, res_bpp, res_psnr, res_rmse, dd); 
  } 
} 

extern int RD_GetQForSnr(RD_Job *job, RD_float snr, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd)   
{ 
  DataDest ddest; 
  if (job->State != RD_STATE_OPTDONE) return 0; 
  if (!dd) {
    dd = &ddest;
    DD_Init(dd);
  } 
  if (job->UseLagrangian) { 
    return RD_LagrGetQForSnr(job, snr, PrintStats,
      res_size, res_bpp, res_psnr, res_rmse, dd); 
  } else {
    return RD_DP_GetQForSnr(job, snr, PrintStats,
      res_size, res_bpp, res_psnr, res_rmse, dd); 
  } 
} 

extern int RD_GetQForRmse(RD_Job *job, RD_float rmse, 
  boolean PrintStats, 
  long *res_size, RD_float *res_bpp, RD_float *res_psnr,
  RD_float *res_rmse, DataDest *dd)   
{ 
  DataDest ddest; 
  if (job->State != RD_STATE_OPTDONE) return 0; 
  if (!dd) {
    dd = &ddest;
    DD_Init(dd);
  } 
  if (job->UseLagrangian) { 
    return RD_LagrGetQForRmse(job, rmse, PrintStats,
      res_size, res_bpp, res_psnr, res_rmse, dd); 
  } else {
    return RD_DP_GetQForRmse(job, rmse, PrintStats,
      res_size, res_bpp, res_psnr, res_rmse, dd); 
  } 
} 

extern int RD_GetFirstPoint(RD_Job *job, 
  long *res_size, RD_float *res_bpp,
  RD_float *res_psnr, RD_float *res_rmse) 
{ 
  DataDest ddest, *dd; 
  if (job->State != RD_STATE_OPTDONE) return 0; 
  dd = &ddest;
  DD_Init(dd);
  return RD_GetQForBpp(job, (RD_float) 0, 0, 
      res_size, res_bpp, res_psnr, res_rmse, dd); 
} 

extern int RD_GetLastPoint(RD_Job *job, 
  long *res_size, RD_float *res_bpp,
  RD_float *res_psnr, RD_float *res_rmse) 
{ 
  DataDest ddest, *dd; 
  if (job->State != RD_STATE_OPTDONE) return 0; 
  dd = &ddest;
  DD_Init(dd);
  return RD_GetQForBpp(job,
      (RD_float) (job->SampleBytes*ImPlanesOfImg(job->Im)*8), 0, 
      res_size, res_bpp, res_psnr, res_rmse, dd); 
} 


extern int RD_BppPsnrCurve(RD_Job *job, 
   RD_float minbpp, RD_float maxbpp, int npoints, 
   boolean PrintStats, boolean printbpps,  
   DataDest *dd, char *qfile_prefix, int do_compress,
   void *ssrc, long ssrcbytes, int ssrckind) 
{ 
  RD_float rdminbpp, rdmaxbpp, bincr, bpp, ansbpp = 0; 
  long tsize = 0, act_size = 0;
  RD_float tpsnr = 0, trmse = 0; 
  DataDest ddest, qdest, *qdd; 
  char qfname[RD_STRLENMAX]; 
  int i; 
  DataDest tempddest, *tdd = &tempddest; 
  void *qtsrc = 0;
  long qtsrcbytes = 0;
  int qtsrckind = 0;


  if (job->State != RD_STATE_OPTDONE) return 0; 
  if (!dd) {
    dd = &ddest;
    DD_Init(dd);
  } 
  if (!RD_GetFirstPoint(job, &tsize, &rdminbpp, &tpsnr, &trmse) )
    return 0; 
  if (!RD_GetLastPoint(job, &tsize, &rdmaxbpp, &tpsnr, &trmse) )
    return 0; 
  if (minbpp < rdminbpp) minbpp = rdminbpp; 
  if (maxbpp > rdmaxbpp) maxbpp = rdmaxbpp; 
  if (maxbpp < minbpp) maxbpp = minbpp; 
  if (npoints < 2) npoints = 2; 


  bincr = (maxbpp - minbpp)/((RD_float) (npoints-1)); 
  bpp = minbpp; 
  
  if (printbpps) {
    if (!DD_printf(dd,"# Bpp      Size       PSNR   Qfile\n"))
      return 0; 
  } else {
    if (!DD_printf(dd,"# Size       PSNR   Qfile\n"))
      return 0; 
  }

  for (i=0;i<npoints;i++) { 
    if (i==(npoints-1)) bpp = maxbpp; 

    qdd = 0;

    if (!qfile_prefix) {
      if (do_compress) { 
        qdd = &qdest; 
        DD_Init(qdd);
	DD_Use_Auto_Mem(qdd);
	DD_OpenMemory(qdd, 0, 0);
        strcpy(qfname,"");
      } else { 
        qdd = 0; 
        strcpy(qfname,"");
      } 
    } else {
      qdd = &qdest; 
      DD_Init(qdd);
      sprintf(qfname,"%s.%d", qfile_prefix, i); 
      (void) DD_OpenFile(qdd, qfname); 
    } 
    (void) RD_GetQForBpp(job, bpp, PrintStats, &tsize, &ansbpp,
	   &tpsnr, &trmse, qdd); 
    qtsrc = 0; 
    if (qdd) {
      if (do_compress) {
	if (!qfile_prefix) {
	  qtsrc = DD_auto_mem(qdd);
	  qtsrcbytes = DD_auto_mem_level(qdd);
	  qtsrckind = DS_KIND_MEMORY; 
	} else {
	  qtsrc = qfname; 
	  qtsrcbytes = 0; 
	  qtsrckind = DS_KIND_FILE; 
	} 
      } 
      DD_Close(qdd); 
    }  
    #ifdef HAVE_JPEGLIB
    if (do_compress) {
       DD_Init(tdd);
       DD_OpenNull(tdd); 
       act_size = ImSendJpeg(job->Im, 1, 
              0, 0, 0, 0, 0, tdd, 0, 0, 0, 0, 0, 
	      qtsrc, qtsrcbytes, qtsrckind, 
              ssrc, ssrcbytes, ssrckind); 
       DD_Close(tdd); 
       if (act_size > 0) {
	 tsize = act_size;
	 ansbpp = ((RD_float) tsize*8)/job->NumPixelsForBpp; 
       } 
    }
    #endif

    if (printbpps) { 
      (void) DD_printf(dd, "%lf %7ld %lf %s\n",
        (double) ansbpp, tsize, (double) tpsnr, qfname); 
    } else {
      (void) DD_printf(dd, "%7ld %lf %s\n",
        tsize, (double) tpsnr, qfname); 
    }
    if (qtsrckind == DS_KIND_MEMORY) RD_FREE(qtsrc); 

    bpp += bincr; 
  }
  return 1; 
}


extern void RD_Free(RD_Job *job)
{
  int n, unum; 
  if ( (job->State == RD_STATE_OPTIMIZING) ||
       (job->State == RD_STATE_OPTDONE) ) {
    RD_FreeHistogram(job); 
    if (job->UseLagrangian) {
      for (unum=0;unum<job->NumTables;unum++) { 
        RD_FREE(job->opt_method.lagr.Err[unum][0]); 
        RD_FREE(job->opt_method.lagr.Bpp[unum][0]); 
        RD_FREE(job->opt_method.lagr.Lambda[unum][0]);  
        RD_FREE(job->opt_method.lagr.SortedIndex[unum][0]);  
      } 
    } else {
      RD_FREE(job->opt_method.dp.CombinedLastRow); 
      RD_FREE(job->opt_method.dp.ToCombine[1]); 
      for (unum=0;unum<job->NumTables;unum++) { 
        RD_FREE(job->opt_method.dp.LastRow[unum]); 
        RD_FREE(job->opt_method.dp.Err[unum][0]); 
        RD_FREE(job->opt_method.dp.Bpp[unum][0]); 
        RD_FREE(job->opt_method.dp.QChoice[unum][0]); 
        RD_FREE(job->opt_method.dp.TChoice[unum][0]); 
      }
      for (n=0;n<64;n++) {
	RD_FREE(job->opt_method.dp.QforBpp[n]); 
	RD_FREE(job->opt_method.dp.TforBpp[n]); 
      } 
    } 
    DD_Close(&job->ErrDD); 
  } else if (job->State == RD_STATE_INITED) { 
    DD_Close(&job->ErrDD); 
  }
  job->State = RD_STATE_DEAD;  

}


extern int RD_SetErrDest(RD_Job *job, void *dest, int destkind) 
{
  if ( (job->State != RD_STATE_INITED) && 
       (job->State != RD_STATE_OPTDONE)) return 0;
  if (destkind==DD_KIND_FILE) { 
    return DD_OpenFile(&job->ErrDD, (char *) dest);
  } else if (destkind==DD_KIND_STREAM) { 
    return DD_OpenStream(&job->ErrDD, (FILE *) dest);
  } else if ((destkind==DD_KIND_FD) && dest) { 
    return DD_OpenFd(&job->ErrDD, *((int *) dest));
  } 
  return 0; 
}

extern int RD_ApplyCorrection(RD_Job *job, RD_float add_bpp)
{
  if (job->State != RD_STATE_OPTDONE) return 0;
  job->CorrectionBpp = add_bpp; 
  return 1; 
}



extern int RD_FillQclicCurve(RD_Job *job, 
   RD_float minbpp, RD_float maxbpp, int npoints, 
   char *qfile_prefix, int do_compress,
   void *ssrc, long ssrcbytes, int ssrckind)
{
  int i, j; 
  QclicInfo *qinfo; 
  RD_float bpp, bincr,  ansbpp, trmse, tpsnr, rdminbpp, rdmaxbpp; 
  long tsize; 
  DataDest tempddest, *tdd = &tempddest; 


  DataDest qdest, *qdd; 
  char qfname[RD_STRLENMAX]; 

  if (!qfile_prefix) return 0; 
  if (job->State != RD_STATE_OPTDONE) return 0;
  if ( (npoints <= 1) || (minbpp >= maxbpp) ) return 0; 

  qinfo = &(job->Im->qclic); 

  RD_FREE(qinfo->curve_size); 
  RD_FREE(qinfo->curve_better_size); 
  RD_FREE(qinfo->curve_quality); 
  if ((qinfo->curve_points > 0) && qinfo->curve_parfiles) { 
    for (i=0;i<qinfo->curve_points;i++) 
      RD_FREE(qinfo->curve_parfiles[i]);
  }
  RD_FREE(qinfo->curve_parfiles);

  qinfo->curve_points = npoints; 

  qinfo->curve_size = (long *) calloc(1,sizeof(long)*npoints); 
  qinfo->curve_better_size = (long *) calloc(1,sizeof(long)*npoints); 
  qinfo->curve_quality = (float *) calloc(1,sizeof(float)*npoints); 
  qinfo->curve_parfiles = (char **) calloc(1,sizeof(char *)*npoints); 

  if (!qinfo->curve_size || !qinfo->curve_better_size || 
      !qinfo->curve_quality || !qinfo->curve_parfiles ) {
    RD_FREE(qinfo->curve_size); 
    RD_FREE(qinfo->curve_better_size); 
    RD_FREE(qinfo->curve_quality); 
    RD_FREE(qinfo->curve_parfiles); 
    qinfo->curve_points = 0; 
    return 0; 
  } 
  for (i=0;i<npoints;i++) {
    if ((qinfo->curve_parfiles[i] = (char *) 
       calloc(1,sizeof(char)*QCLIC_STRLENMAX)) == NULL) {
      for (j=0;j<i;j++) RD_FREE(qinfo->curve_parfiles[j]); 
      RD_FREE(qinfo->curve_size); 
      RD_FREE(qinfo->curve_better_size); 
      RD_FREE(qinfo->curve_quality); 
      RD_FREE(qinfo->curve_parfiles); 
      qinfo->curve_points = 0; 
      return 0; 
    } 
  } 

  qinfo->curve_points = npoints; 

  rdminbpp = 0; rdmaxbpp = 24.0; 
  (void) RD_GetFirstPoint(job, &tsize, &rdminbpp, &tpsnr, &trmse); 
  (void) RD_GetLastPoint(job, &tsize, &rdmaxbpp, &tpsnr, &trmse); 
  if (minbpp < rdminbpp) minbpp = rdminbpp; 
  if (maxbpp > rdmaxbpp) maxbpp = rdmaxbpp; 
  if (maxbpp < minbpp) maxbpp = minbpp; 


  bincr = (maxbpp - minbpp)/((RD_float) (npoints-1)); 
  bpp = minbpp; 
  
  for (i=0;i<npoints;i++) { 
    if (i==(npoints-1)) bpp = maxbpp; 

    qdd = &qdest; 
    DD_Init(qdd);
    sprintf(qfname,"%s.%d", qfile_prefix, i); 
    (void) DD_OpenFile(qdd, qfname); 
    (void) RD_GetQForBpp(job, bpp, 1, &qinfo->curve_size[i], &ansbpp,
	   &tpsnr, &trmse, qdd); 
    qinfo->curve_quality[i] = (float) tpsnr; 
    strcpy(qinfo->curve_parfiles[i],qfname);
    DD_Close(qdd); 
    
    #ifdef HAVE_JPEGLIB
    if (do_compress) {
       DD_Init(tdd);
       DD_OpenNull(tdd); 
       qinfo->curve_better_size[i] = ImSendJpeg(job->Im, 1, 
              0, 0, 0, 0, 0, tdd, 0, 0, 0, 0, 0, 
              qfname, 0, DS_KIND_FILE, 
              ssrc, ssrcbytes, ssrckind); 
       DD_Close(tdd); 
    }
    #endif

    bpp += bincr; 
  } 
  
  if (ImPlanesOfImg(job->Im) == 1) 
    strcpy(qinfo->curve_qmetric, Qmetric(QM_PSNR));
  else 
    strcpy(qinfo->curve_qmetric, Qmetric(QM_AVG_PSNR));
  strcpy(qinfo->curve_format,"JPEG"); 
  return 1; 
} 


