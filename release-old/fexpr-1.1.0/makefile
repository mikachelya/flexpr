CC      ?= gcc
CFLAGS  ?= -Wall -g
LDFLAGS ?=
LDLIBS  := -lm

OBJDIR  := object
OBJS    := $(OBJDIR)/tokenizer.o $(OBJDIR)/shuntingyard.o $(OBJDIR)/evaluator.o
BINDIR  := $(DESTDIR)/usr/bin
MANDIR  := $(DESTDIR)/usr/share/man/man1

.PHONY: all clean install uninstall

all: fexpr

fexpr: fexpr.c fexpr.h help.h $(OBJS) | $(OBJDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ fexpr.c $(OBJS) $(LDLIBS)

$(OBJDIR)/%.o: %.c makefile | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) fexpr

install: fexpr
	# Ensure the target directories exist in the build staging area
	install -d $(BINDIR)
	install -d $(MANDIR)
	
	# Install the binary with proper executable permissions (rwxr-xr-x)
	install -m 755 fexpr $(BINDIR)/fexpr
	
	# Install the manual page with read-only permissions (rw-r--r--)
	# (Uncomment this once your pandoc/ronn manpage is ready)
	# install -m 644 fexpr.1 $(MANDIR)/fexpr.1

uninstall:
	rm -f $(BINDIR)/fexpr
	rm -f $(MANDIR)/fexpr.1