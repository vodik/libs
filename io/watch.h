#ifndef LIBS_WATCH
#define LIBS_WATCH

#include "io.h"

enum io_events {
	IO_IN  = 0x01,
	IO_HUP = 0x02,
	IO_ERR = 0x04,
};

struct io_backend {
	void (*add)(int fd, int events);
	void (*rem)(int fd);
	void (*poll)(int timeout);

	void (*callback)(int fd, int events);
};

typedef void (*iofunc)(struct io *, int, void *);

void io_watch_init(struct io_backend *backend);
void io_watch(struct io *io, int events, iofunc func, void *data);
void io_unwatch(struct io *io);

void io_poll(int timeout);
void io_run();

#endif
