/********************************************************************
	created:	2016/03/13
	created:	13:3:2016   20:54
	filename: 	D:\OneDrive\3D\DpLib\DpOpenGL\ogl_camera.h
	file path:	D:\OneDrive\3D\DpLib\DpOpenGL
	file base:	ogl_camera
	file ext:	h
	author:		mi2think@gmail.com

	purpose:	ogl camera
*********************************************************************/
#ifndef __OGL_CAMERA_H__
#define __OGL_CAMERA_H__

#include "DpVector2.h"
#include "DpVector3.h"
using namespace dopixel::math;

namespace ogl
{
	// FPS camera 
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
}

#endif
