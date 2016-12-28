SRCDIR = $(realpath .)
TOPDIR = $(realpath ..)

include $(TOPDIR)/Make.version
include $(TOPDIR)/Make.rules
include $(TOPDIR)/Make.defaults

MAN1TARGETS =
MAN3TARGETS =

all :

clean :

test :

install :
	$(INSTALL) -d -m 755 $(DESTDIR)$(mandir)/man1
	$(foreach x, $(MAN1TARGETS), $(INSTALL) -m 644 $(x) $(DESTDIR)/$(mandir)/man1/;)
	$(INSTALL) -d -m 755 $(DESTDIR)$(mandir)/man3
	$(foreach x, $(MAN3TARGETS), $(INSTALL) -m 644 $(x) $(DESTDIR)/$(mandir)/man3/;)

.PHONY: all clean install

include $(TOPDIR)/Make.rules
