#include "Qclic.h"
#include <string.h>

#define FREE_AND_ZERO(ptr) {if (ptr) free(ptr); (ptr) = 0;}

extern void QclicInfoInit(QclicInfo *qinfo)
{
  if (!qinfo) return;

  qinfo->have_info = 0; 
  strcpy(qinfo->name,"Unknown");
  qinfo->width = qinfo->height = qinfo->planes = 0;

  qinfo->icurve_points = 0; 
  strcpy(qinfo->icurve_qmetric,"unknown");
  qinfo->icurve_size = (long *) 0; 
  qinfo->icurve_quality = (float *) 0; 

  qinfo->curve_points = 0; 
  strcpy(qinfo->curve_qmetric,"unknown");
  strcpy(qinfo->curve_format,"unknown");
  qinfo->curve_size = (long *) 0; 
  qinfo->curve_better_size = (long *) 0; 
  qinfo->curve_quality = (float *) 0; 
  qinfo->curve_parfiles = (char **) 0; 

  strcpy(qinfo->source,QCLIC_UNKNOWN_SOURCE);
  qinfo->know_lossless = qinfo->is_lossless = 0;
  strcpy(qinfo->lossless,QCLIC_UNKNOWN_SOURCE);

  strcpy(qinfo->dir,".");
  strcpy(qinfo->webdir,".");

  qinfo->qclic_info_end_offset = 0;
}



extern void QclicInfoCopy(QclicInfo *qtgt, QclicInfo *qsrc)
{
  /** copies only:
      name,width,height,planes
      dir, webdir
      know_lossless, lossless
      curve_*
   ** should be called before QclicInfoRead
   *******************************************/
  int i, j;

  if (!qsrc->have_info) return;

  qtgt->have_info = 1; 
  strcpy(qtgt->name, qsrc->name);
  qtgt->width = qsrc->width; 
  qtgt->height = qsrc->height; 
  qtgt->planes = qsrc->planes; 
  strcpy(qtgt->dir, qsrc->dir);
  strcpy(qtgt->webdir, qsrc->webdir);
  if ((qtgt->know_lossless = qsrc->know_lossless) != 0) {
    strcpy(qtgt->lossless, qsrc->lossless);
  }

  if (qsrc->curve_points > 0) {
    if (qtgt->curve_points > 0) {
      if (qtgt->curve_size) FREE_AND_ZERO(qtgt->curve_size);
      if (qtgt->curve_better_size) FREE_AND_ZERO(qtgt->curve_better_size);
      if (qtgt->curve_quality) FREE_AND_ZERO(qtgt->curve_quality);
      if (qtgt->curve_parfiles) {
	for (i=0;i<qtgt->curve_points;i++) {
	  if (qtgt->curve_parfiles[i]) FREE_AND_ZERO(qtgt->curve_parfiles[i]);
	}
	FREE_AND_ZERO(qtgt->curve_parfiles);
      }
    }

    qtgt->curve_size = (long *) calloc(1,sizeof(long)*qsrc->curve_points);
    qtgt->curve_better_size = (long *) calloc(1,sizeof(long)*qsrc->curve_points);
    qtgt->curve_quality = (float *) calloc(1,sizeof(float)*qsrc->curve_points);
    qtgt->curve_parfiles = (char **) calloc(1,sizeof(char *)*qsrc->curve_points);
    if (!qtgt->curve_size || !qtgt->curve_better_size || 
	!qtgt->curve_quality || !qtgt->curve_parfiles) {
      FREE_AND_ZERO(qtgt->curve_size);
      FREE_AND_ZERO(qtgt->curve_better_size);
      FREE_AND_ZERO(qtgt->curve_quality);
      FREE_AND_ZERO(qtgt->curve_parfiles);
      return; 
    }
    for (i=0;i<qsrc->curve_points;i++) {
      if (qsrc->curve_parfiles[i]) {
	if ((qtgt->curve_parfiles[i] = (char *)
	      malloc(QCLIC_STRLENMAX*sizeof(char))) == NULL) {
	  for (j=0;j<i;j++) FREE_AND_ZERO(qtgt->curve_parfiles[j]);
	  FREE_AND_ZERO(qtgt->curve_size);
          FREE_AND_ZERO(qtgt->curve_better_size);
          FREE_AND_ZERO(qtgt->curve_quality);
          FREE_AND_ZERO(qtgt->curve_parfiles);
          return; 
	}
      }
    }
    /* all memallocs OK.. copy data */
    memcpy((char *) qtgt->curve_size, (char *) qsrc->curve_size,
      sizeof(long)*qsrc->curve_points);
    memcpy((char *) qtgt->curve_better_size, (char *) qsrc->curve_better_size,
      sizeof(long)*qsrc->curve_points);
    memcpy((char *) qtgt->curve_quality, (char *) qsrc->curve_quality,
      sizeof(float)*qsrc->curve_points);
    for (i=0;i<qsrc->curve_points;i++) {
      if (qsrc->curve_parfiles[i])
	strcpy(qtgt->curve_parfiles[i], qsrc->curve_parfiles[i]);
    }
    qtgt->curve_points = qsrc->curve_points;
    strcpy(qtgt->curve_qmetric,qsrc->curve_qmetric);
    strcpy(qtgt->curve_format,qsrc->curve_format);

  }


}




extern void QclicInfoFree(QclicInfo *qinfo)
{
  int i; 
  if (!qinfo) return;
  if (qinfo->icurve_points) {
    if (qinfo->icurve_size) FREE_AND_ZERO(qinfo->icurve_size); 
    if (qinfo->icurve_quality) FREE_AND_ZERO(qinfo->icurve_quality); 
  }
  qinfo->icurve_points = 0;
  if (qinfo->curve_points > 0) {
      if (qinfo->curve_size) FREE_AND_ZERO(qinfo->curve_size);
      if (qinfo->curve_better_size) FREE_AND_ZERO(qinfo->curve_better_size);
      if (qinfo->curve_quality) FREE_AND_ZERO(qinfo->curve_quality);
      if (qinfo->curve_parfiles) {
	for (i=0;i<qinfo->curve_points;i++) {
	  if (qinfo->curve_parfiles[i]) FREE_AND_ZERO(qinfo->curve_parfiles[i]);
	}
	FREE_AND_ZERO(qinfo->curve_parfiles);
      }
  }
  qinfo->curve_points = 0; 
}

#define QCLIC_STRLAST (QCLIC_STRLENMAX-1)

static void StrSub(char *s, char *subsrc, char *subtgt)
{
  int nsubs = 0, lorig, lsubsrc, lsubtgt, ldiff, lfinal, i, j;
  char s_copy[QCLIC_STRLENMAX]; 
  char *s_loc = s, *s_copyptr;
  while ((s_loc = strstr(s_loc, subsrc)) != NULL){
    nsubs++;s_loc++;
  }

  lorig = strlen(s);
  lsubsrc = strlen(subsrc);
  lsubtgt = strlen(subtgt);
  ldiff = lsubtgt - lsubsrc; 
  i=0,lfinal=lorig;
  while ((lfinal + ldiff < QCLIC_STRLAST) && (i < nsubs)) {
    lfinal += ldiff; i++;
  }
  if (!(nsubs = i)) return;
  strcpy(s_copy, s);
  s_loc = s;
  s_copyptr = s_copy;
  for (i=0;i<nsubs;i++) {
    while (strncmp(s_loc,subsrc,lsubsrc)) {
      s_loc++;s_copyptr++;
    }
    for (j=0;j<lsubtgt;j++) *s_loc++ = subtgt[j];
    s_copyptr += lsubsrc;
    strcpy(s_loc, s_copyptr);
  }
}

static int GetALine(char *line, DataSrc *ds, QclicInfo *qinfo) 
{
  int i = 0, lastc = ' ';
  char c; 
  /**** does macro expansions for:
	$DIR $WEBDIR $SOURCE $LOSSLESS $NAME 
  *****/ 

  while (i < QCLIC_STRLAST) {
    if (!DS_Read(ds, &c, 1)) return(0);
    if (c == '\n') {
      /* success!! */
      line[i] = '\0';

      if (strstr(line,"$DIR"))
	StrSub(line,"$DIR",qinfo->dir); 
      if (strstr(line,"$(DIR)"))
	StrSub(line,"$(DIR)",qinfo->dir); 

      if (strstr(line,"$WEBDIR"))
	StrSub(line,"$WEBDIR",qinfo->webdir); 
      if (strstr(line,"$(WEBDIR)"))
	StrSub(line,"$(WEBDIR)",qinfo->webdir); 

      if (strstr(line,"$SOURCE"))
	StrSub(line,"$SOURCE",qinfo->source); 
      if (strstr(line,"$(SOURCE)"))
	StrSub(line,"$(SOURCE)",qinfo->source); 

      if (strstr(line,"$LOSSLESS"))
	StrSub(line,"$LOSSLESS",qinfo->lossless); 
      if (strstr(line,"$(LOSSLESS)"))
	StrSub(line,"$(LOSSLESS)",qinfo->lossless); 

      if (strstr(line,"$NAME"))
	StrSub(line,"$NAME",qinfo->name); 
      if (strstr(line,"$(NAME)"))
	StrSub(line,"$(NAME)",qinfo->name); 
      
      return(1); 
    } else {
      if ((c==' ') || (c=='\t')) {
	if (lastc != ' ') line[i++] = c;
	lastc = ' ';
      } else { 
        lastc = line[i++] = c;
      } 
    }
  }

  return(0); 
}

#define QclicRead_Any 0
#define QclicRead_Icurve 1
#define QclicRead_Curve 2
#define QclicRead_BetterCurve 3
#define QclicRead_Done 10 
#define QclicRead_Error -1


extern int QclicInfoRead(QclicInfo *qinfo, DataSrc *ds) 
{
  char QclicLine[QCLIC_STRLENMAX], foo[QCLIC_SMALLSTRLENMAX];
  char qfname[QCLIC_SMALLSTRLENMAX];
  int i, ret, where=0, npoints=0; 
  int doing; 
  long ltemp; 

  doing = QclicRead_Any; 

  qinfo->have_info = 1; 

  while ((doing != QclicRead_Done) && (doing != QclicRead_Error)) {

    while (((ret = GetALine(QclicLine,ds,qinfo)) != 0)  
	  && ((QclicLine[0] == '#') || (!strcmp(QclicLine,"")))); 
    if (!ret) break;

    switch(doing) {
      case QclicRead_Any:
        if (!strncasecmp(QclicLine,"QCLIC-END",9)) {
  	  doing = QclicRead_Done; 
        } else if (!strncasecmp(QclicLine,"SOURCE",3) || 
                   !strncasecmp(QclicLine,"SRC",3) ) {
	  sscanf(QclicLine,"%s%s",foo,qinfo->source);
        } else if (!strncasecmp(QclicLine,"DIR",3)) { 
	  sscanf(QclicLine,"%s%s",foo,qinfo->dir);
        } else if (!strncasecmp(QclicLine,"WEBDIR",4)) { 
	  sscanf(QclicLine,"%s%s",foo,qinfo->webdir);
        } else if (!strncasecmp(QclicLine,"NAME",4)) { 
	  sscanf(QclicLine,"%s%s",foo,qinfo->name);
        } else if (!strncasecmp(QclicLine,"LOSSLESS",4)) { 
	  if (!qinfo->is_lossless) {
	    qinfo->know_lossless = 1; 
	    sscanf(QclicLine,"%s%s",foo,qinfo->lossless);
	  }
	} else if (!strncasecmp(QclicLine,"DIM",3)) { 
	  sscanf(QclicLine,"%s%d%d%d",foo,
	    &qinfo->width, &qinfo->height, &qinfo->planes); 
	} else if (!strncasecmp(QclicLine,"NOLOSS",5)) {
	  qinfo->know_lossless =
	  qinfo->is_lossless = 1;
          /* copy source to lossless in the end */ 
	} else if (!strncasecmp(QclicLine,"ICURVE",3)) {
	  if (qinfo->icurve_points > 0) {
	    qinfo->icurve_points = 0;
	    FREE_AND_ZERO(qinfo->icurve_size);
	    FREE_AND_ZERO(qinfo->icurve_quality);
	  } 
	  ret = sscanf(QclicLine,"%s%d%s",foo,
	    &qinfo->icurve_points, qinfo->icurve_qmetric);
	  if ((ret < 2) || (qinfo->icurve_points <= 0)) {
             doing = QclicRead_Error;
	     break;
	  } 
	  npoints = qinfo->icurve_points;
	  where = 0; 
	  qinfo->icurve_size = (long *) malloc(sizeof(long)*npoints); 
	  qinfo->icurve_quality=(float *) malloc(sizeof(float)*npoints); 
	  if (!qinfo->icurve_size || !qinfo->icurve_quality) {
            FREE_AND_ZERO(qinfo->icurve_size); 
            FREE_AND_ZERO(qinfo->icurve_quality); 
	    qinfo->icurve_points = 0; 
	  }
	  if (ret < 3)
	    strcpy(qinfo->icurve_qmetric,"unknown"); 
	  doing = QclicRead_Icurve;
	} else if (!strncasecmp(QclicLine,"CURVE",3)) {
	  if (qinfo->curve_points > 0) {
	    FREE_AND_ZERO(qinfo->curve_size);
	    FREE_AND_ZERO(qinfo->curve_better_size);
	    FREE_AND_ZERO(qinfo->curve_quality);
	    for (i=0;i<qinfo->curve_points;i++)
	      FREE_AND_ZERO(qinfo->curve_parfiles[i]);
	    FREE_AND_ZERO(qinfo->curve_parfiles);
	    qinfo->curve_points = 0;
	  } 
	  strcpy(qinfo->curve_qmetric,"unknown"); 
	  strcpy(qinfo->curve_format,"unknown"); 
	  ret = sscanf(QclicLine,"%s%d%s%s",foo,
	    &qinfo->curve_points, qinfo->curve_format, qinfo->curve_qmetric); 
	  if ((ret < 2) || (qinfo->curve_points <= 0)) {
             doing = QclicRead_Error;
	     break;
	  } 
	  npoints = qinfo->curve_points;
	  where = 0; 
	  qinfo->curve_size = (long *) calloc(1,sizeof(long)*npoints); 
	  qinfo->curve_better_size = (long *) calloc(1,sizeof(long)*npoints); 
	  qinfo->curve_quality=(float *) calloc(1,sizeof(float)*npoints); 
	  qinfo->curve_parfiles = (char **) calloc(1,sizeof(char *)*npoints); 
	  if (!qinfo->curve_size || !qinfo->curve_quality ||
	      !qinfo->curve_better_size || !qinfo->curve_parfiles) {
            FREE_AND_ZERO(qinfo->curve_size); 
            FREE_AND_ZERO(qinfo->curve_better_size); 
            FREE_AND_ZERO(qinfo->curve_parfiles);
            FREE_AND_ZERO(qinfo->curve_quality); 
	    qinfo->curve_points = 0; 
	  }
	  doing = QclicRead_Curve;
	} else if (!strncasecmp(QclicLine,"BETTER_CURVE",3)) {
	  ret = sscanf(QclicLine,"%s%d",foo, &npoints);
	  if ((ret != 2) || (npoints <= 0)) {
	    doing = QclicRead_Error;
	    break;
	  }
	  where = 0;
	  doing = QclicRead_BetterCurve;
        } else {
	  doing = QclicRead_Error;
	}
	break;
      case QclicRead_Icurve:
	if (qinfo->icurve_points > 0)  { 
	  sscanf(QclicLine,"%ld%f",
	    &qinfo->icurve_size[where], 
	    &qinfo->icurve_quality[where]); 
        } 
	where++;
	if (where >= npoints) doing = QclicRead_Any;
	break;
      case QclicRead_Curve:
	if (qinfo->curve_points > 0)  { 
	  qfname[0] = '\0';
	  sscanf(QclicLine,"%ld%f%s",
	    &qinfo->curve_size[where], 
	    &qinfo->curve_quality[where], qfname); 
	  if ((qfname[0] != '\0') &&
	      ((qinfo->curve_parfiles[where] =
		(char *) malloc(QCLIC_STRLENMAX*sizeof(char))) != NULL)) {
	    strcpy(qinfo->curve_parfiles[where], qfname);
	  }
        } 
	where++;
	if (where >= npoints) doing = QclicRead_Any;
	break;
      case QclicRead_BetterCurve:
	if (qinfo->curve_points > 0)  { 
	  ltemp = 0;
	  if ((sscanf(QclicLine,"%d%ld", &i, &ltemp)==2) &&
	      (i >= 0) && (i < qinfo->curve_points)) {
	    qinfo->curve_better_size[i] = ltemp;  
	  }
        } 
	where++;
	if (where >= npoints) doing = QclicRead_Any;
	break;
      default:
	doing = QclicRead_Error;
	break;
    } 
  }

  qinfo->qclic_info_end_offset = DS_total_count(ds); 
  
  if (doing != QclicRead_Done) {
    QclicInfoFree(qinfo);
    return(0);
  }

  if (qinfo->is_lossless) strcpy(qinfo->lossless,qinfo->source);
  
  return(1);
}


extern int QclicInfoWrite(QclicInfo *qinfo, DataDest *dd) 
{
  int where, numbetter; 

  if (!qinfo->have_info) return(1);

  (void) DD_printf(dd,"QCLIC\n"); 

  if (strcmp(qinfo->name,QCLIC_UNKNOWN_SOURCE)) {
    (void) DD_printf(dd,"NAME %s\n",qinfo->name); 
  }
  
  (void) DD_printf(dd,"DIM %d %d %d\n", qinfo->width,
	    qinfo->height, qinfo->planes); 

  if (qinfo->is_lossless) { 
    (void) DD_printf(dd,"NOLOSS\n"); 
  } else if (qinfo->know_lossless) { 
    (void) DD_printf(dd,"LOSSLESS %s\n", qinfo->lossless); 
  }

  (void) DD_printf(dd,"SOURCE %s\n",qinfo->source); 

  (void) DD_printf(dd,"DIR %s\n",qinfo->dir); 

  (void) DD_printf(dd,"WEBDIR %s\n",qinfo->webdir); 

  if (qinfo->icurve_points > 0) {
    (void) DD_printf(dd,"ICURVE %d %s\n", qinfo->icurve_points,
	       qinfo->icurve_qmetric);
    for (where=0; where < qinfo->icurve_points; where++) { 
      (void) DD_printf(dd,"%ld %f\n", qinfo->icurve_size[where], 
                                      qinfo->icurve_quality[where]);
    } 
  }
  
  if (qinfo->curve_points > 0) {
    (void) DD_printf(dd,"CURVE %d %s %s\n", qinfo->curve_points,
	       qinfo->curve_format, qinfo->curve_qmetric);
    numbetter = 0; 
    for (where=0; where < qinfo->curve_points; where++) { 
      if (qinfo->curve_better_size[where] > 0) numbetter++;
      (void) DD_printf(dd,"%7ld %f %s\n",
	qinfo->curve_size[where], qinfo->curve_quality[where],
	qinfo->curve_parfiles[where] ? qinfo->curve_parfiles[where] : "");
    }
    if (numbetter > 0) {
      (void) DD_printf(dd,"BETTER_CURVE %d\n", numbetter);
      for (where=0; where < qinfo->curve_points; where++) { 
        if (qinfo->curve_better_size[where] > 0) {
	  (void) DD_printf(dd,"%2d  %7ld\n", where,
	   qinfo->curve_better_size[where]);
	}
      }
    }
  }

  (void) DD_printf(dd,"QCLIC-END\n"); 

  return(1);
}

static char *QmetricNames[QM_COUNT] = { 
   "SIZE", "PSIZE", 
   "PSNR","PSNR+","AVG-PSNR", "AVG-PSNR+", 
   "IPSNR","IPSNR+","AVG-IPSNR", "AVG-IPSNR+", 
   "ITP-PSNR","ITP-PSNR+","ITP-AVG-PSNR", "ITP-AVG-PSNR+", 
   "ITP-IPSNR","ITP-IPSNR+","ITP-AVG-IPSNR", "ITP-AVG-IPSNR+", 
   "BPP", "UNKNOWN" };

extern int QmetricKind(char *qm)
{
  int i;
  if (!qm) return QM_DEFAULT; 
  for (i=0;i<QM_COUNT;i++) {
    if (!strcasecmp(qm,QmetricNames[i])) return i; 
  }
  return QM_DEFAULT; 
}


extern char *Qmetric(int qm_kind)
{
  if ((qm_kind < 0) || (qm_kind >= QM_COUNT))
    return QmetricNames[QM_UNKNOWN];
  return QmetricNames[qm_kind]; 
}


