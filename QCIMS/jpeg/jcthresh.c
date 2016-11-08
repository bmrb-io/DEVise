
#include "cdjpeg.h"


#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "jcthresh.h"

/************************************
  Read the next line from fd into s[]
  Replace '\n' with '\0'
  Read at most limit characters
  Return number of characters read or EOF
*************************************/

LOCAL int
newline(char s[], int fd, int limit)
{
  int i;
  char c;
  i = 0;
  while (read(fd,&c,1) == 1) {
    if (c == '\n') {
      s[i] = '\0';
      return (i+1);
    }
    s[i] = c;
    i++;
    if (i == (limit-1)) {
      s[i] = '\0';
      return (limit);
    }
  }
  if (i > 0) {
    s[i] = '\0';
    return (i+1);
  } else {
    return EOF;
  }
}

/*************************************************************
  Read one table of 64 thresholds from fd into tab
  The lines in the file that contain the threshold table
  begin with "#T "
  Since DCT calculations will do scaling, apply the
  same scaling (depending on dct_method) to the thresholds.
  Return TRUE iff whole table could be read
************************************************************/

LOCAL boolean 
read_float_table(int fd, float *tab, J_DCT_METHOD dct_method)
{
  int i,j,n;
  char nextline[100], *nextplace; 
  float ftemp;
  static const float aanscalefactor[8] = {
            1.0, 1.387039845, 1.306562965, 1.175875602,
            1.0, 0.785694958, 0.541196100, 0.275899379
          };


  n = 0;

  while ((n < 64) && (newline(nextline,fd, 100) != EOF)) {
    if ((nextline[0] != '#') ||
	(nextline[1] != 'T') ||
	(nextline[2] != ' ')) continue;
    nextplace = nextline + 3;
    while ((nextplace) && (n < 64)) {
      while ((*nextplace == ' ') || (*nextplace == '\t')) nextplace++;
      if (sscanf(nextplace,"%f",&ftemp) != 1) break;
      tab[n++] = ftemp;
      nextplace = strpbrk(nextplace," \t");
    }
  }
  
  if (n != 64) return (FALSE);

  switch (dct_method) {
#ifdef DCT_ISLOW_SUPPORTED
  case JDCT_ISLOW:
    n=0;
    for (i=0;i<8;i++) {
      for (j=0;j<8;j++) {
        tab[n++] *= 8.0;
      }
    }
    break;
#endif
#ifdef DCT_IFAST_SUPPORTED
  case JDCT_IFAST:
    n=0;
    for (i=0;i<8;i++) {
      for (j=0;j<8;j++) {
        tab[n++] *= (aanscalefactor[i]*aanscalefactor[j]*8.0);
      }
    }
    break;
#endif
#ifdef DCT_FLOAT_SUPPORTED
  case JDCT_FLOAT:
    n=0;
    for (i=0;i<8;i++) {
      for (j=0;j<8;j++) {
        tab[n++] *= (aanscalefactor[i]*aanscalefactor[j]*8.0);
      }
    }
    break;
#endif
  default:
    return (FALSE);
  }


  return (TRUE); 
}


boolean jc_do_thresh = FALSE;
float jc_thresh_table[NUM_QUANT_TBLS][DCTSIZE2];
int jc_num_thresh_tables = 0;

/**************************************************************
  Read all thresholding tables from filr named threshfile
***************************************************************/

GLOBAL void
read_thresh_tables (char *threshfile, J_DCT_METHOD dct_method)
{
  int fd;

  if ((fd = open(threshfile, O_RDONLY)) < 0) return;

  while (read_float_table(fd, jc_thresh_table[jc_num_thresh_tables], dct_method)) {
    jc_num_thresh_tables++;
    if (jc_num_thresh_tables >= NUM_QUANT_TBLS) break;
  }
  close(fd);
}

/**********************************************************
  Apply thresholding to scaled DCT coefs in workspace
***********************************************************/

GLOBAL void
apply_thresholding (DCTELEM *workspace, int comp_num)
{
  int i, tnum;
  float val, temp;

  if (jc_num_thresh_tables <= 0) {jc_do_thresh = FALSE; return; }

  if ((tnum = comp_num) >= jc_num_thresh_tables)
    tnum = jc_num_thresh_tables - 1;


  for (i=0;i<64;i++) {
    val = (float) workspace[i];
    if ((temp = ((float) 0.0) - val) > ((float) 0.0)) val = temp;
    if (val < jc_thresh_table[tnum][i]) workspace[i] = (DCTELEM) 0;
  }



}


#ifdef DCT_FLOAT_SUPPORTED

/**********************************************************
  Apply thresholding to scaled float DCT coefs in workspace
***********************************************************/

GLOBAL void
apply_float_thresholding (FAST_FLOAT *workspace, int comp_num)
{
  int i, tnum;
  float val, temp;

  if (jc_num_thresh_tables <= 0) {jc_do_thresh = FALSE; return; }

  if ((tnum = comp_num) >= jc_num_thresh_tables)
    tnum = jc_num_thresh_tables - 1;
  

  for (i=0;i<64;i++) {
    val = (float) workspace[i];
    if ((temp = ((float) 0.0) - val) > ((float) 0.0)) val = temp;
    if (val < jc_thresh_table[tnum][i]) workspace[i] = (FAST_FLOAT) 0;
  }




}

#endif

