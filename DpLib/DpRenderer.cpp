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
	enum TriangleCullType
	{
		Cull_Face		= BIT(0),
		CUll_ClipPlane	= BIT(1),
		Cull_Frustum	= BIT(2),
	};

	class Renderer::Impl
	{
	public:
		virtual ~Impl() {}

		void SetPrimitiveType(int primitiveType) { primitiveType_ = primitiveType; }

		Ref<VertexArray4f>& Posotions() { return positions_; }
		Ref<VertexArray3f>& Normals() { return normals_; }
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
		Ref<VertexArray3f>& AllocNormals(int count) { ALLOC(normals_, VertexArray3f, count); }
		Ref<VertexArray3f>& AllocColors(int count) { ALLOC(colors_, VertexArray3f, count); }
		Ref<VertexArray3f>& AllocTriangleNormals(int count) { ALLOC(triangleNormals_, VertexArray3f, count); }

		IndexBufferRef& IndexBuf() { return indexBuf_; }
		void IndexBuf(const IndexBufferRef& indexBuf) { indexBuf_ = indexBuf; }

		void Transform(const math::Matrix44f& matrix);
		void GenTriangleNormals();
		void GenVertexNormals();

		int* VertexRefBuf() { return &vertexRefBuf_[0]; }
		int* VertexCullBuf() { return &vertexCullBuf_[0]; }
		int* TriangleCullBuf() { return &triangleCullBuff_[0]; }
		int* VertexRefBuf(int count) { vertexRefBuf_.resize(count); return &vertexRefBuf_[0];}
		int* VertexCullBuf(int count) { vertexCullBuf_.resize(count); return &vertexCullBuf_[0]; }
		int* TriangleCullBuf(int count) { triangleCullBuff_.resize(count); return &triangleCullBuff_[0];}
		void ResetCounter(int vertexCount, int triangleCount);

		void CullFace(const math::Vector3f& eyeWorldPos, CullMode::Type cullMode);
	private:
		// primitive type
		int primitiveType_;

		Ref<VertexArray4f> positions_;
		Ref<VertexArray3f> normals_;
		Ref<VertexArray3f> colors_;
		Ref<VertexArray3f> triangleNormals_;

		IndexBufferRef indexBuf_;

		// counter
		vector<int> vertexRefBuf_;
		vector<int> vertexCullBuf_;
		vector<int> triangleCullBuff_;
	};

	void Renderer::Impl::GenTriangleNormals()
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
				const auto& p0 = *(position + *(indices + i));
				const auto& p1 = *(position + *(indices + i + 1));
				const auto& p2 = *(position + *(indices + i + 2));
				auto tp0 = p0.DivW();
				auto tp1 = p1.DivW();
				auto tp2 = p2.DivW();

				auto u = tp1 - tp0;
				auto v = tp2 - tp0;
				auto n = math::CrossProduct(u, v);
				n.Normalize();
				*(normal + j) = n;
			}
		}
		else
		{
			for (int i = 0, j = 0; i < vertexCount; i += 3, ++j)
			{
				const auto& p0 = *(position + i);
				const auto& p1 = *(position + i + 1);
				const auto& p2 = *(position + i + 2);
				auto tp0 = p0.DivW();
				auto tp1 = p1.DivW();
				auto tp2 = p2.DivW();

				auto u = tp1 - tp0;
				auto v = tp2 - tp0;
				auto n = math::CrossProduct(u, v);
				n.Normalize();
				*(normal + j) = n;
			}
		}
	}

	void Renderer::Impl::GenVertexNormals()
	{
		ASSERT(positions_ && primitiveType_ == PrimitiveType::Triangles);
		ASSERT(normals_);

		const int vertexCount = positions_->GetVertexCount();
		const math::Vector4f* position = positions_->DataAs<math::Vector4f>();
		math::Vector3f* normal = normals_->DataAs<math::Vector3f>();

		// zero init value
		memset(normal, 0, normals_->GetDataStride() * normals_->GetVertexCount());

		if (indexBuf_)
		{
			ASSERT(indexBuf_->GetPrimitiveType() == PrimitiveType::Triangles);

			const unsigned int* indices = indexBuf_->GetData();
			const int indexCount = indexBuf_->GetIndexCount();
			for (int i = 0; i < indexCount; i += 3)
			{
				const auto index0 = *(indices + i);
				const auto index1 = *(indices + i + 1);
				const auto index2 = *(indices + i + 2);
				const auto& p0 = *(position + index0);
				const auto& p1 = *(position + index1);
				const auto& p2 = *(position + index2);
				auto tp0 = p0.DivW();
				auto tp1 = p1.DivW();
				auto tp2 = p2.DivW();

				auto u = tp1 - tp0;
				auto v = tp2 - tp0;
				auto n = math::CrossProduct(u, v);
				normal[index0] += n;
				normal[index1] += n;
				normal[index2] += n;
			}
		}
		else
		{
			// compute each triangle normal
			for (int i = 0; i < vertexCount; i += 3)
			{
				const auto index0 = i;
				const auto index1 = i + 1;
				const auto index2 = i + 2;
				const auto& p0 = *(position + index0);
				const auto& p1 = *(position + index1);
				const auto& p2 = *(position + index2);
				auto tp0 = p0.DivW();
				auto tp1 = p1.DivW();
				auto tp2 = p2.DivW();

				auto u = tp1 - tp0;
				auto v = tp2 - tp0;
				auto n = math::CrossProduct(u, v);
				normal[index0] += n;
				normal[index1] += n;
				normal[index2] += n;
			}
			// may some vertex have same place

			// k: vertex
			// v: normals
			struct CompareVector4f
			{
				bool operator() (const math::Vector4f& lhs, const math::Vector4f& rhs) const
				{
					math::Vector3f a = lhs.DivW();
					math::Vector3f b = rhs.DivW();
					if (a.x != b.x)
						return a.x < b.x;
					if (a.y != b.y)
						return a.y < b.y;
					return a.z < b.z;
				}
			};
			
			map<math::Vector4f, math::Vector3f, CompareVector4f> vertexNormals;
			for (int i = 0; i < vertexCount; ++i)
			{
				auto& p = position[i];
				auto& nt = vertexNormals[p];
				nt += normal[i];
			}

			// normalize
			for (auto& kv : vertexNormals)
			{
				kv.second.Normalize();
			}

			// assign
			for (int i = 0; i < vertexCount; ++i)
			{
				auto& p = position[i];
				auto& nt = vertexNormals[p];
				normal[i] = nt;
			}
		}
	}

	void Renderer::Impl::Transform(const math::Matrix44f& matrix)
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
					// normal do not need translate
					math::Vector4f nt(n.x, n.y, n.z, 0.0f);
					nt *= matrixINVT;
					n = nt.DivW();
				}
			}

			if (normals_)
			{
				math::Vector3f* normals = normals_->DataAs<math::Vector3f>();
				for (int i = 0; i < vertexCount; ++i)
				{
					math::Vector3f& n = *(normals + i);
					// normal do not need translate
					math::Vector4f nt(n.x, n.y, n.z, 0.0f);
					nt *= matrixINVT;
					n = nt.DivW();
				}
			}
		}
	}

	void Renderer::Impl::ResetCounter(int vertexCount, int triangleCount)
	{
		VertexRefBuf(vertexCount);
		VertexCullBuf(vertexCount);
		TriangleCullBuf(triangleCount);

		memset(&vertexCullBuf_[0], 0, sizeof(int) * vertexCount);
		memset(&triangleCullBuff_[0], 0, sizeof(int) * triangleCount);

		if (indexBuf_)
		{
			memset(&vertexRefBuf_[0], 0, sizeof(int) * vertexCount);

			const unsigned int* indices = indexBuf_->GetData();
			const int indexCount = indexBuf_->GetIndexCount();
			for (int i = 0; i < indexCount; i += 3)
			{
				auto index0 = *(indices + i);
				auto index1 = *(indices + i + 1);
				auto index2 = *(indices + i + 2);

				++vertexRefBuf_[index0];
				++vertexRefBuf_[index1];
				++vertexRefBuf_[index2];
			}
		}
		else
		{
			// set to 1
			vertexRefBuf_.assign(vertexRefBuf_.size(), 1);
		}
	}

	void Renderer::Impl::CullFace(const math::Vector3f& eyeWorldPos, CullMode::Type cullMode)
	{
		ASSERT(triangleNormals_);

		math::Vector4f* position = positions_->DataAs<math::Vector4f>();
		math::Vector3f* normal = triangleNormals_->DataAs<math::Vector3f>();
		int* vertexCullBuf = VertexCullBuf();
		int* vertexRefBuf = VertexRefBuf();

		if (indexBuf_)
		{
			const unsigned int* indices = indexBuf_->GetData();
			const int indexCount = indexBuf_->GetIndexCount();
			for (int i = 0 , j = 0; i < indexCount; i += 3, ++j)
			{
				auto index0 = *(indices + i);
				auto index1 = *(indices + i + 1);
				auto index2 = *(indices + i + 2);

				math::Vector3f* p0 = (math::Vector3f*)(position + index0);
				math::Vector3f vertexToEye = eyeWorldPos - *p0;
				float dp = math::DotProduct(vertexToEye, *(normal + j));
				if (dp <= 0 && cullMode == CullMode::Back || dp >= 0 && cullMode == CullMode::Front)
				{
					vertexCullBuf[j] |= Cull_Face;
					--vertexRefBuf[index0];
					--vertexRefBuf[index1];
					--vertexRefBuf[index2];
				}
			}
		}
		else
		{
			const int vertexCount = positions_->GetVertexCount();
			for (int i = 0, j = 0; i < vertexCount; i += 3, ++j)
			{
				auto index0 = i;
				auto index1 = i + 1;
				auto index2 = i + 2;

				math::Vector3f* p0 = (math::Vector3f*)(position + index0);
				math::Vector3f vertexToEye = eyeWorldPos - *p0;
				float dp = math::DotProduct(vertexToEye, *(normal + j));
				if (dp <= 0 && cullMode == CullMode::Back || dp >= 0 && cullMode == CullMode::Front)
				{
					vertexCullBuf[j] |= Cull_Face;
					--vertexRefBuf[index0];
					--vertexRefBuf[index1];
					--vertexRefBuf[index2];
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
		, transformValid_(false)
		, impl_(new Impl())
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
		transformValid_ = false;
	}

	const math::Matrix44f& Renderer::GetTransform(Transform::Type type) const
	{
		ASSERT(type < Transform::Max);
		return matrixs_[type];
	}

	void Renderer::UpdateTransform()
	{
		transformValid_ = true;

		MatrixMultiply(worldViewMatrix_, matrixs_[Transform::World], matrixs_[Transform::View]);
		MatrixMultiply(worldViewProjMatrix_, worldViewMatrix_, matrixs_[Transform::Projection]);
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
		eyeWorldPos_ = sceneManager_->GetActiveCamera()->GetWorldPosition();
		impl_->SetPrimitiveType(vertexBuffer_->GetPrimitiveType());
		impl_->IndexBuf(indexBuffer_);

		const int  vertexCount = vertexBuffer_->GetVertexCount();
		const int  triangleCount = (indexBuffer_ ? indexBuffer_->GetPrimitiveCount() : vertexCount / 3);
		const bool useVertexColor = (shadeMode_ != ShadeMode::WireFrame && shadeMode_ != ShadeMode::Constant || vertexBuffer->GetColors());
		const bool useTexture = (vertexBuffer->GetVertexType() & VertexType::TexCoord) && material_->GetTexture();
		const bool useLighting = (shadeMode_ != ShadeMode::WireFrame && shadeMode_ != ShadeMode::Constant);
		const bool useCull = (cullMode_ != CullMode::None);

		// counter
		impl_->ResetCounter(vertexCount, triangleCount);
		int* vertexRefBuf = impl_->VertexRefBuf();
		int* vertexCullBuf = impl_->VertexCullBuf();
		int* triangleCullBuf = impl_->TriangleCullBuf();

		// copy position to cache
		math::Vector3f* raw_position = vertexBuffer->GetPositions()->DataAs<math::Vector3f>();
		ASSERT(raw_position);
		auto& positions = impl_->AllocPositions(vertexCount);
		math::Vector4f* position = positions->DataAs<math::Vector4f>();
		for (int i = 0; i < vertexCount; ++i)
		{
			const auto& p = *(raw_position + i);
			*(position + i) = math::Vector4f(p.x, p.y, p.z, 1.0f);
		}

		// copy color to cache
		if (useVertexColor)
		{
			auto& colors = impl_->AllocColors(vertexCount);
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
		if (useLighting)
		{
			auto& normals = impl_->AllocNormals(vertexCount);
			const auto& raw_normals_ref = vertexBuffer->GetNormals();
			genVertexNormals = !raw_normals_ref;
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
		bool genTriangleNormals = false;
		if (useCull)
		{
			genTriangleNormals = !triangleNormals_;
			if (triangleNormals_)
			{
				auto& triangleNormals = impl_->AllocTriangleNormals(triangleCount);
				memcpy(triangleNormals->GetData(), triangleNormals_->GetData(), triangleCount * sizeof(math::Vector3f));
			}
		}
		
		// transform vertex from model to world
		impl_->Transform(matrixs_[Transform::World]);

		// generate triangle normals for remove back face
		// and generate vertex normals if need
		if (genTriangleNormals)
		{
			impl_->AllocTriangleNormals(triangleCount);
			impl_->GenTriangleNormals();
		}
		if (genVertexNormals)
		{
			// has allocated before
			impl_->GenVertexNormals();
		}

		// remove back face base on cull type
		if (useCull)
		{
			impl_->CullFace(eyeWorldPos_, cullMode_);
		}

		// clip by plane

		// transform vertex by WVP, only shared by at least one triangle

		// clip by CCW?

		// lighting for flat or gouraud shade mode here, phong will light in pixel shader

		// transform projected vertex to screen

		// draw triangle

		// end
	}
}