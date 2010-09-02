#include "buf.h"

#include <stdlib.h>
#include <stdio.h>

struct obuf {
	char *ptr;
	size_t offset, len, size;
	buf_fill fill;
	void *arg;
	int do_read;
};

static inline size_t
obuf_fill(struct obuf *buf)
{
	if (!buf->ptr)
		buf->ptr = malloc(buf->size);

	buf->offset = 0;
	buf->do_read = 0;
	buf->len = buf->fill(buf->ptr, buf->size, buf->arg);

	return buf->len;
}

struct obuf *
obuf_new(size_t size, buf_fill fill, void *arg)
{
	struct obuf *buf = malloc(sizeof(struct obuf));

	buf->ptr = NULL;
	buf->size = size;
	buf->len = buf->offset = 0;
	buf->do_read = 1;
	buf->fill = fill;
	buf->arg = arg;

	return buf;
}

size_t
obuf_read(struct obuf *buf, char *dest, size_t len)
{
	if (buf->do_read)
		obuf_fill(buf);

	size_t ret = 0;
	while(--len) {
		*dest++ = *(buf->ptr + buf->offset++);
		++ret;

		if (buf->offset == buf->size) {
			if (obuf_fill(buf) == 0)
				return ret;
		} else if (buf->offset == buf->len) {
			buf->do_read = 1;
			return ret;
		}
	}
	return ret;
}
