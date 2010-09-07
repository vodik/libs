#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <libs/io/io.h>
#include <libs/thr_pool/pool.h>

void *
print_num(void *arg)
{
	int i;
	for (i = 0; i < 10; ++i)
		printf("%d\n", i);
	return NULL;
}

int
main(int argc, char *argv[])
{
	struct pool *pool = pool_new(0);
	pool_queue(pool, print_num, NULL);
	pool_queue(pool, print_num, NULL);
	pool_wait(pool);
	return 0;
}
