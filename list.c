
/* This linked list needs serious work to be considered usable.
 * Just some really old code I dug up. Why did I do that :( */

#include <string.h>
#include <stdlib.h>

struct list
{
	struct list_elem *front, *back;
	struct list_elem *current;
	unsigned circular:1;
	unsigned size:31;
};

struct list_elem
{
	struct list_elem *nxt, *prv;
	void *item;
};

struct list *list_create(int iscircle);
void list_delete(struct list *lst);
void *list_next(struct list *lst);
void *list_getitem(struct list *lst);
int list_gotoitem(struct list *lst, void *item, int (*compare)(void *orig, void* chk));
int list_setitem(struct list *lst, void *item);
int list_insert(struct list *lst, void *item);
struct list *list_map(struct list *lst, void *(*mapping)(void *));
void list_apply(struct list *lst, void (*func)(void *));

struct list *list_create(int iscircle)
{
	struct list *ret = malloc(sizeof(struct list));
	ret->circular = iscircle;
	ret->back = ret->front = ret->current = malloc(sizeof(struct list_elem));
	memset(ret->back, 0, sizeof(struct list_elem));
	ret->size = 0;
	return ret;
}

void list_delete(struct list *lst)
{
	lst->current = lst->front;
	while(lst->current)	{
		lst->front = lst->current;
		lst->current = lst->current->nxt;
		free(lst->front);
	}
	free(lst);
}

struct list *list_sort(struct list *lst,
											 int (*compare)(const void **lhs, const void **rhs))
{
	struct list *sorted = list_create(lst->circular);
	void **data = malloc(sizeof(void *[lst->size]));
	struct list_elem *el;
	unsigned i;
	for(el = lst->front, i = 0; el; el = el->nxt, i++) {
		data[i] = el->item;
	}
	qsort(data, sizeof(void *), i, (int (*)(const void *, const void *))compare);
	unsigned j;
	for(j = 0; j < i; j++) {
		list_insert(sorted, data[j]);
	}
	return sorted;
}

void *list_next(struct list *lst)
{
	if(lst->current != NULL) {
		if(lst->circular && lst->current->nxt == NULL)
			lst->current = lst->front;
		else
			lst->current = lst->current->nxt;
		if(lst->current)
			return lst->current->item;
	}
	return NULL;
}

void *list_prev(struct list *lst)
{
	if(lst->current != NULL) {
		if(lst->circular && lst->current->prv == NULL)
			lst->current = lst->back;
		else
			lst->current = lst->current->prv;
		if(lst->current)
			return lst->current->item;
	}
	return NULL;
}

int list_hasnext(struct list *lst)
{
	return (lst->circular && lst->current) ||
		(lst->current != NULL && lst->current->nxt != NULL);
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
			list_next(lst)) {
		if(lst->current == NULL)
			lst->current = lst->front;
	}
	return (lst->current != finder);
}

void list_gotofront(struct list *lst)
{
	lst->current = lst->front;
}

int list_setitem(struct list *lst, void *item)
{
	lst->current->item = item;
	return 1;
}

void list_removeitem(struct list *lst)
{
	struct list_elem *nxt = lst->current->nxt,
		*prv = lst->current->prv;
	free(lst->current);
	if(lst->current == lst->front) {
		lst->front = nxt;
	}
	if(lst->current == lst->back) {
		lst->back = prv;
	}
	if(nxt) {
		nxt->prv = prv;
		lst->current = nxt;
	}
	else
		lst->current = lst->front;
	if(prv)
		prv->nxt = nxt;
	lst->size--;
}

int list_hasprev(struct list *lst)
{
	return lst->current->prv != NULL;
}

void list_gotoback(struct list *lst)
{
	lst->current = lst->back;
}

int list_insert(struct list *lst, void *item)
{
	if(lst->current == NULL)
		return 0;
	struct list_elem *prv = lst->current;
	struct list_elem *nxt = lst->current->nxt;
	prv->nxt = lst->current = malloc(sizeof(struct list_elem));
	if(nxt)
		nxt->prv = lst->current;
	lst->current->nxt = nxt;
	lst->current->prv = prv;
	int ret = list_setitem(lst, item);
	lst->size++;
	if(lst->back == prv)
		lst->back = lst->current;
	return ret;
}

struct list *list_map(struct list *lst, void *(*mapping)(void *))
{
	struct list *ret = list_create(lst->circular);
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

int list_size(struct list *lst)
{
	return lst->size;
}
