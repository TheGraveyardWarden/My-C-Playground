#include <iostream>
#include "httpclient.h"
#include "file.h"

using namespace Multix;

struct Token : public Jsonable
{
	bool isValid = false;
	int expiresAt = 0;

	Token() = default;

	Token(Token&& other) noexcept
	{
		isValid = other.isValid;
		expiresAt = other.expiresAt;

		other.isValid = false;
		other.expiresAt = 0;
	}

	Token& operator=(Token&& other)
	{
		isValid = other.isValid;
		expiresAt = other.expiresAt;

		other.isValid = false;
		other.expiresAt = 0;

		return *this;
	}

	void JSONHandleBool(const std::string& key, bool value)
	{
		if (key == "is_valid")
			isValid = value;
	}

	void JSONHandleInt(const std::string& key, int value)
	{
		if (key == "expires_at")
			expiresAt = value;
	}
};

int main()
{
	return 0;
}
