CC      ?= gcc
CFLAGS  ?= -Wall -g
LDFLAGS ?=
LDLIBS  := -lm

OBJDIR  := object
OBJS    := $(OBJDIR)/tokenizer.o $(OBJDIR)/shuntingyard.o $(OBJDIR)/evaluator.o
prefix = /usr
BINDIR  := $(DESTDIR)$(prefix)/bin
MANDIR  := $(DESTDIR)$(prefix)/share/man/man1

.PHONY: all clean install uninstall

all: flexpr

flexpr: flexpr.c flexpr.h help.h $(OBJS) | $(OBJDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ flexpr.c $(OBJS) $(LDLIBS)

$(OBJDIR)/%.o: %.c makefile | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) flexpr

install: flexpr
	# Ensure the target directories exist in the build staging area
	install -d $(BINDIR)
	install -d $(MANDIR)
	
	# Install the binary with proper executable permissions (rwxr-xr-x)
	install -m 755 flexpr $(BINDIR)/flexpr
	
	# Install the manual page with read-only permissions (rw-r--r--)
	# (Uncomment this once your pandoc/ronn manpage is ready)
	# install -m 644 flexpr.1 $(MANDIR)/flexpr.1

uninstall:
	rm -f $(BINDIR)/flexpr
	rm -f $(MANDIR)/flexpr.1

zip-%:
	tar -cvzf ../flexpr-$*.tar.gz *.c *.h LICENSE.md makefile