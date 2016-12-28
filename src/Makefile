SRCDIR = $(realpath .)
TOPDIR = $(realpath ..)

include $(TOPDIR)/Make.version
include $(TOPDIR)/Make.rules
include $(TOPDIR)/Make.defaults

LIBTARGETS=libilloader.so
STATICLIBTARGETS=
BINTARGETS=
PCTARGETS=libilloader.pc
TARGETS=$(LIBTARGETS) $(STATICLIBTARGETS) $(BINTARGETS) $(PCTARGETS) \
	$(patsubst %.so,%.so.1,$(LIBTARGETS))

LIBILLOADER_SOURCES = ll.c arena.c
LIBILLOADER_OBJECTS = $(patsubst %.c,%.o,$(LIBILLOADER_SOURCES))
ALL_SOURCES = \
	$(LIBILLOADER_SOURCES) \
	$(wildcard include/libilloader/*.h)

$(call deps-of,$(ALL_SOURCES)) : | deps
-include $(call deps-of,$(ALL_SOURCES))

all : $(TARGETS)

libilloader.so : $(LIBILLOADER_OBJECTS)

deps : $(ALL_SOURCES)
	$(MAKE) -f $(SRCDIR)/Make.deps deps SOURCES="$(ALL_SOURCES)"

clean : 
	@rm -rfv *~ *.o *.a *.E *.so *.so.* *.pc *.bin .*.d *.map \
		$(BINTARGETS)
	@# remove the deps files we used to create, as well.
	@rm -rfv .*.P .*.h.P *.S.P include/libilloader/.*.h.P
	@$(MAKE) -C test TOPDIR=$(TOPDIR) $@

install : all
	$(INSTALL) -d -m 755 $(DESTDIR)$(libdir)
	$(foreach x,$(LIBTARGETS), \
		$(INSTALL) -m 755 $(x) $(DESTDIR)$(libdir)/$(x).1.$(VERSION) ;\
		ln -fs $(x).1.$(VERSION) $(DESTDIR)$(libdir)/$(x).1 ;\
		ln -fs $(x).1.$(VERSION) $(DESTDIR)$(libdir)/$(x) ;\
		)
	$(INSTALL) -d -m 755 $(DESTDIR)$(PCDIR)
	$(foreach x, $(PCTARGETS), $(INSTALL) -m 644 $(x) $(DESTDIR)$(PCDIR) ;)
	$(INSTALL) -d -m 755 $(DESTDIR)$(includedir)/libilloader
	$(foreach x, $(wildcard $(TOPDIR)/src/include/libilloader/*.h), $(INSTALL) -m 644 $(x) $(DESTDIR)$(includedir)/libilloader/$(notdir $(x));)
	$(INSTALL) -d -m 755 $(DESTDIR)$(bindir)
	$(foreach x, $(BINTARGETS), $(INSTALL) -m 755 $(x) $(DESTDIR)$(bindir);)

test : all
	$(NM) libilloader.so | grep ' T [^_]' | cut -d\  -f3 | sort > symbols
	cmp symbols libilloader.symbols
	$(MAKE) -C test $@

.PHONY: test deps
.SECONDARY : liblibilloader.so.1.$(VERSION) liblibilloader.so.1
.INTERMEDIATE : symbols
