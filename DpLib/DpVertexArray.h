/********************************************************************
	created:	2015/11/14
	created:	14:11:2015   21:33
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpVertexArray.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpVertexArray
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Vertex Array
*********************************************************************/
#ifndef __DP_VERTEX_ARRAY_H__
#define __DP_VERTEX_ARRAY_H__

namespace dopixel
{
	namespace scene
	{
		template<typename T, int Count>
		class VertexArray
		{
		public:
			VertexArray(int vertexCount)
				: vertexCount_(vertexCount)
				, componentCount_(Count)
				, dataStride_(sizeof(T) * Count)
				, componentStride_(sizeof(T))
				, data_(new T[vertexCount])
			{}

			~VertexArray() { delete[] data_; }

			int GetVertexCount() const { return vertexCount_; }

			int GetComponentStride() const { return componentStride_; }

			int GetDataStride() const { return dataStride_; }

			const T* GetData() const { return data_; }
			T* GetData() { return data_; }

			template<typename U>
			U* As(int index = 0)
			{
				return reinterpret_cast<U*>(data_ + index * componentCount_);
			}
		private:
			// vertex count in array
			int vertexCount_;
			// vertex contain how many T?
			int componentCount_;
			// data stride, equal sizeof(T) * componentCount_
			int dataStride_;
			// sizeof(T)
			int componentStride_;

			T* data_;
		};
	}
}


#endif

