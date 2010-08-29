#ifndef LIBS_IO_STORE
#define LIBS_IO_STORE

struct io;
struct io_store;

struct io_store *io_store_create();
void io_store_add(struct io_store *t, struct io *io);
void io_store_delete(struct io_store *t, struct io *io);
struct io *io_store_find(struct io_store *t, int fd);

#endif
