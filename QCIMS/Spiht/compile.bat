rm *.o
make P=$1/codetree E=aritcode D=NONE T=ENCODER -f codetree.mk
rm codetree.o
make P=$1/decdtree E=aritcode D=NONE T=DECODER -f codetree.mk
rm codetree.o
make P=$1/fastcode E=bincode D=BINCODE T=ENCODER -f codetree.mk
rm codetree.o
make P=$1/fastdecd E=bincode D=BINCODE T=DECODER -f codetree.mk
rm codetree.o
rm image_bw.o
make P=$1/progcode E=aritcode D=LOSSLESS T=ENCODER -f codetree.mk
rm codetree.o
make P=$1/progdecd E=aritcode D=LOSSLESS T=DECODER -f codetree.mk
make P=$1/codecolr E=aritcode D=NONE T=ENCODER -f codecolr.mk
rm codecolr.o
make P=$1/decdcolr E=aritcode D=NONE T=DECODER -f codecolr.mk
rm *.o
