
#include <string.h>
#include <stdlib.h>

struct list
{
	struct list_elem *front;
	struct list_elem *current;
	size_t elem_size;
	int circular:1;
};

struct list_elem
{
	struct list_elem *nxt;
	void *item;
};

struct list *list_create(size_t elem_size, int iscircle);
void list_delete(struct list *lst);
void *list_next(struct list *lst);
void *list_getitem(struct list *lst);
int list_gotoitem(struct list *lst, void *item, int (*compare)(void *orig, void* chk));
int list_setitem(struct list *lst, void *item);
int list_insert(struct list *lst, void *item);
struct list *list_map(struct list *lst, void *(*mapping)(void *));
void list_apply(struct list *lst, void (*func)(void *));

struct list *list_create(size_t elem_size, int iscircle)
{
	struct list *ret = malloc(sizeof(struct list));
	ret->elem_size = elem_size;
	ret->circular = iscircle;
	ret->front = ret->current = malloc(sizeof(struct list_elem));
	memset(ret->front, 0, sizeof(struct list_elem));
	return ret;
}

void list_delete(struct list *lst)
{
	lst->current = lst->front;
	while(lst->current)
		{
			lst->front = lst->current;
			lst->current = lst->current->nxt;
			free(lst->front->item);
			free(lst->front);
		}
	free(lst);
}

void *list_next(struct list *lst)
{
	if(lst->current != NULL)
		{
			if(lst->circular && lst->current->nxt == NULL)
				lst->current = lst->front;
			else
				lst->current = lst->current->nxt;
		}
	return lst->current->item;
}

void *list_getitem(struct list *lst)
{
	return lst->current->item;
}

int list_gotoitem(struct list *lst, void *item, int (*compare)(void *orig, void  *chk))
{
	struct list_elem *finder = lst->current;
	for(list_next(lst);
		!compare(item, lst->current->item) && lst->current != finder;
		list_next(lst))
		{
			if(lst->current == NULL)
				lst->current = lst->front;
		}
	return (lst->current != finder);
}

int list_setitem(struct list *lst, void *item)
{
	lst->current->item = malloc(lst->elem_size);
	memcpy(lst->current->item, item, lst->elem_size);
	return 1;
}

int list_insert(struct list *lst, void *item)
{
	if(lst->current == NULL)
		return 0;
	struct list_elem *prv = lst->current;
	struct list_elem *nxt = lst->current->nxt;
	prv->nxt = lst->current = malloc(sizeof(struct list_elem));
	lst->current->nxt = nxt;
	int ret = list_setitem(lst, item);
	return ret;
}

struct list *list_map(struct list *lst, void *(*mapping)(void *))
{
	struct list *ret = list_create(lst->elem_size, lst->circular);
	if(lst->front->nxt == NULL)
		return ret;
	struct list_elem *cur = lst->front->nxt;
	while(cur && cur != lst->front) {
		list_insert(ret, mapping(cur->item));
		cur = cur->nxt;
	}
	return ret;
}

void list_apply(struct list *lst, void (*func)(void *))
{
	if(lst->front == NULL)
		return;
	struct list_elem *cur = lst->front->nxt;
	while(cur && cur != lst->front) {
		func(cur->item);
		cur = cur->nxt;
	}
}
