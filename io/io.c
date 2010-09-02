#include "io.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "io_ref.h"
#include "watch.c"

static size_t
io_obuf_read(char *buf, size_t len, void *arg)
{
	struct io *io = arg;
	printf("filling from %d %d bytes\n", io->fd, len);
	ssize_t ret = read(io->fd, buf, len);
	if (ret < 0) {
		perror("read");
		exit(EXIT_FAILURE); /* TODO error handling */
	}
	return (size_t)ret;
}

struct io *
io_new_fd(int fd)
{
	struct io *io = malloc(sizeof(struct io));
	io->fd = fd;
	io->refs = 1;

	io->iofunc = NULL;
	io->arg = NULL;
	io->obuf = obuf_new(512, io_obuf_read, io);

	return io;
}

void
io_ref(struct io *io)
{
	++io->refs;
}

void
io_close(struct io *io)
{
	if (--io->refs == 0) {
		close(io->fd);
		free(io);
	}
}

int
io_get_fd(struct io *io)
{
	return io->fd;
}

size_t
io_read(struct io *io, char *buf, size_t len)
{
	return obuf_read(io->obuf, buf, len);
}

size_t
io_write(struct io *io, char *buf, size_t len)
{
	ssize_t ret = write(io->fd, buf, len);
	if (ret < 0)
		return 0; /* TODO error handling */
	return (size_t)ret;
}
