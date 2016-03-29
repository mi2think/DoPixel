#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogl_app.h"
#include "ogl_glut.h"
#include "ogl_pipeline.h"
#include "ogl_camera.h"
#include "ogl_texture.h"
#include "ogl_mesh.h"
#include "ogl_basic_lighting.h"
#include "ogl_skybox.h"

using namespace ogl;

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "DpOpenGL.lib")
#pragma comment(lib, "DpLib.lib")
#pragma comment(lib, "assimp.lib")

#define WINDOW_WIDTH	1920
#define WINDOW_HEIGHT	1200

class SkyboxApp : public ICallbacks, public App
{
public:
	SkyboxApp();
	~SkyboxApp();

	bool Init();

	void Run();

	virtual void RenderSceneCB() override;

	virtual void PassiveMouseCB(int x, int y) override;
private:
	BasicLightingTechnique* lightingTechnique_;
	Camera* camera_;
	DirectionalLight dirLight_;
	Mesh* mesh_;
	Skybox* skybox_;
	PersProjInfo persProjInfo_;
	float scale_;
};

SkyboxApp::SkyboxApp()
	: lightingTechnique_(nullptr)
	, camera_(nullptr)
	, mesh_(nullptr)
	, skybox_(nullptr)
	, scale_(0.0f)
{
	dirLight_.ambientIntensity_ = 0.2f;
	dirLight_.diffuseIntensity_ = 0.8f;
	dirLight_.color_ = Vector3f(1.0f, 1.0f, 1.0f);
	dirLight_.direction_ = Vector3f(1.0f, -1.0f, 1.0f);

	persProjInfo_.FOV = 60.0f;
	persProjInfo_.Height = WINDOW_HEIGHT;
	persProjInfo_.Width = WINDOW_WIDTH;
	persProjInfo_.zNear = 1.0f;
	persProjInfo_.zFar = 100.0f;
}

SkyboxApp::~SkyboxApp()
{
	SAFE_DELETE(lightingTechnique_);
	SAFE_DELETE(camera_);
	SAFE_DELETE(mesh_);
	SAFE_DELETE(skybox_);
}

bool SkyboxApp::Init()
{
	Vector3f pos(0.0f, 1.0f, -20.0f);
	Vector3f target(0.0f, 0.0f, 1.0f);
	Vector3f up(0.0f, 1.0f, 0.0f);

	camera_ = new Camera(pos, target, up, WINDOW_WIDTH, WINDOW_HEIGHT);

	lightingTechnique_ = new BasicLightingTechnique();
	if (!lightingTechnique_->Init())
	{
		fprintf(stderr, "error init lighting technique\n");
		return false;
	}
	lightingTechnique_->Enable();
	lightingTechnique_->SetDirectionalLight(dirLight_);
	lightingTechnique_->SetTextureUnit(0);

	mesh_ = new Mesh();
	if (!mesh_->LoadMesh("../Resource/phoenix_ugv.md2"))
		return false;

	skybox_ = new Skybox(camera_, persProjInfo_);
	if (!skybox_->Init("../Resource/sp3right.jpg",
		"../Resource/sp3left.jpg",
		"../Resource/sp3top.jpg",
		"../Resource/sp3bot.jpg",
		"../Resource/sp3front.jpg",
		"../Resource/sp3back.jpg",
		"../Resource/sphere.obj"))
	{
		return false;
	}

	return true;
}

void SkyboxApp::Run()
{
	GLUTBackendRun(this);
}

void SkyboxApp::RenderSceneCB()
{
	camera_->OnRender();
	scale_ += 0.05f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	lightingTechnique_->Enable();

	Pipeline p;
	p.Scale(0.1f, 0.1f, 0.1f);
	p.Rotate(0.0f, angle2radian(scale_), 0.0f);
	p.WorldPos(0.0f, -5.0f, 3.0f);
	p.SetCamera(*camera_);
	p.SetPerspectiveProj(persProjInfo_);

	lightingTechnique_->SetWVP(p.GetOGLWorldViewProjTrans());
	lightingTechnique_->SetWorldMatrix(p.GetOGLWorldTrans());
	mesh_->Render();

	skybox_->Render();

	glutSwapBuffers();
}

void SkyboxApp::PassiveMouseCB(int x, int y)
{
	camera_->OnMouse(x, y);
}

int main(int argc, char** argv)
{
	GLUTBackendInit(argc, argv, true, false);

	if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false, "tutorial 25"))
		return 1;

	SkyboxApp* app = new SkyboxApp();
	if (!app->Init())
		return 1;

	app->Run();

	delete app;

	return 0;
}
