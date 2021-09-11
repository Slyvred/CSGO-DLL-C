#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>

const int dwForceJump = 0x524F21C;
const int dwLocalPlayer = 0xD8C2CC;
const int m_lifeState = 0x25F;
const int m_fFlags = 0x104;
const int m_bSpotted = 0x93D;
const int dwEntityList = 0x4DA542C;

#define FL_ONGROUND (1 << 0)

int client;
int localPlayer;
int playerState;
BYTE flags;

enum LifeState
{
	LIFE_ALIVE,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY
};

void Bunnyhop(bool enable)
{
	if (enable)
	{
		if (playerState == LIFE_ALIVE)
			if (GetAsyncKeyState(VK_SPACE) && flags & FL_ONGROUND)
				*(int*)(client + dwForceJump) = 6;
	}
}

void Radarhack(bool enable)
{
	if (enable)
	{
		for (int i = 1; i < 64; i++) // Looping through all entities
		{
			int ent = *(int*)((client + dwEntityList) + i * 0x10); // Accessing each one of them
			if (ent) *(bool*)(ent + m_bSpotted) = 1;
		}
	}
}

int Main(HMODULE hMod)
{
	// Grabbing client.dll base address
	client = (int)GetModuleHandle("client.dll");

	while (!GetAsyncKeyState(VK_END))
	{
		// Reading memory in real time
		localPlayer = *(int*)(client + dwLocalPlayer);
		playerState = *(int*)(localPlayer + m_lifeState);
		flags = *(BYTE*)(localPlayer + m_fFlags);

		// Calling our functions/features
		Bunnyhop(true);
		Radarhack(true);

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