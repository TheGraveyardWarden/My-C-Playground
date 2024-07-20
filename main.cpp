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

	Token& operator=(Token&& other) noexcept
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
	File file("./shit");
	Token token;
	file.ReadJson(token);
	std::cout << token.isValid << ", " << token.expiresAt << std::endl;
	std::cout << (int)file.Algo() << std::endl;
	std::cout << file.Filepath() << std::endl;
	return 0;
}
