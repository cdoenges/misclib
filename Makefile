# Configure the environment.
USING_MACOSX := $(shell uname | egrep Darwin)


INCDIR = inc
OBJDIR = obj
SRCDIR = src
SRCS := $(wildcard ${SRCDIR}/*.c)
OBJS := $(addprefix ${OBJDIR}/,$(notdir $(SRCS:.c=.o)))

CC = gcc
CFLAGS = -fPIC -g -O2 -Wall -Wextra -I${INCDIR}
LDFLAGS =   # linking flags
RM = rm -f  # rm command

ifdef USING_MACOSX
    STATIC =
else
    STATIC = -static
endif


.PHONY: all
all: libmisclib.a libmisclib.so unittest/misclibTest


${OBJDIR}:
	mkdir -p $@

libmisclib.a: $(OBJS)
	ar rcs $@ $^

libmisclib.so: $(OBJS)
	$(CC) ${LDFLAGS} -shared -o $@ $^

unittest/misclibTest: $(wildcard unittest/*.c) libmisclib.a
	$(CC) ${STATIC} $(CFLAGS) -L. -lmisclib -o $@ $^


${OBJDIR}/%.o: ${SRCDIR}/%.c ${OBJDIR}
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	-${RM} libmisclib.a libmisclib.so unittest/misclibTest
	-rm -Rf ${OBJDIR}
