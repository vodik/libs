#ifndef LIBS_RBTREE
#define LIBS_RBTREE

typedef int (*compare_func)(void *left, void *right);

struct rbtree;

struct rbtree *rbtree_create();
void rbtree_add(struct rbtree *t, void *key, void *value);
void rbtree_delete(struct rbtree *t, void *key);
void *rbtree_find(struct rbtree *t, void *key);

#endif
