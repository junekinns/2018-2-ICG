/*
-----------------------------------------------------------------------
Lecture: Interactive Computer Graphics
Instructor: 박경주(Park Kyoung Ju)
Name / StudentID / Major: 김 준(Kim June) / 20156260 / Digital Imaging
OS: MS Windows 10
IDE:MS Visual Studio 2017 Community Ver 15.8.4
-----------------------------------------------------------------------
Project 1 - 02: Make an interactive drawing tool
Interactive drawing tool
-------------구현 된 것--------------
	* 사용자의 키보드 인터랙션을 기반으로 점, 선, 삼각형, 사각형, 타원을 선택
		'p'  점
		'l' 선
		't' 삼각형
		'q' 사각형
		'e' 타원

	* 사용자의 마우스 인터랙션을 기반으로 도형 위치를 선정
		파워포인트의 도형 그리기 마우스 인터랙션과 동일하게 할 것
	* 사용자의 키보드 인터랙션을 기반으로 색상 선택
	   'r'  빨강색
	   'g' 초록색
	   'b' 파란색
	   'c' 시안
	   'm' 마젠타
	   'y' 노랑
	 * 사용자의 키보드 인터랙션을 기반으로 도형의 특성 변경
	   '+'   앞으로 그릴 점, 선의 크기 증가
	   '-'   앞으로 그릴 점, 선의 크기 감소
--------------------------------------

------------구현 안 된 것-------------
    * 도형선택
	* 도형선택하여 이동하는 기능 : 마우스로 도형영역을 클릭하여 마우스를 드래그한 후 릴리스하면, 도형이 릴리스된 위치로 이동
--------------------------------------
Due Date: 2018.10.07(Sun)
*/
#include <iostream>
#include <cmath>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include "../vec.h"
using namespace std;

//-------Function Declaration------------//
void closeAllBrush();
void keyboardDown(unsigned char key, int x, int y);
void savePointBrush(int x, int y);
void mouseButton(int button, int state, int x, int y);
void mouseDrag(int x, int y);
void pointsDisplay();
void lineDisplay();
void quadDisplay();
void triangleDisplay();
void ellipseDisplay();
void RenderString(GLdouble x, GLdouble y, const std::string &string);
void uiDisplay();
//--------------------------------------//

float r = 1.0, g = 0.0, b = 0.0, a = 1.0; //Color value
int width = 800; int height = 600;
int selectX = -100; // Selected ui x
int scaleValue = 1; // ScaleValue
GLfloat mouseX, mouseY;
GLfloat drag_mouseX, drag_mouseY;
GLfloat tx, ty;
GLfloat tx_old, ty_old;
bool translateMode = false;
vec2 centerV = (-10, -10);
vec2 sumV = (0, 0);

struct myQuad {
	float r;
	float g;
	float b;
	float a;
	vec2 p1;
	vec2 p2;
};
myQuad *quads;
int quadCount = 0;
vec2 curQuad1; vec2 curQuad2;

struct myTriangle {
	float r;
	float g;
	float b;
	float a;
	vec2 p1;
	vec2 p2;
};
myTriangle *triangles;
int triangleCount = 0;
vec2 curTri1; vec2 curTri2;

struct myLine {
	float r;
	float g;
	float b;
	float a;
	float size;
	vec2 p1;
	vec2 p2;
};
vec2 curLine1;
vec2 curLine2;
myLine *lines;
int lineCount = 0;

struct  myBrush { //Point
	float r;
	float g;
	float b;
	float a;
	float size;
	vec2 p;
};
myBrush *brushes;
int brCount = 0;

struct myEllipse {
	float r;
	float g;
	float b;
	float a;
	float size;
	float radius_width;
	float radius_height;
	vec2 center;
	vec2 p1;
	vec2 p2;
};
myEllipse curEllipse;
int ellipseCount = 0;
myEllipse *ellipses;

bool isPointBrush = false;
bool isLineDraw = false;
bool isQuadDraw = false;
bool isEllipseDraw = false;
bool isTriangleDraw = false;

void closeAllBrush() {
	isLineDraw = false;
	isPointBrush = false;
	isQuadDraw = false;
	isTriangleDraw = false;
	isEllipseDraw = false;
}
void keyboardDown(unsigned char key, int x, int y)
{
	switch (key)
	{
		/*
	case 'z':
		translateMode = !translateMode; //Test
		break;*/
	case '+':
		scaleValue++;
		break;
	case '-':
		scaleValue--;
		if (scaleValue < 0)
			scaleValue = 0;
		break;
	case 'r':
		r = 1.0, g = 0.0, b = 0.0, a = 1.0;
		break;
	case 'g':
		r = 0.0, g = 1.0, b = 0.0, a = 1.0;
		break;
	case 'b':
		r = 0.0, g = 0.0, b = 1.0, a = 1.0;
		break;
	case 'c':
		r = 0.0, g = 1.0, b = 1.0, a = 1.0;
		break;
	case 'm':
		r = 1.0, g = 0.0, b = 1.0, a = 1.0;
		break;
	case 'y':
		r = 1.0, g = 1.0, b = 0.0, a = 1.0;
		break;
	case 'p':
		if (isPointBrush) {
			isPointBrush = false;
			selectX = -100;
		}
		else {
			closeAllBrush();
			isPointBrush = true;
			selectX = 0;
		}
		printf("PointBrush\n");
		break;
	case 'l':
		if (isLineDraw) {
			isLineDraw = false;
			selectX = -100;
		}
		else {
			closeAllBrush();
			isLineDraw = true;
			selectX = 1;
		}
		printf("Line\n");
		break;
	case 't':
		if (isTriangleDraw) {
			isTriangleDraw = false;
			selectX = -100;
		}
		else {
			closeAllBrush();
			isTriangleDraw = true;
			selectX = 2;
		}
		printf("Triangle\n");
		break;
	case 'q':
		if (isQuadDraw) {
			isQuadDraw = false;
			selectX = -100;
		}
		else {
			closeAllBrush();
			isQuadDraw = true;
			selectX = 3;
		}
		printf("Quad\n");
		break;
	case 'e':
		if (isEllipseDraw) {
			isEllipseDraw = false;
			selectX = -100;
		}
		else {
			closeAllBrush();
			isEllipseDraw = true;
			selectX = 4;
		}
		printf("FCircle\n");
		break;
	default:
		selectX = -100;
		closeAllBrush();
		break;
	case 27: //ESC key
		exit(EXIT_SUCCESS);
		break;
	}
	glutPostRedisplay();
}
void savePointBrush(int x, int y) {
	if (brCount % 100 == 99) {
		brushes = (myBrush*)realloc(brushes, (100 + brCount) * sizeof(myBrush));
	}
	myBrush bb;
	bb.r = r;
	bb.g = g;
	bb.b = b;
	bb.a = a;
	bb.p.x = x;
	bb.p.y = y;
	bb.size = (float)scaleValue;
	brushes[brCount] = bb;

	brCount++;
}
void mouseButton(int button, int state, int x, int y) {
	mouseX = (GLfloat)x;
	mouseY = -(GLfloat)y + (GLfloat)height; // Set OpenGL Window -> Lowerleft = (0,0), UpperRight = (width,height)
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		if (isPointBrush) {
			savePointBrush(mouseX, mouseY);
		}
		if (isLineDraw) {
			curLine1.x = mouseX;
			curLine1.y = mouseY;
			curLine2.x = mouseX;
			curLine2.y = mouseY;
		}
		if (isQuadDraw) {
			curQuad1.x = mouseX;
			curQuad1.y = mouseY;
			curQuad2.x = mouseX;
			curQuad2.y = mouseY;
		}
		if (isTriangleDraw) {
			curTri1.x = mouseX;
			curTri1.y = mouseY;
			curTri2.x = mouseX;
			curTri2.y = mouseY;
		}
		if (isEllipseDraw) {
			curEllipse.p1.x = mouseX;
			curEllipse.p1.y = mouseY;
			curEllipse.p2.x = mouseX;
			curEllipse.p2.y = mouseY;
		}
		cout << "mouse Down" << mouseX << "-" << mouseY << endl;
	}
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)) {
		std::cout << "mouse up " << mouseX << "-" << mouseY << std::endl;
		if (isLineDraw)
		{
			curLine2.x = mouseX;
			curLine2.y = mouseY;

			if (lineCount % 10 == 9) {
				lines = (myLine*)realloc(lines, (10 + lineCount) * sizeof(myLine));
			}
			myLine ll;
			ll.r = r;
			ll.g = g;
			ll.b = b;
			ll.a = a;
			ll.p1.x = curLine1.x;
			ll.p1.y = curLine1.y;
			ll.p2.x = curLine2.x;
			ll.p2.y = curLine2.y;
			sumV = ll.p1 + ll.p2;
			centerV = sumV / 2;
			ll.size = (float)scaleValue;
			lines[lineCount] = ll;
			lineCount++;
		}
		if (isQuadDraw) {
			curQuad2.x = mouseX;
			curQuad2.y = mouseY;
			if (quadCount % 10 == 9) {
				quads = (myQuad*)realloc(quads, (10 + quadCount) * sizeof(myQuad));
			}

			myQuad qq;
			qq.r = r;
			qq.g = g;
			qq.b = b;
			qq.a = a;
			qq.p1.x = curQuad1.x;
			qq.p1.y = curQuad1.y;
			qq.p2.x = curQuad2.x;
			qq.p2.y = curQuad2.y;
			//Center Test
			sumV = qq.p1 + qq.p2;
			centerV = sumV / 2;
			cout << "Center Vector Test: " << centerV << endl;
			quads[quadCount] = qq;
			quadCount++;

			//Translate Test
			if (translateMode) {
				tx_old += tx;       //현재의 이동값을 이전의 이동값으로 저장
				ty_old += ty;
			}
		}
		if (isTriangleDraw) {
			curTri2.x = mouseX;
			curTri2.y = mouseY;
			if (triangleCount % 10 == 9) {
				triangles = (myTriangle*)realloc(triangles, (10 + triangleCount) * sizeof(myTriangle));
			}

			myTriangle tt;
			tt.r = r;
			tt.g = g;
			tt.b = b;
			tt.a = a;
			tt.p1.x = curTri1.x;
			tt.p1.y = curTri1.y;
			tt.p2.x = curTri2.x;
			tt.p2.y = curTri2.y;
			//Center Test
			sumV = tt.p1 + tt.p2;
			centerV = sumV / 2;
			triangles[triangleCount] = tt;
			triangleCount++;
		}
		if (isEllipseDraw) {
			curEllipse.p2.x = mouseX;
			curEllipse.p2.y = mouseY;
			curEllipse.center.x = (curEllipse.p1.x + curEllipse.p2.x) / 2;
			curEllipse.center.y = (curEllipse.p1.y + curEllipse.p2.y) / 2;
			curEllipse.radius_width = abs(curEllipse.p1.x - curEllipse.p2.x) / 2;
			curEllipse.radius_height = abs(curEllipse.p1.y - curEllipse.p2.y) / 2;

			if (ellipseCount % 10 == 9) {
				ellipses = (myEllipse*)realloc(ellipses, (10 + ellipseCount) * sizeof(myEllipse));
			}

			myEllipse ee;
			ee.r = r;
			ee.g = g;
			ee.b = b;
			ee.a = a;
			ee.center = curEllipse.center;
			ee.radius_width = curEllipse.radius_width;
			ee.radius_height = curEllipse.radius_height;
			ee.p1 = curEllipse.p1;
			ee.p2 = curEllipse.p2;
			//Center Test
			sumV = ee.p1 + ee.p2;
			centerV = sumV / 2;
			ee.size = (float)scaleValue;
			ellipses[ellipseCount] = ee;
			ellipseCount++;

			curEllipse.radius_width = 0;
			curEllipse.radius_height = 0;
		}
	}
	glutPostRedisplay();
}
void mouseDrag(int x, int y) { //Drawing outline when hold the mouse
	std::cout << "mouse move " << drag_mouseX << "-" << drag_mouseY << std::endl;

	drag_mouseX = (GLfloat)x;
	drag_mouseY = -(GLfloat)y + (GLfloat)height; // Set OpenGL Window -> Lowerleft = (0,0), UpperRight = (width,height)

	tx = drag_mouseX - mouseX;            //Mouse dragging diatance
	ty = drag_mouseY - mouseY;
	glMatrixMode(GL_MODELVIEW); //Set-up for the modeling & viewing matrix
	glLoadIdentity();
	if (isPointBrush) {
		savePointBrush(drag_mouseX, drag_mouseY);
	}
	if (isLineDraw) {
		curLine2.x = drag_mouseX;
		curLine2.y = drag_mouseY;
	}
	if (isQuadDraw) {
		curQuad2.x = drag_mouseX;
		curQuad2.y = drag_mouseY;
	}
	if (isTriangleDraw) {
		curTri2.x = drag_mouseX;
		curTri2.y = drag_mouseY;
	}
	if (isEllipseDraw) {
		curEllipse.p2.x = drag_mouseX;
		curEllipse.p2.y = drag_mouseY;
		curEllipse.center.x = (curEllipse.p1.x + curEllipse.p2.x) / 2;
		curEllipse.center.y = (curEllipse.p1.y + curEllipse.p2.y) / 2;
		curEllipse.radius_width = abs(curEllipse.p1.x - curEllipse.p2.x) / 2;
		curEllipse.radius_height = abs(curEllipse.p1.y - curEllipse.p2.y) / 2;
	}
	if (translateMode) {
		glTranslatef(tx, ty, 0.0);            //현재 변화시키는 x,y값만큼 이동
		glTranslatef(tx_old, ty_old, 0.0);
	}    //이전 변화시킨 값을 저장하여 다음 이동이 이 이동 다음에 이루어지도록 이전 이동만큼 translate 시켜준다.

	glutPostRedisplay();
}
void pointsDisplay() {
	for (int i = 0; i < brCount; i++)
	{
		glColor4f(brushes[i].r, brushes[i].g, brushes[i].b, brushes[i].a);
		glPointSize(brushes[i].size);

		glBegin(GL_POINTS);
		glVertex2i(brushes[i].p.x, brushes[i].p.y);
		glEnd();
	}
}
void lineDisplay() {
	glColor4f(r, g, b, a);
	glLineWidth(scaleValue);
	glBegin(GL_LINES);
	glVertex2i(curLine1.x, curLine1.y);
	glVertex2i(curLine2.x, curLine2.y);
	glEnd();

	for (int i = 0; i < lineCount; i++)
	{
		glPushMatrix();
		glColor4f(lines[i].r, lines[i].g, lines[i].b, lines[i].a);
		glLineWidth(lines[i].size);
		glBegin(GL_LINES);
		glVertex2i(lines[i].p1.x, lines[i].p1.y);
		glVertex2i(lines[i].p2.x, lines[i].p2.y);
		glEnd();
		glPopMatrix();
	}
}
void quadDisplay() {
	glColor4f(r, g, b, a);
	glBegin(GL_QUADS);
	glVertex2i(curQuad1.x, curQuad1.y);
	glVertex2i(curQuad1.x, curQuad2.y);
	glVertex2i(curQuad2.x, curQuad2.y);
	glVertex2i(curQuad2.x, curQuad1.y);
	glEnd();

	for (int i = 0; i < quadCount; i++)
	{
		glColor4f(quads[i].r, quads[i].g, quads[i].b, quads[i].a);

		glBegin(GL_QUADS);
		glVertex2i(quads[i].p1.x, quads[i].p1.y);
		glVertex2i(quads[i].p1.x, quads[i].p2.y);
		glVertex2i(quads[i].p2.x, quads[i].p2.y);
		glVertex2i(quads[i].p2.x, quads[i].p1.y);
		glEnd();
	}
}
void triangleDisplay() {
	glColor4f(r, g, b, a);
	glBegin(GL_TRIANGLES);
	if (curTri2.y > curTri1.y) {
		glVertex2i(curTri1.x, curTri1.y);
		glVertex2i(curTri2.x, curTri1.y);
		glVertex2i((curTri2.x + curTri1.x) / 2, curTri2.y);
	}
	else {
		glVertex2i(curTri2.x, curTri2.y);
		glVertex2i(curTri1.x, curTri2.y);
		glVertex2i((curTri2.x + curTri1.x) / 2, curTri1.y);
	}
	glEnd();

	for (int i = 0; i < triangleCount; i++)
	{
		glColor4f(triangles[i].r, triangles[i].g, triangles[i].b, triangles[i].a);
		glBegin(GL_TRIANGLES);
		if (triangles[i].p2.y > triangles[i].p1.y) {
			glVertex2i(triangles[i].p1.x, triangles[i].p1.y);
			glVertex2i(triangles[i].p2.x, triangles[i].p1.y);
			glVertex2i((triangles[i].p2.x + triangles[i].p1.x) / 2, triangles[i].p2.y);
		}
		else {
			glVertex2i(triangles[i].p2.x, triangles[i].p2.y);
			glVertex2i(triangles[i].p1.x, triangles[i].p2.y);
			glVertex2i((triangles[i].p2.x + triangles[i].p1.x) / 2, triangles[i].p1.y);
		}
		glEnd();
	}
}
void ellipseDisplay() {
	//ellipse
	float dx, dy;
	glColor4f(r, g, b, a);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 36; i++) {
		float angle = radians(10.0*i);
		dx = curEllipse.radius_width * cos(angle);
		dy = curEllipse.radius_height * sin(angle);
		glVertex2f(curEllipse.center.x + dx, curEllipse.center.y + dy);
	}
	glEnd();
	for (int i = 0; i < ellipseCount; i++)
	{
		myEllipse ee = ellipses[i];
		glLineWidth(ee.size);
		glColor4f(ee.r, ee.g, ee.b, ee.a);
		glBegin(GL_POLYGON);
		for (int i = 0; i < 36; i++) {
			float angle = radians(10.0*i);
			dx = ee.radius_width * cos(angle);
			dy = ee.radius_height * sin(angle);
			glVertex2f(ee.center.x + dx, ee.center.y + dy);
		}
		glEnd();
	}
}
void RenderString(GLdouble x, GLdouble y, const std::string &string)
{
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2d(x, y);
	for (int n = 0; n < string.size(); ++n) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[n]);
	}
}
void uiDisplay() {
	int numofInfo = 8;
	int unit = width / numofInfo;
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	glVertex2i(0, 0);
	glVertex2i(width, 0);
	glVertex2i(width, 50);
	glVertex2i(0, 50);
	glEnd();

	glLineWidth(5);
	glColor3f(0, 0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glVertex2i(1, 1);
	glVertex2i(width - 1, 1);
	glVertex2i(width, 50);
	glVertex2i(0, 50);
	glEnd();

	glBegin(GL_LINES);
	for (int i = 1; i < numofInfo; i++) {
		glVertex2i(unit*i, 50);
		glVertex2i(unit*i, 0);
	}
	glEnd();
	//Point UI
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex2i(unit / 2, 25);
	glEnd();

	//Line UI
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex2i(unit*1.2f, 10);
	glVertex2i(unit*1.8f, 40);
	glEnd();

	//Triangle UI
	glBegin(GL_TRIANGLES);
	glVertex2i(unit*2.2f, 10);
	glVertex2i(unit*2.8f, 10);
	glVertex2i(unit*2.5f, 40);
	glEnd();

	//Quad UI
	glBegin(GL_QUADS);
	glVertex2i(unit*3.2f, 10);
	glVertex2i(unit*3.8f, 10);
	glVertex2i(unit*3.8f, 40);
	glVertex2i(unit*3.2f, 40);
	glEnd();

	//Ellipse UI
	float dx, dy;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 36; i++) {
		float angle = radians(10.0*i);
		dx = (unit / 3) * cos(angle);
		dy = (unit / 6) * sin(angle);
		glVertex2f(unit*4.5 + dx, 25 + dy);
	}
	glEnd();

	RenderString(unit*5.2f, 30, "SCALE");
	RenderString(unit*5.4f, 10, std::to_string(scaleValue));
	//Selected state quad
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor4f(r, g, b, 0.5f);
	glBegin(GL_QUADS);
	glVertex2i(selectX * unit, 1);
	glVertex2i(selectX * unit + unit, 1);
	glVertex2i(selectX * unit + unit, 50);
	glVertex2i(selectX * unit, 50);
	glEnd();
}
void init()
{
	glClearColor(0.8, 0.0, 0.8, 0.9); // background

	// Set your projection matrix
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, width, 0, height, -1, 1);
	// Restore the default matrix mode
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void display()
{
	glClearColor(0.9, 0.9, 0.9, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	uiDisplay();
	quadDisplay();
	ellipseDisplay();
	lineDisplay();
	pointsDisplay(); // brush
	triangleDisplay();

	//CenterPoint Test
	glPointSize(10);
	glBegin(GL_POINTS);
	glColor3f(0.9, 0.2, 0.2);
	glVertex2f(centerV.x, centerV.y);
	glEnd();

	glutSwapBuffers();
}
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	glutCreateWindow("SimpleDrawingTool");
	glewInit();
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseDrag);

	brushes = (myBrush*)malloc(100 * sizeof(myBrush));
	lines = (myLine*)malloc(10 * sizeof(myLine));
	quads = (myQuad*)malloc(10 * sizeof(myQuad));
	triangles = (myTriangle*)malloc(10 * sizeof(myTriangle));
	ellipses = (myEllipse*)malloc(10 * sizeof(myEllipse));

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardDown);
	init();
	glutMainLoop();
	free(brushes);
	free(lines);
	free(quads);
	free(triangles);
	free(ellipses);
	return 0;
}