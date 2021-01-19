ROOT=..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

PROGS = primo secondo

all:	primo

%:	%.c $(LIBSNP)
	$(CC) $(CFLAGS) $@.c -o $@ $(LDFLAGS) $(LDLIBS) $(EXTRAS)

systemv:
	$(CC) $(CFLAGS) primo.c -o primo $(LDFLAGS) $(LDLIBS) -DSYSV $(EXTRAS)
	$(CC) $(CFLAGS) secondo.c -o secondo $(LDFLAGS) $(LDLIBS) -DSYSV $(EXTRAS)

posix:
	$(CC) $(CFLAGS) primo.c -o primo $(LDFLAGS) $(LDLIBS) -DPOSIX $(EXTRAS)
	$(CC) $(CFLAGS) secondo.c -o secondo $(LDFLAGS) $(LDLIBS) -DPOSIX $(EXTRAS)

clean:
	rm -f $(PROGS) $(MOREPROGS) $(TEMPFILES) *.o 


include $(ROOT)/Make.libsnp.inc

