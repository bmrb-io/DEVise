#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "Image.h"
#include "ImageInternal.h" 

#define PS_GOOD_PWID 612
#define PS_GOOD_PHGT 762

static int PS_PutProlog(DataDest *dd, char *title,
  int imcols, int imrows, int planes,
  int pgwid, int pghgt, int do_center)
{
  int llx, lly, urx, ury;

  if (do_center) {
    llx = (pgwid - imcols) / 2; 
    lly = (pghgt - imrows) / 2; 
  } else {
    llx = lly = 0;
  }
  urx = llx + imcols - 1; 
  ury = lly + imrows - 1; 

  if (!DD_printf(dd,"%%!PS-Adobe-2.0 EPSF-2.0\n")) return 0; 
  if (!DD_printf(dd,"%%%%Creator: libImg\n")) return 0; 
  if (!DD_printf(dd,"%%%%Title: %s\n",title)) return 0; 
  if (!DD_printf(dd,"%%%%Pages: 1\n")) return 0; 
  if (!DD_printf(dd,"%%%%BoundingBox: %d %d %d %d\n",
    llx, lly, urx, ury)) return 0; 


  if (!DD_printf(dd, "%%%%EndComments\n")) return 0; 
  if (!DD_printf(dd, "/readstring {\n")) return 0; 
  if (!DD_printf(dd, "  currentfile exch readhexstring pop\n")) 
    return 0; 
  if (!DD_printf(dd, "} bind def\n")) return 0; 

  if (planes == 3 ) { 
    if (!DD_printf(dd, "/rstr %d string def\n", imcols)) return 0; 
    if (!DD_printf(dd, "/gstr %d string def\n", imcols)) return 0;
    if (!DD_printf(dd, "/bstr %d string def\n", imcols)) return 0;
  } else { 
    if (!DD_printf(dd, "/grstr %d string def\n", imcols)) return 0;
  } 

  if (!DD_printf(dd, "%%%%EndProlog\n")) return 0; 
  if (!DD_printf(dd, "%%%%Page: 1 1\n")) return 0; 
  if (!DD_printf(dd, "gsave\n")) return 0; 
  if (!DD_printf(dd, "%d %d translate\n", llx, lly)) return 0; 
  if (!DD_printf(dd, "%d %d scale\n", imcols, imrows)) return 0; 

  if (!DD_printf(dd, "%d %d 8\n", imcols, imrows)) return 0; 
  if (!DD_printf(dd, "[ %d 0 0 -%d 0 %d ]\n", imcols, imrows, imrows))
    return 0; 
  if (planes==3) {
    if (!DD_printf(dd,  "{ rstr readstring }\n")) return 0; 
    if (!DD_printf(dd,  "{ gstr readstring }\n")) return 0; 
    if (!DD_printf(dd,  "{ bstr readstring }\n")) return 0; 
    if (!DD_printf(dd,  "true 3\n")) return 0; 
    if (!DD_printf(dd,  "colorimage\n")) return 0; 
  } else {
    if (!DD_printf(dd,  "{ grstr readstring }\n")) return 0; 
    if (!DD_printf(dd,  "image\n")) return 0; 
  } 
  return 1;  
}



extern long ImPSSendImg(Image *Im, DataDest *dd, 
  int do_center, int pgwid, int pghgt)
{
  const char *hexchars = "0123456789abcdef";
  int row, col; 
  int scl_pix, ct, rct, gct, bct; 
  long start_bytes; 

  if (Im->ImState < ImStateReadSome)
    ImNonFatalError("ImPSSendImg: image has not been read");
  else if (Im->ImState >= ImStateDead)
    ImNonFatalError("ImPSSendImg: image is already dead");

  if (!Im->AllPlanesEqual) 
    ImNonFatalError("ImPSSendImg: unequal planes sizes"); 
  if ((Im->NumPlanes != 1) && (Im->NumPlanes != 3)) 
    ImNonFatalError("ImPSSendImg: 1 or 3 planes only allowed");
   
  if (do_center < 0) do_center = 1;
  if (pgwid <= 0) pgwid = PS_GOOD_PWID;
  if (pghgt <= 0) pghgt = PS_GOOD_PHGT;
  
  start_bytes = DD_total_count(dd); 

  if (!PS_PutProlog(dd, Im->ImName, Im->CurrCols[0], Im->CurrRows[0],
     Im->NumPlanes, pgwid, pghgt, do_center)) {
    ImNonFatalError("ImPSSendImg: could not write PostScript header");
  }

  if (Im->NumPlanes==1) {
    if (Im->SampleBytes[0]==1) { 
      ct = 0; 
      for (row=0; row < Im->CurrRows[0]; row++) { 
        for (col=0; col < Im->CurrCols[0]; col++,ct++) { 
	  if (!DD_printf(dd,"%c%c",
		   hexchars[Im->Im[0][ct] >> 4], 
		   hexchars[Im->Im[0][ct] & 15])) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  if ((ct%30)==29) {
	    if (!DD_printf(dd,"\n")) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  } 
	} 
      } 
    } else if (Im->SampleBytes[0]==2) { 
      ct = 0; 
      for (row=0; row < Im->CurrRows[0]; row++) { 
        for (col=0; col < Im->CurrCols[0]; col++,ct++) { 
	  scl_pix = (int) ((float) ((float) (Im->Im2[0][ct]*255)) / 
				   ((float) Im->MaxSample[0])); 
          if (scl_pix < 0) scl_pix = 0; 
          if (scl_pix > 255) scl_pix = 255; 
	  if (!DD_printf(dd,"%c%c",
		   hexchars[scl_pix >> 4], 
		   hexchars[scl_pix & 15])) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  if ((ct%30)==29) {
	    if (!DD_printf(dd,"\n")) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  } 
	} 
      } 
    } else { 
      ct = 0; 
      for (row=0; row < Im->CurrRows[0]; row++) { 
        for (col=0; col < Im->CurrCols[0]; col++,ct++) { 
	  scl_pix = (int) ((float) ((float) (Im->Im4[0][ct]*255)) / 
				   ((float) Im->MaxSample[0])); 
          if (scl_pix < 0) scl_pix = 0; 
          if (scl_pix > 255) scl_pix = 255; 
	  if (!DD_printf(dd,"%c%c",
		   hexchars[scl_pix >> 4], 
		   hexchars[scl_pix & 15])) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  if ((ct%30)==29) {
	    if (!DD_printf(dd,"\n")) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  } 
	} 
      } 
    } 
  } else {
    /* RGB image */   
    rct = gct = bct = ct = 0; 
    for (row=0; row < Im->CurrRows[0]; row++) { 
      /********* red row *********/ 
      if (Im->SampleBytes[0]==1) { 
        for (col=0; col < Im->CurrCols[0]; col++,ct++,rct++) { 
	  if (!DD_printf(dd,"%c%c",
		   hexchars[Im->Im[0][rct] >> 4], 
		   hexchars[Im->Im[0][rct] & 15])) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  if ((ct%30)==29) {
	    if (!DD_printf(dd,"\n")) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  } 
	} 
      } else if (Im->SampleBytes[0]==2) { 
        for (col=0; col < Im->CurrCols[0]; col++,ct++,rct++) { 
	  scl_pix = (int) ((float) ((float) (Im->Im2[0][rct]*255)) / 
				   ((float) Im->MaxSample[0])); 
          if (scl_pix < 0) scl_pix = 0; 
          if (scl_pix > 255) scl_pix = 255; 
	  if (!DD_printf(dd,"%c%c",
		   hexchars[scl_pix >> 4], 
		   hexchars[scl_pix & 15])) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  if ((ct%30)==29) {
	    if (!DD_printf(dd,"\n")) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  } 
	} 
      } else { 
        for (col=0; col < Im->CurrCols[0]; col++,ct++,rct++) { 
	  scl_pix = (int) ((float) ((float) (Im->Im4[0][rct]*255)) / 
				   ((float) Im->MaxSample[0])); 
          if (scl_pix < 0) scl_pix = 0; 
          if (scl_pix > 255) scl_pix = 255; 
	  if (!DD_printf(dd,"%c%c",
		   hexchars[scl_pix >> 4], 
		   hexchars[scl_pix & 15])) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  if ((ct%30)==29) {
	    if (!DD_printf(dd,"\n")) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  } 
	} 
      } 
      /********* green row *********/ 
      if (Im->SampleBytes[1]==1) { 
        for (col=0; col < Im->CurrCols[0]; col++,ct++,gct++) { 
	  if (!DD_printf(dd,"%c%c",
		   hexchars[Im->Im[1][gct] >> 4], 
		   hexchars[Im->Im[1][gct] & 15])) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  if ((ct%30)==29) {
	    if (!DD_printf(dd,"\n")) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  } 
	} 
      } else if (Im->SampleBytes[1]==2) { 
        for (col=0; col < Im->CurrCols[0]; col++,ct++,gct++) { 
	  scl_pix = (int) ((float) ((float) (Im->Im2[1][gct]*255)) / 
				   ((float) Im->MaxSample[1])); 
          if (scl_pix < 0) scl_pix = 0; 
          if (scl_pix > 255) scl_pix = 255; 
	  if (!DD_printf(dd,"%c%c",
		   hexchars[scl_pix >> 4], 
		   hexchars[scl_pix & 15])) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  if ((ct%30)==29) {
	    if (!DD_printf(dd,"\n")) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  } 
	} 
      } else { 
        for (col=0; col < Im->CurrCols[0]; col++,ct++,gct++) { 
	  scl_pix = (int) ((float) ((float) (Im->Im4[1][gct]*255)) / 
				   ((float) Im->MaxSample[1])); 
          if (scl_pix < 0) scl_pix = 0; 
          if (scl_pix > 255) scl_pix = 255; 
	  if (!DD_printf(dd,"%c%c",
		   hexchars[scl_pix >> 4], 
		   hexchars[scl_pix & 15])) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  if ((ct%30)==29) {
	    if (!DD_printf(dd,"\n")) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  } 
	} 
      } 
      /********* blue row *********/ 
      if (Im->SampleBytes[2]==1) { 
        for (col=0; col < Im->CurrCols[0]; col++,ct++,bct++) { 
	  if (!DD_printf(dd,"%c%c",
		   hexchars[Im->Im[2][bct] >> 4], 
		   hexchars[Im->Im[2][bct] & 15])) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  if ((ct%30)==29) {
	    if (!DD_printf(dd,"\n")) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  } 
	} 
      } else if (Im->SampleBytes[2]==2) { 
        for (col=0; col < Im->CurrCols[0]; col++,ct++,bct++) { 
	  scl_pix = (int) ((float) ((float) (Im->Im2[2][bct]*255)) / 
				   ((float) Im->MaxSample[2])); 
          if (scl_pix < 0) scl_pix = 0; 
          if (scl_pix > 255) scl_pix = 255; 
	  if (!DD_printf(dd,"%c%c",
		   hexchars[scl_pix >> 4], 
		   hexchars[scl_pix & 15])) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  if ((ct%30)==29) {
	    if (!DD_printf(dd,"\n")) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  } 
	} 
      } else { 
        for (col=0; col < Im->CurrCols[0]; col++,ct++,bct++) { 
	  scl_pix = (int) ((float) ((float) (Im->Im4[2][bct]*255)) / 
				   ((float) Im->MaxSample[2])); 
          if (scl_pix < 0) scl_pix = 0; 
          if (scl_pix > 255) scl_pix = 255; 
	  if (!DD_printf(dd,"%c%c",
		   hexchars[scl_pix >> 4], 
		   hexchars[scl_pix & 15])) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  if ((ct%30)==29) {
	    if (!DD_printf(dd,"\n")) 
	      ImNonFatalError("ImPSSendImg: error while writing"); 
	  } 
	} 
      } 
    } 
  } 


  if (!DD_printf(dd, "\ngrestore\nshowpage\n%%%%Trailer\n"))
      ImNonFatalError("ImPSSendImg: error while writing"); 

  return (DD_total_count(dd) - start_bytes); 
}





extern long ImPSWriteImg(Image *Im, int fd, 
  int do_center, int pgwid, int pghgt)   
{
  DataDest dd;
  long ans; 
  DD_Init(&dd);
  if (!DD_OpenFd(&dd, fd))
    ImNonFatalError("ImPSWriteImg: bad fd given");
  ans = ImPSSendImg(Im, &dd, do_center, pgwid, pghgt); 
  DD_Close(&dd);
  return ans;
}

extern long ImPSStreamWriteImg(Image *Im, FILE *fp, 
  int do_center, int pgwid, int pghgt)   
{
  DataDest dd;
  long ans; 
  DD_Init(&dd);
  if (!DD_OpenStream(&dd, fp))
    ImNonFatalError("ImPSStreamWriteImg: bad fp given");
  ans = ImPSSendImg(Im, &dd, do_center, pgwid, pghgt); 
  DD_Close(&dd);
  return ans;
}

extern long ImPSSaveImg(Image *Im, char *fname, 
  int do_center, int pgwid, int pghgt)   
{
  DataDest dd;
  long ans; 
  DD_Init(&dd);
  if (!DD_OpenFile(&dd, fname))
    ImNonFatalError("ImPSSaveImg: bad file given");
  ans = ImPSSendImg(Im, &dd, do_center, pgwid, pghgt); 
  DD_Close(&dd);
  return ans;
}

extern long ImPSBufferImg(Image *Im, char *buff, long buffsize,
  char **extramem, int do_center, int pgwid, int pghgt)   
{
  DataDest dd;
  long ans; 

  DD_Init(&dd);
  if (!DD_OpenMemory(&dd, buff, buffsize))
    ImNonFatalError("ImPSBufferImg: bad memory given");
  if (extramem) { 
    *extramem = (char *) 0; 
    DD_Use_Auto_Mem(&dd); 
    DD_Set_Auto_Mem_Step(&dd, (Im->CurrCols[0] * 6)); 
  } 
  ans = ImPSSendImg(Im, &dd, do_center, pgwid, pghgt); 
  if (ans && extramem && DD_using_auto_mem(&dd)) { 
    *extramem = DD_auto_mem(&dd); 
  } 
  DD_Close(&dd);
  return ans;
}




