
#ifndef __POLYGON_H
#define __POLYGON_H

/* TODO: Make polygon another type of matrix for
 * efficiency reasons such as:
 *   Pointer arithmetic
 *   System calls for memory allocation
 *   Slightly smaller memory footprint */

#include "matrix.h"
#include "draw.h"
#include "list.h"

struct polygon;

struct polygon *polyCreate(unsigned verts);
struct polygon *polyCreateList(struct list *lst);
struct polygon *polyCreatePoints(struct pt *points,
																 unsigned verts);
struct polygon *polyCopy(struct polygon *src);
void polyFree(struct polygon *poly);

void polySetPoint(struct polygon *poly,
									unsigned index, struct pt *pos);
struct pt polyPoint(struct polygon *poly,
										unsigned index);

struct polygon *polyTransform(struct polygon *poly,
															struct matrix *mtx);
void polyTransformIP(struct polygon *poly,
										 struct matrix *mtx);
struct polygon *polyClip(struct polygon *poly);

/* Returns a list of monotonic polygons 
 * which together form the original */
struct list *polySubdivide(struct polygon *poly);
/* Returns a list of triangles
 * which together form the original polygon */
struct polygon **polyTessellate(struct polygon *poly);

struct matrix *polyToMatrix(struct polygon *poly);
struct list *polyToPtList(struct polygon *poly);

void polyDraw(struct polygon *poly);

#endif
