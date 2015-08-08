/********************************************************************
	created:	2015/08/05
	created:	5:8:2015   23:50
	filename: 	D:\OneDrive\3D\DpLib\TestCases\dptest_json.cpp
	file path:	D:\OneDrive\3D\DpLib\TestCases
	file base:	dptest_json
	file ext:	cpp
	author:		mi2think@gmail.com
	
	purpose:	Json Test
*********************************************************************/
#include "DpUnitTest.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" // for stringify JSON

using namespace std;
using namespace DoPixel::DpTest;
using namespace rapidjson;

#undef RAPIDJSON_HAS_STDSTRING

//////////////////////////////////////////////////////////////////////////
// simples from rapidjson org


void simple_dom()
{
	// 1. 把JSON解析至DOM。
	const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
	Document d;
	d.Parse(json);
	
	// 2. 利用DOM作出修改。
	Value& s = d["stars"];
	s.SetInt(s.GetInt() + 1);
	
	// 3. 把DOM转换（stringify）成JSON。
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);
	
	// Output {"project":"rapidjson","stars":11}
	std::cout << buffer.GetString() << std::endl;
}

void simple_writer()
{
	StringBuffer s;
	Writer<StringBuffer> writer(s);

	writer.StartObject();
	writer.String("hello");
	writer.String("world");
	writer.String("t");
	writer.Bool(true);
	writer.String("f");
	writer.Bool(false);
	writer.String("n");
	writer.Null();
	writer.String("i");
	writer.Uint(123);
	writer.String("pi");
	writer.Double(3.1416);
	writer.String("a");
	writer.StartArray();
	for (unsigned i = 0; i < 4; i++)
		writer.Uint(i);
	writer.EndArray();
	writer.EndObject();

	std::cout << s.GetString() << std::endl;
}

void simple_reader()
{
	struct MyHandler {
		bool Null() { cout << "Null()" << endl; return true; }
		bool Bool(bool b) { cout << "Bool(" << boolalpha << b << ")" << endl; return true; }
		bool Int(int i) { cout << "Int(" << i << ")" << endl; return true; }
		bool Uint(unsigned u) { cout << "Uint(" << u << ")" << endl; return true; }
		bool Int64(int64_t i) { cout << "Int64(" << i << ")" << endl; return true; }
		bool Uint64(uint64_t u) { cout << "Uint64(" << u << ")" << endl; return true; }
		bool Double(double d) { cout << "Double(" << d << ")" << endl; return true; }
		bool String(const char* str, SizeType length, bool copy) {
			cout << "String(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
			return true;
		}
		bool StartObject() { cout << "StartObject()" << endl; return true; }
		bool Key(const char* str, SizeType length, bool copy) {
			cout << "Key(" << str << ", " << length << ", " << boolalpha << copy << ")" << endl;
			return true;
		}
		bool EndObject(SizeType memberCount) { cout << "EndObject(" << memberCount << ")" << endl; return true; }
		bool StartArray() { cout << "StartArray()" << endl; return true; }
		bool EndArray(SizeType elementCount) { cout << "EndArray(" << elementCount << ")" << endl; return true; }
	};

	const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";

	MyHandler handler;
	Reader reader;
	StringStream ss(json);
	reader.Parse(ss, handler);
}


class Person {
public:
	Person(const std::string& name, unsigned age) : name_(name), age_(age) {}
	virtual ~Person();

protected:
	template <typename Writer>
	void Serialize(Writer& writer) const {
		// This base class just write out name-value pairs, without wrapping within an object.
		writer.String("name");
#ifdef RAPIDJSON_HAS_STDSTRING
		writer.String(name_);
#else
		writer.String(name_.c_str(), (SizeType)name_.length()); // Supplying length of string is faster.
#endif
		writer.String("age");
		writer.Uint(age_);
	}

private:
	std::string name_;
	unsigned age_;
};

Person::~Person() {
}

class Education {
public:
	Education(const std::string& school, double GPA) : school_(school), GPA_(GPA) {}

	template <typename Writer>
	void Serialize(Writer& writer) const {
		writer.StartObject();

		writer.String("school");
#ifdef RAPIDJSON_HAS_STDSTRING
		writer.String(school_);
#else
		writer.String(school_.c_str(), (SizeType)school_.length());
#endif

		writer.String("GPA");
		writer.Double(GPA_);

		writer.EndObject();
	}

private:
	std::string school_;
	double GPA_;
};

class Dependent : public Person {
public:
	Dependent(const std::string& name, unsigned age, Education* education = 0) : Person(name, age), education_(education) {}
	Dependent(const Dependent& rhs) : Person(rhs), education_(0) { education_ = (rhs.education_ == 0) ? 0 : new Education(*rhs.education_); }
	virtual ~Dependent();

	Dependent& operator=(const Dependent& rhs) {
		if (this == &rhs)
			return *this;
		delete education_;
		education_ = (rhs.education_ == 0) ? 0 : new Education(*rhs.education_);
		return *this;
	}

	template <typename Writer>
	void Serialize(Writer& writer) const {
		writer.StartObject();

		Person::Serialize(writer);

		writer.String("education");
		if (education_)
			education_->Serialize(writer);
		else
			writer.Null();

		writer.EndObject();
	}

private:

	Education *education_;
};

Dependent::~Dependent() {
	delete education_;
}

class Employee : public Person {
public:
	Employee(const std::string& name, unsigned age, bool married) : Person(name, age), dependents_(), married_(married) {}
	virtual ~Employee();

	void AddDependent(const Dependent& dependent) {
		dependents_.push_back(dependent);
	}

	template <typename Writer>
	void Serialize(Writer& writer) const {
		writer.StartObject();

		Person::Serialize(writer);

		writer.String("married");
		writer.Bool(married_);

		writer.String(("dependents"));
		writer.StartArray();
		for (std::vector<Dependent>::const_iterator dependentItr = dependents_.begin(); dependentItr != dependents_.end(); ++dependentItr)
			dependentItr->Serialize(writer);
		writer.EndArray();

		writer.EndObject();
	}

private:
	std::vector<Dependent> dependents_;
	bool married_;
};

Employee::~Employee() {
}


void simple_serialize()
{
	std::vector<Employee> employees;

	employees.push_back(Employee("Milo YIP", 34, true));
	employees.back().AddDependent(Dependent("Lua YIP", 3, new Education("Happy Kindergarten", 3.5)));
	employees.back().AddDependent(Dependent("Mio YIP", 1));

	employees.push_back(Employee("Percy TSE", 30, false));

	StringBuffer sb;
	//PrettyWriter<StringBuffer> writer(sb);
	Writer<StringBuffer> writer(sb);

	writer.StartArray();
	for (std::vector<Employee>::const_iterator employeeItr = employees.begin(); employeeItr != employees.end(); ++employeeItr)
		employeeItr->Serialize(writer);
	writer.EndArray();

	puts(sb.GetString());
}

//////////////////////////////////////////////////////////////////////////

DPTEST(json)
{
	simple_dom();

	simple_writer();

	simple_reader();

	simple_serialize();
}