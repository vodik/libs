#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <libs/io/io.h>

struct io *server = NULL;

void start(const char *path);
void server_accept(struct io *io, int events, void *arg);
void client_in(struct io *io, int events, void *arg);

void
start(const char *path)
{
	int fd;
	struct sockaddr_un addr;
	int buf_len;

	fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	buf_len = sizeof(addr.sun_path);
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, path, buf_len);

	unlink(addr.sun_path);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		close(fd);
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if (listen(fd, 5) != 0) {
		close(fd);
		perror("listen");
		exit(EXIT_FAILURE);
	}

	server = io_new_fd(fd);
	io_watch(server, IO_IN, server_accept, NULL);
}

void
server_accept(struct io *io, int events, void *arg)
{
	int cfd, fd = io_get_fd(io);
	struct sockaddr_un addr = { 0 };
	size_t addr_len = sizeof(addr);
	struct io *client;

	if (events & IO_IN) {
		if ((cfd = accept(fd, (struct sockaddr *)&addr, &addr_len)) > -1) {
			client = io_new_fd(cfd);
			printf("-- [%02d] connected\n", cfd);
			io_watch(client, IO_IN | IO_HUP, client_in, NULL);
		}
	}
}

void
client_in(struct io *io, int events, void *arg)
{
	if (events & IO_HUP) {
		printf("-- [%02d] disconnected\n", io_get_fd(io));
		io_close(io);
	} else if (events & IO_IN) {
		char buf[1024];
		size_t ret;

		ret = io_read(io, buf, 1024);
		buf[ret] = '\0';
		printf("[%02d]%s\n", io_get_fd(io), buf);

		//write_back(buf, ret);
	}
}

int
main(int argc, char *argv[])
{
	start("/tmp/nmd_ctrl");
	io_run();
	return 0;
}
