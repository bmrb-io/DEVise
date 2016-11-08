C=g++ -O2

$P : general.o $E.o image_bw.o codetree.o
	$C -o $P general.o $E.o image_bw.o codetree.o -lm

general.o  : general.h general.C
	$C -c general.C

$E.o : general.h $E.h $E.C
	$C -c $E.C

image_bw.o : general.h image_bw.h image_bw.C
	$C -c -D$D image_bw.C

codetree.o : general.h $E.h image_bw.h codetree.C
	$C -c -D$T -D$D codetree.C
