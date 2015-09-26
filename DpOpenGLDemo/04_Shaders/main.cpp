#include <stdio.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "DpVector3.h"
using namespace dopixel::math;

#pragma comment(lib, "glew32.lib")


GLuint VBO;

const char* vs = "#version 330\n"
"layout (location = 0) in vec3 Position;\n"
"void main()"
"{"
"gl_Position = vec4(0.5 * Position.x, 0.5 * Position.y, Position.z, 1.0);"
"}";

const char* ps = "#version 330\n"
"void main()"
"{"
"gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);"
"}";


void CreateVertexBuffer()
{
	Vector3f vertices[3] = { Vector3f{ -1.0f, -1.0f, 0.0f }, Vector3f{ 0.0f, 1.0f, 0.0f }, Vector3f{ 1.0f, -1.0f, 0.0f } };

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}


void AddShader(GLint shaderProgram, const char* strShader, GLenum shaderType)
{
	GLuint shaderObj = glCreateShader(shaderType);
	if (shaderObj == 0)
	{
		fprintf(stderr, "error create shader type\n");
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
		fprintf(stderr, "error compile shader: \n%s\n", infoLog);
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
	if (success == 0)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), nullptr, errorLog);
		fprintf(stderr, "error linking program: \n%s\n", errorLog);
		exit(1);
	}

	glValidateProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);
	if (success == 0)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(errorLog), nullptr, errorLog);
		fprintf(stderr, "error invalidate program: \n%s\n", errorLog);
		exit(1);
	}

	glUseProgram(shaderProgram);
}


void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);

	glutSwapBuffers();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("tutorial 04");
	glutDisplayFunc(RenderScene);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error:%s\n", glewGetErrorString(res));
		return 1;
	}

	printf("GL Version:%s\n", glGetString(GL_VERSION));

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	CreateVertexBuffer();

	CompileShaders();

	glutMainLoop();

	return 0;
}
