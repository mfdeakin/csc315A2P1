
#ifndef _DRAW_H_
#define _DRAW_H_

#include <GL/gl.h>
#include <GL/glut.h>
#include <stdbool.h>

#define VIEWHEIGHT 500.0
#define VIEWWIDTH 500.0
#define OFFHEIGHT 100
#define OFFWIDTH 100
#define CENTERX 250
#define CENTERY 250

struct pt {
	GLint x, y;
};

void drawLine(struct pt start, struct pt end);
bool clipLine(struct pt *p1, struct pt *p2);
void resize(GLsizei width, GLsizei height);
void mpress(int btn, int state, int x, int y);
bool ptCompare(struct pt lhs, struct pt rhs);

enum Region pointRegion(struct pt point);
int interpolateX(struct pt p1, struct pt p2, int p1x);
int interpolateY(struct pt p1, struct pt p2, int p1y);

#endif
