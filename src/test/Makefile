SRCDIR = $(realpath .)
TOPDIR = $(realpath $(SRCDIR)/../..)

include $(TOPDIR)/Make.defaults
include $(TOPDIR)/Make.version

ccldflags += -L$(TOPDIR)/src/ -Wl,-rpath=$(TOPDIR)/src/
LIBS=illoader

all : tester

install :

clean :
	@rm -rfv tester *.o *.E *.S

test : tester
	./tester

tester : | ../arena.c
tester : tester.o
	$(CC) $(cflags) $(LDFLAGS) -Wl,-rpath,$(TOPDIR)/src -L$(TOPDIR)/src -o $@ $^ -lilloader

.PHONY: all clean install test

include $(TOPDIR)/Make.rules
