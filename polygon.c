
#include "polygon.h"
#include <string.h>
#include <stdlib.h>

struct polygon {
	struct matrix *mtx;
	unsigned verts;
};

struct polygon *polyCreate(unsigned verts)
{
	struct polygon *poly = malloc(sizeof(struct polygon));
	poly->mtx = mtxCreate(verts, 3);
	poly->verts = verts;
	return poly;
}

struct polygon *polyCreateList(struct list *lst)
{
	struct polygon *poly = polyCreate(list_size(lst));
	int i;
	list_gotofront(lst);
	for(i = 0; i < poly->verts; i++) {
		struct pt *pt = (struct pt*)list_next(lst);
		mtxSet(poly->mtx, i, 0, pt->x);
		mtxSet(poly->mtx, i, 1, pt->y);
		mtxSet(poly->mtx, i, 2, 1);
	}
	return poly;
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
	mtxFree(p->mtx);
	free(p);
}

void polySetPoint(struct polygon *p,
									unsigned i, struct pt *pos)
{
	mtxSet(p->mtx, i, 0, pos->x);
	mtxSet(p->mtx, i, 1, pos->y);
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

struct sortPoint {
	struct pt ends[2];
	unsigned index;
	bool start;
};

int compSPoint(const struct sortPoint *lhs,
							 const struct sortPoint *rhs)
{
	if(rhs->index == lhs->index)
		return 0;
	if(lhs->ends[0].y == rhs->ends[0].y)
		return rhs->index - lhs->index;
	return lhs->ends[0].y - rhs->ends[0].y;
}

/* Not done yet */
struct list *polySubdivide(struct polygon *poly)
{
	struct sortPoint *eStart, *eEnd, *eRef;
	eStart = malloc(sizeof(*eStart) * poly->verts);
	eEnd = malloc(sizeof(*eEnd) * poly->verts);
	eRef = malloc(sizeof(*eRef) * poly->verts);
	/* Collect all of our points into the array for sorting */
	unsigned i, j, k;
	struct pt cur = polyPoint(poly, poly->verts - 1),
		prv;
	for(i = 0; i < poly->verts; i++) {
		prv = cur;
		cur = polyPoint(poly, i);
		if(prv.y <= cur.y) {
			eStart[i].ends[0] = cur;
			eStart[i].ends[1] = prv;
			eEnd[i].ends[0] = prv;
			eEnd[i].ends[1] = cur;
		}
		else {
			eStart[i].ends[0] = prv;
			eStart[i].ends[1] = cur;
			eEnd[i].ends[0] = cur;
			eEnd[i].ends[1] = prv;
		}
		eStart[i].index = i;
		eStart[i].start = true;
		eEnd[i].index = i;
		eEnd[i].start = false;
		eRef[i] = eStart[i];
	}
	qsort(eStart, sizeof(*eStart), poly->verts,
				(__compar_fn_t)compSPoint);
	qsort(eStart, sizeof(*eEnd), poly->verts,
				(__compar_fn_t)compSPoint);
	/* The edges are sorted, now merge them into one array
	 * This way we know when an edge ends and starts.
	 * The edge array is a reference for relative to the others */
	struct sortPoint *edges = malloc(sizeof(*edges) * poly->verts * 2),
		*active = malloc(sizeof(*active) * poly->verts);
	for(i = 0, j = 0, k = 0; i < poly->verts * 2; i++) {
		if(eStart[j].ends[0].y > eEnd[k].ends[0].y) {
			edges[i] = eStart[j];
			j++;
		}
		else {
			edges[i] = eEnd[k];
			k++;
		}
	}
	free(eStart);
	free(eEnd);
	/* Now the fun part */
	unsigned numActive = 0;
	for(i = 0; i < poly->verts * 2; i++) {
		if(edges[i].start) {
			numActive++;
		}
		else {
			/* I am terrible person. for loop abuser */
			for(j = 0;
					compSPoint(&active[j], &edges[i]);
					j++);
			numActive--;
			while(j < numActive) {
				active[j] = active[j + 1];
				j++;
			}
		}
	}
	struct list *polys;
	polys = list_create(0);
	free(edges);
	return polys;
}

bool lineIntersect(struct pt a1, struct pt a2,
									 struct pt b1, struct pt b2)
{
	struct pt A = {a2.x - a1.x, a2.y - a1.y},
		u1 = {b1.x - a1.x, b1.y - a1.y},
		u2 = {b2.x - a1.x, b2.y - a1.y};
	unsigned aS1 = ptVecProd(A, u1),
		aS2 = ptVecProd(A, u2);
	if(aS1 == 0 || aS2 == 0)
		/* We don't consider it interesection if b1 or b2 is on A */
		return false;
	if((aS1 < 0 && aS2 < 0) ||
		 (aS1 > 0 && aS2 > 0))
		/* Same sign, so b1 and b2 are on the same side of A */
		return false;
	struct pt B = {b2.x - b1.x, b2.y - b1.y},
		u3 = {a2.x - b1.x, a2.y - b1.y};
	unsigned bS1 = ptVecProd(u1, B),
		bS2 = ptVecProd(B, u3);
	if((bS1 < 0 && bS2 < 0) ||
		 (bS1 > 0 && bS2 > 0))
		return false;
	return true;
}

unsigned nextUnused(bool used[], unsigned cur, unsigned count) {
	unsigned pos;
	/* WARNING: For Loop Abuse */
	for(pos = (cur + 1) % count;
			used[pos] && pos != cur;
			pos = (pos + 1) % count);
	return pos;
}

struct polygon **polyTessellate(struct polygon *poly)
{
	/* This code is terrible, because I'm not using a linked list (like I should!)
	 * However, I need a new linked list object, because this is better than using
	 * my current one. */
	struct polygon **tess = malloc(sizeof(struct polygon *[poly->verts - 2]));
	unsigned i;
	for(i = 0; i < poly->verts - 2; i++)
		tess[i] = polyCreate(3);
	bool *used = malloc(sizeof(bool[poly->verts]));
	memset(used, false, sizeof(bool[poly->verts]));
	unsigned prv, cur, nxt, count;
	cur = 0;
	nxt = 1;
	count = 0;
	while(count < poly->verts - 3) {
		prv = cur;
		cur = nxt;
		nxt = nextUnused(used, nxt, poly->verts);
		/* Yes I hate Hungarian notation as much as the next guy,
		 * but I'm too busy/lazy to come up with better names for these */
		struct pt ptPrv = polyPoint(poly, prv),
			ptCur = polyPoint(poly, cur),
			ptNxt = polyPoint(poly, nxt);
		ptCur.x -= ptPrv.x;
		ptCur.y -= ptPrv.y;
		ptNxt.x -= ptPrv.x;
		ptNxt.y -= ptPrv.y;
		if(ptVecProd(ptCur, ptNxt) > 0) {
			/* Conditional Acceptance, because of counterclockwise winding order */
			ptNxt.x += ptPrv.x;
			ptNxt.y += ptPrv.y;
			bool add = true;
			unsigned cmpA = nextUnused(used, nxt, poly->verts),
				cmpB;
			for(cmpB = nextUnused(used, cmpA, poly->verts);
					cmpB != prv;
					cmpA = cmpB,
						cmpB = nextUnused(used, cmpB, poly->verts)) {
				cmpB = nextUnused(used, cmpB, poly->verts);
				struct pt ptCmpA = polyPoint(poly, cmpA),
					ptCmpB = polyPoint(poly, cmpB);
				if(lineIntersect(ptCmpA, ptCmpB,
												 ptPrv, ptNxt)) {
					add = false;
					break;
				}
			}
			if(add) {
				struct pt tmp;
				tmp = polyPoint(poly, prv);
				polySetPoint(tess[count], 0, &tmp);
				tmp = polyPoint(poly, cur);
				polySetPoint(tess[count], 1, &tmp);
				tmp = polyPoint(poly, nxt);
				polySetPoint(tess[count], 2, &tmp);
				used[cur] = true;
				count++;
				/* Need to skip cur, do this by advancing an extra step.
				 * We don't need to update prv, it will be overwritten anyways */
				cur = nxt;
				nxt = nextUnused(used, nxt, poly->verts);
			}
		}
	}
	struct pt tmp;
	tmp = polyPoint(poly, prv);
	polySetPoint(tess[count], 0, &tmp);
	tmp = polyPoint(poly, cur);
	polySetPoint(tess[count], 1, &tmp);
	tmp = polyPoint(poly, nxt);
	polySetPoint(tess[count], 2, &tmp);
	free(used);
	return tess;
}

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
	if(poly->verts > 3) {
		struct polygon **tessellated = polyTessellate(poly);
		unsigned i;
		for(i = 0; i < poly->verts - 2; i++) {
			polyDraw(tessellated[i]);
			polyFree(tessellated[i]);
		}
		free(tessellated);
	}
	else {
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
		list_delete(lst);
	}
}
