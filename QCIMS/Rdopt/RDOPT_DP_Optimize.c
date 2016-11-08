#include "RDOPT.h"
#include "RDOPT_Internal.h"

static void ResetRow(RD_float *row, int width)
{
  int i;
  for (i=0;i<width;i++) row[i] = RD_INFINITY;
}

extern int RD_OptimizeUnit(RD_Job *job, int unum)
{
  RD_float *CurrRow, *PrevRow, *TempRow;
  long LeastLast, MostLast, i, Loc, Loc2, n, q, k;
  long t2, indx, tspanplus1 = job->ThreshSpan+1;
  RD_float ErrTemp;

  
  /****** allocate ******/
  if ((CurrRow = (RD_float *)
         calloc(1,job->opt_method.dp.TableRowSize*sizeof(RD_float)))
	   ==NULL) {
    RD_FatalError("Optimize out of memory");
  }
  if ((PrevRow = (RD_float *)
	 calloc(1,job->opt_method.dp.TableRowSize*sizeof(RD_float)))
	   ==NULL) {
    RD_FREE(CurrRow); 
    RD_FatalError("Optimize out of memory");
  }

  ResetRow(CurrRow,job->opt_method.dp.TableRowSize);
  ResetRow(PrevRow,job->opt_method.dp.TableRowSize);

  if ((job->opt_method.dp.QChoice[unum][0] = (RD_Qentry *) 
	 calloc(1,job->opt_method.dp.TableRowSize*64*sizeof(RD_Qentry)))
	   ==NULL) {
    RD_FREE(CurrRow); 
    RD_FREE(PrevRow); 
    RD_FatalError("Optimize out of memory");
  }

  for (n=1;n<64;n++) {
    job->opt_method.dp.QChoice[unum][n] =
      job->opt_method.dp.QChoice[unum][n-1] + 
      job->opt_method.dp.TableRowSize;
  }
  
  if (job->ThreshSpan > 0) {

    if ((job->opt_method.dp.TChoice[unum][0] = (RD_Qentry *) 
	 calloc(1,job->opt_method.dp.TableRowSize*64*sizeof(RD_Qentry)))
	   ==NULL) {
      RD_FREE(CurrRow); 
      RD_FREE(PrevRow); 
      RD_FatalError("Optimize out of memory");
    }

    for (n=1;n<64;n++) {
      job->opt_method.dp.TChoice[unum][n] =
	job->opt_method.dp.TChoice[unum][n-1] + 
	    job->opt_method.dp.TableRowSize;
    }
  
    if (job->opt_method.dp.ErrEncodesRow[unum][63]) {
      if (job->VerboseLevel > 2)
	(void) DD_printf(&job->ErrDD, "\t\tCalculating row for compacted coeff #63\n");

      Loc2 = 0;
      for (Loc = job->opt_method.dp.BppOffsetInEncoding[unum][63];
  	 Loc <= job->opt_method.dp.BppMaxInEncoding[unum][63];
	 Loc++,Loc2++) {
        PrevRow[Loc] = job->opt_method.dp.Err[unum][63][Loc2];
        job->opt_method.dp.QChoice[unum][63][Loc] =
  	  job->opt_method.dp.QforBpp[63][Loc2];
        job->opt_method.dp.TChoice[unum][63][Loc] =
          job->opt_method.dp.TforBpp[63][Loc2];
      }
      RD_FREE(job->opt_method.dp.QforBpp[63]);
      RD_FREE(job->opt_method.dp.TforBpp[63]);
      LeastLast = job->opt_method.dp.BppOffsetInEncoding[unum][63];
      MostLast = job->opt_method.dp.BppMaxInEncoding[unum][63];
    } else {
      if (job->VerboseLevel > 2)
	(void) DD_printf(&job->ErrDD, "\t\tCalculating row for coeff #63\n");
      LeastLast = job->opt_method.dp.TableRowSize-1;
      MostLast = 0;
      indx = job->MinTable[unum][63]*tspanplus1;
      for (q=job->MinTable[unum][63]; q<=job->MaxTable[unum][63];q++) {
        for (t2=0;t2<=job->ThreshSpan;t2++,indx++) {
          if ((Loc = job->opt_method.dp.Bpp[unum][63][indx])
  		>= job->opt_method.dp.TableRowSize) {
  	    continue;
  	  }
          if (job->opt_method.dp.Err[unum][63][indx] < PrevRow[Loc]) {
            if (Loc < LeastLast) LeastLast = Loc;
            if (Loc > MostLast) MostLast = Loc;
            job->opt_method.dp.QChoice[unum][63][Loc] = ((RD_Qentry) q);
  	    job->opt_method.dp.TChoice[unum][63][Loc] = ((RD_Qentry)t2);
            PrevRow[Loc] = job->opt_method.dp.Err[unum][63][indx];
          }
        }
      }
    }
  
    if (job->VerboseLevel > 3)
      (void) DD_printf(&job->ErrDD,
	"\t\t\tBPP range: %ld/%ld to %ld/%ld\n",
        LeastLast,job->opt_method.dp.BppScale,
	MostLast,job->opt_method.dp.BppScale);

  
    for (n=62;n>=0;n--) {
      if (job->opt_method.dp.ErrEncodesRow[unum][n]) {
  
        if (job->VerboseLevel > 2)
	  (void) DD_printf(&job->ErrDD,
	    "\t\tCalculating row for compacted coeff #%d\n",n);

        for (i=MostLast;i>=LeastLast;i--) {
  	  if (PrevRow[i] == RD_INFINITY) continue;
          Loc2 = 0;
          for (k = job->opt_method.dp.BppOffsetInEncoding[unum][n];
  	       k <= job->opt_method.dp.BppMaxInEncoding[unum][n];
	       k++,Loc2++) {
  	    if ((Loc = i+k) >= job->opt_method.dp.TableRowSize)
	      continue;
  	    ErrTemp = PrevRow[i] +
		      job->opt_method.dp.Err[unum][n][Loc2];
  	    if (ErrTemp < CurrRow[Loc]) {
  	      CurrRow[Loc] = ErrTemp;
              job->opt_method.dp.QChoice[unum][n][Loc] =
  	        job->opt_method.dp.QforBpp[n][Loc2];
              job->opt_method.dp.TChoice[unum][n][Loc] =
  	        job->opt_method.dp.TforBpp[n][Loc2];
  	    }
          }
        }
        RD_FREE(job->opt_method.dp.QforBpp[n]);
        RD_FREE(job->opt_method.dp.TforBpp[n]);

      } else {
  
        if (job->VerboseLevel > 2)
	  (void) DD_printf(&job->ErrDD,
	    "\t\tCalculating row for #%d\n",n);

        for (i=MostLast;i>=LeastLast;i--) {
          if (PrevRow[i] == RD_INFINITY) continue;
          indx = job->MinTable[unum][n]*tspanplus1;
          for (q=job->MinTable[unum][n];
	       q<=job->MaxTable[unum][n];q++) {
            for (t2=0;t2<=job->ThreshSpan;t2++,indx++) {
  
  	      k = job->opt_method.dp.Bpp[unum][n][indx];
  	      if ((Loc = i+k) >= job->opt_method.dp.TableRowSize)
		continue;
  	      ErrTemp = PrevRow[i] +
		 job->opt_method.dp.Err[unum][n][indx];
  	      if (ErrTemp < CurrRow[Loc]) {
  		  
  	        CurrRow[Loc] = ErrTemp;
  	        job->opt_method.dp.QChoice[unum][n][Loc] = ((RD_Qentry) q);
  		job->opt_method.dp.TChoice[unum][n][Loc] = ((RD_Qentry) t2);
  
              }
  	    }
          }
        }
      }
  
      while (CurrRow[LeastLast]==RD_INFINITY) LeastLast++;
      MostLast = job->opt_method.dp.TableRowSize-1;
      while (CurrRow[MostLast]==RD_INFINITY) MostLast--;
  
      if (job->VerboseLevel > 3)
        (void) DD_printf(&job->ErrDD,
	  "\t\t\tBPP range: %ld/%ld to %ld/%ld\n",
          LeastLast,job->opt_method.dp.BppScale,
	  MostLast,job->opt_method.dp.BppScale);

      TempRow = CurrRow;
      CurrRow = PrevRow;
      PrevRow = TempRow;
      ResetRow(CurrRow,job->opt_method.dp.TableRowSize);
    }
  } else {

    /* No Thresholding */
    if (job->opt_method.dp.ErrEncodesRow[unum][63]) {
      if (job->VerboseLevel > 2)
	(void) DD_printf(&job->ErrDD,"\t\tCalculating row for compacted coeff #63\n");

      Loc2 = 0;
      for (Loc = job->opt_method.dp.BppOffsetInEncoding[unum][63];
  	 Loc <= job->opt_method.dp.BppMaxInEncoding[unum][63];
	 Loc++,Loc2++) {
        PrevRow[Loc] = job->opt_method.dp.Err[unum][63][Loc2];
        job->opt_method.dp.QChoice[unum][63][Loc] =
  	  job->opt_method.dp.QforBpp[63][Loc2];
      }
      RD_FREE(job->opt_method.dp.QforBpp[63]);
      LeastLast = job->opt_method.dp.BppOffsetInEncoding[unum][63];
      MostLast = job->opt_method.dp.BppMaxInEncoding[unum][63];

    } else {
      if (job->VerboseLevel > 2)
	(void) DD_printf(&job->ErrDD,"\t\tCalculating row for coeff #63\n");

      LeastLast = job->opt_method.dp.TableRowSize-1;
      MostLast = 0;
      for (q=job->MinTable[unum][63]; q<=job->MaxTable[unum][63];q++) {
        if ((Loc = job->opt_method.dp.Bpp[unum][63][q])
  		>= job->opt_method.dp.TableRowSize) {
  	  continue;
  	}
        if (job->opt_method.dp.Err[unum][63][q] < PrevRow[Loc]) {
            if (Loc < LeastLast) LeastLast = Loc;
            if (Loc > MostLast) MostLast = Loc;
            job->opt_method.dp.QChoice[unum][63][Loc] = ((RD_Qentry) q);
            PrevRow[Loc] = job->opt_method.dp.Err[unum][63][q];
        }
      }
    }
  
    if (job->VerboseLevel > 3)
        (void) DD_printf(&job->ErrDD,
	  "\t\t\tBPP range: %ld/%ld to %ld/%ld\n",
          LeastLast,job->opt_method.dp.BppScale,
	  MostLast,job->opt_method.dp.BppScale);

    for (n=62;n>=0;n--) {
      if (job->opt_method.dp.ErrEncodesRow[unum][n]) {
        if (job->VerboseLevel > 2)
	  (void) DD_printf(&job->ErrDD,
	    "\t\tCalculating row for compacted coeff #%d\n",n);
  
        for (i=MostLast;i>=LeastLast;i--) {
  	  if (PrevRow[i] == RD_INFINITY) continue;
          Loc2 = 0;
          for (k = job->opt_method.dp.BppOffsetInEncoding[unum][n];
  	     k <= job->opt_method.dp.BppMaxInEncoding[unum][n]; k++,Loc2++) {
  	    if ((Loc = i+k) >= job->opt_method.dp.TableRowSize) continue;
  	    ErrTemp = PrevRow[i] + job->opt_method.dp.Err[unum][n][Loc2];
  	    if (ErrTemp < CurrRow[Loc]) {
  	      CurrRow[Loc] = ErrTemp;
              job->opt_method.dp.QChoice[unum][n][Loc] =
  	         job->opt_method.dp.QforBpp[n][Loc2];
  	    }
          }
        }
        RD_FREE(job->opt_method.dp.QforBpp[n]);

      } else {
        if (job->VerboseLevel > 2)
	  (void) DD_printf(&job->ErrDD,
	    "\t\tCalculating row for coeff #%d\n",n);
  
        for (i=MostLast;i>=LeastLast;i--) {
          if (PrevRow[i] == RD_INFINITY) continue;
          for (q=job->MinTable[unum][n];q<=job->MaxTable[unum][n];q++) {
  
  	    k = job->opt_method.dp.Bpp[unum][n][q];
  	    if ((Loc = i+k) >=job->opt_method.dp.TableRowSize) continue;
  	    ErrTemp = PrevRow[i] + job->opt_method.dp.Err[unum][n][q];
  	    if (ErrTemp < CurrRow[Loc]) {
  		  
  	      CurrRow[Loc] = ErrTemp;
  	      job->opt_method.dp.QChoice[unum][n][Loc] =((RD_Qentry) q);
  	    }
          }
        }
      }
  
      while (CurrRow[LeastLast]==RD_INFINITY) LeastLast++;
      MostLast = job->opt_method.dp.TableRowSize-1;
      while (CurrRow[MostLast]==RD_INFINITY) MostLast--;
  
      if (job->VerboseLevel > 3)
        (void) DD_printf(&job->ErrDD,
	  "\t\t\tBPP range: %ld/%ld to %ld/%ld\n",
          LeastLast,job->opt_method.dp.BppScale,
	  MostLast,job->opt_method.dp.BppScale);

  
      TempRow = CurrRow;
      CurrRow = PrevRow;
      PrevRow = TempRow;
      ResetRow(CurrRow,job->opt_method.dp.TableRowSize);
    }
  }

  RD_FREE(CurrRow);
  job->opt_method.dp.LeastLast[unum] = LeastLast;
  job->opt_method.dp.MostLast[unum] = MostLast;

  /*** ensure monotonicity ******/
  ErrTemp = RD_INFINITY;
  for (i=LeastLast;i<=MostLast;i++) {
    if (PrevRow[i] >= ErrTemp) PrevRow[i] = RD_INFINITY;
    else ErrTemp = PrevRow[i];
  }

  job->opt_method.dp.LastRow[unum] = PrevRow;
  return 1; 
}


extern int RD_CombineUnits(RD_Job *job)
{
  long i,j,lim, unum, Loc;
  RD_float ErrTemp, *PrevRow = 0, *TempRow;

  /********* should not be called if job->NumTables==1 ******/
  /***** allocate ******/
  if ((job->opt_method.dp.CombinedLastRow = (RD_float *)
	    calloc(1,job->opt_method.dp.TableRowSize*
	       sizeof(RD_float)))==NULL) {
    RD_FatalError("CombineUnits out of memory");
  }
  ResetRow(job->opt_method.dp.CombinedLastRow,
	   job->opt_method.dp.TableRowSize);

  if (job->NumTables > 2) {
    if ((PrevRow = (RD_float *)
	    calloc(1,job->opt_method.dp.TableRowSize*
	       sizeof(RD_float)))==NULL) {
      RD_FatalError("CombineUnits out of memory");
    }
    ResetRow(PrevRow, job->opt_method.dp.TableRowSize);
  }

  if ((job->opt_method.dp.ToCombine[1] = (long *)
	calloc(1,job->opt_method.dp.TableRowSize*
		(job->NumTables-1)*sizeof(long)))==NULL) {
    RD_FREE(PrevRow); 
    RD_FatalError("CombineUnits out of memory");
  }

  for (i=2;i<job->NumTables;i++)
    job->opt_method.dp.ToCombine[i] =
      job->opt_method.dp.ToCombine[i-1] +
      job->opt_method.dp.TableRowSize;

  if (job->VerboseLevel) {
    (void) DD_printf(&job->ErrDD,"\tIncorporating unit 0\n");
    if (job->VerboseLevel > 1) {
      (void) DD_printf(&job->ErrDD,
      "\t\tBPP range: %ld/%ld through %ld/%ld\n",
        job->opt_method.dp.LeastLast[0],job->opt_method.dp.BppScale,
        job->opt_method.dp.MostLast[0],job->opt_method.dp.BppScale);
    }
    (void) DD_printf(&job->ErrDD,"\tIncorporating unit 1\n");
  }

  /** combine rows **/
  for (i=job->opt_method.dp.LeastLast[0];
       i<=job->opt_method.dp.MostLast[0];i++) {
    lim = job->opt_method.dp.TableRowSize - i - 1;
    if (job->opt_method.dp.MostLast[1] < lim)
      lim = job->opt_method.dp.MostLast[1];
    for (j=job->opt_method.dp.LeastLast[1];j<= lim; j++) {
      ErrTemp = job->opt_method.dp.LastRow[0][i] +
	job->opt_method.dp.LastRow[1][j];
      Loc = i+j;
      if (ErrTemp < job->opt_method.dp.CombinedLastRow[Loc]) {
         job->opt_method.dp.CombinedLastRow[Loc] = ErrTemp;
	 job->opt_method.dp.ToCombine[1][Loc] = j;
      }
    }
  }
  job->opt_method.dp.CombinedLeast=0;
  while ( 
  job->opt_method.dp.CombinedLastRow[job->opt_method.dp.CombinedLeast]
     == RD_INFINITY)
    job->opt_method.dp.CombinedLeast++;
  job->opt_method.dp.CombinedMost=job->opt_method.dp.TableRowSize-1;
  while ( 
  job->opt_method.dp.CombinedLastRow[job->opt_method.dp.CombinedMost]
     == RD_INFINITY)
    job->opt_method.dp.CombinedMost--;


  if (job->VerboseLevel > 1) {
    (void) DD_printf(&job->ErrDD,"\t\tBPP range: %ld/%ld through %ld/%ld\n",
      job->opt_method.dp.CombinedLeast,job->opt_method.dp.BppScale,
      job->opt_method.dp.CombinedMost,job->opt_method.dp.BppScale);
  }


  for (unum=2;unum < job->NumTables; unum++) {

    if (job->VerboseLevel) (void) DD_printf(&job->ErrDD,
          "\tIncorporating unit %d\n",unum);
    TempRow = job->opt_method.dp.CombinedLastRow;
    job->opt_method.dp.CombinedLastRow = PrevRow;
    PrevRow = TempRow;
    ResetRow(job->opt_method.dp.CombinedLastRow,
      job->opt_method.dp.TableRowSize);
   
    for (i=job->opt_method.dp.CombinedLeast;
      i<=job->opt_method.dp.CombinedMost;i++) {
      lim = job->opt_method.dp.TableRowSize - i - 1;
      if (job->opt_method.dp.MostLast[unum] < lim)
	lim = job->opt_method.dp.MostLast[unum];
      for (j=job->opt_method.dp.LeastLast[unum];j<= lim; j++) {
        ErrTemp = PrevRow[i] + job->opt_method.dp.LastRow[unum][j];
        Loc = i+j;
        if (ErrTemp < job->opt_method.dp.CombinedLastRow[Loc]) {
           job->opt_method.dp.CombinedLastRow[Loc] = ErrTemp;
  	   job->opt_method.dp.ToCombine[unum][Loc] = j;
        }
      }
    }
    while ( 
    job->opt_method.dp.CombinedLastRow[job->opt_method.dp.CombinedLeast]
       == RD_INFINITY)
      job->opt_method.dp.CombinedLeast++;
    job->opt_method.dp.CombinedMost=job->opt_method.dp.TableRowSize-1;
    while ( 
    job->opt_method.dp.CombinedLastRow[job->opt_method.dp.CombinedMost]
       == RD_INFINITY)
      job->opt_method.dp.CombinedMost--;

    if (job->VerboseLevel > 1) {
      (void) DD_printf(&job->ErrDD,"\t\tBPP range: %ld/%ld through %ld/%ld\n",
        job->opt_method.dp.CombinedLeast,job->opt_method.dp.BppScale,
        job->opt_method.dp.CombinedMost,job->opt_method.dp.BppScale);
    }


  }  

  if (job->NumTables > 2) RD_FREE(PrevRow);

  /*** ensure monotonicity ***/
  ErrTemp = RD_INFINITY;
  for (i=job->opt_method.dp.CombinedLeast;
       i<=job->opt_method.dp.CombinedMost;i++) {
    if (job->opt_method.dp.CombinedLastRow[i] >=
	ErrTemp) job->opt_method.dp.CombinedLastRow[i] = RD_INFINITY;
    else ErrTemp = job->opt_method.dp.CombinedLastRow[i];
  }
  return 1; 

}

extern void RD_Epilogue(RD_Job *job)
{
  long unum, i, Loc1, Loc2; 
  RD_float err, bpp, psnr; 

  /**** compute LogSignalSq ****/
  job->LogTotalSignalSq = 0.0;
  for (unum=0;unum < job->NumTables; unum++) {
    job->LogTotalSignalSq += job->LogSignalSq[unum];
    job->LogSignalSq[unum] =
      ((RD_float) log10((double) job->LogSignalSq[unum]));
  }
  job->LogTotalSignalSq =
    ((RD_float) log10((double) job->LogTotalSignalSq));

  /**** take logarithms of all error rows ***/
  for (unum=0;unum < job->NumTables; unum++) {
    for (i=job->opt_method.dp.LeastLast[unum];
	 i<= job->opt_method.dp.MostLast[unum]; i++) {
      if (job->opt_method.dp.LastRow[unum][i] < RD_INFINITY)
        job->opt_method.dp.LastRow[unum][i] =
	  ((RD_float) log10((double)
	    (job->opt_method.dp.LastRow[unum][i]/
	     job->UnitWeights[unum])));
    }
  }

  if (job->NumTables > 1) {
    for (i=job->opt_method.dp.CombinedLeast;
	 i<= job->opt_method.dp.CombinedMost; i++) {
      if (job->opt_method.dp.CombinedLastRow[i] < RD_INFINITY)
        job->opt_method.dp.CombinedLastRow[i] =
	  ((RD_float) log10((double)
	     job->opt_method.dp.CombinedLastRow[i]));
    }
  }

  if (job->VerboseLevel) {
    /** print range **/
    if (job->NumTables > 1) {
      Loc1 = job->opt_method.dp.CombinedLeast;
      while ((Loc1 <= job->opt_method.dp.CombinedMost) &&
        (job->opt_method.dp.CombinedLastRow[Loc1] >= RD_INFINITY)) Loc1++;
      Loc2 = job->opt_method.dp.CombinedMost;
      while ((Loc2 >= job->opt_method.dp.CombinedLeast) &&
        (job->opt_method.dp.CombinedLastRow[Loc2] >= RD_INFINITY)) Loc2--;

      if ((Loc1 > job->opt_method.dp.CombinedMost) ||
          (Loc2 < job->opt_method.dp.CombinedLeast)) {
        (void) DD_printf(&job->ErrDD,
	  "No feasible quantization tables found in range\n");
      } else {
        err = job->opt_method.dp.CombinedLastRow[Loc1];
        psnr = (job->LogTotalPeakSq - err)*10.0;
        bpp = ((RD_float) Loc1)/((RD_float) job->opt_method.dp.BppScale);
        (void) DD_printf(&job->ErrDD,
	  " Range: bpp (psnr) %lf (%lf) to ", (double) bpp, (double) psnr);
        err = job->opt_method.dp.CombinedLastRow[Loc2];
        psnr = (job->LogTotalPeakSq - err)*10.0;
        bpp = ((RD_float) Loc2)/((RD_float) job->opt_method.dp.BppScale);
        (void) DD_printf(&job->ErrDD,"  %lf (%lf)\n", (double) bpp, (double) psnr);
      }
    } else {
      Loc1 = job->opt_method.dp.LeastLast[0];
      while ((Loc1 <= job->opt_method.dp.MostLast[0]) &&
        (job->opt_method.dp.LastRow[0][Loc1] >= RD_INFINITY)) Loc1++;
      Loc2 = job->opt_method.dp.MostLast[0];
      while ((Loc2 >= job->opt_method.dp.LeastLast[0]) &&
        (job->opt_method.dp.LastRow[0][Loc2] >= RD_INFINITY)) Loc2--;

      if ((Loc1 > job->opt_method.dp.MostLast[0]) ||
          (Loc2 < job->opt_method.dp.LeastLast[0])) {
        (void) DD_printf(&job->ErrDD,
	  "No feasible quantization tables found in range\n");
      } else {
        err = job->opt_method.dp.LastRow[0][Loc1];
        psnr = (job->LogTotalPeakSq - err)*10.0;
        bpp = ((RD_float) Loc1)/((RD_float) job->opt_method.dp.BppScale);
        (void) DD_printf(&job->ErrDD," Range: bpp (psnr) %lf (%lf) to ",
                                     (double) bpp, (double) psnr);
        err = job->opt_method.dp.LastRow[0][Loc2];
        psnr = (job->LogTotalPeakSq - err)*10.0;
        bpp = ((RD_float) Loc2)/((RD_float) job->opt_method.dp.BppScale);
        (void) DD_printf(&job->ErrDD,"  %lf (%lf)\n", (double) bpp, (double) psnr);
      }
    }

  }

  
}

