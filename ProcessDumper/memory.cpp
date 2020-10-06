#include "memory.hpp"

bool memory::init(const std::string_view process_name)
{
    detail::process_id = util::get_process_id(process_name);

    printf("[+] process id [%d]\n", detail::process_id);

    if (!detail::process_id)
    {
        printf("[!] failed to get process id of %s\n", process_name);
        return false;
    }

	detail::process_handle = OpenProcess(
		PROCESS_VM_READ |
		PROCESS_VM_WRITE,
		FALSE,
		detail::process_id
	);

	if (detail::process_handle == INVALID_HANDLE_VALUE)
	{
		printf("[!] failed to open process handle of (%d)%s\n", detail::process_id, process_name);
		return false;
	}

	printf("[+] process handle opened [0x%X]\n", detail::process_handle);

	detail::process_base_address = util::get_process_base_address(detail::process_id);

	if (!detail::process_base_address)
	{
		printf("[+] failed to get base address of process [0x%lX]\n", detail::process_base_address, GetLastError());
		return false;
	}

	printf("[+] base address [0x%llX]\n", detail::process_base_address);

    return true;
}

void memory::exit()
{
	if (detail::process_handle != INVALID_HANDLE_VALUE)
		CloseHandle(detail::process_handle);
}
