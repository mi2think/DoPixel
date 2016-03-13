#include <stdio.h>
#include <assert.h>

#include <vector>
#include <map>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogl_app.h"
#include "ogl_glut.h"
#include "ogl_pipeline.h"
#include "ogl_camera.h"
#include "ogl_mesh.h"
#include "ogl_basic_lighting.h"
using namespace ogl;

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "DpOpenGL.lib")
#pragma comment(lib, "DpLib.lib")
#pragma comment(lib, "assimp.lib")

#define WINDOW_WIDTH	1920
#define WINDOW_HEIGHT	1200

#define FILED_DEPTH		10.0f

class AssimpLibApp : public ICallbacks, public App
{
public:
	AssimpLibApp();
	~AssimpLibApp();

	bool Init();

	void Run();

	virtual void PassiveMouseCB(int x, int y) override;

	virtual void KeyboardCB(int key) override;

	virtual void RenderSceneCB() override;
private:
	DirectionalLight directionalLight_;
	BasicLightingTechnique* effect_;
	Mesh* mesh_;
	Camera* camera_;
	PersProjInfo persProjInfo_;
	float scale_;
};

AssimpLibApp::AssimpLibApp()
	: effect_(nullptr)
	, mesh_(nullptr)
	, camera_(nullptr)
	, scale_(0.0f)
{
	directionalLight_.color_ = Vector3f(1.0f, 1.0f, 1.0);
	directionalLight_.ambientIntensity_ = 1.0f;
	directionalLight_.diffuseIntensity_ = 0.01f;
	directionalLight_.direction_ = Vector3f(1.0f, -1.0f, 0.0f);

	persProjInfo_.FOV = 60.0f;
	persProjInfo_.Width = WINDOW_WIDTH;
	persProjInfo_.Height = WINDOW_HEIGHT;
	persProjInfo_.zNear = 1.0f;
	persProjInfo_.zFar = 50.0f;
}

AssimpLibApp::~AssimpLibApp()
{
	delete effect_;
	delete mesh_;
	delete camera_;
}

bool AssimpLibApp::Init()
{
	Vector3f pos(3.0f, 7.0f, -10.0f);
	Vector3f target(0.0f, -0.2f, 1.0f);
	Vector3f up(0.0f, 1.0f, 0.0f);

	camera_ = new Camera(pos, target, up, WINDOW_WIDTH, WINDOW_HEIGHT);

	effect_ = new BasicLightingTechnique();
	if (!effect_->Init())
	{
		fprintf(stderr, "error init lighting technique\n");
		return false;
	}
	effect_->Enable();
	effect_->SetTextureUnit(0);

	mesh_ = new Mesh();
	return mesh_->LoadMesh("../Resource/phoenix_ugv.md2");
}

void AssimpLibApp::Run()
{
	GLUTBackendRun(this);
}

void AssimpLibApp::PassiveMouseCB(int x, int y)
{
	camera_->OnMouse(x, y);
}

void AssimpLibApp::KeyboardCB(int key)
{
	camera_->OnKayboard(key);
}

void AssimpLibApp::RenderSceneCB()
{
	scale_ += 0.01f;
	camera_->OnRender();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	PointLight pl[2];
	pl[0].ambientIntensity_ = 0.0f;
	pl[0].diffuseIntensity_ = 0.25f;
	pl[0].color_ = Vector3f(1.0f, 0.5f, 0.0f);
	pl[0].position_ = Vector3f(3.0f, 1.0f, FILED_DEPTH * (cosf(scale_) + 1.0f) / 2.0f);
	pl[0].attenuation_.kc = 1.0f;
	pl[0].attenuation_.kl = 0.1f;
	pl[0].attenuation_.kq = 0.0f;

	pl[1].ambientIntensity_ = 0.0f;
	pl[1].diffuseIntensity_ = 0.25f;
	pl[1].color_ = Vector3f(0.0f, 0.5f, 1.0f);
	pl[1].position_ = Vector3f(7.0f, 1.0f, FILED_DEPTH * (sinf(scale_) + 1.0f) / 2.0f);
	pl[1].attenuation_.kc = 1.0f;
	pl[1].attenuation_.kl = 0.1f;
	pl[1].attenuation_.kq = 0.0f;

	effect_->SetPointLights(2, pl);

	SpotLight sl[1];
	sl[0].ambientIntensity_ = 0.0f;
	sl[0].diffuseIntensity_ = 0.9f;
	sl[0].color_ = Vector3f(0.0f, 1.0f, 1.0f);
	sl[0].position_ = camera_->GetPosition();
	sl[0].direction_ = camera_->GetTarget();
	sl[0].attenuation_.kc = 1.0f;
	sl[0].attenuation_.kl = 0.1f;
	sl[0].attenuation_.kq = 0.0f;
	sl[0].pf_ = 1.0f;
	sl[0].theta_ = angle2radian(10);
	sl[0].phi_ = angle2radian(30);

	effect_->SetSpotLights(1, sl);

	Pipeline p;
	p.Scale(0.1f, 0.1f, 0.1f);
	p.Rotate(0.0f, angle2radian(scale_), 0.0f);
	p.WorldPos(0.0f, 0.0f, 10.0f);
	p.SetCamera(*camera_);
	p.SetPerspectiveProj(persProjInfo_);

	effect_->SetWVP(p.GetOGLWorldViewProjTrans());
	effect_->SetWorldMatrix(p.GetOGLWorldTrans());
	effect_->SetDirectionalLight(directionalLight_);
	effect_->SetEyeWorldPos(camera_->GetPosition());
	effect_->SetSpecularIntensity(0.0f);
	effect_->SetSpecularPower(0.0f);

	mesh_->Render();

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	GLUTBackendInit(argc, argv, true, false);

	if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false, "tutorial 22"))
		return 1;

	AssimpLibApp* app = new AssimpLibApp();
	if (!app->Init())
		return 1;

	app->Run();

	delete app;

	return 0;
}
