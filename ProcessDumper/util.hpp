#pragma once
#include <Windows.h>
#include <memory>
#include <string_view>
#include <TlHelp32.h>

namespace util
{
	using unique_handle = std::unique_ptr<void, decltype(&CloseHandle)>;

	__forceinline uint32_t get_process_id(const std::string_view process_name)
	{
		PROCESSENTRY32 processentry = {};

		const unique_handle snapshot_handle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0), &CloseHandle);

		if (snapshot_handle.get() == INVALID_HANDLE_VALUE)
		{
			printf("[!] Failed to create ToolHelp32Snapshot [0x%lX]\n", GetLastError());
			return 0;
		}

		processentry.dwSize = sizeof(MODULEENTRY32);

		while (Process32Next(snapshot_handle.get(), &processentry) == TRUE) {
			if (process_name.compare(processentry.szExeFile) == 0)
			{
				return processentry.th32ProcessID;
			}
		}

		return 0;
	}

	__forceinline ULONG64 get_process_base_address(const uint32_t& process_id)
	{
		MODULEENTRY32 module_entry = {};

		const unique_handle snapshot_handle(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id), &CloseHandle);

		if (snapshot_handle.get() == INVALID_HANDLE_VALUE)
		{
			printf("[!] Failed to create ToolHelp32Snapshot [0x%lX]\n", GetLastError());
			return 0;
		}

		module_entry.dwSize = sizeof(module_entry);

		Module32First(snapshot_handle.get(), &module_entry);
		return (ULONG64)module_entry.modBaseAddr;
	}
}