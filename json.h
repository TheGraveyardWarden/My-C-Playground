#pragma once

#include <json/reader.h>
#include <sstream>

namespace Multix
{

// TODO:
// we dont need all these 4 functions to exist!
// only the ones neccessary.
// but idk how to implement that :(
template<typename T>
concept JSONParsable = requires(T& t)
{
	JSONHandle(t, "key", "value");
	JSONHandle(t, "key", 12);
	JSONHandle(t, "key", false);
	JSONHandle(t, "key", 12.5f);
};

enum class JSONStatus
{
	None = 0,
	OK,
	Bad
};

template<JSONParsable T>
class JSONParser
{
public:
	JSONParser() = default;
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
			JSONHandle(m_Result, key, jVal.asBool());
		else if (jVal.isString())
			JSONHandle(m_Result, key, jVal.asString());
		else if (jVal.isInt())
			JSONHandle(m_Result, key, jVal.asInt());
		else if (jVal.isDouble())
			JSONHandle(m_Result, key, jVal.asFloat());
	}
private:
	T m_Result;
	std::string m_Errs;
	JSONStatus m_Status;
};
}
