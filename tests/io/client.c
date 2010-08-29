#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <libs/io/io.h>
#include <libs/io/watch.h>

struct io *conn = NULL;

void
stdin_send(struct io *io, int events, void *arg)
{
	if (events == IO_IN) {
		char buf[1024];
		size_t ret = io_read(io, buf, 1024);

		if (buf[ret - 1] == '\n')
			buf[ret - 1] = '\0';
		else
			buf[ret] = '\0';

		if (strcmp(buf, "quit") == 0) {
			io_close(conn);
			exit(0);
		}
		else
			io_write(conn, buf, ret);
	}
}

int
main(int argc, char *argv[])
{
	struct sockaddr_un addr;
	size_t addr_len;

	int fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket");
		exit(1);
	}

	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, "/tmp/nmd_ctrl");
	addr_len = sizeof(addr);

	if (connect(fd, (struct sockaddr *)&addr, addr_len) != 0) {
		perror("connect");
		exit(1);
	}

	conn = io_new_fd(fd);

	struct io *io_in = io_new_fd(0);
	io_watch(io_in, IO_IN, stdin_send, NULL);
	io_run();
}
