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
#include "DpRasterizer.h"

namespace dopixel
{
	DECLARE_ENUM(TriangleCull)
		Face = BIT(0),
		ClipPlane = BIT(1),
		Frustum = BIT(2)
	END_DECLARE_ENUM()

	DECLARE_ENUM(VertexCull)
		ClipPlane	= BIT(0),
		Frustum		= BIT(1)
	END_DECLARE_ENUM()

	DECLARE_ENUM(UsingStatus)
		VertexColor = BIT(0),
		Lighting	= BIT(1),
		Texture		= BIT(2),
		Cull		= BIT(3)
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

		void AllocVertexBuf(int vertexCount, bool copy);
		void AllocIndexBuf(int vertexCount, bool copy);
		void PrepareBuf(const VertexBufferRef& vertexBuf, const IndexBufferRef& indexBuf, const Ref<VertexArray3f>& triangleNormalsBuf, int usingStatus);
		void AllocCounter(int vertexCount, int triangleCount);
		void ResetCounter(int vertexCount, int triangleCount);
		void Transform(const math::Matrix44f& matrix, bool transNormal);
		void GenTriangleNormals();
		void GenVertexNormals();
		void GenNormals();
		void CullFace(const math::Vector3f& eyeWorldPos, CullMode::Type cullMode);
		void CullPlanes(const vector<math::Plane>& clipPlanes);
		void CutTriangle(const math::Plane& plane, int clipVertexCount, int i0, int i1, int i2);
		void Interpolate(int newIndex, int index0, int index1, float k);
		void ToCVV();
		void ToViewport(int viewportWidth, int viewportHeight);
		void DrawPrimitives(RasterizerRef& rasterizer, ShadeMode::Type shadeMode);
		void DrawPrimitive(RasterizerRef& rasterizer, ShadeMode::Type shadeMode, int index0, int index1, int index2);
	private:
		// cache buffer
		Ref<VertexArray4f> positions_;
		Ref<VertexArray3f> normals_;
		Ref<VertexArray3f> colors_;
		Ref<VertexArray2f> texCoords_;
		Ref<VertexArray3f> triangleNormals_;
		IndexBufferRef indexBuf_;

		int vertexCount_;
		int primitiveCount_;
		int primitiveType_;

		// counter
		vector<int> vertexRefBuf_;
		vector<int> vertexCullBuf_;
		vector<int> triangleCullBuff_;

		bool genVertexNormals_;
		bool genTriangleNormals_;

		int usingStatus_;
	};

	Renderer::Impl::Impl()
		: genVertexNormals_(false)
		, genTriangleNormals_(false)
		, vertexCount_(0)
		, primitiveCount_(0)
		, primitiveType_(PrimitiveType::Triangles)
		, usingStatus_(0)
	{
	}

	void Renderer::Impl::AllocIndexBuf(int primitiveCount, bool copy)
	{
		if (!indexBuf_)
			indexBuf_ = new IndexBuffer(primitiveType_, primitiveCount);
		else
			indexBuf_->Resize(primitiveCount);
		primitiveCount_ = primitiveCount;
	}

	void Renderer::Impl::AllocVertexBuf(int vertexCount, bool copy)
	{
		AllocPositions(vertexCount, copy);
		if (usingStatus_ & UsingStatus::VertexColor)
			AllocColors(vertexCount, copy);
		if (usingStatus_ & UsingStatus::Lighting)
			AllocNormals(vertexCount);
		if (usingStatus_ & UsingStatus::Texture)
			AllocTexCoords(vertexCount);
	}
	
	void Renderer::Impl::PrepareBuf(const VertexBufferRef& vertexBuf, const IndexBufferRef& indexBuf, const Ref<VertexArray3f>& triangleNormalsBuf, int usingStatus)
	{
		ASSERT(vertexBuf);

		usingStatus_ = usingStatus;
		vertexCount_ = vertexBuf->GetVertexCount();
		primitiveCount_ = (indexBuf ? indexBuf->GetPrimitiveCount() : vertexBuf->GetPrimitiveCount());
		
		AllocVertexBuf(vertexCount_, false);
	
		// copy indices to cache
		if (indexBuf)
		{
			AllocIndexBuf(primitiveCount_, false);
			*indexBuf_ = *indexBuf;
		}

		// copy position to cache
		math::Vector3f* raw_position = vertexBuf->GetPositions()->DataAs<math::Vector3f>();
		ASSERT(raw_position);
		math::Vector4f* position = positions_->DataAs<math::Vector4f>();
		for (int i = 0; i < vertexCount_; ++i)
		{
			const auto& p = *(raw_position + i);
			*(position + i) = math::Vector4f(p.x, p.y, p.z, 1.0f);
		}

		// copy color to cache
		if (usingStatus_ & UsingStatus::VertexColor)
		{
			math::Vector3f* color = colors_->DataAs<math::Vector3f>();
			const auto& raw_colors = vertexBuf->GetColors();
			if (raw_colors)
			{
				memcpy(color, raw_colors->GetData(), vertexCount_ * sizeof(math::Vector3f));
			}
			else
			{
				math::Vector3f v(1.0f, 1.0f, 1.0f);
				math::Vector3f* color = colors_->DataAs<math::Vector3f>();
				for (int i = 0; i < vertexCount_; ++i)
				{
					*(color + i) = v;
				}
			}
		}

		// copy normal to cache if have
		genVertexNormals_ = false;
		if (usingStatus_ & UsingStatus::Lighting)
		{
			const auto& raw_normals_ref = vertexBuf->GetNormals();
			genVertexNormals_ = !raw_normals_ref;
			if (!genVertexNormals_)
			{
				math::Vector3f* raw_normal = raw_normals_ref->DataAs<math::Vector3f>();
				math::Vector4f* normal = normals_->DataAs<math::Vector4f>();
				for (int i = 0; i < vertexCount_; ++i)
				{
					const auto& n = *(raw_normal + i);
					*(normal + i) = math::Vector4f(n.x, n.y, n.z, 1.0f);
				}
			}
		}

		// copy texture coords
		if (usingStatus_ & UsingStatus::Texture)
		{
			math::Vector2f* texCoord = texCoords_->DataAs<math::Vector2f>();
			const auto& raw_texCoords = vertexBuf->GetTexCoords();
			if (raw_texCoords)
			{
				memcpy(texCoord, raw_texCoords->GetData(), vertexCount_ * sizeof(math::Vector2f));
			}
		}

		// copy triangle normal to cache if have
		genTriangleNormals_ = false;
		if (usingStatus_ & UsingStatus::Cull)
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

	void Renderer::Impl::Transform(const math::Matrix44f& matrix, bool transNormal)
	{
		ASSERT(positions_);

		math::Vector4f* position = positions_->DataAs<math::Vector4f>();
		for (int i = 0; i < vertexCount_; ++i)
		{
			if (vertexRefBuf_[i] == 0)
				continue;
			math::Vector4f& p = *(position + i);
			p *= matrix;
		}

		if (transNormal && (triangleNormals_ || normals_))
		{
			math::Matrix44f matrixINVT;
			math::MatrixInverse(matrixINVT, matrix);
			matrixINVT.Transpose();

			if (triangleNormals_)
			{
				math::Vector3f* triangleNormals = triangleNormals_->DataAs<math::Vector3f>();
				for (int i = 0; i < primitiveCount_; ++i)
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
				for (int i = 0; i < vertexCount_; ++i)
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

	void Renderer::Impl::AllocCounter(int vertexCount, int triangleCount)
	{
		VertexRefBuf(vertexCount);
		VertexCullBuf(vertexCount);
		TriangleCullBuf(triangleCount);
	}

	void Renderer::Impl::ResetCounter(int vertexCount, int triangleCount)
	{
		AllocCounter(vertexCount, triangleCount);

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
				unsigned int* indices = indexBuf_->GetData();
				int indexCount = indexBuf_->GetIndexCount();
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
					CutTriangle(plane, clipVertexCount, index0, index1, index2);
					indices = indexBuf_->GetData();
					indexCount = indexBuf_->GetIndexCount();
				}
			}
			else
			{

			}
		}
	}

	void Renderer::Impl::CutTriangle(const math::Plane& plane, int clipVertexCount, int i0, int i1, int i2)
	{
		// resize buffer for add 2 vertex and 1 or 2 triangles, 
		// since we marked old vertex and triangle as clipped
		vertexCount_ += 2;
		primitiveCount_ += (clipVertexCount == 1 ? 2 : 1);
		AllocVertexBuf(vertexCount_, true);
		AllocIndexBuf(primitiveCount_, true);
		AllocCounter(vertexCount_, primitiveCount_);

		// sort vertex
		int triangle[3] = { i0, i1, i2 };
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
			if (vertexCullBuf_[i0] != 0)
			{
				// [i0]-i1-i2
				swap_t(triangle[0], triangle[1]);
				swap_t(triangle[0], triangle[2]);
			}
			else if (vertexCullBuf_[i2] != 0)
			{
				//i0-i1-[i2]
				swap_t(triangle[0], triangle[1]);
				swap_t(triangle[1], triangle[2]);
			}
		}
		else
		{
			// for a e.g. triangle: i0-[i1]-[i2], we will generate 2 vertex as i3, i4
			// i3 <- interpolate between i0 and i1
			// i4 <- interpolate between i0 and i2
			// then new triangle: i0-i3-i4

			int triangle[3] = { i0, i1, i2 };
			if (vertexCullBuf_[i1] != 0)
			{
				if (vertexCullBuf_[i0] != 0)
				{
					// [i0]-[i1]-i2
					swap_t(triangle[0], triangle[2]);
					swap_t(triangle[1], triangle[2]);
				}
			}
			else ASSERT(0);
		}

		// index and counter of new triangle index
		int i3 = vertexCount_ - 2;
		int i4 = vertexCount_ - 1;
		auto* indices = indexBuf_->GetData();

		if (clipVertexCount == 1)
		{
			int t1 = primitiveCount_ - 2;
			int t2 = primitiveCount_ - 1;
			auto* p1 = indices + t1 * 3;
			auto* p2 = indices + t2 * 3;
			p1[0] = triangle[0]; p1[1] = i3; p1[2] = triangle[2];
			p2[0] = i3; p2[1] = i4; p2[2] = triangle[2];

			triangleCullBuff_[t1] = 0;
			triangleCullBuff_[t2] = 0;
			vertexRefBuf_[triangle[0]] += 1;
			vertexRefBuf_[triangle[2]] += 2;
			vertexRefBuf_[i3] = 2;
			vertexRefBuf_[i4] = 1;
			vertexCullBuf_[i3] = 0;
			vertexCullBuf_[i4] = 0;
		}
		else
		{
			int t1 = primitiveCount_ - 1;
			auto* p1 = indices + t1 * 3;
			p1[0] = triangle[0]; p1[1] = i3; p1[2] = i4;

			triangleCullBuff_[t1] = 0;
			vertexRefBuf_[i3] = 1;
			vertexRefBuf_[i4] = 1;
			vertexCullBuf_[i3] = 0;
			vertexCullBuf_[i4] = 0;
		}

		// interpolate
		math::Vector4f* pos = positions_->DataAs<math::Vector4f>();
		math::Vector4f& pos0 = pos[triangle[0]];
		math::Vector4f& pos1 = pos[triangle[1]];
		math::Vector4f& pos2 = pos[triangle[2]];
		float d0 = fabs(plane.Distance(pos0.DivW()));
		float d1 = fabs(plane.Distance(pos1.DivW()));
		float d2 = fabs(plane.Distance(pos2.DivW()));

		if (clipVertexCount == 1)
		{
			// i3: i3 = i0 + k * i0i1
			float k3 = d0 / (d0 + d1);
			Interpolate(i3, i0, i1, k3);
			// i4: i4 = i2 + k * i2i1
			float k4 = d2 / (d1 + d2);
			Interpolate(i4, i2, i1, k4);

		}
		else
		{
			// i3: i3 = i0 + k * i0i1
			float k3 = d0 / (d0 + d1);
			Interpolate(i3, i0, i1, k3);
			// i4: i4 = i0 + k * i0i2
			float k4 = d0 / (d0 + d2);
			Interpolate(i4, i0, i2, k4);
		}
	}

	void Renderer::Impl::Interpolate(int newIndex, int index0, int index1, float k)
	{
		// pos
		math::Vector4f* pos = positions_->DataAs<math::Vector4f>();
		pos[newIndex] = pos[index0] + (pos[index1] - pos[index0]) * k;
		// normal
		if (usingStatus_ & UsingStatus::Lighting)
		{
			math::Vector3f* normal = normals_->DataAs<math::Vector3f>();
			normal[newIndex] = normal[index0] + (normal[index1] - normal[index0]) * k;
		}
		// color
		if (usingStatus_ & UsingStatus::VertexColor)
		{
			math::Vector3f* color = colors_->DataAs<math::Vector3f>();
			color[newIndex] = color[index0] + (color[index1] - color[index0]) * k;
		}
		// texture coords
		if (usingStatus_ & UsingStatus::Texture)
		{
			math::Vector2f* texCoord = texCoords_->DataAs<math::Vector2f>();
			texCoord[newIndex] = texCoord[index0] + (texCoord[index1] - texCoord[index0]) * k;
		}
	}

	void Renderer::Impl::ToCVV()
	{
		math::Vector4f* pos = positions_->DataAs<math::Vector4f>();
		for (int i = 0; i < vertexCount_; ++i)
		{
			if (vertexRefBuf_[i] == 0)
				continue;
			math::Vector4f* p = pos + i;
			ASSERT(!math::Equal(p->w, 0.0f));
			(*p) /= p->w;
			if (p->x < -1 || p->x > 1 ||
				p->y < -1 || p->y > 1 ||
				p->z < 0 || p->z > 1)
			{
				vertexCullBuf_[i] |= VertexCull::Frustum;
			}
		}

		if (indexBuf_)
		{
			unsigned int* indices = indexBuf_->GetData();
			int indexCount = indexBuf_->GetIndexCount();
			for (int i = 0, j = 0; i < indexCount; i += 3, ++j)
			{
				if (triangleCullBuff_[j])
					continue;

				auto index0 = *(indices + i);
				auto index1 = *(indices + i + 1);
				auto index2 = *(indices + i + 2);
				if (vertexCullBuf_[index0] && vertexCullBuf_[index1] && vertexCullBuf_[index2])
				{
					triangleCullBuff_[j] |= TriangleCull::Frustum;
					--vertexRefBuf_[index0];
					--vertexRefBuf_[index1];
					--vertexRefBuf_[index2];
				}
			}
		}
	}

	void Renderer::Impl::ToViewport(int viewportWidth, int viewportHeight)
	{
		// assume the coordinate of object is normalized, range is [-1,1]
		// then scale it by viewport, and reverse Y axis

		// results: 
		// x: [0, viewportWidth]
		// y: [0, viewportHeight]

		// so:
		// x -> (x + 1) * viewportWidth / 2
		// y -> viewportHeight - (y + 1) * viewportHeight / 2

		math::Vector4f* pos = positions_->DataAs<math::Vector4f>();
		for (int i = 0; i < vertexCount_; ++i)
		{
			if (vertexRefBuf_[i] == 0)
				continue;
			math::Vector4f* p = pos + i;
			p->x = (p->x + 1) * viewportWidth / 2;
			p->y = (1 - p->y) * viewportHeight / 2;
		}
	}

	void Renderer::Impl::DrawPrimitives(RasterizerRef& rasterizer, ShadeMode::Type shadeMode)
	{
		if (indexBuf_)
		{
			unsigned int* indices = indexBuf_->GetData();
			int indexCount = indexBuf_->GetIndexCount();
			for (int i = 0, j = 0; i < indexCount; i += 3, ++j)
			{
				if (triangleCullBuff_[j])
					continue;
				auto index0 = *(indices + i);
				auto index1 = *(indices + i + 1);
				auto index2 = *(indices + i + 2);
				DrawPrimitive(rasterizer, shadeMode, index0, index1, index2);
			}
		}
		else
		{
			for (int i = 0, j = 0; i < vertexCount_; i += 3, ++j)
			{
				if (triangleCullBuff_[j])
					continue;
				DrawPrimitive(rasterizer, shadeMode, i, i + 1, i + 2);
			}
		}
	}

	void Renderer::Impl::DrawPrimitive(RasterizerRef& rasterizer, ShadeMode::Type shadeMode, int index0, int index1, int index2)
	{
		auto pos = positions_->DataAs<math::Vector4f>();
		auto p0 = pos + index0;
		auto p1 = pos + index1;
		auto p2 = pos + index2;

		auto color = colors_->DataAs<math::Vector3f>();
		auto c0 = color + index0;
		auto c1 = color + index1;
		auto c2 = color + index2;
		if (shadeMode == ShadeMode::Wireframe)
		{
			rasterizer->DrawFrameTriangle(int(p0->x), int(p0->y), int(p1->x), int(p1->y),
				int(p2->x), int(p2->y), Color(*c0));
		}
	}

	//////////////////////////////////////////////////////////////////////////

	Renderer::Renderer()
		: shadeMode_(ShadeMode::WireFrame)
		, cullMode_(CullMode::Back)
		, zbufType_(ZBuffer::None)
		, zfunc_(Condition::LessThan)
		, textureWrap_(TextureWrap::ClampToEdge)
		, textureFilter_(TextureFilter::Linear)
		, frameBuf_(nullptr)
		, width_(0)
		, height_(0)
		, pitch_(0)
		, transformValid_(false)
		, impl_(new Impl())
		, rasterizer_(new Rasterizer())
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

		rasterizer_->SetBuffer(frameBuf_, width_, height_, pitch_, zbuf_.empty() ? nullptr : &zbuf_[0]);
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

	void Renderer::SetCameraNode(CameraSceneNodeRef cameraNode)
	{
		cameraNode_ = cameraNode;
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

		math::MatrixMultiply(worldViewMatrix_, matrixs_[Transform::World], matrixs_[Transform::View]);
		math::MatrixMultiply(worldViewProjMatrix_, worldViewMatrix_, matrixs_[Transform::Projection]);
		math::MatrixMultiply(viewProjMatrix_, matrixs_[Transform::View], matrixs_[Transform::Projection]);
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
		if (!transformValid_)
		{
			UpdateTransform();
		}
		auto cameraNode = cameraNode_;
		eyeWorldPos_ = cameraNode->GetWorldPosition();
		viewFrustum_ = cameraNode->GetViewFrustum();
		impl_->SetPrimitiveType(vertexBuffer_->GetPrimitiveType());

		// using status
		int usingStatus = 0;
		if (shadeMode_ != ShadeMode::WireFrame && shadeMode_ != ShadeMode::Constant || vertexBuffer->GetColors())
			usingStatus |= UsingStatus::VertexColor;
		if ((vertexBuffer->GetVertexType() & VertexType::TexCoord) && material_->GetTexture())
			usingStatus |= UsingStatus::Texture;
		if (shadeMode_ != ShadeMode::WireFrame && shadeMode_ != ShadeMode::Constant)
			usingStatus |= UsingStatus::Lighting;
		if (cullMode_ != CullMode::None && vertexBuffer_->GetPrimitiveType() == PrimitiveType::Triangles)
			usingStatus |= UsingStatus::Cull;

		const int  vertexCount = vertexBuffer_->GetVertexCount();
		const int  triangleCount = (indexBuffer_ ? indexBuffer_->GetPrimitiveCount() : vertexCount / 3);

		// counter
		impl_->ResetCounter(vertexCount, triangleCount);

		// prepare buffer
		impl_->PrepareBuf(vertexBuffer_, indexBuffer_, triangleNormalsBuf_, usingStatus);

		// transform vertex from model to world
		impl_->Transform(matrixs_[Transform::World], true);

		// generate triangle normals for remove back face and vertex normals if need
		impl_->GenNormals();

		// remove back face base on cull type
		if (usingStatus & UsingStatus::Cull)
		{
			impl_->CullFace(eyeWorldPos_, cullMode_);
		}

		// clip by plane, planes are in  world space yet, include near z plane.
		vector<math::Plane> planes = clipPlanes_;
		planes.push_back(viewFrustum_.GetPlane(math::Frustum::PlaneNear));
		impl_->CullPlanes(planes);

		// lighting for flat or gouraud shade mode here, phong will light in pixel shader

		// transform vertex to projection space
		impl_->Transform(viewProjMatrix_, false);

		// clip by CVV
		impl_->ToCVV();

		// transform projected vertex to viewport
		impl_->ToViewport(width_, height_);

		// draw triangle
		impl_->DrawPrimitives(rasterizer_, shadeMode_);

		// end
	}
}