#ifndef FUTURE_H
#define FUTURE_H

struct future;

int future_done(struct future *future);
void *future_result(struct future *future);

void *future_join(struct future *future);

#endif
