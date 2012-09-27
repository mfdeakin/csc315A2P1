
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

/* Whether or not to fill the arrow */
bool fill;
/* How fast the arrow is rotating */
int speed;

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
		//		drawLine(prev, next);
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2i(prev.x, prev.y);
		glVertex2i(next.x, next.y);
		glEnd();

		mtxFree(mtx);
		mtx = list_next(lst);
	}
	/* drawLine(next, first); */
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2i(first.x, first.y);
	glVertex2i(next.x, next.y);
	glEnd();
	list_delete(lst);
}

struct list *clipArrow()
{
	struct list *lst = list_create(0);
	for(int i = 0; i < ARROWPOINTS; i++) {
		struct pt cur = mtxToPoint(arrow[i]);
		struct pt prv;
		if(i == 0) {
			prv = mtxToPoint(arrow[ARROWPOINTS - 1]);
		}
		else {
			prv = mtxToPoint(arrow[i - 1]);
		}
		struct pt curbuf = cur,
			prvbuf = prv;
		clipLine(&curbuf, &prvbuf);
		struct matrix *mtx;
		mtx = ptToMatrix(&prvbuf);
		list_insert(lst, mtx);
		/* We always need to insert the current point */
		mtx = ptToMatrix(&curbuf);
		list_insert(lst, mtx);
	}
	return lst;
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
	case 'm':
		transformArrow(rotateInv);
		break;
	case 'n':
		transformArrow(rotate);
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
		break;
	}
}

void initMatrices(void)
{
	float points[ARROWPOINTS][3] = {
		{200, 0, 1},
		{75, 100, 1},
		{100, 50, 1},
		{-200, 50, 1},
		{-200, -50, 1},
		{100, -50, 1},
		{75, -100, 1}};
	for(int i = 0; i < ARROWPOINTS; i++) {	
		arrow[i] = mtxCreate(1, 3);
		for(int j = 0; j < 3; j++)  {
			mtxSet(arrow[i], 0, j, points[i][j]);
		}
	}

	float rotPoints[3][3] = {
		{cos(1 * PI / 180), -sin(1 * PI / 180), 0},
		{sin(1 * PI / 180), cos(1 * PI / 180), 0},
		{0, 0, 1}};
	rotate = mtxFromArray((float *)rotPoints, 3, 3);

	float rotnegPoints[3][3] = {
		{cosf(1 * PI / 180), sinf(1 * PI / 180), 0},
		{-sinf(1 * PI / 180), cosf(1 * PI / 180), 0},
		{0, 0, 1}};
	rotateInv = mtxFromArray((float *)rotnegPoints, 3, 3);

	float scalePoints[3][3] = {
		{1.05f, 0.0f, 0.0f},
		{0.0f, 1.05f, 0.0f},
		{0.0f, 0.0f, 1.0f}};
	scale = mtxFromArray((float *)scalePoints, 3, 3);

	float scaleInvPoints[3][3] = {
		{1 / 1.05f, 0.0f, 0.0f},
		{0.0f, 1 / 1.05f, 0.0f},
		{0.0f, 0.0f, 1.0f}};
	scaleInv = mtxFromArray((float *)scaleInvPoints, 3, 3);

	float reflectPoints[3][3] = {
		{-1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}};
	reflectX = mtxFromArray((float *)reflectPoints, 3, 3);
	transform = mtxCreateI(3);
}

struct pt mtxToPoint(struct matrix *mtx)
{
	/* Matrix is expected to be in arrow coordinates,
	 * so convert it to global coordinates */
	return (struct pt){
		(GLint)mtxGet(mtx, 0, 0) + CENTERX + OFFWIDTH,
			(GLint)mtxGet(mtx, 0, 1) + CENTERY + OFFHEIGHT
			};
}

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
