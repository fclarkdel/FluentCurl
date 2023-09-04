#include <type_traits>

#include <gtest/gtest.h>

#include <FluentCurl/Handle.hpp>

using namespace FluentCurl;

TEST(Handle, default_constructor)
{
	Handle original;
}
TEST(Handle, copy_constructor)
{
	Handle original;

	Handle copy(original);
}
TEST(Handle, copy_assignment)
{
	Handle original;

	Handle copy;

	copy = original;
}
TEST(Handle, move_constructor)
{
	Handle original;

	Handle move(std::move(original));
}
TEST(Handle, move_assignment)
{
	Handle original;

	Handle move;

	move = std::move(original);
}
TEST(Handle, should_set_options)
{
	Handle handle;

	std::string write_to;

	handle
		.option<CURLOPT_URL>("https://example.com/")
		.option<CURLOPT_WRITEFUNCTION>([](char*, size_t, size_t, void*)
									   {
										   return size_t{};
									   })
		.option<CURLOPT_WRITEDATA>(&write_to);
}
TEST(Handle, should_reset_options)
{
	Handle handle;

	std::string write_to;

	handle
		.option<CURLOPT_URL>("https://example.com/")
		.option<CURLOPT_WRITEFUNCTION>([](char*, size_t, size_t, void*)
									   {
										   return size_t{};
									   })
		.option<CURLOPT_WRITEDATA>(&write_to);

	handle.reset();
}
TEST(Handle, should_return_raw_handle)
{
	Handle handle;

	handle.raw_handle();
}