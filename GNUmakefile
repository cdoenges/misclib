# Attempt to use bash instead of sh.
SHELL = /bin/bash

CFLAGS = -MMD -g -O0 -Wall -I.

# Note: This is the command-line for exuberant ctags, located in /opt/local/bin/ctags.
# Your installation may be different, requiring a different path.
# (Extra credit for dynamically determining the correct executable to use. :)
CTAGS  = /opt/local/bin/ctags --excmd=number --tag-relative=no --fields=+a+m+n+S -f tags -R "$(PWD)" -R ../viwa
# Use the ctags built into BBEdit (Mac OS X).
#CTAGS   = bbedit --maketags
DOXYGEN = doxygen
LDFLAGS = -g
LIBS = -lm

OBJS = base64.o \
begetset.o \
itoa.o \
legetset.o \
portable_timer.o \
factorial.o \
hex.o \
keyvalue.o \
logging.o \
tcputils.o

LINTPATH = $(HOME)/Applications/pclint
LINT = wine $(LINTPATH)/LINT-NT.EXE
#LINT=/cygdrive/c/Programme/pclint/lint-nt.exe
OBJDIR = objs
SRCS = $(OBJS:.o=.c) $(VIWA_OBJS:.o=.c)
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


.PHONY: lint
# Run static analysis using PC-Lint/Flexe-Lint
lint: $(SRCS) $(LINTPATH)/lint_cmac.h
	-rm -f _lint.txt
	-$(LINT) +v -i$(LINTPATH) co-gcc.lnt env-vim.lnt  $(SRCS) -summary >_lint.txt
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
