CC      ?= gcc
CFLAGS  ?= -Wall -g
LDFLAGS ?=
LDLIBS  := -lm

OBJDIR  := object
OBJS    := $(OBJDIR)/tokenizer.o $(OBJDIR)/shuntingyard.o $(OBJDIR)/evaluator.o

.PHONY: all clean

all: fexpr

fexpr: fexpr.c fexpr.h help.h $(OBJS) | $(OBJDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ fexpr.c $(OBJS) $(LDLIBS)

$(OBJDIR)/%.o: %.c makefile | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) fexpr