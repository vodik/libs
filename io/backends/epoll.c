#include "epoll.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include "epoll.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>

#define MAX_EVENTS 1

void add(int fd, int events);
void rem(int fd);
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
get_events(int events) {
	int epoll_events = 0;

	if (events & IO_IN)
		epoll_events |= EPOLLIN;
	if (events & IO_OUT)
		epoll_events |= EPOLLOUT;
	if (events & IO_HUP) {
		epoll_events |= EPOLLRDHUP | EPOLLHUP;
		printf("get poop!\n");
	}

	return epoll_events;
}

static int
read_events(int epoll_events) {
	int events = 0;

	if (epoll_events & EPOLLIN)
		events |= IO_IN;
	if (epoll_events & EPOLLOUT)
		events |= IO_OUT;
	if (epoll_events & EPOLLRDHUP || epoll_events & EPOLLHUP) {
		events |= IO_HUP;
		printf("read poop!\n");
	}
	if (epoll_events & EPOLLERR)
		events |= IO_ERR;

	return events;
}

void
add(int fd, int events)
{
	struct epoll_event ev = { .events = EPOLLET };
	ev.events = get_events(events);

	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		perror("epoll_ctl: EPOLL_CTL_ADD");
		exit(EXIT_FAILURE);
	}
}

void
rem(int fd)
{
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1) {
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

	for (i = 0; i < nfds; ++i)
		epoll_backend.callback(events[i].data.fd, read_events(events[i].events));
}
