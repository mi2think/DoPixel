#include <stdio.h>
#include <assert.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "DpVector4.h"
#include "DpMatrix44.h"
using namespace dopixel::math;

#pragma comment(lib, "glew32.lib")

GLuint VBO;
GLuint gMatWorldLocation;

const char* vs = "#version 330\n"
"layout(location = 0) in vec4 Position;\n"
"uniform mat4 gMatWorld;"
"void main()"
"{"
"gl_Position = gMatWorld * Position;"
"}";

const char* ps = "#version 330\n"
"void main()"
"{"
"gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);"
"}";


void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);
	static float scale = 0.0f;
	scale += 0.001f;

	// built mat, note, opengl matrix is col major
	float sinScale = sinf(scale);

	Matrix44f mat;
	mat.m11 = sinScale; mat.m12 = 0; mat.m13 = 0; mat.m14 = 0;
	mat.m21 = 0; mat.m22 = sinScale; mat.m23 = 0; mat.m24 = 0;
	mat.m31 = 0; mat.m32 = 0; mat.m33 = sinScale; mat.m34 = 0;
	mat.m41 = 0; mat.m42 = 0; mat.m43 = 0; mat.m44 = 1;

	glUniformMatrix4fv(gMatWorldLocation, 1, GL_TRUE, &mat.m11);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(00);

	glutSwapBuffers();
}

void CreateVertexBuffer()
{
	Vector4f vertices[3] = { Vector4f{ -1.0f, -1.0f, 0.0f, 1.0f }, Vector4f{ 0.0f, 1.0f, 0.0f, 1.0f }, Vector4f{ 1.0f, -1.0f, 0.0f, 1.0f } };

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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
	GLuint shaderProgram = glCreateProgram();
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

	gMatWorldLocation = glGetUniformLocation(shaderProgram, "gMatWorld");
	assert(gMatWorldLocation != 0xffffffff);
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("tutorial 08");

	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "error:%s\n", glewGetErrorString(res));
		return 1;
	}

	CreateVertexBuffer();

	CompileShaders();

	glutMainLoop();

	return 0;
}