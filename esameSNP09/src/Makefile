ROOT=..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

PROGS = server client multicaster

all:	$(PROGS)

server:	server.c $(LIBSNP)
	$(CC) $(CFLAGS) $@.c -o $@ $(LDFLAGS) $(LDLIBS) -pthread

%:	%.c $(LIBSNP)
	$(CC) $(CFLAGS) $@.c -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) $(MOREPROGS) $(TEMPFILES) *.o 


include $(ROOT)/Make.libsnp.inc

