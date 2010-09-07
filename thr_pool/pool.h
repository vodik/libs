#ifndef POOL_H
#define POOL_H

#include <stddef.h>
#include "future.h"

typedef void *(*thr_func)(void *arg);

struct job;
struct pool;

struct pool *pool_new(size_t size);
void pool_queue(struct pool *pool, thr_func func, void *arg);

#endif
