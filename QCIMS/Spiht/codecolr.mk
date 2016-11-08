C=g++ -O2

$P : general.o $E.o image_3c.o codecolr.o
	$C -o $P general.o $E.o image_3c.o codecolr.o -lm

general.o  : general.h general.C
	$C -c general.C

$E.o : general.h $E.h $E.C
	$C -c $E.C

image_3c.o : general.h image_3c.h image_3c.C
	$C -c -D$D image_3c.C

codecolr.o : general.h $E.h image_3c.h codecolr.C
	$C -c -D$T -D$D codecolr.C
