########################################################################### 
#                         Copyright (c) 1996                              #
# by Sovereign Hill Software, Inc., Amherst, Massachusetts, U.S.A.        #
#        (413-256-6916;   http://www.sovereign-hill.com)                  #
#                          All Rights Reserved.                           #
#                                                                         #
# The SOFTWARE was developed in part at the Center for Intelligent        #
# Information Retrieval (CIIR) at the University of Massachusetts at      #
# Amherst (For more information, contact 413-545-0463 or                  #
# http://ciir.cs.umass.edu).  The SOFTWARE was acquired by Sovereign      #
# Hill Software Inc. and is the property of Sovereign Hill Software.      #
# ACSIOM is authorized, under an agreement with Sovereign Hill Software,  #
# to license the SOFTWARE for non-commercial, non-revenue-producing       #
# prototyping and internal evaluation purposes only.                      #
#                                                                         #
# This SOFTWARE was developed in significant part with private funding.   #
# The INQUERY SOFTWARE is proprietary to Sovereign Hill Software and may  #
# not be distributed in whole or in part or used in any way without       #
# authorized permission in writing from Sovereign Hill Software.          #
# Possession, use, duplication or dissemination of the INQUERY SOFTWARE   #
# and media is authorized only pursuant to a valid written license from   #
# ACSIOM or Sovereign Hill Software, Inc.                                 #
#                                                                         #
# RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the U.S.  #
# Government is subject to restrictions as set forth in subparagraph      #
# (c)(l)(ii) of the Rights in Technical Data and Computer Software clause #
# at DFARS 252.227-7013, or in FAR 52.227-19, as applicable.              #
########################################################################### 

SHELL=/bin/sh

LIB_NAMES=libdocman.a \
	libinquery.a \
	libinqparse.a \
	libinqmerge.a \
	libsvrnet.a \
	libinqtcp.a \
	libinq4inr.a \
	libinqadmin.a \
	libjinquery.a

OBJECTS=inquery \
	inbuild \
	inparse \
	inquery-ip \
	inquery-server \
	inquery-server-tcp \
	inqueryb \
	tclinq \
	mul_tclinq \
	mdb_ui \
	getquery \
	fetchdoc

# binaries that are included in the release but that
# are not customizable;  this is included to make 
# the custom installation complete

NONCOMP=conn_server_ip \
	deval \
	deval2 \
	deval3 \
	dumpstops \
	inf_util \
	merge_btl \
	xinquery \
	qbtl \
	set_col \
	list_cols \
	filtera \
	mkdoctab \
	parse_inq \
	pmerge \
	build_infinder \
	incsparse \
	jbuild \
	deldoc \
	purgedb-key \
	build_docsum \
	inrecover \
	db_util


all: $(OBJECTS)

include ../make/make.rules

# Installation targets

install: install_libs install_bin

install_libs: 
	@ if test -f *.c -o -f *.l ; then $(MAKE) cust_lib_install ; else $(MAKE) static_lib_install ; fi

static_lib_install:
	@ echo "Standard library installation."
	cp $(LIBS)/* $(LIB_DIR)

cust_lib_install: $(LIB_NAMES)
	@ echo "Custom library installation."
	cp $(LIB_NAMES) $(LIB_DIR)

install_bin: 
	@ if test -f *.c -o -f *.l ; then $(MAKE) cust_bin_install ; else $(MAKE) static_bin_install ; fi

static_bin_install: 
	@ echo "Standard binary installation."
	cp $(INQROOT)/binary/* $(INSTALL_DIR)

cust_bin_install: $(OBJECTS) 
	@ echo "Custom binary installation."
	cp $(OBJECTS) $(INSTALL_DIR)
	(cd $(INQROOT)/binary; cp $(NONCOMP) $(INSTALL_DIR))

# Library targets 

libdocman.a::
	$(MAKE) -f $(INQROOT)/make/inquery.make libdocman.a

libinquery.a::
	$(MAKE) -f $(INQROOT)/make/inquery.make libinquery.a

libinqparse.a::
	$(MAKE) -f $(INQROOT)/make/inquery.make libinqparse.a

libinqmerge.a::
	$(MAKE) -f $(INQROOT)/make/inquery.make libinqmerge.a

libsvrnet.a::
	$(MAKE) -f $(INQROOT)/make/inquery.make libsvrnet.a

libinqtcp.a::
	$(MAKE) -f $(INQROOT)/make/inquery.make libinqtcp.a

libinq4inr.a::
	$(MAKE) -f $(INQROOT)/make/inquery.make libinq4inr.a

libinqadmin.a::
	$(MAKE) -f $(INQROOT)/make/inquery.make libinqadmin.a

libjinquery.a::
	$(MAKE) -f $(INQROOT)/make/inquery.make libjinquery.a


# Executable targets

tclinq::
	$(MAKE) -f $(INQROOT)/make/inquery.make tclinq

mul_tclinq::
	$(MAKE) -f $(INQROOT)/make/inquery.make mul_tclinq

inquery::
	$(MAKE) -f $(INQROOT)/make/inquery.make inquery

inqueryb::
	$(MAKE) -f $(INQROOT)/make/inquery.make inqueryb

inquery-ip::
	$(MAKE) -f $(INQROOT)/make/inquery.make inquery-ip

inquery-server::
	$(MAKE) -f $(INQROOT)/make/inquery.make inquery-server

inquery-server-tcp::
	$(MAKE) -f $(INQROOT)/make/inquery.make inquery-server-tcp

inbuild::
	$(MAKE) -f $(INQROOT)/make/inquery.make inbuild

inparse::
	$(MAKE) -f $(INQROOT)/make/inquery.make inparse

jbuild::
	$(MAKE) -f $(INQROOT)/make/inquery.make jbuild

incsparse::
	$(MAKE) -f $(INQROOT)/make/inquery.make incsparse

mdb_ui::
	$(MAKE) -f $(INQROOT)/make/inquery.make mdb_ui

getquery::
	$(MAKE) -f $(INQROOT)/make/inquery.make getquery

fetchdoc::
	$(MAKE) -f $(INQROOT)/make/inquery.make fetchdoc

# administrative targets

test:	
	(cd $(INQROOT)/tests; \
	verify-time-sgml )

clean:
	rm -f *.o *~

realclean: clean
	rm -f $(OBJECTS) *.save lib*.a


