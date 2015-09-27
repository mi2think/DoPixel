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

const char* vs = "#version 330\n"
"layout(location = 0) in vec4 Positon;\n"
"layout(location = 1) in vec4 Color;\n"
"out vec4 OutColor;"
"void main()"
"{"
"gl_Position = Positon;"
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
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glutSwapBuffers();
}

void CreateVertexBuffer()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	Vector4f positions[3] = { Vector4f{ -1.0f, -1.0f, 0.0f, 1.0f }, Vector4f{ 0.0f, 1.0f, 0.0f, 1.0f }, Vector4f{ 1.0f, -1.0f, 0.0f, 1.0f } };
	Vector4f colors[3] = { Vector4f(1.0, 0.0, 0.0, 1.0), Vector4f(0.0, 1.0, 0.0, 1.0), Vector4f(0.0, 0.0, 1.0, 1.0) };

	glGenBuffers(2, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
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