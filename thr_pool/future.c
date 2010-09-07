#include "future.h"

#include <stdlib.h>
#include <stdio.h>

struct future {
	void *result;
	int done;
	/* TODO: mutex and cond */
};

int future_done(struct future *future)
{
	return 0;
}

void *future_result(struct future *future)
{
	return future->result;
}

void *future_join(struct future *future)
{
	void *result = NULL;

	/* TODO: lock */
	/*if (!future->done)
		pthread_cond_wait(future->joined);*/
	result = future->result;
	/* TODO: unlock */
	
	return result;
}
