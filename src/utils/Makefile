##******************************************************************************
## Utilities Makefile
## Copyright (c) 1997 DEVise Group
## DEVise Color Management
##******************************************************************************
## File: Makefile
## Last modified: Thu Oct 16 13:58:16 1997 by Chris Weaver
##******************************************************************************
## Modification History:
##
## 960509 [CEW]: Original file.
## 961016 [CEW]: Added TEMPLATE_FLAGS, removed -fhuge-objects.
##
##******************************************************************************
##
##
##
##******************************************************************************

UTILS_INCLUDES = -I/s/X11/include -I../color/ -I../utils/ \
	-I../../DTE/Core -I../../DTE/DeviseSpecific -I../../DTE/RTree \
	-I../../graphics -I../../graphics.new -I../../tape -I../../unidata
UTILS_LIBS = ../utils/libutils.a

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

UTILS_CFLAGS = $(UTILS_INCLUDES) $(TEMPLATE_FLAGS) $(OPTFLAG) -Wall -Wno-unused

UTILS_SRC =

UTILS_OBJ = $(UTILS_SRC:.C=.o)

UTILS_LIB = libutils.a

all:	$(UTILS_LIB)

$(UTILS_LIB):	$(UTILS_OBJ) templates.o
	$(AR) cr $@ $(UTILS_OBJ) templates.o

.C.o:
	$(CC) -c $(UTILS_CFLAGS) $<

templates.o:
	$(CC) -c $(UTILS_INCLUDES) \
	$(OPTFLAG) -Wall -Wno-unused templates.C $<

clean:	mostlyclean
	-rm $(UTILS_LIB)

mostlyclean:
	-rm $(UTILS_OBJ) templates.o

depend:
	makedepend -fMakefile $(UTILS_INCLUDES) $(UTILS_SRC)

# DO NOT DELETE
