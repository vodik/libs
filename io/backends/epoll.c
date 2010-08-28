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

void
add(int fd, int events)
{
	struct epoll_event ev = { .events = EPOLLIN };

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
		epoll.callback(events[i].data.fd, IO_IN);
}
