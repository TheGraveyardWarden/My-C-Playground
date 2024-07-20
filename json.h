#ifndef MULTIX_JSON_H
#define MULTIX_JSON_H

#include <json/reader.h>
#include <sstream>
#include <type_traits>

namespace Multix
{
class Jsonable
{
public:
	virtual void JSONHandleStr(const std::string& key, const std::string& value) {}
	virtual void JSONHandleBool(const std::string& key, bool value) {}
	virtual void JSONHandleInt(const std::string& key, int value) {}
	virtual void JSONHandleFloat(const std::string& key, float value) {}
};

enum class JSONStatus
{
	None = 0,
	OK,
	Bad
};

template<typename T>
class JSONParser
{
public:
	JSONParser()
	{
		static_assert(std::is_base_of_v<Jsonable, T>, "type must inherit from Jsonable");
	}
	~JSONParser() = default;

	JSONParser& operator<<(const std::string& jsonStr)
	{
		std::stringstream ss;
		bool ok;
		Json::Value root;
		Json::CharReaderBuilder rbuilder;

		m_Status = JSONStatus::OK;

		ss << jsonStr;
		ok = Json::parseFromStream(rbuilder, ss, &root, &m_Errs);

		if (!ok)
		{
			m_Status = JSONStatus::Bad;
			goto end;
		}

		for (const auto& m : root.getMemberNames())
			handle(m, root[m]);

	end:
		return *this;
	}

	inline T& Result() { return m_Result; }
	inline const T& Result() const { return m_Result; }

	inline JSONStatus Status() const { return m_Status; }

	inline std::string& Errs() { return m_Errs; }
	inline const std::string& Errs() const { return m_Errs; }
private:
	// TODO:
	// implement a way to handle jVal.isArray() and jVal.isObject()
	void handle(const std::string& key, Json::Value& jVal)
	{
		if (jVal.isBool())
			m_Result.JSONHandleBool(key, jVal.asBool());
		else if (jVal.isString())
			m_Result.JSONHandleStr(key, jVal.asString());
		else if (jVal.isInt())
			m_Result.JSONHandleInt(key, jVal.asInt());
		else if (jVal.isDouble())
			m_Result.JSONHandleFloat(key, jVal.asFloat());
	}
private:
	T m_Result;
	std::string m_Errs;
	JSONStatus m_Status;
};
}

#endif // MULTIX_JSON_H
