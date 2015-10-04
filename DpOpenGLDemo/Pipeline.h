#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include <GL/glew.h>

#include "DoPixel.h"
#include "DpVector2.h"
#include "DpVector3.h"
#include "DpMatrix44.h"
#include "DpQuaternion.h"
using namespace dopixel;
using namespace dopixel::math;

class Texture
{
public:
	Texture(GLenum textureTarget, const CString& fileName);

	bool Load();

	void Bind(GLenum textureUnit);
private:
	CString fileName_;
	int width_;
	int height_;
	unsigned char* data_;

	GLenum textureTarget_;
	GLuint textureObj_;
};

class Camera
{
public:
	Camera(int windowWidth, int windowHight);
	Camera(const Vector3f& pos, const Vector3f& target, const Vector3f& up, int windowWidth, int windowHight);

	bool OnKayboard(int key);
	void OnMouse(int x, int y);

	void Update();
	void OnRender();

	void SetStepScale(float stepScale);

	const Vector3f& GetPosition() const { return position_; }
	const Vector3f& GetTarget() const { return target_; }
	const Vector3f& GetUp() const { return up_; }
private:
	void Init();

	Vector3f position_;
	Vector3f target_;
	Vector3f up_;

	float stepScale_;

	Vector2i mousePos_;
	float angleH_;
	float angleV_;

	bool onLEdge_;	// on left edge
	bool onREdge_;	// on right edge
	bool onTEdge_;	// on top edge
	bool onBEdge_;	// on bottom edge

	int windowWidth_;
	int windowHeight_;

	bool needUpdate_;
};


struct PersProjInfo
{
	float FOV;
	float Width;
	float Height;
	float zNear;
	float zFar;
};

class Pipeline
{
public:
	Pipeline();

	void Scale(float s);
	void Scale(float x, float y, float z);
	void Scale(const Vector3f& scale);
	
	void WorldPos(float x, float y, float z);
	void WorldPos(const Vector3f& pos);
	
	void Rotate(float x, float y, float z);
	void Rotate(const Vector3f& rorate);

	void SetPerspectiveProj(const PersProjInfo& p);

	void SetCamera(const Camera& camera);

	// world
	const Matrix44f& GetOGLWorldTrans();
	// view
	const Matrix44f& GetOGLViewTrans();
	// projection
	const Matrix44f& GetOGLProjTrans();

	// world projection
	const Matrix44f& GetOGLWorldProjTrans();
	// world view projection
	const Matrix44f& GetOGLWorldViewProjTrans();
private:
	Vector3f scale_;
	Vector3f position_;
	Vector3f rotate_;

	PersProjInfo persProjInfo_;
	const Camera* camera_;

	Matrix44f worldTrans_;
	Matrix44f viewTrans_;
	Matrix44f projTrans_;

	Matrix44f worldProjTrans_;
	Matrix44f worldViewProjTrans_;
};

#endif