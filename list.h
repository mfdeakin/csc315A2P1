
#ifndef _CLINKEDLIST_H_
#define _CLINKEDLIST_H_

struct list;

struct list *list_create(int iscircle);
void list_delete(struct list *lst);
void list_gotofront(struct list *lst);
void *list_next(struct list *lst);
void *list_getitem(struct list *lst);
int list_hasnext(struct list *lst);
int list_gotoitem(struct list *lst, void *item, int (*compare)(void *orig, void* chk));
int list_setitem(struct list *lst, void *item);
int list_insert(struct list *lst, void *item);
int list_size(struct list *lst);

#endif
