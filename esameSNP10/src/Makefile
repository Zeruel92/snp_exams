ROOT=..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

PROGS = esame10

all:	$(PROGS)

%:	%.c $(LIBSNP)
	$(CC) $(CFLAGS) $@.c -o $@ $(LDFLAGS) $(LDLIBS) $(EXTRAS)

clean:
	rm -f $(PROGS) $(MOREPROGS) $(TEMPFILES) *.o 


include $(ROOT)/Make.libsnp.inc

