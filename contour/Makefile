C_COMPILER = g++
CC_COMPILER = g++
CFLAGS = -c -g
STD_INCLUDES = -I/usr/include -I/usr/include/CC -I. -Iinc
CON_INCLUDES = $(STD_INCLUDES) -Iinc/Contour 
FLX_INCLUDES = $(STD_INCLUDES) -Iinc/Felix
LIBS = -lm

BooleanH = inc/Contour/Boolean.h
RealH = inc/Contour/Real.h

#Contour library
ContourLib = lib/libcontour.a

#Felix library
FelixLib = lib/libfelix.a

#Definitions file, dependence list, source file, and object file 
#respectively for ContourScheme 
ContourSchemeH = inc/Contour/ContourScheme.hxx
ContourSchemeD = $(ContourSchemeH) $(BooleanH) $(RealH)
ContourSchemeC = src/Contour/ContourScheme.cxx
ContourSchemeO = obj/Contour/ContourScheme.o

#Definitions file, dependence list, source file, and object file 
#respectively for Drawable2D 
Drawable2DH = inc/Contour/Drawable2D.hxx
Drawable2DD = $(Drawable2DH)
Drawable2DC = src/Contour/Drawable2D.cxx
Drawable2DO = obj/Contour/Drawable2D.o

#Definitions file and dependence list for StreamDrawable
StreamDrawableH = inc/Contour/StreamDrawable.hxx
StreamDrawableD = $(StreamDrawableH) $(Drawable2DD)

#Definitions file, dependence list, source file, and object file 
#respectively for PixelMap 
PixelMapH = inc/Contour/PixelMap.hxx
PixelMapD = $(PixelMapH) $(Drawable2DH)
PixelMapC = src/Contour/PixelMap.cxx
PixelMapO = obj/Contour/PixelMap.o

#Definition file for TemplateList
TemplateListH = inc/Contour/TemplateList.hxx

#Definition and dependence list for BoundaryPointList
BoundaryPointListH = inc/Contour/BoundaryPointList.hxx
BoundaryPointListD = $(TemplateListH) $(BoundaryPointListH) 

#Definitions file, dependence list, source file, and object file 
#respectively for ContourMap 
ContourMapH = inc/Contour/ContourMap.hxx
ContourMapD = $(PixelMapD) $(BoundaryPointListD) $(ContourMapH) $(ContourSchemeD) 
ContourMapC = src/Contour/ContourMap.cxx
ContourMapO = obj/Contour/ContourMap.o

#Definitions file, dependence list, source file, and object file 
#respectively for BinaryFile 
BinaryFileH = inc/Contour/BinaryFile.h
BinaryFileD = $(BinaryFileH)
BinaryFileC = src/Contour/BinaryFile.cxx
BinaryFileO = obj/Contour/BinaryFile.o

#Definitions file, dependence list, source file, and object file 
#respectively for io 
ioH = inc/Felix/io.h
ioD = $(ioH) 
ioC = src/Felix/io.c
ioO = obj/Felix/io.o

#Definitions file, dependence list, source file, and object file 
#respectively for nrtuil 
nrutilH = inc/Felix/nrutil.h
nrutilD = $(nrutilH)
nrutilC = src/Felix/nrutil.c
nrutilO = obj/Felix/nrutil.o

#Definitions file, dependence list, source file, and object file 
#respectively for flxinit 
flxinitH = inc/Felix/flxinit.h
flxinitD = $(flxinitH) $(nrutilD) $(ioD)
flxinitC = src/Felix/flxinit.c
flxinitO = obj/Felix/flxinit.o

#Definitions file, dependence list, source file, and object file 
#respectively for ioflx 
ioflxH = inc/Felix/ioflx.h
ioflxD = $(ioflxH) $(flxinitD) 
ioflxC = src/Felix/ioflx.c
ioflxO = obj/Felix/ioflx.o

#Definitions file, dependence list, source file, and object file 
#respectively for flxpos 
flxposH = inc/Felix/flxpos.h
flxposD = $(flxposH) $(flxinitD)
flxposC = src/Felix/flxpos.c
flxposO = obj/Felix/flxpos.o

#List of object files in contour library
CON_OBJS = $(ContourMapO) $(ContourSchemeO) $(BinaryFileO) \
	$(Drawable2DO) $(PixelMapO)
#List of object files in FELIX library
FLX_OBJS = $(nrutilO) $(ioO) $(ioflxO) $(flxposO) $(flxinitO)

#Source, dependency, object, and executable for application cplot
cplotC = src/cplot.c
cplotD = $(StreamDrawableD) $(ContourMapD)
cplotO = obj/cplot.o
cplotOUT = bin/cplot.out

#Source, dependency, object, and executable for application ppm_plot
ppm_plotC = src/ppm_plot.c
ppm_plotD = $(StreamDrawableD) $(ContourMapD)
ppm_plotO = obj/ppm_plot.o
ppm_plotOUT = bin/ppm_plot.out

#Source, object, and executable for application flx_extract 
flx_extractC = src/flx_extract.c
flx_extractO = obj/flx_extract.o
flx_extractOUT = bin/flx_extract.out

all : $(ContourLib) $(FelixLib) $(cplotO) $(cplotOUT) $(flx_extractO) $(flx_extractOUT) $(ppm_plotO) $(ppm_plotOUT)

clean :
	rm bin/*
	rm lib/*.a
	rm obj/*.o
	rm obj/Contour/*.o
	rm obj/Felix/*.o

$(ContourLib) : $(CON_OBJS)
	ar rc $(ContourLib) $(CON_OBJS) 

$(FelixLib) : $(FLX_OBJS) 
	ar rc $(FelixLib) $(FLX_OBJS) 

$(cplotOUT) : $(cplotO) $(ContourLib) 
	$(CC_COMPILER) -o $(cplotOUT) $(cplotO) $(ContourLib) $(LIBS)

$(ppm_plotOUT) : $(ppm_plotO) $(ContourLib) 
	$(CC_COMPILER) -o $(ppm_plotOUT) $(ppm_plotO) $(ContourLib) $(LIBS)

$(flx_extractOUT) : $(flx_extractO) $(FelixLib) 
	$(CC_COMPILER) -o $(flx_extractOUT) $(flx_extractO) $(FelixLib) $(LIBS)

$(ContourSchemeO) : $(ContourSchemeD) $(ContourSchemeC)
	$(CC_COMPILER) $(CON_INCLUDES) $(CFLAGS) $(ContourSchemeC) 
	mv ContourScheme.o obj/Contour/

$(ContourMapO) : $(ContourMapD) $(ContourMapC) 
	$(CC_COMPILER) $(CON_INCLUDES) $(CFLAGS) $(ContourMapC) 
	mv ContourMap.o obj/Contour/

$(PixelMapO) : $(PixelMapD) $(PixelMapC) 
	$(CC_COMPILER) $(CON_INCLUDES) $(CFLAGS) $(PixelMapC) 
	mv PixelMap.o obj/Contour/

$(Drawable2DO) : $(Drawable2DD) $(Drawable2DC) 
	$(CC_COMPILER) $(CON_INCLUDES) $(CFLAGS) $(Drawable2DC) 
	mv Drawable2D.o obj/Contour/

$(BinaryFileO) : $(BinaryFileD) $(BinaryFileC) 
	$(CC_COMPILER) $(CON_INCLUDES) $(CFLAGS) $(BinaryFileC) 
	mv BinaryFile.o obj/Contour/

$(nrutilO) : $(nrutilD) $(nrutilC)
	$(C_COMPILER) $(FLX_INCLUDES) $(CFLAGS) $(nrutilC) 
	mv nrutil.o obj/Felix/

$(ioO) : $(ioD) $(ioC)
	$(C_COMPILER) $(FLX_INCLUDES) $(CFLAGS) $(ioC) 
	mv io.o obj/Felix/

$(ioflxO) : $(ioflxD) $(ioflxC)
	$(C_COMPILER) $(FLX_INCLUDES) $(CFLAGS) $(ioflxC) 
	mv ioflx.o obj/Felix/

$(flxposO) : $(flxposD) $(flxposC)
	$(C_COMPILER) $(FLX_INCLUDES) $(CFLAGS) $(flxposC) 
	mv flxpos.o obj/Felix/

$(flxinitO) : $(flxpinitD) $(flxinitC)
	$(C_COMPILER) $(FLX_INCLUDES) $(CFLAGS) $(flxinitC) 
	mv flxinit.o obj/Felix/

$(cplotO) : $(cplotC) $(cplotD)
	$(CC_COMPILER) $(CON_INCLUDES) $(CFLAGS) $(cplotC)
	mv cplot.o obj/

$(ppm_plotO) : $(ppm_plotC) $(ppm_plotD)
	$(CC_COMPILER) $(CON_INCLUDES) $(CFLAGS) $(ppm_plotC)
	mv ppm_plot.o obj/

$(flx_extractO) : $(flx_extractC) $(ioflxD)
	$(CC_COMPILER) $(FLX_INCLUDES) $(CFLAGS) $(flx_extractC)
	mv flx_extract.o obj/

