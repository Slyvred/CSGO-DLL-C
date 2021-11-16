#pragma once
#include <Windows.h>
#include <Psapi.h>

// credits @gh for the first two
MODULEINFO GetModuleInfo(const char* szModule);
int FindPattern(const char* mod, const char* pattern, const char* mask);
int Scan(const char* mod, const char* pattern, const char* mask, int offset, int extra);