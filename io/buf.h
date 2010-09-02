#ifndef IO_BUF
#define IO_BUF

#include <stddef.h>

typedef size_t (*buf_fill)(char *buf, size_t len, void *arg);
typedef size_t (*buf_push)(const char *buf, size_t len, void *arg);

struct ibuf;
struct obuf;

struct buf {
	struct ibuf *i;
	struct obuf *o;
};

struct obuf *obuf_new(size_t size, buf_fill fill, void *arg);
size_t obuf_read(struct obuf *buf, char *dest, size_t len);

struct ibuf *ibuf_new(size_t size, buf_push push, void *arg);
size_t ibuf_write(struct ibuf *buf, const char *src, size_t len);
void ibuf_flush(struct ibuf *buf);

#endif
