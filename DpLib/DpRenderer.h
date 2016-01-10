/********************************************************************
	created:	2015/11/20
	created:	20:11:2015   11:26
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpRenderer.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpRenderer
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Renderer
*********************************************************************/
#ifndef __DP_RENDERER_H__
#define __DP_RENDERER_H__

#include "DoPixel.h"
#include "DpMatrix44.h"
#include "DpGeometry.h"
#include "DpVertexArray.h"

namespace dopixel
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void SetShadeMode(ShadeMode::Type shadeMode);
		void SetCullMode(CullMode::Type cullMode);
		void SetZBufferType(ZBuffer::Type zbufType);
		void SetZFuncType(Condition::Type zfunc);
		void SetTextureWrap(TextureWrap::Type wrap);
		void SetTextureFilter(TextureFilter::Type filter);

		ShadeMode::Type GetShadeMode() const;
		CullMode::Type GetCullMode() const;
		ZBuffer::Type GetZBufferType() const;
		Condition::Type GetZFuncType() const;
		TextureWrap::Type GetTextureWrap() const;
		TextureFilter::Type GetTextureFilter() const;

		void SetFrameBuffer(unsigned char* frameBuf, int width, int height, int pitch);
		void BeginScene();
		void EndScene();

		void AddClipPlane(const math::Plane& plane);
		void ClearClipPlane();

		void DrawPrimitive(const VertexBufferRef& vertexBuffer);
		void DrawPrimitive(const VertexBufferRef& vertexBuffer, const MaterialRef& material);
		void DrawPrimitive(const VertexBufferRef& vertexBuffer, const IndexBufferRef& indexBuffer);
		void DrawPrimitive(const VertexBufferRef& vertexBuffer, const IndexBufferRef& indexBuffer, const MaterialRef& material);

		void SetTransform(Transform::Type type, const math::Matrix44f& matrix);
		const math::Matrix44f& GetTransform(Transform::Type type) const;

		void RenderSubMesh(const SubMeshRef& submesh);
	private:
		void UpdateTransform();

		ShadeMode::Type shadeMode_;
		CullMode::Type cullMode_;
		ZBuffer::Type zbufType_;
		Condition::Type zfunc_;
		TextureWrap::Type textureWrap_;
		TextureFilter::Type textureFilter_;

		unsigned char* frameBuf_;
		int width_;
		int height_;
		int pitch_;
		vector<float> zbuf_;
		RasterizerRef rasterizer_;

		SceneManagerRef sceneManager_;
		MaterialRef material_;
		VertexBufferRef vertexBuffer_;
		IndexBufferRef indexBuffer_;
		Ref<VertexArray3f> triangleNormalsBuf_;

		math::Matrix44f matrixs_[Transform::Max];
		// view frustum
		math::Frustum viewFrustum_;
		// eye world position
		math::Vector3f eyeWorldPos_;
		// clip plane
		vector<math::Plane> clipPlanes_;

		// internal state
		bool transformValid_;
		math::Matrix44f viewProjMatrix_;
		math::Matrix44f worldViewMatrix_;
		math::Matrix44f worldViewProjMatrix_;
		class Impl;
		Ref<Impl> impl_;
	};
}

#endif