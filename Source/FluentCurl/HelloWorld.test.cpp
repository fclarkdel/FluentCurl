#include <mutex>

#include <gtest/gtest.h>

#include <curl/curl.h>

#include <FluentCurl/HelloWorld.hpp>

using namespace CppProjectTemplate;

struct CurlResource
{
	CurlResource()
	{
		std::scoped_lock scoped_lock(_global_lock);

		++curl_resource_count;

		if(curl_resource_count == 1)
			curl_global_init(CURL_GLOBAL_DEFAULT);
	};
	~CurlResource()
	{
		std::scoped_lock scoped_lock(_global_lock);

		if(curl_resource_count == 1)
			curl_global_cleanup();

		--curl_resource_count;
	}

private:
	static inline std::mutex _global_lock;
	static inline size_t curl_resource_count;
};

namespace Protocols
{
struct HTTP
{
	static std::string
	send(std::string data)
	{
		return data;
	}
};
}

template<typename Type>
concept Protocol =
	requires() {
		Type::send(std::string("data"));
	};

template<Protocol protocol>
struct Session: CurlResource
{
	CURL* base_handle;
	std::string write_to;

	Session():
		base_handle(curl_easy_init())
	{
		curl_easy_setopt(base_handle, CURLOPT_WRITEFUNCTION, &Session::write_func);
		curl_easy_setopt(base_handle, CURLOPT_WRITEDATA, &write_to);
	}
	~Session()
	{
		curl_easy_cleanup(base_handle);
	}
	void
	perform()
	{
		curl_easy_perform(base_handle);
	};
	template<CURLoption curl_opt>
	Session&
	option(const char*)
	{
		return *this;
	}
	std::string
	send(std::string data)
	{
		return protocol::send(data);
	}

private:
	static size_t
	write_func(
		char* data,
		size_t data_member_size,
		size_t data_member_quantity,
		void* userdata)
	{
		size_t total_size = data_member_size * data_member_quantity;

		auto response = static_cast<std::string*>(userdata);

		response->append(data, total_size);

		return total_size;
	};
};
template<>
template<>
Session<Protocols::HTTP>&
Session<Protocols::HTTP>::option<CURLOPT_URL>(const char* url)
{
	curl_easy_setopt(base_handle, CURLOPT_URL, url);

	return *this;
}

TEST(HelloWorld, print)
{
	Session<Protocols::HTTP> session;

	session
		.option<CURLOPT_URL>("https://example.com/")
		.perform();

	std::cout << session.write_to << '\n';
}