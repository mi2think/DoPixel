/********************************************************************
	created:	2015/11/20
	created:	20:11:2015   11:27
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpRenderer.cpp
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpRenderer
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Renderer
*********************************************************************/
#include "DpRenderer.h"
#include "DpSceneManager.h"
#include "DpMesh.h"
#include "DpMaterial.h"
#include "DpVertexBuffer.h"
#include "DpIndexBuffer.h"

namespace dopixel
{
	class Renderer::Cache
	{
	public:
		Cache() {}

		void SetPrimitiveType(int primitiveType) { primitiveType_ = primitiveType; }

		Ref<VertexArray4f>& Posotions() { return positions_; }
		Ref<VertexArray4f>& Normals() { return normals_; }
		Ref<VertexArray3f>& Colors() { return colors_; }
		Ref<VertexArray3f>& TriangleNormals() { return triangleNormals_; }

#define ALLOC(ref, T, count) { \
		if (!ref) \
			ref = new T(count); \
		else \
			ref->SetVertexCount(count); \
		return ref; \
		}

		Ref<VertexArray4f>& AllocPositions(int count) { ALLOC(positions_, VertexArray4f, count); }
		Ref<VertexArray4f>& AllocNormals(int count) { ALLOC(normals_, VertexArray4f, count); }
		Ref<VertexArray3f>& AllocColors(int count) { ALLOC(colors_, VertexArray3f, count); }
		Ref<VertexArray3f>& AllocTriangleNormals(int count) { ALLOC(triangleNormals_, VertexArray3f, count); }

		IndexBufferRef& IndexBuf() { return indexBuf_; }
		void IndexBuf(const IndexBufferRef& indexBuf) { indexBuf_ = indexBuf; }

		void TransformBy(const math::Matrix44f& matrix);
		void GenTriangleNormals();
		void GenVertexNormals();
	private:

		// primitive type
		int primitiveType_;

		Ref<VertexArray4f> positions_;
		Ref<VertexArray4f> normals_;
		Ref<VertexArray3f> colors_;

		IndexBufferRef indexBuf_;

		Ref<VertexArray3f> triangleNormals_;
	};

	void Renderer::Cache::GenTriangleNormals()
	{
		ASSERT(positions_ && primitiveType_ == PrimitiveType::Triangles);
		ASSERT(triangleNormals_);

		const int vertexCount = positions_->GetVertexCount();
		const math::Vector4f* position = positions_->DataAs<math::Vector4f>();
		math::Vector3f* normal = triangleNormals_->DataAs<math::Vector3f>();

		if (indexBuf_)
		{
			ASSERT(indexBuf_->GetPrimitiveType() == PrimitiveType::Triangles);

			const unsigned int* indices = indexBuf_->GetData();
			const int indexCount = indexBuf_->GetIndexCount();
			for (int i = 0, j = 0; i < indexCount; i += 3, ++j)
			{
				unsigned int index0 = *(indices + i);
				unsigned int index1 = *(indices + i + 1);
				unsigned int index2 = *(indices + i + 2);
				const auto& p0 = *(position + index0);
				const auto& p1 = *(position + index1);
				const auto& p2 = *(position + index2);

				auto u = p1 - p0;
				auto v = p2 - p0;
				auto n = math::CrossProduct(u, v);
				*(normal + j) = math::Vector3f(n.x, n.y, n.z);
			}
		}
		else
		{
			for (int i = 0, j = 0; i < vertexCount; i += 3, ++j)
			{
				const auto& p0 = *(position + i);
				const auto& p1 = *(position + i + 1);
				const auto& p2 = *(position + i + 2);

				auto u = p1 - p0;
				auto v = p2 - p0;
				auto n = math::CrossProduct(u, v);
				*(normal + j) = math::Vector3f(n.x, n.y, n.z);
			}
		}
	}

	void Renderer::Cache::GenVertexNormals()
	{
		ASSERT(!normals_);
	}

	void Renderer::Cache::TransformBy(const math::Matrix44f& matrix)
	{
		ASSERT(positions_);
		const int vertexCount = positions_->GetVertexCount();

		math::Vector4f* position = positions_->DataAs<math::Vector4f>();
		for (int i = 0; i < vertexCount; ++i)
		{
			math::Vector4f& p = *(position + i);
			p *= matrix;
		}

		if (triangleNormals_ || normals_)
		{
			math::Matrix44f matrixINVT;
			math::MatrixInverse(matrixINVT, matrix);
			matrixINVT.Transpose();

			if (triangleNormals_)
			{
				math::Vector3f* triangleNormals = triangleNormals_->DataAs<math::Vector3f>();
				for (int i = 0; i < vertexCount; ++i)
				{
					math::Vector3f& n = *(triangleNormals + i);
					math::Vector4f nt(n.x, n.y, n.z, 1.0f);
					nt *= matrixINVT;
					nt /= nt.w;
					n.x = nt.x;
					n.y = nt.y;
					n.z = nt.z;
				}
			}

			if (normals_)
			{
				math::Vector4f* normals = normals_->DataAs<math::Vector4f>();
				for (int i = 0; i < vertexCount; ++i)
				{
					math::Vector4f& n = *(normals + i);
					n *= matrixINVT;
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	Renderer::Renderer(const SceneManagerRef& sceneManager)
		: shadeMode_(ShadeMode::WireFrame)
		, cullMode_(CullMode::Back)
		, zbufType_(ZBuffer::None)
		, zfunc_(Condition::LessThan)
		, textureWrap_(TextureWrap::ClampToEdge)
		, textureFilter_(TextureFilter::Linear)
		, sceneManager_(sceneManager)
		, frameBuf_(nullptr)
		, width_(0)
		, height_(0)
		, pitch_(0)
		, matrixValid_(false)
		, cache_(new Cache())
	{
		for (auto& matrix : matrixs_)
		{
			matrix.Identity();
		}
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::SetShadeMode(ShadeMode::Type shadeMode)
	{
		shadeMode_ = shadeMode;
	}

	void Renderer::SetCullMode(CullMode::Type cullMode)
	{
		cullMode_ = cullMode;
	}

	void Renderer::SetZBufferType(ZBuffer::Type zbufType)
	{
		zbufType_ = zbufType;
	}

	void Renderer::SetZFuncType(Condition::Type zfunc)
	{
		zfunc_ = zfunc;
	}

	void Renderer::SetTextureWrap(TextureWrap::Type wrap)
	{
		textureWrap_ = wrap;
	}

	void Renderer::SetTextureFilter(TextureFilter::Type filter)
	{
		textureFilter_ = filter;
	}

	ShadeMode::Type Renderer::GetShadeMode() const
	{
		return shadeMode_;
	}

	CullMode::Type Renderer::GetCullMode() const
	{
		return cullMode_;
	}

	ZBuffer::Type Renderer::GetZBufferType() const
	{
		return zbufType_;
	}

	Condition::Type Renderer::GetZFuncType() const
	{
		return zfunc_;
	}

	TextureWrap::Type Renderer::GetTextureWrap() const
	{
		return textureWrap_;
	}

	TextureFilter::Type Renderer::GetTextureFilter() const
	{
		return textureFilter_;
	}

	void Renderer::SetFrameBuffer(unsigned char* frameBuf, int width, int height, int pitch)
	{
		frameBuf_ = frameBuf;
		width_ = width;
		height_ = height;
		pitch_ = pitch;

		if (zbufType_ != ZBuffer::None)
		{
			zbuf_.resize(width_ * height_);
		}
	}

	void Renderer::BeginScene()
	{
		unsigned char* p = frameBuf_;
		for (int i = 0; i < height_; ++i)
		{
			memset(p, 0xff, width_ * 4);
			p += pitch_;
		}
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::SetTransform(Transform::Type type, const math::Matrix44f& matrix)
	{
		ASSERT(type < Transform::Max);
		matrixs_[type] = matrix;
		matrixValid_ = false;
	}

	const math::Matrix44f& Renderer::GetTransform(Transform::Type type) const
	{
		ASSERT(type < Transform::Max);
		return matrixs_[type];
	}

	void Renderer::UpdateMatrix()
	{
		MatrixMultiply(worldViewMatrix_, matrixs_[Transform::World], matrixs_[Transform::View]);
		MatrixMultiply(worldViewProjMatrix_, worldViewMatrix_, matrixs_[Transform::Projection]);
		matrixValid_ = true;
	}

	void Renderer::RenderSubMesh(const SubMeshRef& submesh)
	{
		triangleNormals_ = submesh->GetTriangleNormals();
		DrawPrimitive(submesh->GetVertexBuffer(), submesh->GetIndexBuffer(), submesh->GetMaterial());
	}

	void Renderer::DrawPrimitive(const VertexBufferRef& vertexBuffer)
	{
		DrawPrimitive(vertexBuffer, IndexBufferRef(), MaterialRef());
	}

	void Renderer::DrawPrimitive(const VertexBufferRef& vertexBuffer, const IndexBufferRef& indexBuffer)
	{
		DrawPrimitive(vertexBuffer, indexBuffer, MaterialRef());
	}

	void Renderer::DrawPrimitive(const VertexBufferRef& vertexBuffer, const MaterialRef& material)
	{
		DrawPrimitive(vertexBuffer, IndexBufferRef(), material);
	}

	void Renderer::DrawPrimitive(const VertexBufferRef& vertexBuffer, const IndexBufferRef& indexBuffer, const MaterialRef& material)
	{
		vertexBuffer_ = vertexBuffer;
		indexBuffer_ = indexBuffer;
		material_ = material;

		// init state
		cache_->SetPrimitiveType(vertexBuffer_->GetPrimitiveType());
		cache_->IndexBuf(indexBuffer_);

		const int  vertexCount = vertexBuffer_->GetVertexCount();
		const int  triangleCount = (indexBuffer_ ? indexBuffer_->GetPrimitiveCount() : vertexCount / 3);
		const bool useVertexColor = (shadeMode_ != ShadeMode::WireFrame && shadeMode_ != ShadeMode::Constant || vertexBuffer->GetColors());
		const bool useTexture = (vertexBuffer->GetVertexType() & VertexType::TexCoord) && material_->GetTexture();
		const bool useLighting = (shadeMode_ != ShadeMode::WireFrame && shadeMode_ != ShadeMode::Constant);
		const bool useNormal = useLighting;

		// copy position to cache
		math::Vector3f* raw_position = vertexBuffer->GetPositions()->DataAs<math::Vector3f>();
		ASSERT(raw_position);
		auto& positions = cache_->AllocPositions(vertexCount);
		math::Vector4f* position = positions->DataAs<math::Vector4f>();
		for (int i = 0; i < vertexCount; ++i)
		{
			const auto& p = *(raw_position + i);
			*(position + i) = math::Vector4f(p.x, p.y, p.z, 1.0f);
		}

		// copy color to cache
		if (useVertexColor)
		{
			auto& colors = cache_->AllocColors(vertexCount);
			math::Vector3f* color = colors->DataAs<math::Vector3f>();
			const auto& raw_colors = vertexBuffer->GetColors();
			if (raw_colors)
			{
				memcpy(color, raw_colors->GetData(), vertexCount * sizeof(math::Vector3f));
			}
			else
			{
				math::Vector3f v(1.0f, 1.0f, 1.0f);
				math::Vector3f* color = colors->DataAs<math::Vector3f>();
				for (int i = 0; i < vertexCount; ++i)
				{
					*(color + i) = v;
				}
			}
		}

		// copy normal to cache if have
		bool genVertexNormals = false;
		if (useNormal)
		{
			auto& normals = cache_->AllocNormals(vertexCount);
			const auto& raw_normals_ref = vertexBuffer->GetNormals();
			genVertexNormals = (raw_normals_ref == nullptr);
			if (!genVertexNormals)
			{
				math::Vector3f* raw_normal = raw_normals_ref->DataAs<math::Vector3f>();
				math::Vector4f* normal = normals->DataAs<math::Vector4f>();
				for (int i = 0; i < vertexCount; ++i)
				{
					const auto& n = *(raw_normal + i);
					*(normal + i) = math::Vector4f(n.x, n.y, n.z, 1.0f);
				}
			}
		}

		// copy triangle normal to cache if have
		bool genTriangleNormals = true;
		if (triangleNormals_)
		{
			auto& triangleNormals = cache_->AllocTriangleNormals(triangleCount);
			memcpy(triangleNormals->GetData(), triangleNormals_->GetData(), triangleCount * sizeof(math::Vector3f));
			genTriangleNormals = false;
		}
		
		// transform vertex from model to world
		cache_->TransformBy(matrixs_[Transform::World]);

		// generate triangle normals for remove back face
		// and generate vertex normals if need
		if (genTriangleNormals)
		{
			cache_->AllocTriangleNormals(triangleCount);
			cache_->GenTriangleNormals();
		}
		if (genVertexNormals)
		{
			// has allocated before
			cache_->GenVertexNormals();
		}

		// vertex ref buffer record each vertex shared by how many triangles

		// triangle cull buffer record culled triangle index

		// remove back face base on cull type

		// clip by plane

		// transform vertex by WVP, only shared by at least one triangle

		// clip by CCW?

		// lighting for flat or gouraud shade mode here, phong will light in pixel shader

		// transform projected vertex to screen

		// draw triangle

		// end
	}
}