#
# Makefile for all components of DEVise
#

MODULES = graphics graphics.new tape issm compustat parser ams generic multi

all:
	for i in $(MODULES); do echo -- $$i; cd $$i; make; cd ..; done

