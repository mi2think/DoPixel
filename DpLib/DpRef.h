/********************************************************************
	created:	2015/05/23
	created:	23:5:2015   20:30
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpRef.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpRef
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Ref
*********************************************************************/

#ifndef __DP_REF__
#define __DP_REF__

namespace DoPixel
{
	namespace Core
	{
		template<typename T>
		class Ref
		{
		public:
			Ref() : ptr(nullptr), counter(nullptr)
			{

			}

			Ref(T* p) : ptr(p), counter(new int(0))
			{
				IncRef();
			}

			Ref(const Ref<T>& r) : ptr(r.ptr), counter(r.counter)
			{
				IncRef();
			}

			Ref(Ref<T>&& r) : ptr(r.ptr), counter(r.counter)
			{
				r.ptr = nullptr;
				r.counter = nullptr;
			}

			template<typename U>
			Ref(const Ref<U>& r) : ptr(r.ptr), counter(r.counter)
			{
				IncRef();
			}

			~Ref()
			{
				DecRef();
			}

			Ref<T>& operator=(const Ref<T>& r)
			{
				if (&r != this)
				{
					DecRef();
					ptr = r.ptr;
					counter = r.counter;
					IncRef();
				}
				return *this;
			}

			Ref<T>& operator=(T* p)
			{
				DecRef();
				if (p)
				{
					counter = new int(0);
					ptr = p;
					IncRef();
				}
				else
				{
					ptr = nullptr;
					counter = nullptr;
				}
				return *this;
			}

			Ref<T>& operator=(Ref<T>&& r)
			{
				if (&r != this)
				{
					DecRef();
					ptr = r.ptr;
					counter = r.counter;

					r.ptr = nullptr;
					r.counter = nullptr;
				}
				return *this;
			}

			template<typename U>
			Ref<T>& operator=(const Ref<U>& r)
			{
				DecRef();
				
				ptr = r.ptr;
				counter = r.counter;
				IncRef();
				return *this;
			}

			T& operator*() const { return *ptr; }
			T* operator->() const { return ptr; }
			operator bool() const { return ptr != nullptr; }
			T* Get() const { return ptr; }
			int Count() const { return counter ? *counter : 0; }

			template<typename U>
			Ref<U> As() const
			{
				U* p = dynamic_cast<U*>(ptr);
				return Ref<U>(p, (p ? counter : 0));
			}

			bool operator==(const Ref<T>& r) const
			{
				return ptr == r.ptr;
			}

			bool operator!=(const Ref<T>& r) const
			{
				return !operator==(r);
			}

			bool operator>(const Ref<T>& r) const
			{
				return ptr > r.ptr;
			}

			bool operator>=(const Ref<T>& r) const
			{
				return ptr >= r.ptr;
			}

			bool operator<(const Ref<T>& r) const
			{
				return !operator>=(r);
			}

			bool operator<=(const Ref<T>& r) const
			{
				return !operator>(r);
			}
		private:
			Ref(T* p, int* c) : ptr(p), counter(c)
			{
				IncRef();
			}

			void IncRef()
			{
				if (counter)
				{
					++(*counter);
				}
			}

			void DecRef()
			{
				if (counter)
				{
					if ((--(*counter)) == 0)
					{
						delete ptr;
						delete counter;
					}
				}
			}

			T* ptr;
			mutable int* counter;
		};
	}
}

#endif