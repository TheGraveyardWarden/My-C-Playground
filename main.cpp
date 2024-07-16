#include <iostream>
#include "httpclient.h"
#include "json.h"


struct Token : Multix::Jsonable
{
	bool isValid = false;
	unsigned int expiresAt = 0;

	void JSONHandleBool(const std::string& key, bool value) override
	{
		std::cout << std::boolalpha;
		if (key == "is_valid")
			isValid = value;
	}

	void JSONHandleInt(const std::string& key, int value) override
	{
		if (key == "expires_at")
			expiresAt = value;
	}
};

struct Shit {};

std::ostream& operator<<(std::ostream& os, Token& token)
{
	os << std::boolalpha;
	os << "isValid: " << token.isValid << ", expiresAt: " << token.expiresAt;
	return os;
}

int main()
{
	Multix::HttpClient cli;
	Multix::HttpStatus status;

	if ((status = cli.Get("http://localhost:9000/validtoken")) != Multix::HttpStatus::OK)
	{
		std::cout << "req failed!" << std::endl;
		return 1;
	}

	Multix::JSONParser<Token> parser;
	parser << cli.Response().content;

	if (parser.Status() != Multix::JSONStatus::OK) goto end;

	std::cout << parser.Result() << std::endl;

	if ((status = cli.Get("http://localhost:9000/validctoken")) != Multix::HttpStatus::OK)
	{
		std::cout << "req failed!" << std::endl;
		return 1;
	}

	parser << cli.Response().content;
	std::cout << parser.Result() << std::endl;

	return 0;

end:
	std::cout << "something went wrong!" << std::endl;
	std::cout << parser.Errs() << std::endl;
	return 1;
}
