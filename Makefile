#
# Makefile
# Adrian Perez, 2010-07-27 17:39
#

CFLAGS  ?= -O0 -g -Wall -W -std=c99
DESTDIR ?=
prefix  ?= /usr/local


all: vfand vfand-temperature

vfand: sonypi.o sysfs.o vfand.o
vfand-temperature: sonypi.o sysfs.o vfand-temperature.o

man: vfand.8 vfand-temperature.8

%.8: %.rst
	rst2man $< $@


clean:
	$(RM) vfand.o vfand-temperature.o sonypi.o sysfs.o
	$(RM) vfand vfand-temperature


install:
	install -d $(DESTDIR)$(prefix)/share/man/man8
	install -m 644 vfand.8 vfand-temperature.8 \
		$(DESTDIR)$(prefix)/share/man/man8
	install -d $(DESTDIR)$(prefix)/bin
	install -m 755 vfand vfand-temperature \
		$(DESTDIR)$(prefix)/bin


.PHONY: man install

# vim:ft=make
#

