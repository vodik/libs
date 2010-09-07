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
	pthread_cond_t queued_jobs, d_wait;

	int running;
	size_t size, active;
	struct job *head, *tail;
};

static struct job *
get_next_job(struct pool *pool)
{
	struct job *job = NULL;

	pthread_mutex_lock(&pool->mutex);
	if (!pool->head) {
		if (!pool->running)
			return NULL;
		else {
			pthread_cond_broadcast(&pool->d_wait);
			pthread_cond_wait(&pool->queued_jobs, &pool->mutex);
		}
	}

	if (!pool->running)
		return NULL;

	job = pool->head;
	pool->head = job->next;
	if (pool->head)
		pool->head->prev = NULL;
	pthread_mutex_unlock(&pool->mutex);

	return job;
}

static void
add_job(struct pool *pool, struct job *job)
{
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
}

static void *
threaded_worker(void *arg)
{
	struct pool *pool = arg;
	struct job *job = NULL;

	while ((job = get_next_job(pool))) {
		job->func(job->arg);

		/* TODO: handle future: if future is waiting, signal completion */
	}
	//pthread_exit(NULL);
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
	pool->size = size;
	pool->running = 1;
	pthread_mutex_init(&pool->mutex, NULL);
	pthread_cond_init(&pool->queued_jobs, NULL);
	pthread_cond_init(&pool->d_wait, NULL);

	pool->threads = malloc(sizeof(pthread_t) * size);
	for (i = 0; i < pool->size; ++i)
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

	add_job(pool, job);

	pthread_cond_signal(&pool->queued_jobs);
}

void
pool_wait(struct pool *pool)
{
	int i;

	pthread_mutex_lock(&pool->mutex);
	if (pool->head)
		pthread_cond_wait(&pool->d_wait, &pool->mutex);
	pool->running = 0;
	pthread_cond_broadcast(&pool->queued_jobs);
	pthread_mutex_unlock(&pool->mutex);

	for (i = 0; i < pool->size; ++i)
		pthread_join(pool->threads[i], NULL);
}
