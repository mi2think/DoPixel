/********************************************************************
	created:	2015/11/14
	created:	14:11:2015   23:18
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpIndexBuffer.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpIndexBuffer
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Index Buffer
*********************************************************************/
#ifndef __DP_INDEX_BUFFER_H__
#define __DP_INDEX_BUFFER_H__

#include "DoPixel.h"

namespace dopixel
{
	class IndexBuffer
	{
	public:
		IndexBuffer(int primitiveType, int primitiveCount)
			: primitiveType_(primitiveType_)
			, primitiveCount_(primitiveCount)
			, indices_(primitiveCount * GetVertexNumByPerPrimitive(primitiveType))
		{}
		// copy data
		IndexBuffer(const IndexBuffer& indexBuffer)
			: primitiveType_(indexBuffer.primitiveType_)
			, primitiveCount_(indexBuffer.primitiveCount_)
			, indices_(indexBuffer.indices_)
		{
		}
		IndexBuffer& operator=(const IndexBuffer& indexBuffer)
		{
			primitiveType_ = indexBuffer.primitiveType_;
			primitiveCount_ = indexBuffer.primitiveCount_;
			indices_ = indexBuffer.indices_;
			return *this;
		}

		int GetPrimitiveType() const { return primitiveType_; }
		int GetPrimitiveCount() const { return primitiveCount_; }

		int GetIndexCount() const { return indices_.size(); }

		unsigned int* GetData() { return &indices_[0]; }
		const unsigned int* GetData() const { return &indices_[0]; }

		void Resize(int primitiveCount)
		{
			if (primitiveCount != primitiveCount_)
			{
				primitiveCount_ = primitiveCount;
				int indicesCount = primitiveCount * GetVertexNumByPerPrimitive(primitiveType_);
				indices_.resize(indicesCount);
			}
		}
	private:
		// primitive type
		int primitiveType_;
		// primitive count
		int primitiveCount_;

		vector<unsigned int> indices_;
	};
}

#endif
