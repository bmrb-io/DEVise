#include "RDOPT.h"
#include "RDOPT_Internal.h"


#define H(n) job->Histogram[(n)]

extern int RD_PrepareForErrBpp(RD_Job *job, int unum)
{
  long offset, totalentries, max, qe;
  int n; 
  long *iptr;
  RD_float *fptr;

  /*** reset MinTable and MaxTable ***/
  totalentries = 64;



  for (n=0;n<64;n++) {
    max = H(n).PlusSize;
    if (max < H(n).MinusSize) max = H(n).MinusSize;
    qe = (long) RD_QToQentry(max, job->QentryToQ, TRUE /* round up */); 
    if (qe < job->MaxTable[unum][n]) {
      job->MaxTable[unum][n] = qe;
      if (job->MaxTable[unum][n] < job->MinTable[unum][n])
        job->MinTable[unum][n] = job->MaxTable[unum][n];
    }

    totalentries += job->MaxTable[unum][n];
  }

  totalentries *= (job->ThreshSpan+1);

  if ((iptr = (long *) calloc(1,totalentries*sizeof(long)))==NULL) {
    RD_FatalError("PrepareForErrBpp out of memory");
  }
  if ((fptr = (RD_float *) calloc(1,totalentries*sizeof(RD_float)))==NULL) {
    RD_FREE(iptr); 
    RD_FatalError("PrepareForErrBpp out of memory");
  }

  offset = 0;
  for (n=0;n<64;n++) {
    job->opt_method.dp.Err[unum][n] = fptr + offset;
    job->opt_method.dp.Bpp[unum][n] = iptr + offset;
    offset += ((job->MaxTable[unum][n]+1)*(job->ThreshSpan+1));
  }

  if (job->VerboseLevel > 1) {
    (void) DD_printf(&job->ErrDD,"\t\tQuantTable Min/Max entries:\n");
    for (n=0;n<64;n+=8) {
      (void) DD_printf(&job->ErrDD,
       "%3ld/%3ld %3ld/%3ld %3ld/%3ld %3ld/%3ld %3ld/%3ld %3ld/%3ld %3ld/%3ld %3ld/%3ld\n",
       job->QentryToQ(job->MinTable[unum][n+0]),
       job->QentryToQ(job->MaxTable[unum][n+0]),
       job->QentryToQ(job->MinTable[unum][n+1]),
       job->QentryToQ(job->MaxTable[unum][n+1]),
       job->QentryToQ(job->MinTable[unum][n+2]),
       job->QentryToQ(job->MaxTable[unum][n+2]),
       job->QentryToQ(job->MinTable[unum][n+3]),
       job->QentryToQ(job->MaxTable[unum][n+3]),
       job->QentryToQ(job->MinTable[unum][n+4]),
       job->QentryToQ(job->MaxTable[unum][n+4]),
       job->QentryToQ(job->MinTable[unum][n+5]),
       job->QentryToQ(job->MaxTable[unum][n+5]),
       job->QentryToQ(job->MinTable[unum][n+6]),
       job->QentryToQ(job->MaxTable[unum][n+6]),
       job->QentryToQ(job->MinTable[unum][n+7]),
       job->QentryToQ(job->MaxTable[unum][n+7])); 
    }
  }

  return 1; 
}



/*** MACROS for Err and Bpp ***/

#define SetErrPrepareThresh(E) \
{ \
  long i,longquant, ilim, idx ; \
  RD_float orig, err; \
  register RD_float ans; \
  register long *ctptr; \
      ans = ((RD_float) 0.0); \
      if (q < H(n).PlusSize) { \
        if ((ilim = (q + job->ThreshSpan)) >= H(n).PlusSize) \
	   ilim = H(n).PlusSize - 1; \
        for (i=H(n).PlusSize-1,ctptr=&(H(n).PlusCount[i]);i>ilim;i--) { \
          orig = UnDiscretizePlus(i); \
          longquant = QuantizeDis(i,q); \
          err = orig - RealUnQuantize(longquant,q); \
          ans += (err*err*((RD_float) (*ctptr--))); \
        } \
        idx = ilim - q; \
        for (i=ilim,ctptr=&(H(n).PlusCount[i]);i>=q;i--,idx--) { \
          orig = UnDiscretizePlus(i); \
          longquant = QuantizeDis(i,q); \
          err = orig - RealUnQuantize(longquant,q); \
	  ErrSavedPlus[idx] = (err*err*((RD_float) (*ctptr--))); \
          ans += ErrSavedPlus[idx]; \
        } \
        for (i=q-1,ctptr=&(H(n).PlusCount[i]);i>=0;i--) { \
          err = UnDiscretizePlus(i); \
          ans += (err*err*((RD_float) (*ctptr--))); \
        } \
      } \
      else { \
        for (i=H(n).PlusSize-1,ctptr=&(H(n).PlusCount[i]);i>=0;i--) { \
          err = UnDiscretizePlus(i); \
          ans += (err*err*((RD_float) (*ctptr--))); \
        } \
      } \
      if (q < H(n).MinusSize) { \
        if ((ilim = (q + job->ThreshSpan)) >= H(n).MinusSize) \
	   ilim = 1 - H(n).MinusSize ; \
        else ilim = 0 - ilim; \
        for (i=1-H(n).MinusSize,ctptr=&(H(n).MinusCount[0-i]);i<ilim;i++) { \
          orig = UnDiscretizeMinus(i); \
          longquant = QuantizeDis(i,q); \
          err = orig - RealUnQuantize(longquant,q); \
          ans += (err*err*((RD_float) (*ctptr--))); \
        } \
        idx = 0 - ilim - q; \
        for (i=ilim,ctptr=&(H(n).MinusCount[0-i]);i<=(0-q);i++, idx--) { \
          orig = UnDiscretizeMinus(i); \
          longquant = QuantizeDis(i,q); \
          err = orig - RealUnQuantize(longquant,q); \
	  ErrSavedMinus[idx] = (err*err*((RD_float) (*ctptr--))); \
          ans += ErrSavedMinus[idx]; \
        } \
        for (i=(1-q),ctptr=&(H(n).MinusCount[0-i]);i<=0;i++) { \
          err = UnDiscretizeMinus(i); \
          ans += (err*err*((RD_float) (*ctptr--))); \
        } \
      } \
      else { \
        for (i=1-H(n).MinusSize,ctptr=&(H(n).MinusCount[0-i]);i<=0;i++) { \
          err = UnDiscretizeMinus(i); \
          ans += (err*err*((RD_float) (*ctptr--))); \
        } \
      } \
      if (job->WeightedCoefs[unum]) { \
	ans *= job->CoefWeights[unum][n]; \
	for (i = 0; i<= job->ThreshSpan; i++) { \
	  ErrSavedPlus[i] *= job->CoefWeights[unum][n]; \
	  ErrSavedMinus[i] *= job->CoefWeights[unum][n]; \
	} \
      } \
      LastErr = ans; \
      E = ans*job->UnitWeights[unum]; \
}

#define SetErr(E) \
{ \
  /* t2/2 is the threshold */  \
  register RD_float err2, err; \
  long t2less1; \
      E = LastErr; \
      t2less1 = t2 - 1; \
      err2 = ((RD_float) 0.0); \
      if (t2less1 < H(n).PlusSize) { \
	E -= ErrSavedPlus[t2less1 - q]; \
	err = UnDiscretizePlus(t2less1); \
        err2 = (err*err*((RD_float) H(n).PlusCount[t2less1])); \
      } \
      if (t2less1 < H(n).MinusSize) { \
	E -= ErrSavedMinus[t2less1 - q]; \
	err = UnDiscretizeMinus((0-t2less1)); \
        err2 += (err*err*((RD_float) H(n).MinusCount[t2less1])); \
      } \
      if (job->WeightedCoefs[unum]) err2 *= job->CoefWeights[unum][n]; \
      E += err2; \
      LastErr = E; \
      E *= job->UnitWeights[unum]; \
}

#define SetErrNoThresh(E) \
{ \
  long i,longquant,ilim; \
  RD_float orig, err; \
  register RD_float ans; \
  register long *ctptr; \
      ans = ((RD_float) 0.0); \
      for (i=H(n).PlusSize-1,ctptr=&(H(n).PlusCount[i]);i>=q;i--) { \
        orig = UnDiscretizePlus(i); \
        longquant = QuantizeDis(i,q); \
        err = orig - RealUnQuantize(longquant,q); \
        ans += (err*err*((RD_float) (*ctptr--))); \
      } \
      for (i=q-1,ctptr=&(H(n).PlusCount[i]);i>=0;i--) { \
        err = UnDiscretizePlus(i); \
        ans += (err*err*((RD_float) (*ctptr--))); \
      } \
      ilim = 0 - q; \
      for (i=1-H(n).MinusSize,ctptr=&(H(n).MinusCount[0-i]);i<=ilim;i++) { \
        orig = UnDiscretizeMinus(i); \
        longquant = QuantizeDis(i,q); \
        err = orig - RealUnQuantize(longquant,q); \
        ans += (err*err*((RD_float) (*ctptr--))); \
      } \
      for (i=1-q,ctptr=&(H(n).MinusCount[0-i]);i<=0;i++) { \
        err = UnDiscretizeMinus(i); \
        ans += (err*err*((RD_float) (*ctptr--))); \
      } \
      if (job->WeightedCoefs[unum]) ans *= job->CoefWeights[unum][n]; \
      E = ans*job->UnitWeights[unum]; \
}

#define SetBppPrepareThresh(B) \
{\
  long v, lastVal,lastCount,quant;\
  long vh,vl,qvh,qvl; \
  register RD_float Entropy,entr_temp;\
  register long *ctptr; \
  vh = q + job->ThreshSpan; \
  qvh = QuantizeDis(vh, q); \
  vl = 0 - q - job->ThreshSpan; \
  qvl = QuantizeDis(vl, q); \
  Entropy = 0.0;\
  lastVal = -5000; lastCount = 0;\
  for (v=1-H(n).MinusSize,ctptr = &(H(n).MinusCount[0-v]);v<=0;v++) {\
      quant = QuantizeDis(v, q);\
      if (quant > lastVal ) {\
        if (lastCount > 0) {\
	  entr_temp = \
                   (logtotal - ((RD_float) log10((double) lastCount)))\
                         * ((RD_float) lastCount) * entr_multf; \
        } else { \
	  entr_temp = ((RD_float) 0.0); \
        } \
        Entropy += entr_temp; \
        if (lastVal >= qvl) { \
	    SavedCounts[lastVal] = lastCount; \
	    SavedEntr[lastVal] = entr_temp; \
        } \
        lastCount = 0;\
        lastVal = quant;\
      }\
      lastCount += (*ctptr--); \
  }\
  for (v=0,ctptr = &(H(n).PlusCount[v]);v<H(n).PlusSize;v++) {\
      quant = QuantizeDis(v, q);\
      if (quant > lastVal ) {\
        if (lastCount > 0) {\
          entr_temp = \
                   (logtotal - ((RD_float) log10((double) lastCount)))\
                         * ((RD_float) lastCount) * entr_multf; \
        } else { \
	  entr_temp = ((RD_float) 0.0); \
        } \
	Entropy += entr_temp; \
	if (lastVal <= qvh) { \
	    SavedCounts[lastVal] = lastCount; \
	    SavedEntr[lastVal] = entr_temp; \
	} \
        lastCount = 0;\
        lastVal = quant;\
      }\
      lastCount += (*ctptr++); \
  }\
  if (lastCount > 0) {\
      entr_temp = \
           (logtotal - ((RD_float) log10((double) lastCount)))\
                 * ((RD_float) lastCount) * entr_multf; \
  } else { \
      entr_temp = ((RD_float) 0.0); \
  } \
  Entropy += entr_temp; \
  if (lastVal <= qvh) { \
      SavedCounts[lastVal] = lastCount; \
      SavedEntr[lastVal] = entr_temp; \
  } \
  LastEntr = Entropy; \
  B = Entropy;\
}

#define SetBpp(B) \
{\
  long v; \
  register RD_float Entropy;\
  long qv,qvneg,change; \
  Entropy = LastEntr; \
  change = 0; \
  v = t2-1; \
  qv = QuantizeDis(v, q); \
  qvneg = 0 - qv; \
  if ((v < H(n).MinusSize) && (H(n).MinusCount[v] > 0)) { \
    Entropy -= SavedEntr[qvneg]; \
    SavedCounts[qvneg] -= H(n).MinusCount[v]; \
    if (SavedCounts[qvneg] > 0) { \
      SavedEntr[qvneg] = \
         (logtotal - ((RD_float) log10((double) SavedCounts[qvneg])))\
                 * ((RD_float) SavedCounts[qvneg]) *\
		 entr_multf; \
    } else { \
      SavedEntr[qvneg] = ((RD_float) 0.0); \
    } \
    Entropy += SavedEntr[qvneg]; \
    SavedCounts[0] += H(n).MinusCount[v]; \
    change = 1; \
  } \
  if ((v < H(n).PlusSize) && (H(n).PlusCount[v] > 0)) { \
    Entropy -= SavedEntr[qv]; \
    SavedCounts[qv] -= H(n).PlusCount[v]; \
    if (SavedCounts[qv] > 0) { \
      SavedEntr[qv] = \
         (logtotal - ((RD_float) log10((double) SavedCounts[qv])))\
                 * ((RD_float) SavedCounts[qv]) *\
		 entr_multf; \
    } else { \
      SavedEntr[qv] = ((RD_float) 0.0); \
    } \
    Entropy += SavedEntr[qv]; \
    SavedCounts[0] += H(n).PlusCount[v]; \
    change = 1; \
  } \
  if (change) { \
    Entropy -= SavedEntr[0]; \
    SavedEntr[0] = \
         (logtotal - ((RD_float) log10((double) SavedCounts[0])))\
                 * ((RD_float) SavedCounts[0]) *\
		 entr_multf; \
    Entropy += SavedEntr[0]; \
  } \
  LastEntr = Entropy; \
  B = Entropy; \
}

#define SetBppNoThresh(B) \
{\
  long v, lastVal,lastCount,quant;\
  register RD_float Entropy;\
  register long * ctptr; \
  Entropy = 0.0;\
  lastVal = -5000; lastCount = 0;\
  for (v=1-H(n).MinusSize,ctptr=&(H(n).MinusCount[0-v]);v<=0;v++) {\
        quant = QuantizeDis(v, q);\
        if (quant > lastVal ) {\
          if (lastCount > 0) {\
            Entropy = Entropy +\
                   (logtotal - ((RD_float) log10((double) lastCount)))\
                         * ((RD_float) lastCount) *\
			 entr_multf; \
          }\
          lastCount = 0;\
          lastVal = quant;\
        }\
        lastCount += (*ctptr--); \
  }\
  for (v=0,ctptr=&(H(n).PlusCount[v]);v<H(n).PlusSize;v++) {\
        quant = QuantizeDis(v, q);\
        if (quant > lastVal ) {\
          if (lastCount > 0) {\
            Entropy = Entropy +\
                   (logtotal - ((RD_float) log10((double) lastCount)))\
                         * ((RD_float) lastCount) *\
			 entr_multf; \
          }\
          lastCount = 0;\
          lastVal = quant;\
        }\
        lastCount += (*ctptr++); \
  }\
  if (lastCount > 0) {\
        Entropy = Entropy +\
           (logtotal - ((RD_float) log10((double) lastCount)))\
                 * ((RD_float) lastCount) *\
		 entr_multf; \
  }\
  B = Entropy;\
}

#define ToIntBpp(b) ((long) ((RD_float) ((b)*multiplier)+0.5))

extern int RD_SetErrBpp(RD_Job *job, int unum)
{
  long n, q, qe, t2, t2lim, maxqvplus, maxqvminus; 
  long  bppspan, leastlongbpp,  indx, longbpp;
  long tspanplus1 = job->ThreshSpan + 1;
  RD_float leastbpp, mostbpp,  multiplier, bpp, error;
  RD_float ErrSavedPlus[RD_QENTRY_MAX+1], ErrSavedMinus[RD_QENTRY_MAX+1];
  RD_float LastErr;
  RD_float logtotal; 
  long *SavedCounts, * SavedCountBuff = 0; 
  RD_float *SavedEntr, * SavedEntrBuff = 0; 
  RD_float LastEntr;
  register RD_float entr_multf = job->bppWeight[unum]/ 
    (job->NumBlocks[unum] * ((RD_float) log10((double) 2.0) * 64.0)); 


  
  multiplier = ((RD_float) job->opt_method.dp.BppScale);
  logtotal = ((RD_float) log10((double) job->NumBlocks[unum])); 
  
  if (job->VerboseLevel > 2) (void) DD_printf(&job->ErrDD,"\t\t");

  if (job->ThreshSpan > 0) {
    
    /* figure out max possible quantized value */
    maxqvplus = 0;
    maxqvminus = 0;
    for (n=0;n<64;n++) {
       if (H(n).PlusSize > maxqvplus) maxqvplus = H(n).PlusSize; 
       if (H(n).MinusSize > maxqvminus) maxqvminus = H(n).MinusSize; 
    }
    maxqvplus /= 2; maxqvplus += 2; 
    maxqvminus /= 2; maxqvminus += 2; 
    
    SavedCountBuff = (long *) malloc(sizeof(long) *
				      (maxqvminus + maxqvminus)); 
    SavedEntrBuff = (RD_float *) malloc(sizeof(RD_float) *
				      (maxqvminus + maxqvminus)); 
    if (!SavedEntrBuff || !SavedCountBuff) {
      RD_FREE(SavedCountBuff);
      RD_FREE(SavedEntrBuff);
      RD_FatalError("SetErrBpp: out of memory"); 
    } 

    SavedCounts = SavedCountBuff + maxqvminus; 
    SavedEntr = SavedEntrBuff + maxqvminus; 

    for (n=0;n<64;n++) {
  
      qe = job->MinTable[unum][n];

      q = job->QentryToQ(qe);
      
      SetBppNoThresh(mostbpp); 
      bppspan = ToIntBpp(mostbpp) + 5;

      if (bppspan < 
        ((job->MaxTable[unum][n]-
	    job->MinTable[unum][n]+1)*tspanplus1)) {
  
        job->opt_method.dp.ErrEncodesRow[unum][n] = TRUE;
  
        /* allocate QforBpp and TforBpp */
        job->opt_method.dp.QforBpp[n] =
  	  (RD_Qentry *) calloc(1, sizeof(RD_Qentry) * bppspan);
        job->opt_method.dp.TforBpp[n] =
  	  (RD_Qentry *) calloc(1, sizeof(RD_Qentry) * bppspan);
        if ((!job->opt_method.dp.TforBpp[n]) ||
  	    (!job->opt_method.dp.QforBpp[n])) {
            RD_FREE(SavedCountBuff);
            RD_FREE(SavedEntrBuff);
  	    RD_FatalError("Could not allocate memory: SetErrBpp");
        }
  
        job->opt_method.dp.BppOffsetInEncoding[unum][n] = 0;
        job->opt_method.dp.BppMaxInEncoding[unum][n] = bppspan -1;
        for (longbpp = 0; longbpp < bppspan; longbpp++) {
  	  job->opt_method.dp.Err[unum][n][longbpp] = RD_INFINITY;
        }
  
        indx = job->MinTable[unum][n]*tspanplus1;
        for (qe=job->MinTable[unum][n];qe<=
		   job->MaxTable[unum][n];qe++) {
  
          q = job->QentryToQ(qe);
          
	  SetBppPrepareThresh(bpp);
	  longbpp = ToIntBpp(bpp);
  	  if (longbpp > job->opt_method.dp.BppMaxInEncoding[unum][n])
  	    longbpp = job->opt_method.dp.BppMaxInEncoding[unum][n];
  	  job->opt_method.dp.Bpp[unum][n][indx++] = longbpp;
	  SetErrPrepareThresh(error);
  	  if (error < job->opt_method.dp.Err[unum][n][longbpp]) {
  	      job->opt_method.dp.Err[unum][n][longbpp] = error;
  	      job->opt_method.dp.QforBpp[n][longbpp] = (RD_Qentry) qe;
  	      job->opt_method.dp.TforBpp[n][longbpp] = (RD_Qentry) 0;
  	  }

  	  t2lim = q + job->ThreshSpan;

  
  	  for (t2 = q+1; t2 <= t2lim; t2++, indx++) {
  	  
  	    SetBpp(bpp);
  
  	    longbpp = ToIntBpp(bpp);
  
  	    if (longbpp > job->opt_method.dp.BppMaxInEncoding[unum][n])
  	      longbpp = job->opt_method.dp.BppMaxInEncoding[unum][n];
  
  	    job->opt_method.dp.Bpp[unum][n][indx] = longbpp;
  
  	    SetErr(error);
  
  	    if (error < job->opt_method.dp.Err[unum][n][longbpp]) {
  	      job->opt_method.dp.Err[unum][n][longbpp] = error;
  	      job->opt_method.dp.QforBpp[n][longbpp] = (RD_Qentry) qe;
  	      job->opt_method.dp.TforBpp[n][longbpp] =
		(RD_Qentry) ((long) t2 - q);
  	    }
          }
        }
  
      } else {
        job->opt_method.dp.ErrEncodesRow[unum][n] = FALSE;
  
        indx = job->MinTable[unum][n]*tspanplus1;
        for (qe=job->MinTable[unum][n];
	     qe<=job->MaxTable[unum][n];qe++) {
  
          q = job->QentryToQ(qe);

  	  SetErrPrepareThresh(job->opt_method.dp.Err[unum][n][indx]);
  	  SetBppPrepareThresh(bpp);

  	 
  	  job->opt_method.dp.Bpp[unum][n][indx] = ToIntBpp(bpp);
	  indx++;

  	  t2lim = q + job->ThreshSpan;
  
  	  for (t2 = q+1; t2 <= t2lim; t2++,indx++) {
  	  
  	    SetErr(job->opt_method.dp.Err[unum][n][indx]);
  	    SetBpp(bpp);
  	 
  	    job->opt_method.dp.Bpp[unum][n][indx] = ToIntBpp(bpp);
          }
        }
      }
      if (job->VerboseLevel > 2) (void) DD_printf(&job->ErrDD,"."); 
    }
    RD_FREE(SavedCountBuff);
    RD_FREE(SavedEntrBuff);
  } else {
    /* No thresholding */
    for (n=0;n<64;n++) {
  
      qe = job->MaxTable[unum][n];
  
      q = job->QentryToQ(qe);
  
      SetBppNoThresh(leastbpp);
     
      leastlongbpp = ToIntBpp(leastbpp);
  
      qe = job->MinTable[unum][n];
      q = job->QentryToQ(qe);
      
      SetBppNoThresh(mostbpp);
  
      bppspan = ToIntBpp(mostbpp) - leastlongbpp + 5;
      
  
      if (bppspan < 
        (job->MaxTable[unum][n]-job->MinTable[unum][n]+1)) {
  
        job->opt_method.dp.ErrEncodesRow[unum][n] = TRUE;
  
        /* allocate QforBpp */
        job->opt_method.dp.QforBpp[n] =
  	  (RD_Qentry *) calloc(1, sizeof(RD_Qentry) * bppspan);

        if (!job->opt_method.dp.QforBpp[n]) {
  	    RD_FatalError("Could not allocate memory: SetErrBpp");
        }
  
        job->opt_method.dp.BppOffsetInEncoding[unum][n] = leastlongbpp;
        job->opt_method.dp.BppMaxInEncoding[unum][n] =
	   leastlongbpp + bppspan -1;
        for (longbpp = 0; longbpp < bppspan; longbpp++) {
  	  job->opt_method.dp.Err[unum][n][longbpp] = RD_INFINITY;
        }
  
        for (qe=job->MinTable[unum][n];
	     qe<=job->MaxTable[unum][n];qe++) {
  
          q = job->QentryToQ(qe);
  	  SetBppNoThresh(bpp);
  	  longbpp = ToIntBpp(bpp);
  
  	  if (longbpp < leastlongbpp) longbpp = leastlongbpp;
  	  if (longbpp > job->opt_method.dp.BppMaxInEncoding[unum][n])
  	    longbpp = job->opt_method.dp.BppMaxInEncoding[unum][n];
  
  	  job->opt_method.dp.Bpp[unum][n][qe] = longbpp;
  	  longbpp -= leastlongbpp;
  	  SetErrNoThresh(error);
  	 
  	  if (error < job->opt_method.dp.Err[unum][n][longbpp]) {
  	    job->opt_method.dp.Err[unum][n][longbpp] = error;
  	    job->opt_method.dp.QforBpp[n][longbpp] = (RD_Qentry) qe;
  	  }
        }
  
      } else {
        job->opt_method.dp.ErrEncodesRow[unum][n] = FALSE;
  
        for (qe=job->MinTable[unum][n];qe<=job->MaxTable[unum][n];qe++) {
  
          q = job->QentryToQ(qe);
  	  
  	  SetErrNoThresh(job->opt_method.dp.Err[unum][n][qe]);
  	  SetBppNoThresh(bpp);
  	 
  	  job->opt_method.dp.Bpp[unum][n][qe] = ToIntBpp(bpp);
        }
      }
      if (job->VerboseLevel > 2) (void) DD_printf(&job->ErrDD,"."); 
    }
  }

  if (job->VerboseLevel > 2) (void) DD_printf(&job->ErrDD,"\n"); 

  return 1; 
}




