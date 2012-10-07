
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
#include "polygon.h"

#define PI 3.141592653589793
#define ARROWSIZE 7
struct pt ARROWPOINTS[ARROWSIZE] = {
	{200, 0},
	{75, 100},
	{100, 50},
	{-200, 50},
	{-200, -50},
	{100, -50},
	{75, -100}};

struct polygon *arrow;

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
	polyTransformIP(arrow, transform);
	polyDraw(arrow);
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
			polyTransformIP(arrow, scale);
		}
		else if(btn == GLUT_RIGHT_BUTTON) {
			polyTransformIP(arrow, scaleInv);
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
		polyTransformIP(arrow, reflectX);
		break;
	case 'f':
		fill = !fill;
		break;
	case 's':
		mtxFree(transform);
		transform = mtxCreateI(3);
		speed = 0;
		break;
	case 'p': {
		struct matrix *tmp = mtxMul(scale, transform);
		mtxFree(transform);
		transform = tmp;
	}
		break;
	case 'o': {
		struct matrix *tmp = mtxMul(scaleInv, transform);
		mtxFree(transform);
		transform = tmp;
	}
		break;
	case 'm':
		polyTransformIP(arrow, rotateInv);
		break;
	case 'n':
		polyTransformIP(arrow, rotate);
		break;
	case 'w': {
		/* Resets the arrow */
		if(arrow) {
			polyFree(arrow);
		}
		arrow = polyCreatePoints(ARROWPOINTS, ARROWSIZE);
	}
		break;
	}
}

void initMatrices(void)
{
	arrow = polyCreatePoints(ARROWPOINTS, ARROWSIZE);

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
