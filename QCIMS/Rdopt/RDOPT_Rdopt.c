#include "RDOPT.h"
#include <ctype.h>

static int blankline(char *s)
{
  int i,l;
  if (!s) return 1;
  l = strlen(s); 
  for (i=0;i<l;i++) {
    if (!isspace(s[i])) return 0;
  }
  return 1;
}

static void BriefUsage(void)
{
  printf("Usage: ImRdopt [options] [imagefile]\n"); 
  printf(" Options can be:\n");
  printf("  [-v] [-silent] [-cmdf file] [-numtab T] [-thresh T]\n");
  printf("  [-coarseness C] [-mintable fname] [-maxtable fname]\n");
  printf("  [-weights cwfile] [-pweights w0[,w1,..]] [-method meth]\n");
  printf("  [-rgbto2ycc] [-rgbtoycc] [-noycc]\n");
  printf("  [-raw W H N B] [-sbits s]\n");
  printf("  [-clampDC d] [-dontclampDC] [-dcdpcm]\n");
  printf("  [-bppmax b] [-bppscale B] [-errfile fname]\n");
  exit(1);
}



static void Usage(void)
{
  printf("ImRdopt [flags] [imfile]\n");
  printf("  where, flags can be:\n\n");
  printf("   -v: verbose output: The more -v's, the more verbose the output.\n");
  printf("   -silent: Normal stderr output will be suppressed. \n");
  printf("   -cmdfile <cfile>: After optimization, read commands from \n");
  printf("      <cfile> instead of stdin.  \n");
  printf("   -numtables T: T tables will be used for quantization.\n");
  printf("      1 table each for color planes numbered 0..(T-2). Color\n");
  printf("      planes numbered T-1 thru N-1 will be quantized together,\n");
  printf("      where N=#image planes. (default N). \n");
  printf("   -thresh T: Try zeroing thresholds upto T/2 greater than q/2.\n");
  printf("      Default 0. \n");
  printf("   -coarseness C: Use a coarse search for Q entries. C determines\n");
  printf("      the coarseness. C = 0 means try every possible value; C=8 is\n");
  printf("      the coarsest, currently. Default is 2. \n");
  printf("   -mintable fname: Q tables will have entries no less than those in the\n");
  printf("      table listed in the file fname. Upto N tables can be given in the\n");
  printf("      file. \n");
  printf("   -maxtable fname: Q tables will have entries no more than those in the\n");
  printf("      table listed in the file fname. Upto N tables can be given in the\n");
  printf("      file. \n");
  printf("   -weights cwfile: The errors in various DCT coefficients are to be\n");
  printf("      weighted according to the weights listed in cwfile. You can give\n");
  printf("      upto N (=#planes) 8x8 tables (row-major order). These\n");
  printf("      non-negative, real-valued weights will be normalized so that they\n");
  printf("      add to 64. Useful for using perceptually weighted distortion\n");
  printf("      measures. \n");
  printf("   -pweights w0[,w1,..]: For aggregate error, unit n will have weight\n");
  printf("      wn. Unit means a set of plabes with a common quantization table.\n");
  printf("      Missing weights will be set to the last specified weight. \n");
  printf("      Weights will be normalized to add up to numtables. \n");
  printf("   -rgbto2ycc: Convert RGB to YCbCr, subsample 2:1 in horiz and\n");
  printf("      vert directions for Cb and Cr planes. This is the default for\n");
  printf("      3-plane images.\n");
  printf("   -rgbtoycc: Convert RGB to YCbCr. \n");
  printf("   -noycc: Do not convert RGB to YCbCr, do not subsample.\n");
  printf("   -raw W H N B: Indicates that the image data is in RAW\n");
  printf("      (headerless) format. W=width, H=height, N=#planes, B=bytes\n");
  printf("      per pixel. B can be 1 or 2 (MSBfirst endian for 2). Planes\n");
  printf("      are stored consecutively, each plane in row-major order.\n");
  printf("   -sbits s: Use only s bits per pixel from the data. s can be\n");
  printf("      8, 10, 12, or 16. Useful for PSNR calculations for images\n");
  printf("      with 2-bytes per pixel (such as with B=2 in -raw).\n");
  printf("   -clampDC d: Don't consider qtable entries greater than d for the\n");
  printf("      DC coefficient (default 255, ie, -dontclampDC). \n");
  printf("   -dontclampDC: Consider all qtable entries for DC too. \n");
  printf("   -dcdpcm: Use an approximation of dpcm coding done for the DC coef\n");
  printf("      to estimate its entropy.\n");
  printf("   -method meth: Sepcifies the optimization method: meth can be\n");
  printf("      lagrange (default) or dynamic.\n");
  printf("   -bppmax b: Consider bits per pixel values upto b (default 1.0).\n");
  printf("      Ignored if method is lagrangian.\n");
  printf("   -bppscale B: Discretize bpp by the integer B (default 5000). The\n");
  printf("      slowness of the program grows linearly with b*B. Ignored if\n");
  printf("      method is lagrangian.\n");
  printf("   -errfile fname: Send stderr to file fname (can be - for stdout). \n");
  exit(1);
}

static void FatalError(char *s)
{
  printf("%s\n", s);
  exit(1);
}

static void BriefListCommands(RD_Job *job)
{
  #ifdef HAVE_JPEGLIB
  DD_printf(&job->ErrDD,
    "Valid commands: [compress/correct] size/bpp/psnr/snr/rmse target\n");
  DD_printf(&job->ErrDD,
    "                nocorrect/sequential/progressive [scanfile]\n"); 
  #else
  DD_printf(&job->ErrDD,
    "Valid commands: size/bpp/psnr/snr/rmse target\n");
  #endif
  DD_printf(&job->ErrDD,
    "                cfile fname/help/qfile fname/stats/nostats/quit\n");
  DD_printf(&job->ErrDD,
    "                curve file minbpp maxbpp npoints actbpp? [qfileprefix] \n");
  DD_printf(&job->ErrDD,
    "                fillqclic minbpp maxbpp npoints qfileprefix [actbpp?]\n");
  DD_printf(&job->ErrDD,
    "                qclic file\n");
  if (job->UseLagrangian)
    DD_printf(&job->ErrDD,
      "                deltabpp val/deltamse val/deltalambda val\n");
}

static char *seqScanScript1 = "\
0: 0 63 0 0;\0";
static char *seqScanScript3 = "\
0,1,2: 0 63 0 0;\0";

int main(int argc, char *argv[])
{
  char *cmdfile = 0;
  char *errfile = 0;
  FILE *errstream = stderr; 

  RD_Job TheJob;
  Image TheImage; 

  char command[RD_STRLENMAX], *nextcmdchar, foo[RD_STRLENMAX]; 
  char qfname[RD_STRLENMAX], cfname[RD_STRLENMAX]; 
  char scanfname[RD_STRLENMAX]; 
  char *scansrc = 0;
  long scansrcbytes = 0;
  int scansrckind = 0; 
  DataDest ddest, *dd = &ddest;
  DataSrc cmdsrc, *cmdds = &cmdsrc; 
  RD_float bpp, bpp_actual, bpp_correct = 0, psnr, rmse;
  long nbytes, nbytes_actual; 
  int i = 0;
  char *mintabs = 0; 
  char *maxtabs = 0; 
  char *cweights = 0;
  int num_uweights = 0;
  RD_float uweights[IM_MAXPLANES]; 
  int forceraw = 0;
  int nplanes=0, rows=0, cols=0;
  int sbytes=0, sbits = 0; 
  char *imfile = 0;
  int numtables = 0; 
  long thresh = 0; 
  char *method = 0; 
  int coarseness = 2;
  int verbose = 0; 
  int dcdpcm = 0;
  long bppscale = 0;
  RD_float bppmax = 0;
  long dcclamp = 0; 
  int toycc = 1;
  int chromsubsamp = 1;
  long maxsample; 
  double ftemp;
  long ltemp; 
  char *nextchar; 
  char *qfileprefix = 0, qfileprefname[RD_STRLENMAX]; 
  char curvefile[RD_STRLENMAX];
  int cnpoints; 
  double cminbpp, cmaxbpp; 
  int printstats = 0; 
  int act_bpp = 0; 
  DataDest cddest, *cdd = &cddest; 

  #ifdef HAVE_JPEGLIB
  int doingCompression = 0; 
  int doingCorrection = 0; 
  #endif


  while (++i<argc) {
    
    if (!strncmp(argv[i],"-dcdpcm",3)) {
      dcdpcm = 1; 
    } else if (!strncmp(argv[i],"-silent",4)) {
      verbose = 0; 
    } else if (!strncmp(argv[i],"-raw",4)) {
      if (++i >= argc) BriefUsage();
      if ((cols = atoi(argv[i])) <= 0) {
	FatalError("Image width must be > 0!");
      }
      if (++i >= argc) BriefUsage();
      if ((rows = atoi(argv[i])) <= 0) {
	FatalError("Image height must be > 0!");
      }
      if (++i >= argc) BriefUsage();
      if ((nplanes = atoi(argv[i])) <= 0) {
	FatalError("Number of color planes must be > 0!");
      } else if (nplanes > IM_MAXPLANES) { 
	FatalError("Number of color planes too high");
      } 
      if (++i >= argc) BriefUsage();
      if (((sbytes = atoi(argv[i])) != 1) && (sbytes != 2) ) 
	FatalError("Sample bytes must be 1 or 2"); 
      forceraw = 1; 
    } else if (!strncmp(argv[i],"-v",2)) { 
      verbose++; 
    } else if (!strncmp(argv[i],"-sbits",4)) { 
      if (++i>=argc) BriefUsage();
      if (((sbits = atoi(argv[i])) != 8)  && 
	  (sbits != 10) && (sbits != 12) && (sbits != 16))
	FatalError("SampleBits must be 8, 10, 12, or 16"); 
    } else if (!strncmp(argv[i],"-method",4)) {
      if (++i>=argc) BriefUsage();
      method = argv[i]; 
    } else if (!strncmp(argv[i],"-coarseness",5)) {
      if (++i>=argc) BriefUsage();
      if ((coarseness = atoi(argv[i])) < 0)  
        FatalError("Bad value in -coarseness");
    } else if (!strncmp(argv[i],"-numtables",5)) {
      if (++i>=argc) BriefUsage();
      if (((numtables = atoi(argv[i])) <= 0)  ||
	  (numtables > IM_MAXPLANES)) {
	    FatalError("Bad value in -numtables");
      }
    } else if (!strncmp(argv[i],"-clampDC",5)) {
      if (++i>=argc) BriefUsage();
      if ((dcclamp = (long) atoi(argv[i])) <= 0)
	FatalError("Bad value in -clampDC");
    } else if (!strncmp(argv[i],"-dontclampDC",6)) {
      dcclamp = 0; 
    } else if (!strncmp(argv[i],"-mintable",5)) {
      if (++i >= argc) BriefUsage();
      mintabs = argv[i]; 
    } else if (!strncmp(argv[i],"-maxtable",5)) {
      if (++i >= argc) BriefUsage();
      maxtabs = argv[i]; 
    } else if (!strncmp(argv[i],"-bppmax",5)) {
      if (++i >= argc) BriefUsage();
      if ((bppmax = ((RD_float) atof(argv[i]))) <= 0) 
	FatalError("Bad value in -bppmax");
    } else if (!strncmp(argv[i],"-bppscale",5)) {
      if (++i >= argc) BriefUsage();
      if ((bppscale = (long) atoi(argv[i])) <= 1) 
	FatalError("Bad value in -bppscale");
    } else if (!strncmp(argv[i],"-thresh",3)) {
      if (++i >= argc) BriefUsage();
      if (((thresh = (long) atoi(argv[i]))  < 0) ||
	   (thresh > RD_QENTRY_MAX) ) 
	FatalError("Bad value in -thresh");
    } else if (!strncmp(argv[i],"-weights",4)) {
      if (++i >= argc) BriefUsage();
      cweights = argv[i]; 
    } else if (!strncmp(argv[i],"-pweights",3)) {
      if (++i >= argc) BriefUsage();
      num_uweights = 0;
      nextchar = argv[i];
      while (nextchar && sscanf(nextchar,"%lf",&ftemp)==1) {
	uweights[num_uweights++] = (RD_float) ftemp; 
	nextchar = strchr(nextchar,',');
	if (nextchar) nextchar++; 
      } 
    } else if (!strncmp(argv[i],"-errfile",5)) {
      if (++i >= argc) BriefUsage();
      errfile = argv[i]; 
    } else if (!strncmp(argv[i],"-help",4)) {
      Usage();
    } else if (!strncmp(argv[i],"-cmdfile",4)) {
      if (++i >= argc) BriefUsage();
      cmdfile = argv[i]; 
    } else if (!strncmp(argv[i],"-im",3)) {
      if (++i >= argc) BriefUsage();
      imfile = argv[i]; 
    } else if (!strncmp(argv[i],"-rgbtoycc",7)) {
      toycc = 1;
      chromsubsamp = 0; 
    } else if (!strncmp(argv[i],"-rgbto2ycc",8)) {
      toycc = 1;
      chromsubsamp = 1; 
    } else if (!strncmp(argv[i],"-noycc",5)) {
      toycc = 0;
      chromsubsamp = 0; 
    } else {
      if (i==(argc-1) && !imfile)
	imfile = argv[i];
      else 
        BriefUsage();
    }
  }
  if (errfile) {
    if (!strcmp(errfile,"-")) errstream = stdout; 
    else if ((errstream = fopen(errfile,"w")) == NULL)
      errstream = stderr;
  } 

  ImInitImg(&TheImage);
  ImSetErrStream(&TheImage, errstream);

  #ifndef HAVE_JPEGLIB 
  ImDiscardOriginal(&TheImage);
  #endif 

  if (verbose == 0) ImSetSilent(&TheImage);
  if (forceraw) { 
    ImSetKindRaw(&TheImage, nplanes, rows, cols, 1); 
    for (i=0;i<nplanes;i++) ImSetSampleBits(&TheImage, sbytes*8, i);  
  } 
  if (!ImSetImgFile(&TheImage, imfile, 0)) 
    FatalError("Could not open image file");
  if (!ImPeekImg(&TheImage)) 
    FatalError("Cound not peek into image file");
  sbytes = ImAllSampleBytesEqual(&TheImage);
  if ( (sbytes != 1) && (sbytes != 2))
    FatalError("Image format must have 8 or 16 bits per pel"); 
  if (!sbits) sbits = sbytes*8;
  else {
    if ((sbytes*8) < sbits) FatalError("Bad value in -sbits"); 
  } 
  nplanes = ImPlanesOfImg(&TheImage); 
  if (!numtables) numtables = nplanes;
  else if (numtables > nplanes) numtables = nplanes; 

  if (!ImReadImg(&TheImage)) 
    FatalError("Cound not read image file");

  if (nplanes==3) {
    if (toycc) {
      if (!ImChangeCspace(&TheImage,ImC_YCbCr))
	FatalError("Could not do RGB->YCbCr");
    }
    if (chromsubsamp) { 
      if (!ImStretchAndShrink(&TheImage, 1, 1, 2, 1, 2, 0, 0) ||
          !ImStretchAndShrink(&TheImage, 2, 1, 2, 1, 2, 0, 0)) 
	FatalError("Could not subsample");
    } 
  }

  maxsample = (1L << sbits) - 1; 

  /* all done with reading the image */
  RD_Init(&TheJob, &TheImage);

  RD_SetErrDest(&TheJob, errstream, DD_KIND_STREAM);
  RD_SetMaxSample(&TheJob, maxsample); 
  RD_SetVerboseLevel(&TheJob, verbose); 
  RD_SetNumTables(&TheJob, numtables);
  if (method) RD_SetOptMethod(&TheJob, method);
  if (dcclamp) RD_SetDCclamp(&TheJob, dcclamp);
  if (dcdpcm) RD_SetDCDPCM(&TheJob, dcdpcm);
  if (num_uweights) RD_SetUnitWeights(&TheJob, num_uweights, uweights);
  if (mintabs) RD_SetMinTables(&TheJob, 0, mintabs, 0, DS_KIND_FILE); 
  if (maxtabs) RD_SetMaxTables(&TheJob, 0, maxtabs, 0, DS_KIND_FILE); 
  if (cweights) RD_SetCoefWeights(&TheJob, 0,cweights, 0, DS_KIND_FILE);
  if (bppmax) RD_SetBppMax(&TheJob, bppmax); 
  if (bppscale) RD_SetBppScale(&TheJob, bppscale); 
  if (thresh) RD_SetThreshSpan(&TheJob, thresh); 
  if (coarseness) RD_SetCoarseness(&TheJob, coarseness); 

  if (!RD_Optimize(&TheJob)) FatalError("Failed to optimize");
  

  DS_Init(cmdds, 0);
  if (cmdfile && !strcmp(cmdfile,"-")) cmdfile = 0; 
  if (!DS_OpenFile(cmdds, cmdfile, 0))
    FatalError("Error opening command src"); 

  strcpy(qfname,"-");
  sprintf(cfname,"%s.%s.jpg",ImNameOfImg(&TheImage),RD_IDSTRING);

  while (1) {
    if (!cmdfile) {fprintf(stdout,"Command> ");fflush(stdout);}

    #ifdef HAVE_JPEGLIB
    doingCorrection = 0;
    doingCompression = 0;
    #endif

    if (!DS_ReadLine(cmdds, command, RD_STRLENMAX)) break;

    if (blankline(command)) continue;

    nextcmdchar = command;
    while (isspace(*nextcmdchar) && (*nextcmdchar != '\0'))
      nextcmdchar++; 

    if (nextcmdchar[0] == '#') continue; 

    #ifdef HAVE_JPEGLIB
    if (!strncasecmp(nextcmdchar,"sequential",3)) {
      if (nplanes==1) 
        scansrc = seqScanScript1; 
      else 
        scansrc = seqScanScript3; 
      scansrcbytes = (long) strlen(scansrc); 
      scansrckind = DS_KIND_MEMORY; 
      continue;
    }
    if (!strncasecmp(nextcmdchar,"progressive",4)) {
      if (sscanf(nextcmdchar,"%s%s",foo,scanfname) == 2) {
	scansrc = scanfname;
	scansrcbytes = 0L; 
	scansrckind = DS_KIND_FILE; 
      } else {
	scansrc = (char *) 0; 
	scansrcbytes = 0L;
	scansrckind = 0; 
      } 
      continue;
    }
    #endif

    if (!strncasecmp(nextcmdchar,"qfile",2)) {
      sscanf(nextcmdchar,"%s%s",foo,qfname);
      continue;
    }
    if (!strncasecmp(nextcmdchar,"cfile",2)) {
      sscanf(nextcmdchar,"%s%s",foo,cfname);
      continue;
    }
    if (TheJob.UseLagrangian) {
      if (!strncasecmp(nextcmdchar,"deltabpp",6)) {
	ftemp = (double) -1; 
	sscanf(nextcmdchar,"%s%lf",foo,&ftemp);
	if (ftemp != (double) -1)
	  RD_LagrSetDeltaBpp(&TheJob, (RD_float) ftemp);
	continue; 
      }
      if (!strncasecmp(nextcmdchar,"deltamse",6)) {
	ftemp = (double) -1; 
	sscanf(nextcmdchar,"%s%lf",foo,&ftemp);
	if (ftemp != (double) -1)
	  RD_LagrSetDeltaMse(&TheJob, (RD_float) ftemp);
	continue; 
      }
      if (!strncasecmp(nextcmdchar,"deltalambda",6)) {
	ftemp = (double) -1; 
	sscanf(nextcmdchar,"%s%lf",foo,&ftemp);
	if (ftemp != (double) -1)
	  RD_LagrSetDeltaLambda(&TheJob, (RD_float) ftemp);
	continue; 
      }
    }

    if (!strncasecmp(nextcmdchar,"stats",3)) {
      printstats = 1;
      continue; 
    } 
    if (!strncasecmp(nextcmdchar,"nostats",4)) {
      printstats = 0;
      continue; 
    } 

    if (!strncasecmp(nextcmdchar,"nocorrect",5)) {
      RD_ApplyCorrection(&TheJob, (RD_float) 0);
      bpp_correct = 0; 
      continue;
    }
    
    if (!strncasecmp(nextcmdchar,"help",2)) {
      BriefListCommands(&TheJob);
      continue;
    }

    if (!strncasecmp(nextcmdchar,"quit",4)) break;

    if (!strncasecmp(nextcmdchar,"curve",2)) {
      qfileprefname[0] = '\0';
      if (sscanf(nextcmdchar,"%s%s%lf%lf%d%d%s", foo, curvefile,
	  &cminbpp, &cmaxbpp, &cnpoints, &act_bpp, qfileprefname) < 6) {
	    BriefListCommands(&TheJob);
	    continue;
      }

      if (qfileprefname[0] == '\0') qfileprefix = 0;
      else qfileprefix = qfileprefname;

      DD_Init(cdd);
      if (!DD_OpenFile(cdd, curvefile)) {
	fprintf(stdout,"Could not open curve file %s\n", curvefile);
        DD_Close(cdd);
	continue;
      }
      if (!RD_BppPsnrCurve(&TheJob, (RD_float) cminbpp, (RD_float) cmaxbpp,
	   cnpoints, printstats, 1, cdd, qfileprefix, act_bpp,
	   scansrc, scansrcbytes, scansrckind)) {
	fprintf(stdout,"Failed to generate curve\n");
      }
      DD_Close(cdd);
      continue;
    }

    if (!strncasecmp(nextcmdchar,"fillqclic",4)) {
      act_bpp = 0; 
      if (sscanf(nextcmdchar,"%s%lf%lf%d%s%d", foo, 
	  &cminbpp, &cmaxbpp, &cnpoints, qfileprefname, &act_bpp) < 5) {
	    BriefListCommands(&TheJob);
	    continue;
      }
      if (!RD_FillQclicCurve(&TheJob, (RD_float) cminbpp, (RD_float) cmaxbpp,
	   cnpoints, qfileprefname, act_bpp,
	   scansrc, scansrcbytes, scansrckind)) {
	fprintf(stdout,"Failed to generate quality profile\n");
      }
      continue;
    }

    if (!strncasecmp(nextcmdchar,"qclic",4)) {
      if (sscanf(nextcmdchar,"%s%s", foo, curvefile) != 2) { 
	    BriefListCommands(&TheJob);
	    continue;
      }
      DD_Init(cdd);
      if (!DD_OpenFile(cdd, curvefile)) {
	fprintf(stdout,"Could not open curve file %s\n", curvefile);
        DD_Close(cdd);
	continue;
      }
      if (!ImWriteQclicInfo(&TheImage, cdd)) { 
	fprintf(stdout,"Failed to write Qclic info\n");
      }
      DD_Close(cdd);
      continue;
    }


    #ifdef HAVE_JPEGLIB
    if (!strncasecmp(nextcmdchar,"compress",4)) {
      doingCompression = 1;
      while (!isspace(*nextcmdchar) && (*nextcmdchar != '\0'))
	nextcmdchar++;
      while (isspace(*nextcmdchar) && (*nextcmdchar != '\0'))
	nextcmdchar++;
    } else if (!strncasecmp(nextcmdchar,"correct",4)) {
      doingCorrection = 1;
      while (!isspace(*nextcmdchar) && (*nextcmdchar != '\0'))
	nextcmdchar++;
      while (isspace(*nextcmdchar) && (*nextcmdchar != '\0'))
	nextcmdchar++;
    }
    if (doingCompression || doingCorrection) {
      if (sbytes != 1) { 
	fprintf(stdout,"Can only compress 8-bit images\n");
	continue;
      }
      if (nplanes==3) {
	if (!toycc || !chromsubsamp) {
	  fprintf(stdout,"Can only compress RGB images when YCbCr-space with chroma-subsampling is used\n");
	  continue; 
	} 
      } else if (nplanes != 1) { 
	fprintf(stdout,"Can only compress grayscale or RGB images\n");
	continue;
      }
    } 

    #endif

    DD_Init(dd); 

    #ifdef HAVE_JPEGLIB
    if (doingCompression || doingCorrection) {
      DD_Use_Auto_Mem(dd);
      DD_OpenMemory(dd,0,0);
      if (doingCorrection) RD_ApplyCorrection(&TheJob, (RD_float) 0);
    } else { 
      DD_OpenFile(dd, qfname);
    }
    #else
    DD_OpenFile(dd, qfname);
    #endif

    if (!strncasecmp(nextcmdchar,"size",3)) {
      ltemp = 0; 
      sscanf(nextcmdchar,"%s%ld",foo,&ltemp);
      RD_GetQForSize(&TheJob, ltemp, printstats,
        &nbytes, &bpp, &psnr, &rmse, dd); 

    } else if (!strncasecmp(nextcmdchar,"bpp", 3)) {
      ftemp = 0; 
      sscanf(nextcmdchar,"%s%lf",foo,&ftemp);
      RD_GetQForBpp(&TheJob, (RD_float) ftemp, printstats,
        &nbytes, &bpp, &psnr, &rmse, dd); 

    } else if (!strncasecmp(nextcmdchar,"rmse", 3)) {
      ftemp = 0; 
      sscanf(nextcmdchar,"%s%lf",foo,&ftemp);
      RD_GetQForRmse(&TheJob, (RD_float) ftemp, printstats,
        &nbytes, &bpp, &psnr, &rmse, dd); 

    } else if (!strncasecmp(nextcmdchar,"psnr", 3)) {
      ftemp = 0; 
      sscanf(nextcmdchar,"%s%lf",foo,&ftemp);
      RD_GetQForPsnr(&TheJob, (RD_float) ftemp, printstats,
        &nbytes, &bpp, &psnr, &rmse, dd); 
    } else if (!strncasecmp(nextcmdchar,"snr", 3)) {
      ftemp = 0; 
      sscanf(nextcmdchar,"%s%lf",foo,&ftemp);
      RD_GetQForSnr(&TheJob, (RD_float) ftemp, printstats,
        &nbytes, &bpp, &psnr, &rmse, dd); 
    } else {
      BriefListCommands(&TheJob); 
      DD_Close(dd);
      if (doingCorrection) RD_ApplyCorrection(&TheJob, bpp_correct); 
      continue;
    }

    #ifdef HAVE_JPEGLIB
    if (doingCompression) {

      if (!cmdfile) {
	fprintf(stdout,"Predicted %ld bytes (%f bpp), %f dB PSNR (%f rmse).\n",
	  nbytes, (float) bpp, (float) psnr, (float) rmse );
	fflush(stdout);
      }

      nbytes_actual = ImSaveJpeg(&TheImage, 1, 0, 0, -1, -1, 0,
	cfname, 0, 0, 0,
	DD_auto_mem(dd), DD_auto_mem_level(dd), DS_KIND_MEMORY,
	scansrc, scansrcbytes, scansrckind);

      DD_Free_Auto_Mem(dd); 

      bpp_actual = ((RD_float) (nbytes_actual*8))/TheJob.NumPixelsForBpp;

      if (!cmdfile) {
	fprintf(stdout,"Saved %ld bytes (%f bpp).\n",
	  nbytes_actual, (float) bpp_actual); 
	fflush(stdout);
      }
    } else if (doingCorrection) {

      DD_Init(cdd);
      DD_OpenNull(cdd); 

      if (!cmdfile) {
	fprintf(stdout,"Predicted %ld bytes (%f bpp), %f dB PSNR (%f rmse).\n",
	  nbytes, (float) bpp, (float) psnr, (float) rmse );
	fflush(stdout);
      }

      nbytes_actual = ImSendJpeg(&TheImage, 1, 0, 0, -1, -1,  0,
	cdd, 0, 0, 0, 0, 0, 
	DD_auto_mem(dd), DD_auto_mem_level(dd), DS_KIND_MEMORY,
	0, 0, 0);

      DD_Free_Auto_Mem(dd); 
      DD_Close(cdd); 

      bpp_actual = ((RD_float) (nbytes_actual*8))/TheJob.NumPixelsForBpp;
      bpp_correct = bpp_actual - bpp; 

      RD_ApplyCorrection(&TheJob, (RD_float) bpp_correct);

      if (!cmdfile) {
	fprintf(stdout,"Actual: %ld bytes (%f bpp). Correction = %f\n",
	  nbytes_actual, (float) bpp_actual, (float) bpp_correct); 
	fflush(stdout);
      }
    }

    #endif

    DD_Close(dd);
  }

  ImFreeImg(&TheImage);
  RD_Free(&TheJob); 
  return 0; 
}

