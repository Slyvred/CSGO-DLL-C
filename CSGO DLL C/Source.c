#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>
#include "Pattern scanner.h"

// Signatures (they change regularly so we need to dynamically find them)
struct signatures 
{
	int dwForceJump;
	int dwForceAttack;
	int dwLocalPlayer;
	int dwEntityList;
}signatures;

// Netvars (they almost never change)
struct netvars
{
	int m_lifeState;
	int m_fFlags;
	int m_bSpotted;
	int m_bDormant;
	int m_iTeamNum;
	int m_iCrosshairId;
}netvars;

// Entity life states
enum LifeState
{
	LIFE_ALIVE,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY
};

// Entity on ground flag
#define FL_ONGROUND (1 << 0)

// Global vars
int client, localPlayer;

void ScanPatterns()
{
	// Credits for the patterns goes to https://github.com/frk1/hazedumper
	// My pattern formatting script: https://pastebin.com/cKQAf6WE

	signatures.dwForceJump = Scan("client.dll", "\x8B\x0D\x00\x00\x00\x00\x8B\xD6\x8B\xC1\x83\xCA\x02", "xx????xxxxxxx", 2, 0);
	signatures.dwForceAttack = Scan("client.dll", "\x89\x0D\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x8B\xF2\x8B\xC1\x83\xCE\x04", "xx????xx????xxxxxxx", 2, 0);
	signatures.dwLocalPlayer = Scan("client.dll", "\x8D\x34\x85\x00\x00\x00\x00\x89\x15\x00\x00\x00\x00\x8B\x41\x08\x8B\x48\x04\x83\xF9\xFF", "xxx????xx????xxxxxxxxx", 3, 4);
	signatures.dwEntityList = Scan("client.dll", "\xBB\x00\x00\x00\x00\x83\xFF\x01\x0F\x8C\x00\x00\x00\x00\x3B\xF8", "x????xxxxx????xx", 1, 0);
}

void Init()
{
	// Grabbing client.dll base address
	client = (int)GetModuleHandle("client.dll");

	// Init Signatures
	ScanPatterns();

	// Init Netvars
	netvars.m_lifeState = 0x25F;
	netvars.m_fFlags = 0x104;
	netvars.m_bSpotted = 0x93D;
	netvars.m_bDormant = 0xED;
	netvars.m_iTeamNum = 0xF4;
	netvars.m_iCrosshairId = 0xB3E8;
}


void Bunnyhop(bool enable)
{
	if (enable == true)
	{
		BYTE flags = *(BYTE*)(localPlayer + netvars.m_fFlags);

		if (GetAsyncKeyState(VK_SPACE) && flags & FL_ONGROUND)
			*(int*)(client + signatures.dwForceJump) = 6;
	}
}

void Radarhack(bool enable)
{
	if (enable == true)
	{
		for (int i = 1; i < 64; i++) // Looping through all entities
		{
			int ent = *(int*)((client + signatures.dwEntityList) + i * 0x10); // Accessing each one of them
			if (ent) *(bool*)(ent + netvars.m_bSpotted) = 1;
		}
	}
}

void Triggerbot(bool enable, int vKey)
{
	if (enable == true)
	{
		int crossId = *(int*)(localPlayer + netvars.m_iCrosshairId);
		int myTeam = *(int*)(localPlayer + netvars.m_iTeamNum);

		if (crossId && crossId < 64)
		{
			int ent = *(int*)((client + signatures.dwEntityList) + (crossId - 1) * 0x10);

			if (ent && !*(bool*)(ent + netvars.m_bDormant))
			{
				int entTeam = *(int*)(ent + netvars.m_iTeamNum);

				if (GetAsyncKeyState(vKey) && entTeam != myTeam)
					*(int*)(client + signatures.dwForceAttack) = 6;
			}
		}
	}
}

int WINAPI Main(HMODULE hMod)
{
	// Initializing global variables
	Init();

	while (!GetAsyncKeyState(VK_END))
	{
		// Reading memory in real time
		localPlayer = *(int*)(client + signatures.dwLocalPlayer);

		// Calling our functions/features
		if (localPlayer && *(int*)(localPlayer + netvars.m_lifeState) == LIFE_ALIVE)
		{
			Bunnyhop(true);
			Radarhack(true);
			Triggerbot(true, 'A');
		}

		// Sleep to save some cpu
		Sleep(5);
	}

	FreeLibraryAndExitThread(hMod, EXIT_SUCCESS);
}

BOOL APIENTRY DllMain(HMODULE hMod, int dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hMod);
		HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Main, hMod, 0, NULL);
		if (hThread) CloseHandle(hThread);
	}

	return TRUE;
}