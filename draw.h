
#ifndef _DRAW_H_
#define _DRAW_H_

#include <GL/gl.h>
#include <GL/glut.h>
#include <stdbool.h>

#include "matrix.h"

#define VIEWHEIGHT 500.0
#define VIEWWIDTH 500.0
#define OFFHEIGHT 100
#define OFFWIDTH 100
#define CENTERX 250
#define CENTERY 250

struct pt {
	GLint x, y;
};

enum Region {
	CENTER = 0,
	LEFT = 1,
	RIGHT = 2,
	TOP = 8,
	BOTTOM = 4
};

void drawView();
void drawLine(struct pt start, struct pt end);
bool clipLine(struct pt *p1, struct pt *p2);
bool ptCompare(struct pt lhs, struct pt rhs);
bool inViewport(int x, int y);

bool ptCompare(struct pt lhs, struct pt rhs);
enum Region pointRegion(struct pt point);
int interpolateX(struct pt p1, struct pt p2, int p1x);
int interpolateY(struct pt p1, struct pt p2, int p1y);
struct matrix *ptToMatrix(struct pt *pt);

#endif
