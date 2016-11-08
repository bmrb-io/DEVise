#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

#include "VR.h"
#define NEED_IM_KIND_NAMES
#include "Image.h"
#include "Tasvir.h"


#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

/** forward declarations **/
static int CloseControl(int handle);
static int InvokeControl(int handle);
static void RefreshControl(int handle);
static int HandleResizing(VR_WinId wid, XEvent *evt);
static int HandleControlToggle(VR_WinId wid, XEvent *event);
static int HandleQuit(VR_WinId wid, XEvent *event);
static unsigned long ccubeToflag(int c);


static FILE * LogFile;
extern int errno;
static int VR_return;
static int IM_return;
static int tas_port = TASVIR_PORT; 

#define TASVIR_MAX_ARGS 300
static char command[TASVIR_MAX_STR_LENGTH]; 
static int CmdArgc;
static char *CmdArgv[TASVIR_MAX_ARGS];

/********************************************************
   read a \n-terminated line from fd into s, replacing
   \n by \0. do not read more than maxc-1 characters
   return the number of characters read (excluding \n). 
*********************************************************/
static int
newlinefd(char *s, int fd, int maxc)
{
  int i, lim = maxc-1; 
  char c;
  i = 0;
  while ((i < lim) && (read(fd,&c,1) == 1)) {
    if (c == '\n') {
      s[i] = '\0';
      return (i);
    }
    s[i++] = c;
  }
  s[i] = '\0';
  return(-1); 
}


/******************************************************
  Read num bytes from fd into buff. Return
  # bytes actually read, which may be less only when
  EOF is prematurely reached.
*******************************************************/ 
static int
MyRead(int fd, unsigned char *buff, int num)
{
  unsigned char *temp;
  int tocome = num;
  int thistime;
  
  temp = buff;

  while (tocome > 0) {
    thistime = read(fd,temp,tocome);
    if (thistime <= 0) return(num-tocome);
    tocome -= thistime;
    temp = temp + thistime;
  }

  return(num);
}


/******************************************************
  Write num bytes into fd from buff. Return
  # bytes actually written, which may be less only when
  write(fd,..,..) returns <=0 at some point. 
*********************************************************/ 
static int MyWrite(int fd, unsigned char *buff, int num)
{
  unsigned char *temp;
  int tocome = num;
  int thistime;
  
  temp = buff;

  while (tocome > 0) {
    thistime = write(fd,temp,tocome);
    if (thistime <= 0) return(num-tocome);
    tocome -= thistime;
    temp = temp + thistime;
  }

  return(num);
}

#define TASVIR_MAX_IMAGES 1000

static int NumImages;
static int LastHandle = 0;
static int replySent; 
static Window root_win; 

struct ImageManagerStruct { 
  int InUse;
  char Name[IM_STRLENMAX];
  VR_WinId wid;
  boolean no_subwin; /* is the image being shown in the same
			window as was given in the show command?
			0 implies a subwindow was created */ 
  VR_TransId tid;
  VR_ImId iid; 
  boolean ImExists; 
  Image  * Im; 
  int owidth;
  int oheight;
  int width;
  int height;
  int nplanes; 
  int x; /* if no_subwin then x and y give the location of the
	    top-left corner of the image in the X window;
	    else x = y = 0
	 */ 
  int y; 
  boolean do_smooth;
  boolean do_dither;
  boolean maintain_ar;
  boolean aspected;
  boolean handle_resize;
  boolean am_refreshing;
  int ccube;
  int maxc;
  boolean private_cmap; 

  char format[IM_STRLENMAX];
  char filter[IM_STRLENMAX];


  boolean CtrlExists;
  int CtrlPid;
  Window CtrlWin;
  int PipeToCtrl[2];
  int PipeFromCtrl[2];

} Images[TASVIR_MAX_IMAGES];

static int NewHandle(void)
{
  if (NumImages  == TASVIR_MAX_IMAGES) return(-1);
  while (Images[LastHandle].InUse) {
    LastHandle++;
    if (LastHandle >= TASVIR_MAX_IMAGES) LastHandle = 0;
  }

  return(LastHandle);
} 

static void CopyImgToIid(Image *Im, VR_ImId iid)
{
  int sbytes[3], offs[3];
  int ntocopy, i, allbytes1;

  if (ImPlanesOfImg(Im) < 3) ntocopy = 1;
  else ntocopy = 3;

  allbytes1 = 1;
  for (i=0;i<ntocopy;i++) {
    sbytes[i] = ImPlaneSampleBytes(Im, i);
    if (sbytes[i] != 1) allbytes1 = 0;
    if (ImPlaneHostEndian(Im,i) == Endian_LSBFirst) {
      offs[i] = sbytes[i] - 1;
    } else {
      offs[i] = 0;
    }
  }


  if (allbytes1) {
    if (ntocopy == 3) {
      VR_return =
      VR_SetRGBImageData(iid, ImPlane(Im,0), ImPlane(Im,1), ImPlane(Im,2));
    } else {
      VR_return =
      VR_SetImageData(iid, ImPlane(Im,0));
    }
  } else {
    if (ntocopy == 3) {
      VR_return =
      VR_SetNon8RGBImageData(iid, ImPlane(Im,0) + offs[0],
        ImPlane(Im,1) + offs[1], ImPlane(Im,2) + offs[2],
	sbytes[0], sbytes[1], sbytes[2]);
    } else {
      VR_return =
      VR_SetNon8ImageData(iid, ImPlane(Im,0) + offs[0],
        sbytes[0], sbytes[0], sbytes[0]);
    }
  }

}

static int FindTasvirImageFromWid(VR_WinId wid)
{
  int i,j;
  j=0;
  for (i=0;i<NumImages;i++) {
    while (!Images[j].InUse) j++;
    if (Images[j].wid == wid) return(j);
    j++;
  }
  return(-1);
}

static int HandleResizing(VR_WinId wid, XEvent *evt)
{
  int new_wd = evt->xconfigure.width, new_ht = evt->xconfigure.height;
  int handle, iid;
  XWindowChanges wchan;
  
 

  if (!VR_WidValid(wid)) return(0);


  if ((handle = FindTasvirImageFromWid(wid)) < 0) return(0);

  if ((VR_Windows[wid].width == new_wd)
     && (VR_Windows[wid].height == new_ht) &&
	(!Images[handle].maintain_ar || Images[handle].aspected)) 
    return(1);

  if (!Images[handle].handle_resize) return(0);


  iid = Images[handle].iid;
  if (!VR_IidValid(iid)) return(0);

  if (!Images[handle].ImExists) return(0);

  VR_ShowWaitCursor(wid); 

  if (!ImSetImgDim(Images[handle].Im, -1, new_ht, new_wd, 
       Images[handle].do_smooth, Images[handle].maintain_ar)) {

	 VR_ShowNormalCursor(wid); 
	 return(0);
  }

  /* re-aspecting might have changed width and height */
  new_wd = ImCurrCols(Images[handle].Im);
  new_ht = ImCurrRows(Images[handle].Im);
  VR_ResetWinSize(wid, new_wd, new_ht);
  VR_ResetImageSize(iid, new_wd, new_ht);
  CopyImgToIid(Images[handle].Im, iid);

  Images[handle].width = new_wd;
  Images[handle].height = new_ht;
  Images[handle].aspected = Images[handle].maintain_ar;

  VR_return =
  VR_ShowImage(iid, wid, Images[handle].do_dither);

  if ((new_wd != evt->xconfigure.width) ||
      (new_ht != evt->xconfigure.height)) { 
    /* avoid resizing top-level windows below the min size possible
       for the WM */ 
    if ((VR_Windows[wid].parwin !=
	   RootWindow(VR_TheDisplay, VR_TheScreen)) || (new_wd >= 96)) {
      wchan.width = new_wd;
      wchan.height = new_ht;
      XConfigureWindow(VR_TheDisplay, VR_Windows[Images[handle].wid].win,
        CWWidth | CWHeight, &wchan);
      /* this will again invoke HandleResizing via an XEvent,
         but that call won't invoke ImSetImgDim */
      XFlush(VR_TheDisplay);
    }
  }

  VR_ShowNormalCursor(wid); 

  if (Images[handle].CtrlExists) RefreshControl(handle);

  return(1);

}

/* send reply */
static void WriteReply(int ok, const char *reply, int fd, int handle)
{
  int len, actual_c, total_c, im_done;
  long bytes_read;
  char big_reply[TASVIR_MAX_STR_LENGTH];
  static XEvent poke_ctrl;
  static int poke_ctrl_set = 0;
  
  if (ok) {
    if (handle >= 0) {
      actual_c = VR_Translators[Images[handle].tid].TotalColorsUsed;
      total_c = VR_Colormaps[VR_Translators[Images[handle].tid].VR_cmap].NumAllocated;
      if (Images[handle].ImExists) {
        bytes_read = ImBytesRead(Images[handle].Im);
        im_done = (ImStateOfImg(Images[handle].Im) == ImStateReadAll) ? 1 : 0;
      } else {
        bytes_read = 0;
        im_done = 1;
      }
      sprintf(big_reply,"OK %d %d %d %d %d %ld %d %d %d %d %d %d %d %d %s\n",
        handle, actual_c, total_c, Images[handle].ImExists ? 1 : 0,
        im_done, bytes_read,
	Images[handle].width, Images[handle].height,
	Images[handle].maxc, Images[handle].ccube,
	Images[handle].maintain_ar ? 1 : 0,
	Images[handle].do_smooth ? 1 : 0,
	Images[handle].do_dither ? 1 : 0,
	Images[handle].private_cmap ? 1 : 0,
	reply);
    } else {
      sprintf(big_reply,"OK %s\n", reply);
    }
  } else {
    sprintf(big_reply,"ERROR %s", reply);
    if (VR_return != VR_SUCCESS) {
      strcat(big_reply," VR_Error:");
      strcat(big_reply,VR_ReturnCodeToString(VR_return));
    } 
    if (IM_return != 1) {
      strcat(big_reply," IM_Error:");
      strcat(big_reply, ImErrorString);
    } 
    strcat(big_reply,"\n");
  }
  if (replySent && (fd != -1)) {
    #ifdef TASVIR_SERVER
    if (LogFile) 
      fprintf(LogFile,"Too late to send reply to fd = %d\n  Reply was: %s",
        fd, big_reply);
    #endif
    return;
  }

  replySent = 1;

  if (fd < 0) {
    #ifdef TASVIR_SERVER
    if (LogFile) 
      fprintf(LogFile,"Client said -noreply and failed to get reply: %s", big_reply);
    #endif
    return;
  }

  if ((handle >= 0) && (fd > 2) && (Images[handle].CtrlExists)
      && (Images[handle].PipeToCtrl[1] == fd)) {
        if (!poke_ctrl_set) {
	  poke_ctrl.type = EnterNotify;
	  poke_ctrl.xcrossing.send_event = True;
	  poke_ctrl.xcrossing.display = VR_TheDisplay;
	  poke_ctrl.xcrossing.same_screen = True;
	  poke_ctrl.xcrossing.root = RootWindow(VR_TheDisplay, VR_TheScreen);
	  poke_ctrl.xcrossing.x =
	  poke_ctrl.xcrossing.y =
	  poke_ctrl.xcrossing.x_root = 
	  poke_ctrl.xcrossing.y_root = 0;
	  poke_ctrl.xcrossing.time = 0;
	  poke_ctrl.xcrossing.focus = True;
	  poke_ctrl.xcrossing.state = 0;
	  poke_ctrl.xcrossing.subwindow = None;
	  poke_ctrl.xcrossing.detail = NotifyAncestor;
	  poke_ctrl.xcrossing.mode = NotifyNormal;
	  poke_ctrl_set = 1;
	}
	poke_ctrl.xcrossing.window = Images[handle].CtrlWin;
	if (!XSendEvent(VR_TheDisplay, Images[handle].CtrlWin,
	  True, EnterWindowMask, &poke_ctrl)) {
	    if (LogFile)
	      fprintf(LogFile,"Control panel for handle %d seems to have died\n",handle);
	    CloseControl(handle);
	    return;
	}
	XFlush(VR_TheDisplay);
        if ((len = strlen(big_reply)) > TASVIR_MAX_STR_LENGTH)
          len = TASVIR_MAX_STR_LENGTH; 
        (void) MyWrite(fd, big_reply, len); 
	return; 
  }
  #ifdef TASVIR_SERVER
  if ((len = strlen(big_reply)) > TASVIR_MAX_STR_LENGTH)
    len = TASVIR_MAX_STR_LENGTH; 
  if (MyWrite(fd, big_reply, len) != len) {
    if (LogFile) 
      fprintf(LogFile,"Failed to send part of reply to fd = %d\n  Reply was: %s",
        fd, big_reply);
  }
  #endif
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

static char * tolowertilldash(char *s)
{
  int delta;
  while ((*s != '\0') && (*s != '-')) {
    delta = *s - 'A';
    if ((delta >= 0) && (delta < 26)) *s = 'a' + delta;
    s++;
  }
  return(++s);
}

static int charToXK(int c, long *val)
{
  if ((c >= 'a') && (c <= 'z')) *val = XK_a + (c - 'a');
  else if ((c >= 'A') && (c <= 'Z')) *val = XK_A + (c - 'A');
  else if ((c >= '0') && (c <= '9')) *val = XK_0 + (c - '0');
  else if (c=='?') *val = XK_question;
  else return(0);

  return(1);

}

static int getEventType(char *s, int *etype, long *val)
{
  char *postdash;
  if (!s) return(0);
  postdash = tolowertilldash(s);
  if (!strncmp(s,"keypress-",9)) {
    *etype = KeyPress;
    if (!charToXK(postdash[0],val)) return(0);
  } else if (!strncmp(s,"keyrelease-",11)) {
    *etype = KeyRelease;
    if (!charToXK(postdash[0],val)) return(0);
  } else if (!strncmp(s,"buttonpress-",11)) {
    *etype = ButtonPress;
    if (postdash[0]=='1') {
      *val = Button1;
    } else if (postdash[0]=='2') {
      *val = Button2;
    } else if (postdash[0]=='3') {
      *val = Button2;
    } else return(0);
  } else if (!strncmp(s,"buttonrelease-",11)) {
    *etype = ButtonRelease;
    if (postdash[0]=='1') {
      *val = Button1;
    } else if (postdash[0]=='2') {
      *val = Button2;
    } else if (postdash[0]=='3') {
      *val = Button2;
    } else return(0);
  } else return(0);
  return(1);
}



static void ProcessPSShow(int argc, char *argv[], int fd, int in_fd)
{ 
  char * imfname = 0; 
  int nextarg = 0;
  Image * Im, TheIm;
  DataDest ddest, *dd = &ddest;
  long ans; 
  char ansstr[30], unit[30]; 
  int ishandle = 0; 
  int pgwid = -1;
  int pghgt = -1; 
  float ftemp; 
  int do_quick_return =0; 
  int image_follows =0; 
  int handle;
  int iwidth=512, iheight=512, nplanes=1; 
  int maintain_ar=1, wwidth = 0, wheight = 0; 
  int do_smooth=1; 
  char * title = (char *) 0; 
  long nbytes = 0, nbytes_tot = 0, nbytes_qual = 0;
  int ImKind = IM_NOT_RAW;
  float xscale=1.0,yscale=1.0;
  float quality;
  int qual_given = 0; /* 1=abs,2=percent */
  char *outfile = 0;
  int send_data_back = 0; 
  int do_center = 1; 

  /* get file name */ 
  nextarg++;
  if (nextarg >= argc) { 
    WriteReply(0,"No image given: psshow failed", fd, -1);
    return;
  } 

  if (!strcmp(argv[nextarg],"-")) {
    if (in_fd < 0) {
      WriteReply(0,"Cannot receive image from here: psshow failed",
	fd, handle);
      return;
    } 
    image_follows = 1; 
  } else {
    imfname = argv[nextarg];
  } 

  /* get outfile name */  
  if (++nextarg >= argc) { 
    WriteReply(0,"No target given: psshow failed", fd, -1);
    return;
  } 
  if (!strcmp(argv[nextarg],"-")) {
    if (fd <= 0) { 
      WriteReply(0,"Target - given with -noreply: psshow failed",
	fd, -1);
      return;
    } 
    send_data_back = 1;
  } else {  
    outfile = argv[nextarg]; 
  }  

  /* get options */
  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-title",4)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -title: psshow failed",
	      fd, -1); 
	return; 
      } 
      title = argv[nextarg]; 
    } else if (!strncmp(argv[nextarg],"-bytes",2)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -bytes: psshow failed",
	      fd, -1); 
	return; 
      } 
      sscanf(argv[nextarg],"%ld",&nbytes_tot);
    } else if (!strncmp(argv[nextarg],"-rbytes",3)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -rbytes: psshow failed",
	  fd, -1); 
	return; 
      } 
      sscanf(argv[nextarg],"%ld",&nbytes);
    } else if (!strncmp(argv[nextarg],"-quality",5)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -quality: psshow failed",
	     fd, -1); 
	return; 
      } 
      quality = (float) 0; 
      if (!strncmp(argv[nextarg],"%",1)) {
	sscanf(argv[nextarg],"%%%f",&quality);
	qual_given = 2;
      } else {
        sscanf(argv[nextarg],"%f",&quality);
	qual_given = 1;
      }
      if (quality <= (float) 0) {
	WriteReply(0, "Value for -quality not positive: psshow failed",
	    fd, -1); 
	return; 
      }

    } else if (!strncmp(argv[nextarg],"-raw",4)) {
      ImKind = IM_RAW; 
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing width for -raw: psshow failed",fd, -1); 
	return; 
      } 
      iwidth = atoi(argv[nextarg]); 
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing height for -raw: psshow failed",fd, -1); 
	return; 
      } 
      iheight = atoi(argv[nextarg]); 
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing num-planes for -raw: psshow failed",
	   fd, -1); 
	return; 
      } 
      nplanes = atoi(argv[nextarg]); 
      if ((nplanes != 1) && (nplanes != 3)) { 
	WriteReply(0, "Num-planes for -raw must be 1 or 3: psshow failed",
	    fd, -1); 
	return; 
      } 
    } else if (!strncmp(argv[nextarg],"-handle",4)) {
      ishandle = 1; 
    } else if (!strncmp(argv[nextarg],"-noaspect",5)) {
      maintain_ar = 0;
    } else if (!strncmp(argv[nextarg],"-aspect",5)) {
      maintain_ar = 1;
    } else if (!strncmp(argv[nextarg],"-nosmooth",5)) {
      do_smooth = 0;
    } else if (!strncmp(argv[nextarg],"-smooth",5)) {
      do_smooth = 1;
    } else if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1; 
    } else if (!strncmp(argv[nextarg],"-nocenter",4)) {
      do_center = 0; 
    } else if (!strncmp(argv[nextarg],"-width",4)) { 
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -width: psshow failed",
	      fd, -1); 
	return; 
      } 
      if (!strncmp(argv[nextarg],"0x",2)) {
	sscanf(argv[nextarg],"0x%f",&xscale);
	if (xscale <= 0) { 
	  WriteReply(0, "Bad scale value for -width: psshow failed",
	      fd, -1); 
	  return; 
	}  
      } else { 
	strcpy(unit,""); 
	ftemp = -1; 
	sscanf(argv[nextarg],"%f%s",&ftemp,unit); 
	if (ftemp <= 0) { 
	  WriteReply(0, "Bad value for -width: psshow failed",
	      fd, -1); 
	  return; 
	}  
	if (unit[0]=='c') {
	  wwidth = (int) (ftemp*72.0/2.54 + 0.5); 
	} else if (unit[0]=='i') { 
	  wwidth = (int) (ftemp*72.0 + 0.5); 
	} else { 
	  wwidth = (int) (ftemp + 0.5); 
	} 
      }  
    } else if (!strncmp(argv[nextarg],"-height",4)) { 
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -height: psshow failed",
	      fd, -1); 
	return; 
      } 
      if (!strncmp(argv[nextarg],"0x",2)) {
	sscanf(argv[nextarg],"0x%f",&yscale);
	if (yscale <= 0) { 
	  WriteReply(0, "Bad scale value for -height: psshow failed",
	      fd, -1); 
	  return; 
	}  
      } else { 
	strcpy(unit,""); 
	ftemp = -1; 
	sscanf(argv[nextarg],"%f%s",&ftemp,unit); 
	if (ftemp <= 0) { 
	  WriteReply(0, "Bad value for -height: psshow failed",
	      fd, -1); 
	  return; 
	}  
	if (unit[0]=='c') {
	  wheight = (int) (ftemp*72.0/2.54 + 0.5); 
	} else if (unit[0]=='i') { 
	  wheight = (int) (ftemp*72.0 + 0.5); 
	} else { 
	  wheight = (int) (ftemp + 0.5); 
	} 
      } 
    } else if (!strncmp(argv[nextarg],"-pwidth",4)) { 
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -pwidth: psshow failed",
	      fd, -1); 
	return; 
      } 
      strcpy(unit,""); 
      ftemp = -1; 
      sscanf(argv[nextarg],"%f%s",&ftemp,unit); 
      if (ftemp <= 0) { 
	  WriteReply(0, "Bad value for -pwidth: psshow failed",
	      fd, -1); 
	  return; 
      }  
      if (unit[0]=='c') {
	  pgwid = (int) (ftemp*72.0/2.54 + 0.5); 
      } else if (unit[0]=='i') { 
	  pgwid = (int) (ftemp*72.0 + 0.5); 
      } else { 
	  pgwid = (int) (ftemp + 0.5); 
      } 
    } else if (!strncmp(argv[nextarg],"-pheight",4)) { 
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -pheight: psshow failed",
	      fd, -1); 
	return; 
      } 
      strcpy(unit,""); 
      ftemp = -1; 
      sscanf(argv[nextarg],"%f%s",&ftemp,unit); 
      if (ftemp <= 0) { 
	  WriteReply(0, "Bad value for -pheight: psshow failed",
	      fd, -1); 
	  return; 
      }  
      if (unit[0]=='c') {
	  pghgt = (int) (ftemp*72.0/2.54 + 0.5); 
      } else if (unit[0]=='i') { 
	  pghgt = (int) (ftemp*72.0 + 0.5); 
      } else { 
	  pghgt = (int) (ftemp + 0.5); 
      } 
    } else {
      WriteReply(0, "Bad option given: psshow failed",fd, -1); 
      return; 
    }
  }

  if (ishandle && image_follows) {
      WriteReply(0, "Cannot use image as - with -handle: psshow failed",
	 fd, -1); 
      return; 
  } 
  
  if (send_data_back) do_quick_return = 0; 


  if (ishandle) {
    sscanf(imfname,"%d", &handle); 
    if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
        !Images[handle].InUse || !Images[handle].ImExists) { 
      WriteReply(0,"Invalid handle given: psshow failed", fd, -1);
      return;
    } 
    Im = Images[handle].Im; 
    wwidth = ImCurrCols(Im); 
    wheight = ImCurrRows(Im); 
    if (do_quick_return) 
      WriteReply(1,"0", fd, handle);

  } else { 

    Im = &TheIm; 
    ImInitImg(Im); 
    ImSetSilent(Im);
    if (LogFile) ImSetErrStream(Im, LogFile);
    else ImSetErrStream(Im, stderr);

    if (ImKind == IM_RAW)
      (void) ImSetKindRaw(Im, nplanes, iheight, iwidth, FALSE);
    else
      (void) ImSetKind(Im, ImKind, FALSE);


    ImSetTotalBytes(Im, nbytes_tot, (nbytes_tot == 0)); 

    if (image_follows) {
      IM_return =  ImSetImgFd(Im, in_fd, nbytes);
    } else {
      if (!(IM_return = ImSetImgFile(Im, imfname, nbytes))) {
        WriteReply(0,"Could not open image file", fd, -1);
        ImFreeImg(Im);
        return;
      }
    }
    
    if (!(IM_return = ImPeekImg(Im)) ||
	 (ImKindOfImg(Im) == IM_UNKNOWN)) {
      WriteReply(0, "Error reading image: psshow failed",fd, -1); 
      ImFreeImg(Im);
      return; 
    } 

    if (ImKindSubSetOf(ImKindOfImg(Im),IM_JPEG)) {
	  ImSetRoughIDCT(Im, JDCT_FLOAT); 
	  ImSetFinalIDCT(Im, JDCT_FLOAT); 
    }
    
    if (wwidth <= 0) wwidth = ImOrigCols(Im); 
    if (wheight <= 0) wheight = ImOrigRows(Im); 
  
    if (title) ImSetName(Im, title);

    if (do_quick_return && !image_follows) {
      WriteReply(1, "0", fd, -1); 
    }
  
    if (image_follows) {
      qual_given = 0; 
      if (!nbytes && !ImTotalBytes(Im)) { 
        WriteReply(0, "Do not know the number of bytes to read! Use -bytes or -rbytes or a Qclic image", fd, -1);
        ImFreeImg(Im);
        return;
      }
    }
    if (qual_given /* && !image_follows */ ) {
      nbytes_qual = ImQclicIcurveSofQ(Im, quality, (qual_given==2));
      if (nbytes_qual > 0) {
        if (ImBytesRead(Im) > nbytes_qual)
	  nbytes_qual = ImBytesRead(Im);
        if ((nbytes == 0) || (nbytes > nbytes_qual)) {
	  if (!(IM_return = ImSetImgBytes(Im, nbytes_qual))) {
            WriteReply(0, "Error using Qclic info: psshow failed",
	      fd, -1); 
            ImFreeImg(Im);
            return; 
	  }
        }
      }
    } 

    if (xscale != 1.0) {
      wwidth = (int) ((float) xscale*ImOrigCols(Im) + 0.5); 
    } 
    if (wwidth < 2) wwidth = 2;
    if (yscale != 1.0) {
      wheight = (int) ((float) yscale*ImOrigRows(Im) + 0.5); 
    } 
    if (wheight < 2) wheight = 2;

    if (!(IM_return = ImReadImg(Im))) {
      WriteReply(0, "Error reading image: psshow failed",fd, -1); 
      ImFreeImg(Im);
      return; 
    }

    if ((wwidth != ImOrigCols(Im)) || (wheight != ImOrigRows(Im))) { 
      if (!(IM_return = 
        ImSetImgDim(Im, -1, wheight, wwidth, do_smooth, maintain_ar))) {
          WriteReply(0, "Error resizing image: psshow failed",fd, -1); 
          ImFreeImg(Im);
          return; 
      } 
    }  


    if (do_quick_return && image_follows) 
      WriteReply(1, "0", fd, -1); 
  }
  
  if (send_data_back) {
    /* first figure out #bytes */
    DD_Init(dd);
    DD_OpenNull(dd);
    ans = ImPSSendImg(Im, dd, do_center, pgwid, pghgt); 
    if (ans <= 0) { 
      WriteReply(0, "Error postscripting image: psshow failed",fd, -1); 
      if (!ishandle) ImFreeImg(Im);
      return; 
    } 
    sprintf(ansstr,"%ld",ans); 
    WriteReply(1, ansstr, fd, ishandle ? handle : -1); 
    DD_Init(dd);
    DD_OpenFd(dd, fd); 
  } else {
    DD_Init(dd);
    if (!DD_OpenFile(dd, outfile)) { 
      sprintf(ansstr,"Error: <%s>",outfile); 
      WriteReply(0, "Error opening outfile: psshow failed",fd, -1); 
      if (!ishandle) ImFreeImg(Im);
      return; 
    } 
  } 

  ans = ImPSSendImg(Im, dd, do_center, pgwid, pghgt); 
  DD_Close(dd); 
  if (!ishandle) ImFreeImg(Im);
  if (ans <= 0) { 
    WriteReply(0, "Error postscripting image: psshow failed",fd, -1); 
  } else {
    if (!send_data_back) { 
      sprintf(ansstr,"%ld",ans); 
      WriteReply(1, ansstr, fd, ishandle ? handle : -1); 
    } 
  }

}  

static int glob_qual_exists = 0;
static float glob_qual = 0;

static void ProcessDefQual(int argc, char *argv[], int fd)
{
  float quality = 0; 
  if (1 >= argc) { 
    WriteReply(0,"No quality given: defqual failed", fd, -1);
    return;
  } 
  if (!strncmp(argv[1],"%",1)) {
    sscanf(argv[1],"%%%f",&quality);
    glob_qual_exists = 2;
  } else {
    sscanf(argv[1],"%f",&quality);
    glob_qual_exists = 1;
  }
  if (quality <= (float) 0) {
    glob_qual_exists = 0;
  } else {
    glob_qual = quality; 
  }
  WriteReply(1,"", fd, -1);
}

static char SmallImageIcon[512] = \
  "P2\n8 8\n255\n\
0 255 0 255 0 255 0 255 255 0 255 0 255 0 255 0\n\
0 255 0 255 0 255 0 255 255 0 255 0 255 0 255 0\n\
0 255 0 255 0 255 0 255 255 0 255 0 255 0 255 0\n\
0 255 0 255 0 255 0 255 255 0 255 0 255 0 255 0\0"; 

static int IgnoreSmallImage = 0; 
static int SmallImageWidth = 16, SmallImageHeight = 16; 
static int SmallDimensionEither = 0; 

static void ProcessSmallIcon(int argc, char *argv[], int fd)
{
  int w, h, nextarg, either; 
  if (argc < 3) { 
    WriteReply(0,"Small width/height missing: smlicon failed", fd, -1);
    return;
  } 
  sscanf(argv[1],"%d",&w); 
  sscanf(argv[2],"%d",&h); 
  either = 0; 
  nextarg = 2; 
  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-either",4)) {
      either = 1; 
    } else if (!strncmp(argv[nextarg],"-both",4)) {
      either = 0;
    } else {
      WriteReply(0,"Invalid argument: smlicon failed", fd, -1);
      return;
    } 
  } 
  if ((w < 0) || (h < 0)) {
    WriteReply(0,"Bad width/height: smlicon failed", fd, -1);
    return;
  } 
  IgnoreSmallImage = 1; 
  SmallImageWidth = w; 
  SmallImageHeight = h; 
  SmallDimensionEither = either; 
  WriteReply(1,"", fd, -1);
}

static void ProcessNoSmallIcon(int argc, char *argv[], int fd)
{
  IgnoreSmallImage = 0; 
  WriteReply(1,"", fd, -1);
}


static void ProcessShow(int argc, char *argv[], int fd, int in_fd)
{ 
  char * imfname = 0; 
  int nextarg = 0;
  Image * Im;
  VR_ImId iid; 
  char *rough_idct = 0, *final_idct = 0; 
  int i, dump_orig = 0, do_refresh = 0, do_quit_on_q = 0;
  int do_quick_return =0, tryback = 0;
  int image_follows =0, is_url = 0, no_subwin = 0;
  int ccube = 4, maxc = VR_MaxPixel, handle;
  long show_after = 0, sync = 0;
  int nloops, show_row_start, show_row_end, read_step;
  long remaining = 0; 
  int show_ctrl = 0; 
  int private_cmap = 0; 
  int do_dither = 0, handle_resize=1;
  Window win; 
  int low_x, low_y, high_x, high_y; 
  unsigned long cflag; 
  int iwidth=512, iheight=512, nplanes=1, nplanes_toshow;
  int width, height, maintain_ar=1, wwidth = 0, wheight = 0; 
  int do_smooth=1, no_ops=0;
  char * title = (char *) 0; 
  long nbytes = 0, nbytes_tot = 0, nbytes_qual = 0;
  long nbytes_read; 
  int ImKind = IM_NOT_RAW;
  int needXimage;
  int ctrlToggle = 0, ctrlToggleEvent;
  long ctrlToggleVal;
  float xscale=1.0,yscale=1.0;
  float quality = glob_qual;
  int qual_given = glob_qual_exists; /* 1=abs,2=percent */
  int url_waiter = 1;
  int file_waiter = 0; 
  Window waiterWin = 0; 
  int ignore_small_image = IgnoreSmallImage; 
  int small_w = SmallImageWidth;
  int small_h = SmallImageHeight;
  int small_dim_either = SmallDimensionEither; 

  


  if (NumImages >= TASVIR_MAX_IMAGES) {
    WriteReply(0,"Too many images: show failed", fd, -1);
    return;
  }

  handle = NewHandle();


  /* get file name */ 
  nextarg++;
  if (nextarg >= argc) { 
    WriteReply(0,"No image given: show failed", fd, -1);
    return;
  } 

  if (!strcmp(argv[nextarg],"-")) {
    image_follows = 1; 
  } else {
    imfname = argv[nextarg];
    if (!strncmp(imfname,"http://",7)) {
      is_url = 1; 
      sync = show_after = -1; 
    }
  } 

  /* get window */ 
  nextarg++; 
  if (nextarg >= argc) { 
    WriteReply(0,"No XWindow given: show failed", fd, -1);
    return;
  } 
  if (!strcmp(argv[nextarg],"-")) win = root_win;
  else if ((win = (Window) strtol(argv[nextarg],(char **) 0,16)) == 0) win = root_win; 

  /* get coordinates */ 

  nextarg++; 
  if (nextarg >= argc) { 
    WriteReply(0,"Missing coordinate(s): show failed", fd, -1);
    return;
  } 
  
  low_x = atoi(argv[nextarg]);

  nextarg++; 
  if (nextarg >= argc) { 
    WriteReply(0,"Missing coordinate(s): show failed", fd, -1);
    return;
  } 
  
  low_y = atoi(argv[nextarg]);

  nextarg++; 
  if (nextarg >= argc) { 
    WriteReply(0,"Missing coordinate(s): show failed", fd, -1);
    return;
  } 
  if (!strncmp(argv[nextarg],"0x",2)) {
    high_x = 0;
    sscanf(argv[nextarg],"0x%f",&xscale);
    if (xscale <= 0) {
      WriteReply(0,"Bad x scaling factor: show failed", fd, -1);
      return;
    }
  } else {
    high_x = atoi(argv[nextarg]);
  }

  nextarg++; 
  if (nextarg >= argc) { 
    WriteReply(0,"Missing coordinate(s): show failed", fd, -1);
    return;
  } 
  
  if (!strncmp(argv[nextarg],"0x",2)) {
    high_y = 0;
    sscanf(argv[nextarg],"0x%f",&yscale);
    if (xscale <= 0) {
      WriteReply(0,"Bad y scaling factor: show failed", fd, -1);
      return;
    }
  } else {
    high_y = atoi(argv[nextarg]);
  }

  if ((high_x != 0) && ((wwidth = high_x - low_x + 1) <= 1)) {
    WriteReply(0,"Bad coordinate(s): show failed", fd, -1);
    return;
  } 
  
  if ((high_y != 0) && ((wheight = high_y - low_y + 1) <= 1)) { 
    WriteReply(0,"Bad coordinate(s): show failed", fd, -1);
    return;
  } 
  
  /* get options */
  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-ccube",3)) { 
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -ccube: show failed",fd, -1); 
	return; 
      }
      ccube = atoi(argv[nextarg]);
    } else if (!strncmp(argv[nextarg],"-maxc",4)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -maxc: show failed",fd, -1); 
	return; 
      } 
      maxc = atoi(argv[nextarg]); 
    } else if (!strncmp(argv[nextarg],"-title",4)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -title: show failed",fd, -1); 
	return; 
      } 
      title = argv[nextarg]; 
    } else if (!strncmp(argv[nextarg],"-idct",4)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -idct: show failed",fd, -1); 
	return; 
      } 
      rough_idct = argv[nextarg]; 
    } else if (!strncmp(argv[nextarg],"-fidct",5)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -fidct: show failed",fd, -1); 
	return; 
      } 
      final_idct = argv[nextarg]; 
    } else if (!strncmp(argv[nextarg],"-bytes",2)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -bytes: show failed",fd, -1); 
	return; 
      } 
      sscanf(argv[nextarg],"%ld",&nbytes_tot);
    } else if (!strncmp(argv[nextarg],"-rbytes",3)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -rbytes: show failed",fd, -1); 
	return; 
      } 
      sscanf(argv[nextarg],"%ld",&nbytes);
    } else if (!strncmp(argv[nextarg],"-showafter",6)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -showafter: show failed",fd, -1); 
	return; 
      } 
      sscanf(argv[nextarg],"%ld",&show_after);
    } else if (!strncmp(argv[nextarg],"-sync",3)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -sync: show failed",fd, -1); 
	return; 
      } 
      sscanf(argv[nextarg],"%ld",&sync);
    } else if (!strncmp(argv[nextarg],"-quality",5)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -quality: show failed",fd, -1); 
	return; 
      } 
      quality = (float) 0; 
      if (!strncmp(argv[nextarg],"%",1)) {
	sscanf(argv[nextarg],"%%%f",&quality);
	qual_given = 2;
      } else {
        sscanf(argv[nextarg],"%f",&quality);
	qual_given = 1;
      }
      if (quality <= (float) 0) {
	WriteReply(0, "Value for -quality not positive: show failed",fd, -1); 
	return; 
      }

    } else if (!strncmp(argv[nextarg],"-raw",4)) {
      ImKind = IM_RAW; 
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing width for -raw: show failed",fd, -1); 
	return; 
      } 
      iwidth = atoi(argv[nextarg]); 
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing height for -raw: show failed",fd, -1); 
	return; 
      } 
      iheight = atoi(argv[nextarg]); 
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing num-planes for -raw: show failed",fd, -1); 
	return; 
      } 
      nplanes = atoi(argv[nextarg]); 
      if ((nplanes != 1) && (nplanes != 3)) { 
	WriteReply(0, "Num-planes for -raw must be 1 or 3: show failed",fd, -1); 
	return; 
      } 
    } else if (!strncmp(argv[nextarg],"-waiter",4)) {
      file_waiter = 1; 
    } else if (!strncmp(argv[nextarg],"-nowaiter",5)) {
      file_waiter = 0; 
    } else if (!strncmp(argv[nextarg],"-urlwaiter",5)) {
      url_waiter = 1; 
    } else if (!strncmp(argv[nextarg],"-nourlwaiter",6)) {
      url_waiter = 0; 
    } else if (!strncmp(argv[nextarg],"-smlicon",4)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing width for -smlicon: show failed",fd, -1); 
	return;
      }
      small_w = atoi(argv[nextarg]);
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing height for -smlicon: show failed",fd, -1); 
	return;
      }
      small_h = atoi(argv[nextarg]);
      if ((small_w < 0) || (small_h < 0)) { 
	WriteReply(0, "Bad width/height for -smlicon: show failed",fd, -1); 
	return;
      }
      ignore_small_image = 1;
    } else if (!strncmp(argv[nextarg],"-nosmlicon",6)) {
      ignore_small_image = 0;
    } else if (!strncmp(argv[nextarg],"-either",4)) {
      small_dim_either = 1; 
    } else if (!strncmp(argv[nextarg],"-both",4)) {
      small_dim_either = 4; 
    } else if (!strncmp(argv[nextarg],"-refresh",4)) {
      do_refresh = 1; 
    } else if (!strncmp(argv[nextarg],"-dorig",4)) {
      dump_orig = 1; 
    } else if (!strncmp(argv[nextarg],"-sorig",4)) {
      dump_orig = 0; 
    } else if (!strncmp(argv[nextarg],"-popcontrol",5)) {
      show_ctrl = 1; 
    } else if (!strncmp(argv[nextarg],"-nosubwin",6)) {
      no_subwin = 1; 
    } else if (!strncmp(argv[nextarg],"-remap",5)) {
      /* nothing to be done */ 
    } else if (!strncmp(argv[nextarg],"-controls",5)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -controls: show failed",fd, -1); 
	return;
      }
      if (!getEventType(argv[nextarg],&ctrlToggleEvent, &ctrlToggleVal)) {
	WriteReply(0, "Bad event type for -controls: show failed",fd, -1); 
	return;
      }
      ctrlToggle = 1;
    } else if (!strncmp(argv[nextarg],"-quitonq",5)) {
      do_quit_on_q = 1; 
    } else if (!strncmp(argv[nextarg],"-noaspect",5)) {
      maintain_ar = 0;
    } else if (!strncmp(argv[nextarg],"-aspect",5)) {
      maintain_ar = 1;
    } else if (!strncmp(argv[nextarg],"-dither",4)) {
      do_dither = 1; 
    } else if (!strncmp(argv[nextarg],"-nodither",4)) {
      do_dither = 0; 
    } else if (!strncmp(argv[nextarg],"-nosmooth",5)) {
      do_smooth = 0;
    } else if (!strncmp(argv[nextarg],"-smooth",5)) {
      do_smooth = 1;
    } else if (!strncmp(argv[nextarg],"-noresize",5)) {
      handle_resize = 0;
    } else if (!strncmp(argv[nextarg],"-privcolors",4)) {
      private_cmap = 1; 
    } else if (!strncmp(argv[nextarg],"-noprivcolors",5)) {
      private_cmap = 0; 
    } else if (!strncmp(argv[nextarg],"-xbacking",3)) {
      tryback = 1; 
    } else if (!strncmp(argv[nextarg],"-noops",4)) {
      no_ops = 1; 
    } else if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1; 
    } else {
      WriteReply(0, "Bad option given: show failed",fd, -1); 
      return; 
    }
  }

  if (no_subwin && ((long) win == (long) root_win)) no_subwin = 0;

  if (no_subwin) {
    tryback = 1;
    do_refresh = 0;
    do_quit_on_q = 0;
    ctrlToggle = 0; 
    private_cmap = 0; 
  }


  if (image_follows) {
    qual_given = 0; 
    ignore_small_image = 0;
  } 

  if (url_waiter && is_url)  { 
    file_waiter = 1; 
  } 

  if (no_ops) {
    handle_resize = 0;
  }

  if (ignore_small_image && (high_x > 0) && (high_y > 0) &&
      ( (small_dim_either &&
	  ((wwidth <= small_w) || (wheight <= small_h))) ||  
        (!small_dim_either &&
	  ((wwidth <= small_w) && (wheight <= small_h))) ) ) {  
    file_waiter = 0;
    handle_resize = 0;
    no_ops = 1;
    maxc = 2;
    dump_orig = 1;
    nbytes = 0;
    nbytes_tot = 0;
    nbytes_qual = 0;
    qual_given = 0;
    do_smooth = 0;
  } else {
    ignore_small_image = 0;
  }

  Im = Images[handle].Im
     = (Image *) malloc(sizeof(Image));
  
  if (!Im) { 
    WriteReply(0,"Out of memory: show failed", fd, -1);
    return;
  }

  if (do_quick_return && !image_follows) {
    WriteReply(1, "", fd, handle); 
  }
  
  if (file_waiter) { 
    waiterWin = VR_GetXWindow(250, 100, low_x, low_y, 1, win,
      "Tasvir: WhileYouWait!"); 
    XDrawImageString(VR_TheDisplay, waiterWin, VR_TheGC, 
      20, 20, "Initializing Image...", 21); 
    XFlush(VR_TheDisplay); 
  } 

  ImInitImg(Im); 
  ImSetSilent(Im);
  if (dump_orig) ImDiscardOriginal(Im); 
  if (LogFile) ImSetErrStream(Im, LogFile);
  else ImSetErrStream(Im, stderr);

  if (ImKind == IM_RAW)
    (void) ImSetKindRaw(Im, nplanes, iheight, iwidth, FALSE);
  else
    (void) ImSetKind(Im, ImKind, FALSE);


  ImSetTotalBytes(Im, nbytes_tot, (nbytes_tot == 0)); 

  if (file_waiter) { 
    XClearWindow(VR_TheDisplay, waiterWin); 
    if (is_url) 
      XDrawImageString(VR_TheDisplay, waiterWin, VR_TheGC, 
        20, 20, "Making http connection...", 25); 
    else 
      XDrawImageString(VR_TheDisplay, waiterWin, VR_TheGC, 
        20, 20, "Opening image source...", 23); 
    XFlush(VR_TheDisplay); 
  } 
  

  if (ignore_small_image) {
    IM_return =  ImSetImgMemory(Im, SmallImageIcon, 512);
  } else if (image_follows) {
    IM_return =  ImSetImgFd(Im, in_fd, nbytes);
  } else {
    if (!(IM_return = ImSetImgFile(Im, imfname, nbytes))) {
      WriteReply(0,"Could not open image file", fd, -1);
      ImFreeImg(Im);
      free(Im); 
      if (file_waiter) XDestroyWindow(VR_TheDisplay, waiterWin); 
      return;
    }
  }
  
  if (file_waiter) { 
    XClearWindow(VR_TheDisplay, waiterWin); 
    XDrawImageString(VR_TheDisplay, waiterWin, VR_TheGC, 
      20, 20, "Reading image header...", 23); 
    XFlush(VR_TheDisplay); 
  } 
  
  if (!(IM_return = ImPeekImg(Im)) || (ImKindOfImg(Im) == IM_UNKNOWN)) {
      WriteReply(0, "Error reading image: show failed",fd, -1); 
      ImFreeImg(Im);
      free(Im); 
      if (file_waiter) XDestroyWindow(VR_TheDisplay, waiterWin); 
      return; 
  } 

  if (ImKindSubSetOf(ImKindOfImg(Im),IM_JPEG)) {
    if (rough_idct) {
      if (!strncasecmp(rough_idct,"float",2))
	ImSetRoughIDCT(Im, JDCT_FLOAT); 
      else if (!strncasecmp(rough_idct,"islow",2))
	ImSetRoughIDCT(Im, JDCT_ISLOW); 
      else if (!strncasecmp(rough_idct,"ifast",2))
	ImSetRoughIDCT(Im, JDCT_IFAST); 
    } 
    if (final_idct) {
      if (!strncasecmp(final_idct,"float",2))
	ImSetFinalIDCT(Im, JDCT_FLOAT); 
      else if (!strncasecmp(final_idct,"islow",2))
	ImSetFinalIDCT(Im, JDCT_ISLOW); 
      else if (!strncasecmp(final_idct,"ifast",2))
	ImSetFinalIDCT(Im, JDCT_IFAST); 
    } 
  }

  
  if (title) ImSetName(Im, title);

  if (file_waiter) { 
    XClearWindow(VR_TheDisplay, waiterWin); 
    XDrawImageString(VR_TheDisplay, waiterWin, VR_TheGC, 
      20, 20, "Reading image...", 16); 
    XFlush(VR_TheDisplay); 
  } 
  

  if (image_follows) {
    if (!nbytes && !ImTotalBytes(Im)) { 
      WriteReply(0, "Do not know the number of bytes to read! use -bytes or -rbytes or a Qclic image", fd, -1);
      ImFreeImg(Im);
      free(Im); 
      if (file_waiter) XDestroyWindow(VR_TheDisplay, waiterWin); 
      return;
    }
  }
  if (qual_given /* && !image_follows */ ) {
    nbytes_qual = ImQclicIcurveSofQ(Im, quality, (qual_given==2));
    if (nbytes_qual > 0) {
      if (ImBytesRead(Im) > nbytes_qual) nbytes_qual = ImBytesRead(Im);
      if ((nbytes == 0) || (nbytes > nbytes_qual)) {
	if (!(IM_return = ImSetImgBytes(Im, nbytes_qual))) {
          WriteReply(0, "Error using Qclic info: show failed",fd, -1); 
          ImFreeImg(Im);
          free(Im); 
          if (file_waiter) XDestroyWindow(VR_TheDisplay, waiterWin); 
          return; 
	}
      }
    }
  } 
  strcpy(Images[handle].Name, ImNameOfImg(Im));

  iwidth = ImOrigCols(Im);
  iheight = ImOrigRows(Im);
  nplanes = ImPlanesOfImg(Im);
  
  if (high_x == 0) {
    if (xscale != 1.0) {
      high_x = low_x + ((int) ((float) ((float) iwidth)*xscale)) + 1;
      wwidth = high_x - low_x + 1;
    } else {
      high_x = low_x + iwidth - 1;
      wwidth = iwidth;
    }
  }
  if (wwidth < 2) {
    wwidth = 2;
    high_x = low_x + 1;
  }

  if (high_y == 0) {
    if (yscale != 1.0) {
      high_y = low_y + ((int) ((float) ((float) iheight)*yscale)) + 1;
      wheight = high_y - low_y + 1;
    } else {
      high_y = low_y + iheight - 1;
      wheight = iheight;
    }
  }
  if (wheight < 2) {
    wheight = 2;
    high_y = low_y + 1;
  }

   
  /***** now figure out how much of the image to read *****/

  nbytes_read = DS_bytes_read(&(Im->ImgSrc)); 
  nbytes = DS_bytes_to_read(&(Im->ImgSrc));
  if (nbytes == 0) nbytes = DS_total_in_src(&(Im->ImgSrc)) -
    (DS_total_count(&(Im->ImgSrc)) - nbytes_read) ; 

  if ( strcmp(ImFilterOfImg(Im),"none") ||
       ((show_after==0) && (sync==0)) ) {
    show_after = nbytes; 
  } else {
    if (show_after == 0) show_after = sync; 
    if (show_after < 0) {
      /* use a heuristic for now */
      if (ImTotalBytes(Im) > 0) {
	show_after = ImTotalBytes(Im) * 40 / 100;
      } else { 
        show_after = ((long) iwidth)*iheight*40
	  *nplanes*ImSampleBytes(Im)/100; 
        if (ImIsCompressed(Im)) {
  	  /* 10:1 compression seems like a reasonable assumption */
	  show_after /= 10; 
        }
      } 
    } 
    if (show_after < (nbytes_read + 100)) 
      show_after = nbytes_read + 100; 
    if ((nbytes > 0) && (show_after > nbytes)) show_after = nbytes; 
    if (sync <= 0) sync = show_after/2;  
  } 

  if (show_after == nbytes) read_step = 0; 
  else {
    if (!DS_ResetSrcBytes(&(Im->ImgSrc), show_after)) { 
      WriteReply(0, "Error handling image source: show failed",fd, -1); 
      ImFreeImg(Im);
      free(Im); 
      if (file_waiter) XDestroyWindow(VR_TheDisplay, waiterWin); 
      return; 
    }
    read_step = 1; 
  }


  if (!(IM_return = ImReadImg(Im))) {
      WriteReply(0, "Error reading image: show failed",fd, -1); 
      ImFreeImg(Im);
      free(Im); 
      if (file_waiter) XDestroyWindow(VR_TheDisplay, waiterWin); 
      return; 
  }

  if (file_waiter) { 
    XClearWindow(VR_TheDisplay, waiterWin); 
    XDrawImageString(VR_TheDisplay, waiterWin, VR_TheGC, 
      20, 20, "Resizing image...", 17); 
    XFlush(VR_TheDisplay); 
  } 
  

  if (!(IM_return = 
    ImSetImgDim(Im, -1, wheight, wwidth, do_smooth, maintain_ar))) {
      WriteReply(0, "Error resizing image: show failed",fd, -1); 
      ImFreeImg(Im);
      free(Im); 
      if (file_waiter) XDestroyWindow(VR_TheDisplay, waiterWin); 
      return; 
  }
  
  width = ImCurrCols(Im);
  height = ImCurrRows(Im);

  Images[handle].aspected = maintain_ar || ((width==iwidth) && (height==iheight));

  nplanes_toshow = (nplanes < 3) ? 1 : 3;

  Images[handle].iid =
  iid = VR_GetNewImage(width, height, (nplanes_toshow == 1) ? VR_Gray :
		       VR_RGB, ImNameOfImg(Im));
  
  if (iid < 0) {
    VR_return = VR_SEE_ERR_MSG; 
    WriteReply(0, "Viewer error: show failed", fd, -1);
    ImFreeImg(Im);
    free(Im); 
    if (file_waiter) XDestroyWindow(VR_TheDisplay, waiterWin); 
    return;
  }


  if (do_quick_return && image_follows && !read_step) {
    WriteReply(1, "", fd, handle); 
  }

  CopyImgToIid(Im, iid);

  if (file_waiter) { 
    XClearWindow(VR_TheDisplay, waiterWin); 
    XDrawImageString(VR_TheDisplay, waiterWin, VR_TheGC, 
      20, 20, "Allocating colors...", 20); 
    XFlush(VR_TheDisplay); 
  } 
  

  cflag = ccubeToflag(ccube); 
  VR_CFL_SET_NCOLORS(cflag, maxc);
  if (private_cmap) {
    cflag |= VR_CFL_PRIVATE; 
  }

  Images[handle].tid = VR_GetTranslatorFromCMofXWin(iid, cflag, win); 

  if (file_waiter) XDestroyWindow(VR_TheDisplay, waiterWin); 

  if (Images[handle].tid < 0) { 
    VR_return = VR_SEE_ERR_MSG; 
    ImFreeImg(Im);
    VR_FreeImage(iid); 
    free(Im); 
    WriteReply(0, "Viewer error getting translator: show failed", fd, -1);
    return;
  }
  
  if (no_subwin) { 
    Images[handle].x = low_x + ((wwidth - width)/2); 
    Images[handle].y = low_y + ((wheight - height)/2); 
    Images[handle].wid = VR_SetWindow(width, height,
      Images[handle].x, Images[handle].y, 
      Images[handle].tid, win, 
      1, 1, &tryback, 1); 
  } else {
    Images[handle].x = 0; 
    Images[handle].y = 0; 
    Images[handle].wid = VR_GetWindow(width, height,
      low_x + ((wwidth - width)/2), low_y + ((wheight - height)/2), 
      Images[handle].tid, 1, win, 
      !do_refresh, 1, &tryback); 
  }

  if (Images[handle].wid < 0) { 
    VR_return = VR_SEE_ERR_MSG; 
    ImFreeImg(Im);
    VR_FreeImage(iid); 
    VR_FreeTranslator(Images[handle].tid); 
    free(Im); 
    WriteReply(0, "Viewer error getting window: show failed", fd, -1);
    return;
  }
  
  VR_ShowWaitCursor(Images[handle].wid); 

  Images[handle].InUse = 1;
  Images[handle].ImExists = 1;
  Images[handle].no_subwin = no_subwin; 
  Images[handle].maxc = maxc; 
  Images[handle].ccube = ccube; 
  Images[handle].private_cmap = private_cmap; 
  Images[handle].do_dither = do_dither;
  Images[handle].do_smooth = do_smooth;
  Images[handle].maintain_ar = maintain_ar;
  Images[handle].handle_resize = handle_resize;
  Images[handle].am_refreshing = (do_refresh && !tryback && !no_subwin);
  Images[handle].CtrlExists = 0; 
  Images[handle].owidth = iwidth; 
  Images[handle].oheight = iheight; 
  Images[handle].width = width;
  Images[handle].height = height;
  Images[handle].nplanes = ImPlanesOfImg(Im);
  strcpy(Images[handle].format,ImKindString(ImKindOfImg(Im)));
  strcpy(Images[handle].filter,ImFilterOfImg(Im));
  NumImages++;



  VR_return = VR_ShowImage(iid, Images[handle].wid, do_dither);
  
  if (show_ctrl) InvokeControl(handle);

  if (read_step) { 
    if (nbytes == 0) {
      nloops = -1;
    } else {
      nbytes -= show_after;
      nloops = nbytes/sync;
      remaining = nbytes - (nloops*sync); 
      if (remaining) nloops++;
      else remaining = sync;
    } 
    i = 0;
    while (((nloops < 0) || (i < nloops)) && 
	   !DS_file_eof(&(Im->ImgSrc)) &&
	   (ImStateOfImg(Im) == ImStateReadSome)) { 
      if (i==(nloops-1)) sync = remaining; 
      if (!DS_ExtendSrc(&(Im->ImgSrc), sync)) break;

      if (!ImReadMoreImg(Im)) break; 
      CopyImgToIid(Im, iid);

      show_row_start = ImFirstChanged(Im, 0);
      show_row_end = ImLastChanged(Im, 0);
      if (height < iheight) {
        show_row_start = show_row_start * height/ iheight; 
        show_row_end = show_row_end * height/ iheight; 
      } else if (height > iheight) {
        show_row_start = (show_row_start * height/ iheight) - 1 -
           (height/iheight); 
        if (show_row_start < 0) show_row_start = 0; 
        show_row_end = (show_row_end * height/ iheight) + 1 + 
           (height/iheight); 
        if (show_row_end >= height) show_row_end = height - 1; 
      }

      VR_return = VR_ShowImageRect(iid, Images[handle].wid,
        0, show_row_start, width, show_row_end - show_row_start + 1, 
        do_dither);

      if (Images[handle].CtrlExists) RefreshControl(handle); 
      i++; 
    }  
  }

  WriteReply(1, "", fd, handle); 
  

  if (do_quit_on_q && !no_subwin)
    VR_RegisterEventHandler(Images[handle].wid, KeyPress,
      XK_q, HandleQuit);
  if (handle_resize && !no_subwin)
    VR_RegisterEventHandler(Images[handle].wid, ConfigureNotify,
      0, HandleResizing);
  if (ctrlToggle && !no_subwin)
    VR_RegisterEventHandler(Images[handle].wid, ctrlToggleEvent,
      ctrlToggleVal, HandleControlToggle);

  needXimage = Images[handle].am_refreshing; /* || 
     (!no_ops && (ImStateOfImg(Im) == ImStateReadSome)); */ 

  if (!needXimage) VR_FreeWindowXImage(Images[handle].wid);

  if (no_ops) {
    Images[handle].ImExists = 0;
    ImFreeImg(Im);
    free(Im);
    VR_FreeTransTable(Images[handle].tid);
  }

  VR_ShowNormalCursor(Images[handle].wid); 

} 

static void ProcessFree(int argc, char *argv[], int fd)
{ 
  int nextarg = 0, do_quick_return = 0;
  int handle;

  /* get handle name */ 
  nextarg++;
  if (nextarg >= argc) { 
    WriteReply(0,"No handle given: free failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: free failed", fd, -1);
    return;
  } 


  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1;
    } else {
      WriteReply(0,"Bad option given: free failed", fd, handle);
    }
  }

  if (do_quick_return) WriteReply(1,"",fd, -1);

  CloseControl(handle);
  VR_FreeWindow(Images[handle].wid); 
  VR_FreeTranslator(Images[handle].tid); 

  if (Images[handle].ImExists) { 
     Images[handle].ImExists = 0; 
     ImFreeImg(Images[handle].Im);
     free(Images[handle].Im); 
  } 

  VR_FreeImage(Images[handle].iid);

  Images[handle].InUse = 0;
  NumImages--;
  WriteReply(1, "", fd, -1); 
} 

static void ProcessDOrig(int argc, char *argv[], int fd)
{ 
  int nextarg = 0, do_quick_return = 0;
  int handle;

  /* get handle name */ 
  nextarg++;
  if (nextarg >= argc) { 
    WriteReply(0,"No handle given: dorig failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: dorig failed", fd, -1);
    return;
  } 

  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1;
    } else {
      WriteReply(0,"Bad option given: dorig failed", fd, handle);
    }
  }

  if (do_quick_return) WriteReply(1,"",fd, handle);

  if (Images[handle].ImExists) { 
     ImDiscardOriginal(Images[handle].Im);
  } 

  WriteReply(1, "", fd, handle); 
  
} 

static int HandleQuit(VR_WinId wid, XEvent *evt)
{
  int handle;
  char cmd[2][50];

  if (!VR_WidValid(wid)) return(0);


  if ((handle = FindTasvirImageFromWid(wid)) < 0) return(0);
  
  strcpy(cmd[0],"free");
  sprintf(cmd[1],"%d",handle);
  CmdArgc = 2;
  CmdArgv[0] = cmd[0];
  CmdArgv[1] = cmd[1];
  ProcessFree(CmdArgc, CmdArgv, -1);
  return(1);

}

static void ProcessXBackup(int argc, char *argv[], int fd)
{ 
  if (VR_ScreenDoesBacking) WriteReply(1, "Have xbacking", fd, -1); 
  else WriteReply(0,"Do not have xbacking",fd, -1);
} 

static void ProcessReset(int argc, char *argv[], int fd)
{ 
  int handle;
  int nextarg = 0, do_quick_return = 0;

  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1;
    } else {
      WriteReply(0,"Bad option given: reset failed", fd, -1); 
    }
  } 

  if (do_quick_return) WriteReply(1,"",fd, -1);

  for (handle=0;handle < TASVIR_MAX_IMAGES; handle++) { 
    if (Images[handle].InUse) { 
      CloseControl(handle);
      VR_FreeWindow(Images[handle].wid); 
      VR_FreeTranslator(Images[handle].tid); 
      if (Images[handle].ImExists) { 
        Images[handle].ImExists = 0; 
        ImFreeImg(Images[handle].Im);
        free(Images[handle].Im); 
      }

      VR_FreeImage(Images[handle].iid);
      Images[handle].InUse = 0;
    }
  }

  NumImages = 0;

  WriteReply(1, "", fd, -1); 

} 

static void ProcessQuery(int argc, char *argv[], int fd)
{ 
  int nextarg = 0; 
  int handle;


  /* get handle name */ 
  nextarg++;
  if (nextarg >= argc) {
    WriteReply(0,"No handle given: query failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: query failed", fd, -1);
    return;
  } 

  WriteReply(1, Images[handle].Name, fd, handle); 

} 

static void ProcessXv(int argc, char *argv[], int fd)
{ 
  int nextarg = 0; 
  int handle, ans;
  int do_quick_return = 0; 
  int pipe_to_xv[2];
  int xv_pid;


  /* get handle name */ 
  nextarg++;
  if (nextarg >= argc) {
    WriteReply(0,"No handle given: xv failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: xv failed", fd, -1);
    return;
  } 
  if ( !Images[handle].ImExists) { 
    WriteReply(0,"Image has been discarded: xv failed", fd, handle);
    return;
  }

  /* get options */
  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-quickreturn", 5))  { 
      do_quick_return = 1;
    } else { 
      WriteReply(0,"Bad option given: xv failed", fd, handle);
      return;
    } 
  }

  if (do_quick_return)
    WriteReply(1, "", fd, handle); 

  if (pipe(pipe_to_xv) < 0) {
    WriteReply(0, "Could not create pipe: xv failed", fd, handle); 
    return;
  }
  if ((xv_pid = fork()) < 0) {
    WriteReply(0, "Could not fork: xv failed", fd, handle); 
    return;
  }

  if (xv_pid == 0) {
    /* child */
    close(pipe_to_xv[1]);
    dup2(pipe_to_xv[0], 0);
    execlp("xv","xv","-", (char *) 0);
  } else {
    /* parent */
    close(pipe_to_xv[0]);
    ans = ImWriteImg(Images[handle].Im, pipe_to_xv[1], IM_PNM);
    close(pipe_to_xv[1]);
    if (!ans) { 
      kill(xv_pid, SIGKILL); 
      WriteReply(0,"Could not WriteImg: xv failed", fd, handle);
      return;
    } 
  } 
  WriteReply(1, "", fd, handle); 

} 

static void ProcessSave(int argc, char *argv[], int fd)
{ 
  int nextarg = 0; 
  int handle;
  int do_quick_return = 0; 
  char *fname; 

  /* get handle name */ 
  nextarg++;
  if (nextarg >= argc) {
    WriteReply(0,"No handle given: save failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: save failed", fd, -1);
    return;
  } 
  if ( !Images[handle].ImExists) { 
    WriteReply(0,"Image has been discarded: save failed", fd, handle);
    return;
  }
  nextarg++;
  if (nextarg >= argc) {
    WriteReply(0,"No file name given: save failed", fd, handle);
    return;
  } 
  if (!strncmp(argv[nextarg],"-",1)) { 
    WriteReply(0,"Bad file name given: save failed", fd, handle);
    return;
  } 
  fname = argv[nextarg];

  /* get options */
  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-quickreturn", 5))  { 
      do_quick_return = 1;
    } else { 
      WriteReply(0,"Bad option given: save failed", fd, handle);
      return;
    } 
  }

  if (do_quick_return)
    WriteReply(1, "", fd, handle); 

  if (!(IM_return = ImSaveImg(Images[handle].Im, fname, IM_PNM))) {
    WriteReply(0,"Could not SaveImg: save failed", fd, handle);
    return;
  } 

  WriteReply(1, "", fd, handle); 

} 

#include "RDOPT.h"

static void ProcessSaveCompressed(int argc, char *argv[],
  int fd)
{ 
  int nextarg = 0; 
  int handle;
  int do_quick_return = 0; 
  char *fname; 
  int on_original = 0;
  char *qclicfname = 0;
  char *qmetric = 0; 
  void *qtabsrc = 0, *sscriptsrc = 0;
  long qtabbytes = 0, sscriptbytes = 0;
  int qtabkind = DS_KIND_NONE, sscriptkind = DS_KIND_NONE;
  void *target = 0;
  int target_qmkind = 0; 
  char *target_qmetric = 0;
  int target_num_retries = 100; 
  long ltarget, jpeg_bytes, qclic_bytes;
  char bytes_reply[40]; 
  float ftarget; 
  Image *Im; 

  int do_rdopt = 1;
  int rd_coarseness = 10;
  long rd_thresh = 10; 
  float postol = -1, negtol = -1;
  int postolset = 0, negtolset = 0; 
  char *rd_maxtabsrc = 0; 
  long rd_maxtabbytes = 0;
  int rd_maxtabkind = DS_KIND_NONE;
  float rd_target = 0;
  int rd_target_kind = 0; 
  RD_Job rdjob;
  long nbytes = 0;
  RD_float bpp, psnr, rmse;
  Image TempImg, *rdimg;
  int using_TempImg = 0; 
  long planepixels=0; 
  DataDest rdqdest, *rdqdd = 0;




  /* get handle name */ 
  nextarg++;
  if (nextarg >= argc) {
    WriteReply(0,"No handle given: save-compressed failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: save-compressed failed", fd, -1);
    return;
  } 
  if ( !Images[handle].ImExists) { 
    WriteReply(0,"Image has been discarded: save-compressed failed", fd, handle);
    return;
  }
  Im = Images[handle].Im; 
  if ( ImAllSampleBytesEqual(Im) != 1) {
    WriteReply(0,"Can only compress 8-bit images", fd, handle);
    return;
  }
  
  if (++nextarg >= argc) {
    WriteReply(0,"No file name given: save-compressed failed", fd, handle);
    return;
  } 
  if (!strncmp(argv[nextarg],"-",1)) { 
    WriteReply(0,"Bad file name given: save-compressed failed", fd, handle);
    return;
  } 
  fname = argv[nextarg];

  /* get options */
  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-quickreturn", 5))  { 
      do_quick_return = 1;
    } else if (!strncmp(argv[nextarg],"-original", 5))  { 
      on_original = 1; 
    } else if (!strncmp(argv[nextarg],"-qclicfile", 5))  { 
      if (++nextarg >= argc) {
        WriteReply(0,"Missing qclicfile: save-compressed failed", fd, handle);
        return;
      } 
      if (!strncmp(argv[nextarg],"-",1)) { 
        WriteReply(0,"Bad qclicfile name given: save-compressed failed", fd, handle);
        return;
      } 
      qclicfname = argv[nextarg];
    } else if (!strncmp(argv[nextarg],"-qmetric", 5))  { 
      if (++nextarg >= argc) {
        WriteReply(0,"Missing qmetric: save-compressed failed", fd, handle);
        return;
      } 
      qmetric = argv[nextarg];
    } else if (!strncmp(argv[nextarg],"-qtables", 4))  { 
      if (++nextarg >= argc) {
        WriteReply(0,"Missing qtables: save-compressed failed", fd, handle);
        return;
      } 
      if (!strcmp(argv[nextarg],"-")) { 
	qtabsrc = (void *) &fd;
	qtabkind = DS_KIND_FD; 
      } else {
	qtabsrc = (void *) argv[nextarg]; 
	qtabkind = DS_KIND_FILE; 
      } 
      if (++nextarg >= argc) {
        WriteReply(0,"Missing #qtable-bytes: save-compressed failed", fd, handle);
        return;
      } 
      qtabbytes = (long) atoi(argv[nextarg]);
      if (qtabbytes  < 0) qtabbytes = 0;
      if ((qtabkind == DS_KIND_FD) && !qtabbytes) { 
        WriteReply(0,"Non-zero #qtabbytes needed: save-compressed failed", fd, handle);
        return;
      } 
    } else if (!strncmp(argv[nextarg],"-scans", 4))  { 
      if (++nextarg >= argc) {
        WriteReply(0,"Missing scanscript: save-compressed failed", fd, handle);
        return;
      } 
      if (!strcmp(argv[nextarg],"-")) { 
	sscriptsrc = (void *) &fd;
	sscriptkind = DS_KIND_FD; 
      } else {
	sscriptsrc = (void *) argv[nextarg]; 
	sscriptkind = DS_KIND_FILE; 
      } 
      if (++nextarg >= argc) {
        WriteReply(0,"Missing #scanscript-bytes: save-compressed failed", fd, handle);
        return;
      } 
      sscriptbytes = (long) atoi(argv[nextarg]);
      if (sscriptbytes  < 0) sscriptbytes = 0;
      if ((sscriptkind == DS_KIND_FD) && !sscriptbytes) { 
        WriteReply(0,"Non-zero #scanscript-bytes needed: save-compressed failed", fd, handle);
        return;
      } 
    } else if (!strncmp(argv[nextarg],"-maxtab", 5))  { 
      if (++nextarg >= argc) {
        WriteReply(0,"Missing maxtab: save-compressed failed", fd, handle);
        return;
      } 
      if (!strcmp(argv[nextarg],"-")) { 
	rd_maxtabsrc = (void *) &fd;
	rd_maxtabkind = DS_KIND_FD; 
      } else {
	rd_maxtabsrc = (void *) argv[nextarg]; 
	rd_maxtabkind = DS_KIND_FILE; 
      } 
      if (++nextarg >= argc) {
        WriteReply(0,"Missing #maxtab-bytes: save-compressed failed", fd, handle);
        return;
      } 
      rd_maxtabbytes = (long) atoi(argv[nextarg]);
      if (rd_maxtabbytes  < 0) rd_maxtabbytes = 0;
      if ((rd_maxtabkind == DS_KIND_FD) && !rd_maxtabbytes) { 
        WriteReply(0,"Non-zero #maxtab-bytes needed: save-compressed failed", fd, handle);
        return;
      } 
    } else if (!strncmp(argv[nextarg],"-target", 4))  { 
      if (++nextarg >= argc) {
        WriteReply(0,"Missing target qmetric: save-compressed failed", fd, handle);
        return;
      } 
      target_qmetric = argv[nextarg]; 
      if (++nextarg >= argc) {
        WriteReply(0,"Missing target: save-compressed failed", fd, handle);
        return;
      } 
      target_qmkind = QmetricKind(target_qmetric); 
      if (target_qmkind == QM_SIZE) {
	ltarget = (long) atoi(argv[nextarg]);
	target = (void *) &ltarget; 
      } else {
	sscanf(argv[nextarg],"%f",&ftarget);
	target = (void *) &ftarget; 
      } 
    } else if (!strncmp(argv[nextarg], "-rdtarget", 6)) {
      if (++nextarg >= argc) { 
        WriteReply(0,"Missing rdtarget qmetric: save-compressed failed", fd, handle);
        return;
      } 
      rd_target_kind = QmetricKind(argv[nextarg]); 
      if (++nextarg >= argc) { 
        WriteReply(0,"Missing target: save-compressed failed", fd, handle);
        return;
      } 
      sscanf(argv[nextarg],"%f",&rd_target); 
    } else if (!strncmp(argv[nextarg], "-ntries", 5)) {
      if (++nextarg >= argc) {
        WriteReply(0,"Missing target #retries: save-compressed failed", fd, handle);
        return;
      } 
      target_num_retries = atoi(argv[nextarg]); 
      if (target_num_retries < 0) target_num_retries = 0; 
    } else if (!strncmp(argv[nextarg], "-negtol", 5)) {
      if (++nextarg >= argc) {
        WriteReply(0,"Missing neg-tolerance: save-compressed failed", fd, handle);
        return;
      } 
      negtol = atof(argv[nextarg]); 
      negtolset = 1; 
    } else if (!strncmp(argv[nextarg], "-postol", 5)) {
      if (++nextarg >= argc) {
        WriteReply(0,"Missing pos-tolerance: save-compressed failed", fd, handle);
        return;
      } 
      postol = atof(argv[nextarg]); 
      postolset = 1; 
    } else if (!strncmp(argv[nextarg], "-rdopt", 5)) {
      do_rdopt = 1;
    } else if (!strncmp(argv[nextarg], "-nordopt", 5)) {
      do_rdopt = 0;
    } else if (!strncmp(argv[nextarg], "-rdcoarseness", 5)) {
      if (++nextarg >= argc) {
        WriteReply(0,"Missing rdopt coarseness: save-compressed failed", fd, handle);
        return;
      } 
      rd_coarseness = atoi(argv[nextarg]); 
      if (rd_coarseness < 0) rd_coarseness = 0; 
    } else if (!strncmp(argv[nextarg], "-rdthresh", 5)) {
      if (++nextarg >= argc) {
        WriteReply(0,"Missing rdopt thresholding param: save-compressed failed", fd, handle);
        return;
      } 
      rd_thresh = atoi(argv[nextarg]); 
      if (rd_thresh < 0) rd_thresh = 0; 
    } else { 
      WriteReply(0,"Bad option given: save-compressed failed", fd, handle);
      return;
    } 
  }

  if ( (do_quick_return) && (qtabkind != DS_KIND_FD) && 
       (sscriptkind != DS_KIND_FD) && (rd_maxtabkind != DS_KIND_FD) ) 
    WriteReply(1, "", fd, handle); 

  qclic_bytes =  0; 

  if (qtabsrc) do_rdopt = 0;
  if (ImIsCompressed(Im)) do_rdopt = 0; 
  if (!ImIsCompressed(Im) &&
       ImHasQclicCurve(Im) &&
       !strncasecmp(ImQclicCurveFormat(Im),"JPEG",4))
    do_rdopt = 0;
  
  if (do_rdopt) {

    using_TempImg = 0;
    if (on_original || !ImChanged(Im)) 
      planepixels = ((long) ImOrigRows(Im)) *
			    ImOrigCols(Im);
    else 
      planepixels = ((long) ImCurrRows(Im)) *
			    ImCurrCols(Im);

    if (Images[handle].nplanes == 3) {
      if (!on_original || !ImChanged(Im)) {
        rdimg = &TempImg;
	ImInitImg(rdimg);
	ImSetSilent(rdimg);
	ImDiscardOriginal(rdimg);
	ImSetKindRaw(rdimg, 3, ImCurrRows(Im),
	  ImCurrCols(Im), 1);
	if (!ImSetImgMemory(rdimg,
	       ImPlane(Im,0),
	      planepixels) ||
            !ImPeekImg(rdimg) || !ImReadImg(rdimg) ||
	    !ImSetImgMemory(rdimg,
	       ImPlane(Im,1),
	      planepixels) ||
	    !ImReadMoreImg(rdimg) ||
	    !ImSetImgMemory(rdimg,
	       ImPlane(Im,2),
	      planepixels) ||
	    !ImReadMoreImg(rdimg) ||
	    !ImChangeCspace(rdimg, ImC_YCbCr) ||
	    !ImStretchAndShrink(rdimg, 1, 1, 2, 1, 2, 0, 0) ||
	    !ImStretchAndShrink(rdimg, 2, 1, 2, 1, 2, 0, 0) ) {
	  ImFreeImg(rdimg);
	  goto do_compress;
	}
	using_TempImg = 1;
      } else {
        rdimg = &TempImg;
	ImInitImg(rdimg);
	ImSetSilent(rdimg);
	ImDiscardOriginal(rdimg);
	ImSetKindRaw(rdimg, 3, ImOrigRows(Im),
	  ImOrigCols(Im), 1);
	if (!ImSetImgMemory(rdimg,
	      (ImPlaneChanged(Im,0) ?
	       ImRetainedData(Im,0) :
	       (char *) ImPlane(Im,0) ),
	      planepixels) ||
            !ImPeekImg(rdimg) || !ImReadImg(rdimg) ||
	    !ImSetImgMemory(rdimg,
	      (ImPlaneChanged(Im,1) ?
	       ImRetainedData(Im,1) :
	       (char *) ImPlane(Im,1) ),
	      planepixels) ||
	    !ImReadMoreImg(rdimg) ||
	    !ImSetImgMemory(rdimg,
	      (ImPlaneChanged(Im,2) ?
	       ImRetainedData(Im,2) :
	       (char *) ImPlane(Im,2) ),
	      planepixels) ||
	    !ImReadMoreImg(rdimg) ||
	    !ImChangeCspace(rdimg, ImC_YCbCr) ||
	    !ImStretchAndShrink(rdimg, 1, 1, 2, 1, 2, 0, 0) ||
	    !ImStretchAndShrink(rdimg, 2, 1, 2, 1, 2, 0, 0) ) {
	  ImFreeImg(rdimg);
	  goto do_compress;
	}
	using_TempImg = 1;
      } 
    } else {
      if (!on_original || !ImChanged(Im))
	rdimg = Im;
      else {
        rdimg = &TempImg;
	ImInitImg(rdimg);
	ImSetSilent(rdimg);
	ImDiscardOriginal(rdimg);
	ImSetKindRaw(rdimg, 1, ImOrigRows(Im),
	  ImOrigCols(Im), 1);
	if (!ImSetImgMemory(rdimg, ImRetainedData(Im,0),
	      planepixels) ||
            !ImPeekImg(rdimg) || !ImReadImg(rdimg)) {
	  ImFreeImg(rdimg);
	  goto do_compress;
	}
	using_TempImg = 1;
      }
    }

    RD_Init(&rdjob, rdimg);


    if (rd_target <= 0) { 
      if (!target) {
        rd_target = 1.0;
        rd_target_kind = QM_BPP;
      } else {
        if (target_qmkind == QM_SIZE) {
	  rd_target = (float) ltarget;
          rd_target_kind = QM_SIZE;
        } else { 
	  rd_target = ftarget; 
          rd_target_kind = target_qmkind; 
	} 
        if (!QM_NEEDS_PSNR(rd_target_kind) && (target_num_retries > 1)
	      && (!postolset || (postol >= 0))) { 
           rd_target *= 1.05; /* try to ensure an over-estimate */
    } } }   

    if (rd_target_kind == QM_SIZE) {
      rd_target *= ((float)8.0/
	 ((float)ImCurrPlaneRows(rdimg,0)*ImCurrPlaneCols(rdimg,0))); 
      rd_target_kind = QM_BPP;
    } else if (rd_target_kind == QM_PSIZE) {
      rd_target *= ((float) 8.0* ((float) ImPlanesOfImg(rdimg))/100.0); 
      rd_target_kind = QM_BPP;
    } 

    RD_SetCoarseness(&rdjob, rd_coarseness);

    if (rd_maxtabsrc) {
      RD_SetMaxTables(&rdjob, 0, rd_maxtabsrc, rd_maxtabbytes, rd_maxtabkind);
    }

    #ifdef HAVE_IJG_EXTENSIONS
    RD_SetThreshSpan(&rdjob, rd_thresh);
    #endif
    
    if (!RD_Optimize(&rdjob)) {
      RD_Free(&rdjob); 
      if (using_TempImg) ImFreeImg(rdimg);
      goto do_compress; 
    }

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

    RD_Free(&rdjob);
    if (using_TempImg) ImFreeImg(rdimg);

    qtabsrc = DD_auto_mem(rdqdd);
    qtabbytes = DD_auto_mem_level(rdqdd);
    qtabkind = DS_KIND_MEMORY;

  }
 
  do_compress:

  if (target) { 
    if (QM_NEEDS_PSNR(target_qmkind)) {
      if (!negtolset) negtol = (float) 0.05; 
      if (!postolset) postol = (float) -1; 
    } else if (target_qmkind == QM_SIZE) {
      if (!negtolset) negtol = (float) 100; 
      if (!postolset) postol = (float) 0; 
    } else if (target_qmkind == QM_PSIZE) {
      if (!negtolset) negtol = (float) 1; 
      if (!postolset) postol = (float) 0; 
    } else if (target_qmkind == QM_BPP) {
      if (!negtolset) negtol = (float) 0.02; 
      if (!postolset) postol = (float) 0; 
    } 
  } 


  jpeg_bytes = ImSaveJpeg(Im, 
      on_original, target, target_qmetric,
      negtol, postol, 
      target_num_retries,
      fname, qclicfname, &qclic_bytes, qmetric,
      qtabsrc, qtabbytes, qtabkind, 
      sscriptsrc, sscriptbytes, sscriptkind);

  if (!jpeg_bytes) { 
    IM_return = 0; 
    WriteReply(0,"Could not SaveImg: save-compressed failed", fd, handle);
    return;
  } 

  if (rdqdd) {
    DD_Free_Auto_Mem(rdqdd);
    DD_Close(rdqdd);
  }

  sprintf(bytes_reply,"%ld %ld", jpeg_bytes, qclic_bytes); 

  WriteReply(1, bytes_reply, fd, handle); 

} 

static void ProcessAdd(int argc, char *argv[], int fd, int in_fd)
{ 
  char * imfname = 0; 
  int nextarg = 0;
  Image * Im;
  VR_ImId iid; 
  char *rough_idct = 0, *final_idct = 0; 
  int i, iheight,  do_quick_return =0;
  int image_follows =0; 
  int ccube, maxc, handle;
  int private_cmap; 
  int do_dither;
  unsigned long cflag; 
  long nbytes = 0, nbytes_qual = 0;
  long sync = 0, remaining = 0;
  int nloops, read_step, show_row_start, show_row_end; 
  int qual_given = 0; 
  float quality, curr_qual; 
  int old_imagefile = 0;
  int new_trans = 0; 
  VR_TransId new_tid; 
  int needXimage;
  

  /* get handle name */ 
  nextarg++;
  if (nextarg >= argc) {
    WriteReply(0,"No handle given: add failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: add failed", fd, -1);
    return;
  } 

  if (!Images[handle].ImExists) {  
    WriteReply(0,"Image already complete/-noops was specified: add failed", fd, handle);
    return;
  } 


  Im = Images[handle].Im;

  if (ImStateOfImg(Im) != ImStateReadSome) {
    WriteReply(0,"Cannot add data to this image", fd, handle);
    return;
  } 

  /* get file name */ 
  nextarg++;
  if (nextarg >= argc) {
    WriteReply(0,"No image data given: add failed", fd, handle);
    return;
  } 

  if (!strcmp(argv[nextarg],"-")) {
    image_follows = 1; 
    if (in_fd < 0) {
      WriteReply(0,"Cannot receive image from here: add failed", fd, handle);
      return;
    } 
  } else if (!strcmp(argv[nextarg],".")) {
    if (ImUserGivenFd(Im) && !ImStdinSrc(Im)) {
      WriteReply(0,"No previous image file: add failed", fd, handle);
      return;
    } 
    old_imagefile = 1;
  } else {
    imfname = argv[nextarg];
  } 

  
  /* get options */
  do_dither = Images[handle].do_dither;
  maxc = Images[handle].maxc;
  ccube = Images[handle].ccube;
  private_cmap = Images[handle].private_cmap;

  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-ccube",3)) { 
      nextarg++;
      if (nextarg >= argc) {
	WriteReply(0, "Missing value for -ccube: add failed",fd, handle); 
	return; 
      }
      ccube = atoi(argv[nextarg]);
    } else if (!strncmp(argv[nextarg],"-maxc",4)) {
      nextarg++;
      if (nextarg >= argc) {
	WriteReply(0, "Missing value for -maxc: add failed",fd, handle); 
	return; 
      } 
      maxc = atoi(argv[nextarg]); 
    } else if (!strncmp(argv[nextarg],"-idct",4)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -idct: add failed",fd, -1); 
	return; 
      } 
      rough_idct = argv[nextarg]; 
    } else if (!strncmp(argv[nextarg],"-fidct",5)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -fidct: add failed",fd, -1); 
	return; 
      } 
      final_idct = argv[nextarg]; 
    } else if (!strncmp(argv[nextarg],"-rbytes",3)) {
      nextarg++;
      if (nextarg >= argc) {
	WriteReply(0, "Missing value for -rbytes: add failed",fd, handle); 
	return; 
      } 
      sscanf(argv[nextarg],"%ld",&nbytes);
    } else if (!strncmp(argv[nextarg],"-sync",3)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -sync: add failed",fd, -1); 
	return; 
      } 
      sscanf(argv[nextarg],"%ld",&sync);
    } else if (!strncmp(argv[nextarg],"-quality",5)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -quality: add failed",fd, handle); 
	return; 
      } 
      quality = (float) 0; 
      if (!strncmp(argv[nextarg],"%",1)) {
	sscanf(argv[nextarg],"%%%f",&quality);
	qual_given = 2;
      } else {
        sscanf(argv[nextarg],"%f",&quality);
	qual_given = 1;
      }
      if (quality <= (float) 0) {
	WriteReply(0, "Value for -quality not positive: add failed",fd, handle); 
	return; 
      } 
    } else if (!strncmp(argv[nextarg],"-dither",4)) {
      do_dither = 1; 
    } else if (!strncmp(argv[nextarg],"-nodither",4)) {
      do_dither = 0; 
    } else if (!strncmp(argv[nextarg],"-remap",4)) {
      new_trans = 1; 
    } else if (!strncmp(argv[nextarg],"-privcolors",4)) {
      private_cmap = 1; 
    } else if (!strncmp(argv[nextarg],"-noprivcolors",5)) {
      private_cmap = 0; 
    } else if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1; 
    } else {
      WriteReply(0, "Bad option given: add failed",fd, handle); 
      return; 
    }
  }

  if (image_follows) {
    qual_given = 0; 
    if (!nbytes && !ImTotalBytes(Im)) { 
      WriteReply(0, "Do not know the number of bytes to read! use -rbytes", fd, handle);
      return;
    }
  }
  if (qual_given /* && !image_follows */ ) {
    curr_qual = ImQclicIcurveQofS(Im, ImBytesRead(Im), (qual_given==2));
    nbytes_qual = ImQclicIcurveSofQ(Im, quality + curr_qual, (qual_given==2));
    if (nbytes_qual > 0) { 
      nbytes_qual -= ImBytesRead(Im); 
      if (nbytes_qual > 0) {
        if ((nbytes == 0) || (nbytes > nbytes_qual)) {
  	  nbytes = nbytes_qual; 
        }
      } else {
        WriteReply(0, "Image already has the specified quality", fd, handle);
        return;
      } 
    }
  } 
  if (do_quick_return && !image_follows) {
    WriteReply(1, "", fd, handle); 
  }
  
  if ((ccube != Images[handle].ccube) || 
      (maxc != Images[handle].maxc) || 
      (private_cmap != Images[handle].private_cmap))
	new_trans = 1;

  if (image_follows) {
    IM_return = ImSetImgFd(Im, in_fd, nbytes);
  } else if (old_imagefile) {
    IM_return = ImExtendImgBytes(Im, nbytes);
  } else {
    if (!(IM_return = ImSetImgFile(Im, imfname, nbytes))) {
      WriteReply(0,"Could not open image file", fd, handle);
      return;
    }
  }
  
  VR_ShowWaitCursor(Images[handle].wid); 
  
  /***** now figure out how much of the image to read *****/

  nbytes = DS_bytes_to_read(&(Im->ImgSrc));
  nbytes -= DS_bytes_read(&(Im->ImgSrc)); 
  if (nbytes <= 0) {
    nbytes = DS_total_in_src(&(Im->ImgSrc)) - 
                            DS_total_count(&(Im->ImgSrc)); 
  }
  if (nbytes < 0) nbytes = 0; 

  if ( strcmp(ImFilterOfImg(Im),"none") || (sync==0) ) {
    sync = nbytes; 
  } else {
    if (sync < 0) {
      /* use a heuristic for now */
      if (ImTotalBytes(Im) > 0) {
	sync = ImTotalBytes(Im) * 20 / 100;
      } else { 
        sync = ((long) Im->NumCols)*Im->NumRows*20
	  *ImPlanesOfImg(Im)*ImSampleBytes(Im)/100; 
        if (ImIsCompressed(Im)) {
  	  /* 10:1 compression seems like a reasonable assumption */
	  sync /= 10; 
        }
      } 
    } 
    if (sync <= 0) sync = 100; 
    if ((nbytes > 0) && (sync > nbytes)) sync = nbytes; 
  } 

  if (sync == nbytes) read_step = 0; 
  else {
    if (!DS_ResetSrcBytes(&(Im->ImgSrc),
        sync + DS_bytes_read(&(Im->ImgSrc)))) { 
      WriteReply(0, "Error handling image source: add failed",fd, -1); 
      VR_ShowNormalCursor(Images[handle].wid); 
      return; 
    }
    read_step = 1; 
  }

  if (ImKindSubSetOf(ImKindOfImg(Im),IM_JPEG)) {
    if (rough_idct) {
      if (!strncasecmp(rough_idct,"float",2))
	ImSetRoughIDCT(Im, JDCT_FLOAT); 
      else if (!strncasecmp(rough_idct,"islow",2))
	ImSetRoughIDCT(Im, JDCT_ISLOW); 
      else if (!strncasecmp(rough_idct,"ifast",2))
	ImSetRoughIDCT(Im, JDCT_IFAST); 
    } 
    if (final_idct) {
      if (!strncasecmp(final_idct,"float",2))
	ImSetFinalIDCT(Im, JDCT_FLOAT); 
      else if (!strncasecmp(final_idct,"islow",2))
	ImSetFinalIDCT(Im, JDCT_ISLOW); 
      else if (!strncasecmp(final_idct,"ifast",2))
	ImSetFinalIDCT(Im, JDCT_IFAST); 
    } 
  }

  if (!(IM_return = ImReadMoreImg(Im))) {
    WriteReply(0,"Reading more image data failed", fd, handle);
    VR_ShowNormalCursor(Images[handle].wid); 
    return;
  }
  
  iid = Images[handle].iid; 

  if (do_quick_return && image_follows && !read_step) {
    WriteReply(1, "", fd, handle); 
  }


  CopyImgToIid(Im, iid);



  if (new_trans) { 
    cflag = ccubeToflag(ccube); 
    VR_CFL_SET_NCOLORS(cflag, maxc);
    if (private_cmap)
      cflag |= VR_CFL_PRIVATE;

    new_tid = VR_GetTranslatorFromCM(iid, cflag,
      VR_Colormaps[VR_Translators[Images[handle].tid].VR_cmap].cmap);
    if (new_tid >= 0) { 
       if (VR_ChangeTranslator(Images[handle].wid, new_tid)
	    == VR_SUCCESS) {
	  Images[handle].tid = new_tid;
       } else {
	 VR_FreeTranslator(new_tid); 
	 new_trans = 0; 
       }
    } else new_trans = 0;  
  }
  

  iheight = Images[handle].oheight;

  if (Im->WasCropped) { 
    show_row_start = 0;
    show_row_end = Images[handle].height - 1;
  } else {
    show_row_start = ImFirstChanged(Im, 0); 
    show_row_end = ImLastChanged(Im, 0);
  
    if (Images[handle].height < iheight) {
      show_row_start = show_row_start * Images[handle].height/ iheight; 
      show_row_end = show_row_end * Images[handle].height/ iheight; 
    } else if (Images[handle].height > iheight) {
      show_row_start = (show_row_start * Images[handle].height/ iheight) - 1 -
         (Images[handle].height/iheight); 
      if (show_row_start < 0) show_row_start = 0; 
      show_row_end = (show_row_end * Images[handle].height/ iheight) + 1 + 
         (Images[handle].height/iheight); 
      if (show_row_end >= Images[handle].height) show_row_end = Images[handle].height - 1; 
    }
  }

  VR_ShowImageRect(iid, Images[handle].wid,
    0, show_row_start, Images[handle].width,
    show_row_end - show_row_start + 1, 
    do_dither);

  Images[handle].do_dither = do_dither;

  if (new_trans) { 
    Images[handle].maxc = maxc;
    Images[handle].ccube = ccube;
    Images[handle].private_cmap = private_cmap;
  } 


  if (Images[handle].CtrlExists) RefreshControl(handle);


  if (read_step) { 
    if (nbytes == 0) {
      nloops = -1;
    } else {
      nbytes -= sync; 
      nloops = nbytes/sync;
      remaining = nbytes - (nloops*sync); 
      if (remaining) nloops++;
      else remaining = sync;
    } 
    i = 0;
    while (((nloops < 0) || (i < nloops)) && 
	   !DS_file_eof(&(Im->ImgSrc)) &&
	   (ImStateOfImg(Im) == ImStateReadSome)) { 
      if (i==(nloops-1)) sync = remaining; 
      if (!DS_ExtendSrc(&(Im->ImgSrc), sync)) break;
      if (!ImReadMoreImg(Im)) break; 
      CopyImgToIid(Im, iid);

      if (Im->WasCropped) { 
	show_row_start = 0; 
	show_row_end = Images[handle].height - 1;
      } else {
        show_row_start = ImFirstChanged(Im, 0);
        show_row_end = ImLastChanged(Im, 0);
	if (Images[handle].height < iheight) {
          show_row_start = show_row_start * Images[handle].height/ iheight; 
          show_row_end = show_row_end * Images[handle].height/ iheight; 
        } else if (Images[handle].height > iheight) {
          show_row_start = (show_row_start * Images[handle].height/ iheight) - 1 -
             (Images[handle].height/iheight); 
          if (show_row_start < 0) show_row_start = 0; 
          show_row_end = (show_row_end * Images[handle].height/ iheight) + 1 + 
             (Images[handle].height/iheight); 
          if (show_row_end >= Images[handle].height) show_row_end = Images[handle].height - 1; 
        }
      }

      VR_ShowImageRect(iid, Images[handle].wid,
        0, show_row_start, Images[handle].width,
	show_row_end - show_row_start + 1, 
        do_dither);

      if (Images[handle].CtrlExists) RefreshControl(handle); 
      i++; 
    }  
  }

  WriteReply(1, "", fd, handle); 

  needXimage = Images[handle].am_refreshing; /* ||
     (ImStateOfImg(Im) == ImStateReadSome); */ 

  if (!needXimage) VR_FreeWindowXImage(Images[handle].wid);
  VR_ShowNormalCursor(Images[handle].wid); 

} 



static void ProcessReplace(int argc, char *argv[], int fd, int in_fd)
{ 
  char * imfname = 0; 
  int nextarg = 0;
  Image * Im, *OldIm;
  VR_ImId iid; 
  char *rough_idct = 0, *final_idct = 0; 
  int i, do_quick_return =0;
  int image_follows =0; 
  int ccube, maxc, handle;
  int private_cmap; 
  int do_dither;
  unsigned long cflag; 
  int qual_given = 0; 
  float quality; 
  
  int new_trans = 0; 
  VR_TransId new_tid; 
  int needXimage;
  int do_crop = 0, sc = 0, sr = 0, ec = 0, er = 0; 
  int cp_qclic = 0;
  
  int iwidth=512, iheight=512, nplanes=1; 
  char * title = (char *) 0; 
  long nbytes = 0, nbytes_tot = 0, nbytes_qual = 0;
  long nbytes_read; 
  long show_after = 0, sync = 0, remaining = 0; 
  int nloops, show_row_start, show_row_end, read_step;
  int ImKind = IM_NOT_RAW;

  /* get handle name */ 
  nextarg++;
  if (nextarg >= argc) {
    WriteReply(0,"No handle given: replace failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: replace failed", fd, -1);
    return;
  } 

  /* get file name */ 
  nextarg++;
  if (nextarg >= argc) {
    WriteReply(0,"No image data given: replace failed", fd, handle);
    return;
  } 

  if (!strcmp(argv[nextarg],"-")) {
    image_follows = 1; 
    if (in_fd < 0) {
      WriteReply(0,"Cannot receive image from here: replace failed", fd, handle);
      return;
    } 
  } else {
    imfname = argv[nextarg];
  } 

  
  /* get options */
  do_dither = Images[handle].do_dither;
  maxc = Images[handle].maxc;
  ccube = Images[handle].ccube;
  private_cmap = Images[handle].private_cmap;

  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-ccube",3)) { 
      nextarg++;
      if (nextarg >= argc) {
	WriteReply(0, "Missing value for -ccube: replace failed",fd, handle); 
	return; 
      }
      ccube = atoi(argv[nextarg]);
    } else if (!strncmp(argv[nextarg],"-maxc",4)) {
      nextarg++;
      if (nextarg >= argc) {
	WriteReply(0, "Missing value for -maxc: replace failed",fd, handle); 
	return; 
      } 
      maxc = atoi(argv[nextarg]); 
    } else if (!strncmp(argv[nextarg],"-idct",4)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -idct: replace failed",fd, -1); 
	return; 
      } 
      rough_idct = argv[nextarg]; 
    } else if (!strncmp(argv[nextarg],"-fidct",5)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -fidct: replace failed",fd, -1); 
	return; 
      } 
      final_idct = argv[nextarg]; 
    } else if (!strncmp(argv[nextarg],"-title",4)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -title: replace failed",fd, handle); 
	return; 
      } 
      title = argv[nextarg]; 
    } else if (!strncmp(argv[nextarg],"-bytes",2)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -bytes: replace failed",fd, handle); 
	return; 
      } 
      sscanf(argv[nextarg],"%ld",&nbytes_tot);
    } else if (!strncmp(argv[nextarg],"-rbytes",3)) {
      nextarg++;
      if (nextarg >= argc) {
	WriteReply(0, "Missing value for -rbytes: replace failed",fd, handle); 
	return; 
      } 
      sscanf(argv[nextarg],"%ld",&nbytes);
    } else if (!strncmp(argv[nextarg],"-showafter",6)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -showafter: replace failed",fd, -1); 
	return; 
      } 
      sscanf(argv[nextarg],"%ld",&show_after);
    } else if (!strncmp(argv[nextarg],"-sync",3)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -sync: replace failed",fd, -1); 
	return; 
      } 
      sscanf(argv[nextarg],"%ld",&sync);
    } else if (!strncmp(argv[nextarg],"-quality",5)) {
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing value for -quality: replace failed",fd, handle); 
	return; 
      } 
      quality = (float) 0; 
      if (!strncmp(argv[nextarg],"%",1)) {
	sscanf(argv[nextarg],"%%%f",&quality);
	qual_given = 2;
      } else {
        sscanf(argv[nextarg],"%f",&quality);
	qual_given = 1;
      }
      if (quality <= (float) 0) {
	WriteReply(0, "Value for -quality not positive: replace failed",fd, handle); 
	return; 
      } 
    } else if (!strncmp(argv[nextarg],"-raw",4)) {
      ImKind = IM_RAW; 
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing width for -raw: replace failed",fd, handle); 
	return; 
      } 
      iwidth = atoi(argv[nextarg]); 
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing height for -raw: replace failed",fd, handle); 
	return; 
      } 
      iheight = atoi(argv[nextarg]); 
      nextarg++; 
      if (nextarg >= argc) { 
	WriteReply(0, "Missing num-planes for -raw: replace failed",fd, handle); 
	return; 
      } 
      nplanes = atoi(argv[nextarg]); 
      if ((nplanes != 1) && (nplanes != 3)) { 
	WriteReply(0, "Num-planes for -raw must be 1 or 3: replace failed",fd, handle); 
	return; 
      } 
    } else if (!strncmp(argv[nextarg],"-dither",4)) {
      do_dither = 1; 
    } else if (!strncmp(argv[nextarg],"-nodither",4)) {
      do_dither = 0; 
    } else if (!strncmp(argv[nextarg],"-remap",4)) {
      new_trans = 1; 
    } else if (!strncmp(argv[nextarg],"-privcolors",4)) {
      private_cmap = 1; 
    } else if (!strncmp(argv[nextarg],"-noprivcolors",5)) {
      private_cmap = 0; 
    } else if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1; 
    } else if (!strncmp(argv[nextarg],"-qclicsame",5)) {
      cp_qclic = 1;
    } else {
      WriteReply(0, "Bad option given: replace failed",fd, handle); 
      return; 
    }
  }

  if (image_follows) {
    qual_given = 0; 
    /* 
    if (qual_given) { 
      WriteReply(0, "Cannot use -quality for reading image data off the connection", fd, handle);
      return;
    } 
    */ 
  }

  Im = (Image *) malloc(sizeof(Image));
  
  if (!Im) { 
    WriteReply(0,"Out of memory: replace failed", fd, handle);
    return;
  }

  if (do_quick_return && !image_follows) {
    WriteReply(1, "", fd, handle); 
  }
  
  ImInitImg(Im); 

  if (cp_qclic && Images[handle].ImExists)
    ImCopyQclicInfo(Im, Images[handle].Im);

  ImSetSilent(Im);
  if (LogFile) ImSetErrStream(Im, LogFile);
  else ImSetErrStream(Im, stderr);

  if (ImKind == IM_RAW)
    (void) ImSetKindRaw(Im, nplanes, iheight, iwidth, FALSE);
  else
    (void) ImSetKind(Im, ImKind, FALSE);


  ImSetTotalBytes(Im, nbytes_tot, (nbytes_tot == 0)); 

  if (image_follows) {
    IM_return = ImSetImgFd(Im, in_fd, nbytes);
  } else {
    if (!(IM_return = ImSetImgFile(Im, imfname, nbytes))) {
      WriteReply(0,"Could not open image file", fd, handle);
      ImFreeImg(Im);
      free(Im); 
      return;
    }
  }
  
  VR_ShowWaitCursor(Images[handle].wid); 

  if (!(IM_return = ImPeekImg(Im)) || (ImKindOfImg(Im) == IM_UNKNOWN)) {
      WriteReply(0, "Error reading image: replace failed",fd, handle); 
      ImFreeImg(Im);
      free(Im); 
      VR_ShowNormalCursor(Images[handle].wid); 
      return; 
  } 
  
  if (ImKindSubSetOf(ImKindOfImg(Im),IM_JPEG)) {
    if (rough_idct) {
      if (!strncasecmp(rough_idct,"float",2))
	ImSetRoughIDCT(Im, JDCT_FLOAT); 
      else if (!strncasecmp(rough_idct,"islow",2))
	ImSetRoughIDCT(Im, JDCT_ISLOW); 
      else if (!strncasecmp(rough_idct,"ifast",2))
	ImSetRoughIDCT(Im, JDCT_IFAST); 
    } 
    if (final_idct) {
      if (!strncasecmp(final_idct,"float",2))
	ImSetFinalIDCT(Im, JDCT_FLOAT); 
      else if (!strncasecmp(final_idct,"islow",2))
	ImSetFinalIDCT(Im, JDCT_ISLOW); 
      else if (!strncasecmp(final_idct,"ifast",2))
	ImSetFinalIDCT(Im, JDCT_IFAST); 
    } 
  }

  if (title) ImSetName(Im, title);

  if (image_follows) {
    if (!nbytes && !ImTotalBytes(Im)) { 
      WriteReply(0, "Do not know the number of bytes to read! use -bytes or -rbytes or a Qclic image", fd, handle);
      ImFreeImg(Im);
      free(Im); 
      VR_ShowNormalCursor(Images[handle].wid); 
      return;
    }
  }
  if (qual_given /* && !image_follows */ ) {
    nbytes_qual = ImQclicIcurveSofQ(Im, quality, (qual_given==2));
    if (nbytes_qual > 0) {
      if (ImBytesRead(Im) > nbytes_qual) nbytes_qual = ImBytesRead(Im);
      if ((nbytes == 0) || (nbytes > nbytes_qual)) {
	if (!(IM_return = ImSetImgBytes(Im, nbytes_qual))) {
          WriteReply(0, "Error using Qclic info: replace failed",fd, handle); 
          ImFreeImg(Im);
          free(Im); 
          VR_ShowNormalCursor(Images[handle].wid); 
          return; 
	}
      }
    }
  } 

  iwidth = ImOrigCols(Im);
  iheight = ImOrigRows(Im);
  nplanes = ImPlanesOfImg(Im);
  
  if (nplanes != Images[handle].nplanes) { 
      WriteReply(0, "Image-num-planes mismatch: replace failed",fd, handle); 
      ImFreeImg(Im);
      free(Im); 
      VR_ShowNormalCursor(Images[handle].wid); 
      return; 
  }

  /***** now figure out how much of the image to read *****/

  nbytes_read = DS_bytes_read(&(Im->ImgSrc)); 
  nbytes = DS_bytes_to_read(&(Im->ImgSrc));
  if (nbytes == 0) nbytes = DS_total_in_src(&(Im->ImgSrc)) -
    (DS_total_count(&(Im->ImgSrc)) - nbytes_read) ; 

  if ( strcmp(ImFilterOfImg(Im),"none") ||
       ((show_after==0) && (sync==0)) ) {
    show_after = nbytes; 
  } else {
    if (show_after == 0) show_after = sync; 
    if (show_after < 0) {
      /* use a heuristic for now */
      if (ImTotalBytes(Im) > 0) {
	show_after = ImTotalBytes(Im) * 40 / 100;
      } else { 
        show_after = ((long) iwidth)*iheight*40
	  *nplanes*ImSampleBytes(Im)/100; 
        if (ImIsCompressed(Im)) {
  	  /* 10:1 compression seems like a reasonable assumption */
	  show_after /= 10; 
        }
      } 
    } 
    if (show_after < (nbytes_read + 100)) 
      show_after = nbytes_read + 100; 
    if ((nbytes > 0) && (show_after > nbytes)) show_after = nbytes; 
    if (sync <= 0) sync = show_after/2;  
  } 

  if (show_after == nbytes) read_step = 0; 
  else {
    if (!DS_ResetSrcBytes(&(Im->ImgSrc), show_after)) { 
      WriteReply(0, "Error handling image source: replace failed",fd, -1); 
      VR_ShowNormalCursor(Images[handle].wid); 
      ImFreeImg(Im);
      free(Im); 
      return; 
    }
    read_step = 1; 
  }

  if (!(IM_return = ImReadImg(Im))) {
      WriteReply(0, "Error reading image: replace failed",fd, handle); 
      ImFreeImg(Im);
      free(Im); 
      VR_ShowNormalCursor(Images[handle].wid); 
      return; 
  }

  OldIm = Images[handle].Im;
  Images[handle].Im = Im;

  if (do_quick_return && image_follows && !read_step) {
    WriteReply(1, "", fd, handle); 
  }

  if (Images[handle].ImExists) {
    ImGetCropInfo(Images[handle].Im, do_crop, sr, sc, er, ec);
    if (do_crop && (sr >= 0) && (sc >= 0) &&
		   (er < iheight) && (ec < iwidth)) {
      if (!(IM_return = ImCrop(Im, sr, sc, er, ec))) {
	Images[handle].Im = OldIm; 
        WriteReply(0,"Crop-image failed: could not replace", fd, handle);
        ImFreeImg(Im);
        free(Im); 
        VR_ShowNormalCursor(Images[handle].wid); 
        return;
      }
    } 
  }
  if (!(IM_return =
    ImSetImgDim(Im, -1, Images[handle].height, Images[handle].width,
          Images[handle].do_smooth, 0))) { 
        WriteReply(0,"Resize-image failed: could not replace", fd, handle);
	Images[handle].Im = OldIm; 
        ImFreeImg(Im);
        free(Im); 
        VR_ShowNormalCursor(Images[handle].wid); 
        return;
  }

  iid = Images[handle].iid; 

  CopyImgToIid(Im, iid);

  if ((ccube != Images[handle].ccube) || 
      (maxc != Images[handle].maxc) || 
      VR_TransLacksTable(Images[handle].tid) || 
      (private_cmap != Images[handle].private_cmap))
	new_trans = 1;



  if (new_trans) { 
    cflag = ccubeToflag(ccube); 
    VR_CFL_SET_NCOLORS(cflag, maxc);
    if (private_cmap)
      cflag |= VR_CFL_PRIVATE;

    new_tid = VR_GetTranslatorFromCM(iid, cflag,
      VR_Colormaps[VR_Translators[Images[handle].tid].VR_cmap].cmap);
    if (new_tid >= 0) { 
       if (VR_ChangeTranslator(Images[handle].wid, new_tid)
	    == VR_SUCCESS) {
	  Images[handle].tid = new_tid;
       } else {
	 VR_FreeTranslator(new_tid); 
	 new_trans = 0; 
       }
    } else new_trans = 0;  
  }

  if (!new_trans && VR_TransLacksTable(Images[handle].tid)) { 
	VR_return = VR_SEE_ERR_MSG; 
        WriteReply(0,"Replace failed: could not get translator", fd, handle);
	Images[handle].Im = OldIm; 
        ImFreeImg(Im);
        free(Im); 
        VR_ShowNormalCursor(Images[handle].wid); 
        return;
  }



  VR_ShowImage(iid, Images[handle].wid, do_dither);

  Images[handle].do_dither = do_dither;
  strcpy(Images[handle].Name, ImNameOfImg(Im));

  if (new_trans) { 
    Images[handle].maxc = maxc;
    Images[handle].ccube = ccube;
    Images[handle].private_cmap = private_cmap;
  } 
  
  Images[handle].owidth = ImOrigCols(Im);
  Images[handle].oheight = ImOrigRows(Im);

  strcpy(Images[handle].format,ImKindString(ImKindOfImg(Im)));
  strcpy(Images[handle].filter,ImFilterOfImg(Im));

  Images[handle].aspected = ((((float) Images[handle].width)/
			      ((float) iwidth)) == 
                             (((float) Images[handle].height)/
			      ((float) Images[handle].oheight)));

  if (Images[handle].CtrlExists) InvokeControl(handle);

  if (read_step) { 
    if (nbytes == 0) {
      nloops = -1;
    } else {
      nbytes -= show_after;
      nloops = nbytes/sync;
      remaining = nbytes - (nloops*sync); 
      if (remaining) nloops++;
      else remaining = sync;
    } 
    i = 0;
    while (((nloops < 0) || (i < nloops)) && 
	   !DS_file_eof(&(Im->ImgSrc)) &&
	   (ImStateOfImg(Im) == ImStateReadSome)) { 
      if (i==(nloops-1)) sync = remaining; 
      if (!DS_ExtendSrc(&(Im->ImgSrc), sync)) break;
      if (!ImReadMoreImg(Im)) break; 
      CopyImgToIid(Im, iid);

      if (Im->WasCropped) { 
	show_row_start = 0; 
	show_row_end = Images[handle].height - 1; 
      } else {
        show_row_start = ImFirstChanged(Im, 0);
        show_row_end = ImLastChanged(Im, 0);
	if (Images[handle].height < iheight) {
          show_row_start = show_row_start * Images[handle].height/ iheight; 
          show_row_end = show_row_end * Images[handle].height/ iheight; 
        } else if (Images[handle].height > iheight) {
          show_row_start = (show_row_start * Images[handle].height/ iheight) - 1 -
             (Images[handle].height/iheight); 
          if (show_row_start < 0) show_row_start = 0; 
          show_row_end = (show_row_end * Images[handle].height/ iheight) + 1 + 
             (Images[handle].height/iheight); 
          if (show_row_end >= Images[handle].height) show_row_end = Images[handle].height - 1; 
        }
      }

      VR_ShowImageRect(iid, Images[handle].wid,
        0, show_row_start, Images[handle].width,
	show_row_end - show_row_start + 1, 
        do_dither);

      if (Images[handle].CtrlExists) RefreshControl(handle); 
      i++; 
    }  
  }

  WriteReply(1, "", fd, handle); 


  if (!Images[handle].ImExists) {
    Images[handle].Im = (Image *) 0; 
    ImFreeImg(Im);
    free(Im);
    VR_FreeTransTable(Images[handle].tid);
  } else {
    ImFreeImg(OldIm); 
    free(OldIm); 
  }


  needXimage = Images[handle].am_refreshing; /* ||
     (ImStateOfImg(Im) == ImStateReadSome); */ 

  if (!needXimage) VR_FreeWindowXImage(Images[handle].wid);

  VR_ShowNormalCursor(Images[handle].wid); 

} 


static void ProcessReshow(int argc, char *argv[], int fd)
{ 
  int nextarg = 0;
  Image * Im;
  VR_ImId iid; 
  int do_quick_return =0;
  int ccube, maxc, handle;
  int private_cmap; 
  int do_dither;
  unsigned long cflag; 
  int new_trans = 0; 
  VR_TransId new_tid; 
  int needXimage;
  

  /* get handle name */ 
  nextarg++;
  if (nextarg >= argc) {
    WriteReply(0,"No handle given: reshow failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: reshow failed", fd, -1);
    return;
  } 

  if (!Images[handle].ImExists) {  
    WriteReply(0,"Image already thrown away: reshow failed", fd, handle);
    return;
  } 

  
  Im = Images[handle].Im;

  
  /* get options */
  do_dither = Images[handle].do_dither;
  maxc = Images[handle].maxc;
  ccube = Images[handle].ccube;
  private_cmap = Images[handle].private_cmap;
  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-ccube",3)) { 
      nextarg++;
      if (nextarg >= argc) {
	WriteReply(0, "Missing value for -ccube: reshow failed",fd, handle); 
	return; 
      }
      ccube = atoi(argv[nextarg]);
    } else if (!strncmp(argv[nextarg],"-maxc",4)) {
      nextarg++;
      if (nextarg >= argc) {
	WriteReply(0, "Missing value for -maxc: reshow failed",fd, handle); 
	return; 
      } 
      maxc = atoi(argv[nextarg]); 
    } else if (!strncmp(argv[nextarg],"-dither",4)) {
      do_dither = 1; 
    } else if (!strncmp(argv[nextarg],"-nodither",4)) {
      do_dither = 0; 
    } else if (!strncmp(argv[nextarg],"-remap",4)) {
      new_trans = 1; 
    } else if (!strncmp(argv[nextarg],"-privcolors",4)) {
      private_cmap = 1; 
    } else if (!strncmp(argv[nextarg],"-noprivcolors",5)) {
      private_cmap = 0; 
    } else if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1; 
    } else {
      WriteReply(0, "Bad option given: reshow failed",fd, handle); 
      return; 
    }
  }

  if (do_quick_return) {
    WriteReply(1, "", fd, handle); 
  }
  
  if ((ccube != Images[handle].ccube) || 
      (maxc != Images[handle].maxc) || 
      (private_cmap != Images[handle].private_cmap))
	new_trans = 1;

  
  iid = Images[handle].iid; 

  VR_ShowWaitCursor(Images[handle].wid); 

  if (new_trans) { 
    cflag = ccubeToflag(ccube); 
    VR_CFL_SET_NCOLORS(cflag, maxc);
    if (private_cmap) 
      cflag |= VR_CFL_PRIVATE; 
    new_tid = VR_GetTranslatorFromCM(iid, cflag,
      VR_Colormaps[VR_Translators[Images[handle].tid].VR_cmap].cmap);
    if (new_tid >= 0) { 
       if (VR_ChangeTranslator(Images[handle].wid, new_tid)
	    == VR_SUCCESS) {
	  Images[handle].tid = new_tid;
       } else {
	 VR_FreeTranslator(new_tid); 
	 new_trans = 0; 
       }
    } else new_trans = 0; 
  }
  
  Images[handle].do_dither = do_dither;

  if (new_trans) { 
    Images[handle].maxc = maxc;
    Images[handle].ccube = ccube;
    Images[handle].private_cmap = private_cmap;
  }

  WriteReply(1, "", fd, handle); 

  VR_ShowImage(iid, Images[handle].wid, do_dither);

  needXimage = Images[handle].am_refreshing; /* ||
     (ImStateOfImg(Im) == ImStateReadSome); */ 

  if (!needXimage) VR_FreeWindowXImage(Images[handle].wid);

  VR_ShowNormalCursor(Images[handle].wid); 
  if (Images[handle].CtrlExists) RefreshControl(handle);
} 


static void ProcessBrighten(int argc, char *argv[], int fd)
{ 
  int nextarg = 0;
  Image * Im;
  VR_ImId iid; 
  int do_quick_return =0;
  int ccube, maxc, handle;
  int private_cmap; 
  int do_dither;
  unsigned long cflag; 
  int new_trans = 0; 
  VR_TransId new_tid; 
  int needXimage;
  int bright;
  int pnum = -1;
  int on_ycc = 0;
  


  /* get handle name */ 
  if (++nextarg >= argc) {
    WriteReply(0,"No handle given: brighten failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: brighten failed", fd, -1);
    return;
  } 

  if (!Images[handle].ImExists) {  
    WriteReply(0,"Image already thrown away: brighten failed", fd, handle);
    return;
  } 

  /* get brightness */
  if (++nextarg >= argc) {
    WriteReply(0,"No brightness given: brighten failed", fd, handle);
    return;
  } 
  bright = atoi(argv[nextarg]);
  

  Im = Images[handle].Im;

  
  /* get options */
  do_dither = Images[handle].do_dither;
  maxc = Images[handle].maxc;
  ccube = Images[handle].ccube;
  private_cmap = Images[handle].private_cmap;

  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-ccube",3)) { 
      if (++nextarg >= argc) {
	WriteReply(0, "Missing value for -ccube: brighten failed",fd, handle); 
	return; 
      }
      ccube = atoi(argv[nextarg]);
    } else if (!strncmp(argv[nextarg],"-maxc",4)) {
      if (++nextarg >= argc) {
	WriteReply(0, "Missing value for -maxc: brighten failed",fd, handle); 
	return; 
      } 
      maxc = atoi(argv[nextarg]); 
    } else if (!strncmp(argv[nextarg],"-plane",4)) {
      if (++nextarg >= argc) {
	WriteReply(0, "Missing value for -plane: brighten failed",fd, handle); 
	return; 
      } 
      pnum = atoi(argv[nextarg]); 
      if (pnum >= ImPlanesOfImg(Im)) {
	WriteReply(0, "Bad value for -plane: brighten failed",fd, handle); 
	return; 
      } 
    } else if (!strncmp(argv[nextarg],"-dither",4)) {
      do_dither = 1; 
    } else if (!strncmp(argv[nextarg],"-nodither",4)) {
      do_dither = 0; 
    } else if (!strncmp(argv[nextarg],"-remap",4)) {
      new_trans = 1; 
    } else if (!strncmp(argv[nextarg],"-ycc",4)) {
      on_ycc = 1;
    } else if (!strncmp(argv[nextarg],"-privcolors",4)) {
      private_cmap = 1;
    } else if (!strncmp(argv[nextarg],"-noprivcolors",5)) {
      private_cmap = 0; 
    } else if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1; 
    } else {
      WriteReply(0, "Bad option given: brighten failed",fd, handle); 
      return; 
    }
  }

  if (do_quick_return) {
    WriteReply(1, "", fd, handle); 
  }
  
  if ((ccube != Images[handle].ccube) || 
      (maxc != Images[handle].maxc) || 
      (private_cmap != Images[handle].private_cmap))
	new_trans = 1;

  VR_ShowWaitCursor(Images[handle].wid); 
  
  on_ycc = on_ycc && (ImColorSpace(Im) == ImC_RGB);

  if (on_ycc) ImChangeCspace(Im, ImC_YCbCr);

  if (!(IM_return = ImBrighten(Im, pnum, bright))) {
    WriteReply(0,"Brighten-image failed", fd, handle);
    if (on_ycc) ImChangeCspace(Im, ImC_RGB);
    VR_ShowNormalCursor(Images[handle].wid); 
    return;
  }
  
  if (on_ycc) ImChangeCspace(Im, ImC_RGB);

  iid = Images[handle].iid; 

  /* not needed: same data pointers */
  /* CopyImgToIid(Im, iid); */


  if (new_trans) { 
    cflag = ccubeToflag(ccube); 
    VR_CFL_SET_NCOLORS(cflag, maxc);
    if (private_cmap)
      cflag |= VR_CFL_PRIVATE; 
    new_tid = VR_GetTranslatorFromCM(iid, cflag,
      VR_Colormaps[VR_Translators[Images[handle].tid].VR_cmap].cmap);
    if (new_tid >= 0) { 
       if (VR_ChangeTranslator(Images[handle].wid, new_tid)
	    == VR_SUCCESS) {
	  Images[handle].tid = new_tid;
       } else {
	 VR_FreeTranslator(new_tid); 
	 new_trans = 0; 
       }
    } else new_trans = 0; 
  }
  
  Images[handle].do_dither = do_dither;

  if (new_trans) { 
    Images[handle].maxc = maxc;
    Images[handle].ccube = ccube;
    Images[handle].private_cmap = private_cmap;
  } 
  WriteReply(1, "", fd, handle); 

  VR_ShowImage(iid, Images[handle].wid, do_dither);
  needXimage = Images[handle].am_refreshing; /* ||
     (ImStateOfImg(Im) == ImStateReadSome); */ 

  if (!needXimage) VR_FreeWindowXImage(Images[handle].wid);
  VR_ShowNormalCursor(Images[handle].wid); 
  if (Images[handle].CtrlExists) RefreshControl(handle);

} 

static void ProcessContrast(int argc, char *argv[], int fd)
{ 
  int nextarg = 0;
  Image * Im;
  VR_ImId iid; 
  int do_quick_return =0;
  int ccube, maxc, handle;
  int private_cmap; 
  int do_dither;
  unsigned long cflag; 
  int new_trans = 0; 
  VR_TransId new_tid; 
  int needXimage;
  float contrast;
  int pnum = -1;
  int on_ycc = 0;
  


  /* get handle name */ 
  if (++nextarg >= argc) {
    WriteReply(0,"No handle given: change-contrast failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: change-contrast failed", fd, -1);
    return;
  } 

  if (!Images[handle].ImExists) {  
    WriteReply(0,"Image already thrown away: change-contrast failed", fd, handle);
    return;
  } 

  /* get contrast */
  if (++nextarg >= argc) {
    WriteReply(0,"No contrast given: change-contrast failed", fd, handle);
    return;
  } 
  contrast = atof(argv[nextarg]);
  

  Im = Images[handle].Im;

  
  /* get options */
  do_dither = Images[handle].do_dither;
  maxc = Images[handle].maxc;
  ccube = Images[handle].ccube;
  private_cmap = Images[handle].private_cmap;

  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-ccube",3)) { 
      if (++nextarg >= argc) {
	WriteReply(0, "Missing value for -ccube: change-contrast failed",fd, handle); 
	return; 
      }
      ccube = atoi(argv[nextarg]);
    } else if (!strncmp(argv[nextarg],"-maxc",4)) {
      if (++nextarg >= argc) {
	WriteReply(0, "Missing value for -maxc: change-contrast failed",fd, handle); 
	return; 
      } 
      maxc = atoi(argv[nextarg]); 
    } else if (!strncmp(argv[nextarg],"-plane",4)) {
      if (++nextarg >= argc) {
	WriteReply(0, "Missing value for -plane: change-contrast failed",fd, handle); 
	return; 
      } 
      pnum = atoi(argv[nextarg]); 
      if (pnum >= ImPlanesOfImg(Im)) {
	WriteReply(0, "Bad value for -plane: change-contrast failed",fd, handle); 
	return; 
      } 
    } else if (!strncmp(argv[nextarg],"-dither",4)) {
      do_dither = 1; 
    } else if (!strncmp(argv[nextarg],"-nodither",4)) {
      do_dither = 0; 
    } else if (!strncmp(argv[nextarg],"-remap",4)) {
      new_trans = 1; 
    } else if (!strncmp(argv[nextarg],"-ycc",4)) {
      on_ycc = 1;
    } else if (!strncmp(argv[nextarg],"-privcolors",4)) {
      private_cmap = 1; 
    } else if (!strncmp(argv[nextarg],"-noprivcolors",5)) {
      private_cmap = 0; 
    } else if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1; 
    } else {
      WriteReply(0, "Bad option given: change-contrast failed",fd, handle); 
      return; 
    }
  }

  if (do_quick_return) {
    WriteReply(1, "", fd, handle); 
  }
  
  if ((ccube != Images[handle].ccube) || 
      (maxc != Images[handle].maxc) || 
      (private_cmap != Images[handle].private_cmap))
	new_trans = 1;

  VR_ShowWaitCursor(Images[handle].wid); 

  on_ycc = on_ycc && (ImColorSpace(Im) == ImC_RGB);

  if (on_ycc) ImChangeCspace(Im, ImC_YCbCr);

  if (!(IM_return = ImContrast(Im, pnum, contrast))) {
    WriteReply(0,"Contrast-image failed", fd, handle);
    if (on_ycc) ImChangeCspace(Im, ImC_RGB);
    VR_ShowNormalCursor(Images[handle].wid); 
    return;
  }
  
  if (on_ycc) ImChangeCspace(Im, ImC_RGB);

  iid = Images[handle].iid; 

  /* not needed: same data pointers */
  /* CopyImgToIid(Im, iid); */

  if (new_trans) { 
    cflag = ccubeToflag(ccube); 
    VR_CFL_SET_NCOLORS(cflag, maxc);
    if (private_cmap)
      cflag |= VR_CFL_PRIVATE; 
    new_tid = VR_GetTranslatorFromCM(iid, cflag,
      VR_Colormaps[VR_Translators[Images[handle].tid].VR_cmap].cmap);
    if (new_tid >= 0) { 
       if (VR_ChangeTranslator(Images[handle].wid, new_tid)
	    == VR_SUCCESS) {
	  Images[handle].tid = new_tid;
       } else {
	 VR_FreeTranslator(new_tid); 
	 new_trans = 0; 
       }
    } else new_trans = 0; 
  }

  Images[handle].do_dither = do_dither;

  if (new_trans) { 
    Images[handle].maxc = maxc;
    Images[handle].ccube = ccube;
    Images[handle].private_cmap = private_cmap;
  } 

  WriteReply(1, "", fd, handle); 

  VR_ShowImage(iid, Images[handle].wid, do_dither);

  needXimage = Images[handle].am_refreshing; /* ||
     (ImStateOfImg(Im) == ImStateReadSome); */

  if (!needXimage) VR_FreeWindowXImage(Images[handle].wid);
  VR_ShowNormalCursor(Images[handle].wid); 
  if (Images[handle].CtrlExists) RefreshControl(handle);

} 

static void ProcessNormalize(int argc, char *argv[], int fd)
{ 
  int nextarg = 0;
  Image * Im;
  VR_ImId iid; 
  int do_quick_return =0;
  int ccube, maxc, handle;
  int private_cmap; 
  int do_dither;
  unsigned long cflag; 
  int new_trans = 0; 
  VR_TransId new_tid; 
  int needXimage;
  XWindowChanges wchan;
  


  /* get handle name */ 
  if (++nextarg >= argc) {
    WriteReply(0,"No handle given: normalize failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: normalize failed", fd, -1);
    return;
  } 

  if (!Images[handle].ImExists) {  
    WriteReply(0,"Image already thrown away: normalize failed", fd, handle);
    return;
  } 


  Im = Images[handle].Im;

  
  /* get options */
  do_dither = Images[handle].do_dither;
  maxc = Images[handle].maxc;
  ccube = Images[handle].ccube;
  private_cmap = Images[handle].private_cmap;

  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-ccube",3)) { 
      if (++nextarg >= argc) {
	WriteReply(0, "Missing value for -ccube: normalize failed",fd, handle); 
	return; 
      }
      ccube = atoi(argv[nextarg]);
    } else if (!strncmp(argv[nextarg],"-maxc",4)) {
      if (++nextarg >= argc) {
	WriteReply(0, "Missing value for -maxc: normalize failed",fd, handle); 
	return; 
      } 
      maxc = atoi(argv[nextarg]); 
    } else if (!strncmp(argv[nextarg],"-dither",4)) {
      do_dither = 1; 
    } else if (!strncmp(argv[nextarg],"-nodither",4)) {
      do_dither = 0; 
    } else if (!strncmp(argv[nextarg],"-remap",4)) {
      new_trans = 1; 
    } else if (!strncmp(argv[nextarg],"-privcolors",4)) {
      private_cmap = 1; 
    } else if (!strncmp(argv[nextarg],"-noprivcolors",5)) {
      private_cmap = 0; 
    } else if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1; 
    } else {
      WriteReply(0, "Bad option given: normalize failed",fd, handle); 
      return; 
    }
  }

  if (do_quick_return) {
    WriteReply(1, "", fd, handle); 
  }
  
  if ((ccube != Images[handle].ccube) || 
      (maxc != Images[handle].maxc) || 
      (private_cmap != Images[handle].private_cmap))
	new_trans = 1;

  VR_ShowWaitCursor(Images[handle].wid); 

  if (!(IM_return = ImBackToNormal(Im))) {
    WriteReply(0,"Normalize-image failed", fd, handle);
    VR_ShowNormalCursor(Images[handle].wid); 
    return;
  }
  
  iid = Images[handle].iid; 

  VR_ResetWinSize(Images[handle].wid, ImOrigCols(Im), ImOrigRows(Im));

  VR_ResetImageSize(iid, ImOrigCols(Im), ImOrigRows(Im));

  CopyImgToIid(Im, iid);

  if (new_trans) { 
    cflag = ccubeToflag(ccube); 
    VR_CFL_SET_NCOLORS(cflag, maxc);
    if (private_cmap)
      cflag |= VR_CFL_PRIVATE; 
    new_tid = VR_GetTranslatorFromCM(iid, cflag,
      VR_Colormaps[VR_Translators[Images[handle].tid].VR_cmap].cmap);
    if (new_tid >= 0) { 
       if (VR_ChangeTranslator(Images[handle].wid, new_tid)
	    == VR_SUCCESS) {
	  Images[handle].tid = new_tid;
       } else {
	 VR_FreeTranslator(new_tid); 
	 new_trans = 0; 
       }
    }  else new_trans = 0; 
  }
  
  Images[handle].do_dither = do_dither;

  if (new_trans) { 
    Images[handle].maxc = maxc;
    Images[handle].ccube = ccube;
    Images[handle].private_cmap = private_cmap;
  }

  Images[handle].width = ImOrigCols(Im);
  Images[handle].height = ImOrigRows(Im);

  Images[handle].aspected = 1;

  WriteReply(1, "", fd, handle); 
  VR_ShowImage(iid, Images[handle].wid, do_dither);

  needXimage = Images[handle].am_refreshing; /* ||
     (ImStateOfImg(Im) == ImStateReadSome); */ 

  if (!needXimage) VR_FreeWindowXImage(Images[handle].wid);
  
  wchan.width = ImOrigCols(Im);
  wchan.height = ImOrigRows(Im);
  XConfigureWindow(VR_TheDisplay, VR_Windows[Images[handle].wid].win,
    CWWidth | CWHeight, &wchan);
  XFlush(VR_TheDisplay);
  VR_ShowNormalCursor(Images[handle].wid); 

  if (Images[handle].CtrlExists) RefreshControl(handle);
} 

static void ProcessPixelNormalize(int argc, char *argv[], int fd)
{ 
  int nextarg = 0; 
  Image * Im;
  VR_ImId iid; 
  int do_quick_return =0;
  int ccube, maxc, handle;
  int private_cmap; 
  int do_dither;
  unsigned long cflag; 
  int new_trans = 0; 
  VR_TransId new_tid; 
  int needXimage;
  

  /* get handle name */ 
  if (++nextarg >= argc) {
    WriteReply(0,"No handle given: pixel-normalize failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: pixel-normalize failed", fd, -1);
    return;
  } 

  if (!Images[handle].ImExists) {  
    WriteReply(0,"Image already thrown away: pixel-normalize failed", fd, handle);
    return;
  } 


  Im = Images[handle].Im;

  
  /* get options */
  do_dither = Images[handle].do_dither;
  maxc = Images[handle].maxc;
  ccube = Images[handle].ccube;
  private_cmap = Images[handle].private_cmap;

  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-ccube",3)) { 
      if (++nextarg >= argc) {
	WriteReply(0, "Missing value for -ccube: pixel-normalize failed",fd, handle); 
	return; 
      }
      ccube = atoi(argv[nextarg]);
    } else if (!strncmp(argv[nextarg],"-maxc",4)) {
      if (++nextarg >= argc) {
	WriteReply(0, "Missing value for -maxc: pixel-normalize failed",fd, handle); 
	return; 
      } 
      maxc = atoi(argv[nextarg]); 
    } else if (!strncmp(argv[nextarg],"-dither",4)) {
      do_dither = 1; 
    } else if (!strncmp(argv[nextarg],"-nodither",4)) {
      do_dither = 0; 
    } else if (!strncmp(argv[nextarg],"-remap",4)) {
      new_trans = 1; 
    } else if (!strncmp(argv[nextarg],"-privcolors",4)) {
      private_cmap = 1; 
    } else if (!strncmp(argv[nextarg],"-noprivcolors",5)) {
      private_cmap = 0; 
    } else if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1; 
    } else {
      WriteReply(0, "Bad option given: pixel-normalize failed",fd, handle); 
      return; 
    }
  }

  if (do_quick_return) {
    WriteReply(1, "", fd, handle); 
  }
  
  if ((ccube != Images[handle].ccube) || 
      (maxc != Images[handle].maxc) || 
      (private_cmap != Images[handle].private_cmap))
	new_trans = 1;

  VR_ShowWaitCursor(Images[handle].wid); 

  if (!(IM_return = ImBackToNormalPixels(Im))) {
    WriteReply(0,"Pixel-Normalize-image failed", fd, handle);
    VR_ShowNormalCursor(Images[handle].wid); 
    return;
  }
  
  iid = Images[handle].iid; 

  CopyImgToIid(Im, iid);

  if (new_trans) { 
    cflag = ccubeToflag(ccube); 
    VR_CFL_SET_NCOLORS(cflag, maxc);
    if (private_cmap)
      cflag |= VR_CFL_PRIVATE; 
    new_tid = VR_GetTranslatorFromCM(iid, cflag,
      VR_Colormaps[VR_Translators[Images[handle].tid].VR_cmap].cmap);
    if (new_tid >= 0) { 
       if (VR_ChangeTranslator(Images[handle].wid, new_tid)
	    == VR_SUCCESS) {
	  Images[handle].tid = new_tid;
       } else {
	 VR_FreeTranslator(new_tid); 
	 new_trans = 0; 
       }
    } else new_trans = 0; 
  }
  
  Images[handle].do_dither = do_dither;

  if (new_trans) { 
    Images[handle].maxc = maxc;
    Images[handle].ccube = ccube;
    Images[handle].private_cmap = private_cmap;
  } 

  WriteReply(1, "", fd, handle); 
  VR_ShowImage(iid, Images[handle].wid, do_dither);

  needXimage = Images[handle].am_refreshing; /* ||
     (ImStateOfImg(Im) == ImStateReadSome); */ 

  if (!needXimage) VR_FreeWindowXImage(Images[handle].wid);
  VR_ShowNormalCursor(Images[handle].wid); 
  if (Images[handle].CtrlExists) RefreshControl(handle);

} 

static void ProcessRelocate(int argc, char *argv[], int fd)
{ 
  int nextarg = 0;
  int handle;
  int low_x, low_y, high_x, high_y;
  int high_x_iswidth, high_y_isheight; 
  int ccube, maxc; 
  int private_cmap;
  unsigned long cflag; 
  int new_trans = 0;
  int maintain_ar, do_dither, do_smooth;
  VR_TransId new_tid; 
  int do_quick_return = 0; 
  VR_ImId iid; 
  VR_WinId wid; 
  int wwidth, wheight;
  XWindowChanges wchan;
  unsigned int wchanmask;
  float xscale = 1.0, yscale = 1.0;
  


  /* get handle name */ 
  if (++nextarg >= argc) {
    WriteReply(0,"No handle given: relocate failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: relocate failed", fd, -1);
    return;
  } 

  if (!Images[handle].ImExists) {  
    WriteReply(0,"Image already thrown away: relocate failed", fd, handle);
    return;
  } 


  /* get coordinates */ 
  wchanmask = 0;

  if (++nextarg >= argc) {
    WriteReply(0,"Missing coordinate(s): relocate failed", fd, handle);
    return;
  } 

  if (!strncmp(argv[nextarg],"-",1)) {
    /* retain current value of low_x */
    low_x = 0;
    high_x_iswidth = 1; 
  } else {
    low_x = atoi(argv[nextarg]);
    wchanmask |= CWX;
    high_x_iswidth = 0; 
  }

  if (++nextarg >= argc) {
    WriteReply(0,"Missing coordinate(s): relocate failed", fd, handle);
    return;
  } 
  
  if (!strncmp(argv[nextarg],"-",1)) {
    /* retain current value of low_y */
    low_y = 0;
    high_y_isheight = 1; 
  } else {
    low_y = atoi(argv[nextarg]);
    wchanmask |= CWY;
    high_y_isheight = 0; 
  }


  if (++nextarg >= argc) {
    WriteReply(0,"Missing coordinate(s): relocate failed", fd, handle);
    return;
  } 
  
  if (!strncmp(argv[nextarg],"-x",2)) {
    /* scale current width */
    sscanf(argv[nextarg],"-x%f",&xscale);
    if (xscale <= 0) {
      WriteReply(0,"Bad x scaling factor: relocate failed", fd, handle);
      return;
    }
    high_x = low_x +
	     ((int) ((float) ((float) ImCurrCols(Images[handle].Im))*xscale)) - 1;
    if ((high_x - low_x + 1) != ImCurrCols(Images[handle].Im))
      wchanmask |= CWWidth; 

  } else if (!strncmp(argv[nextarg],"-",1)) {
    /* retain current width */
    high_x = low_x + ImCurrCols(Images[handle].Im) - 1;

  } else if (!strncmp(argv[nextarg],"0x",2)) {
    /* scale original width */
    sscanf(argv[nextarg],"0x%f",&xscale);
    if (xscale <= 0) {
      WriteReply(0,"Bad x scaling factor: relocate failed", fd, handle);
      return;
    }
    high_x = low_x +
	     ((int) ((float) ((float) ImOrigCols(Images[handle].Im))*xscale)) - 1;
    if ((high_x - low_x + 1) != ImCurrCols(Images[handle].Im))
      wchanmask |= CWWidth; 

  } else if (!strcmp(argv[nextarg],"0")) {
    /* retain original width */
    high_x = low_x + ImOrigCols(Images[handle].Im) - 1;
    if ((high_x - low_x + 1) != ImCurrCols(Images[handle].Im))
      wchanmask |= CWWidth; 

  } else {
    /* high_x sepcified */
    high_x = atoi(argv[nextarg]);
    high_x -= high_x_iswidth;
    if ((high_x - low_x + 1) != ImCurrCols(Images[handle].Im))
      wchanmask |= CWWidth; 
  }


  if (++nextarg >= argc) {
    WriteReply(0,"Missing coordinate(s): relocate failed", fd, handle);
    return;
  } 
  
  if (!strncmp(argv[nextarg],"-x",2)) {
    /* scale current height */
    sscanf(argv[nextarg],"-x%f",&yscale);
    if (yscale <= 0) {
      WriteReply(0,"Bad y scaling factor: relocate failed", fd, handle);
      return;
    }
    high_y = low_y +
	     ((int) ((float) ((float) ImCurrRows(Images[handle].Im))*yscale)) - 1;
    if ((high_y - low_y + 1) != ImCurrRows(Images[handle].Im))
      wchanmask |= CWHeight; 

  } else if (!strncmp(argv[nextarg],"-",1)) {
    /* retain current height */
    high_y = low_y + ImCurrRows(Images[handle].Im) - 1;

  } else if (!strncmp(argv[nextarg],"0x",2)) {
    /* scale original height */
    sscanf(argv[nextarg],"0x%f",&yscale);
    if (yscale <= 0) {
      WriteReply(0,"Bad y scaling factor: relocate failed", fd, handle);
      return;
    }
    high_y = low_y +
	     ((int) ((float) ((float) ImOrigRows(Images[handle].Im))*yscale)) - 1;
    if ((high_y - low_y + 1) != ImCurrRows(Images[handle].Im))
      wchanmask |= CWHeight; 

  } else if (!strcmp(argv[nextarg],"0")) {
    /* retain original height */
    high_y = low_y + ImOrigRows(Images[handle].Im) - 1;
    if ((high_y - low_y + 1) != ImCurrRows(Images[handle].Im))
      wchanmask |= CWHeight; 

  } else {
    /* high_y sepcified */
    high_y = atoi(argv[nextarg]);
    high_y -= high_y_isheight;
    if ((high_y - low_y + 1) != ImCurrRows(Images[handle].Im))
      wchanmask |= CWHeight; 
  }
  
  if ((wwidth = high_x - low_x + 1) <= 1) {
    WriteReply(0,"Bad coordinate(s): relocate failed", fd, handle);
    return;
  } 
  
  if ((wheight = high_y - low_y + 1) <= 1) { 
    WriteReply(0,"Bad coordinate(s): relocate failed", fd, handle);
    return;
  } 

  
  
  /* get options */
  maxc = Images[handle].maxc;
  ccube = Images[handle].ccube;
  private_cmap = Images[handle].private_cmap;
  do_dither = Images[handle].do_dither;
  do_smooth = Images[handle].do_smooth;
  maintain_ar = Images[handle].maintain_ar;

  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-ccube",3)) { 
      if (++nextarg >= argc) {
	WriteReply(0, "Missing value for -ccube: relocate failed",fd, handle); 
	return; 
      }
      ccube = atoi(argv[nextarg]);
    } else if (!strncmp(argv[nextarg],"-maxc",4)) {
      if (++nextarg >= argc) {
	WriteReply(0, "Missing value for -maxc: relocate failed",fd, handle); 
	return; 
      } 
      maxc = atoi(argv[nextarg]); 
    } else if (!strncmp(argv[nextarg],"-noaspect",5)) {
      maintain_ar = 0;
    } else if (!strncmp(argv[nextarg],"-aspect",5)) {
      maintain_ar = 1;
    } else if (!strncmp(argv[nextarg],"-dither",4)) {
      do_dither = 1; 
    } else if (!strncmp(argv[nextarg],"-nodither",4)) {
      do_dither = 0; 
    } else if (!strncmp(argv[nextarg],"-nosmooth",5)) {
      do_smooth = 0;
    } else if (!strncmp(argv[nextarg],"-smooth",5)) {
      do_smooth = 1;
    } else if (!strncmp(argv[nextarg],"-remap",4)) {
      new_trans = 1;
    } else if (!strncmp(argv[nextarg],"-privcolors",4)) {
      private_cmap = 1;
    } else if (!strncmp(argv[nextarg],"-noprivcolors",5)) {
      private_cmap = 0;
    } else if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1; 
    } else {
      WriteReply(0, "Bad option given: relocate failed",fd, handle); 
      return; 
    }
  }

  if (do_quick_return) 
    WriteReply(1, "", fd, handle); 

  if ((ccube != Images[handle].ccube) || 
      (maxc != Images[handle].maxc) || 
      (private_cmap != Images[handle].private_cmap))
	new_trans = 1;


  if (maintain_ar && !Images[handle].aspected && !wchanmask)
    wchanmask = CWWidth | CWHeight;

  Images[handle].do_smooth = do_smooth; 
  Images[handle].maintain_ar = maintain_ar;

  if (!wchanmask) {
    if (!do_quick_return) WriteReply(1, "", fd, handle);
    if (Images[handle].CtrlExists) RefreshControl(handle);
    return;
  }

  iid = Images[handle].iid;
  wid = Images[handle].wid;

  VR_ShowWaitCursor(wid); 

  if (!(IM_return = ImSetImgDim(Images[handle].Im, -1, wheight, wwidth, 
       do_smooth, maintain_ar))) {

	 VR_ShowNormalCursor(wid); 
	 WriteReply(0,"Could not resize image: relocate failed",fd, handle);
	 return;
  }
  Images[handle].aspected = Images[handle].maintain_ar;

  /* re-aspecting might have changed width and height */
  wwidth = ImCurrCols(Images[handle].Im);
  wheight = ImCurrRows(Images[handle].Im);
  VR_ResetWinSize(wid, wwidth, wheight);
  VR_ResetImageSize(iid, wwidth, wheight);
  CopyImgToIid(Images[handle].Im, iid);

  Images[handle].width = wwidth;
  Images[handle].height = wheight;
  
  if (!Images[handle].no_subwin) { 
    wchan.x = low_x;
    wchan.y = low_y;
    wchan.width = wwidth;
    wchan.height = wheight;
    XConfigureWindow(VR_TheDisplay, VR_Windows[Images[handle].wid].win,
        wchanmask, &wchan);
        /* will not! invoke HandleResizing via an XEvent */
    XFlush(VR_TheDisplay);
  } else { 
    if (wchanmask & CWX) VR_ResetWinLocX(wid, low_x); 
    if (wchanmask & CWY) VR_ResetWinLocY(wid, low_y); 
    Images[handle].x = low_x; 
    Images[handle].y = low_y; 
  }

  if (new_trans) { 
      cflag = ccubeToflag(ccube); 
      VR_CFL_SET_NCOLORS(cflag, maxc);
      if (private_cmap)
        cflag |= VR_CFL_PRIVATE; 
      new_tid = VR_GetTranslatorFromCM(iid, cflag,
        VR_Colormaps[VR_Translators[Images[handle].tid].VR_cmap].cmap);
      if (new_tid >= 0) { 
         if (VR_ChangeTranslator(Images[handle].wid, new_tid)
  	    == VR_SUCCESS) {
  	    Images[handle].tid = new_tid;
         } else {
	   VR_FreeTranslator(new_tid); 
	   new_trans = 0;
         }
      } else new_trans = 0;  
  }

  if (new_trans) {
    Images[handle].ccube = ccube;
    Images[handle].maxc = maxc;
    Images[handle].private_cmap = private_cmap;
  }
  Images[handle].do_dither = do_dither; 

  WriteReply(1, "", fd, handle);

  VR_ShowImage(iid, wid, do_dither);

  VR_ShowNormalCursor(wid); 
  if (Images[handle].CtrlExists) RefreshControl(handle);
} 

static void ProcessCrop(int argc, char *argv[], int fd)
{ 
  int nextarg = 0;
  int handle;
  int low_x, low_y, high_x, high_y;
  int ccube, maxc, do_dither; 
  int private_cmap, do_quick_return = 0; 
  int wwidth, wheight;
  unsigned long cflag;
  int new_trans = 0; 
  XWindowChanges wchan;
  VR_TransId new_tid;
  VR_ImId iid;
  int needXimage; 
  Image *Im; 
  


  /* get handle name */ 
  if (++nextarg >= argc) {
    WriteReply(0,"No handle given: crop failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: crop failed", fd, -1);
    return;
  } 

  if (!Images[handle].ImExists) {  
    WriteReply(0,"Image already thrown away: crop failed", fd, handle);
    return;
  } 

  Im = Images[handle].Im; 
  /* get coordinates */ 

  if (++nextarg >= argc) {
    WriteReply(0,"Missing coordinate(s): crop failed", fd, handle);
    return;
  } 
  
  low_x = atoi(argv[nextarg]);

  if (++nextarg >= argc) {
    WriteReply(0,"Missing coordinate(s): crop failed", fd, handle);
    return;
  } 
  
  low_y = atoi(argv[nextarg]);

  if (++nextarg >= argc) {
    WriteReply(0,"Missing coordinate(s): crop failed", fd, handle);
    return;
  } 
  
  high_x = atoi(argv[nextarg]);

  if (++nextarg >= argc) {
    WriteReply(0,"Missing coordinate(s): crop failed", fd, handle);
    return;
  } 
  
  high_y = atoi(argv[nextarg]);
  
  if ((low_x < 0) || (low_y < 0) || 
      (high_x >= ImCurrCols(Im)) ||
      (high_y >= ImCurrRows(Im)) ) { 
    WriteReply(0,"Bad coordinate(s): crop failed", fd, handle);
    return;
  } 

  if ((wwidth = high_x - low_x + 1) <= 1) {
    WriteReply(0,"Bad coordinate(s): crop failed", fd, handle);
    return;
  } 
  
  if ((wheight = high_y - low_y + 1) <= 1) { 
    WriteReply(0,"Bad coordinate(s): crop failed", fd, handle);
    return;
  } 

  
  
  /* get options */
  do_dither = Images[handle].do_dither;
  maxc = Images[handle].maxc;
  ccube = Images[handle].ccube;
  private_cmap = Images[handle].private_cmap;


  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-ccube",3)) { 
      if (++nextarg >= argc) {
	WriteReply(0, "Missing value for -ccube: crop failed",fd, handle); 
	return; 
      }
      ccube = atoi(argv[nextarg]);
    } else if (!strncmp(argv[nextarg],"-maxc",4)) {
      if (++nextarg >= argc) {
	WriteReply(0, "Missing value for -maxc: crop failed",fd, handle); 
	return; 
      } 
      maxc = atoi(argv[nextarg]); 
    } else if (!strncmp(argv[nextarg],"-dither",4)) {
      do_dither = 1; 
    } else if (!strncmp(argv[nextarg],"-nodither",4)) {
      do_dither = 0; 
    } else if (!strncmp(argv[nextarg],"-remap",4)) {
      new_trans = 1; 
    } else if (!strncmp(argv[nextarg],"-privcolors",4)) {
      private_cmap = 1; 
    } else if (!strncmp(argv[nextarg],"-noprivcolors",5)) {
      private_cmap = 0; 
    } else if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1;
    } else {
      WriteReply(0, "Bad option given: crop failed",fd, handle); 
      return; 
    }
  }

  if (do_quick_return) {
    WriteReply(1, "", fd, handle); 
  }
  
  
  if ((ccube != Images[handle].ccube) || 
      (maxc != Images[handle].maxc) || 
      (private_cmap != Images[handle].private_cmap))
	new_trans = 1;

  VR_ShowWaitCursor(Images[handle].wid); 

  if (!(IM_return = ImCrop(Im, low_y, low_x, high_y, high_x))) {
    WriteReply(0,"Crop-image failed", fd, handle);
    VR_ShowNormalCursor(Images[handle].wid); 
    return;
  }
  
  VR_ResetWinSize(Images[handle].wid, wwidth, wheight); 

  iid = Images[handle].iid; 
  
  VR_ResetImageSize(iid, wwidth, wheight); 

  CopyImgToIid(Im, iid);

  if (new_trans) { 
    cflag = ccubeToflag(ccube); 
    VR_CFL_SET_NCOLORS(cflag, maxc);
    if (private_cmap)
      cflag |= VR_CFL_PRIVATE; 
    new_tid = VR_GetTranslatorFromCM(iid, cflag,
      VR_Colormaps[VR_Translators[Images[handle].tid].VR_cmap].cmap);
    if (new_tid >= 0) { 
       if (VR_ChangeTranslator(Images[handle].wid, new_tid)
	    == VR_SUCCESS) {
	  Images[handle].tid = new_tid;
       } else {
	 VR_FreeTranslator(new_tid); 
	 new_trans = 0; 
       }
    } else new_trans = 0; 
  }

  Images[handle].do_dither = do_dither;

  if (new_trans) { 
    Images[handle].maxc = maxc;
    Images[handle].ccube = ccube;
    Images[handle].private_cmap = private_cmap;
  }

  Images[handle].width = wwidth;
  Images[handle].height = wheight;

  WriteReply(1, "", fd, handle); 
  VR_ShowImage(iid, Images[handle].wid, do_dither);

  needXimage = Images[handle].am_refreshing; /* ||
     (ImStateOfImg(Im) == ImStateReadSome); */ 

  if (!needXimage) VR_FreeWindowXImage(Images[handle].wid);

  if (!Images[handle].no_subwin) { 
    wchan.width = wwidth;
    wchan.height = wheight;
  
    XConfigureWindow(VR_TheDisplay, VR_Windows[Images[handle].wid].win,
      CWWidth | CWHeight, &wchan);
    XFlush(VR_TheDisplay);
  }

  VR_ShowNormalCursor(Images[handle].wid); 

  if (Images[handle].CtrlExists) RefreshControl(handle);
} 

static void ProcessStatus(int argc, char *argv[], int fd)
{ 
  char reply[TASVIR_MAX_STR_LENGTH]; 


  sprintf(reply,"%d Images, %d Colormaps, port: %d, visual: %s, depth: %d",
    NumImages, VR_NumColormaps, tas_port,
    VR_ViewerClassName(VR_VisualClass), VR_Depth);  
  WriteReply(1, reply, fd, -1); 

} 


static void ProcessQuit(int argc, char *argv[], int fd)
{ 
  int i;

  WriteReply(1, "", fd, -1); 

  for (i=0;i<TASVIR_MAX_IMAGES;i++) {
    if (Images[i].InUse) { 
      VR_FreeWindow(Images[i].wid); 
      VR_FreeTranslator(Images[i].tid); 
    }
  } 
  NumImages = 0;

  
} 


static void ProcessLogfile(int argc, char *argv[], int fd)
{ 
  int nextarg = 0;
  FILE *tempLogfile; 

  if (NumImages > 0) { 
    WriteReply(0,"Cannot change log while images are being shown"
      , fd, -1);
    return;
  } 
  
  /* get file name */ 
  if (++nextarg >= argc) {
    WriteReply(0,"No file name given: logfile failed", fd, -1);
    return;
  } 
  
  if (!strcmp(argv[nextarg],"none")) tempLogfile = (FILE *) 0;
  else if ((tempLogfile = fopen(argv[nextarg],"w")) == NULL) { 
    WriteReply(0,"Cannot open file: logfile failed", fd, -1);
    return;
  }

  if (LogFile) {
    fprintf(LogFile,"Changing logfile to %s\n",argv[nextarg]);
    if (LogFile != stderr) fclose(LogFile);
  }

  LogFile = tempLogfile; 
  if (LogFile) VR_errfile = LogFile; 

  WriteReply(1, "", fd, -1); 
  
} 

/*
static char *ShellSafe(char *s)
{
  static char ans[TASVIR_MAX_STR_LENGTH];
  int i=0,j=0;
  while ((i<(TASVIR_MAX_STR_LENGTH-3)) && (j < (TASVIR_MAX_STR_LENGTH-3))) {
    if ((s[i] == '?') || (s[i] == '\\') || (s[i] == '!') ||
	(s[i] == '*') || (s[i] == '<') || (s[i] == '>')) 
	  ans[j++] = '\\';
    if ((ans[j++] = s[i++]) == '\0') break;
  }
  return(ans);
}
*/

static void RefreshControl(int handle)
{
  int replyStatus = replySent; 
  replySent = 0; 
  WriteReply(1, "", Images[handle].PipeToCtrl[1], handle);
  replySent = replyStatus; 
}

static int InvokeControl(int handle)
{

  char *CtrlArgs[100], CtrlArgStore[100][50];
  int numCtrlArgs;
  char stat_string[20];
  int i, npoints, islossless;

  if (Images[handle].CtrlExists) {
    if (!CloseControl(handle)) return(0);
  }

  if (pipe(Images[handle].PipeToCtrl) == -1) {
    return(0);
  }

  if (pipe(Images[handle].PipeFromCtrl) == -1) {
    close(Images[handle].PipeToCtrl[0]);
    close(Images[handle].PipeToCtrl[1]);
    return(0);
  }

  if ((Images[handle].CtrlPid=fork())==-1) {
    close(Images[handle].PipeToCtrl[0]);
    close(Images[handle].PipeToCtrl[1]);
    close(Images[handle].PipeFromCtrl[0]);
    close(Images[handle].PipeFromCtrl[1]);
    return(0);
  }


  if (Images[handle].CtrlPid==0) {
    /* child */
    close(Images[handle].PipeFromCtrl[0]);
    close(Images[handle].PipeToCtrl[1]);
    if (dup2(Images[handle].PipeFromCtrl[1],1) == -1) exit(1);
    if (dup2(Images[handle].PipeToCtrl[0],0) == -1) exit(1);

    numCtrlArgs = 0;
    strcpy(CtrlArgStore[numCtrlArgs],"TasvirControl");
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;

    sprintf(CtrlArgStore[numCtrlArgs],"%d", handle);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    CtrlArgs[numCtrlArgs] = Images[handle].Name; 
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"0x%x", 
      (unsigned) VR_Windows[Images[handle].wid].win);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"%d", Images[handle].x);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"%d", Images[handle].y);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"%d", Images[handle].owidth);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"%d", Images[handle].oheight);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"%d", Images[handle].nplanes);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"%d", Images[handle].width);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"%d", Images[handle].height);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"%d", Images[handle].do_smooth);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"%d", Images[handle].maintain_ar);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"%d",
      (Images[handle].ImExists) ? 8*ImSampleBytes(Images[handle].Im) : 8); 
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    CtrlArgs[numCtrlArgs] = Images[handle].format;
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"%d",
      ImIsCompressed(Images[handle].Im)); 
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    CtrlArgs[numCtrlArgs] = Images[handle].filter;
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"%d", Images[handle].do_dither);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    CtrlArgs[numCtrlArgs] = VR_ViewerClassName(VR_VisualClass);
    numCtrlArgs++;

    sprintf(CtrlArgStore[numCtrlArgs],"%d", VR_BitsPerPixel);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"%u", VR_MaxPixel);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    sprintf(CtrlArgStore[numCtrlArgs],"%d", VR_FixedColors);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    if (!VR_FixedColors) {
      
      sprintf(CtrlArgStore[numCtrlArgs],"%d", 
	 VR_Translators[Images[handle].tid].TotalColorsUsed);
      CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
      numCtrlArgs++;
      
      sprintf(CtrlArgStore[numCtrlArgs],"%d",  Images[handle].maxc);
      CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
      numCtrlArgs++;
      
      sprintf(CtrlArgStore[numCtrlArgs],"%d",  Images[handle].ccube);
      CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
      numCtrlArgs++;
      
      sprintf(CtrlArgStore[numCtrlArgs],"%d",  Images[handle].private_cmap);
      CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
      numCtrlArgs++;
    }

    sprintf(CtrlArgStore[numCtrlArgs],"%d", Images[handle].ImExists);
    CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
    numCtrlArgs++;
    
    if (Images[handle].ImExists) {

      CtrlArgs[numCtrlArgs] = ImFileNameOfImg(Images[handle].Im); 
      numCtrlArgs++;
      
      sprintf(CtrlArgStore[numCtrlArgs],"%ld", ImBytesRead(Images[handle].Im));
      CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
      numCtrlArgs++;

      if (ImStateOfImg(Images[handle].Im) == ImStateReadSome)
       strcpy(CtrlArgStore[numCtrlArgs], "0");
      else 
       strcpy(CtrlArgStore[numCtrlArgs], "1");
      CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
      numCtrlArgs++;

      if (ImQclicIcurvePoints(Images[handle].Im) > 1) {

	npoints = ImQclicIcurvePoints(Images[handle].Im);
	if (npoints + numCtrlArgs > 80) npoints = 80 - numCtrlArgs;
        if (npoints <2) npoints = 2;

        sprintf(CtrlArgStore[numCtrlArgs],"%d", npoints); 
        CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
        numCtrlArgs++;

	CtrlArgs[numCtrlArgs] = ImQclicIcurveQmetric(Images[handle].Im);
	numCtrlArgs++;

	for (i=0;i<npoints;i++) {
          sprintf(CtrlArgStore[numCtrlArgs],"%ld",
	    ImQclicHeaderBytes(Images[handle].Im) +
	    ImQclicIcurveS(Images[handle].Im,i));
          CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
          numCtrlArgs++;
          sprintf(CtrlArgStore[numCtrlArgs],"%f", ImQclicIcurveQ(Images[handle].Im,i));
          CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
          numCtrlArgs++;
	}
      } else if (ImTotalBytes(Images[handle].Im) > 0) {
        strcpy(CtrlArgStore[numCtrlArgs],"2");
        CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
        numCtrlArgs++;
        strcpy(CtrlArgStore[numCtrlArgs],"Size");
        CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
        numCtrlArgs++;
        strcpy(CtrlArgStore[numCtrlArgs],"0");
        CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
        numCtrlArgs++;
        strcpy(CtrlArgStore[numCtrlArgs],"0");
        CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
        numCtrlArgs++;
        sprintf(CtrlArgStore[numCtrlArgs],"%ld",ImTotalBytes(Images[handle].Im));
        CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
        numCtrlArgs++;
        CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs-1];
        numCtrlArgs++;
      } else {
	strcpy(CtrlArgStore[numCtrlArgs],"0");
        CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
        numCtrlArgs++;
      }

      islossless = Images[handle].Im->qclic.is_lossless ||
		  !ImIsCompressed(Images[handle].Im);

      sprintf(CtrlArgStore[numCtrlArgs],"%d", islossless); 
      CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
      numCtrlArgs++;

      if (!islossless) {
        sprintf(CtrlArgStore[numCtrlArgs],"%d",
  	  Images[handle].Im->qclic.know_lossless); 
        CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
        numCtrlArgs++;

        if (Images[handle].Im->qclic.know_lossless) {
          CtrlArgs[numCtrlArgs] = 
	    ImQclicLossless(Images[handle].Im); 
          numCtrlArgs++;
        } 
      }

      sprintf(CtrlArgStore[numCtrlArgs],"%d",
	Images[handle].handle_resize); 
      CtrlArgs[numCtrlArgs] = CtrlArgStore[numCtrlArgs];
      numCtrlArgs++;

      
    }


    CtrlArgs[numCtrlArgs] = (char *) 0;

    execvp(CtrlArgs[0], CtrlArgs);

  } else {
    /* parent */
    close(Images[handle].PipeToCtrl[0]);
    close(Images[handle].PipeFromCtrl[1]);

    if ((newlinefd(stat_string, Images[handle].PipeFromCtrl[0], 20) <= 0)
      || ((Images[handle].CtrlWin = (Window)
	    strtol(stat_string,(char **) 0,16)) == 0)) {
      close(Images[handle].PipeToCtrl[1]);
      close(Images[handle].PipeFromCtrl[0]);
      kill(Images[handle].CtrlPid, SIGKILL);
      return(0);
    }

    Images[handle].CtrlExists = TRUE;

    VR_RegisterFd(Images[handle].PipeFromCtrl[0]);

    return(1);
  }
  return 0; 
}

static int CloseControl(int handle)
{

  if (!Images[handle].CtrlExists) {
    return(1);
  }
  close(Images[handle].PipeFromCtrl[0]);
  close(Images[handle].PipeToCtrl[1]);
  kill(Images[handle].CtrlPid, SIGKILL);
  Images[handle].CtrlExists = FALSE;
  VR_UnRegisterFd(Images[handle].PipeFromCtrl[0]);
  return(1);

}

static int HandleControlToggle(VR_WinId wid, XEvent *event)
{
  int handle; 

  if (!VR_WidValid(wid)) return(0);


  if ((handle = FindTasvirImageFromWid(wid)) < 0) return(0);

  if (!Images[handle].CtrlExists) return(InvokeControl(handle));
  else return(CloseControl(handle));
}

static void ProcessShowCtrl(int argc, char *argv[], int fd)
{ 
  int nextarg = 0;
  int handle, do_quick_return = 0; 


  /* get handle */ 
  if (++nextarg >= argc) {
    WriteReply(0,"No handle given: show-control failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);

  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: show-control failed", fd, -1);
    return;
  } 
  
  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1;
    } else {
      WriteReply(0,"Bad option given: ctrl failed", fd, handle);
    }
  }

  if (do_quick_return) WriteReply(1,"",fd, handle);

  if (!InvokeControl(handle)) 
    WriteReply(0,"Error showing control window", fd, handle);
  else 
    WriteReply(1,"",fd, handle);
}

static void ProcessHideCtrl(int argc, char *argv[], int fd)
{ 
  int nextarg = 0, do_quick_return = 0;
  int handle; 

  /* get handle */ 
  if (++nextarg >= argc) {
    WriteReply(0,"No handle given: hide-control failed", fd, -1);
    return;
  } 
  handle = atoi(argv[nextarg]);
  
  if ((handle < 0) || (handle >= TASVIR_MAX_IMAGES) ||
      !Images[handle].InUse) { 
    WriteReply(0,"Invalid handle given: hide-control failed", fd, -1);
    return;
  } 
  
  while (++nextarg < argc) {
    if (!strncmp(argv[nextarg],"-quickreturn",5)) {
      do_quick_return = 1;
    } else {
      WriteReply(0,"Bad option given: hide-ctrl failed", fd, handle);
    }
  }

  if (do_quick_return) WriteReply(1,"",fd, handle);

  if (!CloseControl(handle)) 
    WriteReply(0,"Error hiding control window", fd, handle);
  else 
    WriteReply(1,"",fd, handle);
}


static int ParseCommand(void)
{
  /* Using command, sets CmdArgc, CmdArgv.
     Absorbs "-noreply" option and returns 0 if
     -noreply is present. Returns 1 otherwise */ 

  int ret = 1, saw_quote = 0;
  char *lastarg, *c;


  #define SEPS  " \t\"\0"  

  CmdArgc = 0;
  /* first, make sure that quoted arguments are not
     broken up, by replacing whitespace within quotes
     by \n */
  lastarg = command;
  while (*lastarg != '\0') {
    if (saw_quote) {
      if (*lastarg == '"') saw_quote = 0;
      else if ((*lastarg == ' ') || (*lastarg == '\t')) 
	*lastarg = '\n';
    } else { 
      if (*lastarg == '"') saw_quote = 1;
    }
    lastarg++;
  } 

  lastarg = CmdArgv[CmdArgc] = strtok(command, SEPS);

  while (lastarg) {
    /* replace \n with space */ 
    c = lastarg;
    while (*c != '\0') {
      if (*c == '\n') *c = ' '; 
      c++; 
    } 
    if (!strncmp(lastarg,"-noreply",6)) ret = 0;
    else {
      CmdArgc++;
      lastarg = CmdArgv[CmdArgc] = strtok(0, SEPS);
    }
  }
  return(ret);

}

static void ProcessApply(int argc, char *argv[], int fd)
{
  int numh, i;
  int hlist[TASVIR_MAX_IMAGES];
  int nextarg = 1;
  int cmdarg = 2, newargc;
  char replystr[20];
  char handlestr[20];

  /* verify handle list */
  if (nextarg >= argc) {
    WriteReply(0, "Missing num-handles: apply failed", fd, -1);
    return;
  }
  sscanf(argv[nextarg],"%d",&numh);
  if (numh < 0) {
    numh = 0;
    for (i=0;i<TASVIR_MAX_IMAGES;i++) {
      if (Images[i].InUse) { 
        hlist[numh++] = i;
      }
    }
    if (numh==0) {
      WriteReply(0, "No existing handles: apply failed", fd, -1);
      return;
    }
  } else {
    if (numh > TASVIR_MAX_IMAGES) {
      WriteReply(0, "Too many handles given: apply failed", fd, -1);
      return;
    }
    for (i=0;i<numh;i++) {
      if (++nextarg >= argc) {
	WriteReply(0, "Missing handles: apply failed", fd, -1);
	return;
      }
      sscanf(argv[nextarg],"%d",&hlist[i]);
      if ((hlist[i] < 0) || (hlist[i] >= TASVIR_MAX_IMAGES)) {
	WriteReply(0, "Bad handle given: apply failed", fd, -1);
	return;
      }
    }
    cmdarg += numh;
  }

  if ((cmdarg+1) >= argc) {
    WriteReply(0, "Missing command: apply failed", fd, -1);
    return;
  }

  newargc = argc - cmdarg;
  for (i=0;i<newargc;i++) {
    argv[i] = argv[i+cmdarg];
  }

  sprintf(replystr,"%d", numh);
  WriteReply(1, replystr, fd, -1);

  argv[1] = handlestr;

  for (i=0; i< numh; i++) {
    replySent = 0;
    sprintf(handlestr,"%d",hlist[i]);

    if (!strncmp(argv[0],"free",1)) {
      ProcessFree(newargc, argv, fd);
    } else if (!strncmp(argv[0],"add",2)) {
      ProcessAdd(newargc, argv, fd, -1);
    } else if (!strncmp(argv[0],"dorig",3)) {
      ProcessDOrig(newargc, argv, fd);
    } else if (!strncmp(argv[0],"brighten",3)) {
      ProcessBrighten(newargc, argv, fd);
    } else if (!strncmp(argv[0],"contrast",3)) {
      ProcessContrast(newargc, argv, fd);
    } else if (!strncmp(argv[0],"reshow",4)) {
      ProcessReshow(newargc, argv, fd);
    } else if (!strncmp(argv[0],"ctrl",4)) {
      ProcessShowCtrl(newargc, argv, fd);
    } else if (!strncmp(argv[0],"noctrl",4)) {
      ProcessHideCtrl(newargc, argv, fd);
    } else if (!strncmp(argv[0],"normal",4)) {
      ProcessNormalize(newargc, argv, fd);
    } else if (!strncmp(argv[0],"pixnormal",4)) {
      ProcessPixelNormalize(newargc, argv, fd);
    } else if (!strncmp(argv[0],"query",4)) {
      ProcessQuery(newargc, argv, fd);
    } else if (!strncmp(argv[0],"xv",2)) {
      ProcessXv(newargc, argv, fd);
    } else {
      WriteReply(0,"Unknown command: apply failed",fd, -1); 
    }
  }
}

static void ProcessWApply(int argc, char *argv[], int fd)
{
  int numh, i;
  int hlist[TASVIR_MAX_IMAGES];
  int nextarg = 1;
  int cmdarg = 2, newargc;
  char replystr[20];
  char handlestr[20];
  Window win; 

  /* verify handle list */
  if (nextarg >= argc) {
    WriteReply(0, "Missing window: wapply failed", fd, -1);
    return;
  }
  if (!strcmp(argv[nextarg],"-")) win = root_win;
  else if ((win = (Window) strtol(argv[nextarg],(char **) 0,16)) == 0) win = root_win; 

  numh = 0;
  for (i=0;i<TASVIR_MAX_IMAGES;i++) {
    if (Images[i].InUse && 
          VR_WidValid(Images[i].wid) &&
          (VR_Windows[Images[i].wid].parwin == win)) { 
      hlist[numh++] = i;
    }
  }
  if (numh==0) {
    WriteReply(0, "No existing handles: wapply failed", fd, -1);
    return;
  }

  if ((cmdarg+1) >= argc) {
    WriteReply(0, "Missing command: wapply failed", fd, -1);
    return;
  }

  newargc = argc - cmdarg;
  for (i=0;i<newargc;i++) {
    argv[i] = argv[i+cmdarg];
  }

  sprintf(replystr,"%d", numh);
  WriteReply(1, replystr, fd, -1);

  argv[1] = handlestr;

  for (i=0; i< numh; i++) {
    replySent = 0;
    sprintf(handlestr,"%d",hlist[i]);

    if (!strncmp(argv[0],"free",1)) {
      ProcessFree(newargc, argv, fd);
    } else if (!strncmp(argv[0],"add",2)) {
      ProcessAdd(newargc, argv, fd, -1);
    } else if (!strncmp(argv[0],"dorig",3)) {
      ProcessDOrig(newargc, argv, fd);
    } else if (!strncmp(argv[0],"brighten",3)) {
      ProcessBrighten(newargc, argv, fd);
    } else if (!strncmp(argv[0],"contrast",3)) {
      ProcessContrast(newargc, argv, fd);
    } else if (!strncmp(argv[0],"reshow",4)) {
      ProcessReshow(newargc, argv, fd);
    } else if (!strncmp(argv[0],"ctrl",4)) {
      ProcessShowCtrl(newargc, argv, fd);
    } else if (!strncmp(argv[0],"noctrl",4)) {
      ProcessHideCtrl(newargc, argv, fd);
    } else if (!strncmp(argv[0],"normal",4)) {
      ProcessNormalize(newargc, argv, fd);
    } else if (!strncmp(argv[0],"pixnormal",4)) {
      ProcessPixelNormalize(newargc, argv, fd);
    } else if (!strncmp(argv[0],"query",4)) {
      ProcessQuery(newargc, argv, fd);
    } else if (!strncmp(argv[0],"xv",2)) {
      ProcessXv(newargc, argv, fd);
    } else {
      WriteReply(0,"Unknown command: apply failed",fd, -1); 
    }
  }
}

static int ProcessRequest(int fd, int in_fd)
{

  int reply_fd = fd;

  if (newlinefd(command, fd, TASVIR_MAX_STR_LENGTH) < 0) {
    WriteReply(0,"connection seems to be broken",reply_fd, -1);
    return(1);
  }

  if (!ParseCommand()) reply_fd = -1;
  if (CmdArgc <= 0) {
    WriteReply(0,"no command given",reply_fd, -1); 
    return(1);
  }

  if (!strncmp(CmdArgv[0],"apply",3)) {
    ProcessApply(CmdArgc, CmdArgv, reply_fd); 
  } else if (!strncmp(CmdArgv[0],"wapply",3)) {
    ProcessWApply(CmdArgc, CmdArgv, reply_fd); 
  } else if (!strncmp(CmdArgv[0],"show",2)) {
    ProcessShow(CmdArgc, CmdArgv, reply_fd, in_fd); 
  } else if (!strncmp(CmdArgv[0],"free",1)) {
    ProcessFree(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"replace",3)) {
    ProcessReplace(CmdArgc, CmdArgv, reply_fd, in_fd);
  } else if (!strncmp(CmdArgv[0],"add",2)) {
    ProcessAdd(CmdArgc, CmdArgv, reply_fd, in_fd);
  } else if (!strncmp(CmdArgv[0],"dorig",3)) {
    ProcessDOrig(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"brighten",3)) {
    ProcessBrighten(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"contrast",3)) {
    ProcessContrast(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"reshow",4)) {
    ProcessReshow(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"crop",4)) {
    ProcessCrop(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"ctrl",4)) {
    ProcessShowCtrl(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"noctrl",4)) {
    ProcessHideCtrl(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"normal",4)) {
    ProcessNormalize(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"pixnormal",4)) {
    ProcessPixelNormalize(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"relocate",4)) {
    ProcessRelocate(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"quit",4)) {
    ProcessQuit(CmdArgc, CmdArgv, reply_fd);
    return(0); 
  } else if (!strncmp(CmdArgv[0],"query",4)) {
    ProcessQuery(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"xv",2)) {
    ProcessXv(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"save",4)) {
    ProcessSave(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"csave",4)) {
    ProcessSaveCompressed(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"psshow",4)) {
    ProcessPSShow(CmdArgc, CmdArgv, reply_fd, in_fd);
  } else if (!strncmp(CmdArgv[0],"logfile",1)) {
    ProcessLogfile(CmdArgc, CmdArgv, reply_fd); 
  } else if (!strncmp(CmdArgv[0],"reset",4)) {
    ProcessReset(CmdArgc, CmdArgv, reply_fd); 
  } else if (!strncmp(CmdArgv[0],"status",3)) {
    ProcessStatus(CmdArgc, CmdArgv, reply_fd); 
  } else if (!strncmp(CmdArgv[0],"xbacking",3)) {
    ProcessXBackup(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"defqual",4)) {
    ProcessDefQual(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"smlicon",4)) {
    ProcessSmallIcon(CmdArgc, CmdArgv, reply_fd);
  } else if (!strncmp(CmdArgv[0],"nosmlicon",5)) {
    ProcessNoSmallIcon(CmdArgc, CmdArgv, reply_fd);
  } else {
    WriteReply(0,"Unknown command",reply_fd, -1); 
  }

  return(1); 
}

static int ProcessLocalRequests(fd_set *fdset)
{

  int i,j;
  int fd, reply_fd;

  j=0;
  for (i=0;i<NumImages;i++) {
    while (!Images[j].InUse) j++;
    if (Images[j].CtrlExists) {
      if (FD_ISSET(Images[j].PipeFromCtrl[0], fdset)) {
	fd = Images[j].PipeToCtrl[1];
        if (newlinefd(command, Images[j].PipeFromCtrl[0],
		  TASVIR_MAX_STR_LENGTH) < 0) {
	  WriteReply(0,"Tasvir control seems to have exited",-1, -1);
	  CloseControl(j);
	  j++;
	  continue;
	} 
        if (!ParseCommand()) reply_fd = -1;
	else reply_fd = fd; 
        if (CmdArgc <= 0) {
          WriteReply(0,"no command given",reply_fd, -1); 
        } else {
	  replySent = 0;
          if (!strncmp(CmdArgv[0],"free",1)) {
            ProcessFree(CmdArgc, CmdArgv, reply_fd);
	    #ifndef TASVIR_SERVER
	    return(0);
	    #endif
          } else if (!strncmp(CmdArgv[0],"replace",3)) {
            ProcessReplace(CmdArgc, CmdArgv, reply_fd, -1);
          } else if (!strncmp(CmdArgv[0],"add",2)) {
            ProcessAdd(CmdArgc, CmdArgv, reply_fd, -1);
          } else if (!strncmp(CmdArgv[0],"dorig",3)) {
            ProcessDOrig(CmdArgc, CmdArgv, reply_fd);
          } else if (!strncmp(CmdArgv[0],"brighten",3)) {
            ProcessBrighten(CmdArgc, CmdArgv, reply_fd);
          } else if (!strncmp(CmdArgv[0],"contrast",3)) {
            ProcessContrast(CmdArgc, CmdArgv, reply_fd);
          } else if (!strncmp(CmdArgv[0],"reshow",4)) {
            ProcessReshow(CmdArgc, CmdArgv, reply_fd);
          } else if (!strncmp(CmdArgv[0],"crop",4)) {
            ProcessCrop(CmdArgc, CmdArgv, reply_fd);
          } else if (!strncmp(CmdArgv[0],"normal",4)) {
            ProcessNormalize(CmdArgc, CmdArgv, reply_fd);
          } else if (!strncmp(CmdArgv[0],"pixnormal",4)) {
            ProcessPixelNormalize(CmdArgc, CmdArgv, reply_fd);
          } else if (!strncmp(CmdArgv[0],"relocate",4)) {
            ProcessRelocate(CmdArgc, CmdArgv, reply_fd);
          } else if (!strncmp(CmdArgv[0],"query",4)) {
            ProcessQuery(CmdArgc, CmdArgv, reply_fd);
          } else if (!strncmp(CmdArgv[0],"noctrl",4)) {
            ProcessHideCtrl(CmdArgc, CmdArgv, reply_fd);
          } else if (!strncmp(CmdArgv[0],"xv",2)) {
            ProcessXv(CmdArgc, CmdArgv, reply_fd);
          } else if (!strncmp(CmdArgv[0],"save",4)) {
            ProcessSave(CmdArgc, CmdArgv, reply_fd);
          } else if (!strncmp(CmdArgv[0],"csave",4)) {
            ProcessSaveCompressed(CmdArgc, CmdArgv, reply_fd);
          } else if (!strncmp(CmdArgv[0],"psshow",4)) {
            ProcessPSShow(CmdArgc, CmdArgv, reply_fd, -1);
          } else {
            WriteReply(0,"Unknown command",reply_fd, -1); 
          }
	}
      }
    }
    j++;
  }
  return(1);
}



static void TasvirFatalError(char *msg)
{
  fprintf(stderr,"%s\n",msg);
  exit(1);
} 

int main(int argc, char *argv[]) 
{
  #ifdef TASVIR_SERVER
  int listenFd, socketFd;
  int enable, result, len; 
  struct sockaddr_in servAddr;
  struct sockaddr_in tempAddr;
  #else
  char cmdstore[20][200];
  int cmdstorenum = 0;
  #endif

  fd_set *fdset; 
  int done,n; 
  char * display = NULL;
  VR_ReturnCode VR_ans;
  int vclass, handle; 

  LogFile = stderr;
  VR_errfile = LogFile; 
  vclass = VR_VisDefault;

  #ifdef TASVIR_SERVER
  n=1;
  while (n < argc) {
    if (!strncmp(argv[n],"-display",4)) { 
      n++;
      if (n >= argc) TasvirFatalError("No display specified");
      display = argv[n]; 
    } else if (!strncmp(argv[n],"-visual",4)) { 
      n++;
      if (n >= argc) TasvirFatalError("No visual class specified");
      vclass = atoi(argv[n]);
    } else if (!strncmp(argv[n],"-port",4)) { 
      n++;
      if (n >= argc) TasvirFatalError("No port specified");
      tas_port = atoi(argv[n]);
    } else {
      TasvirFatalError("Bad option: only [-display name] and [-visual vclass_number] supported");
    }
    n++;
  }


  listenFd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenFd < 0) {
    fprintf(LogFile,"Tasvir: socket: %s\n",strerror(errno));
    return(1);
  }

  /* allow listening port number to be reused */ 
  enable = 1;
  result = setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR,
		      (char *)&enable, sizeof(int) );
  if (result < 0) {
    fprintf(LogFile,"Tasvir: setsockopt: %s\n",strerror(errno));
    return(1);
  }

  /* Now bind these to the address.. */ 

  memset(&servAddr, 0, sizeof(struct sockaddr_in)); 
  servAddr.sin_family = AF_INET;
  #ifdef SERV_ANYPORT
    servAddr.sin_port = htons(0);
  #else
    servAddr.sin_port = htons(tas_port);
  #endif
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  result = bind(listenFd, (struct sockaddr *)&servAddr,
		sizeof(struct sockaddr));
  if (result < 0) {
    fprintf(LogFile,"Tasvir: bind: %s\n",strerror(errno));
    return(1);
  }
     
  #ifdef SERV_ANYPORT	
    memset(&tempAddr, 0, sizeof(struct sockaddr));
    len = sizeof(struct sockaddr);

    result = getsockname(listenFd, (sockaddr *)&tempAddr, &len);
    if (result < 0) {
      fprintf(LogFile,"Tasvir: getsockname: %s\n",strerror(errno));
      return(1);
    }
    fprintf(LogFile,"Server listening at port %u.\n", ntohs(tempAddr.sin_port);
  #endif	

  result = listen(listenFd, 5);
  if (result < 0) {
    fprintf(LogFile,"Tasvir: listen: %s\n",strerror(errno));
    return(1);
  }

  #else /* of ifdef TASVIR_SERVER */

  n=1;
  while (n < argc) {
    if (!strncmp(argv[n],"-display",4)) { 
      n++;
      if (n >= argc) TasvirFatalError("No display specified");
      display = argv[n]; 
    } else if (!strncmp(argv[n],"-visual",4)) { 
      n++;
      if (n >= argc) TasvirFatalError("No visual class specified");
      vclass = atoi(argv[n]);
    }
    n++;
  }

  #endif /* of ifdef TASVIR_SERVER */

  if ((VR_ans = VR_InitViewerVisual(display,0,vclass)) != VR_SUCCESS) {
    fprintf(LogFile,"Could not open display: %s\n", VR_ReturnCodeToString(VR_ans) );
    exit(1);
  } 
  root_win = RootWindow(VR_TheDisplay, VR_TheScreen);

  VR_SetTitle("Tasvir:");
  
  #ifdef TASVIR_SERVER

  if (VR_ScreenDoesBacking) {
    fprintf(LogFile,"Display (%d-bit %s) supports window backing stores\n",VR_Depth,VR_ViewerClassName(VR_VisualInfo.vclass));
  } else {
    fprintf(LogFile,"Display (%d-bit %s) does not support window backing stores\n",VR_Depth,VR_ViewerClassName(VR_VisualInfo.vclass));
  }

  VR_RegisterFd(listenFd);
  len = sizeof(struct sockaddr_in);
  done = 0;

  #else /* of ifdef TASVIR_SERVER */

  strcpy(cmdstore[cmdstorenum],"show");
  CmdArgv[0] = cmdstore[cmdstorenum++];

  CmdArgc = 1;
  n = 1;
  while (n < argc) {
    if (!strncmp(argv[n],"-display",4)) { 
      n++;
    } else if (!strncmp(argv[n],"-visual",4)) { 
      n++;
    } else {
      CmdArgv[CmdArgc++] = argv[n];
    }
    n++;
  }

  ProcessShow(CmdArgc, CmdArgv, 1, 0);

  done = !NumImages;

  #endif /* of ifdef TASVIR_SERVER */



  while (!done) {

    VR_return = VR_SUCCESS;
    IM_return = 1; 

    switch(VR_WaitForSomething(&fdset)) {
    
    case VR_SUCCESS:
      #ifdef TASVIR_SERVER
      if (FD_ISSET(listenFd, fdset)) {  
        socketFd = accept(listenFd, (struct sockaddr *)&tempAddr, &len);
        if (socketFd < 0) {
	  if (LogFile) fprintf(LogFile,"Received <0 return from accept\n");
	  break;
        }
        replySent = 0;
        if (!ProcessRequest(socketFd, socketFd)) done = 1;
	close(socketFd);
	FD_CLR(listenFd, fdset);
      } else if (!ProcessLocalRequests(fdset)) done = 1;
      #else /* of ifdef TASVIR_SERVER */
	/* check if we are quitting */
	if (!NumImages) {done = 1;}
	else if (!ProcessLocalRequests(fdset)) done = 1;
      #endif /* of ifdef TASVIR_SERVER */
      break;
    default:
      if (LogFile) fprintf(LogFile,"Received strange event\n");
      break;
    }
  }

  #ifdef TASVIR_SERVER
  close(listenFd); 
  VR_UnRegisterFd(listenFd); 
  #endif

  for (handle=0;handle < TASVIR_MAX_IMAGES; handle++) { 
    if (Images[handle].InUse) { 
      CloseControl(handle);
      VR_FreeWindow(Images[handle].wid); 
      VR_FreeTranslator(Images[handle].tid); 
      if (Images[handle].ImExists) { 
        Images[handle].ImExists = 0; 
        ImFreeImg(Images[handle].Im);
        free(Images[handle].Im); 
      }
      VR_FreeImage(Images[handle].iid);

      Images[handle].InUse = 0;
    }
  }

  VR_CloseViewer();
  return(0);
}

