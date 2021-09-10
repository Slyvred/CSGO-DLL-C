#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

const int dwForceJump = 0x524F21C;
const int dwLocalPlayer = 0xD8C2CC;
const int m_lifeState = 0x25F;
const int m_fFlags = 0x104;
#define FL_ONGROUND (1 << 0)

enum LifeState
{
	LIFE_ALIVE,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY
};

int Main(HMODULE hMod)
{
	int client = (int)GetModuleHandle("client.dll");

	while (!GetAsyncKeyState(VK_END))
	{
		int localPlayer = *(int*)(client + dwLocalPlayer);
		int playerState = *(int*)(localPlayer + m_lifeState);
		BYTE flags = *(BYTE*)(localPlayer + m_fFlags);

		if (playerState == LIFE_ALIVE)
			if (GetAsyncKeyState(VK_SPACE) && flags & FL_ONGROUND)
				*(int*)(client + dwForceJump) = 6;

		Sleep(5);
	}

	FreeLibraryAndExitThread(hMod, EXIT_SUCCESS);
}

BOOL APIENTRY DllMain(HMODULE hMod, intptr_t dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hMod);
		HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Main, hMod, 0, NULL);
		if (hThread) CloseHandle(hThread);
	}

	return TRUE;
}