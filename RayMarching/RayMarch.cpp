#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <vector>
#include "../mat.h"
using namespace std;
//-------Function & Global Variable Declaration------------//
void startup();
void render();
void shutdown();
extern void version();
extern GLuint compile_shaders(const char*  vsource, const char*  fsource);
float cubeAngle = 0.0f;
float s_time = 0.0;
float u_time = 0.0;
float t = 0;
struct scene {
	int width = 900, height = 640;
	vec3 camerapos;
	mat4 look;
	float angle = 0;
	float scaleFactor = 1.0;
	GLuint rendering_program;
};
scene sc;
GLuint vao, vbo, ebo;
vector<vec2> vertices;
vector<GLuint> indices;
//--------------------------------------//
void startup()
{
	/******* OpenGL Initialization */
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); //Passes if the incoming depth value is less than or
	/**** Shaders as variables */
	sc.rendering_program = compile_shaders("RayMarching/rayMarch_v.glsl", "RayMarching/rayMarch_f1.glsl");
	//sc.rendering_program = compile_shaders("RayMarching/rayMarch_v.glsl", "RayMarching/rayMarch_f2.glsl");
	//sc.rendering_program = compile_shaders("RayMarching/rayMarch_v.glsl", "RayMarching/rayMarch_f3.glsl");
	//sc.rendering_program = compile_shaders("RayMarching/rayMarch_v.glsl", "RayMarching/rayMarch_f4.glsl");
	glUseProgram(sc.rendering_program);
#pragma region Vao Vbo
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	unsigned int offset = (unsigned int)vertices.size();

	vertices.push_back({ -1.0f, -1.0f });
	vertices.push_back({ 1.0f, -1.0f });
	vertices.push_back({ -1.0f, 1.0f });
	vertices.push_back({ 1.0f, 1.0f });

	indices.push_back(offset + 0);
	indices.push_back(offset + 1);
	indices.push_back(offset + 2);
	indices.push_back(offset + 2);
	indices.push_back(offset + 1);
	indices.push_back(offset + 3);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (GLvoid*)0);

	glBindVertexArray(0);
#pragma endregion

	glClearColor(0.2, 0.2, 0.2, 1.0);
	// modeling
	// viewing
	//Uniform Resolution
	glUniform2f(glGetUniformLocation(sc.rendering_program, "resolution"), sc.width, sc.height);
	glUniform2f(glGetUniformLocation(sc.rendering_program, "mouse"), 0, 0);
	glUniform2f(glGetUniformLocation(sc.rendering_program, "screenRatio"), (float)sc.width, (float)sc.height);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "time"), u_time);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "s_time"), s_time);
}
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(sc.rendering_program);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "time"), u_time);
	glUniform1f(glGetUniformLocation(sc.rendering_program, "s_time"), s_time);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glutSwapBuffers();
}

void idle() {
	u_time += 0.01;
	t += 0.02f;
	s_time = 5.0 * sin(t);
	cubeAngle += 0.0f;
	glutPostRedisplay();
}

void shutdown()
{
	glDeleteProgram(sc.rendering_program);
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	GLfloat left = -1.0, right = 1.0, bottom = -.2, top = 1.8;
	GLfloat aspect = (GLfloat)w / h;

	if (aspect <= 1.0) {
		bottom /= aspect;
		top /= aspect;
	}
	else {
		left *= aspect;
		right *= aspect;
	}

	sc.width = w; sc.height = h;
	glUniform2f(glGetUniformLocation(sc.rendering_program, "screenRatio"), (float)sc.width, (float)sc.height);
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(sc.width, sc.height);
	glutCreateWindow("RayMarching_DistanceFunction");
	glewInit();

	version();
	startup();

	glutDisplayFunc(render);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutMainLoop();
	shutdown();

	system("pause");
	return 0;
}