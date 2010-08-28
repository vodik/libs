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

struct io_backend select_backend = {
	.add = add,
	.rem = rem,
	.poll = poll,
};

struct io_backend *
select_backend_get()
{
	return &select_backend;
}

void
add(int fd, int events)
{
}

void
rem(int fd)
{
}

void
poll(int timeout)
{
}
