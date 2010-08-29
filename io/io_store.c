#include "io_store.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "io_ref.h"

enum io_node_color { RED, BLACK };

struct io_node {
	struct io *io;
	struct io_node *left, *right, *parent;
	int color;
};

struct io_store {
	struct io_node *root;
};

static inline int
compare(int left, int right)
{
	if (left < right)
		return -1;
	else if (left > right)
		return 1;
	else
		return 0;
}

static inline int
node_color(struct io_node *n)
{
	return !n ? BLACK : n->color;
}

static inline struct io_node *
grandparent(struct io_node *n)
{
	return n->parent->parent;
}

static inline struct io_node *
sibling(struct io_node *n)
{
	return n == n->parent->left ? n->parent->right : n->parent->left;
}

static inline struct io_node *
uncle(struct io_node *n)
{
	return sibling(n->parent);
}

struct io_store *
io_store_create()
{
	struct io_store *t = malloc(sizeof(struct io_store));
	t->root = NULL;
	return t;
}

static struct io_node *
io_node_new(struct io *io, int color)
{
	struct io_node *n = malloc(sizeof(struct io_node));
	n->io = io;
	n->color = color;

	n->left = n->right = n->parent = NULL;
	return n;
}

static struct io_node *
io_node_find(struct io_node *n, int fd)
{
	int res;

	while (n) {
		res = compare(fd, n->io->fd);
		if (res == 0)
			return n;
		else if (res < 0)
			n = n->left;
		else
			n = n->right;
	}
	return NULL;
}

struct io *
io_store_find(struct io_store *t, int fd)
{
	struct io_node *n = io_node_find(t->root, fd);
	return !n ? NULL : n->io;
}

static void
replace_node(struct io_store *t, struct io_node *old, struct io_node *new)
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
rotate_left(struct io_store *t, struct io_node *n)
{
	struct io_node *r = n->right;
	replace_node(t, n, r);
	n->right = r->left;
	if (r->left)
		r->left->parent = n;
	r->left = n;
	n->parent = r;
}

static void
rotate_right(struct io_store *t, struct io_node *n)
{
	struct io_node *l = n->left;
	replace_node(t, n, l);
	n->left = l->right;
	if (l->right)
		l->right->parent = n;
	l->right = n;
	n->parent = l;
}

static void
insert1(struct io_store *t, struct io_node *n)
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
io_store_add(struct io_store *t, struct io *io)
{
	struct io_node *n, *ins;
	int res;

	if (!t->root) {
		ins = t->root = io_node_new(io, RED);
	} else {
		n = t->root;
		while (1) {
			res = compare(io->fd, n->io->fd);
			if (res == 0) {
				n->io = io;
				return;
			} else if (res < 0) {
				if (!n->left) {
					ins = n->left = io_node_new(io, RED);
					break;
				} else
					n = n->left;
			} else {
				if (!n->right) {
					ins = n->right = io_node_new(io, RED);
					break;
				} else
					n = n->right;
			}
		}
		ins->parent = n;
	}
	insert1(t, ins);
}

static struct io_node *
maximum_node(struct io_node *n) {
	while (n->right != NULL)
		n = n->right;
	return n;
}

static void
delete1(struct io_store *t, struct io_node *n)
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
		sibling(n)->right->color = BLACK;
		rotate_left(t, n->parent);
	} else {
		sibling(n)->left->color = BLACK;
		rotate_right(t, n->parent);
	}
}

void
io_store_delete(struct io_store *t, struct io *io)
{
	struct io_node *child, *n;
	if (!(n = io_node_find(t->root, io->fd)))
		return;
	if (n->left && n->right) {
		struct io_node *pred = maximum_node(n->left);
		n->io = pred->io;
		n = pred;
	}

	child = !n->right ? n->left : n->right;
	if (node_color(n) == BLACK) {
		n->color = node_color(child);
		delete1(t, n);
	}
	replace_node(t, n, child);
	if (!n->parent && child)
		child->color = BLACK;
	free(n);
}
