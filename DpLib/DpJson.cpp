#include "DpJson.h"
#include "DpRef.h"

namespace DoPixel
{
	namespace Core
	{
		JsonValue::JsonValue()
			: m_value(nullptr)
		{
		}

		JsonValue::JsonValue(rapidjson::Value * value)
			: m_value(value)
		{
		}

#define JSONVALUE_GETMEMBER(VAR , METHOD) { \
	auto it = m_value->FindMember(VAR.first); \
    if(it != m_value->MemberEnd()) \
	{ \
		*(VAR.second) = it->value.METHOD(); \
	} \
	else \
	{ \
		assert(false && "Invalid member!" ); \
	} \
}
		JsonValue& JsonValue::operator>> (std::pair<const char *, std::string*> var)
		{
			JSONVALUE_GETMEMBER(var, GetString);
			return *this;
		}

		JsonValue& JsonValue::operator>> (std::pair<const char *, bool *> var)
		{
			JSONVALUE_GETMEMBER(var, GetBool);
			return *this;
		}

		JsonValue& JsonValue::operator>> (std::pair<const char *, int *> var)
		{
			JSONVALUE_GETMEMBER(var, GetInt);
			return *this;
		}

		JsonValue& JsonValue::operator>> (std::pair<const char *, unsigned int *> var)
		{
			JSONVALUE_GETMEMBER(var, GetUint);
			return *this;
		}

		JsonValue& JsonValue::operator>> (std::pair<const char *, long long*> var)
		{
			JSONVALUE_GETMEMBER(var, GetInt64);
			return *this;
		}

		JsonValue& JsonValue::operator>> (std::pair<const char *, unsigned long long *> var)
		{
			JSONVALUE_GETMEMBER(var, GetUint64);
			return *this;
		}

		JsonValue& JsonValue::operator>> (std::pair<const char *, float *> var)
		{
			JSONVALUE_GETMEMBER(var, GetDouble);
			return *this;
		}

		JsonValue& JsonValue::operator>> (std::pair<const char *, double *> var)
		{
			JSONVALUE_GETMEMBER(var, GetDouble);
			return *this;
		}

		bool JsonValue::HasMember(const char * key) const
		{
			return m_value->HasMember(key);
		}

		JsonValue JsonValue::GetMember(const char * key)
		{
			return JsonValue(&(m_value->FindMember(key)->value));
		}

		size_t JsonValue::Size() const
		{
			return m_value->Size();
		}

		JsonValue JsonValue::operator[](size_t index)
		{
			return JsonValue(&((*m_value)[index]));
		}

		rapidjson::Value& JsonValue::GetValue()
		{
			return *m_value;
		}

		void JsonValue::SetValue(rapidjson::Value * value)
		{
			m_value = value;
		}
		//////////////////////////////////////////////////////////////////////////
		JsonDoc::JsonDoc()
		{
			m_doc.SetObject();
			SetValue(&m_doc);
		}

#define JSONDOC_ADDMEMBER( VAR ) { \
       rapidjson::Value name(VAR.first, m_doc.GetAllocator ()); \
	   m_doc.AddMember (name, *(VAR.second), m_doc.GetAllocator()); \
}
		JsonDoc& JsonDoc::operator<< (std::pair<const char *, const char*> var)
		{
			rapidjson::Value name(var.first, m_doc.GetAllocator());
			rapidjson::Value val(var.second, m_doc.GetAllocator());
			m_doc.AddMember(name, val, m_doc.GetAllocator());
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (std::pair<const char *, const std::string*> var)
		{
			rapidjson::Value name(var.first, m_doc.GetAllocator());
			rapidjson::Value val(var.second->c_str(), m_doc.GetAllocator());
			m_doc.AddMember(name, val, m_doc.GetAllocator());
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (std::pair<const char *, const bool*> var)
		{
			JSONDOC_ADDMEMBER(var);
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (std::pair<const char *, const int*> var)
		{
			JSONDOC_ADDMEMBER(var);
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (std::pair<const char *, const unsigned int *> var)
		{
			JSONDOC_ADDMEMBER(var);
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (std::pair<const char *, const long long int *> var)
		{
			JSONDOC_ADDMEMBER(var);
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (std::pair<const char *, const unsigned long long int*> var)
		{
			JSONDOC_ADDMEMBER(var);
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (std::pair<const char *, const float*> var)
		{
			JSONDOC_ADDMEMBER(var);
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (std::pair<const char *, const double*> var)
		{
			JSONDOC_ADDMEMBER(var);
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (std::pair< const char *, const JsonDoc*> var)
		{
			auto & doc = const_cast<JsonDoc *>(var.second)->GetDoc();
			rapidjson::Value name(var.first, m_doc.GetAllocator());
			rapidjson::Value value(doc, m_doc.GetAllocator());
			m_doc.AddMember(name, value, m_doc.GetAllocator());
			return *this;
		}

		void JsonDoc::PushBack(JsonDoc& jsonDoc)
		{
			if (!m_doc.IsArray())
				m_doc.SetArray();

			m_doc.PushBack(jsonDoc.GetDoc(), m_doc.GetAllocator());
		}

		std::string JsonDoc::GetJsonString() const
		{
			rapidjson::StringBuffer buffer;
			rapidjson::Writer< rapidjson::StringBuffer > writer(buffer);
			m_doc.Accept(writer);

			return buffer.GetString();
		}

		bool JsonDoc::ParseJsonString(const char * data, unsigned int dataSize)
		{
			if (data == nullptr)
				return false;
			std::string s(data, dataSize);
			return ParseJsonString(s.c_str());
		}

		bool JsonDoc::ParseJsonString(const char * s)
		{
			m_doc.Parse(s);
			return !m_doc.HasParseError();
		}

		bool JsonDoc::ParseJsonString(const std::string& s)
		{
			m_doc.Parse(s.c_str());
			return !m_doc.HasParseError();
		}

		rapidjson::Document& JsonDoc::GetDoc()
		{
			return m_doc;
		}
	}
}
