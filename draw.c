
#include "draw.h"

#include <math.h>

struct matrix *ptToMatrix(struct pt *pt)
{
	/* Point is expected to be in global coordinates,
	 * so convert it to arrow coordinates */
	struct matrix *mtx = mtxCreate(1, 3);
	mtxSet(mtx, 0, 0, pt->x - CENTERX - OFFWIDTH);
	mtxSet(mtx, 0, 1, pt->y - CENTERY - OFFHEIGHT);
	mtxSet(mtx, 0, 2, 1);
	return mtx;
}

bool ptCompare(struct pt lhs, struct pt rhs)
{
	if(lhs.x != rhs.x || lhs.y != rhs.y)
		return false;
	return true;
}

bool inViewport(int x, int y) {
	if(x > OFFWIDTH && y > OFFWIDTH &&
		 x < OFFWIDTH + VIEWWIDTH &&
		 y < OFFWIDTH + VIEWWIDTH)
		return true;
	return false;
}

void drawLine(struct pt start, struct pt end)
{
	/* Points must be in global coordinates */
	if(!ptCompare(start, end) &&
		 clipLine(&start, &end)) {
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POINTS);

		GLint deltax = end.x - start.x,
			deltay = end.y - start.y;
	
		GLfloat mag = sqrtf(deltax * deltax + deltay * deltay),
			dx = deltax / mag,
			dy = deltay / mag,
			x = start.x,
			y = start.y;

		int i;
		for(i = 0; i <= mag; i++) {
			glVertex2i((int)x, (int)y);
			x += dx;
			y += dy;

		}
		glEnd();
	}
}

bool clipLine(struct pt *p1, struct pt *p2)
{
	/* Points must be in global coordinates */
	enum Region reg1 = pointRegion(*p1),
		reg2 = pointRegion(*p2);
	if(reg1 & reg2) {
		return false;
	}
	if(!reg1 && !reg2)
		return true;
	if(p1->x < OFFWIDTH) {
		p1->y = interpolateX(*p1, *p2, OFFWIDTH);
		p1->x = OFFWIDTH;
	}
	else if(p1->x > OFFWIDTH + VIEWWIDTH) {
		p1->y = interpolateX(*p1, *p2, OFFWIDTH + VIEWWIDTH);
		p1->x = OFFWIDTH + VIEWWIDTH;
	}
	if(p1->y < OFFHEIGHT) {
		p1->x = interpolateY(*p1, *p2, OFFHEIGHT);
		p1->y = OFFHEIGHT;
	}
	else if(p1->y > OFFHEIGHT + VIEWHEIGHT) {
		p1->x = interpolateY(*p1, *p2, OFFHEIGHT + VIEWHEIGHT);
		p1->y = OFFHEIGHT + VIEWHEIGHT;
	}

	if(p2->x < OFFWIDTH) {
		p2->y = interpolateX(*p1, *p2, OFFWIDTH);
		p2->x = OFFWIDTH;
	}
	else if(p2->x > OFFWIDTH + VIEWWIDTH) {
		p2->y = interpolateX(*p1, *p2, OFFWIDTH + VIEWWIDTH);
		p2->x = OFFWIDTH + VIEWWIDTH;
	}
	if(p2->y < OFFHEIGHT) {
		p2->x = interpolateY(*p1, *p2, OFFHEIGHT);
		p2->y = OFFHEIGHT;
	}
	else if(p2->y > OFFHEIGHT + VIEWHEIGHT) {
		p2->x = interpolateY(*p1, *p2, OFFHEIGHT + VIEWHEIGHT);
		p2->y = OFFHEIGHT + VIEWHEIGHT;
	}
	reg1 = pointRegion(*p1);
	reg2 = pointRegion(*p2);
	if(reg1 & reg2) {
		return false;
	}
	return true;
}

void drawView(void)
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glVertex2i(OFFWIDTH, OFFHEIGHT);
	glVertex2i(OFFWIDTH, OFFHEIGHT + VIEWHEIGHT);
	glVertex2i(OFFWIDTH + VIEWWIDTH,
						 OFFHEIGHT + VIEWHEIGHT);
	glVertex2i(OFFWIDTH + VIEWWIDTH, OFFHEIGHT);
	glEnd();
}

enum Region pointRegion(struct pt point)
{
	enum Region reg = CENTER;
	if(point.x < OFFWIDTH)
		reg = LEFT;
	else if(point.x > OFFWIDTH + VIEWWIDTH)
		reg = RIGHT;
	if(point.y < OFFHEIGHT)
		reg |= BOTTOM;
	else if(point.y > OFFHEIGHT + VIEWHEIGHT)
		reg |= TOP;
	return reg;
}

int interpolateX(struct pt p1, struct pt p2, int newX)
{
	float dx = p1.x - p2.x,
		dy = p1.y - p2.y;
	return (int)dy / dx * (newX - p2.x) + p2.y;
}

int interpolateY(struct pt p1, struct pt p2, int newY)
{
	float dx = p1.x - p2.x,
		dy = p1.y - p2.y;
	return (int)dx / dy * (newY - p2.y) + p2.x;
}
