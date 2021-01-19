#
# Makefile for misc library.
#
ROOT=..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

LIBMISC	= libsnp.a
OBJS   = daemonize.o error.o pthread_barrier.o
all:	$(LIBMISC)

$(LIBMISC):	$(OBJS)
	$(AR) rv $(LIBMISC) $?
	$(RANLIB) $(LIBMISC)


clean:
	rm -f *.o a.out core temp.* $(LIBMISC)

include $(ROOT)/Make.libsnp.inc
