#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>




#include "VR.h"
#include "Image.h"


void CallAbort(char *msg)
{
  fprintf(stderr,"%s\n",msg);
  exit(1);
}

void Usage(void)
{
  fprintf(stderr,"Usage: xpnm [-display display] [-loc x y] [-title <title>]\n\t[-privmap] [-ack] [-ccube n] [-maxc m] [-parwin <win_number>] [-dither] [-win <win_number>] [-tryXback] [-vis vclass] [imfile]\n");
  exit(1);
}

static unsigned long ccubeToflag(int c)
{
  if (c < 2) return(VR_CCC_2x2x2);
  else if (c == 2) return(VR_CCC_3x3x2);
  else if (c == 3) return(VR_CCC_3x3x3);
  else if (c == 4) return(VR_CCC_4x4x4);
  else if (c == 5) return(VR_CCC_5x5x5);
  else return(VR_CCC_6x5x5);
}

int
main(int argc, char *argv[])
{
  int width, height, nplanes;
  int i,  loc, nlines;
  VR_ImKind kind;
  VR_ImId iid;
  VR_WinId wid;
  VR_TransId tid;
  VR_ReturnCode ans;
  int parent_given;
  Window the_parent;
  int win_given;
  Window the_win;
  fd_set *foo;
  int ccube = 4;
  int ack_needed;
  int tryback; 
  int maxc, dodither;
  int vclass;
  int offs[3];
  unsigned char *c;
  unsigned short cx_short;
  unsigned long cx_long;

  long ptemp;

  Image TheImage;
  unsigned long priv_flag = 0;
  unsigned long ccc_flag = 0;

  char *dname; /* display */
  int wherex, wherey;
  int filegiven;
  int imfilefd;
  
  char ImTitle[100]; 

  
  vclass = VR_VisDefault;
  ack_needed = 0;
  dname = (char *) 0;
  wherex = 0;
  wherey = 0;
  filegiven = 0;
  parent_given = 0;
  win_given = 0;
  ImTitle[0] = '\0'; 
  tryback = 0; 
  maxc = 256;
  dodither = 0;

  /* parse switches */
  i = 1;
  while (i < argc) {
    if (!strncmp(argv[i],"-display", 4)) {
      i++;
      if (i >= argc) Usage();
      dname = argv[i];
    }
    else if (!strncmp(argv[i],"-title", 6)) {
      i++;
      if (i >= argc) Usage();
      strcpy(ImTitle, argv[i]); 
    }
    else if (!strncmp(argv[i],"-loc", 4)) {
      i++;
      if (i >= argc) Usage();
      wherex = atoi(argv[i]);
      i++;
      if (i >= argc) Usage();
      wherey = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-privmap", 6)) {
      priv_flag = VR_CFL_TRY_DEFAULT;
    }
    else if (!strncmp(argv[i],"-dither", 4)) {
      dodither = 1;
    }
    else if (!strncmp(argv[i],"-win", 4)) {
      i++;
      if (i >= argc) Usage();
      ptemp = strtol(argv[i],(char **) 0, 16);
      the_win = (Window) ptemp;
      win_given = 1;
    }
    else if (!strncmp(argv[i],"-oldcmap", 5)) {
      /* no longer used */ 
    }
    else if (!strncmp(argv[i],"-ack", 4)) {
      ack_needed = 1;
    }
    else if (!strncmp(argv[i],"-tryXback", 5)) {
      tryback = 1;
    }
    else if (!strncmp(argv[i],"-ccube", 5)) {
      i++;
      if (i >= argc) Usage();
      ccube = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-visual", 4)) {
      i++;
      if (i >= argc) Usage();
      vclass = atoi(argv[i]);
      if (vclass < 0) vclass = -1;
      if (vclass > 5) vclass = 5;
    }
    else if (!strncmp(argv[i],"-maxc", 3)) {
      i++;
      if (i >= argc) Usage();
      maxc = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-parwin", 7)) {
      i++;
      if (i >= argc) Usage();
      ptemp = strtol(argv[i],(char **) 0, 16);
      the_parent = (Window) ptemp;
      parent_given = 1;
    }
    else {
      /* could be file name */
      if (i == (argc-1)) filegiven = 1;
      else Usage();
    }
    i++;
  }
  
  ans = VR_InitViewerVisual(dname, (FILE *) 0, vclass);
  if (ans != VR_SUCCESS) {
    fprintf(stderr,"InitViewer said: %s\n", VR_ReturnCodeToString(ans));
    exit(1);
  } 

  ccc_flag = ccubeToflag(ccube);
  VR_CFL_SET_NCOLORS(ccc_flag, maxc);

  InitImage(&TheImage);
  TheImage.Silent = 1;

  if (filegiven) {
    if (!SetImageFile(&TheImage, argv[argc-1], 0)) exit(1);
    if (ImTitle[0] == '\0') strcpy(ImTitle, argv[argc-1]); 
  }

  if (ImTitle[0] == '\0') strcpy(ImTitle, "Image"); 

  PeekImage(&TheImage);
  if (TheImage.ImKind == IM_UNKNOWN) exit(1);

  if (TheImage.NumComponents == 1) {
    kind = VR_Gray;
    nplanes = 1;
  }
  else if (TheImage.NumComponents == 3) {
    kind = VR_RGB;
    nplanes = 3;
  }
  else CallAbort("Unknown image format");

  if (ack_needed) {
    fprintf(stdout,"OK\n");
    fflush(stdout);
  }

  width = TheImage.NumCols;
  height = TheImage.NumRows;

  ReadImg(&TheImage);

  iid = VR_GetNewImage(width, height, kind, ImTitle);

  if (TheImage.SampleBytes[0] == 1) {
    if (nplanes == 3) {
      VR_SetRGBImageData(iid, TheImage.Im[0], TheImage.Im[1], TheImage.Im[2]);
    } else {
      VR_SetImageData(iid, TheImage.Im[0]);
    }
  } else {
    for (i=0; i<nplanes; i++) {
      offs[i] = 0;
      if (TheImage.SampleBytes[i]==2) {
	cx_short = 0x0100;
	c = (unsigned char *) &cx_short;
	if (!(*c)) offs[i] = 1;
      } else if (TheImage.SampleBytes[i]==4) {
	cx_long = 0x01000000;
	c = (unsigned char *) &cx_long;
	while (!(*c++)) offs[i]++;
      }
    }
    if (nplanes == 3) {
      VR_SetNon8RGBImageData(iid, TheImage.Im[0] + offs[0],
	TheImage.Im[1] + offs[1], TheImage.Im[2] + offs[2],
        TheImage.SampleBytes[0], TheImage.SampleBytes[1], TheImage.SampleBytes[2]);
    } else {
      VR_SetNon8ImageData(iid, TheImage.Im[0] + offs[0],
        TheImage.SampleBytes[0], TheImage.SampleBytes[1], TheImage.SampleBytes[2]);
    }
  }

  if (!win_given) {
    if (parent_given) {
	    tid = VR_GetTranslatorFromCMofXWin(iid,
	       priv_flag | ccc_flag, the_parent);
    }
    else {
            tid = VR_GetTranslator(iid, priv_flag | ccc_flag);
    }

    wid = VR_GetWindow(width, height, wherex, wherey, tid, 
	     parent_given, the_parent, 0, 0, &tryback);
  } else {
    tid = VR_GetTranslatorFromCMofXWin(iid,
       priv_flag | ccc_flag, the_win);
    wid = VR_SetWindow(width, height, tid, the_win, 0, 0, &tryback);
  }

  if (VR_ShowImage(iid, wid, dodither) != VR_SUCCESS)
	   CallAbort("Could not display");


  FreeImg(&TheImage);
  
  VR_FreeTransTable(tid);

  if (tryback) {
    VR_FreeWindowXImage(wid);
  }

  VR_WaitForSomething(&foo);
  VR_CloseViewer();

  return(0);
}



    
  
