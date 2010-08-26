#include "obj.h"

struct obj {
	void *data;
	unsigned refs;

	destroy_func destroy;
};

struct obj *
obj_create(void *data, destory_func func)
{
	struct obj *obj = malloc(sizeof *obj);
	obj->data = data;
	obj->destory = func;
	obj->refs = 1;
	return obj;
}

void *
obj_get(struct obj *obj)
{
	return obj->data;
}

void
obj_ref(struct obj *obj)
{
	++obj->refs;
}

void
obj_unref(struct obj *obj)
{
	if (--obj->refs == 0) {
		obj->destory(obj->data);
		free(obj);
	}
}
