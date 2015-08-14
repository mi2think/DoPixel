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

#define JSONVALUE_GETMEMBER(VAR, METHOD) { \
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
		const JsonValue& JsonValue::operator>> (std::pair<const char *, std::string*> var) const
		{
			JSONVALUE_GETMEMBER(var, GetString);
			return *this;
		}

		const JsonValue& JsonValue::operator>> (std::pair<const char *, bool *> var) const
		{
			JSONVALUE_GETMEMBER(var, GetBool);
			return *this;
		}

		const JsonValue& JsonValue::operator>> (std::pair<const char *, int *> var) const
		{
			JSONVALUE_GETMEMBER(var, GetInt);
			return *this;
		}

		const JsonValue& JsonValue::operator>> (std::pair<const char *, unsigned int *> var) const
		{
			JSONVALUE_GETMEMBER(var, GetUint);
			return *this;
		}

		const JsonValue& JsonValue::operator>> (std::pair<const char *, long long*> var) const
		{
			JSONVALUE_GETMEMBER(var, GetInt64);
			return *this;
		}

		const JsonValue& JsonValue::operator>> (std::pair<const char *, unsigned long long *> var) const
		{
			JSONVALUE_GETMEMBER(var, GetUint64);
			return *this;
		}

		const JsonValue& JsonValue::operator>> (std::pair<const char *, float *> var) const
		{
			JSONVALUE_GETMEMBER(var, GetDouble);
			return *this;
		}

		const JsonValue& JsonValue::operator>> (std::pair<const char *, double *> var) const
		{
			JSONVALUE_GETMEMBER(var, GetDouble);
			return *this;
		}

		bool JsonValue::HasMember(const char * key) const
		{
			return m_value->HasMember(key);
		}

		JsonValue JsonValue::GetMember(const char * key) const
		{
			return JsonValue(&(m_value->FindMember(key)->value));
		}

		size_t JsonValue::Size() const
		{
			return m_value->Size();
		}

		JsonValue JsonValue::operator[](size_t index) const
		{
			return JsonValue(&((*m_value)[index]));
		}

		const JsonValue& JsonValue::operator >> (std::string& s) const
		{
			s = m_value->GetString();
			return *this;
		}

		const JsonValue& JsonValue::operator>> (bool& b) const
		{
			b = m_value->GetBool();
			return *this;
		}

		const JsonValue& JsonValue::operator>> (int& i) const
		{
			i = m_value->GetInt();
			return *this;
		}

		const JsonValue& JsonValue::operator>> (unsigned int& u) const
		{
			u = m_value->GetUint();
			return *this;
		}

		const JsonValue& JsonValue::operator>> (long long& l) const
		{
			l = m_value->GetInt64();
			return *this;
		}

		const JsonValue& JsonValue::operator>> (unsigned long long& ul) const
		{
			ul = m_value->GetUint64();
			return *this;
		}

		const JsonValue& JsonValue::operator>> (float& f) const
		{
			f = static_cast<float>(m_value->GetDouble());
			return *this;
		}

		const JsonValue& JsonValue::operator>> (double& d) const
		{
			d = m_value->GetDouble();
			return *this;
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

#define JSONDOC_PUSHMEMBER(VAR) { \
	if (!m_doc.IsArray()) \
		m_doc.SetArray(); \
	rapidjson::Value value(VAR); \
	m_doc.PushBack(value, m_doc.GetAllocator()); \
}

		JsonDoc& JsonDoc::operator<< (const JsonDoc& jsonDoc)
		{
			if (!m_doc.IsArray())
				m_doc.SetArray();

			rapidjson::Value value(const_cast<JsonDoc&>(jsonDoc).GetDoc(), m_doc.GetAllocator());
			m_doc.PushBack(value, m_doc.GetAllocator());
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (bool b)
		{
			JSONDOC_PUSHMEMBER(b);
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (int i)
		{
			JSONDOC_PUSHMEMBER(i);
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (unsigned int u)
		{
			JSONDOC_PUSHMEMBER(u);
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (long long l)
		{
			JSONDOC_PUSHMEMBER(l);
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (unsigned long long ul)
		{
			JSONDOC_PUSHMEMBER(ul);
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (float f)
		{
			JSONDOC_PUSHMEMBER(f);
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (double d)
		{
			JSONDOC_PUSHMEMBER(d);
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (const char* s)
		{
			if (!m_doc.IsArray())
				m_doc.SetArray();

			rapidjson::Value value(s, m_doc.GetAllocator());
			m_doc.PushBack(value, m_doc.GetAllocator());
			return *this;
		}

		JsonDoc& JsonDoc::operator<< (const std::string& s)
		{
			if (!m_doc.IsArray())
				m_doc.SetArray();

			rapidjson::Value value(s.c_str(), m_doc.GetAllocator());
			m_doc.PushBack(value, m_doc.GetAllocator());
			return *this;
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
