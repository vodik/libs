#include "buf.h"

#include <stdlib.h>
#include <stdio.h>

struct ibuf {
	char *ptr;
	size_t offset, size;
	buf_push push;
	void *arg;
	int dirty;
};

struct ibuf *
ibuf_new(size_t size, buf_push push, void *arg)
{
	struct ibuf *buf = malloc(sizeof(struct ibuf));

	buf->ptr = NULL;
	buf->size = size;
	buf->offset = 0;
	buf->push = push;
	buf->arg = arg;
	buf->dirty = 1;

	return buf;
}

void ibuf_free(struct ibuf *buf)
{
	if (buf->dirty)
		ibuf_flush(buf);
	if (buf->ptr)
		free(buf->ptr);
	free(buf);
}

size_t
ibuf_write(struct ibuf *buf, const char *src, size_t len)
{
	if (!buf->ptr)
		buf->ptr = malloc(buf->size);

	size_t ret = 0;
	while(--len) {
		*(buf->ptr + buf->offset++) = *src++;
		++ret;

		if (buf->offset == buf->size)
			ibuf_flush(buf);
	}
	return ret;
}

void
ibuf_flush(struct ibuf *buf)
{
	if (buf->ptr) {
		buf->push(buf->ptr, buf->offset, buf->arg);
		buf->offset = 0;
	}
}
