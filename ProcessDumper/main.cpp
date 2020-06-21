#include <Windows.h>
#include <iostream>

#include "memory.hpp"
#include "dump.hpp"

int main(int argc, char** argv)
{
    SetConsoleTitle("Process Dumper");

    if (argc < 2)
    {
        printf("[!] incorrect usage.\n[>] usage: %s abc.exe", argv[0]);
        return -1;
    }

    if (!memory::init(argv[1]))
    {
        printf("[+] failed to initialize memory\n");
        return -1;
    }

    if (!dumper::dump())
    {
        printf("[+] failed to dump process\n");
        return -1;
    }

    return 0;
}