CC = gcc
CFLAGS = -Wall -Werror -pedantic -std=gnu99 -O0 -g -I../../..
LDFLAGS = -L ../../io/ -lio

CLIENT_SRC = client.c
SERVER_SRC = server.c

CLIENT_OBJ = ${CLIENT_SRC:.c=.o}
SERVER_OBJ = ${SERVER_SRC:.c=.o}

all: client server

client : ${CLIENT_OBJ}
	@echo CC -o $@
	@${CC} -static ${CLIENT_OBJ} ${LDFLAGS} -o $@

server : ${SERVER_OBJ}
	@echo CC -o $@
	@${CC} -static ${SERVER_OBJ} ${LDFLAGS} -o $@

%.o: %.c
	@echo CC $@
	@${CC} -o $@ -c $< ${CFLAGS}

clean:
	@echo cleaning...
	@rm client server ${OBJ}

.PHONY: all clean
