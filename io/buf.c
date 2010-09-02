#include "buf.h"

#include <stdlib.h>

struct buf *
buf_new(size_t size, buf_fill fill, buf_push push, void *arg)
{
	struct buf *buf = malloc(sizeof(struct buf));
	buf->i = ibuf_new(size, push, arg);
	buf->o = obuf_new(size, fill, arg);
	return buf;
}

void
buf_free(struct buf *buf)
{
	ibuf_free(buf->i);
	obuf_free(buf->o);
}

size_t
buf_read(struct buf *buf, char *dest, size_t len)
{
	return obuf_read(buf->o, dest, len);
}

size_t
buf_write(struct buf *buf, const char *src, size_t len)
{
	return ibuf_write(buf->i, src, len);
}

void
buf_flush(struct buf *buf)
{
	ibuf_flush(buf->i);
}

