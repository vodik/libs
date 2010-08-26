#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <memory.h>
#include <assert.h>

enum rbnode_color { RED, BLACK };

struct rbnode {
	void *key, *value;
	struct rbnode *left, *right, *parent;
	bool color;
};

struct rbtree {
	struct rbnode *root;
	compare_func compare;
};

static inline bool
node_color(struct rbnode *n)
{
	return !n ? BLACK : n->color;
}

static inline struct rbnode *
grandparent(struct rbnode *n)
{
	assert(n);
	assert(n->parent);
	assert(n->parent->parent);
	return n->parent->parent;
}

static inline struct rbnode *
sibling(struct rbnode *n)
{
	assert(n);
	assert(n->parent);
	return n == n->parent->left ? n->parent->right : n->parent->left;
}

static inline struct rbnode *
uncle(struct rbnode *n)
{
	assert(n);
	assert(n->parent);
	assert(n->parent->parent);
	return sibling(n->parent);
}

/*#define VERIFY_RBTREE
#ifdef VERIFY_RBTREE
static void
verify1(struct rbnode *n)
{
	assert(node_color(n) == RED || node_color(n) == BLACK);
	if (n) {
		verify1(n->left);
		verify1(n->right);
	}
}

static void
verify2(struct rbnode *n)
{
	assert(node_color(n) == BLACK);
}

static void
verify4(struct rbnode *n)
{
	if (node_color(n) == RED) {
		assert(node_color(n->left) == BLACK);
		assert(node_color(n->right) == BLACK);
		assert(node_color(n->parent) == BLACK);
	}
	if (n) {
		verify4(n->left);
		verify4(n->right);
	}
}

static void
verify5_aid(struct rbnode *n, int bcount, int *path_bcount)
{
	if (node_color(n) == BLACK)
		++bcount;
	if (!n) {
		if (*path_bcount == -1)
			*path_bcount = bcount;
		else
			assert(bcount == *path_bcount);
		return;
	}
	verify5_aid(n->left, bcount, path_bcount);
	verify5_aid(n->right, bcount, path_bcount);
}

static void
verify5(struct rbnode *n)
{
	int path_bcount = -1;
	verify5_aid(n, 0, &path_bcount);
}

static void
verify(struct rbtree *t)
{
	verify1(t->root);
	verify2(t->root);
	//verify3(t->root);
	verify4(t->root);
	verify5(t->root);
}
#endif*/

struct rbtree *
rbtree_create(compare_func compare)
{
	struct rbtree *t = malloc(sizeof(struct rbtree));
	t->root = NULL;
	t->compare = compare;
#ifdef VERIFY
	verify(t);
#endif
	return t;
}

static struct rbnode *
rbnode_new(void *key, void *value, int color)
{
	struct rbnode *n = malloc(sizeof(struct rbnode));
	n->key    = key;
	n->value  = value;
	n->color  = color;
	return n;
}

static struct rbnode *
rbnode_find(struct rbnode *n, void *key, compare_func compare)
{
	int res;

	while (n) {
		res = compare(key, n->key);
		if (res == 0)
			return n;
		else if (res < 0)
			n = n->left;
		else
			n = n->right;
	}
	return NULL;
}

void *
rbtree_find(struct rbtree *t, void *key)
{
	struct rbnode *n = rbnode_find(t->root, key, t->compare);
	return !n ? NULL : n->value;
}

static void
replace_node(struct rbtree *t, struct rbnode *old, struct rbnode *new)
{
	if (!old->parent)
		t->root = new;
	else {
		if (old == old->parent->left)
			old->parent->left = new;
		else
			old->parent->right = new;
	}
	if (new != NULL)
		new->parent = old->parent;
}

static void
rotate_left(struct rbtree *t, struct rbnode *n)
{
	struct rbnode *r = n->right;
	replace_node(t, n, r);
	n->right = r->left;
	if (r->left)
		r->left->parent = n;
	r->left = n;
	n->parent = r;
}

static void
rotate_right(struct rbtree *t, struct rbnode *n)
{
	struct rbnode *l = n->left;
	replace_node(t, n, l);
	n->left = l->right;
	if (l->right)
		l->right->parent = n;
	l->right = n;
	n->parent = l;
}

static void
insert1(struct rbtree *t, struct rbnode *n)
{
	if (!n->parent) {
		n->color = BLACK;
		return;
	}
	if (node_color(n->parent) == BLACK)
		return;
	if (node_color(uncle(n)) == RED) {
		n->parent->color = BLACK;
		uncle(n)->color = BLACK;
		grandparent(n)->color = RED;
		insert1(t, grandparent(n));
		return;
	}
	if (n == n->parent->right && n->parent == grandparent(n)->left) {
		rotate_left(t, n->parent);
		n = n->left;
	} else if (n == n->parent->left && n->parent == grandparent(n)->right) {
		rotate_right(t, n->parent);
		n = n->right;
	}
	n->parent->color = BLACK;
	grandparent(n)->color = RED;
	if (n == n->parent->left && n->parent == grandparent(n)->left)
		rotate_right(t, grandparent(n));
	else
		rotate_left(t, grandparent(n));
}

void
rbtree_add(struct rbtree *t, void *key, void *value)
{
	struct rbnode *n, *ins;
	int res;

	if (!t->root) {
		ins = t->root = rbnode_new(key, value, RED);
	} else {
		n = t->root;
		while (1) {
			res = t->compare(key, n->key);
			if (res == 0) {
				n->value = value;
				return;
			} else if (res < 0) {
				if (!n->left) {
					ins = n->left = rbnode_new(key, value, RED);
					break;
				} else
					n = n->left;
			} else {
				if (!n->right) {
					ins = n->right = rbnode_new(key, value, RED);
					break;
				} else
					n = n->right;
			}
		}
		ins->parent = n;
	}
	insert1(t, ins);
#ifdef VERIFY
	verify(t);
#endif
}

static struct rbnode *
maximum_node(struct rbnode *n) {
	assert(n);
	while (n->right != NULL)
		n = n->right;
	return n;
}

static void
delete1(struct rbtree *t, struct rbnode *n)
{
	if (!n->parent)
		return;
	if (node_color(sibling(n)) == RED) {
		n->parent->color = RED;
		sibling(n)->color = BLACK;
		if (n == n->parent->left)
			rotate_left(t, n->parent);
		else
			rotate_right(t, n->parent);
	}
	if (node_color(n->parent) == BLACK &&
		node_color(sibling(n)) == BLACK &&
		node_color(sibling(n)->left) == BLACK &&
		node_color(sibling(n)->right) == BLACK) {
		sibling(n)->color = RED;
		delete1(t, n->parent);
		return;
	}
	if (node_color(n->parent) == RED &&
		node_color(sibling(n)) == BLACK &&
		node_color(sibling(n)->left) == BLACK &&
		node_color(sibling(n)->right) == BLACK) {
		sibling(n)->color = RED;
		n->parent->color = BLACK;
		return;
	}
	if (n == n->parent->left &&
		node_color(sibling(n)) == BLACK &&
		node_color(sibling(n)->left) == RED &&
		node_color(sibling(n)->right) == BLACK) {
		sibling(n)->color = RED;
		sibling(n)->left->color = BLACK;
		rotate_right(t, sibling(n));
	} else if (n == n->parent->right &&
		node_color(sibling(n)) == BLACK &&
		node_color(sibling(n)->left) == BLACK &&
		node_color(sibling(n)->right) == RED) {
		sibling(n)->color = RED;
		sibling(n)->right->color = BLACK;
		rotate_left(t, sibling(n));
	}
	sibling(n)->color = node_color(n->parent);
	n->parent->color = BLACK;
	if (n == n->parent->left) {
		assert(node_color(sibling(n)->right) == RED);
		sibling(n)->right->color = BLACK;
		rotate_left(t, n->parent);
	} else {
		assert(node_color(sibling(n)->left) == RED);
		sibling(n)->left->color = BLACK;
		rotate_right(t, n->parent);
	}
}

void
rbtree_delete(struct rbtree *t, void *key)
{
	struct rbnode *child, *n;
	if (!(n = rbnode_find(t->root, key, t->compare)))
		return;
	if (n->left && n->right) {
		struct rbnode *pred = maximum_node(n->left);
		n->key = pred->key;
		n->value = pred->value;
		n = pred;
	}

	assert(!n->left || !n->right);
	child = !n->right ? n->left : n->right;
	if (node_color(n) == BLACK) {
		n->color = node_color(child);
		delete1(t, n);
	}
	replace_node(t, n, child);
	if (!n->parent && child)
		child->color = BLACK;
	free(n);
#ifdef VERIFY
	verify(t);
#endif
}
