#ifndef MULTIX_CRYPTO_H
#define MULTIX_CRYPTO_H

#include <sstream>
#include <memory>

namespace Multix
{
enum class CryptoAlgo
{
	None = 0,
	XOR
};

class Crypto
{
public:
	virtual std::string Enc(const std::string& data) = 0;
	virtual std::string Dec(const std::string& data) = 0;

	virtual CryptoAlgo Algo() const { return CryptoAlgo::None; }

	static std::unique_ptr<Crypto> Create(CryptoAlgo algo, const std::string& key);
};

class CryptoXOR : public Crypto
{
public:
	CryptoXOR(const std::string& key) : m_Key(key) {}
	~CryptoXOR() = default;

	std::string Enc(const std::string& data) override
	{
		char buff[data.size() + 1];
		const char* d = data.c_str();
		const char* k = m_Key.c_str();

		for (std::size_t i = 0; i < data.size(); i++)
			buff[i] = d[i] ^ k[i % m_Key.size()];
		buff[data.size()] = 0;

		return std::string(buff);
	}

	std::string Dec(const std::string& data) override
	{
		return Enc(data);
	}

	CryptoAlgo Algo() const override { return CryptoAlgo::XOR; }
private:
	std::string m_Key;
};

std::unique_ptr<Crypto> Crypto::Create(CryptoAlgo algo, const std::string& key)
{
	switch(algo)
	{
		case CryptoAlgo::XOR:
			return std::make_unique<CryptoXOR>(key);
			break;
		default:
			std::cout << "Specified Algo is Not Supported!" << std::endl;
			return nullptr;
	}
}

}

#endif // MULTIX_CRYPTO_H
