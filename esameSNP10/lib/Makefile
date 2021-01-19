#
# Makefile for misc library.
#
ROOT=..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

LIBMISC	= libsnp.a
OBJS   =  error.o daemonize.o clrfl.o pathalloc.o setfd.o setfl.o colored_printf.o ttymodes.o common.o

all:	$(LIBMISC) $(OBJS)

$(LIBMISC):	$(OBJS)
	$(AR) rv $(LIBMISC) $?
	$(RANLIB) $(LIBMISC)


clean:
	rm -f *.o a.out core temp.* $(LIBMISC)

include $(ROOT)/Make.libsnp.inc
