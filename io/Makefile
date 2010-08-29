CC = gcc
CFLAGS = -Wall -Werror -pedantic -std=gnu99 -O0 -g -I../../..

LIB = libio.a
SRC = io.c watch.c
OBJ = ${SRC:.c=.o}

all: ${LIB}

${LIB} : ${OBJ}
	@echo AR $@
	@${AR} rcs $@ ${OBJ}

%.o: %.c
	@echo CC $@
	@${CC} -o $@ -c $< ${CFLAGS}

clean:
	@echo cleaning...
	@rm ${LIB}  ${OBJ}

.PHONY: all clean
