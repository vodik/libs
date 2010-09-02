#include "buf.h"

#include <stdlib.h>

struct buf *
buf_new(size_t size, buf_fill fill, buf_push push, void *arg)
{
	struct buf *buf = malloc(sizeof(struct buf));
	buf->i = obuf_new(size, push, arg);
	buf->o = ibuf_new(size, fill, arg);
	return buf;
}

void
buf_free(struct buf *buf)
{
	obuf_free(buf->i);
	ibuf_free(buf->o);
}

size_t
buf_read(struct buf *buf, char *dest, size_t len)
{
	return ibuf_read(buf->o, dest, len);
}

size_t
buf_write(struct buf *buf, const char *src, size_t len)
{
	return obuf_write(buf->i, src, len);
}

void
buf_flush(struct buf *buf)
{
	obuf_flush(buf->i);
}

