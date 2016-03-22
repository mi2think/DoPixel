#include <GL/glew.h>
#include <GL/freeglut.h>

#include "lighting_technique.h"

#include "ogl_app.h"
#include "ogl_glut.h"
#include "ogl_pipeline.h"
#include "ogl_camera.h"
#include "ogl_texture.h"
#include "ogl_mesh.h"
#include "ogl_shadow_map_fbo.h"
#include "ogl_shadow_map_technique.h"

using namespace ogl;

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "DpOpenGL.lib")
#pragma comment(lib, "DpLib.lib")
#pragma comment(lib, "assimp.lib")

#define WINDOW_WIDTH	1920
#define WINDOW_HEIGHT	1200

class ShadowMapApp2 : public ICallbacks, public App
{
public:
	ShadowMapApp2();
	~ShadowMapApp2();

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
	Texture* groundTex_;
	Camera* camera_;
	LightingTechnique* lightTechnique_;
	PersProjInfo persProjInfo_;
	float scale_;
};

ShadowMapApp2::ShadowMapApp2()
	: shadowMapTechnique_(nullptr)
	, mesh_(nullptr)
	, quad_(nullptr)
	, camera_(nullptr)
	, lightTechnique_(nullptr)
	, groundTex_(nullptr)
{
	spotLight_.ambientIntensity_ = 0.1f;
	spotLight_.diffuseIntensity_ = 0.9f;
	spotLight_.color_ = Vector3f(1.0f, 1.0f, 1.0f);
	spotLight_.attenuation_.kl = 0.01f;
	spotLight_.position_ = Vector3f(-20.0f, 20.0f, 1.0f);
	spotLight_.direction_ = Vector3f(1.0f, -1.0f, 0.0f);
	spotLight_.cutoff = angle2radian(20.0f);

	persProjInfo_.FOV = 60.0f;
	persProjInfo_.Width = WINDOW_WIDTH;
	persProjInfo_.Height = WINDOW_HEIGHT;
	persProjInfo_.zNear = 1.0f;
	persProjInfo_.zFar = 50.0f;
}

ShadowMapApp2::~ShadowMapApp2()
{
	SAFE_DELETE(shadowMapTechnique_);
	SAFE_DELETE(mesh_);
	SAFE_DELETE(quad_);
	SAFE_DELETE(camera_);
	SAFE_DELETE(lightTechnique_);
	SAFE_DELETE(groundTex_);
}

bool ShadowMapApp2::Init()
{
	if (!shadowMapFBO_.Init(WINDOW_WIDTH, WINDOW_HEIGHT))
		return false;

	camera_ = new Camera(Vector3f(3.0f, 8.0f, -10.0f), Vector3f(0.0f, -0.2f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f), WINDOW_WIDTH, WINDOW_HEIGHT);

	shadowMapTechnique_ = new ShadowMapTechnique();
	if (!shadowMapTechnique_->Init("../DpOpenGLDemo/24_ShadowMapping2/shadow_map.vs", "../DpOpenGLDemo/24_ShadowMapping2/shadow_map.ps", "gWVP"))
	{
		fprintf(stderr, "error init shadow map technique!\n");
		return false;
	}

	lightTechnique_ = new LightingTechnique();
	if (!lightTechnique_->Init())
	{
		fprintf(stderr, "error init lighting technique\n");
		return false;
	}
	lightTechnique_->Enable();
	lightTechnique_->SetSpotLights(1, &spotLight_);
	lightTechnique_->SetTextureUnit(0);
	lightTechnique_->SetShadowMapTextureUnit(1);

	groundTex_ = new Texture(GL_TEXTURE_2D, "../Resource/checkerboard.png");
	if (!groundTex_->Load())
		return false;

	quad_ = new Mesh();
	if (!quad_->LoadMesh("../Resource/quad.obj"))
		return false;

	mesh_ = new Mesh();
	return mesh_->LoadMesh("../Resource/phoenix_ugv.md2");
}

void ShadowMapApp2::Run()
{
	GLUTBackendRun(this);
}

void ShadowMapApp2::RenderSceneCB()
{
	camera_->OnRender();
	scale_ += 0.05f;

	ShadowMapPass();

	RenderPass();

	glutSwapBuffers();
}

void ShadowMapApp2::ShadowMapPass()
{
	shadowMapFBO_.BindForWriting();
	glClear(GL_DEPTH_BUFFER_BIT);

	Pipeline p;
	p.Scale(0.1f, 0.1f, 0.1f);
	p.Rotate(0.0f, angle2radian(scale_), 0.0f);
	p.WorldPos(0.0f, 0.0f, 3.0f);
	// light orientation
	p.SetCamera(spotLight_.position_, spotLight_.direction_, Vector3f(0.0f, 1.0f, 0.0f));
	p.SetPerspectiveProj(persProjInfo_);
	
	shadowMapTechnique_->Enable();
	shadowMapTechnique_->SetWVP(p.GetOGLWorldViewProjTrans());
	mesh_->Render();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMapApp2::RenderPass()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	lightTechnique_->Enable();
	lightTechnique_->SetEyeWorldPos(camera_->GetPosition());

	shadowMapFBO_.BindForReading(GL_TEXTURE1);

	Pipeline p;
	p.SetPerspectiveProj(persProjInfo_);
	
	p.Scale(10.0f, 10.0f, 10.0f);
	p.Rotate(angle2radian(90.0f), 0.0f, 0.0f);
	p.WorldPos(0.0f, 0.0f, 1.0f);	
	// camera orientation
	p.SetCamera(*camera_);
	lightTechnique_->SetWVP(p.GetOGLWorldViewProjTrans());
	lightTechnique_->SetWorldMatrix(p.GetOGLWorldTrans());
	// light orientation
	p.SetCamera(spotLight_.position_, spotLight_.direction_, Vector3f(0.0f, 1.0f, 0.0f));
	lightTechnique_->SetLightWVP(p.GetOGLWorldViewProjTrans());
	groundTex_->Bind(GL_TEXTURE0);
	quad_->Render();

	p.Scale(0.1f, 0.1f, 0.1f);
	p.Rotate(0.0f, angle2radian(scale_), 0.0f);
	p.WorldPos(0.0f, 0.0f, 3.0f);
	// camera orientation
	p.SetCamera(*camera_);
	lightTechnique_->SetWVP(p.GetOGLWorldViewProjTrans());
	lightTechnique_->SetWorldMatrix(p.GetOGLWorldTrans());
	// light orientation
	p.SetCamera(spotLight_.position_, spotLight_.direction_, Vector3f(0.0f, 1.0f, 0.0f));
	lightTechnique_->SetLightWVP(p.GetOGLWorldViewProjTrans());
	mesh_->Render();
}

int main(int argc, char** argv)
{
	GLUTBackendInit(argc, argv, true, false);

	if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false, "tutorial 24"))
		return 1;

	ShadowMapApp2* app = new ShadowMapApp2();
	if (!app->Init())
		return 1;

	app->Run();

	delete app;

	return 0;
}

