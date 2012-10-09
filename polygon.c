
#include "polygon.h"
#include <stdlib.h>

struct polygon {
	struct matrix *mtx;
	unsigned verts;
};

struct polygon *polyCreate(unsigned verts)
{
	struct polygon *p = malloc(sizeof(struct polygon));
	p->mtx = mtxCreate(verts, 3);
	p->verts = verts;
	return p;
}

struct polygon *polyCreateList(struct list *lst)
{
	struct polygon *p = polyCreate(list_size(lst));
	int i;
	list_gotofront(lst);
	for(i = 0; i < p->verts; i++) {
		struct pt *pt = (struct pt*)list_next(lst);
		mtxSet(p->mtx, i, 0, pt->x);
		mtxSet(p->mtx, i, 1, pt->y);
		mtxSet(p->mtx, i, 2, 1);
	}
	return p;
}

struct polygon *polyCreatePoints(struct pt *points,
																 unsigned verts)
{
	struct polygon *p = polyCreate(verts);
	int i;
	for(i = 0; i < verts; i++) {
		mtxSet(p->mtx, i, 0, points[i].x);
		mtxSet(p->mtx, i, 1, points[i].y);
		mtxSet(p->mtx, i, 2, 1);
	}
	return p;
}

struct polygon *polyCopy(struct polygon *src)
{
	struct polygon *p = malloc(sizeof(struct polygon));
	p->verts = src->verts;
	p->mtx = mtxCopy(src->mtx);
	return p;
}

void polyFree(struct polygon *p)
{
	free(p->mtx);
	free(p);
}

void polySetPoint(struct polygon *p,
									unsigned i, struct pt *pos)
{
	mtxSet(p->mtx, i, 0, pos[i].x);
	mtxSet(p->mtx, i, 1, pos[i].y);
}

struct pt polyPoint(struct polygon *p,
										unsigned i)
{
	struct pt point;
	point.x = mtxGet(p->mtx, i, 0);
	point.y = mtxGet(p->mtx, i, 1);
	return point;
}

struct polygon *polyTransform(struct polygon *poly,
															struct matrix *mtx)
{
	struct polygon *p = malloc(sizeof(struct polygon));
	p->verts = poly->verts;
	p->mtx = mtxMul(mtx, poly->mtx);
	return p;
}

void polyTransformIP(struct polygon *poly,
										 struct matrix *mtx)
{
	struct matrix *tmp = mtxMul(mtx, poly->mtx);
	mtxFree(poly->mtx);
	poly->mtx = tmp;
}

struct list *polyClipHelper(struct polygon *p);

struct polygon *polyClip(struct polygon *poly)
{
	struct list *lst = polyClipHelper(poly);
	struct polygon *p = polyCreateList(lst);
	list_delete(lst);
	return p;
}

/* Nasty function - needs to be cleaned up */
struct list *polyClipHelper(struct polygon *p)
{
	bool once = false;
	struct list *lst = list_create(0);
	struct pt cur = polyPoint(p, p->verts - 1),
		prv;
	cur.x += OFFWIDTH + CENTERX;
	cur.y += OFFHEIGHT + CENTERY;
	for(int i = 0; i < p->verts; i++) {
		prv = cur;
		cur = polyPoint(p, i);
		cur.x += OFFWIDTH + CENTERX;
		cur.y += OFFHEIGHT + CENTERY;
		struct pt curbuf = cur,
			prvbuf = prv;
		struct matrix *mtx;
		if(clipLine(&curbuf, &prvbuf)) {
			once = true;
			mtx = ptToMatrix(&prvbuf);
			list_insert(lst, mtx);
			mtx = ptToMatrix(&curbuf);
			list_insert(lst, mtx);
			enum Region p1 = pointRegion(cur);
			if(p1 == (BOTTOM | LEFT)) {
				mtx = ptToMatrix(&(struct pt){
						OFFWIDTH,
							OFFHEIGHT
							});
				list_insert(lst, mtx);
			}
			if(p1 == (BOTTOM | RIGHT)) {
				mtx = ptToMatrix(&(struct pt){
						OFFWIDTH + VIEWWIDTH,
							OFFHEIGHT
					});
				list_insert(lst, mtx);
			}
			if(p1 == (TOP | LEFT)) {
				mtx = ptToMatrix(&(struct pt){
						OFFWIDTH,
							OFFHEIGHT + VIEWHEIGHT
					});
				list_insert(lst, mtx);
			}
			if(p1 == (TOP | RIGHT)) {
				mtx = ptToMatrix(&(struct pt){
						OFFWIDTH + VIEWWIDTH,
							OFFHEIGHT + VIEWHEIGHT
							});
				list_insert(lst, mtx);
			}
		}
		else {
			struct matrix *mtx;
			enum Region p1 = pointRegion(cur),
				p2 = pointRegion(prv);
			struct {
				enum Region r1, r2;
				struct pt pos;
			} operations[4];
			operations[0].r1 = LEFT;
			operations[0].r2 = BOTTOM;
			operations[0].pos.x = OFFWIDTH;
			operations[0].pos.y = OFFHEIGHT;

			operations[1].r1 = RIGHT;
			operations[1].r2 = BOTTOM;
			operations[1].pos.x = OFFWIDTH + VIEWWIDTH;
			operations[1].pos.y = OFFHEIGHT;

			operations[2].r1 = TOP;
			operations[2].r2 = RIGHT;
			operations[2].pos.x = OFFWIDTH + VIEWWIDTH;
			operations[2].pos.y = OFFHEIGHT + VIEWHEIGHT;

			operations[3].r1 = TOP;
			operations[3].r2 = LEFT;
			operations[3].pos.x = OFFWIDTH;
			operations[3].pos.y = OFFHEIGHT + VIEWHEIGHT;
			int j;
			for(j = 0; j < 4; j++) {
				if((p1 & operations[j].r1 &&
						p2 & operations[j].r2) ||
					 (p2 & operations[j].r1 &&
						p1 & operations[j].r2)) {
					mtx = ptToMatrix(&operations[j].pos);
					list_insert(lst, mtx);
				}
			}
		}
	}
	if(!once) {
		list_delete(lst);
		lst = list_create(0);
		struct matrix *mtx;
		struct pt corners[5];
		corners[0].x = OFFWIDTH;
		corners[0].y = OFFHEIGHT;
		corners[1].x = OFFWIDTH + VIEWWIDTH;
		corners[1].y = OFFHEIGHT;
		corners[2].x = OFFWIDTH + VIEWWIDTH;
		corners[2].y = OFFHEIGHT + VIEWHEIGHT;
		corners[3].x = OFFWIDTH;
		corners[3].y = OFFHEIGHT + VIEWHEIGHT;
		corners[4].x = OFFWIDTH;
		corners[4].y = OFFHEIGHT;
		int i;
		for(i = 0; i < 5; i++) {
			mtx = ptToMatrix(&corners[i]);
			list_insert(lst, mtx);
		}
	}
	return lst;
}

struct list *polySubdivide(struct polygon *p)
{
	struct list *polys;
	struct {
		struct pt pts[2];
		bool active;
	} *activeEdges;
	activeEdges = malloc(sizeof(*activeEdges) * p->verts);
	polys = list_create(0);
	int i;
	struct pt prv,
		cur = polyPoint(p, 0);
	for(i = 1; i < p->verts; i++) {
		prv = cur;
		cur = polyPoint(p, i);
	}
	free(activeEdges);
	return polys;
}

struct list *polyTessellate(struct polygon *poly);

struct matrix *polyToMatrix(struct polygon *poly)
{
	return mtxCopy(poly->mtx);
}

struct list *polyToPtList(struct polygon *poly)
{
	struct list *lst = list_create(0);
	int i;
	for(i = 0; i < poly->verts; i++) {
		struct pt buffer = polyPoint(poly, i),
			*point = malloc(sizeof(struct pt));
		*point = buffer;
		list_insert(lst, point);
	}
	return lst;
}

void polyDraw(struct polygon *poly)
{
	list_delete(polySubdivide(poly));
	struct list *lst = polyClipHelper(poly);
	list_gotofront(lst);
	struct matrix *mtx = list_next(lst);
	if(!mtx)
		return;
	struct pt prv,
		cur = mtxToPoint(mtx);
	while(list_hasnext(lst)) {
		prv = cur;
		mtx = list_next(lst);
		cur = mtxToPoint(mtx);
		mtxFree(mtx);
		drawLine(prv, cur);
	}
}
