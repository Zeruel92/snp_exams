ROOT=..
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

PROGS =	Server Client

all:	$(PROGS)

%:	%.c $(LIBAPUE)
	$(CC) $(CFLAGS) $@.c -o $@ $(LDFLAGS) $(LDLIBS) -pthread

clean:
	rm -rf $(PROGS) $(TEMPFILES) *.o test.txt file_random.r file_random.rr *.dSYM core filediprova.txt .DS_Store client_file_out.txt

include $(ROOT)/Make.libsnp.inc
