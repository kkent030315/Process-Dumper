#include "dump.hpp"

bool dumper::dump()
{
    printf("[>] entering %s\n", __FUNCTION__);

    if (!memory::detail::process_id || 
        !memory::detail::process_handle || 
        !memory::detail::process_base_address
        )
    {
        printf("[!] memory is not initialized.\n");
        return false;
    }

    const auto& base_address = memory::detail::process_base_address;

    auto dos_header = memory::read<IMAGE_DOS_HEADER>(base_address);

    if (!dos_header.e_lfanew)
    {
        printf("[!] failed to get dos header [0x%lX]\n", GetLastError());
        return false;
    }

    printf("[+] dos header [0x%llX]\n", dos_header);

    auto nt_header = memory::read<IMAGE_NT_HEADERS>(base_address + dos_header.e_lfanew);

    if (!nt_header.OptionalHeader.SizeOfImage)
    {
        printf("[!] failed to get nt header [0x%lX]\n", GetLastError());
        return false;
    }

    printf("[+] nt header [0x%llX]\n", nt_header);

    const auto image_size = nt_header.OptionalHeader.SizeOfImage;

    auto buffer = (BYTE*)malloc(image_size);

    if (!buffer)
    {
        printf("[!] failed to allocate buffer [0x%lX]\n", GetLastError());
        return false;
    }

    printf("[+] buffer allocated at [0x%p]\n", buffer);

    for (ULONG iterator = 0x0; iterator < image_size; iterator += 0x1000)
    {
        if (!memory::read_buffer(base_address + iterator, buffer + iterator, 0x1000))
        {
            printf("[!] failed to read buffer at [0x%lX] LastError: [0x%lX]\n", 
                base_address + iterator, GetLastError());

            free(buffer);
            return false;
        }
    }

    auto pdos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(buffer);

    if (!pdos_header->e_lfanew)
    {
        printf("[!] failed to get dos header from buffer\n");
        free(buffer);
        return false;
    }

    printf("[+] dos_header readed [%p]\n", pdos_header);

    if (pdos_header->e_magic != IMAGE_DOS_SIGNATURE)
    {
        printf("[!] invalid dos header signature\n");
        free(buffer);
        return false;
    }

    auto pnt_header = reinterpret_cast<PIMAGE_NT_HEADERS>(buffer + pdos_header->e_lfanew);

    if (!pnt_header)
    {
        printf("[!] failed to read nt header from buffer\n");
        free(buffer);
        return false;
    }

    printf("[+] nt header readed [0x%p]\n", pnt_header);

    if (pnt_header->Signature != IMAGE_NT_SIGNATURE)
    {
        printf("[!] invalid nt header signature from readed nt header\n");
        free(buffer);
        return false;
    }

    auto poptional_header = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(&pnt_header->OptionalHeader);

    if (!poptional_header)
    {
        printf("[!] failed to read optional header from buffer\n");
        free(buffer);
        return false;
    }

    printf("[+] optional header readed [0x%p]\n", poptional_header);

    int i = 0;
    unsigned int section_offset = poptional_header->SizeOfHeaders;

    for (
        PIMAGE_SECTION_HEADER psection_header = IMAGE_FIRST_SECTION(pnt_header);
        i < pnt_header->FileHeader.NumberOfSections;
        i++, psection_header++
        )
    {
        psection_header->Misc.VirtualSize = psection_header->SizeOfRawData;

        memcpy(buffer + section_offset, psection_header, sizeof(IMAGE_SECTION_HEADER));
        section_offset += sizeof(IMAGE_SECTION_HEADER);

        if (!memory::read_buffer(
            poptional_header->ImageBase + psection_header->VirtualAddress,
            buffer + psection_header->PointerToRawData,
            psection_header->SizeOfRawData
        ))
        {
            printf("[!] failed to read buffer from headers\n");
            free(buffer);
            return false;
        }
    }

    free(buffer);

    printf("[>] leaving %s\n", __FUNCTION__);

    return true;
}
