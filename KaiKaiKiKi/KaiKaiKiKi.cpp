/*
-----------------------------------------------------------------------
Lecture: Interactive Computer Graphics
Instructor: π⁄∞Ê¡÷(Park Kyoung Ju)
Name / StudentID / Major: ±Ë ¡ÿ(Kim June) / 20156260 / Digital Imaging
OS: MS Windows 10
IDE:MS Visual Studio 2017 Community Ver 15.8.4
-----------------------------------------------------------------------
Project 1 - 01: Pick a famous painting, Draw it with OpenGL
Artist / Painting: Takashi Murakami - Flower(KaiKai KiKi)
Due Date: 2018.10.02(Tue)
Introduction: It creates a fullscreen window and draw flowers randomly
Control:
-Keyboard 'ESC' down to Exit the program.
-Keyboard 'Backspace' down to clear screen
*/
#include <iostream>
#include <Windows.h>
#include <cmath>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include "../vec.h"

using namespace std;

//-------Function Declaration------------//
int RandomRange(int min, int max);
float Randomf();
void GetDesktopResolution(int& horizontal, int& vertical);
void ellipse(vec2 center, float w, float h);
void halfEllipse(vec2 center, float w, float h);
void body(vec2 center, float w, float h);
void mouth(vec2 center, float flowerSize);
void eyes(float flowerSize);
void leaves(float flowerSize);
void drawFlower(float x, float y, float flowerSize);
void init();
void idle();
void display();
void keyboardDown(unsigned char key, int x, int y);
//--------------------------------------//

int width, height; //Current screen resolution
bool isBodyYellow = false; //flowerbody check

int RandomRange(int min, int max) { //Get random integer min~max
	int rand_num;
	rand_num = rand();
	rand_num = rand() % max + min;
	return rand_num;
}
float Randomf() { //Get random float 0.0f~1.0f
	return (float)rand() / RAND_MAX;
}
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}
void ellipse(vec2 center, float w, float h)
{
	float dx, dy;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 36; i++) {
		float angle = radians(10.0*i);
		dx = w * cos(angle);
		dy = h * sin(angle);
		glVertex2f(center.x + dx, center.y + dy);
	}
	glEnd();

}
void halfEllipse(vec2 center, float w, float h)
{
	float dx, dy;
	glBegin(GL_POLYGON);
	for (int i = 1; i < 18; i++) { //180 degrees for half ellipse
		float angle = radians(10.0*i);
		dx = w * cos(angle);
		dy = h * sin(angle);
		glVertex2f(center.x + dx, center.y + dy);
	}
	glEnd();
}
void body(vec2 center, float w, float h)
{
	//Drawing the flower body
	glPushMatrix();
	float bodyColorPercent = Randomf();
	if (bodyColorPercent < 0.6f) { //Body with white color(60% of chance)
		glColor3f(1, 1, 1);
		isBodyYellow = false;
	}
	else if (bodyColorPercent < 0.9f) {
		glColor3f(1, 1, 0); //Yellow body color, -> Red mouth , black eye with high chance(Not all).
		isBodyYellow = true;
	}
	else {
		glColor3f(Randomf(), Randomf(), Randomf());
		isBodyYellow = false;
	}
	ellipse(center, w, h);
	glPopMatrix();

	//Drawing outline
	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (Randomf() < 0.8f) { //Body outline with black color(80% of chance)
		glColor3f(0, 0, 0);
	}
	else {
		glColor3f(Randomf(), Randomf(), Randomf());
	}
	ellipse(center, w, h);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();
}
void mouth(vec2 center, float flowerSize) {
	float mouthColorPercent = Randomf();

	//Draw the outline of the mouth with two halfEllipses.
	glPushMatrix();
	glLineWidth(flowerSize / 2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(Randomf(), Randomf(), Randomf());
	halfEllipse(vec2(0, 0), 4.5 * flowerSize, 1.5 * flowerSize); //Upper lip
	glRotatef(180, 0, 0, 1);
	halfEllipse(vec2(0, -1.3*flowerSize), 4.5 * flowerSize, 5.8 * flowerSize);//Lower lip
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();

	//Draw the mouth with two halfEllipses.
	glPushMatrix();
	if (isBodyYellow || mouthColorPercent < 0.3f)
		glColor3f(1, 0, 0);
	else
		glColor3f(Randomf(), Randomf(), Randomf());
	halfEllipse(vec2(0, 0), 4.5 * flowerSize, 1.5 * flowerSize); //Upper lip
	glRotatef(180, 0, 0, 1);
	halfEllipse(vec2(0, -1.3*flowerSize), 4.5 * flowerSize, 5.8 * flowerSize); //Lower lip
	glPopMatrix();
}
void eyes(float flowerSize) {
	float eyeColorPercent = Randomf();

	//Left eye
	glPushMatrix();
	glTranslatef(-2.1*flowerSize, 3 * flowerSize, 0);
	glRotatef(-30, 0, 0, 1);

	//Random eye color
	if (isBodyYellow || eyeColorPercent < 0.5f) //if body color is yellow, eye would be black color 
		glColor3f(0, 0, 0);
	else
		glColor3f(Randomf(), Randomf(), Randomf());
	ellipse(vec2(0, 0), 0.45*flowerSize, 0.7*flowerSize); //Outside of eye

	if (eyeColorPercent < 0.5f)
		glColor3f(1, 1, 1);
	else
		glColor3f(Randomf(), Randomf(), Randomf());
	ellipse(vec2(-0.2*flowerSize, 0.3*flowerSize), (0.35*flowerSize) / 2, 0.28*flowerSize); //Inside, Upperside

	if (isBodyYellow || eyeColorPercent < 0.5f)
		glColor3f(1, 1, 1);
	else
		glColor3f(Randomf(), Randomf(), Randomf());
	ellipse(vec2(0.1*flowerSize, -0.35*flowerSize), (0.3*flowerSize) / 2, 0.25*flowerSize); //Inside, Lowerside
	glPopMatrix();

	//Right eye
	glPushMatrix();
	glTranslatef(2.1 * flowerSize, 3 * flowerSize, 0);
	glRotatef(30, 0, 0, 1);
	if (isBodyYellow || eyeColorPercent < 0.5f)
		glColor3f(0, 0, 0);
	else
		glColor3f(Randomf(), Randomf(), Randomf());
	ellipse(vec2(0, 0), 0.45*flowerSize, 0.7*flowerSize); //Outside of eye

	if (eyeColorPercent < 0.5f)
		glColor3f(1, 1, 1);
	else
		glColor3f(Randomf(), Randomf(), Randomf());
	ellipse(vec2(0, 0.4*flowerSize), (0.35*flowerSize) / 2, 0.28*flowerSize); //Inside, Upperside


	if (isBodyYellow || eyeColorPercent < 0.5f)
		glColor3f(1, 1, 1);
	else
		glColor3f(Randomf(), Randomf(), Randomf());
	ellipse(vec2(0.05*flowerSize, -0.4*flowerSize), (0.3*flowerSize) / 2, 0.23*flowerSize); //Inside, Lowerside
	glPopMatrix();
}
void leaves(float flowerSize) {
	float leafPercent = Randomf();

	//Draw the outline(head of leaf)
	float outLineColor = Randomf();
	glPushMatrix();
	glLineWidth(flowerSize / 2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(outLineColor, outLineColor, outLineColor);
	for (int i = 0; i < 12; i++) {
		glRotatef(30, 0, 0, 1); //30 * 12 = 360
		ellipse(vec2(10 * flowerSize, 0), 10 * flowerSize*sin(M_PI / 12), 10 * flowerSize*sin(M_PI / 12));
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();

	//Draw the leaf, leaf drawing modes are two kinds. Single color, Double colors
	glPushMatrix();
	if (leafPercent < 0.5f) { //Single color mode
		glColor3f(Randomf(), Randomf(), Randomf()); //Temp
		for (int i = 0; i < 12; i++) {
			glRotatef(30, 0, 0, 1); //30 * 12 = 360
			glPushMatrix();
			ellipse(vec2(10 * flowerSize, 0), 10 * flowerSize*sin(M_PI / 12), 10 * flowerSize*sin(M_PI / 12));
			glPopMatrix();
			glBegin(GL_TRIANGLES);
			glVertex2f(10 * flowerSize*cos(M_PI / 12), 10 * flowerSize*sin(M_PI / 12));
			glVertex2f(0, 0);
			glVertex2f(10 * flowerSize*cos(-M_PI / 12), 10 * flowerSize*sin(-M_PI / 12));
			glEnd();
		}
	}
	else { //Double color mode. leaf color changes alternatively but more chance with white color
		if (Randomf() < 0.5f)
			glColor3f(1, 1, 1);
		else
			glColor3f(Randomf(), Randomf(), Randomf());
		for (int i = 0; i < 12; i += 2) { //Odd number of leaves
			glRotatef(60, 0, 0, 1);
			glPushMatrix();
			ellipse(vec2(10 * flowerSize, 0), 10 * flowerSize*sin(M_PI / 12), 10 * flowerSize*sin(M_PI / 12));
			glPopMatrix();
			glBegin(GL_TRIANGLES);
			glVertex2f(10 * flowerSize*cos(M_PI / 12), 10 * flowerSize*sin(M_PI / 12));
			glVertex2f(0, 0);
			glVertex2f(10 * flowerSize*cos(-M_PI / 12), 10 * flowerSize*sin(-M_PI / 12));
			glEnd();
		}
		glColor3f(Randomf(), Randomf(), Randomf());
		glRotatef(30, 0, 0, 1);
		for (int i = 1; i < 12; i += 2) { //Even number of leaves
			glRotatef(60, 0, 0, 1);
			glPushMatrix();
			ellipse(vec2(10 * flowerSize, 0), 10 * flowerSize*sin(M_PI / 12), 10 * flowerSize*sin(M_PI / 12));
			glPopMatrix();
			glBegin(GL_TRIANGLES);
			glVertex2f(10 * flowerSize*cos(M_PI / 12), 10 * flowerSize*sin(M_PI / 12));
			glVertex2f(0, 0);
			glVertex2f(10 * flowerSize*cos(-M_PI / 12), 10 * flowerSize*sin(-M_PI / 12));
			glEnd();
		}
	}
	glPopMatrix();

	//Draw the ouline of body-to-leafhead
	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(flowerSize / 6);
	glColor3f(outLineColor, outLineColor, outLineColor);
	for (int i = 0; i < 12; i++) {
		glRotatef(30, 0, 0, 1);
		glBegin(GL_LINES);
		glVertex2f(11 * flowerSize*cos(M_PI / 12), 11 * flowerSize*sin(M_PI / 12));
		glVertex2f(0, 0);
		glEnd();
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();
}
void drawFlower(float x, float y, float flowerSize) {
	glPushMatrix();
	glTranslatef(x, y, 0);
	leaves(flowerSize);
	body(vec2(0, 0), 6 * flowerSize, 6 * flowerSize);
	mouth(vec2(0, 0), flowerSize);
	eyes(flowerSize);
	glPopMatrix();
}
void init()
{
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glOrtho(0, width, 0, height, -1, 1);
}
void idle() {
	Sleep(100); //Delay between drawing one flower
	glutPostRedisplay();
}
void display()
{
	drawFlower(RandomRange(0, width), RandomRange(0, height), RandomRange(3, 17));
	glFlush();
}
void keyboardDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27://Keyboard 'ESC' to exit the program
		exit(EXIT_SUCCESS);
		break;
	case 8: //Keyboard 'Backspace' to clear screen
		glClear(GL_COLOR_BUFFER_BIT);
		break;
	}
}
int main(int argc, char **argv)
{
	int horizontal = 0;
	int vertical = 0;
	GetDesktopResolution(horizontal, vertical);
	width = horizontal;
	height = vertical;
	srand(time(NULL));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
	glutInitWindowSize(width, height);
	glutCreateWindow("InteractiveCG_Project01-1_20156260");
	glewInit();
	init();
	glutFullScreen();
	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardDown);
	glutMainLoop();
	return 0;
}