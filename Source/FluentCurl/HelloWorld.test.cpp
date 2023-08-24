#include <gtest/gtest.h>

#include <curl/curl.h>

#include <FluentCurl/HelloWorld.hpp>

using namespace CppProjectTemplate;

TEST(HelloWorld, print)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);

	CURL* easy_handle = curl_easy_init();

	curl_easy_setopt(easy_handle, CURLOPT_URL, "https://example.com/");

	auto write_func = [](char* data, size_t data_member_size, size_t data_member_quantity, void* userdata)
	{
		size_t total_size = data_member_size * data_member_quantity;

		auto response = static_cast<std::string*>(userdata);

		response->append(data, total_size);

		return total_size;
	};
	curl_easy_setopt(
		easy_handle,
		CURLOPT_WRITEFUNCTION,
		// '+' operator decays the lambda to a void pointer.
		+write_func);

	std::string write_to;
	curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, &write_to);

	curl_easy_perform(easy_handle);

	curl_easy_cleanup(easy_handle);

	curl_global_cleanup();

	std::cout << write_to << '\n';

	Session<HTTP> http_session;

	http_session.option<CURLOPT_URL>("url");
}