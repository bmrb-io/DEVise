#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Image.h"
#include "RDOPT.h" 

Image Im;

void Usage()
{
  fprintf(stderr,"Usage: ImCjpeg [-outfile file][-qclicfile file][-qmetric qm]\n"); 
  fprintf(stderr,"  [-rdopt][-nordopt] [-rdcoarse c] [-rdthresh th]\n"); 
  fprintf(stderr,"  [-rdtarget metric rdtarg] [-rdmaxtab maxt]\n"); 
  fprintf(stderr,"  [-qtables file][-scans file][-sequential]\n");
  fprintf(stderr,"  [-ntries n] [-negtol del] [-postol del]\n");
  fprintf(stderr,"  [-target target_metric target] [imfile]\n");

  exit(1); 
}

static char rd_def_maxtab[700] = "\
64 64 64 64 64 64 64 64 64 64 64 64 64 64 64 64\n\
64 64 64 64 64 64 64 64 64 64 64 64 64 64 64 64\n\
64 64 64 64 64 64 64 64 64 64 64 64 64 64 64 64\n\
64 64 64 64 64 64 64 64 64 64 64 64 64 64 64 64\n\
64 64 64 64 64 64 64 64 64 64 64 64 64 64 64 64\n\
64 64 64 64 64 64 64 64 64 64 64 64 64 64 64 64\n\
64 64 64 64 64 64 64 64 64 64 64 64 64 64 64 64\n\
64 64 64 64 64 64 64 64 64 64 64 64 64 64 64 64\n\
64 64 64 64 64 64 64 64 64 64 64 64 64 64 64 64\n\
64 64 64 64 64 64 64 64 64 64 64 64 64 64 64 64\n\
64 64 64 64 64 64 64 64 64 64 64 64 64 64 64 64\n\
64 64 64 64 64 64 64 64 64 64 64 64 64 64 64 64\n\0\
";

static char *seqScanScript1 = "\
0: 0 63 0 0;\0"; 
static char *seqScanScript3 = "\
0,1,2: 0 63 0 0;\0"; 


int
main(int argc, char *argv[])
{
  int i; 
  int do_qcurve = 0; 
  int act_bpp = 0; 
  int do_rdopt = 1;
  int rd_coarseness = 7;
  long rd_thresh = 30; 
  char *rd_maxtab = 0; 
  float rd_target = 0;
  int rd_target_kind = 0; 
  int do_sequential = 0; 
  RD_Job rdjob;
  long nbytes = 0;
  RD_float bpp, psnr, rmse;

  char *outfname = (char *) 0; 
  char *imfname = (char *) 0;
  char *qclicfname = (char *) 0;
  char *qmetric = (char *) 0;
  int qtab_src_kind = DS_KIND_NONE; 
  int scan_src_kind = DS_KIND_NONE; 
  char *qtabfname = (char *) 0;
  long qtabbytes = 0; 
  char *scanfname = (char *) 0;
  long scanbytes = 0; 
  long qclicbytes = 0; 
  void * target = 0;
  long target_size = 0;
  float float_target = 0; 
  char *target_qm = 0;
  int retries = 100;
  float negtol = (float) -1.0, postol = (float) -1.0; 
  int negtolset = 0, postolset = 0; 
  int target_kind = 0; 
  DataDest rdqdest, *rdqdd = 0; 
  float curve_minbpp = 0, curve_maxbpp = 1.5;
  int curve_npoints = 15; 
  char curve_qprefix[IM_STRLENMAX]; 



  i = 0;
  while (++i < argc) {
    if (!strncmp(argv[i], "-outfile", 4)) {
      if (++i >= argc) Usage(); 
      outfname = argv[i];
    } else if (!strncmp(argv[i], "-rdopt", 4)) {
      do_rdopt = 1; 
    } else if (!strncmp(argv[i], "-nordopt", 4)) {
      do_rdopt = 0; 
    } else if (!strncmp(argv[i], "-qcurve", 4)) {
      do_qcurve = 1; 
      act_bpp = 0; 
    } else if (!strncmp(argv[i], "-actqcurve", 4)) {
      do_qcurve = 1; 
      act_bpp = 1; 
    } else if (!strncmp(argv[i], "-sequential", 4)) {
      do_sequential = 1; 
    } else if (!strncmp(argv[i], "-rdcoarseness", 5)) {
      if (++i >= argc) Usage(); 
      rd_coarseness = atoi(argv[i]); 
    } else if (!strncmp(argv[i], "-rdthresh", 5)) {
      if (++i >= argc) Usage(); 
      rd_thresh = (long) atoi(argv[i]); 
    } else if (!strncmp(argv[i], "-qtables", 4)) {
      if (++i >= argc) Usage(); 
      qtabfname = argv[i];
      qtab_src_kind = DS_KIND_FILE; 
    } else if (!strncmp(argv[i], "-rdmaxtab", 5)) {
      if (++i >= argc) Usage(); 
      rd_maxtab = argv[i];
    } else if (!strncmp(argv[i], "-scans", 4)) {
      if (++i >= argc) Usage(); 
      scanfname = argv[i];
      scan_src_kind = DS_KIND_FILE; 
    } else if (!strncmp(argv[i], "-qclicfile", 4)) {
      if (++i >= argc) Usage(); 
      qclicfname = argv[i];
    } else if (!strncmp(argv[i], "-qmetric", 4)) {
      if (++i >= argc) Usage(); 
      qmetric = argv[i];
    } else if (!strncmp(argv[i], "-target", 4)) {
      if (++i >= argc) Usage(); 
      target_qm = argv[i];
      target_kind = QmetricKind(target_qm);
      if (++i >= argc) Usage(); 
      if (target_kind == QM_SIZE) {
	sscanf(argv[i],"%ld",&target_size);
        target = &target_size; 
      } else {
	sscanf(argv[i],"%f",&float_target);
        target = &float_target; 
      } 
    } else if (!strncmp(argv[i], "-rdtarget", 6)) {
      if (++i >= argc) Usage(); 
      rd_target_kind = QmetricKind(argv[i]); 
      if (++i >= argc) Usage(); 
      sscanf(argv[i],"%f",&rd_target); 
    } else if (!strncmp(argv[i], "-ntries", 4)) {
      if (++i >= argc) Usage(); 
      retries = atoi(argv[i]); 
    } else if (!strncmp(argv[i], "-negtolerance", 4)) {
      if (++i >= argc) Usage(); 
      negtol = (float) atof(argv[i]); negtolset = 1; 
    } else if (!strncmp(argv[i], "-postolerance", 4)) {
      if (++i >= argc) Usage(); 
      postol = (float) atof(argv[i]); postolset = 1; 
    } else if (i == (argc - 1)) { 
      imfname = argv[i]; 
    } else {
      Usage();
    }
  } 

  ImInitImg(&Im);
  if (!ImSetImgFile(&Im, imfname, 0)) {
      fprintf(stderr, "Error opening file\n");
      return(1);
  }

  if (!ImPeekImg(&Im)) {
    fprintf(stderr, "Unknown format\n");
    return(1);
  }
  if (!ImReadImg(&Im)) {
    fprintf(stderr, "Error reading image\n"); 
    ImFreeImg(&Im);
    return(1);
  }
  
  if (target && (!negtolset || !postolset)) {
    if (QM_NEEDS_PSNR(target_kind)) {
      if (!negtolset) negtol = (float) 0.05; 
      if (!postolset) postol = (float) -1; 
    } else if (target_kind == QM_SIZE) {
      if (!negtolset) negtol = (float) 100; 
      if (!postolset) postol = (float) 0; 
    } else if (target_kind == QM_PSIZE) {
      if (!negtolset) negtol = (float) 1; 
      if (!postolset) postol = (float) 0; 
    } else if (target_kind == QM_BPP) {
      if (!negtolset) negtol = (float) 0.02; 
      if (!postolset) postol = (float) 0; 
    } 
  } 

  if (qtabfname) do_rdopt = 0;
  if (ImIsCompressed(&Im)) do_rdopt = 0;

  if (!ImIsCompressed(&Im) && ImHasQclicCurve(&Im) &&
       !strncasecmp(ImQclicCurveFormat(&Im),"JPEG",4)) do_rdopt = 0;

  if (do_sequential) {
    if (ImPlanesOfImg(&Im)==1) {
      scanfname = seqScanScript1; 
    } else { 
      scanfname = seqScanScript3; 
    }
    scanbytes = (long) strlen(scanfname); 
    scan_src_kind = DS_KIND_MEMORY;
  } 

  if (do_rdopt) {

    if (ImPlanesOfImg(&Im)==3) { 
      if (!ImChangeCspace(&Im,ImC_YCbCr) || 
          !ImStretchAndShrink(&Im, 1, 1, 2, 1, 2, 0, 0) ||
          !ImStretchAndShrink(&Im, 2, 1, 2, 1, 2, 0, 0))  { 
        fprintf(stderr, "Error subsampling: not running rdopt\n");
        goto do_compress; 
      }
    }  

    RD_Init(&rdjob, &Im); 

    if (rd_target <= 0) { 
      if (!target) {
        rd_target = 1.0;
        rd_target_kind = QM_BPP;
      } else {
        if (target_kind == QM_SIZE) {
	  rd_target = (float) target_size;
          rd_target_kind = QM_SIZE;
        } else { 
	  rd_target = float_target; 
          rd_target_kind = target_kind; 
	} 
        if (!QM_NEEDS_PSNR(rd_target_kind) && (retries > 1)
	      && (postol >= 0)) { 
           rd_target *= 1.05; /* try to ensure an over-estimate */
    } } }   

    if (rd_target_kind == QM_SIZE) {
      rd_target *= ((float)8.0/
	 ((float)ImOrigRows(&Im)*ImOrigCols(&Im))); 
      rd_target_kind = QM_BPP;
    } else if (rd_target_kind == QM_PSIZE) {
      rd_target *= ((float) 8.0* ((float) ImPlanesOfImg(&Im))/100.0); 
      rd_target_kind = QM_BPP;
    } 
    
    RD_SetCoarseness(&rdjob, rd_coarseness);

    if (rd_maxtab) { 
      RD_SetMaxTables(&rdjob, 0, rd_maxtab, 0, DS_KIND_FILE); 
    } else { 
      if ( ((rd_target_kind == QM_BPP) && (rd_target > 1.0)) ||
         ((rd_target_kind == QM_PSNR) && (rd_target > 35.0)) )
        RD_SetMaxTables(&rdjob, 0, rd_def_maxtab, strlen(rd_def_maxtab),
	   DS_KIND_MEMORY); 
    } 

    #ifdef HAVE_IJG_EXTENSIONS
    RD_SetThreshSpan(&rdjob, rd_thresh); 
    #endif

    fprintf(stderr,"Running RDOPT.."); fflush(stderr);
    if (!RD_Optimize(&rdjob)) {
      fprintf(stderr,"..Error! Ignored.\n"); fflush(stderr);
      RD_Free(&rdjob);
      goto do_compress; 
    } 
    fprintf(stderr,"..Done!\n"); fflush(stderr);
    
    rdqdd = &rdqdest; 
    DD_Init(rdqdd);
    DD_Use_Auto_Mem(rdqdd);
    DD_OpenMemory(rdqdd,0,0);


    if ( rd_target_kind == QM_BPP) 
      RD_GetQForBpp(&rdjob, (RD_float) rd_target, 1, 
        &nbytes, &bpp, &psnr, &rmse, rdqdd); 
    else 
      RD_GetQForPsnr(&rdjob, (RD_float) rd_target, 1, 
        &nbytes, &bpp, &psnr, &rmse, rdqdd); 
    if (do_qcurve && qclicfname && !ImHasQclicCurve(&Im)) {
      sprintf(curve_qprefix,"%s.RDOPT.Q",ImNameOfImg(&Im)); 
      RD_FillQclicCurve(&rdjob, curve_minbpp, curve_maxbpp,
	curve_npoints, curve_qprefix, act_bpp,
        scanfname, scanbytes, scan_src_kind); 
    } 
    RD_Free(&rdjob); 

    qtabfname = DD_auto_mem(rdqdd);
    qtabbytes = DD_auto_mem_level(rdqdd); 
    qtab_src_kind = DS_KIND_MEMORY; 
  }


  do_compress:
  if (!ImSaveJpeg(&Im, 1, 
     target, target_qm, negtol, postol, retries, outfname,
     qclicfname, &qclicbytes, qmetric, 
     qtabfname, qtabbytes, qtab_src_kind, 
     scanfname, scanbytes, scan_src_kind)) { 
    fprintf(stderr, "Error compressing image\n"); 
    ImFreeImg(&Im);
    if (rdqdd) {
      DD_Free_Auto_Mem(rdqdd);
      DD_Close(rdqdd); 
    }  
    return(1);
  }
    
  ImFreeImg(&Im);
  if (rdqdd) {
    DD_Free_Auto_Mem(rdqdd);
    DD_Close(rdqdd); 
  } 
  return(0);
}

