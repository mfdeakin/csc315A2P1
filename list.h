
#ifndef _CLINKEDLIST_H_
#define _CLINKEDLIST_H_

struct list;

struct list *list_create(size_t elem_size, int iscircle);
void list_delete(struct list *lst);
void *list_next(struct list *lst);
void *list_getitem(struct list *lst);
int list_gotoitem(struct list *lst, void *item, int (*compare)(void *orig, void* chk));
int list_setitem(struct list *lst, void *item);
int list_insert(struct list *lst, void *item);

#endif
