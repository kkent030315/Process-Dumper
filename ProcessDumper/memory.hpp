#pragma once
#include <Windows.h>
#include <string>
#include <string_view>

#include "util.hpp"

namespace memory
{
	namespace detail
	{
		inline uint32_t process_id = {};
		inline HANDLE process_handle = {};
		inline ULONG64 process_base_address = {};
	}

	bool init(const std::string_view process_name);
	void exit();

	template<class T> __forceinline T read(uintptr_t address)
	{
		T buff = {};

		if (ReadProcessMemory(
			detail::process_handle,
			reinterpret_cast<LPCVOID>(address),
			&buff,
			sizeof(T),
			nullptr
		))
		{
			return *(T*)&buff;
		}

		return buff;
	}

	__forceinline bool read_buffer(uintptr_t address, void* buffer, size_t size)
	{
		return ReadProcessMemory(
			detail::process_handle,
			reinterpret_cast<LPCVOID>(address),
			buffer,
			size,
			nullptr
		);
	}
}