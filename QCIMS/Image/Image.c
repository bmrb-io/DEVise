#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <setjmp.h>
#include <math.h>


#define NEED_IM_KIND_NAMES
#include "Image.h"
#include "ImageInternal.h"


char ImErrorString[IM_STRLENMAX];

extern int InnerImFatalError(Image *Im, char *s, int copy_reason)
{
  if (!Im->Silent) (void) DD_WriteLine(&Im->ImgErr, s); 
  if (copy_reason) strcpy(ImErrorString, s);
  ImFreeImg(Im);
  return(0);
}

extern int InnerImNonFatalError(Image *Im, char *s, int copy_reason)
{
  if (!Im->Silent) (void) DD_WriteLine(&Im->ImgErr, s); 
  if (copy_reason) strcpy(ImErrorString, s);
  return(0);
}



static int GetImKind(Image *Im);

static int ReadRawImg(Image *Im);
static int ReadMoreRawImg(Image *Im);

static int ReadPnmHeader(Image *Im, long *maxv);
static int ReadPgmAsciiImg(Image *Im);
static int ReadMorePgmAsciiImg(Image *Im);
static int ReadPgmRawImg(Image *Im);
static int ReadMorePgmRawImg(Image *Im);
static int ReadPpmAsciiImg(Image *Im);
static int ReadMorePpmAsciiImg(Image *Im);
static int ReadPpmRawImg(Image *Im);
static int ReadMorePpmRawImg(Image *Im);

#ifdef HAVE_JPEGLIB
extern int ReadJpegHeader(Image *Im);
extern int ReadJpegImg(Image *Im);
extern int ReadMoreJpegImg(Image *Im);
extern void FreeImgJpeg(Image *Im);
#endif

#ifdef HAVE_SPIHT
extern int ReadSpihtHeader(Image *Im, int *pelbytes);
extern int ReadSpihtImg(Image *Im);
extern int ReadMoreSpihtImg(Image *Im);
extern void FreeImgSpiht(Image *Im);
#endif


extern int ImSetErrFile(Image *Im, char *f)
{
  if ((Im->ImState < ImStateInited) || (Im->ImState >= ImStateDead))
    return 0; 
  return DD_OpenFile(&Im->ImgErr, f); 
}

extern int ImSetErrStream(Image *Im, FILE *fp) 
{
  if ((Im->ImState < ImStateInited) || (Im->ImState >= ImStateDead))
    return 0; 
  return DD_OpenStream(&Im->ImgErr, fp); 
}

extern int ImSetErrFd(Image *Im, int fd) 
{
  if ((Im->ImState < ImStateInited) || (Im->ImState >= ImStateDead))
    return 0; 
  return DD_OpenFd(&Im->ImgErr, fd); 
}


static void SetSbitsInner(Image *Im, int sbits, int planenum)
{
  Im->SampleBits[planenum] = sbits;
  Im->SampleBytes[planenum] = sbits/8;
  if ((Im->SampleBytes[planenum]*8) != Im->SampleBits[planenum])
      Im->SampleBytes[planenum]++;
  Im->MaxSample[planenum] = (1 << Im->SampleBits[planenum]) - 1;
}

extern int ImSetSampleBits(Image *Im, int sbits, int planenum)
{
  int i;

  if (Im->ImState <= ImStateNotExtant)
    ImFatalError("ImSetSampleBits: No image here yet!");
  else if (Im->ImState > ImStatePeeked)
    ImFatalError("ImSetSampleBits: Too late to set");

  if (Im->ImKind != IM_RAW)
      ImFatalError("ImSetSampleBits: Can only set for RAW images");

  if ((sbits > 32) || (sbits < 1))
    ImFatalError("ImSetSampleBits: bits per sample must be <= 32 and >= 1");

  if (planenum < 0) {
    for (i=0;i<Im->NumPlanes;i++) {
      SetSbitsInner(Im, sbits, i);
    }
  } else {
    if (planenum >= Im->NumPlanes)
      ImFatalError("ImSetSampleBits: Plane number too high");
    SetSbitsInner(Im, sbits, planenum);
  }
  return(1);
}


extern int  ImSetKindRaw(Image *Im, int nplanes, int rows, int cols, boolean enforce)
{
  if (Im->ImState <= ImStateNotExtant) ImFatalError("ImSetKindRaw: No image here yet!");
  else if (Im->ImState >= ImStatePeeked) ImFatalError("ImSetKindRaw: Too late to set");

  if ((nplanes < 1) || (nplanes > IM_MAXPLANES))
    ImFatalError("ImSetKindRaw: bad number of planes");

  Im->ForceImKind = enforce;
  Im->ImKind = IM_RAW;


  Im->NumPlanes = nplanes;
  Im->NumRows = rows;
  Im->NumCols = cols;

  return(1);

}

extern int  ImSetKind(Image *Im, int imkind, boolean enforce)
{

  if (Im->ImState <= ImStateNotExtant) ImFatalError("ImSetKind: No image here yet!");
  else if (Im->ImState >= ImStatePeeked) ImFatalError("ImSetKind: Too late to set");

  if ((imkind >= IM_KIND_NUM) || (imkind < 0)) 
    ImFatalError("ImSetKind: bad image kind");

  Im->ImKind = imkind;
  Im->ForceImKind = enforce;

  return(1);
}

extern int ImSetOrigCspace(Image *Im, ImColorSpaceType cspace)
{


  if (Im->ImState != ImStatePeeked)
    ImFatalError("ImSetOrigCspace: can call only after ImPeekImg");

  if ((Im->OrigCspace == ImC_None) || ImCspaceCompatible(cspace, Im->OrigCspace)) {
    Im->OrigCspace = cspace;
  } else {
    ImFatalError("ImSetOrigCspace: colorspace mismatch");
  }

  return(1);

}

extern void ImInitImg(Image *Im)
{
  int i;
  Im->ImState = ImStateInited;
  Im->Silent = FALSE;
  Im->NumRows = 512;
  Im->NumCols = 512;
  Im->NumPlanes = 1;
  Im->ImKind = IM_UNKNOWN;
  Im->ForceImKind = FALSE;
  Im->TotalImBytes = 0; 
  Im->GuessedTotalImBytes = TRUE; 
  DS_Init(&(Im->ImgSrc), 0); /* bogus init */ 
  Im->ImgSrcInited = FALSE; 

  DD_Init(&(Im->ImgErr));
  (void) DD_OpenStream(&(Im->ImgErr), stderr); 

  QclicInfoInit(&(Im->qclic)); 

  for (i=0;i<IM_MAXPLANES;i++) {
    Im->Changed[i] = FALSE;
    Im->RetainedData[i] = (char *) 0; 
    Im->SmoothingDone[i] = FALSE;
    Im->Brightness[i] = 0;
    Im->Contrast[i] = ((float) 1.0); 
    Im->FirstChangedRow[i] = 0; 
    Im->LastChangedRow[i] = 0; 
    SetSbitsInner(Im,8,i);
    Im->SrcEndian[i] = Endian_MSBFirst;
    Im->SaveEndian[i] = Endian_MSBFirst;
    Im->HostEndian[i] = Endian_MSBFirst; /* will be set correctly
					    by PeekImg */
  }
  Im->AllPlanesEqual = TRUE;
  Im->WasCropped = FALSE; 
  Im->OrigCspace = ImC_None;
  Im->ImName = Im->qclic.name; 
  strcpy(Im->ImName,"Unnamed");
  Im->RetainOrigOnChange = TRUE;
  strcpy(Im->ImFilterUsed,"none");
  
  #ifdef HAVE_SPIHT
  Im->spiht_data_bytes = 0; 
  #endif 

  #ifdef HAVE_JPEGLIB
  Im->rough_idct = JDCT_IFAST; 
  Im->final_idct = JDCT_FLOAT; 
  Im->jpg_im = (unsigned char *) 0;
  Im->jpg_outbuff = (JSAMPARRAY) 0;
  Im->jinfo_alive = FALSE;
  #endif
}

extern int ImPrintImgChars(Image *Im, DataDest *dd)
{
  int i; 
  char idstr[IM_STRLENMAX], *nextchar = idstr; 

  if (Im->ImState <= ImStateNotExtant) ImNonFatalError("ImPrintImgChars: No image here yet!");
  else if (Im->ImState >= ImStateDead) ImNonFatalError("ImPrintImgChars: image already dead");

  sprintf(nextchar,"%s: %s image: %d x %d x %d ", Im->ImName,
	       ImKindString(Im->ImKind), Im->NumCols,Im->NumRows,
	       Im->NumPlanes);
  nextchar += strlen(nextchar); 

  if (Im->ImState > ImStatePeeked) {
    if (Im->CurrCspace==ImC_YCbCr) {
      sprintf(nextchar,"(YCbCr-space) "); 
      nextchar += strlen(nextchar); 
    } 
    if (ImChanged(Im)) {  
      if (!Im->AllPlanesEqual) {
        for (i=0;i<Im->NumPlanes-1;i++) {
          sprintf(nextchar, "(%d x %d):",Im->CurrCols[i], Im->CurrRows[i]);
          nextchar += strlen(nextchar); 
        }
      }
      sprintf(nextchar, "(%d x %d) ",
	Im->CurrCols[Im->NumPlanes-1], Im->CurrRows[Im->NumPlanes-1]);
      nextchar += strlen(nextchar); 
    }
  }

  sprintf(nextchar, "Filter: %s Pixbits: ",Im->ImFilterUsed);
  nextchar += strlen(nextchar); 
  for (i=0;i<Im->NumPlanes-1;i++) {
    sprintf(nextchar, "%d ",Im->SampleBits[i]);
    nextchar += strlen(nextchar); 
  }
  sprintf(nextchar, "%d",Im->SampleBits[Im->NumPlanes-1]);
    nextchar += strlen(nextchar); 
  if (DD_WriteLine(dd, idstr) <= 0) return 0;

  return(1);
}

static void SetHostEndian(Image *Im)
{
  static int EndianKnown = FALSE;
  static int ShortEndian;
  static int LongEndian;
  int i;

  unsigned char *c;
  unsigned short sx;
  unsigned long sy;

  if (!EndianKnown) {
    sx = 0x0100;
    c = (unsigned char *) &sx;
    if (*c) ShortEndian = Endian_MSBFirst;
    else ShortEndian = Endian_LSBFirst;
    sy = 0x01000000;
    c = (unsigned char *) &sy;
    if (*c) LongEndian = Endian_MSBFirst;
    else LongEndian = Endian_LSBFirst;
    EndianKnown = TRUE;
  }

  for (i=0;i<Im->NumPlanes;i++) {
    if (Im->SampleBytes[i]==2) Im->HostEndian[i] = ShortEndian;
    else if (Im->SampleBytes[i]==4) Im->HostEndian[i] = LongEndian;
  }

}


extern int ImPeekImg(Image *Im)
{
  long maxval, tot;
  int tempKind;
  ImColorSpaceType guess_csp;

  /* check the first few characters. if recognizable
     format, then use the info to fill values
     in Im->.. appropriately. 
  */

  if (Im->ImState != ImStateInited)
    ImFatalError("ImPeekImg: can only peek after ImInitImg");

  fileOf(DS_name(&(Im->ImgSrc)),Im->ImName); 

  tempKind = GetImKind(Im);

  if (Im->ImKind != IM_UNKNOWN) {
    if (Im->ForceImKind && (!ImKindSubSetOf(tempKind,Im->ImKind))) {
      ImFatalError("ImPeekImg: actual format not a sub-format of the specified one");
    }
  } 

  Im->ImKind = tempKind;
  
  if ((!Im->TotalImBytes || Im->GuessedTotalImBytes)) {
      if ((tot = DS_SrcBytes(&(Im->ImgSrc))) > 0) 
          ImSetTotalBytes(Im, tot +
	    (DS_total_count(&(Im->ImgSrc)) -
		DS_bytes_read(&(Im->ImgSrc))), TRUE);
  }

  if ((Im->ImKind == IM_PGM_ASCII) || (Im->ImKind == IM_PGM_RAW)) {
    Im->NumPlanes = 1;
    if (!ReadPnmHeader(Im, &maxval)) ImFatalError("ImPeekImg: mangled PNM format");
    {
      int sbits;
      if (maxval < 256) sbits = 8;
      else if (maxval < 65536) sbits = 16;
      else sbits = 32;
      SetSbitsInner(Im, sbits, 0);
    }
  } else if ((Im->ImKind == IM_PPM_ASCII) || (Im->ImKind == IM_PPM_RAW)) {
    Im->NumPlanes = 3;
    if (!ReadPnmHeader(Im, &maxval)) ImFatalError("ImPeekImg: mangled PNM format");
    {
      int sbits;
      if (maxval < 256) sbits = 8;
      else if (maxval < 65536) sbits = 16;
      else sbits = 32;
      SetSbitsInner(Im, sbits, 0);
      SetSbitsInner(Im, sbits, 1);
      SetSbitsInner(Im, sbits, 2);
    }
  } else if (Im->ImKind == IM_RAW) {
    /*** no action needed ***/

  #ifdef HAVE_JPEGLIB
  } else if (ImKindSubSetOf(Im->ImKind, IM_JPEG)) {
    if (!ReadJpegHeader(Im)) {
      ImFatalError("ImPeekImg: mangled JPEG header");
    }  
    SetSbitsInner(Im, 8, 0);
    SetSbitsInner(Im, 8, 1);
    SetSbitsInner(Im, 8, 2);
  #endif

  #ifdef HAVE_SPIHT
  } else if (ImKindSubSetOf(Im->ImKind, IM_SPIHT_GRAY)) {
    int pelbytes; 
    if (!ReadSpihtHeader(Im, &pelbytes)) {
      ImFatalError("ImPeekImg: mangled SPIHT header");
    }  
    SetSbitsInner(Im, pelbytes*8, 0);
  #endif

  } else {
    ImFatalError("ImPeekImg: unknown format");
  }

  guess_csp = ImC_None;
  if (Im->NumPlanes==1) guess_csp = ImC_Gray;
  else if (Im->NumPlanes==3) guess_csp = ImC_RGB;

  Im->OrigCspace = guess_csp;

  SetHostEndian(Im);
  Im->ImState = ImStatePeeked;
  
  
  if (!Im->qclic.width) Im->qclic.width = Im->NumCols; 
  if (!Im->qclic.height) Im->qclic.height = Im->NumRows; 
  if (!Im->qclic.planes) Im->qclic.planes = Im->NumPlanes; 

  if (!Im->qclic.icurve_points && Im->TotalImBytes) {
    Im->qclic.icurve_size = (long *) malloc(sizeof(long)*2); 
    Im->qclic.icurve_quality = (float *) malloc(sizeof(float)*2); 
    if (!Im->qclic.icurve_size || !Im->qclic.icurve_quality) {
      if (Im->qclic.icurve_size) IM_FREE_PTR(Im->qclic.icurve_size); 
      if (Im->qclic.icurve_quality) IM_FREE_PTR(Im->qclic.icurve_quality); 
    } else {
      Im->qclic.icurve_points = 2;
      strcpy(Im->qclic.icurve_qmetric,"PSIZE");
      Im->qclic.icurve_size[0] = DS_total_count(&(Im->ImgSrc)) -
	  ImQclicHeaderBytes(Im); 
      Im->qclic.icurve_size[1] = Im->TotalImBytes -
	  ImQclicHeaderBytes(Im); 
      Im->qclic.icurve_quality[0] = 
	((float) Im->qclic.icurve_size[0]*100)/
	((float) Im->NumRows * Im->NumCols * Im->NumPlanes); 
      Im->qclic.icurve_quality[1] = 
	((float) Im->qclic.icurve_size[1]*100)/
	((float) Im->NumRows * Im->NumCols * Im->NumPlanes); 
    } 
  }
  
  if (!strcmp(Im->qclic.source,QCLIC_UNKNOWN_SOURCE) &&
      strncmp(DS_name(&(Im->ImgSrc)),"<",1) ) { 
    fileOf(DS_name(&(Im->ImgSrc)),Im->qclic.source); 
    dirOf(DS_name(&(Im->ImgSrc)),Im->qclic.dir); 
    if (!strncmp(DS_name(&(Im->ImgSrc)),"http://",7)) 
      dirOf(DS_name(&(Im->ImgSrc)),Im->qclic.webdir); 
  }
  
  if (!ImIsCompressed(Im)) {
    Im->qclic.is_lossless = 1; 
    Im->qclic.know_lossless = 1;
    strcpy(Im->qclic.lossless, Im->qclic.source); 
  }

  Im->qclic.have_info = 1;

  
  if (!Im->Silent) (void) ImPrintImgChars(Im,&Im->ImgErr);
  return(1);
}


static void ImFlipBytesIfNeeded(Image *Im, int planenum, int endian)
{
  unsigned long imsize, i;
  unsigned short temp;
  unsigned long itemp, itemp2;


  imsize = ((unsigned long) Im->CurrRows[planenum]) *
	   Im->CurrCols[planenum];

  if (Im->SampleBytes[planenum]==2) {
    if (endian != Im->HostEndian[planenum]) {
      /* must flip bytes */

      for (i=0;i<imsize;i++) {
	  itemp = (unsigned long) Im->Im2[planenum][i];
	  temp = (unsigned short) ((unsigned long) (itemp & 0xFF) << 8);
	  temp += (unsigned short) ((unsigned long) itemp >> 8);
	  Im->Im2[planenum][i] = (Pixel2) temp;
    } }
  } else if (Im->SampleBytes[planenum]==4) {
    if (endian != Im->HostEndian[planenum]) {
      /* must flip bytes */

      for (i=0;i<imsize;i++) {
	  itemp = (unsigned long) Im->Im4[planenum][i];
	  itemp2 = (itemp & 0xFF) << 24;
	  itemp2 += (itemp & 0xFF00) << 8;
	  itemp2 += (itemp & 0xFF0000) >> 8;
	  itemp2 += (itemp & 0xFF000000) >> 24;
	  Im->Im4[planenum][i] = (Pixel4) itemp2;
    } }
  }
}

static int ReadRawImg(Image *Im)
{
  long i,imsize, imbytes, bytes_read = 0;
  int planenum, delta = 0;
  unsigned char *buffarea;



  imsize = Im->NumRows * Im->NumCols;

  Im->ImState = ImStateReadAll;
  Im->ImRestartAux[0] = 0; 

  for (planenum=0; planenum < Im->NumPlanes; planenum++) {
    imbytes = imsize*Im->SampleBytes[planenum];

    buffarea = (unsigned char *) Im->Im[planenum];

    if (planenum==0) {
        /* FirstFiveChars were already read in */
	for (i=0;i<5;i++) buffarea[i] = Im->FirstFiveChars[i];
	buffarea = buffarea + 5;
	imbytes -= 5;
	delta = 5; 
    } else {
	delta = 0; 
    }

    bytes_read = DS_Read(&(Im->ImgSrc),buffarea,imbytes); 
  
    ImFlipBytesIfNeeded(Im,planenum,Im->SrcEndian[planenum]);

    if (bytes_read < imbytes) {
      Im->ImRestartPt = planenum; 
      Im->ImRestartAux[0] = bytes_read;
      Im->ImState = ImStateReadSome;
      break;
    }

    Im->ImRestartAux[0] = 0; 

  }

  if (Im->ImState == ImStateReadSome) {
    for (planenum=0; planenum < Im->ImRestartPt; planenum++) {
      Im->FirstChangedRow[planenum] = 0; 
      Im->LastChangedRow[planenum] = Im->NumRows - 1; 
    }
    Im->FirstChangedRow[Im->ImRestartPt] = 0;
    Im->LastChangedRow[Im->ImRestartPt] =
      (bytes_read + delta) /
	(Im->SampleBytes[Im->ImRestartPt] * Im->NumCols); 
  } else {
    for (planenum=0; planenum < Im->NumPlanes; planenum++) {
      Im->FirstChangedRow[planenum] = 0; 
      Im->LastChangedRow[planenum] = Im->NumRows - 1; 
    }
  } 
  return(1);
}


extern int ImSetImgFile(Image *Im, char *fname, long nbytes)
{

  if (Im->ImState < ImStateInited)
    ImFatalError("ImSetImageFile: no image here yet!");
  else if (Im->ImState >= ImStateReadAll)
    ImFatalError("ImSetImageFile: Image has already been fully read");

  if (!Im->ImgSrcInited) {
    DS_Init(&(Im->ImgSrc), Im->TotalImBytes);
    Im->ImgSrcInited = TRUE;
  } 
  if (!DS_OpenFile(&(Im->ImgSrc), fname, nbytes))
    ImFatalError("ImSetImageFile: error opening source file"); 
    
  return(1);
}


extern int ImSetImgStream(Image *Im, FILE *fp, long nbytes)
{

  if (Im->ImState < ImStateInited)
    ImFatalError("ImSetImageStream: no image here yet!");
  else if (Im->ImState >= ImStateReadAll)
    ImFatalError("ImSetImageStream: Image has already been fully read");

  if (!Im->ImgSrcInited) {
    DS_Init(&(Im->ImgSrc), Im->TotalImBytes);
    Im->ImgSrcInited = TRUE;
  } 
  if (!DS_OpenStream(&(Im->ImgSrc), fp, nbytes))
    ImFatalError("ImSetImageStream: error opening source file"); 
    
  return(1);
}


extern int ImSetImgFd(Image *Im, int fd, long nbytes)
{
  if (Im->ImState < ImStateInited)
    ImFatalError("ImSetImgFd: no image here yet!");
  else if (Im->ImState >= ImStateReadAll)
    ImFatalError("ImSetImgFd: Image has already been fully read");

  if (!Im->ImgSrcInited) {
    DS_Init(&(Im->ImgSrc), Im->TotalImBytes);
    Im->ImgSrcInited = TRUE;
  } 
  if (!DS_OpenFd(&(Im->ImgSrc), fd, nbytes))
    ImFatalError("ImSetImgFd: error setting source fd"); 

  return(1);
}

extern int ImSetImgMemory(Image *Im, char *buff, long nbytes)
{
  if (Im->ImState < ImStateInited)
    ImFatalError("ImSetImgMemory: no image here yet!");
  else if (Im->ImState >= ImStateReadAll)
    ImFatalError("ImSetImgMemory: Image has already been fully read");

  if (!Im->ImgSrcInited) {
    DS_Init(&(Im->ImgSrc), Im->TotalImBytes);
    Im->ImgSrcInited = TRUE;
  } 
  if (!DS_OpenMemory(&(Im->ImgSrc), buff, nbytes))
    ImFatalError("ImSetImgMemory: error setting source memory"); 

  return(1);
}

extern int ImExtendImgBytes(Image *Im, long nbytes)
{
  if (Im->ImState < ImStateInited)
    ImFatalError("ImSetImageBytes: no image here yet!");
  else if (Im->ImState >= ImStateReadAll)
    ImFatalError("ImSetImageBytes: Image has already been fully read");

  if (!DS_ExtendSrc(&(Im->ImgSrc), nbytes))
    ImFatalError("ImSetImgBytes: error extending source"); 

  return(1);
}

extern int ImSetImgBytes(Image *Im, long nbytes)
{
  if (Im->ImState < ImStateInited)
    ImFatalError("ImSetImageBytes: no image here yet!");
  else if (Im->ImState >= ImStateReadAll)
    ImFatalError("ImSetImageBytes: Image has already been fully read");

  if (!DS_ResetSrcBytes(&(Im->ImgSrc), nbytes))
    ImFatalError("ImSetImgBytes: error extending source"); 

  return(1);
}

extern int ImReadImg(Image *Im)
{
  unsigned long imsize, imbytes,i;
  int ret =1;
  
  if (Im->ImState != ImStatePeeked)
    ImFatalError("ImReadImg: can be called after ImPeekImg only");

  Im->ImState = ImStateReadSome;

  /* allocate memory */
  imsize = Im->NumRows * Im->NumCols;
  for (i=0;i<Im->NumPlanes;i++) {
    imbytes = imsize*Im->SampleBytes[i];
    if ((Im->Im[i] = (Pixel *) malloc(imbytes))==NULL) {
      ImFatalError("ImReadImg: out of memory");
    }
    memset(Im->Im[i], 255, imbytes); 
    Im->Im2[i] = (Pixel2 *) Im->Im[i];
    Im->Im4[i] = (Pixel4 *) Im->Im[i];
  }

  for (i=0;i<Im->NumPlanes;i++) {
    Im->CurrRows[i] = Im->NumRows;
    Im->CurrCols[i] = Im->NumCols;
  }
  Im->AllPlanesEqual = TRUE;
  Im->CurrCspace = Im->OrigCspace;

  switch(Im->ImKind) {

    #ifdef HAVE_JPEGLIB
    case IM_JPEG:
    case IM_JPEG_RGB:
    case IM_JPEG_GRAY:
      ret = ReadJpegImg(Im);
      break;
    #endif
    
    #ifdef HAVE_SPIHT
    case IM_SPIHT_GRAY:
      ret = ReadSpihtImg(Im);
      break;
    #endif 

    case IM_RAW:
      ret = ReadRawImg(Im);
      break;

    case IM_PGM_RAW:
      ret = ReadPgmRawImg(Im);
      break;
    case IM_PGM_ASCII:
      ret = ReadPgmAsciiImg(Im);
      break;
    case IM_PPM_RAW:
      ret = ReadPpmRawImg(Im);
      break;
    case IM_PPM_ASCII:
      ret = ReadPpmAsciiImg(Im);
      break;

    default:
      ret = 0;
  }

  if (!ret) ImFatalError("ImReadImg: error reading image");



  if ((strcmp(Im->ImFilterUsed,"none")) && 
      (Im->ImState != ImStateReadAll)) {
    if (!Im->Silent)
      (void) DD_WriteLine(&(Im->ImgErr),
	"ImReadImg: warning--incremental reading not supported for this image type; image assumed to be done"); 
    Im->ImState = ImStateReadAll; 
  } else if (ImSrcAllRead(Im) && (Im->ImState != ImStateReadAll)) {
    if (!Im->GuessedTotalImBytes || !ImSetTotalBytes(Im, 0, TRUE)) { 
      if (!Im->Silent)
        (void) DD_WriteLine(&(Im->ImgErr),
	  "ImReadImg: warning--image not fully read but no more data"); 
      Im->ImState = ImStateReadAll; 
    } 
  } 

  if (Im->ImState == ImStateReadAll) DS_Close(&Im->ImgSrc);


  return(1);
}

static int ReadMoreRawImg(Image *Im)
{
  long imsize, imbytes, bytes_read = 0;
  int planenum, delta = 0;
  unsigned char *buffarea;



  imsize = Im->NumRows * Im->NumCols;

  Im->ImState = ImStateReadAll;

  for (planenum=Im->ImRestartPt; planenum < Im->NumPlanes; planenum++) {
    imbytes = imsize*Im->SampleBytes[planenum];

    buffarea = (unsigned char *) Im->Im[planenum];

    if (planenum==Im->ImRestartPt) {
	buffarea = buffarea + Im->ImRestartAux[0]; 
	imbytes -= Im->ImRestartAux[0]; 
        ImFlipBytesIfNeeded(Im,planenum,Im->SrcEndian[planenum]);
	delta = Im->ImRestartAux[0]; 
    } else {
	delta = 0;
    } 
      
    bytes_read = DS_Read(&(Im->ImgSrc),buffarea,imbytes); 
  
    ImFlipBytesIfNeeded(Im,planenum,Im->SrcEndian[planenum]);

    if (bytes_read < imbytes) {
      Im->ImRestartPt = planenum; 
      Im->ImRestartAux[0] += bytes_read;
      Im->ImState = ImStateReadSome;
      break;
    }

    Im->ImRestartAux[0] = 0; 
  }
  if (Im->ImState == ImStateReadSome) {
    for (planenum=0; planenum < Im->ImRestartPt; planenum++) {
      Im->FirstChangedRow[planenum] = 0; 
      Im->LastChangedRow[planenum] = Im->NumRows - 1; 
    }
    Im->FirstChangedRow[Im->ImRestartPt] = 0;
    Im->LastChangedRow[Im->ImRestartPt] =
      (bytes_read + delta) /
	(Im->SampleBytes[Im->ImRestartPt] * Im->NumCols); 
  } else {
    for (planenum=0; planenum < Im->NumPlanes; planenum++) {
      Im->FirstChangedRow[planenum] = 0; 
      Im->LastChangedRow[planenum] = Im->NumRows - 1; 
    }
  } 
  return(1);
}



extern int ImReadMoreImg(Image *Im)
{

  long i; 
  int ret =1, brit[IM_MAXPLANES];
  float contr[IM_MAXPLANES]; 
  int saved_rows[IM_MAXPLANES], saved_cols[IM_MAXPLANES];
  int smooth[IM_MAXPLANES];
  boolean wasCropped;
  int cropStartCol = 0, cropStartRow = 0, cropEndCol = 0, cropEndRow =0;
  ImColorSpaceType saved_csp;
  
  if (Im->ImState != ImStateReadSome)
    ImNonFatalError("ImReadMoreImg: can only be called when image has been partially read");

  /** record current image state **/
  for (i=0;i<Im->NumPlanes;i++) {
    saved_rows[i] = Im->CurrRows[i];
    saved_cols[i] = Im->CurrCols[i];
    brit[i] = Im->Brightness[i];
    contr[i] = Im->Contrast[i];
    smooth[i] = Im->SmoothingDone[i];
  }
  saved_csp = Im->CurrCspace;
  if ((wasCropped = Im->WasCropped)) {
    cropStartRow = Im->CropStartRow;
    cropStartCol = Im->CropStartCol;
    cropEndRow = Im->CropEndRow;
    cropEndCol = Im->CropEndCol;
  } 


  if (!ImBackToNormal(Im)) 
    ImNonFatalError("ImReadMoreImg: couldn't recover already-read part of image");
  


  switch(Im->ImKind) {

  #ifdef HAVE_JPEGLIB
  case IM_JPEG:
  case IM_JPEG_RGB:
  case IM_JPEG_GRAY:
    ret = ReadMoreJpegImg(Im);
    break;
  #endif

  #ifdef HAVE_SPIHT
  case IM_SPIHT_GRAY:
    ret = ReadMoreSpihtImg(Im);
    break;
  #endif 

  case IM_RAW:
    ret = ReadMoreRawImg(Im);
    break;

  case IM_PGM_RAW:
    ret = ReadMorePgmRawImg(Im);
    break;
  case IM_PGM_ASCII:
    ret = ReadMorePgmAsciiImg(Im);
    break;
  case IM_PPM_RAW:
    ret = ReadMorePpmRawImg(Im);
    break;
  case IM_PPM_ASCII:
    ret = ReadMorePpmAsciiImg(Im);
    break;
    
  default:
    ret = 0;
  }

  for (i=0;i<Im->NumPlanes;i++) {
    if (Im->FirstChangedRow[i] > Im->LastChangedRow[i]) {
       Im->FirstChangedRow[i] = 0;
       Im->LastChangedRow[i] = Im->NumRows - 1; 
    } 
  } 

  if (ImSrcAllRead(Im) && (Im->ImState != ImStateReadAll)) {
    if (!Im->GuessedTotalImBytes || !ImSetTotalBytes(Im, 0, TRUE)) { 
      if (!Im->Silent)
        (void) DD_WriteLine(&(Im->ImgErr),
	  "ImReadMoreImg: warning--image not fully read but no more data"); 
      Im->ImState = ImStateReadAll; 
    } 
  }

  if (Im->ImState == ImStateReadAll) DS_Close(&Im->ImgSrc);

  if (!ret) return(0);  /* none of them really returns 0 */

  if (wasCropped)
    ret = ImCrop(Im, cropStartRow, cropStartCol,
		     cropEndRow, cropEndCol); 
  if (!ret) return(0);  

  if (Im->CurrCspace != saved_csp)
    ret = ImChangeCspace(Im, saved_csp);
  if (!ret) return(0);  

  for (i=0;i<Im->NumPlanes;i++) {
    if ((Im->CurrRows[i] != saved_rows[i]) ||
        (Im->CurrCols[i] != saved_cols[i]))  { 
      ret = ImSetImgDim(Im, i, saved_rows[i], saved_cols[i],
		 smooth[i], FALSE);
      if (!ret) return(0); 
    } 
  } 

  /* for now, do not restore changes in brightness and contrast
     for 3-plane images: we are not keeping track of the colorspaces
     in which the brighten/contrasts were done !
  */ 
  if (Im->NumPlanes != 1) return(1); 

  for (i=0;i<Im->NumPlanes;i++) {
    if (brit[i] != 0) { 
      ret = ImBrighten(Im, i, brit[i]); 
      if (!ret) return(0); 
    } 
  } 

  for (i=0;i<Im->NumPlanes;i++) {
    if (contr[i] != ((float) 1.0)) { 
      ret = ImContrast(Im, i, contr[i]); 
      if (!ret) return(0); 
    } 
  } 

  return(1);

}


extern void ImFreeImg(Image *Im)
{
  int planenum;

  DS_Close(&Im->ImgSrc);
  DD_Close(&Im->ImgErr); 

  if ((Im->ImState == ImStateNotExtant) ||
      (Im->ImState == ImStateDead)) return;

  if ( (Im->ImState == ImStateReadSome) || (Im->ImState == ImStateReadAll)) { 
    for (planenum=0;planenum<Im->NumPlanes;planenum++) {
      if (Im->Changed[planenum] && Im->RetainedData[planenum])
	IM_FREE_PTR(Im->RetainedData[planenum]); 
      if (Im->Im[planenum]) IM_FREE_PTR(Im->Im[planenum]);
    }
  } 

  QclicInfoFree(&Im->qclic);

  #ifdef HAVE_SPIHT
  if (ImKindSubSetOf(Im->ImKind, IM_SPIHT_GRAY)) {
    FreeImgSpiht(Im);
  }
  #endif

  #ifdef HAVE_JPEGLIB
  if (ImKindSubSetOf(Im->ImKind, IM_JPEG)) {
    FreeImgJpeg(Im);
  }
  #endif

  Im->ImState = ImStateDead;
}

extern void dirOf(char *s, char *ans)
{
  int l = strlen(s);
  int i = l-1;

  strcpy(ans, s); 
  while ((i >= 0) && (ans[i] != '/')) i--;
  if (i < 0) strcpy(ans,".");
  else { 
    ans [i] = '\0';
  } 
  /* 
  if (!strcmp(ans,"http:/")) strcpy(ans, s);
  */ 
}

extern void fileOf(char *s, char *ans)
{
  int l = strlen(s);
  int i = l-1;

  while ((i >= 0) && (s[i] != '/')) i--;
  strcpy(ans,s + (i+1)); 
}


extern char * ImQclicLossless(Image *Im)
{
  static char newsrc[QCLIC_STRLENMAX];
  QclicInfo *qinfo = &(Im->qclic);
  DataSrc *ds = &(Im->ImgSrc);


  if ((Im->ImState < ImStateInited) || (Im->ImState >= ImStateDead))
    return((char *) 0); 
  if ((!ImIsCompressed(Im)) || Im->qclic.is_lossless ||
      !Im->qclic.know_lossless) 
    return((char *) 0); 
  if (!strcmp(qinfo->lossless,".")) 
    return((char *) 0); 
  if (DS_file_src(ds)) { 
    if (!strcmp(qinfo->dir,".")) {
      dirOf(DS_name(ds), newsrc);
      if (!strcmp(newsrc,".")) strcpy(newsrc,"");
      else strcat(newsrc,"/");
    } else sprintf(newsrc,"%s/",qinfo->dir);
  } else {
    if (!strcmp(qinfo->webdir,".")) {
      dirOf(DS_name(ds), newsrc);
      if (!strcmp(newsrc,".")) strcpy(newsrc,"");
      else strcat(newsrc,"/");
    } else sprintf(newsrc,"%s/",qinfo->webdir);
  } 
  strcat(newsrc,qinfo->lossless);
  return(newsrc); 
}


static int ImReadQclicInfo(Image *Im)
{
  long tot_size; 
  QclicInfo * qinfo = &(Im->qclic);
  DataSrc * ds = &(Im->ImgSrc); 
  char newsrc[QCLIC_STRLENMAX]; 

  if (!QclicInfoRead(qinfo, ds)) return(0); 

  if (qinfo->icurve_points > 0) {
    tot_size = qinfo->icurve_size[qinfo->icurve_points - 1] +
	       DS_total_count(ds);
    if (!ImSetTotalBytes(Im, tot_size, FALSE)) return(0); 
  }

  if (strcmp(qinfo->source,".")) {
    if (!DS_file_src(ds)) { 
      if (!strcmp(qinfo->webdir,".")) {
	dirOf(DS_name(ds), newsrc);
	if (!strcmp(newsrc,".")) strcpy(newsrc,"");
	else strcat(newsrc,"/");
      } else sprintf(newsrc,"%s/",qinfo->webdir);
    } else { 
      if (!strcmp(qinfo->dir,".")) {
	dirOf(DS_name(ds), newsrc);
	if (!strcmp(newsrc,".")) strcpy(newsrc,"");
	else strcat(newsrc,"/");
      } else sprintf(newsrc,"%s/",qinfo->dir);
    }
    strcat(newsrc,qinfo->source);

    if (!DS_OpenFile(ds, newsrc, -1)) return(0);
  }
  
  if (qinfo->width != 0) Im->NumCols = qinfo->width; 
  if (qinfo->height != 0) Im->NumRows = qinfo->height; 
  if (qinfo->planes != 0) Im->NumPlanes = qinfo->planes; 

  return(1);

}


static int GetImKind(Image *Im)
{
  int nread;

  nread = DS_Read(&(Im->ImgSrc), Im->FirstFiveChars, 5);
  if (nread != 5) return(IM_UNKNOWN);
  if ((Im->FirstFiveChars[0] == 'Q') &&
      (Im->FirstFiveChars[1] == 'C') &&
      (Im->FirstFiveChars[2] == 'L') &&
      (Im->FirstFiveChars[3] == 'I') &&
      (Im->FirstFiveChars[4] == 'C')) {
    if (!ImReadQclicInfo(Im)) return(IM_UNKNOWN);
    return(GetImKind(Im));
  }
  if (Im->FirstFiveChars[0] == 'P') {
    if (Im->FirstFiveChars[1] == '2') return(IM_PGM_ASCII);
    if (Im->FirstFiveChars[1] == '5') return(IM_PGM_RAW);
    if (Im->FirstFiveChars[1] == '3') return(IM_PPM_ASCII);
    if (Im->FirstFiveChars[1] == '6') return(IM_PPM_RAW);
  }
  #ifdef HAVE_GIFTOPNM
  if ((Im->FirstFiveChars[0] == 'G') && (Im->FirstFiveChars[1] == 'I') &&
      (Im->FirstFiveChars[2] == 'F')) {
      if (DS_total_in_src(&Im->ImgSrc))
	DS_ExtendSrc(&Im->ImgSrc, 0); 
      if (!DS_UseFilter(&Im->ImgSrc, "giftopnm",5,
         Im->FirstFiveChars)) return(IM_UNKNOWN); 
      strcpy(Im->ImFilterUsed,"giftopnm");
      return(GetImKind(Im));
  }
  #endif
  #ifdef HAVE_TIFFTOPNM
  if (((Im->FirstFiveChars[0] == '\115') &&
       (Im->FirstFiveChars[1] == '\115')) || 
      ((Im->FirstFiveChars[0] == '\111') &&
       (Im->FirstFiveChars[1] == '\111'))) { 
      if (DS_total_in_src(&Im->ImgSrc))
	DS_ExtendSrc(&Im->ImgSrc, 0); 
      if (!DS_UseFilter(&Im->ImgSrc, "tifftopnm",5,
         Im->FirstFiveChars)) return(IM_UNKNOWN); 
      strcpy(Im->ImFilterUsed,"tifftopnm");
      return(GetImKind(Im));
  }
  #endif
  #ifdef HAVE_JPEGLIB
  if ((Im->FirstFiveChars[0] == 0xFF) &&
      (Im->FirstFiveChars[1] == 0xD8) &&
      (Im->FirstFiveChars[2] == 0xFF) &&
      ((Im->FirstFiveChars[3] == 0xE0) ||
       (Im->FirstFiveChars[3] == 0xEE))) { 
      return(IM_JPEG);
  }
  #endif 
  #ifdef HAVE_SPIHT
  if ((Im->FirstFiveChars[0] == 'S') &&
      (Im->FirstFiveChars[1] == 'P') &&
      (Im->FirstFiveChars[2] == 'H') &&
      ((Im->FirstFiveChars[3] == 'T') ||
       (Im->FirstFiveChars[3] == 'G'))) { 
      return(IM_SPIHT_GRAY);
  }
  #endif 
  #ifdef HAVE_DJPEG
  if ((Im->FirstFiveChars[0] == 0xFF) &&
      (Im->FirstFiveChars[1] == 0xD8) &&
      (Im->FirstFiveChars[2] == 0xFF) &&
      ((Im->FirstFiveChars[3] == 0xE0) ||
       (Im->FirstFiveChars[3] == 0xEE))) { 
      if (DS_total_in_src(&Im->ImgSrc))
	DS_ExtendSrc(&Im->ImgSrc, 0); 
      if (!DS_UseFilter(&Im->ImgSrc, "djpeg",5,
        Im->FirstFiveChars)) return(IM_UNKNOWN); 
      strcpy(Im->ImFilterUsed,"djpeg");
      return(GetImKind(Im));
  }
  #endif
  #ifdef HAVE_RASTTOPNM
  if ((Im->FirstFiveChars[0] == 0x59) &&
      (Im->FirstFiveChars[1] == 0xA6) &&
      (Im->FirstFiveChars[2] == 0x6A) &&
      (Im->FirstFiveChars[3] == 0x95)) { 
      if (DS_total_in_src(&Im->ImgSrc))
	DS_ExtendSrc(&Im->ImgSrc, 0); 
      if (!DS_UseFilter(&Im->ImgSrc, "rasttopnm",5,
        Im->FirstFiveChars)) return(IM_UNKNOWN); 
      strcpy(Im->ImFilterUsed,"rasttopnm");
      return(GetImKind(Im));
  }
  #endif
  #ifdef HAVE_FITSTOPNM
  if ((Im->FirstFiveChars[0] == 'S') &&
      (Im->FirstFiveChars[1] == 'I') &&
      (Im->FirstFiveChars[2] == 'M') &&
      (Im->FirstFiveChars[3] == 'P') && 
      (Im->FirstFiveChars[4] == 'L')) { 
      if (DS_total_in_src(&Im->ImgSrc))
	DS_ExtendSrc(&Im->ImgSrc, 0); 
      if (!DS_UseFilter(&Im->ImgSrc, "fitstopnm",5,
        Im->FirstFiveChars)) return(IM_UNKNOWN); 
      strcpy(Im->ImFilterUsed,"fitstopnm");
      return(GetImKind(Im));
  }
  #endif
  return(IM_RAW);
}


static int ReadPnmHeader(Image *Im, long *maxv)
{
  char line[IM_STRLENMAX];
  char *tline;
  int temp, numread = 0;
  int i,first;

  first = 1;

  /*** file must have been confirmed earlier as a PNM file */
  while (numread < 3) {
    
    if (first) {
      first = 0;
      for (i=0;i<5;i++) line[i] = Im->FirstFiveChars[i];
      tline = &line[0] + 5;
    } else {
      tline = &line[0];
    }

    if (!DS_ReadLine(&(Im->ImgSrc), tline, IM_STRLENMAX - 5)) return(0);

    while (line[0] == '#') {
      if (!DS_ReadLine(&(Im->ImgSrc), line, IM_STRLENMAX))  return(0);
    }

    if ((line[0] == 'P') && ('1' < line[1]) &&
	('7' > line[1]) && (!numread)) {
	  line[0] = ' ';
	  line[1] = ' ';
    }
    if (!numread) temp = sscanf(line,"%d%d%ld",
	  &Im->NumCols,&Im->NumRows,maxv);
    else if (numread==1) temp = sscanf(line,"%d%ld",
	  &Im->NumRows,maxv);
    else /* (numread==2) */ temp = sscanf(line,"%ld",maxv);
    if (temp < 0) temp = 0;
    numread += temp;
  }

  return(1);
}


/* restart codes for ReadMorePgmAsciiImg and ReadMorePpmAsciiImg:
   stored in Im->ImRestartAux[0].
   Im->ImRestartPt will store i for the next pixel to read */
#define PnmAsc_ReadingSpace 0
#define PnmAsc_ReadingComment 1 
#define PnmAsc_ReadingVal 2 

static int ReadPgmAsciiImg(Image *Im)
{

  register long val; 
  long i; 
  unsigned char c;
  long totalpixels;

  totalpixels = ((long) Im->NumRows) * Im->NumCols; 

  Im->ImState = ImStateReadAll; 

  /* assumes all the prelude has been read */
  c = ' ';
  for (i=0;i<totalpixels;i++) {
      while ((c==' ') || (c=='\n') || (c=='\t')) {
        if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	  Im->ImState = ImStateReadSome;
	  Im->ImRestartPt = i;
	  Im->LastChangedRow[0] = i/Im->NumCols; 
	  Im->ImRestartAux[0] = PnmAsc_ReadingSpace; 
          return(1);
	}
        while (c== '#') {
          if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	    Im->ImState = ImStateReadSome;
	    Im->ImRestartPt = i;
	    Im->LastChangedRow[0] = i/Im->NumCols; 
	    Im->ImRestartAux[0] = PnmAsc_ReadingComment; 
            return(1);
	  }
          while (c != '\n') {
            if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	      Im->ImState = ImStateReadSome;
	      Im->ImRestartPt = i;
	      Im->LastChangedRow[0] = i/Im->NumCols; 
	      Im->ImRestartAux[0] = PnmAsc_ReadingComment; 
	      return(1);
	    }
	  }
          if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	    Im->ImState = ImStateReadSome;
	    Im->ImRestartPt = i;
	    Im->LastChangedRow[0] = i/Im->NumCols; 
	    Im->ImRestartAux[0] = PnmAsc_ReadingSpace; 
            return(1);
	  }
        }
      }

      /* if c is not a digit, data is corrupt! */
      if ((c < '0') || (c > '9')) {
	if (!Im->Silent)
          (void) DD_WriteLine(&(Im->ImgErr),
	    "Ignoring corrupted data in PNM file");
        Im->LastChangedRow[0] = i/Im->NumCols; 
        return(1); 
      } 
      val = 0;

      while ((c >= '0') && (c <= '9')) {
	val *= 10;
	val += (c - '0');
	if (DS_Read(&(Im->ImgSrc),&c,1) != 1) { 
	  if (i == (totalpixels - 1)) {
	    /* we don't allow suspension while reading the
	       last pixel. heck, how do we tell if it's
	       suspended or at the real end of image?? */
	    break; 
	  } else {  
	    Im->ImState = ImStateReadSome;
	    Im->ImRestartPt = i;
	    Im->LastChangedRow[0] = i/Im->NumCols; 
	    Im->ImRestartAux[0] = PnmAsc_ReadingVal; 
	    Im->ImRestartAux[1] = val; 
	    return(1);
	  } 
	}
	  
      }
      if (val > Im->MaxSample[0]) val = Im->MaxSample[0]; 
      if (Im->SampleBytes[0]==1)
        Im->Im[0][i] = (Pixel) val;
      else if (Im->SampleBytes[0]==2)
        Im->Im2[0][i] = (Pixel2) val;
      else /* (Im->SampleBytes[0]==4) */
        Im->Im4[0][i] = (Pixel4) val;
  }
  Im->LastChangedRow[0] = Im->NumRows -1; 
  return(1);

}

static int ReadMorePgmAsciiImg(Image *Im)
{

  long i; 
  register long val; 
  unsigned char c;
  long totalpixels;
  int pushcomment = 0, pushval = 0; 

  totalpixels = ((long) Im->NumRows) * Im->NumCols; 

  Im->ImState = ImStateReadAll; 


  if (Im->ImRestartAux[0] == PnmAsc_ReadingSpace) c = ' ';
  else if (Im->ImRestartAux[0] == PnmAsc_ReadingComment) {
    c = ' ';
    pushcomment = 1;
  } else {
    /* if (Im->ImRestartAux[0] == PnmAsc_ReadingVal) */
    c = '0';
    pushval = 1;
  } 

  Im->FirstChangedRow[0] = Im->LastChangedRow[0]; 

  for (i=Im->ImRestartPt;i<totalpixels;i++) {
      
      while ((c==' ') || (c=='\n') || (c=='\t')) {
        if (pushcomment) {
	  c = '#';
	  pushcomment = 0;
	} else { 
          if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	    Im->ImState = ImStateReadSome;
	    Im->ImRestartPt = i;
            Im->LastChangedRow[0] = i/Im->NumCols; 
	    Im->ImRestartAux[0] = PnmAsc_ReadingSpace; 
            return(1);
	  }
	} 
        while (c== '#') {
          if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	    Im->ImState = ImStateReadSome;
	    Im->ImRestartPt = i;
            Im->LastChangedRow[0] = i/Im->NumCols; 
	    Im->ImRestartAux[0] = PnmAsc_ReadingComment; 
            return(1);
	  }
          while (c != '\n') {
            if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	      Im->ImState = ImStateReadSome;
	      Im->ImRestartPt = i;
              Im->LastChangedRow[0] = i/Im->NumCols; 
	      Im->ImRestartAux[0] = PnmAsc_ReadingComment; 
	      return(1);
	    }
	  }
          if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	    Im->ImState = ImStateReadSome;
	    Im->ImRestartPt = i;
            Im->LastChangedRow[0] = i/Im->NumCols; 
	    Im->ImRestartAux[0] = PnmAsc_ReadingSpace; 
            return(1);
	  }
        }
      }

      if (pushval) {
	val = Im->ImRestartAux[1];
	if (val) {
	  c = '0' + (val - ((val/10)*10));
	  val /= 10;
	} 
	pushval = 0;
      } else {
        val = 0;
      } 
      /* if c is not a digit, data is corrupt! */
      if ((c < '0') || (c > '9')) {
	if (!Im->Silent)
          (void) DD_WriteLine(&(Im->ImgErr),
	    "Ignoring corrupted data in PNM file");
        Im->LastChangedRow[0] = i/Im->NumCols; 
        return(1); 
      } 
      while ((c >= '0') && (c <= '9')) {
	val *= 10;
	val += (c - '0');
	if (DS_Read(&(Im->ImgSrc),&c,1) != 1) { 
	  if (i == (totalpixels - 1)) {
	    /* we don't allow suspension while reading the
	       last pixel. heck, how do we tell if it's
	       suspended or at the real end of image?? */
	    break; 
	  } else {  
	    Im->ImState = ImStateReadSome;
	    Im->ImRestartPt = i;
            Im->LastChangedRow[0] = i/Im->NumCols; 
	    Im->ImRestartAux[0] = PnmAsc_ReadingVal; 
	    Im->ImRestartAux[1] = val; 
	    return(1);
	  } 
	}
      }
      if (val > Im->MaxSample[0]) val = Im->MaxSample[0]; 
      if (Im->SampleBytes[0]==1)
        Im->Im[0][i] = (Pixel) val;
      else if (Im->SampleBytes[0]==2)
        Im->Im2[0][i] = (Pixel2) val;
      else /* (Im->SampleBytes[0]==4) */
        Im->Im4[0][i] = (Pixel4) val;
  }
  Im->LastChangedRow[0] = Im->NumRows -1; 
  return(1);

}


static int ReadPgmRawImg(Image *Im)
{
  long bytes_read, totalbytes; 

  totalbytes = ((long) Im->NumRows) * Im->NumCols * Im->SampleBytes[0];

  Im->ImState = ImStateReadAll; 

  /* assumes all the prelude has been read */

  bytes_read = DS_Read(&(Im->ImgSrc),Im->Im[0],totalbytes);

  if (bytes_read < totalbytes) {
    Im->ImRestartPt = bytes_read; 
    Im->ImState = ImStateReadSome; 
    Im->LastChangedRow[0] = bytes_read /
      (Im->NumCols * Im->SampleBytes[0]); 
  } else { 
    Im->LastChangedRow[0] = Im->NumRows - 1; 
  } 

  ImFlipBytesIfNeeded(Im,0,Im->SrcEndian[0]);
  return(1);
}

static int ReadMorePgmRawImg(Image *Im)
{
  long bytes_read, totalbytes; 

  totalbytes = ((long) Im->NumRows) * Im->NumCols * Im->SampleBytes[0];
  totalbytes -= Im->ImRestartPt; 

  Im->ImState = ImStateReadAll; 

  ImFlipBytesIfNeeded(Im,0,Im->SrcEndian[0]);

  bytes_read = DS_Read(&(Im->ImgSrc),Im->Im[0] + Im->ImRestartPt,
				 totalbytes);

  Im->FirstChangedRow[0] = Im->LastChangedRow[0]; 
  if (bytes_read < totalbytes) {
    Im->ImRestartPt += bytes_read; 
    Im->ImState = ImStateReadSome; 
    Im->LastChangedRow[0] = Im->ImRestartPt / 
      (Im->NumCols * Im->SampleBytes[0]); 
  } else { 
    Im->LastChangedRow[0] = Im->NumRows - 1; 
  }

  ImFlipBytesIfNeeded(Im,0,Im->SrcEndian[0]);
  return(1);
}

static int ReadPpmAsciiImg(Image *Im)
{

  register long val;
  unsigned char c;
  long totalpixels, i; 


  totalpixels = ((long) Im->NumRows) * Im->NumCols * 3; 

  Im->ImState = ImStateReadAll; 

  for (i=0;i<totalpixels;i++) {
      c = ' ';
      while ((c==' ') || (c=='\n') || (c=='\t')) {
        if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	  Im->ImState = ImStateReadSome;
	  Im->ImRestartPt = i;
	  Im->LastChangedRow[0] = 
	  Im->LastChangedRow[1] = 
	  Im->LastChangedRow[2] = (i/3)/Im->NumCols; 
	  Im->ImRestartAux[0] = PnmAsc_ReadingSpace; 
	  return(1);
	}
        while (c== '#') {
          if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	    Im->ImState = ImStateReadSome;
	    Im->ImRestartPt = i;
	    Im->LastChangedRow[0] = 
	    Im->LastChangedRow[1] = 
	    Im->LastChangedRow[2] = (i/3)/Im->NumCols; 
	    Im->ImRestartAux[0] = PnmAsc_ReadingComment; 
	    return(1);
	  }
          while (c != '\n') {
            if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	      Im->ImState = ImStateReadSome;
	      Im->ImRestartPt = i;
	      Im->LastChangedRow[0] = 
	      Im->LastChangedRow[1] = 
	      Im->LastChangedRow[2] = (i/3)/Im->NumCols; 
	      Im->ImRestartAux[0] = PnmAsc_ReadingComment; 
	      return(1);
	    }
	  }
          if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	    Im->ImState = ImStateReadSome;
	    Im->ImRestartPt = i;
	    Im->LastChangedRow[0] = 
	    Im->LastChangedRow[1] = 
	    Im->LastChangedRow[2] = (i/3)/Im->NumCols; 
	    Im->ImRestartAux[0] = PnmAsc_ReadingSpace; 
	    return(1);
	  } 
        }
      }

      /* if c is not a digit, data is corrupt! */
      if ((c < '0') || (c > '9')) {
	if (!Im->Silent)
          (void) DD_WriteLine(&(Im->ImgErr),
	    "Ignoring corrupted data in PNM file");
	Im->LastChangedRow[0] = 
	Im->LastChangedRow[1] = 
	Im->LastChangedRow[2] = (i/3)/Im->NumCols; 
        return(1); 
      } 
      val = 0;
      while ((c >= '0') && (c <= '9')) {
	val *= 10;
	val += (c - '0');
	if (DS_Read(&(Im->ImgSrc),&c,1) != 1) { 
	  if (i == (totalpixels - 1)) {
	    /* we don't allow suspension while reading the
	       last pixel. heck, how do we tell if it's
	       suspended or at the real end of image?? */
	    break; 
	  } else {  
	    Im->ImState = ImStateReadSome;
	    Im->ImRestartPt = i;
	    Im->LastChangedRow[0] = 
      	    Im->LastChangedRow[1] = 
    	    Im->LastChangedRow[2] = (i/3)/Im->NumCols; 
	    Im->ImRestartAux[0] = PnmAsc_ReadingVal; 
	    Im->ImRestartAux[1] = val; 
	    return(1);
	  } 
	}
      }

      if (val > Im->MaxSample[0]) val = Im->MaxSample[0]; 
      if (Im->SampleBytes[0]==1)
        Im->Im[i%3][i/3] = (Pixel) val;
      else if (Im->SampleBytes[0]==2)
        Im->Im2[i%3][i/3] = (Pixel2) val;
      else /* (Im->SampleBytes[0]==4) */
        Im->Im4[i%3][i/3] = (Pixel4) val;
  }

  Im->LastChangedRow[0] = 
  Im->LastChangedRow[1] = 
  Im->LastChangedRow[2] = Im->NumRows - 1; 
  return(1);
}

static int ReadMorePpmAsciiImg(Image *Im)
{

  long i; 
  register long val; 
  unsigned char c;
  long totalpixels;
  int pushcomment = 0, pushval = 0; 

  totalpixels = ((long) Im->NumRows) * Im->NumCols * 3; 

  Im->ImState = ImStateReadAll; 


  if (Im->ImRestartAux[0] == PnmAsc_ReadingSpace) c = ' ';
  else if (Im->ImRestartAux[0] == PnmAsc_ReadingComment) {
    c = ' ';
    pushcomment = 1;
  } else {
    /* if (Im->ImRestartAux[0] == PnmAsc_ReadingVal) */
    c = '0';
    pushval = 1;
  } 

  Im->FirstChangedRow[0] = Im->LastChangedRow[0]; 
  Im->FirstChangedRow[1] = Im->LastChangedRow[1]; 
  Im->FirstChangedRow[2] = Im->LastChangedRow[2]; 

  for (i=Im->ImRestartPt;i<totalpixels;i++) {
      
      while ((c==' ') || (c=='\n') || (c=='\t')) {
        if (pushcomment) {
	  c = '#';
	  pushcomment = 0;
	} else { 
          if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	    Im->ImState = ImStateReadSome;
	    Im->ImRestartPt = i;
	    Im->LastChangedRow[0] = 
      	    Im->LastChangedRow[1] = 
    	    Im->LastChangedRow[2] = (i/3)/Im->NumCols; 
	    Im->ImRestartAux[0] = PnmAsc_ReadingSpace; 
            return(1);
	  }
	} 
        while (c== '#') {
          if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	    Im->ImState = ImStateReadSome;
	    Im->ImRestartPt = i;
	    Im->LastChangedRow[0] = 
      	    Im->LastChangedRow[1] = 
    	    Im->LastChangedRow[2] = (i/3)/Im->NumCols; 
	    Im->ImRestartAux[0] = PnmAsc_ReadingComment; 
            return(1);
	  }
          while (c != '\n') {
            if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	      Im->ImState = ImStateReadSome;
	      Im->ImRestartPt = i;
	      Im->LastChangedRow[0] = 
      	      Im->LastChangedRow[1] = 
    	      Im->LastChangedRow[2] = (i/3)/Im->NumCols; 
	      Im->ImRestartAux[0] = PnmAsc_ReadingComment; 
	      return(1);
	    }
	  }
          if (DS_Read(&(Im->ImgSrc),&c,1) != 1) {
	    Im->ImState = ImStateReadSome;
	    Im->ImRestartPt = i;
	    Im->LastChangedRow[0] = 
      	    Im->LastChangedRow[1] = 
    	    Im->LastChangedRow[2] = (i/3)/Im->NumCols; 
	    Im->ImRestartAux[0] = PnmAsc_ReadingSpace; 
            return(1);
	  }
        }
      }

      if (pushval) {
	val = Im->ImRestartAux[1];
	if (val) {
	  c = '0' + (val - ((val/10)*10));
	  val /= 10;
	} 
	pushval = 0;
      } else {
        val = 0;
      } 
      /* if c is not a digit, data is corrupt! */
      if ((c < '0') || (c > '9')) {
	if (!Im->Silent)
          (void) DD_WriteLine(&(Im->ImgErr),
	    "Ignoring corrupted data in PNM file");
	Im->LastChangedRow[0] = 
      	Im->LastChangedRow[1] = 
    	Im->LastChangedRow[2] = (i/3)/Im->NumCols; 
        return(1); 
      } 
      while ((c >= '0') && (c <= '9')) {
	val *= 10;
	val += (c - '0');
	if (DS_Read(&(Im->ImgSrc),&c,1) != 1) { 
	  if (i == (totalpixels - 1)) {
	    /* we don't allow suspension while reading the
	       last pixel. heck, how do we tell if it's
	       suspended or at the real end of image?? */
	    break; 
	  } else {  
	    Im->ImState = ImStateReadSome;
	    Im->ImRestartPt = i;
	    Im->LastChangedRow[0] = 
      	    Im->LastChangedRow[1] = 
    	    Im->LastChangedRow[2] = (i/3)/Im->NumCols; 
	    Im->ImRestartAux[0] = PnmAsc_ReadingVal; 
	    Im->ImRestartAux[1] = val; 
	    return(1);
	  } 
	}
      }
      if (val > Im->MaxSample[0]) val = Im->MaxSample[0]; 
      if (Im->SampleBytes[0]==1)
        Im->Im[i%3][i/3] = (Pixel) val;
      else if (Im->SampleBytes[0]==2)
        Im->Im2[i%3][i/3] = (Pixel2) val;
      else /* (Im->SampleBytes[0]==4) */
        Im->Im4[i%3][i/3] = (Pixel4) val;
  }
  Im->LastChangedRow[0] = 
  Im->LastChangedRow[1] = 
  Im->LastChangedRow[2] = Im->NumRows - 1; 
  return(1);

}


#define IM_BUFFER_SIZE 4044 /* buffer size multpl of 3 and 4 */

static int ReadPpmRawImg(Image *Im)
{

  long i, j, n; 
  unsigned char buffer[IM_BUFFER_SIZE];
  long numiters, remaining, bytesiniter, totalbytes; 
  long bytes_read, bytes_used, bytes_per_impel; 
  Pixel2 * buffarea2;
  Pixel4 * buffarea4; 

  bytes_per_impel = 3 * Im->SampleBytes[0]; 
  totalbytes = ((long) Im->NumRows) * Im->NumCols * bytes_per_impel; 

  Im->ImState = ImStateReadAll; 

  numiters = totalbytes/IM_BUFFER_SIZE;
  remaining = totalbytes - (numiters*IM_BUFFER_SIZE);

  if (remaining) numiters++;
  else remaining = IM_BUFFER_SIZE;

  n = 0;
  for (i=0;i<numiters;i++) {

	bytesiniter = (i==(numiters-1))?remaining:IM_BUFFER_SIZE;

        bytes_read = DS_Read(&Im->ImgSrc,buffer,bytesiniter); 

	bytes_used = (bytes_read/bytes_per_impel)*bytes_per_impel; 

	if (Im->SampleBytes[0]==1) {
          j=0;
          while (j<bytes_used) {
  	    Im->Im[0][n] = (Pixel) buffer[j++];
	    Im->Im[1][n] = (Pixel) buffer[j++];
	    Im->Im[2][n] = (Pixel) buffer[j++];
	    n++;
          }
	} else if (Im->SampleBytes[0]==2) {
          j=0;
	  buffarea2 = (Pixel2 *) buffer;
          while (j<(bytes_used/2)) {
  	    Im->Im2[0][n] = (Pixel2) buffarea2[j++];
	    Im->Im2[1][n] = (Pixel2) buffarea2[j++];
	    Im->Im2[2][n] = (Pixel2) buffarea2[j++];
	    n++;
          }
	} else { /* if (Im->SampleBytes[0]==4) */
          j=0;
	  buffarea4 = (Pixel4 *) buffer;
          while (j<(bytes_used/4)) {
  	    Im->Im4[0][n] = (Pixel4) buffarea4[j++];
	    Im->Im4[1][n] = (Pixel4) buffarea4[j++];
	    Im->Im4[2][n] = (Pixel4) buffarea4[j++];
	    n++;
          }
	}
	if (bytes_used < bytesiniter) {
	  Im->ImRestartPt = n;
	  Im->ImRestartAux[0] = bytes_read - bytes_used;
	  for (i=1,j=bytes_used;i<=Im->ImRestartAux[0];i++,j++)
	    Im->ImRestartAux[i] = buffer[j];
	  Im->ImState = ImStateReadSome;
	  break;
	} 
  }

  if (Im->ImState == ImStateReadSome) {
    Im->LastChangedRow[0] = 
    Im->LastChangedRow[1] = 
    Im->LastChangedRow[2] = Im->ImRestartPt / Im->NumCols; 
  } else { 
    Im->LastChangedRow[0] = 
    Im->LastChangedRow[1] = 
    Im->LastChangedRow[2] = Im->NumRows - 1; 
  } 

  ImFlipBytesIfNeeded(Im, 0,Im->SrcEndian[0]);
  ImFlipBytesIfNeeded(Im, 1,Im->SrcEndian[1]);
  ImFlipBytesIfNeeded(Im, 2,Im->SrcEndian[2]);
  return(1);

}

static int ReadMorePpmRawImg(Image *Im)
{

  long i, j, n; 
  unsigned char buffer[IM_BUFFER_SIZE];
  long numiters, remaining, bytesiniter, totalbytes; 
  long bytes_read, bytes_used, bytes_per_impel; 
  Pixel2 * buffarea2;
  Pixel4 * buffarea4; 

  ImFlipBytesIfNeeded(Im, 0,Im->SrcEndian[0]);
  ImFlipBytesIfNeeded(Im, 1,Im->SrcEndian[1]);
  ImFlipBytesIfNeeded(Im, 2,Im->SrcEndian[2]);

  bytes_per_impel = 3 * Im->SampleBytes[0]; 
  totalbytes = ((long) Im->NumRows) * Im->NumCols * bytes_per_impel; 
  n = Im->ImRestartPt;
  totalbytes -= (n*bytes_per_impel); 

  Im->ImState = ImStateReadAll; 

  numiters = totalbytes/IM_BUFFER_SIZE;
  remaining = totalbytes - (numiters*IM_BUFFER_SIZE);

  if (remaining) numiters++;
  else remaining = IM_BUFFER_SIZE;
  
  Im->FirstChangedRow[0] = Im->LastChangedRow[0]; 
  Im->FirstChangedRow[1] = Im->LastChangedRow[1]; 
  Im->FirstChangedRow[2] = Im->LastChangedRow[2]; 

  for (i=0;i<numiters;i++) {

	bytesiniter = (i==(numiters-1))?remaining:IM_BUFFER_SIZE;

	if (i==0) {
	  /** copy saved, unused bytes into buffer **/ 
	  for (j=0;j<Im->ImRestartAux[0];j++) {
	    buffer[j] = (unsigned char) Im->ImRestartAux[j+1]; 
	  }
          bytes_read = DS_Read(&Im->ImgSrc,buffer + Im->ImRestartAux[0],
			 bytesiniter - Im->ImRestartAux[0]); 
          bytes_read += Im->ImRestartAux[0]; 
    	} else { 
          bytes_read = DS_Read(&Im->ImgSrc,buffer,bytesiniter); 
        }

	bytes_used = (bytes_read/bytes_per_impel)*bytes_per_impel; 

	if (Im->SampleBytes[0]==1) {
          j=0;
          while (j<bytes_used) {
  	    Im->Im[0][n] = (Pixel) buffer[j++];
	    Im->Im[1][n] = (Pixel) buffer[j++];
	    Im->Im[2][n] = (Pixel) buffer[j++];
	    n++;
          }
	} else if (Im->SampleBytes[0]==2) {
          j=0;
	  buffarea2 = (Pixel2 *) buffer;
          while (j<(bytes_used/2)) {
  	    Im->Im2[0][n] = (Pixel2) buffarea2[j++];
	    Im->Im2[1][n] = (Pixel2) buffarea2[j++];
	    Im->Im2[2][n] = (Pixel2) buffarea2[j++];
	    n++;
          }
	} else { /* if (Im->SampleBytes[0]==4) */
          j=0;
	  buffarea4 = (Pixel4 *) buffer;
          while (j<(bytes_used/4)) {
  	    Im->Im4[0][n] = (Pixel4) buffarea4[j++];
	    Im->Im4[1][n] = (Pixel4) buffarea4[j++];
	    Im->Im4[2][n] = (Pixel4) buffarea4[j++];
	    n++;
          }
	}
	if (bytes_used < bytesiniter) {
	  Im->ImRestartPt = n;
	  Im->ImRestartAux[0] = bytes_read - bytes_used;
	  for (i=1,j=bytes_used;i<=Im->ImRestartAux[0];i++,j++)
	    Im->ImRestartAux[i] = buffer[j];
	  Im->ImState = ImStateReadSome;
	  break;
	} 
  }
  ImFlipBytesIfNeeded(Im, 0,Im->SrcEndian[0]);
  ImFlipBytesIfNeeded(Im, 1,Im->SrcEndian[1]);
  ImFlipBytesIfNeeded(Im, 2,Im->SrcEndian[2]);

  if (Im->ImState == ImStateReadSome) {
    Im->LastChangedRow[0] = 
    Im->LastChangedRow[1] = 
    Im->LastChangedRow[2] = Im->ImRestartPt / Im->NumCols; 
  } else { 
    Im->LastChangedRow[0] = 
    Im->LastChangedRow[1] = 
    Im->LastChangedRow[2] = Im->NumRows - 1; 
  } 

  return(1);

}


extern long ImSendImg(Image *Im, DataDest *dd, char **extramem, int saveKind)
{
  int ans, planenum;
  long sz, i; 
  int loc_kind; 
  int have_extramem = ((int) extramem != 0);
  long extramem_size = 0, written, rem;
  char *extrabuff = (char *) 0; 

  char buff[IM_STRLENMAX]; 

  if (Im->ImState < ImStateReadSome)
    ImNonFatalError("ImSendImg: image has not been read");
  else if (Im->ImState >= ImStateDead)
    ImNonFatalError("ImSendImg: image is already dead");

  if (saveKind == IM_RAW) {
    loc_kind = IM_RAW; 
  } else if (ImKindSubSetOf(saveKind, IM_PNM)) {
    if (!Im->AllPlanesEqual) 
      ImNonFatalError("ImSendImg: unequal planes can only be saved as RAW"); 
    if ((Im->NumPlanes != 1) && (Im->NumPlanes != 3)) 
      ImNonFatalError("ImSendImg: 1 or 3 planes only for saving PNM"); 
    
    ans = Im->SampleBytes[0]; 
    if ((ans == 3) || (ans < 0) || (ans > 4)) {
      ImNonFatalError("ImSendImg: sample-bytes = 1,2,4 only for saving PNM"); 
    } 

    for (planenum=0; planenum < Im->NumPlanes; planenum++) {
      if (Im->SampleBytes[planenum] != ans) 
        ImNonFatalError("ImSendImg: sample-bytes must be equal in planes for saving PNM"); 
    } 

    loc_kind = IM_PNM; 
  } else { 
    ImNonFatalError("ImSendImg: can only save as RAW or PNM");
  }



  if (loc_kind == IM_RAW) {
    /* must save in raw format */
    ans = 1;
    for (planenum = 0; (ans==1) && (planenum <Im->NumPlanes); planenum++) {

      sz = ((long) Im->CurrRows[planenum]) * 
           Im->CurrCols[planenum] * Im->SampleBytes[planenum];

  
      ImFlipBytesIfNeeded(Im, planenum,Im->SaveEndian[planenum]);
    
      ans = 1;
      
      if ((written = DD_Write(dd, (char *) Im->Im[planenum], sz)) < sz) {
        if (have_extramem && (extramem_size == 0)) {
	  extramem_size = rem = sz - written;
	  for (i=(planenum+1);i<Im->NumPlanes;i++) {
            extramem_size += ( ((long) Im->CurrRows[i]) *
			       Im->CurrCols[i] * Im->SampleBytes[i] );
	  }
	  extrabuff = (char *) malloc(extramem_size);
	  if (!extrabuff) {
	    extramem_size = 0;
	    ans = 0;
	  } else {
            if (!DD_OpenMemory(dd, extrabuff, extramem_size) ||
		(DD_Write(dd, ((char *) Im->Im[planenum]) + written, rem) < rem)) {
	      extramem_size = 0;
	      ans = 0;
	      IM_FREE_PTR(extrabuff);
	    } 
	  }
	} else {
	  ans = 0;
	  if (have_extramem) {
	    /* I did allocate.. shouldn't happen! */
	    extramem_size = 0;
	    IM_FREE_PTR(extrabuff);
	  }
	}
      }

      ImFlipBytesIfNeeded(Im, planenum,Im->SaveEndian[planenum]);
	/* undo flips if done */
  
    }
    
    if (extramem_size > 0) *extramem = extrabuff;

    if (!ans) ImNonFatalError("ImSendImg: error writing image: device full?");
    else return DD_total_count(dd);

  }
  
  /* loc_kind == IM_PNM */ 
  if (Im->NumPlanes == 1) {
    /* save as PGM */
    ans = 1;

    sprintf(buff,"P5\n%d %d\n%lu\n",
	    Im->CurrCols[0], Im->CurrRows[0],
	    Im->MaxSample[0]); 

    sz = ((long) Im->CurrRows[0]) * Im->CurrCols[0] * Im->SampleBytes[0]; 

    if ((written = DD_Write(dd, buff, (i=strlen(buff)))) < i) {
        if (have_extramem && (extramem_size == 0)) {
	  rem = i - written;
	  extramem_size = sz + rem; 
	  extrabuff = (char *) malloc(extramem_size);
	  if (!extrabuff) {
	    extramem_size = 0;
	    ans = 0;
	  } else {
            if (!DD_OpenMemory(dd, extrabuff, extramem_size) ||
		(DD_Write(dd, ((char *) &buff[0]) + written, rem) < rem)) {
	      extramem_size = 0;
	      ans = 0;
	      IM_FREE_PTR(extrabuff);
	    } 
	  }
	} else {
	  ans = 0;
	  if (have_extramem) {
	    /* I did allocate.. shouldn't happen! */
	    extramem_size = 0;
	    IM_FREE_PTR(extrabuff);
	  }
	}
    }

    if (!ans) ImNonFatalError("ImSendImg: error writing image: device full?");

    ImFlipBytesIfNeeded(Im, 0, Im->SaveEndian[0]);
    if ((written = DD_Write(dd, (char *) Im->Im[0], sz)) < sz) {
        if (have_extramem && (extramem_size == 0)) {
	  extramem_size = rem = sz - written;
	  extrabuff = (char *) malloc(extramem_size);
	  if (!extrabuff) {
	    extramem_size = 0;
	    ans = 0;
	  } else {
            if (!DD_OpenMemory(dd, extrabuff, extramem_size) ||
		(DD_Write(dd, ((char *) Im->Im[0]) + written, rem) < rem)) {
	      extramem_size = 0;
	      ans = 0;
	      IM_FREE_PTR(extrabuff);
	    } 
	  }
	} else {
	  ans = 0;
	  if (have_extramem) {
	    /* I did allocate.. shouldn't happen! */
	    extramem_size = 0;
	    IM_FREE_PTR(extrabuff);
	  }
	}

    }
    ImFlipBytesIfNeeded(Im, 0, Im->SaveEndian[0]);

    if (extramem_size > 0) *extramem = extrabuff;

    if (!ans) ImNonFatalError("ImSendImg: error writing image: device full?");
    else return DD_total_count(dd);

  } else /* if (Im->NumPlanes == 3) */  {

    /* save as PPM */

    ans = 1;

    sz = ((unsigned long) Im->CurrRows[0]) * Im->CurrCols[0]; 

    sprintf(buff,"P6\n%d %d\n%lu\n", Im->CurrCols[0], Im->CurrRows[0],
		       Im->MaxSample[0]); 

    if ((written = DD_Write(dd, buff, (i=strlen(buff)))) < i) {
        if (have_extramem && (extramem_size == 0)) {
	  rem = i - written;
	  extramem_size = rem +
	    sz*(Im->SampleBytes[0]+Im->SampleBytes[1]+Im->SampleBytes[2]); 
	  extrabuff = (char *) malloc(extramem_size);
	  if (!extrabuff) {
	    extramem_size = 0;
	    ans = 0;
	  } else {
            if (!DD_OpenMemory(dd, extrabuff, extramem_size) ||
		(DD_Write(dd, ((char *) &buff[0]) + written, rem) < rem)) {
	      extramem_size = 0;
	      ans = 0;
	      IM_FREE_PTR(extrabuff);
	    } 
	  }
	} else {
	  ans = 0;
	  if (have_extramem) {
	    /* I did allocate.. shouldn't happen! */
	    extramem_size = 0;
	    IM_FREE_PTR(extrabuff);
	  }
	}
    }

    if (!ans) ImNonFatalError("ImSendImg: error writing image: device full?");
    
    ImFlipBytesIfNeeded(Im, 0, Im->SaveEndian[0]);
    ImFlipBytesIfNeeded(Im, 1, Im->SaveEndian[1]);
    ImFlipBytesIfNeeded(Im, 2, Im->SaveEndian[2]);

    ans = 1; 
    for (i=0; ((i<sz) && (ans != 0)); i++) {
      for (planenum=0;planenum<3;planenum++) { 
	if ( (written = DD_Write(dd, &(Im->Im[planenum][i]),
	       Im->SampleBytes[planenum])) < Im->SampleBytes[planenum]) {
          if (have_extramem && (extramem_size == 0)) {
	    rem = Im->SampleBytes[planenum] - written;
	    extramem_size = rem +
             (sz-(i+1))*(Im->SampleBytes[0]+Im->SampleBytes[1]+Im->SampleBytes[2]);
	    if (planenum < 2) extramem_size += (long) Im->SampleBytes[2];
	    if (planenum < 1) extramem_size += (long) Im->SampleBytes[1];
	    extrabuff = (char *) malloc(extramem_size);
	    if (!extrabuff) {
	      extramem_size = 0;
	      ans = 0;
	    } else {
              if (!DD_OpenMemory(dd, extrabuff, extramem_size) ||
		(DD_Write(dd, ((char *) &Im->Im[planenum][i])+written,rem)<rem)) {
	        extramem_size = 0;
	        ans = 0;
	        IM_FREE_PTR(extrabuff);
	      } 
	    }
	  } else {
	    ans = 0;
	    if (have_extramem) {
	      /* I did allocate.. shouldn't happen! */
	      extramem_size = 0;
	      IM_FREE_PTR(extrabuff);
	    }
	  }
	}
	if (!ans) break; 
      } 
    } 
    
    ImFlipBytesIfNeeded(Im, 0, Im->SaveEndian[0]);
    ImFlipBytesIfNeeded(Im, 1, Im->SaveEndian[1]);
    ImFlipBytesIfNeeded(Im, 2, Im->SaveEndian[2]);

    if (!ans) ImNonFatalError("ImSendImg: error writing image: device full?");
    else return DD_total_count(dd);
  }


  ImNonFatalError("ImSendImg: Format unknown/not handled");
}

extern long ImWriteImg(Image *Im, int fd, int saveKind)
{
  DataDest dd;
  long ans; 
  DD_Init(&dd);
  if (!DD_OpenFd(&dd, fd)) ImNonFatalError("ImWriteImg: bad fd given");
  ans = ImSendImg(Im, &dd, (char **) 0, saveKind);
  DD_Close(&dd);
  return ans;
}

extern long ImStreamWriteImg(Image *Im, FILE *fp, int saveKind)
{
  DataDest dd;
  long ans; 
  DD_Init(&dd);
  if (!DD_OpenStream(&dd, fp))
    ImNonFatalError("ImStreamWriteImg: bad fp given");
  ans = ImSendImg(Im, &dd, (char **) 0, saveKind);
  DD_Close(&dd);
  return ans;
}

extern long ImSaveImg(Image *Im, char * fname, int saveKind)
{
  DataDest dd;
  long ans; 
  DD_Init(&dd);
  if (!DD_OpenFile(&dd, fname)) ImNonFatalError("ImSaveImg: bad file given");
  ans = ImSendImg(Im, &dd, (char **) 0, saveKind);
  DD_Close(&dd);
  return ans;
}

extern long ImBufferImg(Image *Im, char *buff, long buffsize,
  char **extramem, int saveKind)
{
  DataDest dd;
  long ans; 
  DD_Init(&dd);
  if (!DD_OpenMemory(&dd, buff, buffsize))
    ImNonFatalError("ImBufferImg: bad memory given");
  ans = ImSendImg(Im, &dd, extramem, saveKind);
  DD_Close(&dd);
  return ans;
}

extern int ImDiscardOriginal(Image *Im)
{ 
  int i;

  if (Im->ImState < ImStateInited) 
    ImNonFatalError("ImDiscardOriginal: cannot be called before ImInitImg");
  else if (Im->ImState >= ImStateDead)
    ImNonFatalError("ImDiscardOriginal: image already dead!");

  if (Im->RetainOrigOnChange && (Im->ImState==ImStateReadAll)) { 
    for (i=0; i< Im->NumPlanes;i++) { 
      if ( (Im->Changed[i])  && Im->RetainedData[i]) 
	IM_FREE_PTR(Im->RetainedData[i]); 
    }
  }
  Im->RetainOrigOnChange = FALSE;
  return 1; 
}


extern int ImBackToNormal(Image *Im)
{
  int p;

  if (Im->ImState < ImStateReadSome)
    ImNonFatalError("ImBackToNormal: cannot be called before ImReadImg");
  else if (Im->ImState >= ImStateDead)
    ImNonFatalError("ImBackToNormal: image already dead!");

  for (p=0; p < Im->NumPlanes; p++) { 
      if (Im->Changed[p] && !Im->RetainedData[p]) 
        ImNonFatalError("ImBackToNormal: original image was not saved in memory");
  } 
  for (p=0; p < Im->NumPlanes; p++) {
      if (Im->Changed[p]) { 
        IM_FREE_PTR(Im->Im[p]); 
	Im->Im[p] = (Pixel *) Im->RetainedData[p]; 
	Im->Im2[p] = (Pixel2 *) Im->RetainedData[p]; 
	Im->Im4[p] = (Pixel4 *) Im->RetainedData[p]; 
	Im->RetainedData[p] = (char *) 0; 
      }
  }

  for (p=0; p < Im->NumPlanes; p++) {
    if (Im->Changed[p]) { 
      Im->CurrRows[p] = Im->NumRows; 
      Im->CurrCols[p] = Im->NumCols; 
      Im->Changed[p] = FALSE; 
      Im->Brightness[p] = 0;
      Im->Contrast[p] = ((float) 1.0); 
      Im->SmoothingDone[p] = FALSE; 
    }
  }
  Im->WasCropped = FALSE;
  Im->AllPlanesEqual = TRUE; 
  Im->CurrCspace = Im->OrigCspace; 
  return(1); 
}

extern int ImBackToNormalPixels(Image *Im)
{
  int p;
  int ret = 1;
  int saved_rows[IM_MAXPLANES], saved_cols[IM_MAXPLANES];
  int smooth[IM_MAXPLANES]; 
  boolean wasCropped;
  int cropStartCol=0, cropStartRow=0, cropEndCol=0, cropEndRow=0;

  if (Im->ImState < ImStateReadSome)
    ImNonFatalError("ImBackToNormalPixels: cannot be called before ImReadImg");
  else if (Im->ImState >= ImStateDead)
    ImNonFatalError("ImBackToNormalPixels: image already dead!");

  for (p=0; p < Im->NumPlanes; p++) {
    saved_rows[p] = Im->CurrRows[p];
    saved_cols[p] = Im->CurrCols[p];
    smooth[p] = Im->SmoothingDone[p]; 
  }
  if ((wasCropped = Im->WasCropped) != 0) {
    cropStartRow = Im->CropStartRow;
    cropStartCol = Im->CropStartCol;
    cropEndRow = Im->CropEndRow;
    cropEndCol = Im->CropEndCol;
  } 

  if (!ImBackToNormal(Im)) { 
    ImNonFatalError("ImBackToNormalPixels: could not recover image");
  } 



  if (wasCropped)
    ret = ImCrop(Im, cropStartRow, cropStartCol,
		     cropEndRow, cropEndCol); 
  if (!ret) return(0);  

  for (p=0;p<Im->NumPlanes;p++) {
    if ((Im->CurrRows[p] != saved_rows[p]) ||
        (Im->CurrCols[p] != saved_cols[p]))  { 
      ret = ImSetImgDim(Im, p, saved_rows[p], saved_cols[p],
		 smooth[p], FALSE);
      if (!ret) return(0); 
    } 
  } 
  return(1); 
}

extern float ImQclicIcurveQofS(Image *Im, long nbytes, boolean in_percent)
{
  float bspan, crud;
  float ans; 
  int i;
  if ((Im->ImState < ImStateInited) || (Im->ImState >= ImStateDead) ||
      (!Im->qclic.icurve_points && !Im->TotalImBytes)) return((float) 0);
  if (Im->qclic.icurve_points > 0) {
    nbytes -= Im->qclic.qclic_info_end_offset; 
    if (nbytes > Im->qclic.icurve_size[Im->qclic.icurve_points-1]) 
        nbytes = Im->qclic.icurve_size[Im->qclic.icurve_points-1];  
    if (nbytes <= 0) return((float) 0); 
    i=0;
    while (Im->qclic.icurve_size[i] < nbytes) i++;
    if (i==0) ans = Im->qclic.icurve_quality[0]; 
    else {
      if (Im->qclic.icurve_size[i] == nbytes)
	ans = Im->qclic.icurve_quality[i]; 
      else { 
        bspan = (float) Im->qclic.icurve_size[i] -
		(float) Im->qclic.icurve_size[i-1]; 
        crud = ((float) Im->qclic.icurve_size[i] -
		(float) nbytes)/bspan; 
	crud *= (Im->qclic.icurve_quality[i] -
		 Im->qclic.icurve_quality[i-1]); 
	ans = Im->qclic.icurve_quality[i] - crud;
	if (ans < Im->qclic.icurve_quality[i-1]) 
	    ans = Im->qclic.icurve_quality[i-1]; 
      }
    }

    if (in_percent) {
      ans = (ans - Im->qclic.icurve_quality[0]) * ((float) 100.0)/
	(Im->qclic.icurve_quality[Im->qclic.icurve_points-1]
	- Im->qclic.icurve_quality[0]);
    }
    return(ans); 
  } else {
    /* treat TotalImBytes as the total quality */
    if (nbytes > Im->TotalImBytes) nbytes = Im->TotalImBytes;
    else if (nbytes <= 0) nbytes = 0; 
    if (!in_percent) {
      return((float) nbytes); 
    } else {
      return((float) nbytes * ((float) 100)/((float) Im->TotalImBytes));
    }
  }
}

extern long ImQclicIcurveSofQ(Image *Im, float quality, boolean is_percent)
{
  float qspan, crud;
  long ans;
  int i;
  if ((Im->ImState < ImStateInited) || (Im->ImState >= ImStateDead) ||
      (!Im->qclic.icurve_points && !Im->TotalImBytes)) return(0);
  if (Im->qclic.icurve_points > 0) {
    if (is_percent) {
      if (quality <= 0) quality = 1;
      if (quality > 100) quality = 100;
      quality = Im->qclic.icurve_quality[0] + (quality *
	(Im->qclic.icurve_quality[Im->qclic.icurve_points-1]
	- Im->qclic.icurve_quality[0]) /100.0);
    }
    i=0;
    while ((i < Im->qclic.icurve_points) &&
	   (Im->qclic.icurve_quality[i] < quality)) i++;
    if (i >= Im->qclic.icurve_points)
      return(Im->qclic.icurve_size[Im->qclic.icurve_points-1]
	+ Im->qclic.qclic_info_end_offset); 
    else if (i==0) 
      return(Im->qclic.icurve_size[0] +
	 Im->qclic.qclic_info_end_offset);
    else if (Im->qclic.icurve_quality[i] == quality)
      return(Im->qclic.icurve_size[i] +
	     Im->qclic.qclic_info_end_offset);
    else {
      /* return a size between qclic.icurve_size[i-1]
	 and qclic.icurve_size[i] */
      qspan = Im->qclic.icurve_quality[i] -
	      Im->qclic.icurve_quality[i-1];
      crud = ((Im->qclic.icurve_quality[i] - quality)*
	     ((float) Im->qclic.icurve_size[i] -
		Im->qclic.icurve_size[i-1])/ qspan) + 0.5;
      ans = Im->qclic.icurve_size[i] - ((int) crud);
      if (ans < Im->qclic.icurve_size[i-1])
	ans = Im->qclic.icurve_size[i-1];
      return(ans + Im->qclic.qclic_info_end_offset);
    }
  } else {
    /* treat TotalImBytes as the total quality */
    if (!is_percent) {
      if (quality > Im->TotalImBytes) return(Im->TotalImBytes);
      else return((long) (quality+0.5));
    } else {
      if (quality <= 0) quality = 1;
      if (quality > 100) quality = 100;
      ans = ((long) ((float) 
	 0.5 + (quality*((float) Im->TotalImBytes)/((float) 100))));
      return(ans);
    }
  }
}

extern int ImChanged(Image *Im) 
{
  int i; 
  if ((Im->ImState < ImStateReadSome) ||
      (Im->ImState >= ImStateDead)) return(1);
  for (i=0;i<Im->NumPlanes;i++) 
    if (Im->Changed[i]) return(1);
  return(0);
} 


extern int ImSetTotalBytes(Image *Im, long tot, int isguess) 
{
  if ((Im->ImState < ImStateInited) ||
      (Im->ImState >= ImStateDead)) return(0);

  if (  !Im->TotalImBytes || 
        Im->GuessedTotalImBytes || 
	!isguess) {  
     if (!DS_ResetTotalSrcBytes(&((Im)->ImgSrc), tot))
       return(0);
     Im->TotalImBytes = tot;
     Im->GuessedTotalImBytes = isguess; 
     if (!Im->qclic.icurve_points) {
       Im->qclic.icurve_size = (long *) malloc(sizeof(long)*2); 
       Im->qclic.icurve_quality = (float *) malloc(sizeof(float)*2); 
       if (!Im->qclic.icurve_size || !Im->qclic.icurve_quality) {
         if (Im->qclic.icurve_size) IM_FREE_PTR(Im->qclic.icurve_size); 
         if (Im->qclic.icurve_quality) IM_FREE_PTR(Im->qclic.icurve_quality); 
       } else {
         Im->qclic.icurve_points = 2;
         strcpy(Im->qclic.icurve_qmetric,"PSIZE");
         Im->qclic.icurve_size[0] = 0; 
         Im->qclic.icurve_size[1] = Im->TotalImBytes -
	     ImQclicHeaderBytes(Im); 
         Im->qclic.icurve_quality[0] = 
   	   ((float) Im->qclic.icurve_size[0]*100)/
	   ((float) Im->NumRows * Im->NumCols * Im->NumPlanes); 
         Im->qclic.icurve_quality[1] = 
	   ((float) Im->qclic.icurve_size[1]*100)/
	   ((float) Im->NumRows * Im->NumCols * Im->NumPlanes); 
       } 
     } else if (!strcmp(Im->qclic.icurve_qmetric,"PSIZE")) {
       Im->qclic.icurve_size[1] = Im->TotalImBytes; 
       Im->qclic.icurve_quality[1] = 
	   ((float) Im->qclic.icurve_size[1]*100)/
	   ((float) Im->NumRows * Im->NumCols * Im->NumPlanes); 
     } 
  }
  return(1); 
} 


extern int ImAllSampleBytesEqual(Image *Im)
{
  int i;
  if ((Im->ImState < ImStatePeeked) || (Im->ImState >= ImStateDead))
    return 0;
  for (i=1;i<Im->NumPlanes;i++) {
    if (Im->SampleBytes[i] != Im->SampleBytes[0]) return 0;
  }
  return Im->SampleBytes[0];
}

extern int ImQclicSOnCurve(Image *Im, long S)
{
  int i;
  if ((Im->ImState < ImStatePeeked) || (Im->ImState >= ImStateDead) ||
      !Im->qclic.have_info || (Im->qclic.curve_points <= 0))
    return -1;
  for (i=0;i<Im->qclic.curve_points;i++) {
    if (Im->qclic.curve_size[i] >= S) return i;
  }
  return (Im->qclic.curve_points - 1);
}

extern int ImQclicQOnCurve(Image *Im, float Q)
{
  int i;
  if ((Im->ImState < ImStatePeeked) || (Im->ImState >= ImStateDead) ||
      !Im->qclic.have_info || (Im->qclic.curve_points <= 0))
    return -1;
  for (i=0;i<Im->qclic.curve_points;i++) {
    if (Im->qclic.curve_quality[i] >= Q) return i;
  }
  return (Im->qclic.curve_points - 1);
}

extern int ImQclicCurvePoint(Image *Im, int i, long *S, long *BetterS,
  float *Q, char *qfilename)
{
  int j;
  long upcorr = 0, downcorr = 0;
  if ((Im->ImState < ImStatePeeked) || (Im->ImState >= ImStateDead) ||
      !Im->qclic.have_info || (Im->qclic.curve_points <= 0) ||
      (i < 0) || (i >= Im->qclic.curve_points))
    return 0;
  if (S) *S = Im->qclic.curve_size[i];
  if (Q) *Q = Im->qclic.curve_quality[i];

  if (qfilename) {

    if (Im->qclic.curve_parfiles[i]) { 
      if (DS_file_src(&(Im->ImgSrc))) { 
        if (!strcmp(Im->qclic.dir,".")) {
          dirOf(DS_name(&(Im->ImgSrc)), qfilename);
          if (!strcmp(qfilename,".")) strcpy(qfilename,"");
          else strcat(qfilename,"/");
        } else sprintf(qfilename,"%s/",Im->qclic.dir);
      } else {
        if (!strcmp(Im->qclic.webdir,".")) {
          dirOf(DS_name(&(Im->ImgSrc)), qfilename);
          if (!strcmp(qfilename,".")) strcpy(qfilename,"");
          else strcat(qfilename,"/");
        } else sprintf(qfilename,"%s/",Im->qclic.webdir);
      } 
      strcat(qfilename,Im->qclic.curve_parfiles[i]); 
    } else 
      strcpy(qfilename,"");
  }

  if (BetterS) {
    if (Im->qclic.curve_better_size[i] > 0) {
      *BetterS = Im->qclic.curve_better_size[i];
    } else {
      downcorr = upcorr = 0;
      for (j=(i+1);j<Im->qclic.curve_points;j++) {
        if (Im->qclic.curve_better_size[j] > 0) {
	  upcorr = Im->qclic.curve_better_size[j]
	         - Im->qclic.curve_size[j];
  	  break;
        }
      }
      for (j=(i-1);j>=0;j--) {
        if (Im->qclic.curve_better_size[j] > 0) {
	  downcorr = Im->qclic.curve_better_size[j]
	           - Im->qclic.curve_size[j];
	  break;
        }
      }
      j = 0;
      if (upcorr) j++;
      if (downcorr) j++;
      if (j) {
        upcorr = (upcorr + downcorr)/j;
      }
      *BetterS = Im->qclic.curve_size[i] + upcorr;
    }
  }
  return 1;
}

extern int ImQclicSOnBetterCurve(Image *Im, long S)
{
  int i;
  long betterS;
  if ((Im->ImState < ImStatePeeked) || (Im->ImState >= ImStateDead) ||
      !Im->qclic.have_info || (Im->qclic.curve_points <= 0))
    return -1;
  for (i=0;i<Im->qclic.curve_points;i++) {
    (void) ImQclicCurvePoint(Im, i, 0, &betterS, 0, 0);
    if (betterS >= S) return i;
  }
  return (Im->qclic.curve_points - 1);

}

extern int ImQclicSetBetterSize(Image *Im, int i, long S)
{
  if ((Im->ImState < ImStatePeeked) || (Im->ImState >= ImStateDead) ||
      !Im->qclic.have_info || (Im->qclic.curve_points <= 0) ||
      (i < 0) || (i >= Im->qclic.curve_points))
    return 0;
  Im->qclic.curve_better_size[i] = S;
  return 1;
}

extern int ImWriteQclicInfo(Image *Im, DataDest *dd)
{
  if ((Im->ImState < ImStatePeeked) || (Im->ImState >= ImStateDead) ||
      !Im->qclic.have_info )
	return 0;

  return QclicInfoWrite(&Im->qclic, dd); 

}


#ifdef HAVE_SPIHT

extern void FreeImgSpiht(Image *Im)
{
  if (Im->spiht_data_bytes > 0) {
    remove(Im->spiht_infile);
    Im->spiht_data_bytes = 0;
  }
}


extern int ReadSpihtHeader(Image *Im, int *pelbytes)
{
  unsigned char head_bytes[8];
  unsigned long w, h;

  if (DS_Read(&(Im->ImgSrc), head_bytes, 8) != 8) {
    if (!Im->Silent)
      (void) DD_WriteLine(&Im->ImgErr, 
	"SPIHT data doesn't even have full header!");
    return 0;
  }

  w = ((unsigned long) head_bytes[0]); 
  w += (((unsigned long) head_bytes[1]) << 8); 
  w += (((unsigned long) head_bytes[2]) << 16); 
  h = ((unsigned long) head_bytes[3]); 
  h += (((unsigned long) head_bytes[4]) << 8); 
  h += (((unsigned long) head_bytes[5]) << 16); 
  /* extract pelbytes */
  *pelbytes = head_bytes[7]; 


  tmpnam(Im->spiht_infile);
  DD_Init(&Im->spiht_src); 
  if ( !DD_OpenFile(&Im->spiht_src,Im->spiht_infile)) {
    if (!Im->Silent)
      (void) DD_WriteLine(&Im->ImgErr, 
	"Could not create temporary file for SPIHT data");
    return 0;
  }
  if ((DD_Write(&Im->spiht_src, "SPHTG", 5) != 5) || 
      (DD_Write(&Im->spiht_src, head_bytes, 8) != 8)) {
    DD_Close(&Im->spiht_src);
    remove(Im->spiht_infile);
    if (!Im->Silent)
      (void) DD_WriteLine(&Im->ImgErr,
	"Could not write temporary file for SPIHT data");
  }
  Im->spiht_data_bytes = 13;

  Im->NumRows = h;
  Im->NumCols = w;
  Im->NumPlanes = 1;

  return(1);
}


extern int ReadSpihtImg(Image *Im)
{
  #define SPIHT_BUFF_SIZE 4096
  long bytes_read, totalbytes; 
  char spiht_buff[SPIHT_BUFF_SIZE]; 
  DataSrc ds;
  char spiht_decoder[DS_STRLENMAX]; 

  while ((bytes_read = DS_Read(&(Im->ImgSrc), spiht_buff,
	   SPIHT_BUFF_SIZE)) > 0) {
	Im->spiht_data_bytes += bytes_read;
	DD_Write(&Im->spiht_src, spiht_buff, bytes_read);
  }
  DD_Flush(&Im->spiht_src); 

  sprintf(spiht_decoder,"|decdtree -s %s - %ld", Im->spiht_infile, 
    Im->spiht_data_bytes);
  DS_Init(&ds, 0);
  if (!DS_OpenFile(&ds, spiht_decoder, 0)) {
    DD_Close(&Im->spiht_src);
    remove(Im->spiht_infile);
    Im->spiht_data_bytes = 0;
    if (!Im->Silent)
      (void) DD_WriteLine(&Im->ImgErr, "Could not decode SPIHT data");
    return 0;
  }


  totalbytes = ((long) Im->NumRows) * Im->NumCols * Im->SampleBytes[0];

  /* assumes all the prelude has been read */

  bytes_read = DS_Read(&ds,Im->Im[0],totalbytes);

  DS_Close(&ds);

  Im->ImState = ImStateReadSome; 
  Im->LastChangedRow[0] = Im->NumRows - 1; 

  ImFlipBytesIfNeeded(Im,0,Im->SrcEndian[0]);
  return(1);
}

extern int ReadMoreSpihtImg(Image *Im)
{
  return ReadSpihtImg(Im);
}

#endif /* HAVE_SPIHT */ 

