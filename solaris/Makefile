#
# Makefile for all components of DEVise
#

MODULES = graphics graphics.new tape issm compustat crsp parser \
	generic attrproj multi

all clean mostlyclean:
	for i in $(MODULES); do echo -- $$i; cd $$i; make $@; cd ..; done

