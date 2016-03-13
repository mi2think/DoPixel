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
	Vector3f normal;

	Vertex(const Vector3f& _pos, const Vector2f& _tex)
		: pos(_pos)
		, tex(_tex)
	{
		normal.Zero();
	}
};

class SpecularLighting : public ICallbacks, public App
{
public:
	SpecularLighting();
	~SpecularLighting();

	bool Init();

	void Run();

	virtual void RenderSceneCB();

	virtual void KeyboardCB(int key);

	virtual void PassiveMouseCB(int x, int y);
private:
	void CreateVertexBuffer();
	void CalcNormals(Vertex* pVertices, unsigned int vertexCount);

	PersProjInfo perProjInfo_;
	Camera* camera_;
	Texture* texture_;
	float scale_;

	DirectionalLight directionalLight_;
	float specularLightIntensity_;
	float specularLightPower_;
	LightingTechnique* effect_;

	GLuint VBO_;
};

SpecularLighting::SpecularLighting()
	: texture_(nullptr)
	, camera_(nullptr)
	, effect_(nullptr)
	, scale_(0.0f)
	, specularLightIntensity_(1.0f)
	, specularLightPower_(32)
{
	directionalLight_.color = Vector3f(1.0f, 1.0f, 1.0f);
	directionalLight_.ambientIntensity = 0.0f;
	directionalLight_.diffuseIntensity = 0.2f;
	directionalLight_.direction = Vector3f(0.0f, 0.0f, 1.0f);

	perProjInfo_.FOV = 60.0f;
	perProjInfo_.Width = WINDOW_WIDTH;
	perProjInfo_.Height = WINDOW_HEIGHT;
	perProjInfo_.zNear = 1.0f;
	perProjInfo_.zFar = 100.0f;
}

SpecularLighting::~SpecularLighting()
{
	delete texture_;
	delete camera_;
	delete effect_;
}

bool SpecularLighting::Init()
{
	camera_ = new Camera(Vector3f(0.0f, 0.0f, -3.0f), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f), WINDOW_WIDTH, WINDOW_HEIGHT);

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

void SpecularLighting::CreateVertexBuffer()
{
	const int vertexCount = 12;

	Vertex vertices[vertexCount] = {
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

	CalcNormals(vertices, vertexCount);

	glGenBuffers(1, &VBO_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void SpecularLighting::CalcNormals(Vertex* pVertices, unsigned int vertexCount)
{
	// compute each triangle normal
	for (unsigned int i = 0; i < vertexCount; i += 3)
	{
		auto& v0 = pVertices[i];
		auto& v1 = pVertices[i + 1];
		auto& v2 = pVertices[i + 2];

		Vector3f u = v1.pos - v0.pos;
		Vector3f v = v2.pos - v0.pos;
		Vector3f n = CrossProduct(u, v);
		n.Normalize();

		v0.normal = n;
		v1.normal = n;
		v2.normal = n;
	}

	// compute vertex normal which have same position
	// k: vertex
	// v: normals
	struct CompareVector3f
	{
		bool operator() (const Vector3f& lhs, const Vector3f& rhs) const
		{
			if (lhs.x != rhs.x)
				return lhs.x < rhs.x;
			if (lhs.y != rhs.y)
				return lhs.y < rhs.y;
			return lhs.z < rhs.z;
		}
	};

	std::map<Vector3f, Vector3f, CompareVector3f> vertexNormals;
	for (unsigned int i = 0; i < vertexCount; ++i)
	{
		auto& v = pVertices[i];
		auto& normal = vertexNormals[v.pos];
		normal += v.normal;
	}

	// normalize
	for (auto& kv : vertexNormals)
	{
		kv.second.Normalize();
	}

	// assign to vertex
	for (unsigned int i = 0; i < vertexCount; ++i)
	{
		auto& v = pVertices[i];
		const auto& normal = vertexNormals[v.pos];
		v.normal = normal;
	}
}

void SpecularLighting::Run()
{
	GLUTBackendRun(this);
}

void SpecularLighting::RenderSceneCB()
{
	camera_->OnRender();

	glClear(GL_COLOR_BUFFER_BIT);

	scale_ += 0.1f;

	Pipeline p;
	p.Rotate(0.0f, angle2radian(scale_), 0.0f);
	p.WorldPos(0.0f, 0.0f, 1.0f);
	p.SetCamera(*camera_);
	p.SetPerspectiveProj(perProjInfo_);

	effect_->SetWVP(p.GetOGLWorldViewProjTrans());
	effect_->SetWorldMatrix(p.GetOGLWorldTrans());
	effect_->SetDirectionalLight(directionalLight_);

	effect_->SetEyeWorldPos(camera_->GetPosition());
	effect_->SetSpecularIntensity(specularLightIntensity_);
	effect_->SetSpecularPower(specularLightPower_);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

	texture_->Bind(GL_TEXTURE0);
	glDrawArrays(GL_TRIANGLES, 0, 12);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glutSwapBuffers();
}

void SpecularLighting::KeyboardCB(int key)
{
	switch (key)
	{
	case 'q':
		GLUTBackendLeaveMainLoop();
		break;
	case 'a':
		directionalLight_.ambientIntensity += 0.05f;
		break;
	case 's':
		directionalLight_.ambientIntensity -= 0.05f;
		break;
	case 'z':
		directionalLight_.diffuseIntensity += 0.05f;
		break;
	case 'x':
		directionalLight_.diffuseIntensity -= 0.05f;
		break;
	case 'j':
		specularLightPower_ += 1.0f;
		break;
	case 'k':
		specularLightPower_ -= 1.0f;
		break;
	case 'n':
		specularLightIntensity_ += 0.05f;
		break;
	case 'm':
		specularLightIntensity_ -= 0.05f;
		break;
	default:
		break;
	}
}

void SpecularLighting::PassiveMouseCB(int x, int y)
{
	camera_->OnMouse(x, y);
}

int main(int argc, char** argv)
{
	GLUTBackendInit(argc, argv, false, false);

	if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false, "tutorial 19"))
		return 1;

	SpecularLighting* app = new SpecularLighting();
	if (!app->Init())
		return 1;

	app->Run();

	delete app;
	return 0;
}