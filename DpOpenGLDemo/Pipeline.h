#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include "DpVector3.h"
#include "DpMatrix44.h"
using namespace dopixel::math;

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

	const Matrix44f& GetOGLWorldTrans();

	const Matrix44f& GetOGLProjTrans();

	const Matrix44f& GetOGLWorldProjTrans();
private:
	Vector3f scale_;
	Vector3f position_;
	Vector3f rotate_;

	PersProjInfo persProjInfo_;

	Matrix44f worldTrans_;
	Matrix44f projTrans_;
	Matrix44f worldProjTrans_;
};

#endif