#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ogl_app.h"
#include "ogl_glut.h"
#include "ogl_pipeline.h"
#include "ogl_camera.h"
#include "ogl_basic_lighting.h"
#include "ogl_mesh.h"

#include "shadow_map_fbo.h"
#include "shadow_map_technique.h"

using namespace ogl;

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "DpOpenGL.lib")
#pragma comment(lib, "DpLib.lib")
#pragma comment(lib, "assimp.lib")

#define WINDOW_WIDTH	1920
#define WINDOW_HEIGHT	1200

class ShadowMapApp : public ICallbacks, public App
{
public:
	ShadowMapApp();
	~ShadowMapApp();

	bool Init();

	void Run();

	virtual void RenderSceneCB() override;

	void ShadowMapPass();

	void RenderPass();
private:
	ShadowMapTechnique* shadowMapTechnique_;
	ShadowMapFBO shadowMapFBO_;
	SpotLight spotLight_;

	Mesh* mesh_;
	Mesh* quad_;
	Camera* camera_;
	PersProjInfo persProjInfo_;
	float scale_;

	Vector3f cameraPosition_;
	Vector3f cameraTarget_;
	Vector3f cameraUp_;
};

ShadowMapApp::ShadowMapApp()
	: shadowMapTechnique_(nullptr)
	, mesh_(nullptr)
	, quad_(nullptr)
	, camera_(nullptr)
	, scale_(0.0f)
{
	spotLight_.ambientIntensity_ = 0.0f;
	spotLight_.diffuseIntensity_ = 0.9f;
	spotLight_.color_ = Vector3f(1.0f, 1.0f, 1.0f);
	spotLight_.position_ = Vector3f(-20.0f, 20.0f, 5.0f);
	spotLight_.direction_ = Vector3f(1.0f, -1.0f, 0.0f);
	spotLight_.attenuation_.kl = 0.01f;
	spotLight_.cutoff = 20.0f;

	persProjInfo_.FOV = 60.0f;
	persProjInfo_.Width = WINDOW_WIDTH;
	persProjInfo_.Height = WINDOW_HEIGHT;
	persProjInfo_.zNear = 1.0f;
	persProjInfo_.zFar = 50.0f;
}

ShadowMapApp::~ShadowMapApp()
{
	SAFE_DELETE(shadowMapTechnique_);
	SAFE_DELETE(mesh_);
	SAFE_DELETE(quad_);
	SAFE_DELETE(camera_);
}

bool ShadowMapApp::Init()
{
	if (!shadowMapFBO_.Init(WINDOW_WIDTH, WINDOW_HEIGHT))
		return false;

	camera_ = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);
	// old orientation
	cameraPosition_ = camera_->GetPosition();
	cameraTarget_ = camera_->GetTarget();
	cameraUp_ = camera_->GetUp();

	shadowMapTechnique_ = new ShadowMapTechnique();
	if (!shadowMapTechnique_->Init())
	{
		fprintf(stderr, "error init shadow map technique!\n");
		return false;
	}

	shadowMapTechnique_->Enable();

	quad_ = new Mesh();
	if (!quad_->LoadMesh("../Resource/quad.obj"))
		return false;

	mesh_ = new Mesh();
	return mesh_->LoadMesh("../Resource/phoenix_ugv.md2");
}

void ShadowMapApp::Run()
{
	GLUTBackendRun(this);
}

void ShadowMapApp::RenderSceneCB()
{
	camera_->OnRender();
	scale_ += 0.05f;

	ShadowMapPass();
	RenderPass();

	glutSwapBuffers();
}

void ShadowMapApp::ShadowMapPass()
{
	shadowMapFBO_.BindForWriting();

	glClear(GL_DEPTH_BUFFER_BIT);

	// remember old orientation
	cameraPosition_ = camera_->GetPosition();
	cameraTarget_ = camera_->GetTarget();
	cameraUp_ = camera_->GetUp();

	// set camera at light orientation
	camera_->SetPosition(spotLight_.position_);
	camera_->SetTarget(spotLight_.direction_);
	camera_->SetUp(Vector3f(0.0f, 1.0f, 0.0f));

	Pipeline p;
	p.Scale(0.1f, 0.1f, 0.1f);
	p.Rotate(0.0f, angle2radian(scale_), 0.0f);
	p.WorldPos(0.0f, 0.0f, 5.0f);
	p.SetCamera(*camera_);
	p.SetPerspectiveProj(persProjInfo_);
	shadowMapTechnique_->SetWVP(p.GetOGLWorldViewProjTrans());

	mesh_->Render();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMapApp::RenderPass()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shadowMapTechnique_->SetTextureUnit(0);
	shadowMapFBO_.BindForReading(GL_TEXTURE0);

	// set camera at eye orientation
	camera_->SetPosition(cameraPosition_);
	camera_->SetTarget(cameraTarget_);
	camera_->SetUp(cameraUp_);

	Pipeline p;
	p.Scale(5.0f, 5.0f, 5.0f);
	p.WorldPos(0.0f, 0.0f, 10.0f);
	p.SetCamera(*camera_);
	p.SetPerspectiveProj(persProjInfo_);
	shadowMapTechnique_->SetWVP(p.GetOGLWorldViewProjTrans());

	quad_->Render();
}

int main(int argc, char** argv)
{
	GLUTBackendInit(argc, argv, true, false);

	if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false, "tutorial 23"))
		return 1;

	ShadowMapApp* app = new ShadowMapApp();
	if (!app->Init())
		return 1;

	app->Run();

	delete app;

	return 0;
}
