#ifndef LIBS_OBJ
#define LIBS_OBJ

typedef void (*destroy_func)(void *);

struct obj;

struct obj *obj_create(void *data, destory_func func);
void *obj_get(struct obj *obj);

void obj_ref(struct obj *obj);
void obj_unref(struct obj *obj);

#endif
