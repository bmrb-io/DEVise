#include "RDOPT.h"
#include "RDOPT_Internal.h"

#define H(n) job->Histogram[(n)]

extern int RD_LagrPrepareForErrBpp(RD_Job *job, int unum)
{
  long qe, n, offset, totalentries, max;
  RD_float *fptr1, *fptr2, *fptr3;
  long *iptr;

  /*** set MinTable and MaxTable ***/
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

  if ((fptr1 = (RD_float *)
       calloc(1,totalentries*sizeof(RD_float)))==NULL) {
    RD_FatalError("PrepareForErrBpp out of memory");
  }
  if ((fptr2 = (RD_float *)
       calloc(1,totalentries*sizeof(RD_float)))==NULL) {
    RD_FREE(fptr1); 
    RD_FatalError("PrepareForErrBpp out of memory");
  }
  if ((fptr3 = (RD_float *)
       calloc(1,totalentries*sizeof(RD_float)))==NULL) {
    RD_FREE(fptr1); 
    RD_FREE(fptr2); 
    RD_FatalError("PrepareForErrBpp out of memory");
  }
  if ((iptr = (long *)
       calloc(1,totalentries*sizeof(long)))==NULL) {
    RD_FREE(fptr1); 
    RD_FREE(fptr2); 
    RD_FREE(fptr3); 
    RD_FatalError("PrepareForErrBpp out of memory");
  }

  offset = 0;
  for (n=0;n<64;n++) {
    job->opt_method.lagr.Err[unum][n] = fptr1 + offset;
    job->opt_method.lagr.Bpp[unum][n] = fptr2 + offset;
    job->opt_method.lagr.Lambda[unum][n] = fptr3 + offset;
    job->opt_method.lagr.SortedIndex[unum][n] = iptr + offset;

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
      E = ans*job->UnitWeights[unum]/job->opt_method.lagr.ErrScale; \
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
      E *= (job->UnitWeights[unum]/job->opt_method.lagr.ErrScale); \
}

#define SetErrNoThresh(E) \
{ \
  long i,longquant; \
  RD_float orig, err; \
  register RD_float ans; \
  register long * ctptr; \
      ans = ((RD_float) 0.0); \
      for (i=H(n).PlusSize-1,ctptr=&(H(n).PlusCount[i]);i>=0;i--) { \
        orig = UnDiscretizePlus(i); \
        longquant = QuantizeDis(i,q); \
        err = orig - RealUnQuantize(longquant,q); \
        ans += (err*err*((RD_float) (*ctptr--))); \
      } \
      for (i=1-H(n).MinusSize,ctptr=&(H(n).MinusCount[0-i]);i<=0;i++) { \
        orig = UnDiscretizeMinus(i); \
        longquant = QuantizeDis(i,q); \
        err = orig - RealUnQuantize(longquant,q); \
        ans += (err*err*((RD_float) (*ctptr--))); \
      } \
      if (job->WeightedCoefs[unum]) ans *= job->CoefWeights[unum][n]; \
      E = (ans*job->UnitWeights[unum])/job->opt_method.lagr.ErrScale; \
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
  for (v=1-H(n).MinusSize,ctptr=&(H(n).MinusCount[0-v]);v<=0;v++) {\
      quant = QuantizeDis(v, q);\
      if (quant > lastVal ) {\
        if (lastCount > 0) {\
	  entr_temp = \
                   (logtotal - ((RD_float) log10((double) lastCount)))\
                         * ((RD_float) lastCount) *\
			 entr_multf; \
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
                         * ((RD_float) lastCount) *\
			 entr_multf; \
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
                 * ((RD_float) lastCount) *\
		 entr_multf; \
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
  register long *ctptr; \
  Entropy = 0.0;\
  lastVal = -5000; lastCount = 0;\
  for (v=1-H(n).MinusSize, ctptr = &(H(n).MinusCount[0-v]);v<=0;v++) {\
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


extern int RD_LagrSetErrBpp(RD_Job *job, int unum)
{
  long n, q, qe, t2, t2lim, maxqvplus = 0, maxqvminus = 0;
  long  indx;
  long tspanplus1 = job->ThreshSpan + 1;
  RD_float ErrSavedPlus[RD_QENTRY_MAX+1], ErrSavedMinus[RD_QENTRY_MAX+1];
  RD_float LastErr;
  RD_float logtotal; 
  long *SavedCounts, * SavedCountBuff = 0; 
  RD_float *SavedEntr, * SavedEntrBuff = 0; 
  RD_float LastEntr;
  register RD_float entr_multf = job->bppWeight[unum]/
     (job->NumBlocks[unum] * ((RD_float) log10((double) 2.0) * 64.0));



  
  logtotal = ((RD_float) log10((double) job->NumBlocks[unum])); 

  if (job->VerboseLevel > 2) (void) DD_printf(&job->ErrDD,"\t\t"); 

  if (job->ThreshSpan > 0) {
    /* figure out max possible quantized value */
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
        indx = job->MinTable[unum][n]*tspanplus1;
        for (qe=job->MinTable[unum][n];
	     qe<=job->MaxTable[unum][n];qe++) {
  
	  q = job->QentryToQ(qe); 
  	  SetErrPrepareThresh(job->opt_method.lagr.Err[unum][n][indx]);
  	  SetBppPrepareThresh(job->opt_method.lagr.Bpp[unum][n][indx]);
  	 
	  indx++;
  	  t2lim = q + job->ThreshSpan;
  
  	  for (t2 = q+1; t2 <= t2lim; t2++,indx++) {
  	  
  	    SetErr(job->opt_method.lagr.Err[unum][n][indx]);
  	    SetBpp(job->opt_method.lagr.Bpp[unum][n][indx]);
          }
        }
        if (job->VerboseLevel > 2) (void) DD_printf(&job->ErrDD,"."); 
    }
    RD_FREE(SavedCountBuff);
    RD_FREE(SavedEntrBuff);
  } else {
    /* No thresholding */
    for (n=0;n<64;n++) {
        for (qe=job->MinTable[unum][n];
	     qe<=job->MaxTable[unum][n];qe++) {
	  q = job->QentryToQ(qe); 
  	  SetErrNoThresh(job->opt_method.lagr.Err[unum][n][qe]);
  	  SetBppNoThresh(job->opt_method.lagr.Bpp[unum][n][qe]);
        }
        if (job->VerboseLevel > 2) (void) DD_printf(&job->ErrDD,"."); 
    }
  }
  if (job->VerboseLevel > 2) (void) DD_printf(&job->ErrDD,"\n"); 
  return 1; 
}


#define EQUALITY_DELTA ((RD_float) 1e-30) 

#define IS_RIGHT_TURN(x0,y0,x1,y1,x2,y2) \
   (((((x2)-(x0))*((y1)-(y0))) - (((x1)-(x0))*((y2)-(y0)))) >= 0)


#define PUSH(p,S) \
   stacktop++; \
   (S)[stacktop] = (p)

#define POP(S) stacktop--

#define TOP(S) ((S)[stacktop])

#define NEXT_TO_TOP(S) ((S)[stacktop-1])

/* comparison function for qsort */
static RD_float *tosort;
static int comp_func(const void *i, const void *j)
{
  int a = *((int *) i); 
  int b = *((int *) j); 
  if (tosort[a] >= tosort[b]) return(-1);
  else return(1); 
}

extern int RD_LagrSortErrBpp(RD_Job *job, int unum)
{
  long tspanplus1 = job->ThreshSpan + 1;
  long indx, i, n, tot;
  long temp, done; 
  long *idxptr,*stackptr;
  long *tempIdx;
  RD_float rdiff, rlastval, ddiff, dlastval;
  long stacktop, orig_tot, lastwhere, nextpos;


  if ((tempIdx = (long *)
       calloc(1,sizeof(long)*(RD_QENTRY_MAX+1)*tspanplus1)) == NULL) {
	 RD_FatalError("lagrSortErrBpp out of memory");
  }

#define optstr job->opt_method.lagr

  for (n=0;n<64;n++) {
  
    tot = (job->MaxTable[unum][n] - job->MinTable[unum][n] + 1)*
	  tspanplus1;

    indx = job->MinTable[unum][n]*tspanplus1;
    idxptr = tempIdx;

    for (i=0,indx=job->MinTable[unum][n]*tspanplus1;i<tot;i++,indx++) {
      idxptr[i] = indx;
    }

    if (job->VerboseLevel > 2)
      (void) DD_printf(&job->ErrDD,"\t\tcoefficient #%d ..",n);

    if (job->ThreshSpan == 0) {
      /** bubble sort to ensure decreasing rate **/
      /** "bubble sort" 'cos the array must already be nearly sorted **/
      done = 0;
      while (!done) {
        done = 1;
        for (i=1;i<tot;i++) {
          if (optstr.Bpp[unum][n][idxptr[i-1]] <
	      optstr.Bpp[unum][n][idxptr[i]]) {
	    temp = idxptr[i-1];
	    idxptr[i-1] = idxptr[i];
	    idxptr[i] = temp;
	    done = 0;
          }
        }
      }
    } else {
      /* quick sort */
      tosort = optstr.Bpp[unum][n]; 
      qsort((char *) idxptr, tot, sizeof(long), comp_func);
    }
    
    orig_tot = tot;

    /** ensure "strictly" decreasing rate and "strictly" increasing distortion **/
    rlastval = optstr.Bpp[unum][n][idxptr[0]];
    dlastval = optstr.Err[unum][n][idxptr[0]];
    lastwhere = idxptr[0]; 
    nextpos = 0; 
    for (i=1;i<tot;i++) {
        rdiff = rlastval - optstr.Bpp[unum][n][idxptr[i]]; 
        if (rdiff <= EQUALITY_DELTA) {
          if (optstr.Err[unum][n][idxptr[i]] < dlastval ) { 
	    lastwhere = idxptr[i]; 
            rlastval = optstr.Bpp[unum][n][idxptr[i]]; 
            dlastval = optstr.Err[unum][n][idxptr[i]]; 
	  }
	} else {
	  idxptr[nextpos++] = lastwhere;
	  lastwhere = idxptr[i]; 
          rlastval = optstr.Bpp[unum][n][idxptr[i]]; 
          dlastval = optstr.Err[unum][n][idxptr[i]]; 
	} 
    }
    idxptr[nextpos++] = lastwhere;

    tot = nextpos; 


    rlastval = optstr.Bpp[unum][n][idxptr[0]];
    dlastval = optstr.Err[unum][n][idxptr[0]];
    lastwhere = idxptr[0]; 
    nextpos = 0; 
    for (i=1;i<tot;i++) {
        ddiff = optstr.Err[unum][n][idxptr[i]] - dlastval; 
        if (ddiff > EQUALITY_DELTA) {
	  idxptr[nextpos++] = lastwhere;
	} 
	lastwhere = idxptr[i]; 
        rlastval = optstr.Bpp[unum][n][idxptr[i]]; 
        dlastval = optstr.Err[unum][n][idxptr[i]]; 
    }
    idxptr[nextpos++] = lastwhere;

    tot = nextpos; 

    if (job->VerboseLevel > 2)
      (void) DD_printf(&job->ErrDD,".. %d of %d R-D points\n",tot, orig_tot);

#define Xval(i) job->opt_method.lagr.Err[unum][n][(i)]
#define Yval(i) job->opt_method.lagr.Bpp[unum][n][(i)]

    /* remove points not on the "convex hull" */
    stackptr = optstr.SortedIndex[unum][n];
    if (tot >= 3) {
      stacktop = -1;
      PUSH(idxptr[0],stackptr);
      PUSH(idxptr[1],stackptr);
      PUSH(idxptr[2],stackptr);
      for (i=3;i<tot;i++) {
	while ((stacktop > 0) && 
	  (IS_RIGHT_TURN(Xval(NEXT_TO_TOP(stackptr)),
			 Yval(NEXT_TO_TOP(stackptr)),
			 Xval(TOP(stackptr)),
			 Yval(TOP(stackptr)),
			 Xval(idxptr[i]),
			 Yval(idxptr[i])))) {
	  POP(stackptr);
	}
	PUSH(idxptr[i],stackptr);
      }
      tot = optstr.IndexEntries[unum][n] = stacktop + 1;
    } else {
      for (i=0; i< tot; i++) {
	stackptr[i] = idxptr[i];
      }
      optstr.IndexEntries[unum][n] = tot;
    }

    idxptr = stackptr;


    tot--;

    /* calculate slopes */
    for (i=0;i<tot;i++) {
	optstr.Lambda[unum][n][i] =
	  (optstr.Bpp[unum][n][idxptr[i]] -
	    optstr.Bpp[unum][n][idxptr[i+1]]) /
  	  (optstr.Err[unum][n][idxptr[i+1]] -
	   optstr.Err[unum][n][idxptr[i]]);
    }
    optstr.Lambda[unum][n][tot] = ((RD_float) 0.0);

    if (job->VerboseLevel > 3) 
      (void) DD_printf(&job->ErrDD,
      "\t\t\tBPP range %6.5lf to %6.5lf over %d R-D points\n",
	(double) optstr.Bpp[unum][n][idxptr[tot]],
	(double) optstr.Bpp[unum][n][idxptr[0]],tot+1);

  }

  RD_FREE(tempIdx);
  return 1; 
}

extern void RD_LagrEpilogue(RD_Job *job)
{
  int i, n, unum;
  RD_float err, bpp, bincr, psnr;
  RD_float lambda, askingbpp, delta;
  long tab[IM_MAXPLANES][64]; 

#define optstr job->opt_method.lagr

  if (job->VerboseLevel && (job->NumTables > 1)) {
    /* for compatibility of output # lines with dyn prog */
    (void) DD_printf(&job->ErrDD,"Postprocessing...\n");
    if (job->VerboseLevel > 1) n = 2; /* # of blank lines */
    else n = 1;
    for (unum=0;unum < job->NumTables; unum++) {
      for (i=0;i<n;i++) (void) DD_printf(&job->ErrDD,"\t\n");
    }
  }


  optstr.deltalambda = ((RD_float) 1.0e-8);
  optstr.deltabpp = ((RD_float) 0.001); 
  optstr.deltamse = ((RD_float) 0.01); 

  /**** compute LogSignalSq ****/
  job->LogTotalSignalSq = 0.0;
  for (unum=0;unum < job->NumTables; unum++) {
    job->LogTotalSignalSq += job->LogSignalSq[unum];
    job->LogSignalSq[unum] = ((RD_float)
	 log10((double) job->LogSignalSq[unum]));
  }
  job->LogTotalSignalSq = ((RD_float)
         log10((double) job->LogTotalSignalSq));
  optstr.LambdaMax = ((RD_float) 0.0);
  optstr.BppMax = ((RD_float) 0.0);
  optstr.BppMin = ((RD_float) 0.0);
  optstr.ErrMax = ((RD_float) 0.0);
  optstr.ErrMin = ((RD_float) 0.0);
  for (unum = 0; unum < job->NumTables; unum++) {
    for (n = 0; n < 64; n++) { 
      if (optstr.Lambda[unum][n][0] > optstr.LambdaMax) {
	optstr.LambdaMax = optstr.Lambda[unum][n][0];
      }
      optstr.BppMax +=
	 optstr.Bpp[unum][n][optstr.SortedIndex[unum][n][0]];
      optstr.ErrMin +=
	 optstr.Err[unum][n][optstr.SortedIndex[unum][n][0]];
      optstr.BppMin +=
	 optstr.Bpp[unum][n][optstr.SortedIndex[unum][n][optstr.IndexEntries[unum][n]-1]];
      optstr.ErrMax +=
	 optstr.Err[unum][n][optstr.SortedIndex[unum][n][optstr.IndexEntries[unum][n]-1]];
    }  
  } 

  optstr.LambdaMax += ((RD_float) 0.1);

  optstr.BppHook[0] = optstr.BppMax;
  optstr.LambdaHook[0] = optstr.LambdaMax;
  optstr.ErrHook[0] = optstr.ErrMin;

  optstr.BppHook[RD_NUM_HOOKS-1] = optstr.BppMin;
  optstr.LambdaHook[RD_NUM_HOOKS-1] = ((RD_float) 0.0);
  optstr.ErrHook[RD_NUM_HOOKS-1] = optstr.ErrMax;


  bincr = (optstr.BppMax - optstr.BppMin)/((RD_float) RD_NUM_HOOKS);
  delta = bincr/((RD_float) 10.0);
  askingbpp = optstr.BppMax - bincr;
  for (i = 1; i < (RD_NUM_HOOKS-1); i++, askingbpp -= bincr) {
    bpp = askingbpp;
    RD_LagrFindRate(job, &bpp, &err, 
      0.0, optstr.LambdaHook[i-1], &lambda, tab);
    optstr.BppHook[i] = bpp;
    optstr.LambdaHook[i] = lambda;
    optstr.ErrHook[i] = err;
  }

  if (job->VerboseLevel > 0) {
    /** print range **/
    psnr = (job->LogTotalPeakSq -
     ((RD_float) log10((double) (optstr.ErrMax*optstr.ErrScale)))) *10.0;
    (void) DD_printf(&job->ErrDD," Range: bpp (psnr) %lf (%lf) to ",
                   (double) optstr.BppMin, (double) psnr);
    psnr = (job->LogTotalPeakSq -
     ((RD_float) log10((double) (optstr.ErrMin*optstr.ErrScale)))) *10.0;
    (void) DD_printf(&job->ErrDD,"  %lf (%lf)\n",
       (double) optstr.BppMax, (double) psnr);
  }

}

extern int RD_LagrSetDeltaBpp(RD_Job *job, RD_float deltabpp)
{
  if (job->State != RD_STATE_OPTDONE ||
      !job->UseLagrangian) return 0; 
  job->opt_method.lagr.deltabpp = deltabpp; 
  return 1; 
}

extern int RD_LagrSetDeltaMse(RD_Job *job, RD_float deltamse)  
{
  if (job->State != RD_STATE_OPTDONE ||
      !job->UseLagrangian) return 0; 
  job->opt_method.lagr.deltamse = deltamse; 
  return 1; 
}

extern int RD_LagrSetDeltaLambda(RD_Job *job, RD_float deltalambda)  
{
  if (job->State != RD_STATE_OPTDONE ||
      !job->UseLagrangian) return 0; 
  job->opt_method.lagr.deltalambda = deltalambda; 
  return 1; 
}

