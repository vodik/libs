#include "watch.h"

#include <stdlib.h>
#include <stdio.h>
#include "rbtree.h"

#include "backends/epoll.h"

struct store_data {
	struct io *io;
	iofunc func;
	void *arg;
};

static struct rbtree *tree = NULL;

static int
compare_int(void *leftp, void *rightp)
{
	int left = (int)leftp;
	int right = (int)rightp;

	if (left < right)
		return -1;
	else if (left > right)
		return 1;
	else
		return 0;
}

static void
store(struct io *io, iofunc func, void *arg)
{
	if (!tree)
		tree = rbtree_create(compare_int);

	struct store_data *data = malloc(sizeof *data);
	data->io = io;
	data->func = func;
	data->arg = arg;

	/* TODO */
	rbtree_add(tree, (void *)io_get_fd(io), data);
}

static inline void
unstore(int fd)
{
	if (tree)
		rbtree_delete(tree, (void *)fd);
}

static inline struct store_data *
load(int fd)
{
	return tree ? (struct store_data *)rbtree_find(tree, (void *)fd) : NULL;
}

static void
io_event(int fd, int events)
{
	struct store_data *data = load(fd);
	data->func(data->io, IO_IN, data->arg);
}

static inline struct io_backend *
io_backend_get(struct io_backend *override)
{
	static struct io_backend *backend = NULL;

	if (!backend) {
		backend = override ? override : epoll_backend_get();
		backend->callback = io_event;
	}
	return backend;
}

void
io_watch(struct io *io, int events, iofunc func, void *data)
{
	struct io_backend *backend = io_backend_get(NULL);
	int fd = io_get_fd(io);

	backend->add(fd, events);
	store(io, func, data);
}

void
io_unwatch(struct io *io)
{
	int fd = io_get_fd(io);

	struct store_data *data = load(fd);
	if (data) {
		unstore(fd);
		free(data);
	}
}

void
io_poll(int timeout)
{
	struct io_backend *backend = io_backend_get(NULL);
	backend->poll(timeout);
}

void
io_run()
{
	while (1)
		io_poll(-1);
}
