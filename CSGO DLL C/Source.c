#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>

const int dwForceJump = 0x524F20C;
const int dwForceAttack = 0x31D59A4;
const int dwLocalPlayer = 0xD8C2CC;
const int dwEntityList = 0x4DA541C;
const int m_lifeState = 0x25F;
const int m_fFlags = 0x104;
const int m_bSpotted = 0x93D;
const int m_bDormant = 0xED;
const int m_iTeamNum = 0xF4;
const int m_iCrosshairId = 0xB3E8;

#define FL_ONGROUND (1 << 0)

int client, localPlayer;

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
	if (enable == true)
	{
		BYTE flags = *(BYTE*)(localPlayer + m_fFlags);

		if (GetAsyncKeyState(VK_SPACE) && flags & FL_ONGROUND)
			*(int*)(client + dwForceJump) = 6;
	}
}

void Radarhack(bool enable)
{
	if (enable == true)
	{
		for (int i = 1; i < 64; i++) // Looping through all entities
		{
			int ent = *(int*)((client + dwEntityList) + i * 0x10); // Accessing each one of them
			if (ent) *(bool*)(ent + m_bSpotted) = 1;
		}
	}
}

void Triggerbot(bool enable, int vKey)
{
	if (enable == true)
	{
		int crossId = *(int*)(localPlayer + m_iCrosshairId);
		int myTeam = *(int*)(localPlayer + m_iTeamNum);

		if (crossId && crossId < 64)
		{
			int ent = *(int*)((client + dwEntityList) + (crossId - 1) * 0x10);

			if (ent && !*(bool*)(ent + m_bDormant))
			{
				int entTeam = *(int*)(ent + m_iTeamNum);

				if (entTeam != myTeam)
					if (GetAsyncKeyState(vKey))
						*(int*)(client + dwForceAttack) = 6;
			}
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

		// Calling our functions/features
		if (localPlayer && *(int*)(localPlayer + m_lifeState) == LIFE_ALIVE)
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