/*
 * vars.c: global variables with its initialization and cleanup
 *
 * 2001-08-20 faster
 *   begining
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "d2gs.h"
#include "eventlog.h"
#include "versioncheck.h"
#include "charlist.h"


/*===================================================================*/
/* global variables */
D2GSCONFIGS			d2gsconf;
AutoUpdateSetting	gAutoUpdate;
BOOL				bGERunning;
HANDLE				hGEThread;
CRITICAL_SECTION	csGameList;
CRITICAL_SECTION	gsShutDown;
const char*			gD2GEVersionString;
char				PathName[260];
/*===================================================================*/


/* functions */
int  CleanupRoutineForVars(void);


/*********************************************************************
 * Purpose: to initialize the global variables
 * Return: TRUE or FALSE
 *********************************************************************/
int D2GSVarsInitialize(void)
{
	DWORD	val;
	char* lastSlash = 0;

	ZeroMemory(&d2gsconf, sizeof(d2gsconf));
	ZeroMemory(&gAutoUpdate, sizeof(gAutoUpdate));

	/* set current working dir */
	ZeroMemory(PathName, 260);
	GetModuleFileName(NULL, PathName, 260);

	bGERunning = FALSE;
	hGEThread = 0;

	lastSlash = strrchr(PathName, '\\');
	if (lastSlash)
	{
		(*(lastSlash)) = 0;
		SetCurrentDirectory(PathName);
		D2GSEventLog("D2GSInit", "Set current working directory to \"%s\"", PathName);
	}

	/* calculate file checksum */
	val = VersionCheck();
	if (!val) {
		D2GSEventLog("D2GSVarsInitialize", "Failed calculating file checksum");
		return FALSE;
	}
	d2gsconf.checksum = val;

	/* initialize char list table */
	if (charlist_init(DEFAULT_HASHTBL_LEN)!=0) {
		D2GSEventLog("D2GSVarsInitialize", "Failed initialize charlist table");
		return FALSE;
	}

	/* initialize the CriticalSection Objects */
	InitializeCriticalSection(&csGameList);

	if (CleanupRoutineInsert(CleanupRoutineForVars, "Global Variables")) {
		return TRUE;
	} 

	/* do some cleanup before quiting */
	CleanupRoutineForVars();
	return FALSE;
} /* End of D2GSVarsInitialize() */


/*********************************************************************
 * Purpose: to cleanup the global variables
 * Return: TRUE or FALSE
 *********************************************************************/
int CleanupRoutineForVars(void)
{
	/* destroy all the CriticalSection Objects */
	DeleteCriticalSection(&csGameList);

	/* destroy char list table */
	charlist_destroy();

	return TRUE;

} /* End of CleanupRoutineForVars() */

