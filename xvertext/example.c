/**************************************************************************/
/*                                                                        */
/*                                                                        */
/* This is an example program designed to utilise and manipulate the      */
/* `xvertext' routines.                                                   */
/*                                                                        */
/* Version 5.0                                                            */
/*                                                                        */
/* Copyright (c) 1993 Alan Richardson (mppa3@uk.ac.sussex.syma)           */
/*                                                                        */
/*                                                                        */
/* ********************************************************************** */


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rotated.h"


/* ---------------------------------------------------------------------- */


extern char *getenv();
unsigned long AllocNamedColor();
XFontStruct *LoadFont();
void Draw();
void DrawRosette1();
void DrawRosette2();
void DrawAlignments();
void DrawMagnify1();
void DrawMagnify2();
void DrawRansomString();
void DrawRansom();

#ifdef X11R3
#define NDEMOS 6
#else
void DrawStipple1();
void DrawStipple2();
#define NDEMOS 8
#endif /*X11R3*/


/* ---------------------------------------------------------------------- */


GC gc;
Display *dpy;
Window window;
int depth, screen;
unsigned long white, black;

#define NCOLORS 9

char *colors[]={
    "white", "red", "orange", "yellow", "green", "cyan",
    "gold", "pink", "wheat"
};

unsigned long pixels[NCOLORS];


/* ---------------------------------------------------------------------- */


/**************************************************************************/
/*  Allocate a colour from the default colour map                         */
/**************************************************************************/

unsigned long AllocNamedColor(colname)  
    char *colname;    
{
    XColor scrncol, excol;
    
    if(depth==1)
	return white;
    
    if(XAllocNamedColor(dpy, DefaultColormap(dpy, screen), colname,
			&scrncol, &excol))
	return scrncol.pixel;
    
    else
	return white;
}


/* ---------------------------------------------------------------------- */


/**************************************************************************/
/*  Load a named font, or `fixed' if it isn't there                       */
/**************************************************************************/

XFontStruct *LoadFont(fontname)
    char *fontname;
{
    XFontStruct *font;

    font=XLoadQueryFont(dpy, fontname);
    if(!font) {
	fprintf(stderr, "no font `%s'\n", fontname);
	font=XLoadQueryFont(dpy, "fixed");
    }

    return font;
}


/* ---------------------------------------------------------------------- */


/**************************************************************************/
/*  Main routine                                                          */
/**************************************************************************/

int
main()
{
    XEvent ev;
    char *host;
    char buf[1];
    int mode=0, i;
    float version;
    char copyright[100];
    
    /* get copyright information */
    version=XRotVersion(copyright, 100);
    printf("%s - version %.1f\n", copyright, version);
    
    /* establish connection to display */
    if((host=getenv("DISPLAY"))==NULL) {
	fprintf(stderr,"can't connect to host\n");
	exit(1); 
    }
    
    /* open connection to display */
    if((dpy=XOpenDisplay(host))==NULL) {
	fprintf(stderr,"can't open display\n");
	exit(1); 
    }
    
    /* useful macros */
    screen=DefaultScreen(dpy);
    depth=DefaultDepth(dpy, screen);
    
    /* load the colors */
    white=WhitePixel(dpy, screen);
    black=BlackPixel(dpy, screen);

    for(i=0; i<NCOLORS; i++) 
	pixels[i]=AllocNamedColor(colors[i]);

    /* create a window */
    window=XCreateWindow(dpy, DefaultRootWindow(dpy), 0, 0, 600, 600,
			 0, depth, InputOutput, DefaultVisual(dpy, screen),
			 0, NULL);
    
    XSelectInput(dpy, window, ExposureMask|KeyPressMask);
    XSetWindowBackground(dpy, window, black);
    XMapWindow(dpy, window);
    
    /* create a GC */
    gc=XCreateGC(dpy, window, 0, NULL);

    /* simple event loop */
    for(;;) {
	XNextEvent(dpy, &ev);
	
	/* process each event */
	switch(ev.type) {
	  case Expose:
	    if(ev.xexpose.count==0) 
		Draw(mode);
	    break;
	    
	  case KeyPress: 
	    XLookupString(&ev.xkey, buf, 1, NULL, NULL);

	    if(buf[0]=='q') 
		exit(0);
	    else {
		mode=(++mode)%NDEMOS;
		Draw(mode);
	    }
	    break;
	}
    }
}


/* ---------------------------------------------------------------------- */


/**************************************************************************/
/*   Choose which pic to draw                                             */
/**************************************************************************/

void Draw(mode)
    int mode;
{
    XClearWindow(dpy, window);

    if(mode==0)
	DrawRosette1();
    else if(mode==1)
	DrawRosette2();
    else if(mode==2)
	DrawAlignments();
    else if(mode==3)
	DrawMagnify1();
    else if(mode==4)
	DrawMagnify2();
    else if(mode==5)
	DrawRansom();
#ifndef X11R3
    else if(mode==6)
	DrawStipple1();
    else
	DrawStipple2();
#endif /*X11R3*/
}


/* ---------------------------------------------------------------------- */


/**************************************************************************/
/*  Draw an `Adobe PostScript'-style word rosette                         */
/**************************************************************************/

void DrawRosette1()
{
    int i;
    char *fontname=
	"-adobe-helvetica-medium-o-normal--24-240-75-75-p-130-iso8859-1";
    static XFontStruct *font=NULL;

    /* load font */
    if(!font) 
	font=LoadFont(fontname);
    
    /* loop through each angle */
    for(i=0; i<16; i++) {
	XSetForeground(dpy, gc, pixels[i%NCOLORS]);
	XRotDrawAlignedString(dpy, font, (float)(360*i/16), window, 
			      gc, 300, 270, 
			      "Just like PostScript?",
			      MLEFT);
    }	

    XSetForeground(dpy, gc, white);
    XRotDrawAlignedString(dpy, font, 0., window, gc, 300, 590,
			       "Press any key to continue...", BCENTRE);
}


/* ---------------------------------------------------------------------- */


/**************************************************************************/
/*  Draw several rosettes (to show the benefits of caching!)              */
/**************************************************************************/

void DrawRosette2()
{
    int i, j;
    char *fontname=
	"-adobe-times-bold-r-normal--18-180-75-75-p-99-iso8859-1";
    static XFontStruct *font=NULL;

    /* load font */
    if(!font)
        font=LoadFont(fontname);
    
    /* loop through each rosette */
    for(j=0; j<4; j++) {
	/* loop through each angle */
	for(i=0; i<16; i++) {
	    XSetForeground(dpy, gc, pixels[i%NCOLORS]);
	    XRotDrawAlignedString(dpy, font, (float)(360*i/16), window, 
				  gc, 150+300*(j/2), 150+300*(j%2),
				  "- - ABCDEfghij",
				  MLEFT);
	}	
    }

    XSetForeground(dpy, gc, white);
    XRotDrawAlignedString(dpy, font, 0., window, gc, 300, 300,
			  "Press any key to continue...", MCENTRE);
}


/* ---------------------------------------------------------------------- */


/**************************************************************************/
/*  Demonstrate alignment of blocks of text                               */
/**************************************************************************/

void DrawAlignments()
{
    int i;
    int x, y;
    char *fontname=
        "-adobe-times-bold-r-normal--18-180-75-75-p-99-iso8859-1";
    static XFontStruct *font=NULL;

    static int align[3]={
	BRIGHT, MCENTRE, TLEFT
	};

    /* load font */
    if(!font)
        font=LoadFont(fontname);

    if(depth==1)
        XSetBackground(dpy, gc, black);
    else
        XSetBackground(dpy, gc, pixels[1]);

    /* loop through nine positions */
    for(i=0; i<9; i++) {
	/* position */
	x=(i%3)*200+100;
	y=(i/3)*200+100;
	
	/* draw some crosshairs */
	XSetForeground(dpy, gc, pixels[3]);
	XDrawLine(dpy, window, gc, x-30, y, x+30, y);
	XDrawLine(dpy, window, gc, x, y+30, x, y-30);

	/* draw the text */
	XSetForeground(dpy, gc, pixels[5]);
	XRotDrawAlignedImageString(dpy, font, 120.*(float)(i/3),
				   window, gc, x, y, 
				   "Many\nalignments\nare\npossible",
				   align[i%3]);
    }

    XSetForeground(dpy, gc, white);
    XRotDrawAlignedString(dpy, font, 0., window, gc, 300, 590,
			       "Press any key to continue...",BCENTRE);
}


/* ---------------------------------------------------------------------- */


/**************************************************************************/
/*  Show that some fonts don't look too bad when magnified                */
/**************************************************************************/

void DrawMagnify1()
{
    int i, y=90;
    XPoint *xp;
    char *str1="Fonts can be magnified,\nthey might not look TOO bad...";
    char *str2="Size of an elephant";
    char *fontname=
	"-adobe-times-bold-i-normal--24-240-75-75-p-128-iso8859-1";
    static XFontStruct *font=NULL;

    /* load font */
    if(!font) 
        font=LoadFont(fontname);
    
    /* draw top string */
    XSetForeground(dpy, gc, white);
    XRotDrawAlignedString(dpy, font, 0., window, gc, 300, 3,
			  str1, TCENTRE);

    /* draw its bounding box */
    XRotSetBoundingBoxPad(2);
    xp=XRotTextExtents(dpy, font, 0., 300, 3, str1, TCENTRE);
    if(xp) {
	XDrawLines(dpy, window, gc, xp, 5, CoordModeOrigin);
	free((char *)xp);
    }

    /* loop through ten magnifications */
    for(i=0; i<10; i++) {
	XRotSetMagnification((float)i/7.+1.);

	XSetForeground(dpy, gc, pixels[i%NCOLORS]);
	XRotDrawAlignedString(dpy, font, 0., window, gc,
			      300, y, str2, MCENTRE);

	y+=30.*((float)i/7.+1.);
    }	

    /* reset */
    XRotSetMagnification(1.);

    XSetForeground(dpy, gc, white);
    XRotDrawAlignedString(dpy, font, 0., window, gc, 300, 590,
			       "Press any key to continue...", BCENTRE);
}


/* ---------------------------------------------------------------------- */


/**************************************************************************/
/*  Demo of font magnifying at an angle                                   */
/**************************************************************************/

void DrawMagnify2()
{
    int i, d=0;
    char *str1="Text leaps out";
    char *fontname=
        "-adobe-times-bold-r-normal--18-180-75-75-p-99-iso8859-1";
    static XFontStruct *font=NULL;

    /* load font */
    if(!font)
        font=LoadFont(fontname);

    /* loop through ten magnifications */
    for(i=0; i<10; i++) {
	/* set magnification */
	XRotSetMagnification((float)i/5.+1.);

	/* random colour */
	XSetForeground(dpy, gc, pixels[i%NCOLORS]);

	/* draw strings */
	XRotDrawAlignedString(dpy, font, 0., window, gc,
			      300, 20+d, str1, MCENTRE);

	XRotDrawAlignedString(dpy, font, 90., window, gc,
			      20+d, 300, str1, MCENTRE);

	XRotDrawAlignedString(dpy, font, 180., window, gc,
			      300, 580-d, str1, MCENTRE);

	XRotDrawAlignedString(dpy, font, -90., window, gc,
			      580-d, 300, str1, MCENTRE);

	/* increase offset */
	d+=10;
    }	

    /* reset */
    XRotSetMagnification(1.);

    XSetForeground(dpy, gc, white);
    XRotDrawAlignedString(dpy, font, 0., window, gc, 300, 300,
			       "Press any\nkey to continue...", MCENTRE);
}


/* ---------------------------------------------------------------------- */


/**************************************************************************/
/*  Draw a ransom note                                                    */
/**************************************************************************/

void DrawRansom()
{
    /* if we don't seed random generator here, redraws will be drawn in new
       random fashion and so won't use the cache */

    srand(1);

    DrawRansomString("Cops-", 20, 70, 1.);
    DrawRansomString("This is an example", 20, 150, 1.);
    DrawRansomString("of manipulating", 20, 230, 1.);
    DrawRansomString("bounding boxes &", 20, 310, 1.);
    DrawRansomString("magnification...", 20, 390, 1.);
    DrawRansomString("Scorpio", 450, 470, 0.5);
    DrawRansomString("Press any key to continue", 10, 570, 0.5);
}


/* ---------------------------------------------------------------------- */


/**************************************************************************/
/*  Draw a string in `ransom note' fashion -                              */
/*      inspired by `ransom.ps' released to comp.sources.postscript       */
/**************************************************************************/

void DrawRansomString(str, x, y, overall_factor)
    char *str;
    int x, y;
    float overall_factor;
{
    int i;
    XPoint *xp;
    float angle;
    int x2, y2;
    int font_num;
    float magnify;
    static char letter[2]={' ', '\0'};
    static char *fontnames[]={
	"-adobe-courier-bold-o-normal--24-240-75-75-m-150-iso8859-1",
	"-adobe-courier-bold-r-normal--24-240-75-75-m-150-iso8859-1",
	"-adobe-helvetica-bold-r-normal--24-240-75-75-p-138-iso8859-1",
	"-adobe-helvetica-bold-o-normal--24-240-75-75-p-138-iso8859-1",
	"-adobe-times-bold-i-normal--24-240-75-75-p-128-iso8859-1",
	"-adobe-times-bold-r-normal--24-240-75-75-p-132-iso8859-1"
	};
    static XFontStruct *font[6]={NULL, NULL, NULL, NULL, NULL};

    /* load fonts if not already loaded */
    if(!font[0]) 
	for(i=0; i<6; i++) 
	    font[i]=LoadFont(fontnames[i]);

    /* take each letter in turn */
    for(i=0; i<strlen(str); i++) {
	letter[0]=str[i];

	/* calculate random angle, offset, font */
	angle=(3.-(float)(rand()%6))*5;
	x2=x+rand()%6;
	y2=y+5-rand()%10;
	font_num=rand()%6;

	/* maybe change from upper case to lower case */
	if(letter[0]>='A' && letter[0]<='Z') {
	    if(rand()%1000>500)
		letter[0]+='a'-'A';
	}
	else if(letter[0]>='a' && letter[0]<='z') {
	    if(rand()%1000>500)
		letter[0]-='a'-'A';
	}
	    
	/* any letter that's not a space */
	if(letter[0]!=' ') {
	    /* random magnification above biggest bitmap font available */
	    magnify=((float)(rand()%100)/200.+1.2)*overall_factor;
	    XRotSetMagnification(magnify);

	    /* random bounding box padding */
	    XRotSetBoundingBoxPad(rand()%3+2);

	    /* get bounding box */
	    xp=XRotTextExtents(dpy, font[font_num], angle, x2, y2, 
			       letter, NONE);

	    /* if bounding box was obtained */
	    if(xp) {
		/* blank out box */
		XSetForeground(dpy, gc, black);
		XFillPolygon(dpy, window, gc, xp, 5, Convex, CoordModeOrigin);

		/* draw box */
		XSetForeground(dpy, gc, white);
		XDrawLines(dpy, window, gc, xp, 5, CoordModeOrigin);

		/* free points */
		free((char *)xp);
	    }

	    /* random colour */
	    XSetForeground(dpy, gc, pixels[rand()%NCOLORS]);

	    /* draw the letter */
	    XRotDrawString(dpy, font[font_num], angle, window, gc,
			   x2, y2, letter);

	    /* advance a bit */
	    x+=(float)XTextWidth(font[font_num], letter, 1)*magnify+10;
	}
	else 
	    /* advance for a space */
	    x+=42.*overall_factor;
    }

    /* reset */
    XRotSetMagnification(1.);
}



/* ---------------------------------------------------------------------- */


/**************************************************************************/
/*  Demonstrate the use of stipples                                       */
/**************************************************************************/

/* stipples only available R4+ (XGetGCValues not in R3) */

#ifndef X11R3

void DrawStipple1()
{
    int i;
    GC gc1;
    Pixmap stipple;
    XPoint *xp;
    int size=70, radius=50;
    char *fontname1=
	"-adobe-times-bold-r-normal--10-100-75-75-p-57-iso8859-1";
    char *fontname2=
	"-adobe-helvetica-bold-o-normal--18-180-75-75-p-104-iso8859-1";
    static XFontStruct *font1=NULL, *font2=NULL;
    char *str1="ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ\
ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHI";
    char *str2="This is done using stipples.\n\nPress any key to continue ...";

    /* load fonts */
    if(!font1) {
	font1=LoadFont(fontname1);
	font2=LoadFont(fontname2);
    }
    
    /* create a stipple and depth one GC */
    stipple=XCreatePixmap(dpy, window, size, size, 1);
    gc1=XCreateGC(dpy, stipple, 0, NULL);

    /* clear the stipple */
    XSetForeground(dpy, gc1, 0);
    XFillRectangle(dpy, stipple, gc1, 0, 0, size, size);

    /* the stipple is just a circle */
    XSetForeground(dpy, gc1, 1);
    XFillArc(dpy, stipple, gc1, (size-radius)/2, (size-radius)/2,
	     radius, radius, 0, 64*360);

    /* there's a circle at the centre of the window */
    XSetTSOrigin(dpy, gc, 300-size/2, 300-size/2);

    /* stipple is passed in through GC */
    XSetFillStyle(dpy, gc, FillStippled);
    XSetStipple(dpy, gc, stipple);

    /* draw loads of strings */
    for(i=0; i<50; i++) {
	XSetForeground(dpy, gc, pixels[(i*2)%NCOLORS]);
	XRotDrawString(dpy, font1, -90. , window, gc, i*12, 0, str1);
    }

    /* reset */
    XSetFillStyle(dpy, gc, FillSolid);

    /* free resources */
    XFreePixmap(dpy, stipple);
    XFreeGC(dpy, gc1);

    /* draw `press any key' bounding box */
    XRotSetBoundingBoxPad(10);
    xp=XRotTextExtents(dpy, font2, 0., 300, 500, str2, BCENTRE);
    if(xp) {
	XSetForeground(dpy, gc, black);
	XFillPolygon(dpy, window, gc, xp, 5, Convex, CoordModeOrigin);
	XSetForeground(dpy, gc, white);
        XDrawLines(dpy, window, gc, xp, 5, CoordModeOrigin);
        free((char *)xp);
    }

    XSetForeground(dpy, gc, white);
    XRotDrawAlignedString(dpy, font2, 0., window, gc, 300, 500,
                               str2, BCENTRE);
}

#endif /*X11R3*/



/* ---------------------------------------------------------------------- */


/**************************************************************************/
/*  Use magnified letters as a stipple                                    */
/**************************************************************************/

/* stipples only available R4+ (XGetGCValues not in R3) */

#ifndef X11R3

void DrawStipple2()
{
    int i;
    GC gc1;
    Pixmap stipple;
    char *fontname=
	"-adobe-courier-bold-r-normal--24-240-75-75-m-150-iso8859-1";
    static XFontStruct *font=NULL;
    char *str="abcabcabcabcabcabcabcabcabcabcabcabcabcabc";

    /* load fonts */
    if(!font) 
	font=LoadFont(fontname);
    
    /* the user won't be able to see anything happening! */
    XSetForeground(dpy, gc, white);
    XRotDrawAlignedString(dpy, font, 0., window, gc, 300, 300,
			  "Please wait a second or two...", MCENTRE);
    XFlush(dpy);

    /* create a stipple and depth one GC */
    stipple=XCreatePixmap(dpy, window, 600, 600, 1);
    gc1=XCreateGC(dpy, stipple, 0, NULL);

    /* clear the stipple */
    XSetForeground(dpy, gc1, 0);
    XFillRectangle(dpy, stipple, gc1, 0, 0, 600, 600);

    /* paint a big `abc' on the stipple */
    XSetForeground(dpy, gc1, 1);
    XRotSetMagnification(12.);
    XRotDrawAlignedString(dpy, font, 0., stipple, gc1,
			  300, 300, "abc", MCENTRE);
    XRotSetMagnification(1.);

    /* stipple is as big as window */
    XSetTSOrigin(dpy, gc, 0, 0);

    /* stipple is passed in through GC */
    XSetFillStyle(dpy, gc, FillStippled);
    XSetStipple(dpy, gc, stipple);
    XSetForeground(dpy, gc, pixels[3]);
    
    XClearWindow(dpy, window);

    /* draw loads of small `abc's */
    for(i=0; i<16; i++) 
        XRotDrawString(dpy, font, 0. , window, gc, 0, 180+i*15, str);

    /* reset */
    XSetFillStyle(dpy, gc, FillSolid);

    /* free resources */
    XFreePixmap(dpy, stipple);
    XFreeGC(dpy, gc1);

    XSetForeground(dpy, gc, white);
    XRotDrawAlignedString(dpy, font, 0., window, gc, 300, 590,
                               "Press any key to continue...", BCENTRE);
}

#endif /*X11R3*/


