#include "epoll.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include "epoll.h"
#include "../io_ref.h"

#define MAX_EVENTS 10

void add(struct io *io, int events);
void rem(struct io *io);
void poll(int timeout);

struct io_backend epoll_backend = {
	.add = add,
	.rem = rem,
	.poll = poll,
};

int epfd = 0;
struct epoll_event events[MAX_EVENTS];

static void
epoll_start()
{
	if ((epfd = epoll_create(MAX_EVENTS)) == -1) {
		perror("epoll_create");
		exit(EXIT_FAILURE);
	}
}

struct io_backend *
epoll_backend_get()
{
	if (!epfd)
		epoll_start();
	return &epoll_backend;
}

static int
from_io_events(int events) {
	int epoll_events = 0;

	if (events & IO_IN)
		epoll_events |= EPOLLIN;
	if (events & IO_OUT)
		epoll_events |= EPOLLOUT;
	if (events & IO_HUP)
		epoll_events |= EPOLLRDHUP | EPOLLHUP;

	return epoll_events;
}

static int
to_io_events(int epoll_events) {
	int events = 0;

	if (epoll_events & EPOLLIN)
		events |= IO_IN;
	if (epoll_events & EPOLLOUT)
		events |= IO_OUT;
	if (epoll_events & EPOLLRDHUP || epoll_events & EPOLLHUP)
		events |= IO_HUP;
	if (epoll_events & EPOLLERR)
		events |= IO_ERR;

	return events;
}

void
add(struct io *io, int events)
{
	struct epoll_event ev = { .events = EPOLLET };
	ev.events = from_io_events(events);

	ev.data.ptr = io;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, io->fd, &ev) == -1) {
		perror("epoll_ctl: EPOLL_CTL_ADD");
		exit(EXIT_FAILURE);
	}
}

void
rem(struct io *io)
{
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, io->fd, NULL) == -1) {
		perror("epoll_ctl: EPOLL_CTL_DEL");
		exit(EXIT_FAILURE);
	}
}

void
poll(int timeout)
{
	int nfds, i;

	if ((nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout)) == -1) {
		perror("epoll_wait");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < nfds; ++i) {
		struct io *io = events[i].data.ptr;
		epoll_backend.callback(io->fd, to_io_events(events[i].events), io);
	}
}
