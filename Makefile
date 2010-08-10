#
# Makefile
# Adrian Perez, 2010-07-27 17:39
#

CFLAGS  ?= -O0 -g -Wall -W
DESTDIR ?=
prefix  ?= /usr/local


all: vfand

vfand: sonypi.o vfand.o

man: vfand.8

vfand.8: vfand.rst
	rst2man $< $@

clean:
	$(RM) vfand.o sonypi.o
	$(RM) vfand


install:
	install -d $(DESTDIR)$(prefix)/share/man/man8
	install -m 644 vfand.8 $(DESTDIR)$(prefix)/share/man/man8
	install -d $(DESTDIR)$(prefix)/bin
	install -m 755 vfand $(DESTDIR)$(prefix)/bin


.PHONY: man install

# vim:ft=make
#

