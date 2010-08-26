#include "io.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

struct io {
	int fd;
};

struct io *
io_new_fd(int fd)
{
	struct io *io = malloc(sizeof *io);
	io->fd = fd;
	return io;
}

void
io_close(struct io *io)
{
	close(io->fd);
	free(io);
}

int
io_get_fd(struct io *io)
{
	return io->fd;
}

size_t
io_read(struct io *io, char *buf, size_t len)
{
	ssize_t ret = read(io->fd, buf, len);
	if (ret < 0)
		return 0; /* TODO error handling */
	return (size_t)ret;
}

size_t
io_write(struct io *io, char *buf, size_t len)
{
	ssize_t ret = write(io->fd, buf, len);
	if (ret < 0)
		return 0; /* TODO error handling */
	return (size_t)ret;
}
