#include "watch.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include "io_ref.h"

static const int MAX_EVENTS = 10;

static inline int
get_epoll_fd()
{
	static int epfd = 0;

	if (epfd)
		return epfd;
	else if ((epfd = epoll_create(MAX_EVENTS)) == -1) {
		perror("epoll_create");
		exit(EXIT_FAILURE);
	}
	return epfd;
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
io_watch(struct io *io, int events, iofunc func, void *arg)
{
	int epfd = get_epoll_fd();
	struct epoll_event ev = { .events = EPOLLET };
	ev.events = from_io_events(events);

	io->iofunc = func;
	io->arg = arg;

	ev.data.ptr = io;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, io->fd, &ev) == -1) {
		perror("epoll_ctl: EPOLL_CTL_ADD");
		exit(EXIT_FAILURE);
	}
}

void
io_unwatch(struct io *io)
{
	int epfd = get_epoll_fd();
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, io->fd, NULL) == -1) {
		perror("epoll_ctl: EPOLL_CTL_DEL");
		exit(EXIT_FAILURE);
	}
}

void
io_poll(int timeout)
{
	int epfd = get_epoll_fd();
	int nfds, i;
	struct epoll_event events[MAX_EVENTS];

	if ((nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout)) == -1) {
		perror("epoll_wait");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < nfds; ++i) {
		struct io *io = events[i].data.ptr;
		io->iofunc(io, to_io_events(events[i].events), io->arg);
	}
}

void
io_run()
{
	while (1)
		io_poll(-1);
}
