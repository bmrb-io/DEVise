CC		= g++

CFLAGS		= -I. $(INC) -Wall \
		  -g \
		  -DALWAYS_TELL_FACT -DALWAYS_TELL_ERROR

INC		= -I$(RBMROOT)/inc -I/u/b/e/beyer/dte/src

RBM_SRCS	= BBoxEntry.c ObjectDescriptor.c RangeBufferManager.c \
		  RangeBufferManagerAccessMethod.c RangeAccessMethod.c \
		  ReplacementPolicy.c ReplacementPolicyRandom.c \
		  MemoryManager.c PageEntry.c \
		  findObjectInfo.c

UTIL_SRCS	= BoundingBox.c BoundingBoxList.c

TEST_SRCS	= testRBM.c testPhonyRAM.c

SUPP_SRCS	= PhonyRAM.c

RBM_OBJS	= $(RBM_SRCS:.c=.o)

UTIL_OBJS	= $(UTIL_SRCS:.c=.o)

TEST_OBJS	= $(TEST_SRCS:.c=.o)

SUPP_OBJS	= $(SUPP_SRCS:.c=.o)

OBJS		= $(RBM_OBJS) $(UTIL_OBJS) $(SUPP_OBJS)

.SUFFIXES:      .h .c .o

.c.o:
		$(CC) $(CFLAGS) -c $<

Default:	$(OBJS)
		mv $(OBJS) $(RBMROOT)/obj/


demo:		$(OBJS) demo.c
		$(CC) $(CFLAGS) demo.c  $(OBJS) \
					-o demo
		mv demo $(RBMROOT)/bin/
		mv $(OBJS) $(RBMROOT)/obj/

testPhonyRAM:	$(OBJS) testPhonyRAM.c
		$(CC) $(CFLAGS) testPhonyRAM.c  $(OBJS) \
					-o testPhonyRAM
		mv $(OBJS) $(RBMROOT)/obj/
		mv testPhonyRAM $(RBMROOT)/bin/

testRBM:	$(OBJS) testRBM.c
		$(CC) $(CFLAGS) testRBM.c  $(OBJS) \
					-o testRBM
		mv testRBM $(RBMROOT)/bin/
		mv $(OBJS) $(RBMROOT)/obj/

clean:
		\rm -f $(RBMROOT)/obj/$(OBJS)
		\rm -f $(RBMROOT)/bin/*
		\rm -f *.o
