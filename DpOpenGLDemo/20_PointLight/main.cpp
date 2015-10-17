#include <stdio.h>
#include <assert.h>

#include <vector>
#include <map>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Pipeline.h"

#include "DpVector2.h"
#include "DpVector3.h"
#include "DpMatrix44.h"
using namespace dopixel::math;

#include "lighting_technique.h"

#pragma comment(lib, "glew32.lib")

#define WINDOW_WIDTH	1920
#define WINDOW_HEIGHT	1200

#define FILED_DEPTH		20.0f
#define FILED_WIDTH		10.0f

struct Vertex
{
	Vector3f pos;
	Vector2f tex;
	Vector3f normal;

	Vertex() {}
	Vertex(const Vector3f& _pos, const Vector2f& _tex, const Vector3f& _normal)
		: pos(_pos)
		, tex(_tex)
		, normal(_normal)
	{
	}
};

class PointLightApp : public ICallbacks, public App
{
public:
	PointLightApp();
	~PointLightApp();

	bool Init();

	void Run();

	virtual void RenderSceneCB() override;
private:
	void CreateVertexBuffer();

	GLuint VBO_;
	LightingTechnique* effect_;
	Texture* texture_;
	Camera* camera_;
	PersProjInfo persProjInfo_;
	DirectionalLight directionalLight_;
	float scale_;
};

PointLightApp::PointLightApp()
	: effect_(nullptr)
	, camera_(nullptr)
	, texture_(nullptr)
	, scale_(0.0f)
{
	directionalLight_.color_ = Vector3f(1, 1, 1);
	directionalLight_.ambientIntensity_ = 0.05f;
	directionalLight_.diffuseIntensity_ = 0.01f;
	directionalLight_.direction_ = Vector3f(1, -1, 0);

	persProjInfo_.FOV = 60;
	persProjInfo_.Width = WINDOW_WIDTH;
	persProjInfo_.Height = WINDOW_HEIGHT;
	persProjInfo_.zNear = 1;
	persProjInfo_.zFar = 50;
}

PointLightApp::~PointLightApp()
{
	delete effect_;
	delete camera_;
	delete texture_;
}

bool PointLightApp::Init()
{
	CreateVertexBuffer();

	Vector3f pos(5.0f, 1.0f, -3.0f);
	Vector3f target(0.0f, 0.0f, 1.0f);
	Vector3f up(0.0f, 1.0f, 0.0f);

	camera_ = new Camera(pos, target, up, WINDOW_WIDTH, WINDOW_HEIGHT);

	effect_ = new LightingTechnique();
	if (!effect_->Init())
		return false;
	effect_->Enable();
	effect_->SetTextureUnit(0);

	texture_ = new Texture(GL_TEXTURE_2D, "checkerboard.png");
	if (!texture_->Load())
		return false;

	return true;
}

void PointLightApp::Run()
{
	GLUTBackendRun(this);
}

void PointLightApp::CreateVertexBuffer()
{
	const Vector3f normal = Vector3f(0.0f, 1.0f, 0.0f);

	Vertex vertices[6] = 
	{
		Vertex(Vector3f(0.0f, 0.0f, 0.0f), Vector2f(0.0f, 0.0f), normal),
		Vertex(Vector3f(0.0f, 0.0f, FILED_DEPTH), Vector2f(0.0f, 1.0f), normal),
		Vertex(Vector3f(FILED_WIDTH, 0.0f, 0.0f), Vector2f(1.0f, 0.0f), normal),

		Vertex(Vector3f(FILED_WIDTH, 0.0f, 0.0f), Vector2f(1.0f, 0.0f), normal),
		Vertex(Vector3f(0.0f, 0.0f, FILED_DEPTH), Vector2f(0.0f, 1.0f), normal),
		Vertex(Vector3f(FILED_WIDTH, 0.0f, FILED_DEPTH), Vector2f(1.0f, 1.0f), normal)
	};
	
	glGenBuffers(1, &VBO_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void PointLightApp::RenderSceneCB()
{
	scale_ += 0.0057f;

	camera_->OnRender();

	glClear(GL_COLOR_BUFFER_BIT);

	PointLight pl[2];
	pl[0].ambientIntensity_ = 0.0f;
	pl[0].diffuseIntensity_ = 0.5f;
	pl[0].color_ = Vector3f(1.0f, 1.0f, 1.0f);
	pl[0].position_ = Vector3f(3.0f, 1.0f, FILED_DEPTH * (cosf(scale_) + 1.0f) / 2.0f);
	pl[0].attenuation_.kc = 1.0f;
	pl[0].attenuation_.kl = 0.1f;
	pl[0].attenuation_.kq = 0.0f;

	pl[1].ambientIntensity_ = 0.0f;
	pl[1].diffuseIntensity_ = 0.5f;
	pl[1].color_ = Vector3f(0.0f, 0.5f, 1.0f);
	pl[1].position_ = Vector3f(7.0f, 1.0f, FILED_DEPTH * (sinf(scale_) + 1.0f) / 2.0f);
	pl[1].attenuation_.kc = 1.0f;
	pl[1].attenuation_.kl = 0.1f;
	pl[1].attenuation_.kq = 0.0f;

	effect_->SetPointLights(2, pl);

	Pipeline p;
	p.WorldPos(0.0f, 0.0f, 1.0f);
	p.SetCamera(*camera_);
	p.SetPerspectiveProj(persProjInfo_);

	effect_->SetWVP(p.GetOGLWorldViewProjTrans());
	effect_->SetWorldMatrix(p.GetOGLWorldTrans());
	effect_->SetEyeWorldPos(camera_->GetPosition());
	effect_->SetSpecularIntensity(0.0f);
	effect_->SetSpecularPower(0);
	effect_->SetDirectionalLight(directionalLight_);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

	texture_->Bind(GL_TEXTURE0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	GLUTBackendInit(argc, argv, false, false);

	if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false, "tutorial 20"))
		return 1;

	PointLightApp* app = new PointLightApp();
	if (!app->Init())
		return 1;

	app->Run();

	delete app;

	return 0;
}
