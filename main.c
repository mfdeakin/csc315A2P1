
#include <math.h>
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
struct matrix *transform;

void drawView(void);
void drawArrow(void);
void display(void);
void mpress(int btn, int state, int x, int y);
void resize(GLsizei width, GLsizei height);
void keypress(unsigned char key, int x, int y);
void timer(int val);

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
	/* First update the points with the transform matrix */
	for(int i = 0; i < ARROWPOINTS; i++) {
		struct matrix *tmp = mtxMul(transform, arrow[i]);
		mtxFree(arrow[i]);
		arrow[i] = tmp;
	}
	for(int i = 0; i < ARROWPOINTS - 1; i++) {
		drawLine((struct pt){mtxGet(arrow[i], 0, 0) + CENTERX,
					mtxGet(arrow[i], 0, 1) + CENTERY},
			(struct pt){mtxGet(arrow[i + 1], 0, 0) + CENTERX,
					mtxGet(arrow[i + 1], 0, 1) + CENTERY});
	}

	drawLine((struct pt){mtxGet(arrow[0], 0, 0) + CENTERX,
				mtxGet(arrow[0], 0, 1) + CENTERY},
		(struct pt){mtxGet(arrow[ARROWPOINTS - 1], 0, 0) + CENTERX,
				mtxGet(arrow[ARROWPOINTS - 1], 0, 1) + CENTERY});
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
	if(btn == GLUT_LEFT_BUTTON) {
		struct matrix *tmp = mtxMul(rotate, transform);
		mtxFree(transform);
		transform = tmp;
	}
	else if(btn == GLUT_RIGHT_BUTTON) {
		struct matrix *tmp = mtxMul(rotateInv, transform);
		mtxFree(transform);
		transform = tmp;
	}
}

void keypress(unsigned char key, int x, int y)
{
	key = tolower(key);
	switch(key) {
	case 'q':
		exit(0);
	case 'r':
		glutPostRedisplay();
		break;
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
		{cos(1 * PI / 180), sin(1 * PI / 180), 0},
		{-sin(1 * PI / 180), cos(1 * PI / 180), 0},
		{0, 0, 1}};
	rotateInv = mtxCreate(3, 3);
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			mtxSet(rotateInv, i, j, rotnegPoints[i][j]);
	transform = mtxCreateI(3);
}

int main(int argc, char **argv)
{
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
	glutMainLoop();
  return 0;
}
