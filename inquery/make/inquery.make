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

MAKEFILE=inquery.make

# Library objects

TRANSLATORS=sgml_trans.o \
	    email_trans.o \
	    html_trans.o \
	    dm_trans_tab.o

UI_OBJECTS=scan_qry.o \
	qscan_split_compounds.o

PARSE_OBJECTS=tokenize.o \
	normalize.o \
	spec_scan_tab.o \
	inerror.o

LIB_NAMES=libdocman.a \
	libinquery.a \
	libinqparse.a \
	libinqmerge.a \
	libsvrnet.a \
	libinqtcp.a \
	libinq4inr.a \
	libinqadmin.a \
	libjinquery.a

#
#  The default is to build everything.  (The first rule is the default rule.)
#

all:  tclinq inqueryb inquery xinquery inquery-ip inquery-server inquery-server-tcp inparse inbuild incsparse jbuild mdb_ui getquery fetchdoc

#
#  Implicit rules.
#

include ../make/make.rules

#  Rules for building libraries

libdocman.a: $(TRANSLATORS) inerror.o
	cp $(LIBS)/libdocman.a .
	$(ARCHIVE) libdocman.a $(TRANSLATORS) inerror.o
	$(RANLIB) libdocman.a

libinquery.a: $(PARSE_OBJECTS) $(UI_OBJECTS)
	cp $(LIBS)/libinquery.a .
	$(ARCHIVE) libinquery.a $(PARSE_OBJECTS) $(UI_OBJECTS)
	$(RANLIB) libinquery.a

libinqparse.a: $(PARSE_OBJECTS)
	cp $(LIBS)/libinqparse.a .
	$(ARCHIVE) libinqparse.a $(PARSE_OBJECTS)
	$(RANLIB) libinqparse.a

libinqmerge.a: 
	cp $(LIBS)/libinqmerge.a .
	$(RANLIB) libinqmerge.a

libsvrnet.a: 
	cp $(LIBS)/libsvrnet.a .
	$(RANLIB) libsvrnet.a

libinqtcp.a: inerror.o
	cp $(LIBS)/libinqtcp.a .
	$(ARCHIVE) libinqtcp.a inerror.o
	$(RANLIB) libinqtcp.a

libinq4inr.a:
	cp $(LIBS)/libinq4inr.a .
	$(RANLIB) libinq4inr.a

libinqadmin.a:
	cp $(LIBS)/libinqadmin.a .
	$(RANLIB) libinqadmin.a

libjinquery.a:
	cp $(LIBS)/libjinquery.a .
	$(RANLIB) libjinquery.a

#
#  Rules for building executables.
#
inquery:  ui_utils.o ui.o summary.o libinquery.a libdocman.a
	$(CC) $(CFLAGS) -o inquery ui_utils.o ui.o summary.o -linquery -ldocman -lm $(GENLIBS)

inqueryb:  $(INQROOT)/obj/inqr-b.o $(INQROOT)/obj/inqr.o \
	$(INQROOT)/obj/relevance.o libinquery.a libdocman.a 
	$(CC) $(CFLAGS) -o inqueryb $(INQROOT)/obj/inqr-b.o $(INQROOT)/obj/inqr.o $(INQROOT)/obj/relevance.o -linquery -ldocman -lm $(GENLIBS)

inquery-ip:  client_ui_utils_ip.o client_ui.o inerror.o summary.o libinqtcp.a
	$(CC) $(CFLAGS) -DNETWORK -DUSE_TCPIP -o inquery-ip client_ui_utils_ip.o client_ui.o inerror.o summary.o -linqtcp -lm $(ICNETLIBS) $(GENLIBS)

tclinq:  $(INQROOT)/obj/tclinq.o $(INQROOT)/obj/inq_glue.o \
	$(INQROOT)/obj/inq_highlight.o $(INQROOT)/obj/brf_browse.o \
	$(INQROOT)/obj/btl_browse.o $(INQROOT)/obj/inq_browse_api.o \
	libinquery.a libdocman.a
	$(CC) $(CFLAGS) -o tclinq $(INQROOT)/obj/tclinq.o $(INQROOT)/obj/inq_glue.o $(INQROOT)/obj/inq_highlight.o $(INQROOT)/obj/brf_browse.o $(INQROOT)/obj/btl_browse.o $(INQROOT)/obj/inq_browse_api.o -linquery -ldocman $(TCLLIB) -lm $(GENLIBS)

mul_tclinq:  $(INQROOT)/obj/tclinq.o $(INQROOT)/obj/inq_mul_glue.o \
	$(INQROOT)/obj/text-utils.o $(INQROOT)/obj/inq_highlight.o libinqtcp.a
	$(CC) $(CFLAGS) -o mul_tclinq $(INQROOT)/obj/tclinq.o $(INQROOT)/obj/inq_mul_glue.o $(INQROOT)/obj/text-utils.o $(INQROOT)/obj/inq_highlight.o $(TCLLIB) -linqtcp $(ICNETLIBS) $(GENLIBS) -lm

inquery-server:  $(INQROOT)/obj/inquery-server.o libsvrnet.a libinquery.a libdocman.a
	$(CC) $(CFLAGS) -o inquery-server $(INQROOT)/obj/inquery-server.o -lsvrnet -linquery -ldocman -lm $(ISNETLIBS) $(GENLIBS)

inquery-server-tcp:  $(INQROOT)/obj/inquery-server-tcp.o libsvrnet.a libinquery.a libdocman.a
	$(CC) $(CFLAGS) -o inquery-server-tcp $(INQROOT)/obj/inquery-server-tcp.o -lsvrnet -linquery -ldocman -lm $(ICNETLIBS) $(GENLIBS)

inbuild:  inparse_init.o $(INQROOT)/obj/inbuild.o libinqmerge.a libinqparse.a libdocman.a
	$(CC) $(CFLAGS) -o inbuild inparse_init.o $(INQROOT)/obj/inbuild.o -linqmerge -linqparse -ldocman -lm $(GENLIBS)

inparse:  inparse_init.o inparse_mm.o $(INQROOT)/obj/key_pqbtli.o libinqparse.a libdocman.a
	$(CC) $(CFLAGS) -o inparse inparse_init.o inparse_mm.o $(INQROOT)/obj/key_pqbtli.o -linqparse -ldocman -lm $(GENLIBS)

incsparse:  $(INQROOT)/obj/incsparse.o $(INQROOT)/obj/colsel_parse.o \
	inerror.o $(INQROOT)/obj/key_pqbtli.o $(INQROOT)/obj/cmdline.o \
	libinqparse.a libdocman.a libinqtcp.a libinquery.a
	$(CC) $(CFLAGS) -o incsparse $(INQROOT)/obj/incsparse.o $(INQROOT)/obj/colsel_parse.o inerror.o $(INQROOT)/obj/key_pqbtli.o $(INQROOT)/obj/cmdline.o -linqparse -ldocman -linqtcp -linquery -lm $(ISNETLIBS) $(GENLIBS)

jbuild:  inparse_init.o $(INQROOT)/obj/inbuild.o libinqmerge.a libjinquery.a
	$(CC) $(CFLAGS) -o jbuild inparse_init.o $(INQROOT)/obj/inbuild.o -linqmerge -ljinquery -ldocman -lm $(GENLIBS)

mdb_ui:
	$(CC) -fsigned-char -DINQ_ARCH_solaris -I$(INQROOT)/h \
	-L$(INQROOT)/lib $(INQROOT)/src/mdb_ui.c -o mdb_ui -linqtcp \
	-lsocket -lnsl -lgen -lm

getquery:
	$(CC) -fsigned-char -DINQ_ARCH_solaris -I$(INQROOT)/h \
	-L$(INQROOT)/lib $(INQROOT)/src/get_query.c $(INQROOT)/src/network.c \
	-o getquery -linqtcp -lsocket -lnsl -lgen -lm

fetchdoc:
	$(CC) -fsigned-char -DINQ_ARCH_solaris -I$(INQROOT)/h \
	-L$(INQROOT)/lib $(INQROOT)/src/fetch_doc.c $(INQROOT)/src/network.c \
	-o fetchdoc -linqtcp -lsocket -lnsl -lgen -lm

#
#  Rules for special .o files
#

client_summary.o : summary.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) $(INCLUDE_DIRS) \
        -DNETWORK -o client_summary.o -c $^

client_ui.o : ui.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) $(INCLUDE_DIRS) \
        -DNETWORK -o client_ui.o -c $^
	
client_ui_utils_ip.o : ui_utils.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) $(INCLUDE_DIRS) \
        -DNETWORK -DUSE_TCPIP -o client_ui_utils_ip.o -c $^
	
	




