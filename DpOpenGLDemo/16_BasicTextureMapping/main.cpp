#include <stdio.h>
#include <assert.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogl_pipeline.h"
#include "ogl_camera.h"
#include "ogl_texture.h"
using namespace ogl;

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "DpOpenGL.lib")
#pragma comment(lib, "DpLib.lib")

#define WINDOW_WIDTH	1024
#define WINDOW_HEIGHT	768

PersProjInfo gPersProjInfo;
Camera* gCamera = nullptr;
Texture* gTexture = nullptr;

GLuint VBO;
GLuint gWVPLocation;
GLuint gSampler;

const char* vs = "#version 330\n"
"layout(location = 0) in vec3 Position;\n"
"layout(location = 1) in vec2 TexCoord;\n"
"uniform mat4 gWVP;"
"out vec2 TexCoord0;"
"void main()"
"{"
"gl_Position = gWVP * vec4(Position, 1.0);"
"TexCoord0 = TexCoord;"
"}";

const char* ps = "#version 330\n"
"in vec2 TexCoord0;"
"uniform sampler2D gSampler;"
"void main()"
"{"
"gl_FragColor = texture2D(gSampler, TexCoord0.xy);"
"}";

struct Vertex
{
	Vector3f pos;
	Vector2f tex;
};

void RenderScene()
{
	gCamera->OnRender();
	glClear(GL_COLOR_BUFFER_BIT);

	static float scale = 0.0f;
	scale += 0.3f;

	Pipeline p;
	p.Rotate(0.0f, angle2radian(scale), 0);
	p.WorldPos(0.0f, 0.0f, 3.0f);
	p.SetPerspectiveProj(gPersProjInfo);
	p.SetCamera(*gCamera);

	glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, &(p.GetOGLWorldViewProjTrans().m[0][0]));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
	
	gTexture->Bind(GL_TEXTURE0);
	glDrawArrays(GL_TRIANGLES, 0, 12);

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

	gWVPLocation = glGetUniformLocation(shaderProgram, "gWVP");
	assert(gWVPLocation != 0xffffffff);

	gSampler = glGetUniformLocation(shaderProgram, "gSampler");
	assert(gSampler != 0xffffffff);
}

void CreateVertexBuffer()
{
	Vertex vertices[12] = {
		// 1
		{ Vector3f(0.0f, 1.0f, 0.0f), Vector2f(0.5f, 1.0f) },
		{ Vector3f(-1.0f, -1.0f, 0.5773f), Vector2f(1.0f, 0.0f) },
		{ Vector3f(1.0f, -1.0f, 0.5773f), Vector2f(0.0f, 0.0f) },
		// 2
		{ Vector3f(0.0f, 1.0f, 0.0f), Vector2f(0.5f, 1.0f) },
		{ Vector3f(0.0f, -1.0f, -1.15475f), Vector2f(1.0f, 0.0f) },
		{ Vector3f(-1.0f, -1.0f, 0.5773f), Vector2f(0.0f, 0.0f) },
		// 3
		{ Vector3f(0.0f, 1.0f, 0.0f), Vector2f(0.5f, 1.0f) },
		{ Vector3f(1.0f, -1.0f, 0.5773f), Vector2f(1.0f, 0.0f) },
		{ Vector3f(0.0f, -1.0f, -1.15475f), Vector2f(0.0f, 0.0f) },
		// 4
		{ Vector3f(-1.0f, -1.0f, 0.5773f), Vector2f(0.5f, 1.0f) },
		{ Vector3f(0.0f, -1.0f, -1.15475f), Vector2f(1.0f, 0.0f) },
		{ Vector3f(1.0f, -1.0f, 0.5773f), Vector2f(0.0f, 0.0f) }
	};
	
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void SpecialKeyboardCB(int key, int x, int y)
{
	gCamera->OnKayboard(key);
}

void KeyboardCB(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
		glutLeaveMainLoop();
		break;
	default:
		break;
	}
}

void PassiveMouseCB(int x, int y)
{
	gCamera->OnMouse(x, y);
}

void InitGlutCallbacks()
{
	glutIdleFunc(RenderScene);
	glutDisplayFunc(RenderScene);
	glutSpecialFunc(SpecialKeyboardCB);
	glutKeyboardFunc(KeyboardCB);
	glutPassiveMotionFunc(PassiveMouseCB);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("tutorial 16");

	InitGlutCallbacks();

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "error:%s\n", glewGetErrorString(res));
		return 1;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	CreateVertexBuffer();

	CompileShaders();

	glUniform1i(gSampler, 0);

	gTexture = new Texture(GL_TEXTURE_2D, "checkerboard.png");
	gTexture->Load();
	gCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);

	gPersProjInfo.FOV = 60.0f;
	gPersProjInfo.Width = WINDOW_WIDTH;
	gPersProjInfo.Height = WINDOW_HEIGHT;
	gPersProjInfo.zNear = 1.0f;
	gPersProjInfo.zFar = 100.0f;

	glutMainLoop();

	delete gCamera;
	delete gTexture;
	return 0;
}