#include <stdio.h>
#include <assert.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "DpVector4.h"
#include "DpMatrix44.h"
using namespace dopixel::math;

#pragma comment(lib, "glew32.lib")

GLuint VAO;
GLuint VBO[2];
GLuint IBO;
GLuint gWorldLocation;

const char* vs = "#version 330\n"
"layout(location = 0) in vec4 Position;\n"
"layout(location = 1) in vec4 Color;\n"
"uniform mat4 gWorld;"
"out vec4 OutColor;"
"void main()"
"{"
"gl_Position = gWorld * Position;"
"OutColor = Color;"
"}";

const char* ps = "#version 330\n"
"in vec4 OutColor;"
"void main()"
"{"
"gl_FragColor = OutColor;"
"}";

void RenderScene()
{
	static float scale = 0.0f;
	scale += 0.001f;

	// built mat, note, opengl matrix is col major
	float sinScale = sinf(scale);
	float cosScale = cosf(scale);

	Matrix44f mat;
	mat.m11 = cosScale; mat.m12 = 0; mat.m13 = -sinScale;	mat.m14 = 0;
	mat.m21 = 0;		mat.m22 = 1; mat.m23 = 0;			mat.m24 = 0;
	mat.m31 = sinScale; mat.m32 = 0; mat.m33 = cosScale;	mat.m34 = 0;
	mat.m41 = 0;		mat.m42 = 0; mat.m43 = 0;			mat.m44 = 1;

	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &mat.m11);

	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glutSwapBuffers();
}

void AddShader(GLuint shaderProgram, const char* strShader, GLenum shaderType)
{
	GLuint shaderObj = glCreateShader(shaderType);
	if (shaderObj == 0)
	{
		fprintf(stderr, "error create shader\n");
		exit(1);
	}

	const GLchar* p[1] = { strShader };
	GLint lengths[1] = { (int)strlen(strShader) };

	glShaderSource(shaderObj, 1, p, lengths);
	glCompileShader(shaderObj);
	GLint success;
	glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[1024];
		glGetShaderInfoLog(shaderObj, sizeof(infoLog), nullptr, infoLog);
		fprintf(stderr, "error compile shader:\n%s\n", infoLog);
		exit(1);
	}

	glAttachShader(shaderProgram, shaderObj);
}

void CompileShaders()
{
	GLint shaderProgram = glCreateProgram();
	if (shaderProgram == 0)
	{
		fprintf(stderr, "error create shader program\n");
		exit(1);
	}

	AddShader(shaderProgram, vs, GL_VERTEX_SHADER);
	AddShader(shaderProgram, ps, GL_FRAGMENT_SHADER);

	GLint success = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), nullptr, errorLog);
		fprintf(stderr, "error link program:\n%s\n", errorLog);
		exit(1);
	}

	glUseProgram(shaderProgram);

	GLuint gWorldLocation = glGetUniformLocation(shaderProgram, "gWorld");
	assert(gWorldLocation != 0xffffffff);
}

void CreateVertexBuffer()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	Vector4f positions[4] = { { -1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, -1.0f, 1.0f, 1.0f }, { 1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } };
	Vector4f colors[4] = { {1.0, 0.0, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 0.0, 1.0, 1.0}, { 1.0, 1.0, 1.0, 1.0 } };

	glGenBuffers(2, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
}

void CreateIndexBuffer()
{
	unsigned int indices[] = { 0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2 };

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("tutorial 10");

	glutIdleFunc(RenderScene);
	glutDisplayFunc(RenderScene);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "error:%s\n", glewGetErrorString(res));
		return 1;
	}

	CreateVertexBuffer();

	CreateIndexBuffer();

	CompileShaders();

	glutMainLoop();

	return 0;
}
