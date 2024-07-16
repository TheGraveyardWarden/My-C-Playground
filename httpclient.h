#pragma once

#include <curl/curl.h>

namespace Multix
{
struct HttpResponse
{
	std::string content;
	long statusCode;

	inline void Clear()
	{
		content.clear();
		statusCode = 0;
	}
};

enum class HttpStatus
{
	None = 0,
	OK,
	Bad
};

class HttpClient
{
public:
	HttpClient()
	{
		m_Curl = curl_easy_init();
		if (m_Curl)
			SetWriteOpts();
	}

	~HttpClient()
	{
		if (m_Curl)
			curl_easy_cleanup(m_Curl);
	}

	HttpStatus Get(const char* url)
	{
		CURLcode res;
		m_Status = HttpStatus::Bad;
		m_Response.Clear();
		if (m_Curl)
		{
			SetUrl(url);
			res = Perform();
			if (res == CURLE_OK)
			{
				m_Status = HttpStatus::OK;
				SetResponseCode();
			}
		}
		return m_Status;
	}

	inline const HttpResponse& Response() const { return m_Response; }
	inline HttpStatus Status() const { return m_Status; }
private:
	static size_t cb(char* data, size_t size, size_t nmemb, void* clientp)
	{
		((std::string*)clientp)->append(data, size * nmemb);
		return size * nmemb;
	}
	
	inline void SetResponseCode()
	{
		curl_easy_getinfo(m_Curl, CURLINFO_RESPONSE_CODE, &m_Response.statusCode);
	}

	inline void SetWriteOpts()
	{	
		curl_easy_setopt(m_Curl, CURLOPT_WRITEDATA, (void*)&(m_Response.content));
		curl_easy_setopt(m_Curl, CURLOPT_WRITEFUNCTION, &HttpClient::cb);
	}

	inline void SetUrl(const char* url)
	{
		curl_easy_setopt(m_Curl, CURLOPT_URL, url);
	}

	inline CURLcode Perform()
	{
		return curl_easy_perform(m_Curl);
	}
private:
	CURL* m_Curl;
	HttpResponse m_Response;
	HttpStatus m_Status;
};
}
