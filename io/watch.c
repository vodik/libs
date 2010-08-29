#include "watch.h"

#include <stdlib.h>
#include <stdio.h>
#include "io_store.h"
#include "io_ref.h"

#include "backends/epoll.h"

static struct io_store *store = NULL;

static void
io_store(struct io *io, iofunc func, void *arg)
{
	if (!store)
		store = io_store_create();

	io->iofunc = func;
	io->arg = arg;
	io_store_add(store, io);
}

static void
io_event(int fd, int events)
{
	if (store) {
		struct io *io = io_store_find(store, fd);
		io->iofunc(io, events, io->arg);
	}
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
io_watch(struct io *io, int events, iofunc func, void *arg)
{
	struct io_backend *backend = io_backend_get(NULL);

	backend->add(io->fd, events);
	io_store(io, func, arg);
}

void
io_unwatch(struct io *io)
{
	if (store)
		io_store_delete(store, io);
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
