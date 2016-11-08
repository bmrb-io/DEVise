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
  fprintf(stderr,"Usage: xpnmmovie [-display display] [-visual vclass] [-title <title>] [-maxc <m>]\n");
  fprintf(stderr,"\t[-privmap] [-loc x y] [-ccube n] [-dither] [-numimages <n>] [-parwin <win_number>]\n");
  fprintf(stderr,"\t[-bwidth bw] [-bheight bh] [-tryXbacking] [imfile1 imfile2 ..]\n");
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
  int width, height, nplanes, old_width, old_height;
  int i;
  VR_ImKind kind, old_kind;
  VR_ImId iid;
  VR_WinId wid, gray_tid_retainer, rgb_tid_retainer, new_wid;
  VR_TransId rgb_tid, gray_tid, use_tid;
  VR_ReturnCode ans;
  int parent_given;
  Window the_parent;
  int wherex, wherey; 
  fd_set *foo =0;
  int ccube = 4;
  int bwidth, bheight; 
  int imnamesstart, imnum, num_images, num_ims_on_cmdline;
  int maxc; 
  int postcmd;
  int should_pause;
  int dodither;
  int vclass;

  long ptemp;

  Image TheImage;
  unsigned long priv_flag = 0;
  unsigned long ccc_flag = 0;

  char *dname; /* display */
  int filegiven;
  int imfilefd;
  int tryback = 0;
  
  char ImTitle[100]; 

  
  vclass = VR_VisDefault;
  imnamesstart = -1;
  dname = (char *) 0;
  parent_given = 0;
  ImTitle[0] = '\0'; 
  bwidth = -1;
  bheight = -1;
  wherex = wherey = 50;
  num_ims_on_cmdline = -1;
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
    else if (!strncmp(argv[i],"-privmap", 6)) {
      priv_flag = VR_CFL_TRY_DEFAULT;
    }
    else if (!strncmp(argv[i],"-help", 3)) {
      Usage();
    }
    else if (!strncmp(argv[i],"-dither", 4)) {
      dodither = 1;
    }
    else if (!strncmp(argv[i],"-tryXbacking", 4)) {
      tryback = 1;
    }
    else if (!strncmp(argv[i],"-ccube", 5)) {
      i++;
      if (i >= argc) Usage();
      ccube = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-maxc", 3)) {
      i++;
      if (i >= argc) Usage();
      maxc = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-visual", 4)) {
      i++;
      if (i >= argc) Usage();
      vclass = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-loc", 3)) {
      i++;
      if (i >= argc) Usage();
      wherex = atoi(argv[i]);
      i++;
      if (i >= argc) Usage();
      wherey = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-numimages", 3)) {
      i++;
      if (i >= argc) Usage();
      num_ims_on_cmdline = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-bwidth", 3)) {
      i++;
      if (i >= argc) Usage();
      bwidth = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-bheight", 3)) {
      i++;
      if (i >= argc) Usage();
      bheight = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-parwin", 7)) {
      i++;
      if (i >= argc) Usage();
      ptemp = strtol(argv[i],(char **) 0, 16);
      the_parent = (Window) ptemp;
      parent_given = 1;
    }
    else {
      /* assume to be beginning of file name list */
      imnamesstart = i;
      break;
    }
    i++;
  }
  
  num_images = argc - imnamesstart;
  if (num_images <= 0) Usage();

  ans = VR_InitViewerVisual(dname, (FILE *) 0, vclass);
  if (ans != VR_SUCCESS) {
    fprintf(stderr,"InitViewer said: %s\n", VR_ReturnCodeToString(ans));
    exit(1);
  } 
  
  ccc_flag = ccubeToflag(ccube);
  VR_CFL_SET_NCOLORS(ccc_flag, maxc);

  rgb_tid = -1;
  rgb_tid_retainer = -1;
  gray_tid = -1;
  gray_tid_retainer = -1;

  /* get first image */
  imnum = 0;
  if ((num_ims_on_cmdline != -1) && (imnum >= num_ims_on_cmdline)) {
    CallAbort("No images!");
  }
  InitImage(&TheImage);
  TheImage.Silent = 1;
  TheImage.ImKind = IM_PNM;
  if (imnamesstart != -1) {
    if (imnum >= num_images) CallAbort("No images!");
    if (!SetImageFile(&TheImage, argv[imnum+imnamesstart], 0))
      CallAbort("Error opening first image");
  } 

  PeekImage(&TheImage);
  if (TheImage.ImKind == IM_UNKNOWN) {
    CallAbort("No images or bad first image!");
  }

  if (TheImage.NumComponents == 1) {
      kind = VR_Gray;
      nplanes = 1;
  }
  else if (TheImage.NumComponents == 3) {
      kind = VR_RGB;
      nplanes = 3;
  }
  else CallAbort("Unknown image format");

  width = TheImage.NumCols;
  if (bwidth == -1) bwidth = width;
  height = TheImage.NumRows;
  if (bheight == -1) bheight = height;

  if (ImTitle[0] == '\0') strcpy(ImTitle, "Image"); 
  if (!parent_given) {
    the_parent = VR_GetXWindow(bwidth, bheight,
		     wherex, wherey, 0, 0, ImTitle);
    parent_given = 1;
  }

  ReadImg(&TheImage);

  iid = VR_GetNewImage(width, height, kind, TheImage.ImFileName);
  if (nplanes == 3) {
      VR_SetRGBImageData(iid, TheImage.Im[0], TheImage.Im[1], TheImage.Im[2]);
      use_tid = rgb_tid =
	 VR_GetTranslatorFromCMofXWin(iid, priv_flag | ccc_flag,
	  the_parent);
  } else {
      VR_SetImageData(iid, TheImage.Im[0]);
      use_tid = gray_tid =
	 VR_GetTranslatorFromCMofXWin(iid, priv_flag | ccc_flag,
	  the_parent);
  }


  wid = VR_GetWindow(width, height, (bwidth-width)/2, (bheight-height)/2, use_tid, 
	     parent_given, the_parent, 0, 1, &tryback);

  if (nplanes == 3) {
    rgb_tid_retainer = wid;
  } else {
    gray_tid_retainer = wid;
  }

  if (VR_ShowImage(iid, wid, dodither) != VR_SUCCESS)
	   CallAbort("Could not display");


  for (i=0; i<nplanes; i++) {
      FreeImgComp(&TheImage, i);
  }

  VR_FreeImage(iid); 
  
  if (tryback) VR_FreeWindowXImage(wid);

  VR_RegisterFd(0);


  /* show other images */
  for (imnum = 1; ; imnum++) {
    
    if ((num_ims_on_cmdline != -1) &&
	(imnum >= num_ims_on_cmdline)) break;

    old_kind = kind; 
    old_width = width;
    old_height = height;

    should_pause = 1;

  resume:

    InitImage(&TheImage);
    TheImage.Silent = 1;
    TheImage.ImKind = IM_PNM; 

    if (imnamesstart != -1) {
      if (imnum >= num_images) break;
      if (!SetImageFile(&TheImage, argv[imnum+imnamesstart], 0)) break;
    } 

    PeekImage(&TheImage);
    if (TheImage.ImKind == IM_UNKNOWN) {
      if (should_pause) {
        VR_WaitForSomething(&foo);
        should_pause = 0;
        goto resume;
      } else {
	break;
      }
    } 


    if (TheImage.NumComponents == 1) {
      kind = VR_Gray;
      nplanes = 1;
    }
    else if (TheImage.NumComponents == 3) {
      kind = VR_RGB;
      nplanes = 3;
    }
    else CallAbort("Unknown image format");


    width = TheImage.NumCols;
    height = TheImage.NumRows;

    ReadImg(&TheImage);

    iid = VR_GetNewImage(width, height, kind, TheImage.ImFileName);
    if (nplanes == 3) {
      VR_SetRGBImageData(iid, TheImage.Im[0], TheImage.Im[1], TheImage.Im[2]);
    } else {
      VR_SetImageData(iid, TheImage.Im[0]);
    }

    if (kind != old_kind) {
      if (kind == VR_Gray)  {
	if (gray_tid == -1) {
	  /* need to get gray translator */
          gray_tid =
	    VR_GetTranslatorFromCMofXWin(iid, priv_flag | ccc_flag,
	    the_parent);
	}
	use_tid = gray_tid;
      } else {
	if (rgb_tid == -1) {
	  /* need to get RGB translator */
          rgb_tid =
	    VR_GetTranslatorFromCMofXWin(iid, priv_flag | ccc_flag,
	    the_parent);
	}
	use_tid = rgb_tid;
      }
    }

    VR_FreeWindowXImage(wid);

    if ((kind != old_kind) ||
	(width != old_width) || (height != old_height)) {
      /* need to get new window */
      new_wid = VR_GetWindow(width, height, (bwidth-width)/2,
	  (bheight-height)/2, use_tid, 
	     parent_given, the_parent, 0, 1, &tryback);
      if ((wid != gray_tid_retainer) &&
	  (wid != rgb_tid_retainer)) {
	VR_FreeWindow(wid);
      }
      wid = new_wid;
      if (kind != old_kind) {
	if (kind == VR_Gray) {
	  if (gray_tid_retainer == -1) 
	    gray_tid_retainer = wid;
	} else {
	  if (rgb_tid_retainer == -1) 
	    rgb_tid_retainer = wid;
	}
      }
    } 



    if (VR_ShowImage(iid, wid, dodither) != VR_SUCCESS)
	   CallAbort("Could not display");


    for (i=0; i<nplanes; i++) {
      FreeImgComp(&TheImage, i);
    }

    VR_FreeImage(iid); 

    if (tryback) VR_FreeWindowXImage(wid);

  }


  if (use_tid == rgb_tid) {
    if (gray_tid >= 0) VR_FreeTranslator(gray_tid);
    if (rgb_tid >= 0) VR_FreeTransTable(rgb_tid);
  } else {
    if (rgb_tid >= 0) VR_FreeTranslator(rgb_tid);
    if (gray_tid >= 0) VR_FreeTransTable(gray_tid);
  }

  /* exit on getting stuff on stdin */

  while (VR_WaitForSomething(&foo) == VR_SUCCESS) {
    if (!foo) break;
    foo = 0;
    postcmd = getc(stdin);
    if ((postcmd=='q') || (postcmd=='Q')) break;
  }
  VR_CloseViewer();

  return(0);
}



    
  
