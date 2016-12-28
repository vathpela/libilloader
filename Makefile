TOPDIR = $(shell echo $$PWD)

include $(TOPDIR)/Make.version
include $(TOPDIR)/Make.rules
include $(TOPDIR)/Make.defaults

SUBDIRS := src docs

all : | libilloader.spec Make.version
all :
	@set -e ; for x in $(SUBDIRS) ; do \
		$(MAKE) -C $$x $@ ; \
	done

install :
	@set -e ; for x in $(SUBDIRS) ; do \
		$(MAKE) -C $$x $@ ; \
	done

libilloader.so :
	$(MAKE) -C src $@

$(SUBDIRS) :
	$(MAKE) -C $@

test : all
	@set -e ; for x in $(SUBDIRS) ; do $(MAKE) -C $${x} test ; done

.PHONY: $(SUBDIRS) test

libilloader.spec : | Makefile Make.version

clean :
	@set -e ; for x in $(SUBDIRS) ; do \
		$(MAKE) -C $$x $@ ; \
	done
	@rm -vf libilloader.spec

GITTAG = $(VERSION)

test-archive: libilloader.spec
	@rm -rf /tmp/libilloader-$(VERSION) /tmp/libilloader-$(VERSION)-tmp
	@mkdir -p /tmp/libilloader-$(VERSION)-tmp
	@git archive --format=tar $(shell git branch | awk '/^*/ { print $$2 }') | ( cd /tmp/libilloader-$(VERSION)-tmp/ ; tar x )
	@git diff | ( cd /tmp/libilloader-$(VERSION)-tmp/ ; patch -s -p1 -b -z .gitdiff )
	@mv /tmp/libilloader-$(VERSION)-tmp/ /tmp/libilloader-$(VERSION)/
	@cp libilloader.spec /tmp/libilloader-$(VERSION)/
	@dir=$$PWD; cd /tmp; tar -c --bzip2 -f $$dir/libilloader-$(VERSION).tar.bz2 libilloader-$(VERSION)
	@rm -rf /tmp/libilloader-$(VERSION)
	@echo "The archive is in libilloader-$(VERSION).tar.bz2"

tag:
	git tag -s $(GITTAG) refs/heads/master

archive: tag libilloader.spec
	@rm -rf /tmp/libilloader-$(VERSION) /tmp/libilloader-$(VERSION)-tmp
	@mkdir -p /tmp/libilloader-$(VERSION)-tmp
	@git archive --format=tar $(GITTAG) | ( cd /tmp/libilloader-$(VERSION)-tmp/ ; tar x )
	@mv /tmp/libilloader-$(VERSION)-tmp/ /tmp/libilloader-$(VERSION)/
	@cp libilloader.spec /tmp/libilloader-$(VERSION)/
	@dir=$$PWD; cd /tmp; tar -c --bzip2 -f $$dir/libilloader-$(VERSION).tar.bz2 libilloader-$(VERSION)
	@rm -rf /tmp/libilloader-$(VERSION)
	@echo "The archive is in libilloader-$(VERSION).tar.bz2"


