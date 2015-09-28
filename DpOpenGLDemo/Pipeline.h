#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include "DpVector3.h"
#include "DpMatrix44.h"
using namespace dopixel::math;

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

	const Matrix44f& GetOGLWorldTrans();
private:
	Vector3f scale_;
	Vector3f position_;
	Vector3f rotate_;

	Matrix44f worldTrans_;
};

#endif