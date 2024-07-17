#pragma once

#include <iostream>
#include <type_traits>
#include <memory>
#include "json.h"
#include <fstream>
#include <sstream>
#include "crypto.h"

#ifndef XOR_KEY
#define XOR_KEY "!!#@@#!@#!@"
#endif

namespace Multix
{
enum class FileStatus
{
	None = 0,
	OK,
	Bad
};

enum class FileType
{
	None = 0,
	EncXOR
};

class File
{
public:
	virtual std::string Read() = 0;
	virtual void Write(const std::string& data) = 0;

	virtual std::fstream& fstream() = 0;
	virtual const std::fstream& fstream() const = 0;

	virtual const std::string& Filepath() const = 0;

	template<typename T>
	FileStatus ReadJson(T& t);

	static std::unique_ptr<File> Create(const std::string& filepath, FileType ft);
};

class FileXOR : public File
{
public:
	FileXOR(const std::string& filepath)
		: m_Filepath(filepath)
	{
		m_Crypt = Crypto::Create(CryptoAlgo::XOR, XOR_KEY);
	}

	~FileXOR() = default;

	std::string Read() override
	{
		m_FStream.open(m_Filepath, std::ios::in);
		std::stringstream ss;
		std::string line;

		while (m_FStream)
		{
			getline(m_FStream, line);
			ss << line << std::endl;
		}

		m_FStream.close();

		return m_Crypt->Dec(ss.str().substr(0, ss.str().size()-2));
	}

	void Write(const std::string& data) override
	{
		m_FStream.open(m_Filepath, std::ios::trunc | std::ios::out);
		std::string enc_data = m_Crypt->Enc(data);
		m_FStream << enc_data << std::endl;
		m_FStream.close();
	}

	inline std::fstream& fstream() override { return m_FStream; }
	inline const std::fstream& fstream() const override { return m_FStream; }

	inline const std::string& Filepath() const override { return m_Filepath; }
private:
	std::fstream m_FStream;
	std::string m_Filepath;
	std::unique_ptr<Crypto> m_Crypt;
};

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

std::unique_ptr<File> File::Create(const std::string& filepath, FileType ft)
{
	switch(ft)
	{
		case FileType::EncXOR:
			return std::make_unique<FileXOR>(filepath);
			break;
		default:
			std::cout << "File Type is Not Supported!" << std::endl;
			return nullptr;
	}
}
}
