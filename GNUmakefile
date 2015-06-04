# Attempt to use bash instead of sh.
SHELL = /bin/bash

# Configure the environment.
USING_MACOSX := $(shell uname | egrep Darwin)

HAVE_BBEDIT := $(shell which bbedit)
HAVE_CTAGS := $(shell which ctags)

ifdef HAVE_CTAGS
HAVE_EXUBERANT_CTAGS := $(shell ctags --version | egrep Exuberant)
endif


CFLAGS = -MMD -g -O0 -Wall -Wno-unused-value -I. -DLOGGING_API_USES_VARIADIC_MACROS=0

ifdef HAVE_EXUBERANT_CTAGS
# This is the best ctags implementation by far. If you don't have it, consider
# getting it.
CTAGS = ctags --excmd=number --tag-relative=no --fields=+a+m+n+S -f tags -R "$(PWD)"
else
    ifdef HAVE_BBEDIT
    CTAGS = bbedit --maketags
    else
        ifdef HAVE_CTAGS
        CTAGS = ctags -f tags $SRCS
        endif
    endif
endif


DOXYGEN = doxygen
LDFLAGS = -g
LIBS = -lm

OBJS = base64.o \
begetset.o \
factorial.o \
ffactorial.o \
hex.o \
itoa.o \
keyvalue.o \
legetset.o \
logging.o \
portable_timer.o \
ringbuffer.o \
tcputils.o

#LINTPATH = $(HOME)/Applications/pclint
LINTPATH=$(HOME)/pclint
LINT = wine $(LINTPATH)/LINT-NT.EXE
OBJDIR = objs
SRCS = $(OBJS:.o=.c)
TARGETS = misclib

# Directory containing dependency files (*.P).
DEPDIR = .deps
# The directory part and the file-within-directory part of $* inside DEPDIR.
df = $(DEPDIR)/$(*F)


# Targets
misclib: tags $(OBJS)
	$(CC) $(LDFLAGS) -o misclib $(OBJS) $(LIBS)

tags: $(SRCS)
	$(CTAGS)

.PHONY: docs doc

# Generate documentation.
docs:
	$(DOXYGEN) Doxyfile

doc: docs


.PHONY: lint-gcc
# Run static analysis using PC-Lint/Flexe-Lint
# We #include all required headers in all header files (to allow use as a
# library), so lint will complain about system headers getting #included
# multiple times. We supress this (537) warning globally.
lint-gcc: $(SRCS) $(LINTPATH)/lint_cmac.h
	-rm -f _lint.txt
	-$(LINT) +v -i$(LINTPATH) -e537 co-gcc.lnt env-vim.lnt -DLOGGING_API_USES_VARIADIC_MACROS=0 $(SRCS) -summary >_lint.txt
# Use +v* for lots of debug output.
#	-$(LINT) -b -v -i$(LINTPATH) lint/prj-msc100.lnt env-vim.lnt  $(SRCS) -summary\(\) >_lint_msc100.txt

$(LINTPATH)/lint_cmac.h:
	cd $(LINTPATH) && make -f co-gcc.mak && cd -




.PHONY: all
# Builds everything.
all: misclib tags lint docs

.PHONY: clean
# Removes all generated files.
clean:
	-rm $(TARGETS) *.o tags lint/lint_cmac.h lint/gcc-include-path.lnt
	-rm -Rf $(DEPDIR)
	-rm -Rf doc/html doc/rtf

# Create DEPDIR if it does not exist.
$(DEPDIR):
	mkdir $(DEPDIR)

# Targets including this dependency will always be built.
force: ;


# Rules
# Build .o from .c by running $(CC) $(CFLAGS) –c $(.SOURCE)
# Creates dependencies in $(DEPDIR) automatically.
%.o : %.c $(DEPDIR)
	$(COMPILE.c) -MMD -MF $(DEPDIR)/$*.d -o $@ $<

# Include the dependencies we just built above.
-include $(SRCS:%.c=$(DEPDIR)/%.d)
