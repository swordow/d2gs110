/*
 * main.c: main routine of this program
 * 
 * 2001-08-20 faster
 *   add initialization routine and main loop of this program
 */

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <conio.h>
#include "d2gelib/d2server.h"
#include "d2gs.h"
#include "eventlog.h"
#include "vars.h"
#include "config.h"
#include "d2ge.h"
#include "net.h"
#include "timer.h"
#include "telnetd.h"
#include "d2gamelist.h"
#include "handle_s2s.h"


/* function declarations */
int  DoCleanup(void);
BOOL D2GSCheckRunning(void);
int  CleanupRoutineForServerMutex(void);
void D2GSShutdown(unsigned int exitCode);
void D2GSWatchDogInit();
void D2GSResetWatchDogCounter();
extern BOOL D2GSCheckGameInfo();
extern BOOL D2GSSaveAllGames(DWORD dwMilliseconds);
/* CTRL+C or CTRL+Break signal handler */
BOOL WINAPI ControlHandler(DWORD dwCtrlType);

/* some variables used just in this file */
static HANDLE			hD2GSMutex  = NULL;
static HANDLE			hStopEvent  = NULL;
static HANDLE			hWatchDog = NULL;
static CLEANUP_RT_ITEM	*pCleanupRT = NULL;
static CRITICAL_SECTION	csWatchDog;
static DWORD			dwShutdownStatus = 0;
static DWORD			dwShutdownTickCount = 0;
static DWORD			dwWatchDogCounter = 0;
/********************************************************************************
 * Main procedure begins here
 ********************************************************************************/
#ifdef _DEBUG
int main(int argc, char **argv)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	
	/* reset cleanup routine list */
	pCleanupRT = NULL;

	InitializeCriticalSection(&gsShutDown);

	/* init log system first */
	if (!D2GSEventLogInitialize()) return -1;

	d2gsconf.enablegslog = TRUE;
	D2GSEventLog("main", "Starting GS Server...");

	/* setup signal capture */
	SetConsoleCtrlHandler(ControlHandler, TRUE);

	/* check if another instance is running */
	if (D2GSCheckRunning()) {
		D2GSEventLog("main", "Seems another server is running");
		DoCleanup();
		return -1;
	}

	/* create a name event, for "d2gssvc" server controler to terminate me */
	hStopEvent= CreateEvent(NULL, TRUE, FALSE, D2GS_STOP_EVENT_NAME);
	if (!hStopEvent) {
		D2GSEventLog("main", "failed create stop event object");
		DoCleanup();
		return -1;
	}

	/* init variables */
	if (!D2GSVarsInitialize()) {
		D2GSEventLog("main", "Failed initialize global variables");
		DoCleanup();
		return -1;
	}

	/* read configurations */
	if (!D2GSReadConfig()) {
		D2GSEventLog("main", "Failed getting configurations from registry");
		DoCleanup();
		return -1;
	}

	/* start up game engine */
	if (!D2GEStartup()) {
		D2GSEventLog("main", "Failed Startup Game Engine");
		D2GSEventLog("main", "Please enable GELog, and see the message");
		DoCleanup();
		return -1;
	}

	/* initialize the net connection */
	if (!D2GSNetInitialize()) {
		D2GSEventLog("main", "Failed Startup Net Connector");
		DoCleanup();
		return -1;
	}

	/* administration console */
	if (!D2GSAdminInitialize()) {
		D2GSEventLog("main", "Failed Startup Administration Console");
		DoCleanup();
		return -1;
	}


	/* create timer */
	D2GSWatchDogInit();
	if (!D2GSTimerInitialize()) {
		D2GSEventLog("main", "Failed Startup Timer");
		DoCleanup();
		return -1;
	}

	/* main server loop */
	D2GSEventLog("main", "Entering Main Server Loop");
	while(WaitForSingleObject(hStopEvent, 1000) != WAIT_OBJECT_0) {}

	/* service controler tell me to stop now. "Yes, sir!" */
	D2GSSetD2CSMaxGameNumber(0);
	D2GSActive(FALSE);
	d2gsconf.enablegslog = TRUE;
	D2GSEventLog("main", "I am going to stop");
	d2gsconf.enablegslog = TRUE;
	D2GSSaveAllGames(0x1388);
	d2gsconf.enablegslog = FALSE;	
	Sleep(3000);
	D2GSShutdown(0);
	return 0;

} /* End of main() */


/*********************************************************************
 * Purpose: to add an cleanup routine item to the list
 * Return: TRUE(success) or FALSE(failed)
 *********************************************************************/
int CleanupRoutineInsert(CLEANUP_ROUTINE pRoutine, char *comment)
{
	CLEANUP_RT_ITEM		*pitem;

	if (pRoutine==NULL) return FALSE;
	pitem = (CLEANUP_RT_ITEM *)malloc(sizeof(CLEANUP_RT_ITEM));
	if (!pitem) {
		D2GSEventLog("CleanupRoutineInsert", "Can't alloc memory");
		return FALSE;
	}
	ZeroMemory(pitem, sizeof(CLEANUP_RT_ITEM));

	/* fill the structure */
	if (comment)
		strncpy(pitem->comment, comment, sizeof(pitem->comment)-1);
	else
		strncpy(pitem->comment, "unknown", sizeof(pitem->comment)-1);
	pitem->cleanup = pRoutine;
	pitem->next = pCleanupRT;
	pCleanupRT = pitem;

	return TRUE;

} /* End of CleanupRoutineInsert() */


/*********************************************************************
 * Purpose: call the cleanup routine to do real cleanup work
 * Return: TRUE or FALSE
 *********************************************************************/
int DoCleanup(void)
{
	CLEANUP_RT_ITEM		*pitem, *pprev;

	pitem = pCleanupRT;
	while(pitem)
	{
		D2GSEventLog("DoCleanup", "Calling cleanup routine '%s'", pitem->comment);
		pitem->cleanup();
		pprev = pitem;
		pitem = pitem->next;
		free(pprev);
	}
	pCleanupRT = NULL;

	/* at last, cleanup event log system */
	D2GSEventLog("DoCleanup", "Cleanup done.");
	D2GSEventLogCleanup();

	/* Close the mutex */
	if (hD2GSMutex)	CloseHandle(hD2GSMutex);
	if (hStopEvent) CloseHandle(hStopEvent);

#ifdef DEBUG_ON_CONSOLE
	printf("Press Any Key to Continue");
	_getch();
#endif

	return TRUE;

} /* End of DoCleanup() */


/*********************************************************************
 * Purpose: check if other instance is running
 * Return: TRUE(server is running) or FALSE(not running)
 *********************************************************************/
BOOL D2GSCheckRunning(void)
{
	HANDLE	hMutex;

	hD2GSMutex = NULL;
	hMutex = CreateMutex(NULL, TRUE, D2GSERVER_MUTEX_NAME);
	if (hMutex==NULL) {
		return TRUE;
	} else if (GetLastError()==ERROR_ALREADY_EXISTS) {
		CloseHandle(hMutex);
		return TRUE;
	} else {
		if (CleanupRoutineInsert(CleanupRoutineForServerMutex, "Server Mutex")) {
			hD2GSMutex = hMutex;
			return FALSE;
		} else {
			/* insert cleanup routine failed, assume server is running */
			CloseHandle(hMutex);
			return TRUE;
		}
	}

} /* End of D2GServerCheckRun() */


/*********************************************************************
 * Purpose: cleanup routine for release the global server mutex
 * Return: TRUE or FALSE
 *********************************************************************/
int CleanupRoutineForServerMutex(void)
{
	if (!hD2GSMutex) return FALSE;
	return CloseHandle(hD2GSMutex);

} /* End of CleanupRoutineServerMutex() */


/*********************************************************************
 * Purpose: catch CTRL+C or CTRL+Break signal
 * Return: TRUE or FALSE
 *********************************************************************/
BOOL WINAPI ControlHandler(DWORD dwCtrlType)
{
	switch( dwCtrlType )
	{
		case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
		case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
			D2GSEventLog("ControlHandler", "CTRL_BREAK or CTRL_C event caught");
			DoCleanup();
			ExitProcess(0);
			return TRUE;
			break;
    }
return FALSE;

} /* End of ControlHandler */


/*********************************************************************
 * Purpose: to close the server mutex
 * Return: none
 *********************************************************************/
void CloseServerMutex(void)
{
	if (hD2GSMutex) CloseHandle(hD2GSMutex);
	hD2GSMutex = NULL;

} /* End of CloseServerMutex() */


void D2GSBeforeShutdown(DWORD status, DWORD seconds)
{
	// GetTickCount milliseconds
	DWORD temp = 0;
	D2GSSetD2CSMaxGameNumber(0);
	D2GSActive(FALSE);
	EnterCriticalSection(&gsShutDown);
	dwShutdownStatus = status;
	dwShutdownTickCount = GetTickCount() + seconds * 1000;
	if (status == 1 || status == 3)
	{
		D2GSEventLog("D2GSShutdown", "Restart GS in %lu seconds", seconds);
	}
	else if (status == 2 || status == 4)
	{
		D2GSEventLog("D2GSShutdown", "Shutdown GS in %lu seconds", seconds);
	}
	LeaveCriticalSection(&gsShutDown);
}

DWORD D2GSGetShutdownStatus()
{
	DWORD status = 0;
	EnterCriticalSection(&gsShutDown);
	status = dwShutdownStatus;
	LeaveCriticalSection(&gsShutDown);
	return status;
}

void D2GSShutdown(unsigned int exitCode)
{
	if (D2GSCheckGameInfo())
	{
		D2GSSaveAllGames(0x1388);
		Sleep(0xBB8);
	}

	if (bGERunning != 0)
	{
		if (D2GSAfterEndWrapper() != 0)
		{
			D2GSAfterEndWrapper();
		}
	}

	ExitProcess(exitCode);
}


void D2GSShutdownTimer()
{
	static DWORD ShutdownCount = 0;
	char buffer[256] = { 0 };
	DWORD curTickCount = 0;
	DWORD overSeconds = 0;
	ShutdownCount++;
	if (ShutdownCount >= 0xA)
	{
		ShutdownCount = 0;
		EnterCriticalSection(&gsShutDown);

		if (dwShutdownStatus >= 1 && dwShutdownStatus <= 4)
		{
			curTickCount = GetTickCount();
			if (curTickCount > dwShutdownTickCount)
			{
				d2gsconf.enablegslog = 1;
				D2GSSaveAllGames(0x1388);
				Sleep(0xBB8);
				switch (dwShutdownStatus - 1)
				{
				case 0:
					d2gsconf.enablegslog = 1;
					D2GSEventLog("D2GSShutdownTimer", "Restart GS now");
					D2GSEventLogCleanup();
					d2gsconf.enablegslog = 0;
					CloseServerMutex();
					D2GSShutdown(0);
					break;
				case 1:
					d2gsconf.enablegslog = 1;
					D2GSEventLog("D2GSShutdownTimer", "Shutdown GS now");
					D2GSEventLogCleanup();
					d2gsconf.enablegslog = 0;
					CloseServerMutex();
					D2GSShutdown(1);
					break;
				case 2:
					d2gsconf.enablegslog = 1;
					D2GSEventLog("D2GSShutdownTimer", "D2CS Restart GS now");
					D2GSEventLogCleanup();
					d2gsconf.enablegslog = 0;
					CloseServerMutex();
					D2GSShutdown(0);
					break;
				case 3:
					d2gsconf.enablegslog = 1;
					D2GSEventLog("D2GSShutdownTimer", "D2CS Shutdown GS now");
					D2GSEventLogCleanup();
					d2gsconf.enablegslog = 0;
					CloseServerMutex();
					D2GSShutdown(1);
					break;
				default:
					break;
				}
			}
			else
			{
				overSeconds = (dwShutdownTickCount - curTickCount) / 1000;
				if ((overSeconds / 15) == 0)
				{
					switch (dwShutdownStatus - 1)
					{
					case 0:
						sprintf(buffer, "The game server will restart in %lu seconds", overSeconds);
						chat_message_announce_all(CHAT_MESSAGE_TYPE_SYS_MESSAGE, buffer);
						break;
					case 1:
						sprintf(buffer, "The game server will shutdown in %lu seconds", overSeconds);
						chat_message_announce_all(CHAT_MESSAGE_TYPE_SYS_MESSAGE, buffer);
						break;
					case 2:
						sprintf(buffer, "The realm will restart in %lu seconds", overSeconds);
						chat_message_announce_all(CHAT_MESSAGE_TYPE_SYS_MESSAGE, buffer);
						break;
					case 3:
						sprintf(buffer, "The realm will shutdown in %lu seconds", overSeconds);
						chat_message_announce_all(CHAT_MESSAGE_TYPE_SYS_MESSAGE, buffer);
						break;
					default:
						buffer[0] = 0;
						break;
					}
				}
			}
		}
		LeaveCriticalSection(&gsShutDown);
	}
	return;
}

DWORD CheckWatchDogCounter()
{
	EnterCriticalSection(&csWatchDog);
	dwWatchDogCounter++;
	if (dwWatchDogCounter < 0xF)
	{
		LeaveCriticalSection(&csWatchDog);
		return 0;
	}
	LeaveCriticalSection(&csWatchDog);
	return 1;
}

void D2GSResetWatchDogCounter()
{
	EnterCriticalSection(&csWatchDog);
	dwWatchDogCounter = 0;
	LeaveCriticalSection(&csWatchDog);
}

DWORD D2GSWatchDogThread(LPVOID p)
{
	while (TRUE)
	{
		Sleep(0x1770);
		if (CheckWatchDogCounter() != 0)
		{
			break;
		}
		if (D2GSCheckTickCount && D2GSCheckTickCountWrapper(0) != 0)
		{
			break;
		}
	}
	d2gsconf.enablegslog = TRUE;
	D2GSEventLog("watchdog_thread", "D2GS maybe in deadlock, restart it");
	d2gsconf.enablegslog = FALSE;
	CloseServerMutex();
	D2GSShutdown(0);
	return 0;
}

void D2GSWatchDogInit()
{
	DWORD dwThreadId = 0;
	hWatchDog = 0;
	InitializeCriticalSection(&csWatchDog);
	hWatchDog = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)D2GSWatchDogThread, 0, 0, &dwThreadId);
	if (!hWatchDog)
	{
		DWORD error = GetLastError();
		D2GSEventLog("watchdog_init", "CreateThread watchdog_thread. Code: %lu", error);
		return;
	}
	CloseHandle(hWatchDog);
	D2GSEventLog("watchdog_init", "CreateThread watchdog_thread, %lu", dwThreadId);
	return;
}