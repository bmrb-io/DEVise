CXX	= /s/gcc-2.7.2.1/bin/g++
# Note, change -DSOLARIS as necessary. Replace with a value from Jussi's buffer
# manager
OPT	= -DSOLARIS -ggdb
DEFINES	= -D_DEBUG

MAINS	= test_bit_vec test_ikp test_rtree1 test_aspage test_btree

COMMON_CC	= Common/int_key.cc Common/heap.cc Common/correct_math.cc

DB_UNIX_CC	= DBUnix/dbunix.cc

DB_JUSSI_CC	= DBJussi/dbJussi.cc

PAGES_CC	= PageClasses/fixed_entrysz_p.cc PageClasses/int_keyed_p.cc \
		  PageClasses/stackpage.cc PageClasses/alignedsortedpage.cc

PROGS_CC	= Progs/test_ikp.cc Progs/test_bit_vec.cc Progs/test_rtree1.cc \
		  Progs/test_aspage.cc Progs/test_btree.cc Progs/test_types.cc \
		  Progs/test_rtree_jussi.cc

RTREE_CC	= RTree/rtree.cc RTree/bulk_data.cc

BTREE_CC	= BTree/btreeutils.cc BTree/btree.cc

GLOBAL_CC	= setup.cc

TYPE_SUPPORT_CC	= TypeSupport/typed_key.cc TypeSupport/typed_rtree.cc

SRCS		= $(COMMON_CC) $(DB_UNIX_CC) $(PAGES_CC) $(PROGS_CC) $(RTREE_CC) \
		  $(GLOBAL_CC) $(BTREE_CC) $(TYPE_SUPPORT_CC)

COMMON_OBJ	= Common/int_key.o Common/heap.o Common/correct_math.o

DB_UNIX_OBJ	= DBUnix/dbunix.o

DB_JUSSI_OBJ	= DBJussi/dbJussi.o

PAGES_OBJ	= PageClasses/fixed_entrysz_p.o PageClasses/int_keyed_p.o \
		  PageClasses/stackpage.o PageClasses/alignedsortedpage.o

PROGS_OBJ	= Progs/test_ikp.o Progs/test_bit_vec.o Progs/test_rtree1.o \
		  Progs/test_aspage.o Progs/test_btree.o Progs/test_types \
		  Progs/test_rtree_jussi.o

RTREE_OBJ	= RTree/rtree.o RTree/bulk_data.o

BTREE_OBJ	= BTree/btreeutils.o BTree/btree.o

GLOBAL_OBJ	= setup.o

TYPE_SUPPORT_OBJ= TypeSupport/typed_key.o TypeSupport/typed_rtree.o

OBJS		= $(COMMON_OBJ) $(DB_UNIX_OBJ) $(PAGES_OBJ) $(PROGS_OBJ) \
		  $(RTREE_OBJ) $(GLOBAL_OBJ) $(BTREE_OBJ) $(TYPE_SUPPORT_OBJ)

INCLUDES	= -I. -ICommon -IDBUnix -IPageClasses -IProgs -IRTree -IBTree -IShiftGrids -ITypeSupport -I../JussiBuf/src -IDBJussi

CFLAGS	= -Wcast-qual -Wcomment -Wconversion -Werror -Wformat -Wimplicit -Winline -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wparentheses -Wpointer-arith -Wreturn-type -Wstrict-prototypes -Wswitch -Wtemplate-debugging -Wtraditional -Wtrigraphs $(OPT) $(INCLUDES) $(DEFINES)

LDFLAGS	= -lm

RTREE_LIBS	= $(COMMON_OBJ) $(PAGES_OBJ) $(GLOBAL_OBJ) $(RTREE_OBJ)

TEST_BIT_VEC_LIBS	= $(COMMON_OBJ) Progs/test_bit_vec.o

TEST_IKP_LIBS	= $(COMMON_OBJ) $(DB_UNIX_OBJ) $(PAGES_OBJ) $(GLOBAL_OBJ) Progs/test_ikp.o

TEST_RTREE1_LIBS	= $(COMMON_OBJ) $(DB_UNIX_OBJ) $(PAGES_OBJ) $(GLOBAL_OBJ) $(RTREE_OBJ) Progs/test_rtree1.o

TEST_ASPAGE_LIBS = $(GLOBAL_OBJ) $(COMMON_OBJ) PageClasses/alignedsortedpage.o Progs/test_aspage.o

CONVERT_BULK_LIBS	= $(COMMON_OBJ) TypeSupport/convert_bulk.o

INTERP_FILE_LIBS	= $(COMMON_OBJ) $(TYPE_SUPPORT_OBJ) \
			  TypeSupport/interp_file.o

TEST_TYPES_LIBS		= $(RTREE_LIBS) $(TYPE_SUPPORT_OBJ) $(DB_UNIX_OBJ) Progs/test_types.o

TEST_BTREE_LIBS = $(GLOBAL_OBJ) $(COMMON_OBJ) $(PAGES_OBJ) $(DB_UNIX_OBJ) $(BTREE_OBJ) \
                  Progs/test_btree.o

TEST_DBJUSSI_LIBS	= $(RTREE_LIBS) $(DB_JUSSI_OBJ) Progs/test_rtree_jussi.o

all:	$(MAINS)

convert_bulk:	$(CONVERT_BULK_LIBS) .makedep Makefile
	$(CXX) $(CFLAGS) $(CONVERT_BULK_LIBS) -o TypeSupport/convert_bulk $(LDFLAGS)

test_types:	$(TEST_TYPES_LIBS) .makedep Makefile
	$(CXX) $(CFLAGS) $(TEST_TYPES_LIBS) -o Progs/test_types $(LDFLAGS)

test_rtree_jussi:	$(TEST_DBJUSSI_LIBS) .makedep Makefile
	$(CXX) $(CFLAGS) $(TEST_DBJUSSI_LIBS) -o Progs/test_rtree_jussi $(LDFLAGS) -L../JussiBuf/sun4x_55 -lpthread -lsocket -lnsl -lbm

interp_file:	$(INTERP_FILE_LIBS) .makedep Makefile
	$(CXX) $(CFLAGS) $(INTERP_FILE_LIBS) -o TypeSupport/interp_file $(LDFLAGS)

test_bit_vec:	$(TEST_BIT_VEC_LIBS) .makedep Makefile
	$(CXX) $(CFLAGS) $(TEST_BIT_VEC_LIBS) -o Progs/test_bit_vec $(LDFLAGS)

test_ikp:	$(TEST_IKP_LIBS) .makedep Makefile
	$(CXX) $(CFLAGS) $(TEST_IKP_LIBS) -o Progs/test_ikp $(LDFLAGS)

test_rtree1:	$(TEST_RTREE1_LIBS) .makedep Makefile
	$(CXX) $(CFLAGS) $(TEST_RTREE1_LIBS) -o Progs/test_rtree1 $(LDFLAGS)

test_aspage: $(TEST_ASPAGE_LIBS) .makedep Makefile
	$(CXX) $(CFLAGS) $(TEST_ASPAGE_LIBS) -o Progs/test_aspage $(LDFLAGS)

test_btree: $(TEST_BTREE_LIBS) .makedep Makefile
	$(CXX) $(CFLAGS) $(TEST_BTREE_LIBS) -o Progs/test_btree $(LDFLAGS)

%.o: %.cc Makefile
	$(CXX) $(CFLAGS) $(XCFLAGS) -c -o $@ $<

depend:
	$(CXX) $(CFLAGS) -M $(SRCS) > .makedep
#	@echo '/^# DO NOT DELETE THIS LINE/+2,$$d' > .eddep
#	@echo '$$r .makedep' >>.eddep
#	@echo 'w' >>.eddep
#	@echo 'q' >>.eddep
#	@echo ed - Makefile < .eddep
#	@echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
#	@echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
#	@echo '# see make depend above' >> Makefile
	      
clean:
	rm -f *.o *~ *.a */*.o */*~ */*.a $(MAINS) core

