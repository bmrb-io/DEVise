#ifndef QCLIC_H
#define QCLIC_H

#include "DataSrcDest.h"

#define QCLIC_STRLENMAX DS_STRLENMAX

#define QCLIC_SMALLSTRLENMAX 20

#define QCLIC_UNKNOWN_SOURCE "<unknown>"


struct QclicInfoStruct {
  int have_info; 
  char name[QCLIC_STRLENMAX]; 
  int width, height, planes;

  /* internal (embedded) quality curve */ 
  int icurve_points;
  char icurve_qmetric[QCLIC_SMALLSTRLENMAX];
  long *icurve_size; 
  float *icurve_quality;

  /* external quality curve for the lossless version of this image */ 
  int curve_points;
  char curve_format[QCLIC_SMALLSTRLENMAX]; 
  char curve_qmetric[QCLIC_SMALLSTRLENMAX];
  long *curve_size; /* estimate */ 
  long *curve_better_size; /* "better" estimate */ 
  float *curve_quality;
  char **curve_parfiles; 



  char source[QCLIC_STRLENMAX];

  int is_lossless; 

  int know_lossless; 
  char lossless[QCLIC_STRLENMAX];

  char dir[QCLIC_STRLENMAX];
  char webdir[QCLIC_STRLENMAX];

  long qclic_info_end_offset; 
};

typedef struct QclicInfoStruct QclicInfo;

extern void QclicInfoInit(QclicInfo *qinfo);

extern void QclicInfoCopy(QclicInfo *qtgt, QclicInfo *qsrc);

extern int QclicInfoRead(QclicInfo *qinfo, DataSrc *ds);

extern int QclicInfoWrite(QclicInfo *qinfo, DataDest *dd);

extern void QclicInfoFree(QclicInfo *qinfo);



#define QM_SIZE 0          /* Size (in bytes) used as quality */ 
#define QM_PSIZE 1         /* Percent-size (wrt lossless) used as quality */ 

#define QM_PSNR 2          /* PSNR for grayscale */
#define QM_PSNR_PLUS 3     /* PSNR+ (add 1 to mse) for grayscale */
#define QM_AVG_PSNR 4      /* Average PSNR on Y,Cb/2,Cr/2 for RGB*/
#define QM_AVG_PSNR_PLUS 5 /* Avg PSNR+ on Y,Cb/2,Cr/2 for RGB */
				/* prefix "I" on next 4 defs indicates 
				   that the lossy image was compared
				   with another lossy image, rather
				   than the lossless version */ 
#define QM_IPSNR 6
#define QM_IPSNR_PLUS 7
#define QM_AVG_IPSNR 8
#define QM_AVG_IPSNR_PLUS 9
                                /* prefix "ITP" on next 8 defs indicates
				   that the following PSNRs for 
				   incremental images are the actual
				   PSNRs for the first and last scan,
				   and interpolated values for 
				   intermediate scans */ 
#define QM_ITP_PSNR 10        
#define QM_ITP_PSNR_PLUS 11   
#define QM_ITP_AVG_PSNR 12    
#define QM_ITP_AVG_PSNR_PLUS 13
#define QM_ITP_IPSNR 14
#define QM_ITP_IPSNR_PLUS 15
#define QM_ITP_AVG_IPSNR 16
#define QM_ITP_AVG_IPSNR_PLUS 17

#define QM_BPP 18

#define QM_UNKNOWN 19

#define QM_COUNT 20

#define QM_DEFAULT QM_ITP_PSNR


/************** string names for reference ************
const char *QmetricNames[QM_COUNT] = { 
   "SIZE", "PSIZE", 
   "PSNR","PSNR+","AVG-PSNR", "AVG-PSNR+", 
   "IPSNR","IPSNR+","AVG-IPSNR", "AVG-IPSNR+", 
   "ITP-PSNR","ITP-PSNR+","ITP-AVG-PSNR", "ITP-AVG-PSNR+", 
   "ITP-IPSNR","ITP-IPSNR+","ITP-AVG-IPSNR", "ITP-AVG-IPSNR+", 
   "BPP", "UNKNOWN" };
*******************************************************/ 

extern int QmetricKind(char *qm);

extern char *Qmetric(int qm_kind);

#define QM_NEEDS_PSNR(k) \
  (((k)>=QM_PSNR) && ((k)<=QM_ITP_AVG_IPSNR_PLUS))
#define QM_ADDS1_TO_DIST(k) \
  (((k)==QM_PSNR_PLUS)||((k)==QM_AVG_PSNR_PLUS) || \
   ((k)==QM_IPSNR_PLUS)||((k)==QM_AVG_IPSNR_PLUS) || \
   ((k)==QM_ITP_PSNR_PLUS)||((k)==QM_ITP_AVG_PSNR_PLUS) || \
   ((k)==QM_ITP_IPSNR_PLUS)||((k)==QM_ITP_AVG_IPSNR_PLUS) \
  )
#define QM_INTERPOLATES(k) \
  (((k)>=QM_ITP_PSNR) && ((k)<=QM_ITP_AVG_IPSNR_PLUS))
#define QM_FOR_GRAYSCALE(k) \
  (((k)==QM_SIZE) || ((k)==QM_PSIZE) || ((k)==QM_BPP) || \
   ((k)==QM_PSNR) || ((k)==QM_PSNR_PLUS) || \
   ((k)==QM_IPSNR) || ((k)==QM_IPSNR_PLUS) || \
   ((k)==QM_ITP_PSNR) || ((k)==QM_ITP_PSNR_PLUS) || \
   ((k)==QM_ITP_IPSNR) || ((k)==QM_ITP_IPSNR_PLUS) \
  )
#define QM_FOR_RGB(k) \
  (((k)==QM_SIZE) || ((k)==QM_PSIZE) || ((k)==QM_BPP) || \
     !(QM_FOR_GRAYSCALE(k)))
#define QM_NEEDS_LOSSLESS(k) \
  (((k)==QM_PSNR)||((k)==QM_AVG_PSNR) || \
   ((k)==QM_ITP_PSNR)||((k)==QM_ITP_AVG_PSNR) || \
   ((k)==QM_PSNR_PLUS)||((k)==QM_AVG_PSNR_PLUS) || \
   ((k)==QM_ITP_PSNR_PLUS)||((k)==QM_ITP_AVG_PSNR_PLUS) \
  )

#define QM_MAKE_ADD1_TO_DIST(k) \
  { if (QM_NEEDS_PSNR(k) && !QM_ADDS1_TO_DIST(k)) (k)++; }
#define QM_MAKE_NO_ADD1_TO_DIST(k) \
  { if (QM_NEEDS_PSNR(k) && QM_ADDS1_TO_DIST(k)) (k)--; }

#define QM_MAKE_INTERPOLATE(k) \
  { if (QM_NEEDS_PSNR(k) && !QM_INTERPOLATES(k)) (k)+=8; }
#define QM_MAKE_NO_INTERPOLATE(k) \
  { if (QM_NEEDS_PSNR(k) && QM_INTERPOLATES(k)) (k)-=8; }

#define QM_MAKE_GRAYSCALE(k) \
  { if (QM_NEEDS_PSNR(k) && QM_FOR_RGB(k)) (k)-=2; }
#define QM_MAKE_RGB(k) \
  { if (QM_NEEDS_PSNR(k) && QM_FOR_GRAYSCALE(k)) (k)+=2; }

#define QM_MAKE_NEED_LOSSLESS(k) \
  { if (QM_NEEDS_PSNR(k) && !QM_NEEDS_LOSSLESS(k)) (k)-=4; }
#define QM_MAKE_NO_NEED_LOSSLESS(k) \
  { if (QM_NEEDS_PSNR(k) && QM_NEEDS_LOSSLESS(k)) (k)+=4; }

#endif /* of ifndef QCLIC_H */

