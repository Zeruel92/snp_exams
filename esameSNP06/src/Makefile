ROOT=..
PLATFORM=$(shell $(ROOT)/systype.sh)
EXTRALIBS=-pthread
include $(ROOT)/Make.defines.$(PLATFORM)

PROGS = proc1 proc2

all:	$(PROGS)

%:	%.c $(LIBSNP)
	$(CC) $(CFLAGS) $@.c -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) $(MOREPROGS) $(TEMPFILES) *.o 


include $(ROOT)/Make.libsnp.inc

