#include <iostream>
#include "crypto.h"
#include "json.h"
#include "httpclient.h"

using namespace Multix;

struct Token : public Jsonable
{
	bool isValid = false;
	int expiresAt = 0;

	Token() = default;

	Token(Token&& other) noexcept
	{
		std::cout << "Token Moved!" << std::endl;
		isValid = other.isValid;
		expiresAt = other.expiresAt;

		other.isValid = false;
		other.expiresAt = 0;
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
	HttpClient cli;
	HttpStatus hstatus;
	JSONParser<Token> jparser;
	std::unique_ptr<Crypto> crypt = Crypto::Create(CryptoAlgo::XOR, "MY KEY");

	hstatus = cli.Get("http://localhost:9000/validtoken");
	if (hstatus != HttpStatus::OK)
	{
		std::cout << "req failed" << std::endl;
		return 1;
	}
	
	jparser << cli.Response().content;
	if (jparser.Status() != JSONStatus::OK)
	{
		std::cout << "parser failed" << std::endl;
		return 1;
	}

	Token token = std::move(jparser.Result());
	if (!token.isValid)
	{
		std::cout << "Token is not Valid" << std::endl;
		return 1;
	} else {
		std::cout << "Token is Valid" << std::endl;
	}

	std::string enced = crypt->Enc(cli.Response().content);
	std::cout << enced << std::endl;

	std::string deced = crypt->Dec(enced);
	std::cout << deced << std::endl;

	return 0;
}
