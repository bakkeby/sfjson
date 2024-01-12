# sfjson - a key-value file storage solution
# See LICENSE file for copyright and license details.

include config.mk

SRC = sfjson.c
OBJ = $(SRC:.c=.o)

all: sfjson

.c.o:
	$(CC) -c $(CFLAGS) $<

sfjson: sfjson.o
	$(CC) -o $@ sfjson.o $(LDFLAGS)

clean:
	rm -f sfjson $(OBJ) sfjson-$(VERSION).tar.gz

dist: clean
	mkdir -p sfjson-$(VERSION)
	cp LICENSE Makefile ${SRC} config.mk sfjson-$(VERSION)
	tar -cf sfjson-$(VERSION).tar sfjson-$(VERSION)
	gzip sfjson-$(VERSION).tar
	rm -rf sfjson-$(VERSION)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f sfjson $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/sfjson

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/sfjson

.PHONY: all clean dist install uninstall
