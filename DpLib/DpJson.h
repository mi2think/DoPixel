/********************************************************************
	created:	2015/08/08
	created:	8:8:2015   21:37
	filename: 	D:\OneDrive\3D\DpLib\DpLib\DpJson.h
	file path:	D:\OneDrive\3D\DpLib\DpLib
	file base:	DpJson
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Json Wrapper
*********************************************************************/
#ifndef __EDGE_JSON_WRAPPER__
#define __EDGE_JSON_WRAPPER__

#include <string>
#include <utility>

namespace DoPixel
{
	namespace Core
	{
		class JsonArray;

		class JsonArchive
		{
		public:
			JsonArchive();
			~JsonArchive();

			JsonArchive& operator<< (std::pair<const char*, const char*> var);
			JsonArchive& operator<< (std::pair<const char*, const std::string*> var);
			JsonArchive& operator<< (std::pair<const char*, const bool*> var);
			JsonArchive& operator<< (std::pair<const char*, const int*> var);
			JsonArchive& operator<< (std::pair<const char*, const unsigned int*> var);
			JsonArchive& operator<< (std::pair<const char*, const long long*> var);
			JsonArchive& operator<< (std::pair<const char*, const unsigned long long*> var);
			JsonArchive& operator<< (std::pair<const char*, const float*> var);
			JsonArchive& operator<< (std::pair<const char*, const double*> var);
			JsonArchive& operator<< (std::pair<const char*, const JsonArchive*> var);
			JsonArchive& operator<< (std::pair<const char*, const JsonArray*> var);

			// T requires: JsonArchive& Serialize(JsonArchive&);
			template<typename T> JsonArchive& operator<< (std::pair<const char*, const T*> var)
			{
				JsonArchive jsonArchive;
				var.second->Serialize(jsonArchive);
				*this << std::pair<const char*, const JsonArchive*>(var.first, &jsonArchive);
				return *this;
			}

			JsonArchive& operator>> (std::pair<const char*, std::string*> var);
			JsonArchive& operator>> (std::pair<const char*, bool*> var);
			JsonArchive& operator>> (std::pair<const char*, int*> var);
			JsonArchive& operator>> (std::pair<const char*, unsigned int*> var);
			JsonArchive& operator>> (std::pair<const char*, long long*> var);
			JsonArchive& operator>> (std::pair<const char*, unsigned long long*> var);
			JsonArchive& operator>> (std::pair<const char*, float*> var);
			JsonArchive& operator>> (std::pair<const char*, double*> var);
			JsonArchive& operator>> (std::pair<const char*, JsonArchive*> var);

			// T requires: JsonArchive& Deserialize(JsonArchive&);
			template<typename T> JsonArchive& operator>> (std::pair<const char*, T*> var)
			{
				JsonArchive jsonArchive;
				*this >> std::pair<const char*, JsonArchive*>(var.first, &jsonArchive);
				var.second->Deserialize(jsonArchive);
				return *this;
			}

			std::string GetJsonString() const;
			bool ParseJsonString(const char* s);
			bool ParseJsonString(const char* data, unsigned int dataSize);

			friend class JsonArray;
		private:
			class IMPL;
			IMPL* GetIMPL() const { return m_pImpl; }
			IMPL* m_pImpl;
		};

		template<typename T> inline std::pair<const char*, const T*> MakeJsonVar_IN(const char* name, const T& var) { return std::make_pair(name, &var); }
		inline std::pair<const char*, const char*> MakeJsonVar_IN(const char* name, const char* var) { return std::make_pair(name, var); }
		template<typename T> inline std::pair<const char*, T*> MakeJsonVar_OUT(const char* name, T& var) { return std::make_pair(name, &var); }

#define WHO_AM_I(VAR) (#VAR)
#define JVAR_IN(VAR) (MakeJsonVar_IN(WHO_AM_I(VAR),VAR))
#define JVAR_OUT(VAR) (MakeJsonVar_OUT(WHO_AM_I(VAR), VAR))
#define JVAR_IN_MANUAL(VARNAME, VAR) (MakeJsonVar_IN(VARNAME,VAR))
#define JVAR_OUT_MANUAL(VARNAME, VAR) (MakeJsonVar_OUT(VARNAME,VAR))

		//////////////////////////////////////////////////////////////////////////
		class JsonArray
		{
		public:
			JsonArray(JsonArchive* jsonArchive);
			~JsonArray();

			JsonArray& operator<< (const char* s);
			JsonArray& operator<< (const std::string& s);
			JsonArray& operator<< (bool b);
			JsonArray& operator<< (int i);
			JsonArray& operator<< (unsigned int u);
			JsonArray& operator<< (long long l);
			JsonArray& operator<< (unsigned long long ul);
			JsonArray& operator<< (float f);
			JsonArray& operator<< (double d);
			JsonArray& operator<< (const JsonArray& jsonArray);
			JsonArray& operator<< (const JsonArchive& jsonArchive);

			JsonArray& operator>> (std::string& s);
			JsonArray& operator>> (bool& b);
			JsonArray& operator>> (int& i);
			JsonArray& operator>> (unsigned int& u);
			JsonArray& operator>> (long long& l);
			JsonArray& operator>> (unsigned long long& ul);
			JsonArray& operator>> (float& f);
			JsonArray& operator>> (double& d);

			template<typename T, size_t size> JsonArray& operator>> (T (&arrayVal)[size])
			{
				for (size_t i = 0; i < size; ++i)
				{
					*this >> arrayVal[i];
				}
				return *this;
			}

			// Reset deserialize index to 0
			void ResetDeserialize();
			friend class JsonArchive;
		private:
			class IMPL;
			IMPL* GetIMPL() const { return m_pImpl; }
			IMPL* m_pImpl;
			JsonArchive* m_jsonArchive;
		};
	}
}

#endif