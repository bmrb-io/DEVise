##******************************************************************************
## Color Makefile
## Copyright (c) 1997 DEVise Group
## DEVise Color Management
##******************************************************************************
## File: Makefile
## Last modified: Thu Oct 16 13:58:21 1997 by Chris Weaver
##******************************************************************************
## Modification History:
##
## 960509 [weaver]: Original file.
## 961016 [CEW]: Added TEMPLATE_FLAGS, removed -fhuge-objects.
##
##******************************************************************************
##
##
##
##******************************************************************************

COLOR_INCLUDES = -I/s/X11/include -I../color/ -I../utils/
COLOR_LIBS = -L/s/X11/include -L../color/ -L../utils/ \
	-lX11 ../color/libcolor.a ../utils/libutils.a

CC = g++
AR = ar

ifeq ($(DEV_DEBUG),)
	OPTFLAG =
else
    ifeq ($(DEV_DEBUG),-1)
	OPTFLAG = -O
    else
	OPTFLAG = -gstabs
	endif
endif

TEMPLATE_FLAGS = -fno-implicit-templates

COLOR_CFLAGS = $(COLOR_INCLUDES) $(TEMPLATE_FLAGS) $(OPTFLAG) -Wall -Wno-unused

COLOR_SRC = ActivePalette.C Color.C ColorManager.C Coloring.C HSVColor.C \
	Palette.C PaletteColor.C PaletteManager.C RGB.C RGBColor.C TriColor.C \
	XColorManager.C

COLOR_OBJ = $(COLOR_SRC:.C=.o)

COLOR_LIB = libcolor.a

all:	$(COLOR_LIB)

$(COLOR_LIB):	$(COLOR_OBJ)
	$(AR) cr $@ $(COLOR_OBJ)

.C.o:
	$(CC) -c $(COLOR_CFLAGS) $<

clean:	mostlyclean
	-rm $(COLOR_LIB)

mostlyclean:
	-rm $(COLOR_OBJ) core

depend:	
	makedepend -fMakefile $(COLOR_INCLUDES) $(COLOR_SRC)

# DO NOT DELETE