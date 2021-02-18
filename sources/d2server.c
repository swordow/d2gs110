#include "d2gs.h"
#include "d2ge.h"
#include "vars.h"
#include "d2gelib/d2server.h"

BOOL D2GSStartWrapper(LPD2GSINFO lpD2GSInfo)
{
	return D2GSStart(lpD2GSInfo);
}

BOOL D2GSNewEmptyGameWrapper(LPCSTR lpGameName, LPCSTR lpGamePass,
	LPCSTR lpGameDesc, DWORD dwGameFlag,
	BYTE  bTemplate, BYTE bReserved1,
	BYTE bReserved2, LPWORD pwGameId)
{
	return D2GSNewEmptyGame(lpGameName, lpGamePass, lpGameDesc, dwGameFlag, bTemplate, bReserved1, bReserved2, pwGameId);
}

BOOL D2GSSendDatabaseCharacterWrapper(DWORD dwClientId, LPVOID lpSaveData,
	DWORD dwSize, DWORD dwTotalSize, BOOL bLock,
	DWORD dwReserved1, LPPLAYERINFO lpPlayerInfo, DWORD dwReserved2)
{
	return D2GSSendDatabaseCharacter(dwClientId, lpSaveData, dwSize, dwTotalSize, bLock, dwReserved1, lpPlayerInfo, dwReserved2);
}

VOID D2GSRemoveClientFromGameWrapper(DWORD dwClientId)
{
	D2GSRemoveClientFromGame(dwClientId);
}

VOID D2GSEndAllGamesWrapper(VOID)
{
	D2GSEndAllGames();
}

DWORD D2GSSendClientChatMessageWrapper(DWORD dwClientId,
	DWORD dwType, DWORD dwColor, LPCSTR lpName, LPCSTR lpText)
{
	return D2GSSendClientChatMessage(dwClientId, dwType, dwColor, lpName, lpText);
}

DWORD D2GSSetTickCountWrapper(int TickCount)
{
	return D2GSSetTickCount(TickCount);
}

DWORD D2GSSetACDataWrapper(LPCSTR acstring)
{
	return D2GSSetACData(acstring);
}

DWORD D2GSLoadConfigWrapper(LPCSTR configfile)
{
	return D2GSLoadConfig(configfile);
}

DWORD D2GSAfterEndWrapper()
{
	return D2GSAfterEnd();
}

D2GEInitResult* D2GSInitConfigWrapper()
{
	return D2GSInitConfig();
}
DWORD D2GSCheckTickCountWrapper(DWORD count)
{
	return D2GSCheckTickCount(count);
}