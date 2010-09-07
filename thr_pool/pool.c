#include "pool.h"

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

struct job {
	thr_func func;
	void *arg;
	struct thr_future *future;

	struct job *next, *prev;
};

struct pool {
	pthread_t *threads;
	pthread_mutex_t mutex;
	pthread_cond_t queued_jobs;

	struct job *head, *tail;
};

static int
pool_running(struct pool *pool)
{
	pthread_mutex_lock(&pool->mutex);
	/* TODO: check if pool is running */
	pthread_mutex_unlock(&pool->mutex);
	return 1;
}

static void *
threaded_worker(void *arg)
{
	struct pool *pool = arg;
	struct job *job = NULL;

	while (pool_running(pool)) {
		pthread_cond_wait(&pool->queued_jobs, &pool->mutex);

		pthread_mutex_lock(&pool->mutex);
		job = pool->head;
		pool->head = job->next;
		pool->head->prev = NULL;
		pthread_mutex_unlock(&pool->mutex);

		job->func(job->arg);

		/* TODO: handle future: if future is waiting, signal completion */
	}
	return NULL;
}

struct pool *
pool_new(size_t size)
{
	struct pool *pool = malloc(sizeof(struct pool));
	size_t i;

	if (size <= 0) {
		/* TODO: in this case, size = available cores */
		size = 2;
	}

	pool->head = NULL;
	pool->tail = NULL;
	pthread_mutex_init(&pool->mutex, NULL);
	pthread_cond_init(&pool->queued_jobs, NULL);

	pool->threads = malloc(sizeof(pthread_t) * size);
	for (i = 0; i < size; ++i)
		pthread_create(&pool->threads[i], NULL, threaded_worker, pool);
	
	return pool;
}

struct future *
pool_queue_with_future(struct pool *pool, thr_func func, void *arg)
{
	/* TODO: future optional */
	return NULL;
}

void
pool_queue(struct pool *pool, thr_func func, void *arg)
{
	struct job *job = malloc(sizeof(struct job));
	job->func = func;
	job->arg = arg;

	/* TODO: init future, indicate lack of interest */

	pthread_mutex_lock(&pool->mutex);
	if (!pool->head) {
		pool->head = job;
		job->prev = NULL;
	} else {
		pool->tail->next = job;
		job->prev = pool->tail;
	}
	pool->tail = job;
	job->next = NULL;
	pthread_mutex_unlock(&pool->mutex);

	pthread_cond_signal(&pool->queued_jobs);
}
