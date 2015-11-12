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

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"

namespace dopixel
{
	// a helper class for using rapidjson::Value
	class JsonValue
	{
	public:
		JsonValue();
		JsonValue(rapidjson::Value* value);
		JsonValue & operator = (const JsonValue & jv) = delete;

		// for object
		const JsonValue& operator>> (std::pair<const char*, std::string*> var) const;
		const JsonValue& operator>> (std::pair<const char*, bool*> var) const;
		const JsonValue& operator>> (std::pair<const char*, int*> var) const;
		const JsonValue& operator>> (std::pair<const char*, unsigned int*> var) const;
		const JsonValue& operator>> (std::pair<const char*, long long*> var) const;
		const JsonValue& operator>> (std::pair<const char*, unsigned long long*> var) const;
		const JsonValue& operator>> (std::pair<const char*, float*> var) const;
		const JsonValue& operator>> (std::pair<const char*, double*> var) const;

		bool HasMember(const char* key) const;
		JsonValue GetMember(const char* key) const;

		// for array
		size_t Size() const;
		JsonValue operator[] (size_t index) const;
		// for element in array
		const JsonValue& operator>> (std::string& s) const;
		const JsonValue& operator>> (bool& b) const;
		const JsonValue& operator>> (int& i) const;
		const JsonValue& operator>> (unsigned int& u) const;
		const JsonValue& operator>> (long long& l) const;
		const JsonValue& operator>> (unsigned long long& ul) const;
		const JsonValue& operator>> (float& f) const;
		const JsonValue& operator>> (double& d) const;

		rapidjson::Value& GetValue();
		void SetValue(rapidjson::Value * value);
	private:
		rapidjson::Value* m_value;
	};

	// a helper class for using rapidjson::Document
	class JsonDoc : public JsonValue
	{
	public:
		JsonDoc();
		JsonDoc & operator = (const JsonDoc & jd) = delete;

		// for object
		JsonDoc& operator<< (std::pair<const char*, const char *> var);
		JsonDoc& operator<< (std::pair<const char*, const std::string*> var);
		JsonDoc& operator<< (std::pair<const char*, const bool *> var);
		JsonDoc& operator<< (std::pair<const char*, const int *> var);
		JsonDoc& operator<< (std::pair<const char*, const unsigned int *> var);
		JsonDoc& operator<< (std::pair<const char*, const long long *> var);
		JsonDoc& operator<< (std::pair<const char*, const unsigned long long *> var);
		JsonDoc& operator<< (std::pair<const char*, const float *> var);
		JsonDoc& operator<< (std::pair<const char*, const double *> var);
		JsonDoc& operator<< (std::pair<const char*, const JsonDoc *> var);

		bool ParseJsonString(const char * s);
		bool ParseJsonString(const std::string& s);
		bool ParseJsonString(const char * data, unsigned int dataSize);
		std::string GetJsonString() const;

		// for array
		JsonDoc& operator<< (const JsonDoc& jsonDoc);
		JsonDoc& operator<< (bool b);
		JsonDoc& operator<< (int i);
		JsonDoc& operator<< (unsigned int u);
		JsonDoc& operator<< (long long l);
		JsonDoc& operator<< (unsigned long long ul);
		JsonDoc& operator<< (float f);
		JsonDoc& operator<< (double d);
		JsonDoc& operator<< (const char* s);
		JsonDoc& operator<< (const std::string& s);

		rapidjson::Document& GetDoc();
	private:
		rapidjson::Document m_doc;
	};

	template <typename T> inline std::pair< const char *, const T*> MakeJsonVar_IN(const char * name, const T & var) { return std::make_pair(name, &var); }
	inline std::pair <const char*, const char *> MakeJsonVar_IN(const char * name, const char * var) { return std::make_pair(name, var); }
	template <typename T> inline std::pair< const char *, T *> MakeJsonVar_OUT(const char * name, T & var) { return std::make_pair(name, &var); }

#define WHO_AM_I(VAR) (#VAR)
#define JVAR_IN(VAR) (MakeJsonVar_IN(WHO_AM_I(VAR), VAR))
#define JVAR_OUT(VAR) (MakeJsonVar_OUT(WHO_AM_I(VAR), VAR))
#define JVAR_IN_MANUAL(VARNAME , VAR) (MakeJsonVar_IN(VARNAME , VAR))
#define JVAR_OUT_MANUAL(VARNAME , VAR) (MakeJsonVar_OUT(VARNAME , VAR))
}

#endif