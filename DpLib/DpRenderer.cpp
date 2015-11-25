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
	DECLARE_ENUM(TriangleCull)
		Face = BIT(0),
		ClipPlane = BIT(1),
		Frustum = BIT(2)
	END_DECLARE_ENUM()

	DECLARE_ENUM(VertexCull)
		ClipPlane = BIT(0),
		Frustum = BIT(1)
	END_DECLARE_ENUM()


	class Renderer::Impl
	{
	public:
		Impl();
		virtual ~Impl() {}

		void SetPrimitiveType(int primitiveType) { primitiveType_ = primitiveType; }

		Ref<VertexArray4f>& Posotions() { return positions_; }
		Ref<VertexArray3f>& Normals() { return normals_; }
		Ref<VertexArray3f>& Colors() { return colors_; }
		Ref<VertexArray3f>& TriangleNormals() { return triangleNormals_; }

#define ALLOC(ref, T, count, copy) { \
		if (!ref) \
			ref = new T(count); \
		else \
			ref->SetVertexCount(count, copy); \
		return ref; \
		}

		Ref<VertexArray4f>& AllocPositions(int count, bool copy = false) { ALLOC(positions_, VertexArray4f, count, copy); }
		Ref<VertexArray3f>& AllocNormals(int count, bool copy = false) { ALLOC(normals_, VertexArray3f, count, copy); }
		Ref<VertexArray3f>& AllocColors(int count, bool copy = false) { ALLOC(colors_, VertexArray3f, count, copy); }
		Ref<VertexArray2f>& AllocTexCoords(int count, bool copy = false) { ALLOC(texCoords_, VertexArray2f, count, copy); }
		Ref<VertexArray3f>& AllocTriangleNormals(int count, bool copy = false) { ALLOC(triangleNormals_, VertexArray3f, count, copy); }

		int* VertexRefBuf() { return &vertexRefBuf_[0]; }
		int* VertexCullBuf() { return &vertexCullBuf_[0]; }
		int* TriangleCullBuf() { return &triangleCullBuff_[0]; }
		int* VertexRefBuf(int count) { vertexRefBuf_.resize(count); return &vertexRefBuf_[0];}
		int* VertexCullBuf(int count) { vertexCullBuf_.resize(count); return &vertexCullBuf_[0]; }
		int* TriangleCullBuf(int count) { triangleCullBuff_.resize(count); return &triangleCullBuff_[0];}

		void PrepareBuf(const VertexBufferRef& vertexBuf, const IndexBufferRef& indexBuf, const Ref<VertexArray3f>& triangleNormalsBuf, 
			bool useVertexColor, bool useLighting, bool useTexture, bool useCull);
		void ResetCounter(int vertexCount, int triangleCount);
		void Transform(const math::Matrix44f& matrix, const math::Matrix44f& matrixINVT);
		void GenTriangleNormals();
		void GenVertexNormals();
		void GenNormals();
		void CullFace(const math::Vector3f& eyeWorldPos, CullMode::Type cullMode);
		void CullPlanes(const vector<math::Plane>& clipPlanes);
		void CutTriangle(int clipVertexCount, int i0, int i1, int i2);
	private:
		// cache buffer
		Ref<VertexArray4f> positions_;
		Ref<VertexArray3f> normals_;
		Ref<VertexArray3f> colors_;
		Ref<VertexArray2f> texCoords_;
		Ref<VertexArray3f> triangleNormals_;
		IndexBufferRef indexBuf_;

		bool genVertexNormals_;
		bool genTriangleNormals_;

		int vertexCount_;
		int primitiveCount_;
		int primitiveType_;

		// counter
		vector<int> vertexRefBuf_;
		vector<int> vertexCullBuf_;
		vector<int> triangleCullBuff_;
	};

	Renderer::Impl::Impl()
		: genVertexNormals_(false)
		, genTriangleNormals_(false)
		, vertexCount_(0)
		, primitiveCount_(0)
		, primitiveType_(PrimitiveType::Triangles)
	{
	}
	
	void Renderer::Impl::PrepareBuf(const VertexBufferRef& vertexBuf, const IndexBufferRef& indexBuf, const Ref<VertexArray3f>& triangleNormalsBuf, 
		bool useVertexColor, bool useLighting, bool useTexture, bool useCull)
	{
		ASSERT(vertexBuf);

		indexBuf_ = Ref<IndexBuffer>(new IndexBuffer(*indexBuf));
		vertexCount_ = vertexBuf->GetVertexCount();
		primitiveCount_ = (indexBuf ? indexBuf->GetPrimitiveCount() : vertexBuf->GetPrimitiveCount());

		// copy position to cache
		math::Vector3f* raw_position = vertexBuf->GetPositions()->DataAs<math::Vector3f>();
		ASSERT(raw_position);
		auto& positions = AllocPositions(vertexCount_);
		math::Vector4f* position = positions->DataAs<math::Vector4f>();
		for (int i = 0; i < vertexCount_; ++i)
		{
			const auto& p = *(raw_position + i);
			*(position + i) = math::Vector4f(p.x, p.y, p.z, 1.0f);
		}

		// copy color to cache
		if (useVertexColor)
		{
			auto& colors = AllocColors(vertexCount_);
			math::Vector3f* color = colors->DataAs<math::Vector3f>();
			const auto& raw_colors = vertexBuf->GetColors();
			if (raw_colors)
			{
				memcpy(color, raw_colors->GetData(), vertexCount_ * sizeof(math::Vector3f));
			}
			else
			{
				math::Vector3f v(1.0f, 1.0f, 1.0f);
				math::Vector3f* color = colors->DataAs<math::Vector3f>();
				for (int i = 0; i < vertexCount_; ++i)
				{
					*(color + i) = v;
				}
			}
		}

		// copy normal to cache if have
		genVertexNormals_ = false;
		if (useLighting)
		{
			auto& normals = AllocNormals(vertexCount_);
			const auto& raw_normals_ref = vertexBuf->GetNormals();
			genVertexNormals_ = !raw_normals_ref;
			if (!genVertexNormals_)
			{
				math::Vector3f* raw_normal = raw_normals_ref->DataAs<math::Vector3f>();
				math::Vector4f* normal = normals->DataAs<math::Vector4f>();
				for (int i = 0; i < vertexCount_; ++i)
				{
					const auto& n = *(raw_normal + i);
					*(normal + i) = math::Vector4f(n.x, n.y, n.z, 1.0f);
				}
			}
		}

		// copy texture coords
		if (useTexture)
		{
			auto& texCoords = AllocTexCoords(vertexCount_);
			math::Vector2f* texCoord = texCoords->DataAs<math::Vector2f>();
			const auto& raw_texCoords = vertexBuf->GetTexCoords();
			if (raw_texCoords)
			{
				memcpy(texCoord, raw_texCoords->GetData(), vertexCount_ * sizeof(math::Vector2f));
			}
		}

		// copy triangle normal to cache if have
		genTriangleNormals_ = false;
		if (useCull)
		{
			genTriangleNormals_ = !triangleNormalsBuf;
			if (triangleNormalsBuf)
			{
				auto& triangleNormals = AllocTriangleNormals(primitiveCount_);
				memcpy(triangleNormals->GetData(), triangleNormalsBuf->GetData(), primitiveCount_ * sizeof(math::Vector3f));
			}
		}
	}

	void Renderer::Impl::GenTriangleNormals()
	{
		ASSERT(positions_ && primitiveType_ == PrimitiveType::Triangles);
		ASSERT(triangleNormals_);

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
			for (int i = 0, j = 0; i < vertexCount_; i += 3, ++j)
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

	void Renderer::Impl::GenNormals()
	{
		// generate triangle normals for remove back face and vertex normals if need
		if (genTriangleNormals_)
		{
			AllocTriangleNormals(primitiveCount_);
			GenTriangleNormals();
		}
		if (genVertexNormals_)
		{
			AllocTriangleNormals(vertexCount_);
			GenVertexNormals();
		}
	}

	void Renderer::Impl::Transform(const math::Matrix44f& matrix, const math::Matrix44f& matrixINVT)
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
					triangleCullBuff_[j] |= TriangleCull::Face;
					--vertexRefBuf_[index0];
					--vertexRefBuf_[index1];
					--vertexRefBuf_[index2];
				}
			}
		}
		else
		{
			for (int i = 0, j = 0; i < vertexCount_; i += 3, ++j)
			{
				auto index0 = i;
				auto index1 = i + 1;
				auto index2 = i + 2;

				math::Vector3f* p0 = (math::Vector3f*)(position + index0);
				math::Vector3f vertexToEye = eyeWorldPos - *p0;
				float dp = math::DotProduct(vertexToEye, *(normal + j));
				if (dp <= 0 && cullMode == CullMode::Back || dp >= 0 && cullMode == CullMode::Front)
				{
					triangleCullBuff_[j] |= TriangleCull::Face;
					--vertexRefBuf_[index0];
					--vertexRefBuf_[index1];
					--vertexRefBuf_[index2];
				}
			}
		}
	}

	void Renderer::Impl::CullPlanes(const vector<math::Plane>& clipPlanes)
	{
		math::Vector4f* position = positions_->DataAs<math::Vector4f>();

		for (const auto& plane : clipPlanes)
		{
			// mark vertex culled by clip plane
			for (int i = 0; i < vertexCount_; ++i)
			{
				if (vertexRefBuf_[i] == 0)
					continue;

				math::Vector4f* pos = position + i;
				if (plane.Distance(pos->DivW()) < 0)
				{
					vertexCullBuf_[i] |= VertexCull::ClipPlane;
				}
			}

			if (indexBuf_)
			{
				const unsigned int* indices = indexBuf_->GetData();
				const int indexCount = indexBuf_->GetIndexCount();
				for (int i = 0, j = 0; i < indexCount; i += 3, ++j)
				{
					if (triangleCullBuff_[j] != 0)
						continue;

					auto index0 = *(indices + i);
					auto index1 = *(indices + i + 1);
					auto index2 = *(indices + i + 2);

					int clipVertexCount = 0;
					if (vertexCullBuf_[index0] != 0) ++clipVertexCount;
					if (vertexCullBuf_[index1] != 0) ++clipVertexCount;
					if (vertexCullBuf_[index2] != 0) ++clipVertexCount;

					if (clipVertexCount == 0)
						continue;

					triangleCullBuff_[j] |= TriangleCull::ClipPlane;
					--vertexRefBuf_[index0];
					--vertexRefBuf_[index1];
					--vertexRefBuf_[index2];

					if (clipVertexCount == 3)
						continue;
					// this triangle need cut to 1 or 2 triangles
				}
			}
			else
			{

			}
		}
	}

	void Renderer::Impl::CutTriangle(int clipVertexCount, int i0, int i1, int i2)
	{
		if (clipVertexCount == 1)
		{
			// for a e.g. triangle: i0-[i1]-i2, which "[i1]" means i1 has been cut
			// we will generate 2 vertex as i3, i4 by interpolate between i1 with rest vertex.

			// how generate 2 vertex?
			// interpolate between i1 and i0 generate i3, then get triangle 1: i0-i3-i2
			// interpolate between i1 and i2 generate i4, then get triangle 2: i3-i4-i2

			// but if other vertex has been cut, how we do it conveniently except enumerate
			// we could swap it as follow:
			// [i0]-i1-i2 -> i0-[i1]-i2
			// i0-i1-[i2] -> i0-[i1]-i2

			int triangle[3] = { i0, i1, i2 };

			// [i0]-i1-i2
			if (vertexCullBuf_[i0] != 0)
			{
				swap_t(triangle[i0], triangle[i1]);
				swap_t(triangle[i0], triangle[i2]);
			}
			//i0-i1-[i2]
			else if (vertexCullBuf_[i2] != 0)
			{
				swap_t(triangle[i0], triangle[i1]);
				swap_t(triangle[i1], triangle[i2]);
			}

			// then we get i0-[i1]-i2

			// resize buffer for add 2 vertex and 2 triangle, since we marked old vertex and triangle as clipped

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

	void Renderer::AddClipPlane(const math::Plane& plane)
	{
		clipPlanes_.push_back(plane);
	}

	void Renderer::ClearClipPlane()
	{
		clipPlanes_.clear();
	}

	void Renderer::RenderSubMesh(const SubMeshRef& submesh)
	{
		triangleNormalsBuf_ = submesh->GetTriangleNormals();
		DrawPrimitive(submesh->GetVertexBuffer(), submesh->GetIndexBuffer(), submesh->GetMaterial());
		triangleNormalsBuf_ = nullptr;
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
		auto cameraNode = sceneManager_->GetActiveCamera();
		eyeWorldPos_ = cameraNode->GetWorldPosition();
		viewFrustum_ = cameraNode->GetViewFrustum();
		impl_->SetPrimitiveType(vertexBuffer_->GetPrimitiveType());

		const int  vertexCount = vertexBuffer_->GetVertexCount();
		const int  triangleCount = (indexBuffer_ ? indexBuffer_->GetPrimitiveCount() : vertexCount / 3);
		const bool useVertexColor = (shadeMode_ != ShadeMode::WireFrame && shadeMode_ != ShadeMode::Constant || vertexBuffer->GetColors());
		const bool useTexture = (vertexBuffer->GetVertexType() & VertexType::TexCoord) && material_->GetTexture();
		const bool useLighting = (shadeMode_ != ShadeMode::WireFrame && shadeMode_ != ShadeMode::Constant);
		const bool useCull = (cullMode_ != CullMode::None && vertexBuffer_->GetPrimitiveType() == PrimitiveType::Triangles);

		// counter
		impl_->ResetCounter(vertexCount, triangleCount);
		int* vertexRefBuf = impl_->VertexRefBuf();
		int* vertexCullBuf = impl_->VertexCullBuf();
		int* triangleCullBuf = impl_->TriangleCullBuf();

		// prepare buffer
		impl_->PrepareBuf(vertexBuffer_, indexBuffer_, triangleNormalsBuf_, useVertexColor, useLighting, useTexture, useCull);

		// transform vertex from model to world
		math::Matrix44f matrixINVT;
		math::MatrixInverse(matrixINVT, matrixs_[Transform::World]);
		matrixINVT.Transpose();
		impl_->Transform(matrixs_[Transform::World], matrixINVT);

		// generate triangle normals for remove back face and vertex normals if need
		impl_->GenNormals();

		// remove back face base on cull type
		if (useCull)
		{
			impl_->CullFace(eyeWorldPos_, cullMode_);
		}

		// clip by plane, planes are in  world space yet, include near z plane.
		vector<math::Plane> planes = clipPlanes_;
		planes.push_back(viewFrustum_.GetPlane(math::Frustum::PlaneNear));
		impl_->CullPlanes(planes);

		// transform vertex by WVP, only shared by at least one triangle

		// clip by CCW?

		// lighting for flat or gouraud shade mode here, phong will light in pixel shader

		// transform projected vertex to screen

		// draw triangle

		// end
	}
}