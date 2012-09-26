
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glut.h>

#include "draw.h"

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawView();
	glFlush();
 	glutSwapBuffers();
}

void mpress(int btn, int state, int x, int y)
{
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

void keypress(unsigned char key, int x, int y)
{
	switch(key) {
	case 'q':
	case 'Q':
		exit(0);
	}
}

int main(int argc, char **argv)
{
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
	glutMainLoop();
  return 0;
}
