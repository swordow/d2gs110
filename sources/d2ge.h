#ifndef INCLUDED_D2GE_H
#define INCLUDED_D2GE_H
#include "d2gelib/d2server.h"
/* const */
#define D2GE_INIT_TIMEOUT		(60*1000)
#define D2GE_SHUT_TIMEOUT		(5*1000)

/* functions */
int   D2GEStartup(void);
int   D2GECleanup(void);
int   D2GEThreadInit(void);
DWORD WINAPI D2GEThread(LPVOID lpParameter);


/* local function */
static BOOL D2GSGetInterface(void);
static DWORD __stdcall D2GSErrorHandle(void);

extern D2GSINFO		gD2GSInfo;
#endif /* INCLUDED_D2GE_H */