CC = gcc
CFLAGS = -Wall -Werror -pedantic -std=gnu99 -O0 -g -I../../..
LDFLAGS = -L../../io/ -lio -L../../thr_pool -lthrpool -lpthread

EXE = test
SRC = main.c
OBJ = ${SRC:.c=.o}

all: ${EXE}

${EXE} : ${OBJ}
	@echo CC -o $@
	@${CC} -static ${OBJ} ${LDFLAGS} -o $@

%.o: %.c
	@echo CC $@
	@${CC} -o $@ -c $< ${CFLAGS}

clean:
	@echo cleaning...
	@rm ${EXE} ${OBJ}

.PHONY: all clean
