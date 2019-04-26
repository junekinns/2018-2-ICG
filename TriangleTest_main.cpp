#pragma comment(linker, "/subsystem:\"windows\" \ /entry:\"mainCRTStartup\"")
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

void mydisplay() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 0.0, 1.0);
	glBegin(GL_POLYGON);
	glVertex2f(-0.5, -0.5);
	glVertex2f(-0.5, 0.5);
	glVertex2f(0.5, 0.5);
	glEnd();
	glutSwapBuffers();
}

int main(int argc, char* *argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Triangle Test:)");
	glutDisplayFunc(mydisplay);
	glutMainLoop();
	return 0;
}