#include <stdio.h>
#include <assert.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogl_app.h"
#include "ogl_glut.h"
#include "ogl_pipeline.h"
#include "ogl_camera.h"
#include "ogl_texture.h"
using namespace ogl;

#include "lighting_technique.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "DpOpenGL.lib")
#pragma comment(lib, "DpLib.lib")

#define WINDOW_WIDTH	1024
#define WINDOW_HEIGHT	768

struct Vertex
{
	Vector3f pos;
	Vector2f tex;
};

class AmbientLighting : public ICallbacks, public ogl::App
{
public:
	AmbientLighting();
	~AmbientLighting();

	bool Init();

	void Run();

	virtual void RenderSceneCB();

	virtual void KeyboardCB(int key);

	virtual void PassiveMouseCB(int x, int y);
private:
	void CreateVertexBuffer();

	PersProjInfo perProjInfo_;
	Camera* camera_;
	Texture* texture_;
	float scale_;

	DirectionalLight directionalLight_;
	LightingTechnique* effect_;

	GLuint VBO_;
};

AmbientLighting::AmbientLighting()
	: texture_(nullptr)
	, camera_(nullptr)
	, effect_(nullptr)
	, VBO_(0)
	, scale_(0.0f)
{
	perProjInfo_.FOV = 60.0f;
	perProjInfo_.Width = WINDOW_WIDTH;
	perProjInfo_.Height = WINDOW_HEIGHT;
	perProjInfo_.zNear = 1.0f;
	perProjInfo_.zFar = 100.0f;

	directionalLight_.color = Vector3f(1.0f, 1.0f, 1.0f);
	directionalLight_.ambientIntensity = 0.5f;
}

AmbientLighting::~AmbientLighting()
{
	delete camera_;
	delete texture_;
	delete effect_;
}

bool AmbientLighting::Init()
{
	camera_ = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);

	CreateVertexBuffer();

	texture_ = new Texture(GL_TEXTURE_2D, "checkerboard.png");
	if (!texture_->Load())
		return false;

	effect_ = new LightingTechnique();
	if (!effect_->Init())
		return false;
	effect_->Enable();
	effect_->SetTextureUnit(0);

	return true;
}

void AmbientLighting::CreateVertexBuffer()
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

	glGenBuffers(1, &VBO_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void AmbientLighting::Run()
{
	GLUTBackendRun(this);
}

void AmbientLighting::RenderSceneCB()
{
	camera_->OnRender();

	glClear(GL_COLOR_BUFFER_BIT);

	scale_ += 0.1f;

	Pipeline p;
	p.Rotate(0.0f, angle2radian(scale_), 0.0f);
	p.WorldPos(0.0f, 0.0f, 3.0f);
	p.SetCamera(*camera_);
	p.SetPerspectiveProj(perProjInfo_);

	effect_->SetWVP(p.GetOGLWorldViewProjTrans());
	effect_->SetDirectionalLight(directionalLight_);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);

	texture_->Bind(GL_TEXTURE0);
	glDrawArrays(GL_TRIANGLES, 0, 12);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glutSwapBuffers();

}

void AmbientLighting::PassiveMouseCB(int x, int y)
{
	camera_->OnMouse(x, y);
}

void AmbientLighting::KeyboardCB(int key)
{
	switch (key)
	{
	case 'q':
		GLUTBackendLeaveMainLoop();
		break;
	case 'a':
		directionalLight_.ambientIntensity += 0.05f;
		if (directionalLight_.ambientIntensity > 1.0f)
			directionalLight_.ambientIntensity = 1.0f;
		break;
	case 's':
		directionalLight_.ambientIntensity -= 0.05f;
		if (directionalLight_.ambientIntensity < 0.0f)
			directionalLight_.ambientIntensity = 0.0f;
		break;
	default:
		break;
	}
}

int main(int argc, char** argv)
{
	GLUTBackendInit(argc, argv, false, false);
	
	if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false, "tutorial 17"))
		return 1;

	AmbientLighting* app = new AmbientLighting();
	if (!app->Init())
		return 1;

	app->Run();

	delete app;

	return 0;
}