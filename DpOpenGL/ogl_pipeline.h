/********************************************************************
	created:	2016/03/13
	created:	13:3:2016   22:12
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_pipeline.h
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_pipeline
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	ogl pipeline
*********************************************************************/
#ifndef __OGL_PIPELINE_H__
#define __OGL_PIPELINE_H__

#include "DpVector3.h"
#include "DpMatrix44.h"
using namespace dopixel::math;

namespace ogl
{
	struct PersProjInfo
	{
		float FOV;
		float Width;
		float Height;
		float zNear;
		float zFar;
	};

	class Camera;

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
		void SetCamera(const Vector3f& position, const Vector3f& target, const Vector3f& up);

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

		Vector3f cameraPosition_;
		Vector3f cameraTarget_;
		Vector3f cameraUp_;

		Matrix44f worldTrans_;
		Matrix44f viewTrans_;
		Matrix44f projTrans_;

		Matrix44f worldProjTrans_;
		Matrix44f worldViewProjTrans_;
	};
}

#endif
