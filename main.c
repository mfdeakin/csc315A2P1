
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glut.h>

#include "draw.h"
#include "matrix.h"
#include "list.h"

#define PI 3.141592653589793
#define ARROWPOINTS 7

struct matrix *arrow[ARROWPOINTS];
struct matrix *rotate, *rotateInv;
struct matrix *scale, *scaleInv;
struct matrix *reflectX;
struct matrix *transform;

bool fill;
int speed;

void drawView(void);
void drawArrow(void);
void display(void);
void mpress(int btn, int state, int x, int y);
void resize(GLsizei width, GLsizei height);
void keypress(unsigned char key, int x, int y);
void timer(int val);
struct pt mtxToPoint(struct matrix *mtx);
struct matrix *ptToMatrix(struct pt *pt);
void transformArrow(struct matrix *mtx);
struct list *clipArrow();

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawView();
	drawArrow();
	glFlush();
 	glutSwapBuffers();
}

void timer(int val)
{
	glutPostRedisplay();
	glutTimerFunc(10, timer, val + 1);
}

void drawArrow(void)
{
	transformArrow(transform);
	struct list *lst = clipArrow();
	list_gotofront(lst);
	struct matrix *mtx = list_next(lst);
	struct pt prev, next, first;
	next = mtxToPoint(mtx);
	mtxFree(mtx);
	first = next;
	mtx = list_next(lst);
	while(mtx) {
		prev = next;
		next = mtxToPoint(mtx);
		glBegin(GL_POINTS);
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex2i(next.x + OFFWIDTH, next.y + OFFHEIGHT);
		glVertex2i(prev.x + OFFWIDTH, prev.y + OFFHEIGHT);
		glEnd();
		drawLine(prev, next);
		glBegin(GL_POINTS);
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex2i(next.x + OFFWIDTH, next.y + OFFHEIGHT);
		glVertex2i(prev.x + OFFWIDTH, prev.y + OFFHEIGHT);
		glEnd();
		mtxFree(mtx);
		mtx = list_next(lst);
	}
	drawLine(next, first);
	list_delete(lst);
}

struct list *clipArrow()
{
	struct list *lst = list_create(0);
	for(int i = 0; i < ARROWPOINTS; i++) {
		float x = mtxGet(arrow[i], 0, 0) + CENTERX + OFFWIDTH;
		float y = mtxGet(arrow[i], 0, 1) + CENTERY + OFFHEIGHT;
		if(inViewport(x, y)) {
			struct matrix *tmp = mtxCopy(arrow[i]);
			list_insert(lst, tmp);
		}
		else {
			struct pt cur = mtxToPoint(arrow[i]), prv;
			if(i == 0)
				prv = mtxToPoint(arrow[ARROWPOINTS - 1]);
			else
				prv = mtxToPoint(arrow[i - 1]);
			enum Region r1 = pointRegion(cur),
				r2 = pointRegion(prv);
			struct pt curbuf = cur;
			if(!(r1 & r2)) {
				printf("x: %d, y: %d\n", curbuf.x, curbuf.y);
				glBegin(GL_POINTS);
				if(curbuf.x < 0) {
					glColor3f(0.0f, 1.0f, 0.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
					curbuf.y = interpolateX(curbuf, prv, 0);
					curbuf.x = 0;
					glColor3f(0.0f, 0.0f, 1.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
					printf("new x: %d, new y: %d\n", curbuf.x, curbuf.y);
				}
				if(curbuf.x > VIEWWIDTH) {
					glColor3f(0.0f, 1.0f, 0.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
					curbuf.y = interpolateX(curbuf, prv, VIEWWIDTH);
					curbuf.x = VIEWWIDTH;
					glColor3f(0.0f, 0.0f, 1.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
				}
				if(curbuf.y < 0) {
					glColor3f(0.0f, 1.0f, 0.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
					curbuf.x = interpolateY(curbuf, prv, 0);
					curbuf.y = 0;
					glColor3f(0.0f, 0.0f, 1.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
				}
				if(curbuf.y > VIEWHEIGHT) {
					glColor3f(0.0f, 1.0f, 0.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
					curbuf.x = interpolateY(curbuf, prv, VIEWHEIGHT);
					curbuf.y = VIEWHEIGHT;
					glColor3f(0.0f, 0.0f, 1.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
				}
				struct matrix *mtx = ptToMatrix(&curbuf);
				list_insert(lst, mtx);
				glEnd();
			}
			else {
				/* Not certain what to do here yet */
			}
			struct pt nxt;
			if(i == ARROWPOINTS - 1)
				nxt = mtxToPoint(arrow[0]);
			else
				nxt = mtxToPoint(arrow[i + 1]);
			curbuf = cur;
			r1 = pointRegion(cur);
			r2 = pointRegion(nxt);
			if(!(r1 & r2)) {
				glBegin(GL_POINTS);
				if(curbuf.x < 0) {
					glColor3f(0.0f, 1.0f, 0.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
					curbuf.y = interpolateX(curbuf, nxt, 0);
					curbuf.x = 0;
					glColor3f(0.0f, 0.0f, 1.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
				}
				if(curbuf.x > VIEWWIDTH) {
					glColor3f(0.0f, 1.0f, 0.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
					curbuf.y = interpolateX(curbuf, nxt, VIEWWIDTH);
					curbuf.x = VIEWWIDTH;
					glColor3f(0.0f, 0.0f, 1.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
				}
				if(curbuf.y < 0) {
					glColor3f(0.0f, 1.0f, 0.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
					curbuf.x = interpolateY(curbuf, nxt, 0);
					curbuf.y = 0;
					glColor3f(0.0f, 0.0f, 1.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
				}
				if(curbuf.y > VIEWHEIGHT) {
					glColor3f(0.0f, 1.0f, 0.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
					curbuf.x = interpolateY(curbuf, nxt, VIEWHEIGHT);
					curbuf.y = VIEWHEIGHT;
					glColor3f(0.0f, 0.0f, 1.0f);
					glVertex2i(curbuf.x + OFFWIDTH,
										 curbuf.y + OFFHEIGHT);
				}
				struct matrix *mtx = ptToMatrix(&curbuf);
				list_insert(lst, mtx);
				glEnd();
			}
		}
	}
	return lst;
}

void drawView(void)
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);
	struct pt pos;
	for(pos.x = 100; pos.x < OFFWIDTH + VIEWWIDTH;
			pos.x++)
		for(pos.y = 100; pos.y < OFFHEIGHT + VIEWHEIGHT;
				pos.y++)
			glVertex2i(pos.x, pos.y);
	glEnd();
}

void transformArrow(struct matrix *mtx) {
	for(int i = 0; i < ARROWPOINTS; i++) {
		struct matrix *tmp = mtxMul(mtx, arrow[i]);
		mtxFree(arrow[i]);
		arrow[i] = tmp;
	}
}

void resize(GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, width, 0.0,
					height, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
}

void mpress(int btn, int state, int x, int y)
{
	if(state != GLUT_DOWN)
		return;
	if(inViewport(x, y)) {
		if(btn == GLUT_LEFT_BUTTON && speed < 10) {
			struct matrix *tmp = mtxMul(rotate, transform);
			mtxFree(transform);
			transform = tmp;
			speed++;
		}
		else if(btn == GLUT_RIGHT_BUTTON && speed > -10) {
			struct matrix *tmp = mtxMul(rotateInv, transform);
			mtxFree(transform);
			transform = tmp;
			speed--;
		}
	}
	else {
		if(btn == GLUT_LEFT_BUTTON) {
			transformArrow(scale);
		}
		else if(btn == GLUT_RIGHT_BUTTON) {
			transformArrow(scaleInv);
		}
	}
}

void keypress(unsigned char key, int x, int y)
{
	key = tolower(key);
	switch(key) {
	case 'q':
		exit(0);
	case 'r':
		transformArrow(reflectX);
		break;
	case 'f':
		fill = !fill;
		break;
	case 's':
		mtxFree(transform);
		transform = mtxCreateI(3);
		speed = 0;
		break;
	case 'w': {
		/* Resets the arrow */
		int points[ARROWPOINTS][3] = {
			{200, 0, 1},
			{75, 100, 1},
			{100, 50, 1},
			{-200, 50, 1},
			{-200, -50, 1},
			{100, -50, 1},
			{75, -100, 1}};
		for(int i = 0; i < ARROWPOINTS; i++) {	
			mtxFree(arrow[i]);
			arrow[i] = mtxCreate(1, 3);
			for(int j = 0; j < 3; j++) {
				mtxSet(arrow[i], 0, j, points[i][j]);
			}
		}
	}

	}
}

void initMatrices(void)
{
	int points[ARROWPOINTS][3] = {
		{200, 0, 1},
		{75, 100, 1},
		{100, 50, 1},
		{-200, 50, 1},
		{-200, -50, 1},
		{100, -50, 1},
		{75, -100, 1}};
	for(int i = 0; i < ARROWPOINTS; i++) {	
		arrow[i] = mtxCreate(1, 3);
		for(int j = 0; j < 3; j++) {
			mtxSet(arrow[i], 0, j, points[i][j]);
		}
	}

	float rotPoints[3][3] = {
		{cos(1 * PI / 180), -sin(1 * PI / 180), 0},
		{sin(1 * PI / 180), cos(1 * PI / 180), 0},
		{0, 0, 1}};
	rotate = mtxCreate(3, 3);
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			mtxSet(rotate, i, j, rotPoints[i][j]);

	float rotnegPoints[3][3] = {
		{cosf(1 * PI / 180), sinf(1 * PI / 180), 0},
		{-sinf(1 * PI / 180), cosf(1 * PI / 180), 0},
		{0, 0, 1}};
	rotateInv = mtxCreate(3, 3);
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			mtxSet(rotateInv, i, j, rotnegPoints[i][j]);

	float scalePoints[3][3] = {
		{1.05f, 0.0f, 0.0f},
		{0.0f, 1.05f, 0.0f},
		{0.0f, 0.0f, 1.0f}};
	scale = mtxCreate(3, 3);
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			mtxSet(scale, i, j, scalePoints[i][j]);

	float scaleInvPoints[3][3] = {
		{1 / 1.05f, 0.0f, 0.0f},
		{0.0f, 1 / 1.05f, 0.0f},
		{0.0f, 0.0f, 1.0f}};
	scaleInv = mtxCreate(3, 3);
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			mtxSet(scaleInv, i, j, scaleInvPoints[i][j]);

	float reflectPoints[3][3] = {
		{-1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}};
	reflectX = mtxCreate(3, 3);
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			mtxSet(reflectX, i, j, reflectPoints[i][j]);

	transform = mtxCreateI(3);
}

struct pt mtxToPoint(struct matrix *mtx)
{
	return (struct pt){
		(int)mtxGet(mtx, 0, 0) + CENTERX,
			(int)mtxGet(mtx, 0, 1) + CENTERY
			};
}

struct matrix *ptToMatrix(struct pt *pt)
{
	struct matrix *mtx = mtxCreate(1, 3);
	mtxSet(mtx, 0, 0, pt->x);
	mtxSet(mtx, 0, 1, pt->y);
	mtxSet(mtx, 0, 0, 1);
	return mtx;
}

int main(int argc, char **argv)
{
	fill = false;
	speed = 0;
	initMatrices();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 10);
	glutInitWindowSize(VIEWWIDTH + 2 * OFFWIDTH,
										 VIEWHEIGHT + 2 * OFFHEIGHT);
	glutCreateWindow("Program 2");
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutMouseFunc(mpress);
	glutKeyboardFunc(keypress);
	glutTimerFunc(10, timer, 0);
	glPointSize(5);
	glutMainLoop();
  return 0;
}
