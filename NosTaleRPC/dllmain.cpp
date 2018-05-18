#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include "discord_rpc.h"
#include <ctime>

FILE *dummyStream;
const DWORD charInfoAddr = 0x85155C;

static const char* APPLICATION_ID = "445187290210369547";
int iq = 13;

static void handleDiscordReady(const DiscordUser* connectedUser)
{
	printf("\nDiscord: connected to user %s#%s - %s\n",
		connectedUser->username,
		connectedUser->discriminator,
		connectedUser->userId);
}

static void handleDiscordDisconnected(int errcode, const char* message)
{
	printf("\nDiscord: disconnected (%d: %s)\n", errcode, message);
}

static void handleDiscordError(int errcode, const char* message)
{
	printf("\nDiscord: error (%d: %s)\n", errcode, message);
}

static void handleDiscordJoin(const char* secret)
{
	printf("\nDiscord: join (%s)\n", secret);
}


void Init()
{
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	handlers.ready = handleDiscordReady;
	handlers.disconnected = handleDiscordDisconnected;
	handlers.errored = handleDiscordError;
	handlers.joinGame = handleDiscordJoin;
	Discord_Initialize(APPLICATION_ID, &handlers, 0, NULL);
}

void Update()
{
	char buffer[256];
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.state = "Testing RPC";
	sprintf(buffer, "HoshiIQ: %d", iq);
	discordPresence.details = buffer;
	discordPresence.largeImageKey = "32523";
	discordPresence.largeImageText = "SCHIZO";
	discordPresence.startTimestamp = time(0);
	discordPresence.partyId = "party1234";
	discordPresence.partySize = 1337;
	discordPresence.partyMax = 2137;
	discordPresence.instance = 0;
	Discord_UpdatePresence(&discordPresence);
}

DWORD WINAPI DLLStart(LPVOID param)
{
	Init();
	while (true)
	{
		Update();
		Discord_RunCallbacks();
		Sleep(30 * 1000);
	}
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			AllocConsole();
			freopen_s(&dummyStream, "CONOUT$", "wb", stdout);
			freopen_s(&dummyStream, "CONOUT$", "wb", stderr);
			freopen_s(&dummyStream, "CONIN$", "rb", stdin);
			std::cout << "In DEBUG mode\n";
			CreateThread(0, 0, DLLStart, 0, 0, 0);
			break;
		}
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
		{
			Discord_Shutdown();
			std::cout << "Shut down\n";
			Sleep(5000);
			break;
		}
	}
	return TRUE;
}
