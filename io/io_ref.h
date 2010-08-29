#ifndef LIBS_IO_REF
#define LIBS_IO_REF

struct io {
	int fd;

	void (*iofunc)(struct io *, int, void *);
	void *arg;
};

#endif
