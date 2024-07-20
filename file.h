#ifndef MULTIX_FILE_H
#define MULTIX_FILE_H

#include <iostream>
#include <type_traits>
#include <memory>
#include "json.h"
#include <fstream>
#include <sstream>
#include "crypto.h"

#ifndef FILE_KEY
#define FILE_KEY "!!#@@#!@#!@"
#endif

namespace Multix
{
enum class FileStatus
{
	None = 0,
	OK,
	Bad
};

class File
{
public:
	File(const std::string& filepath, CryptoAlgo alg = CryptoAlgo::XOR);
	~File();

	std::string Read();
	void Write(const std::string& data);

	inline const std::string& Filepath() const { return m_Filepath; }
	inline CryptoAlgo Algo() const { return m_Crypto->Algo(); }

	template<typename T>
	FileStatus ReadJson(T& t);
private:
	std::unique_ptr<Crypto> m_Crypto;
	std::string m_Filepath;
};

File::File(const std::string& filepath, CryptoAlgo alg)
	: m_Filepath(filepath)
{
	m_Crypto = Crypto::Create(alg, FILE_KEY);
}

File::~File()
{}

std::string File::Read()
{
	std::fstream stream;
	stream.open(m_Filepath, std::ios::in);
	std::stringstream ss;
	std::string line;

	while (stream)
	{
		getline(stream, line);
		ss << line << std::endl;
	}
	stream.close();

	return m_Crypto->Dec(ss.str().substr(0, ss.str().size()-2));
}

void File::Write(const std::string& data)
{
	std::fstream stream;
	stream.open(m_Filepath, std::ios::trunc | std::ios::out);
	std::string enc_data = m_Crypto->Enc(data);
	stream << enc_data << std::endl;
	stream.close();
}

template<typename T>
FileStatus File::ReadJson(T& t)
{
	static_assert(std::is_move_assignable_v<T>, "type should be move assignable");

	JSONParser<T> parser;

	std::string data = Read();

	parser << data;

	if (parser.Status() != JSONStatus::OK)
		return FileStatus::Bad;

	t = std::move(parser.Result());

	return FileStatus::OK;
}
}

#endif // MULTIX_FILE_H
