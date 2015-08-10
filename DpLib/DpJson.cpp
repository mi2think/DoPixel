#include "DpJson.h"
#include "DpRef.h"
#include <vector>

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"

namespace DoPixel
{
	namespace Core
	{
		class JsonArchive::IMPL
		{
		public:
			IMPL() {}
			~IMPL() {}

			rapidjson::Document* GetDoc() { return &doc; }
			void ParseJsonString(const char* s) { doc.Parse(s); }
		private:
			rapidjson::Document doc;
		};

		JsonArchive::JsonArchive()
			: m_pImpl(new IMPL())
		{
			m_pImpl->GetDoc()->SetObject();
		}

		JsonArchive::~JsonArchive()
		{
			delete m_pImpl;
		}

#define RAPIDJSON_ADDMEMBER(VAR) do { \
	auto doc = m_pImpl->GetDoc(); \
	rapidjson::Value name(VAR.first, doc->GetAllocator()); \
	doc->AddMember(name, *(VAR.second), doc->GetAllocator()); \
} while (0)

		JsonArchive& JsonArchive::operator<< (std::pair<const char*, const char*> var)
		{
			auto doc = m_pImpl->GetDoc();
			rapidjson::Value name(var.first, doc->GetAllocator());
			rapidjson::Value val(var.second, doc->GetAllocator());
			doc->AddMember(name, val, doc->GetAllocator());
			return *this;
		}

		JsonArchive& JsonArchive::operator<< (std::pair<const char*, const std::string*> var)
		{
			auto doc = m_pImpl->GetDoc();
			rapidjson::Value name(var.first, doc->GetAllocator());
			rapidjson::Value val(var.second->c_str(), doc->GetAllocator());
			doc->AddMember(name, val, doc->GetAllocator());
			return *this;
		}

		JsonArchive& JsonArchive::operator<< (std::pair<const char*, const bool*> var)
		{
			RAPIDJSON_ADDMEMBER(var);
			return *this;
		}

		JsonArchive& JsonArchive::operator<< (std::pair<const char*, const int*> var)
		{
			RAPIDJSON_ADDMEMBER(var);
			return *this;
		}

		JsonArchive& JsonArchive::operator<< (std::pair<const char*, const unsigned int*> var)
		{
			RAPIDJSON_ADDMEMBER(var);
			return *this;
		}

		JsonArchive& JsonArchive::operator<< (std::pair<const char*, const long long*> var)
		{
			RAPIDJSON_ADDMEMBER(var);
			return *this;
		}

		JsonArchive& JsonArchive::operator<< (std::pair<const char*, const unsigned long long*> var)
		{
			RAPIDJSON_ADDMEMBER(var);
			return *this;
		}

		JsonArchive& JsonArchive::operator<< (std::pair<const char*, const float*> var)
		{
			RAPIDJSON_ADDMEMBER(var);
			return *this;
		}

		JsonArchive& JsonArchive::operator<< (std::pair<const char*, const double*> var)
		{
			RAPIDJSON_ADDMEMBER(var);
			return *this;
		}

		JsonArchive& JsonArchive::operator<< (std::pair<const char*, const JsonArchive*> var)
		{
			auto pImpl = var.second->GetIMPL();
			auto d = pImpl->GetDoc();
			auto doc = m_pImpl->GetDoc();
			rapidjson::Value name(var.first, doc->GetAllocator());
			rapidjson::Value value(*d, doc->GetAllocator());
			doc->AddMember(name, value, doc->GetAllocator());
			return *this;
		}

		//////////////////////////////////////////////////////////////////////////
#define RAPIDJSON_GETMEMBER(VAR, METHOD) do { \
	auto doc = m_pImpl->GetDoc(); \
	auto it = doc->FindMember(VAR.first); \
	if (it != doc->MemberEnd()) \
	{ \
		*(VAR.second) = it->value.METHOD(); \
	} \
	else \
	{ \
		assert(false && "Invalid member!"); \
	} \
} while (0)

		JsonArchive& JsonArchive::operator>> (std::pair<const char*, std::string*> var)
		{
			RAPIDJSON_GETMEMBER(var, GetString);
			return *this;
		}

		JsonArchive& JsonArchive::operator>> (std::pair<const char*, bool*> var)
		{
			RAPIDJSON_GETMEMBER(var, GetBool);
			return *this;
		}

		JsonArchive& JsonArchive::operator>> (std::pair<const char*, int*> var)
		{
			RAPIDJSON_GETMEMBER(var, GetInt);
			return *this;
		}

		JsonArchive& JsonArchive::operator>> (std::pair<const char*, unsigned int*> var)
		{
			RAPIDJSON_GETMEMBER(var, GetUint);
			return *this;
		}

		JsonArchive& JsonArchive::operator>> (std::pair<const char*, long long*> var)
		{
			RAPIDJSON_GETMEMBER(var, GetInt64);
			return *this;
		}

		JsonArchive& JsonArchive::operator>> (std::pair<const char*, unsigned long long*> var)
		{
			RAPIDJSON_GETMEMBER(var, GetUint64);
			return *this;
		}

		JsonArchive& JsonArchive::operator>> (std::pair<const char*, float*> var)
		{
			RAPIDJSON_GETMEMBER(var, GetDouble);
			return *this;
		}

		JsonArchive& JsonArchive::operator>> (std::pair<const char*, double*> var)
		{
			RAPIDJSON_GETMEMBER(var, GetDouble);
			return *this;
		}

		std::string JsonArchive::GetJsonString() const
		{
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			m_pImpl->GetDoc()->Accept(writer);

			return buffer.GetString();
		}

		void JsonArchive::ParseJsonString(const char* s)
		{
			m_pImpl->ParseJsonString(s);
		}

		void JsonArchive::ParseJsonString(const char* data, unsigned int dataSize)
		{
			std::string s(data, dataSize);
			ParseJsonString(s.c_str());
		}

		//////////////////////////////////////////////////////////////////////////
		class JsonArray::IMPL
		{
		public:
			IMPL() : arrayVal(rapidjson::kArrayType) {}
			~IMPL() {}

			rapidjson::Value* GetArray() { return &arrayVal; }
		private:
			rapidjson::Value arrayVal;
		};

		JsonArray::JsonArray(JsonArchive* jsonArchive)
			: m_pImpl(new IMPL())
			, m_jsonArchive(jsonArchive)
		{
		}

		JsonArray::~JsonArray()
		{
			delete m_pImpl;
		}

#define RAPIDJSON_ARRAY_ADDMEMBER(var) { \
	auto arrayVal = m_pImpl->GetArray(); \
	arrayVal->PushBack(var, m_jsonArchive->GetIMPL()->GetDoc()->GetAllocator()); \
}

		JsonArray& JsonArray::operator<< (const char* s)
		{
			auto arrayVal = m_pImpl->GetArray();
			auto& alloc = m_jsonArchive->GetIMPL()->GetDoc()->GetAllocator();
			rapidjson::Value value(s, alloc);
			arrayVal->PushBack(value, alloc);
			return *this;
		}

		JsonArray& JsonArray::operator<< (const std::string& s)
		{
			auto arrayVal = m_pImpl->GetArray();
			auto& alloc = m_jsonArchive->GetIMPL()->GetDoc()->GetAllocator();
			rapidjson::Value value(s.c_str(), alloc);
			arrayVal->PushBack(value, alloc);
			return *this;
		}

		JsonArray& JsonArray::operator<< (bool b)
		{
			RAPIDJSON_ARRAY_ADDMEMBER(b);
			return *this;
		}

		JsonArray& JsonArray::operator<< (int i)
		{
			RAPIDJSON_ARRAY_ADDMEMBER(i);
			return *this;
		}

		JsonArray& JsonArray::operator<< (unsigned int u)
		{
			RAPIDJSON_ARRAY_ADDMEMBER(u);
			return *this;
		}

		JsonArray& JsonArray::operator<< (long long l)
		{
			RAPIDJSON_ARRAY_ADDMEMBER(l);
			return *this;
		}

		JsonArray& JsonArray::operator<< (unsigned long long ul)
		{
			RAPIDJSON_ARRAY_ADDMEMBER(ul);
			return *this;
		}

		JsonArray& JsonArray::operator<< (float f)
		{
			RAPIDJSON_ARRAY_ADDMEMBER((double)f);
			return *this;
		}

		JsonArray& JsonArray::operator<< (double d)
		{
			RAPIDJSON_ARRAY_ADDMEMBER(d);
			return *this;
		}

		JsonArray& JsonArray::operator<< (const JsonArchive& jsonArchive)
		{
			auto arrayVal = m_pImpl->GetArray();
			auto& alloc = m_jsonArchive->GetIMPL()->GetDoc()->GetAllocator();
			auto pImpl = jsonArchive.GetIMPL();
			auto doc = pImpl->GetDoc();
			rapidjson::Value value(*doc, alloc);
			arrayVal->PushBack(value, alloc);
			return *this;
		}
		//////////////////////////////////////////////////////////////////////////
		// Must at last
		JsonArchive& JsonArchive::operator<< (std::pair<const char*, const JsonArray*> var)
		{
			auto pImpl = var.second->GetIMPL();
			auto arrayVal = pImpl->GetArray();
			auto doc = m_pImpl->GetDoc();
			rapidjson::Value name(var.first, doc->GetAllocator());
			rapidjson::Value value(*arrayVal, doc->GetAllocator());
			doc->AddMember(name, value, doc->GetAllocator());
			return *this;
		}
	}
}
