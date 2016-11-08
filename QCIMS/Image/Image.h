#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#ifndef IM_STRLENMAX
#define IM_STRLENMAX 400
#endif

#ifndef boolean
#define boolean int
#define HAVE_BOOLEAN
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifdef HAVE_JPEGLIB
#include <setjmp.h>
#define JPEG_INTERNALS
#include "jpeglib.h"
#include "jerror.h"
#include "jdynam.h"

struct my_jpg_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */
  jmp_buf setjmp_buffer;        /* for return to caller */
};

typedef struct my_jpg_error_mgr * my_jpg_error_ptr;

#endif

#include "DataSrcDest.h" 
#include "Qclic.h" 

#define IM_MAXPLANES 5

/****** image kinds **********/


#define IM_UNKNOWN 0 /* not an actual kind */
#define IM_RAW  1        
#define IM_PGM_ASCII 2
#define IM_PGM_RAW 3
#define IM_PPM_ASCII 4
#define IM_PPM_RAW  5
#define IM_PGM 6     /* not an actual kind */
#define IM_PPM 7     /* not an actual kind */
#define IM_PNM 8     /* not an actual kind */
#define IM_NOT_RAW 9 /* not an actual kind */
#define IM_JPEG 10
#define IM_JPEG_GRAY 11
#define IM_JPEG_RGB 12
#define IM_SPIHT_GRAY 13

#define IM_KIND_NUM 14

#if defined(NEED_IM_KIND_NAMES)
static const char *ImKindNames[IM_KIND_NUM] =
{
  "Unknown", 
  "RAW", 
  "PGM (ascii)", 
  "PGM (raw)", 
  "PPM (ascii)", 
  "PPM (raw)", 
  "PGM", 
  "PPM", 
  "PNM", 
  "Not-RAW", 
  "JPEG", 
  "JPEG (Gray)", 
  "JPEG (Color)",
  "SPIHT (Gray)" 
};
#endif
 
#define ImKindString(k) \
  ((((k) < 0) || ((k) >= IM_KIND_NUM)) ? ImKindNames[IM_UNKNOWN] : \
                                     ImKindNames[(k)])

/** ImKind inclusions:
  apart from <kind> < <kind>,

  PGM_ASCII, PGM_RAW < PGM, PNM 
  PPM_ASCII, PPM_RAW < PPM, PNM
   everything < NOT_RAW, except RAW
**/

#define ImKindSubSetOf(a,b) \
     ((a==b) ? 1 : \
     ((((a==IM_PGM_ASCII) || (a==IM_PGM_RAW) || (a==IM_PGM)) && \
       ((b==IM_PGM) || (b==IM_PNM))) ? 1 : \
     ((((a==IM_PPM_ASCII) || (a==IM_PPM_RAW) || (a==IM_PPM)) && \
       ((b==IM_PPM) || (b==IM_PNM))) ? 1 : \
     ((((a==IM_JPEG_GRAY) || (a==IM_JPEG_RGB)) && \
       (b==IM_JPEG)) ? 1 : \
     (((b==IM_NOT_RAW) && (a!=IM_RAW) && (a!=IM_UNKNOWN)) ? 1 : \
	     0)))))


/*********************************************
  Meanings for ImState:
**********************************************/

#define ImStateNotExtant 0
#define ImStateInited 1
#define ImStatePeeked 2
#define ImStateReadSome 3
#define ImStateReadAll 4
#define ImStateDead 5


typedef enum ImColorSpaceEnum {ImC_None, ImC_Gray, ImC_RGB, ImC_YCbCr} ImColorSpaceType;


#define ImPlanesOfCspace(csp) \
  ( ((csp)==ImC_Gray) ? 1  : \
  ( ((csp)==ImC_RGB) ? 3   :  \
  ( ((csp)==ImC_YCbCr) ? 3 :  \
    0 )))

#define ImCspaceCompatible(c1, c2) (ImPlanesOfCspace(c1) == ImPlanesOfCspace(c2))

typedef unsigned char Pixel;
typedef unsigned short Pixel2;
typedef unsigned long Pixel4;

#define Endian_LSBFirst 0
#define Endian_MSBFirst 1



/******* images ****************/
struct ImageStruct {
  char *ImName; 
  int Silent; /* default 0 */
  int NumRows; /* default 512 */
  int NumCols; /* default 512 */
  int NumPlanes; /* default 1 */
  int ImKind; /* may be set by user, in which case
		 an error will be reported if the actual
		 kind is not the same (or not a subkind) */


  boolean ForceImKind;

  int CurrRows[IM_MAXPLANES]; /* current # rows */
  int CurrCols[IM_MAXPLANES]; /* current # cols */

  boolean SmoothingDone[IM_MAXPLANES];
  
  boolean AllPlanesEqual;

  boolean WasCropped;
  int CropStartRow; 
  int CropStartCol; 
  int CropEndRow; 
  int CropEndCol; 

  int Brightness[IM_MAXPLANES];

  float Contrast[IM_MAXPLANES]; 


  ImColorSpaceType OrigCspace;
  ImColorSpaceType CurrCspace;


  boolean Changed[IM_MAXPLANES];

  boolean RetainOrigOnChange;
  char *RetainedData[IM_MAXPLANES]; 


  int ImRestartPt; /* internal info used to resume image reading
		      by ReadMoreImg. Its meaning depends upon
  		      ImKind */
  long ImRestartAux[15];
  
  #ifdef HAVE_SPIHT
  char spiht_infile[L_tmpnam+10];
  DataDest spiht_src;
  long spiht_data_bytes; 
  #endif

  #ifdef HAVE_JPEGLIB
  /** info for decompressing JPEG image **/
  J_DCT_METHOD rough_idct; 
  J_DCT_METHOD final_idct; 
  unsigned char * jpg_im;
  JSAMPARRAY jpg_outbuff;
  struct jpeg_decompress_struct jpg_info;
  struct my_jpg_error_mgr jpg_err;
  boolean jinfo_alive;
  /* ImRestartPt meanings: 0=jpeg_start_decompress
                           1=jpeg_start_output
			   2=jpeg_read_scanlines
			   3=jpeg_finish_output
		      */
  #endif


  DataDest ImgErr;   /* error messages printed here..
		        stderr by default */

  DataSrc ImgSrc; 
  boolean ImgSrcInited; 
  long TotalImBytes; 
  boolean GuessedTotalImBytes; 

  int FirstChangedRow[IM_MAXPLANES];
  int LastChangedRow[IM_MAXPLANES]; 

  int SrcEndian[IM_MAXPLANES]; /* default: Endian_MSBFirst; can be set to Endian_LSBFirst */
  int HostEndian[IM_MAXPLANES]; /* will be set to the machine's endian */
  int SaveEndian[IM_MAXPLANES]; /* default: Endian_MSBFirst; can be set to Endian_LSBFirst */
  int SampleBits[IM_MAXPLANES];
  int SampleBytes[IM_MAXPLANES];
  unsigned long MaxSample[IM_MAXPLANES];

  Pixel *Im[IM_MAXPLANES];
    /* each 2-D component will be stored in row-major order in the
       appropriate Im[][] array
    */
  Pixel2 *Im2[IM_MAXPLANES]; /* Im2 and Im4 are alternate ways to access 
				 images with 2 and 4 bytes-per-sample,
				 respectively */
  Pixel4 *Im4[IM_MAXPLANES];

  char ImFilterUsed[IM_STRLENMAX]; 
  unsigned char FirstFiveChars[5];
  int ImState;

  /* quality information */
  QclicInfo qclic;

};


typedef struct ImageStruct Image;



/********************** prototypes ************************/



extern void ImInitImg(Image *Im);
extern int  ImPrintImgChars(Image *Im, DataDest *dd);
extern int  ImSetSampleBits(Image *Im, int nbits, int planenum);
extern int  ImSetKindRaw(Image *Im, int nplanes, int rows, int cols, boolean enforce);
extern int  ImSetKind(Image *Im, int imkind, boolean enforce);
extern int  ImPeekImg(Image *Im);
extern int  ImSetImgFile(Image *Im, char *fname, long nbytes);
extern int  ImSetImgMemory(Image *Im, char *buff, long nbytes);
extern int  ImSetImgFd(Image *Im, int fd, long nbytes);
extern int  ImSetImgStream(Image *Im, FILE *fp, long nbytes);
extern int  ImExtendImgBytes(Image *Im, long nbytes);
extern int  ImSetImgBytes(Image *Im, long nbytes);
extern int  ImReadImg(Image *Im);
extern int  ImReadMoreImg(Image *Im);

/* next 10 fns return 0 on failure and #bytes written on success */

extern long  ImSaveImg(Image *Im, char * fname, int saveKind);
extern long  ImWriteImg(Image *Im, int fd, int saveKind);
extern long  ImStreamWriteImg(Image *Im, FILE *fp, int saveKind);
extern long  ImBufferImg(Image *Im, char *buff, long buffsize,
			  char **extramem, int saveKind);
extern long  ImSendImg(Image *Im, DataDest *dd, char **extramem,
	    int saveKind);

extern long ImPSSendImg(Image *Im, DataDest *dd, 
  int do_center, int pgwid, int pghgt); 
extern long ImPSSaveImg(Image *Im, char *fname, 
  int do_center, int pgwid, int pghgt); 
extern long ImPSWriteImg(Image *Im, int fd, 
  int do_center, int pgwid, int pghgt); 
extern long ImPSStreamWriteImg(Image *Im, FILE *fp, 
  int do_center, int pgwid, int pghgt); 
extern long ImPSBufferImg(Image *Im, char *buff, long buffsize,
  char **extramem, int do_center, int pgwid, int pghgt); 

extern void ImFreeImg(Image *Im);

#define ImStateOfImg(Im) ((Im)->ImState)

extern int ImSetImgDim(Image *Im, int planenum, int rows, int cols,
	       int do_smooth, int maintain_aspect);
extern int ImStretchAndShrink(Image *Im, int planenum, 
	       int vert_stretch, int vert_shrink,
	       int horz_stretch, int horz_shrink,
	       int do_smooth, int maintain_aspect);
extern int ImCrop(Image *Im, int start_row, int start_col,
   int end_row, int end_col);

/* ImStretchAndShrink always applies to the current image state */
/* for all the above, setting planenum to -1 implies all image planes */

extern int ImBackToNormal(Image *Im);

extern int ImBackToNormalPixels(Image *Im);

extern int ImDiscardOriginal(Image *Im);

#ifdef HAVE_JPEGLIB

#define ImSetRoughIDCT(Im,meth) (Im)->rough_idct = (meth)  
#define ImSetFinalIDCT(Im,meth) (Im)->final_idct = (meth)  

/********************** JPEG compression routines ****************

  These next 4 fns return 0 on failure and #bytes written on success.
  For these JPEG compression routines, here are the meanings of some
  of the parameters:

  int on_original
    1 iff original image is to be compressed.  0 for current image,
    which might have been resized/brightened/contrasted. 
  void *comp_till
    Pointer to a desired quality or size.  "comp_till_metric" determines
    its meaning. Ignored if 0. Overrides the "quality" parameter below.
    If "qsrc" is also provided, it *will* be used and comp_till will be
    ignored. If "retries_comp_till" is set along with "qsrc" and
    "comp_till", then compressed file will be pruned (if needed) to meet
    "comp_till". There never are any guarantees that "comp_till" will
    be met, but with "retries_comp_till" set, we'll get very close. 
  char *comp_till_metric
    Determines the meaning of comp_till. Can be any of the strings
    allowed for "qclicqmetric". The ones you need to really know are:
      "SIZE"
	 comp_till is (long *) pointer to a desired size in bytes. 
      "PSIZE"
	 comp_till is (float *) ptr to a desired percentange size, where
         the percentage is taken wrt the uncompressed size in bytes of
	 the image (rows * cols * #planes). 
      NULL or "PSNR"
	 comp_till is (float *) ptr to a desired PSNR.
   int retries_comp_till
     Try hard to achieve comp_till. If "qsrc" is provided, the
     quantization tables in qsrc put an upper bound on achievable
     quality, and we will "try hard" only if "comp_till" is less
     than that upper bound. If "qsrc" is not provided, we'll pick
     a quantization table (eventually using RDOPT) and/or prune
     the compressed file, to come very close to the target. 

*********************************************************************/

extern long ImSaveJpeg(Image *Im,
  int on_original, 
  void *comp_till, 
  char *comp_till_metric,  
  float neg_tolerance, 
  float pos_tolerance, 
  int retries_comp_till, 
  char *fname,
  char *qclicfname,
  long *qclicbytes,
  char *qclicqmetric, 
  void * qsrc, long qsrc_bytes, int qsrc_kind,  
  void * ssrc, long ssrc_bytes, int ssrc_kind);   
extern long ImWriteJpeg(Image *Im,
  int on_original, 
  void *comp_till, 
  char *comp_till_metric,  
  float neg_tolerance, 
  float pos_tolerance, 
  int retries_comp_till, 
  int fd,
  int qclicfd,
  long *qclicbytes,
  char *qclicqmetric, 
  void * qsrc, long qsrc_bytes, int qsrc_kind,  
  void * ssrc, long ssrc_bytes, int ssrc_kind);   
extern long ImStreamWriteJpeg(Image *Im,
  int on_original, 
  void *comp_till, 
  char *comp_till_metric,  
  float neg_tolerance, 
  float pos_tolerance, 
  int retries_comp_till, 
  FILE *fp, 
  FILE *qclicfp, 
  long *qclicbytes,
  char *qclicqmetric, 
  void * qsrc, long qsrc_bytes, int qsrc_kind,  
  void * ssrc, long ssrc_bytes, int ssrc_kind);   
extern long ImBufferJpeg(Image *Im,
  int on_original, 
  void *comp_till, 
  char *comp_till_metric,  
  float neg_tolerance, 
  float pos_tolerance, 
  int retries_comp_till, 
  char *buff, long buffsize, char **extramem,
  char *qclicbuff, long qclicbuffsize, char **qclicextramem,
  long *qclicbytes,
  char *qclicqmetric, 
  void * qsrc, long qsrc_bytes, int qsrc_kind,  
  void * ssrc, long ssrc_bytes, int ssrc_kind);   
extern long ImSendJpeg(Image *Im,
  int on_original, 
  void *comp_till, 
  char *comp_till_metric,  
  float neg_tolerance, 
  float pos_tolerance, 
  int retries_comp_till, 
  DataDest *dd, char **extramem,
  DataDest *qclicdd, char **qclicextramem,
  long *qclicbytes,
  char *qclicqmetric, 
  void * qsrc, long qsrc_bytes, int qsrc_kind,  
  void * ssrc, long ssrc_bytes, int ssrc_kind);   



#endif

#define ImSetRetainOrig(Im)  (Im)->RetainOrigOnChange = TRUE; 

extern int ImSetOrigCspace(Image *Im, ImColorSpaceType cspace); /* rename original 
							       color space */
extern int ImChangeCspace(Image *Im, ImColorSpaceType cspace);

extern int ImBrighten(Image *Im, int planenum, int bright_add);

extern int ImContrast(Image *Im, int planenum, float contrast); 

#define ImKindOfImg(Im) ((Im)->ImKind)

#define ImSetSilent(Im) (Im)->Silent = 1
#define ImSetNotSilent(Im) (Im)->Silent = 0

#define ImOrigRows(Im) ((Im)->NumRows)
#define ImOrigCols(Im) ((Im)->NumCols)

#define ImCurrRows(Im) ((Im)->CurrRows[0])
#define ImCurrCols(Im) ((Im)->CurrCols[0])

#define ImPlanesOfImg(Im) ((Im)->NumPlanes)

extern int ImChanged(Image *Im);

#define ImPlaneChanged(Im,n) ((Im)->Changed[(n)])

extern int ImSetErrFile(Image *Im, char *f);  
extern int ImSetErrStream(Image *Im, FILE *fp);  
extern int ImSetErrFd(Image *Im, int fd); 

extern int ImAllSampleBytesEqual(Image *Im); 

/* these fields are useful */ 
#define ImFirstChanged(Im,p) (Im)->FirstChangedRow[(p)]
#define ImLastChanged(Im,p) (Im)->LastChangedRow[(p)]

#define ImSampleBytes(Im) ((Im)->SampleBytes[0])
#define ImPlaneSampleBytes(Im, pnum) \
  (((pnum) < 0) ? ((Im)->SampleBytes[0]) : \
  (((pnum) >= (Im)->NumPlanes) ? ((Im)->SampleBytes[(Im)->NumPlanes - 1]) : \
  ((Im)->SampleBytes[(pnum)])))

#define ImHostEndian(Im) ((Im)->HostEndian[0])
#define ImPlaneHostEndian(Im, pnum) \
  (((pnum) < 0) ? ((Im)->HostEndian[0]) : \
  (((pnum) >= (Im)->NumPlanes) ? ((Im)->HostEndian[(Im)->NumPlanes - 1]) : \
  ((Im)->HostEndian[(pnum)])))


#define ImCurrPlaneRows(Im, pnum) \
  (((pnum) < 0) ? ((Im)->CurrRows[0]) : \
  (((pnum) >= (Im)->NumPlanes) ? ((Im)->CurrRows[(Im)->NumPlanes - 1]) : \
  ((Im)->CurrRows[(pnum)])))

#define ImCurrPlaneCols(Im, pnum) \
  (((pnum) < 0) ? ((Im)->CurrCols[0]) : \
  (((pnum) >= (Im)->NumPlanes) ? ((Im)->CurrCols[(Im)->NumPlanes - 1]) : \
  ((Im)->CurrCols[(pnum)])))

#define ImAllPlanesEqual(Im) ((Im)->AllPlanesEqual)

#define ImColorSpace(Im) ((Im)->CurrCspace)
#define ImOrigColorSpace(Im) ((Im)->OrigCspace)


#define ImNameOfImg(Im) ((Im)->ImName)

#define ImSetName(Im, name) strcpy((Im)->ImName, (name))

#define ImGetCropInfo(Im, iscropped, sr, sc, er, ec) \
  (iscropped) = (Im)->WasCropped; \
  (sr) = (Im)->CropStartRow; \
  (sc) = (Im)->CropStartCol; \
  (er) = (Im)->CropEndRow; \
  (ec) = (Im)->CropEndCol

#define ImFileNameOfImg(Im) (DS_name(&((Im)->ImgSrc)))

#define ImFilterOfImg(Im) ((Im)->ImFilterUsed)

#define ImBytesRead(Im) (DS_total_count(&((Im)->ImgSrc)))

extern int ImSetTotalBytes(Image *Im, long tot, int isguess); 

#define ImResetSrcBytes(Im, tot) \
  DS_ResetSrcBytes(&((Im)->ImgSrc), (tot))


#define ImTotalBytes(Im) (Im)->TotalImBytes

#define ImUserGivenFd(Im) (DS_fd_src(&((Im)->ImgSrc))) 

#define ImStdinSrc(Im) (!strcmp(DS_name(&((Im)->ImgSrc)),DSD_NAME_STDIN))

#define ImSrcAllRead(Im) (DS_all_read(&((Im)->ImgSrc)))

#define ImPlane(iptr,n) ((iptr)->Im[(n)])
#define Im2Plane(iptr,n) ((iptr)->Im2[(n)])
#define Im4Plane(iptr,n) ((iptr)->Im4[(n)])

#define ImRetainedData(Im,n) ((Im)->RetainedData[(n)])

extern char ImErrorString[IM_STRLENMAX];

extern long ImQclicIcurveSofQ(Image *Im, float quality, boolean is_percent);

extern float ImQclicIcurveQofS(Image *Im, long nbytes, boolean in_percent);

#define ImQclicIcurveQmetric(Im) (Im)->qclic.icurve_qmetric

#define ImQclicIcurvePoints(Im) (Im)->qclic.icurve_points

#define ImQclicIcurveS(Im,i) (Im)->qclic.icurve_size[(i)] 
#define ImQclicIcurveQ(Im,i) (Im)->qclic.icurve_quality[(i)] 
#define ImQclicHeaderBytes(Im) (Im)->qclic.qclic_info_end_offset

#define ImCopyQclicInfo(Imtgt, Imsrc) \
  QclicInfoCopy(&((Imtgt)->qclic), &((Imsrc)->qclic))

#define ImIsCompressed(Im) \
  (ImKindSubSetOf((Im)->ImKind, IM_JPEG) || \
   ImKindSubSetOf((Im)->ImKind, IM_SPIHT_GRAY))

#define ImHasQclicCurve(Im) \
  ((Im)->qclic.have_info && ((Im)->qclic.curve_points > 0))
#define ImQclicCurveFormat(Im) \
  ( (Im)->qclic.curve_format )
#define ImQclicCurveNumPoints(Im) \
  ((Im)->qclic.have_info ? (Im)->qclic.curve_points : 0 )

extern char * ImQclicLossless(Image *Im);

extern int ImQclicSOnCurve(Image *Im, long S);
extern int ImQclicSOnBetterCurve(Image *Im, long S);
extern int ImQclicQOnCurve(Image *Im, float Q);
extern int ImQclicSetBetterSize(Image *Im, int i, long S);
extern int ImQclicCurvePoint(Image *Im, int i, long *S, long *BetterS,
	   float *Q, char *qfilename);

extern int ImWriteQclicInfo(Image *Im, DataDest *dd);

#endif /* IMAGE_H_INCLUDED */


