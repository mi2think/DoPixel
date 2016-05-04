/********************************************************************
	created:	2015/11/12
	created:	12:11:2015   19:19
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpVertexBuffer.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpVertexBuffer
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Vertex Buffer
*********************************************************************/
#ifndef __DP_VERTEX_BUFFER_H__
#define __DP_VERTEX_BUFFER_H__

#include "DoPixel.h"
#include "DpVertexArray.h"

namespace dopixel
{
	class VertexBuffer
	{
	public:
		VertexBuffer()
			: primitiveType_(PrimitiveType::Triangles)
			, vertexType_(VertexType::Position)
			, vertexCount_(0)
		{}

		VertexBuffer(int primitiveType)
			: primitiveType_(primitiveType)
			, vertexType_(VertexType::Position)
			, vertexCount_(0)
		{}

		int GetPrimitiveType() const { return primitiveType_; }
		int GetVertexType() const { return vertexType_; }
		int GetVertexCount() const { return vertexCount_; }
		int GetPrimitiveCount() const
		{
			int num = GetVertexNumByPerPrimitive(primitiveType_);
			return vertexCount_ / num;
		}

		void SetPositions(const Ref<VertexArray3f>& positions) { positions_ = positions; Internal(VertexType::Position); }
		void SetNormals(const Ref<VertexArray3f>& normals) { normals_ = normals; Internal(VertexType::Normal); }
		void SetColors(const Ref<VertexArray3f>& colors) { colors_ = colors; Internal(VertexType::Color); }
		void SetTexCoords(const Ref<VertexArray2f>& texCoords) { texCoords_ = texCoords; Internal(VertexType::TexCoord); }

		const Ref<VertexArray3f>& GetPositions() const { return positions_; }
		const Ref<VertexArray3f>& GetNormals() const { return normals_; }
		const Ref<VertexArray3f>& GetColors() const { return colors_; }
		const Ref<VertexArray2f>& GetTexCoords() const { return texCoords_; }
	private:
		void Internal(int flag)
		{
			vertexType_ |= flag;
			UpdateVertexCount();
		}
		void UpdateVertexCount()
		{
			if (positions_) { vertexCount_ = positions_->GetVertexCount(); return; }
			if (normals_) { vertexCount_ = normals_->GetVertexCount(); return; }
			if (colors_) { vertexCount_ = colors_->GetVertexCount(); return; }
			if (texCoords_) { vertexCount_ = texCoords_->GetVertexCount(); return; }
			vertexCount_ = 0;
		}

		// primitive type
		int primitiveType_;
		unsigned int vertexType_;
		unsigned int vertexCount_;

		Ref<VertexArray3f> positions_;
		Ref<VertexArray3f> normals_;
		Ref<VertexArray3f> colors_;
		Ref<VertexArray2f> texCoords_;
	};
}

#endif

