#include "Pattern scanner.h"
#include <stdbool.h>

// https://youtu.be/mKUSLJjlajg

MODULEINFO GetModuleInfo(const char* szModule)
{
    MODULEINFO modInfo = { 0 };
    HMODULE hModule = GetModuleHandle(szModule);
    if (hModule == 0) return modInfo;

    GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));
    return modInfo;
}

int FindPattern(const char* mod, const char* pattern, const char* mask)
{
    MODULEINFO mInfo = GetModuleInfo(mod);

    int base = (int)mInfo.lpBaseOfDll;
    int size = (int)mInfo.SizeOfImage;

    int patternLength = (int)strlen(mask);

    for (int i = 0; i < size - patternLength; i++)
    {
        bool found = true;

        for (int j = 0; j < patternLength; j++)
        {
            found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
        }

        if (found) return base + i;
    }
    return 0; // = Fail
}

int Scan(const char* mod, const char* pattern, const char* mask, int offset, int extra) // My own code
{
    int Sig = FindPattern(mod, pattern, mask);
    Sig = (*(int*)(Sig + offset) - (int)GetModuleHandle(mod)) + extra;

    return Sig;
}