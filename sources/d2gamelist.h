#ifndef INCLUDED_D2GAMELIST_H
#define INCLUDED_D2GAMELIST_H

/*
 * define structure to store game info, store character info in the game
 * and the functions to maintain the game and character list
 */

#include <windows.h>
#include "d2gs.h"
#include "bn_types.h"
#include "list.h"

/* structures */
typedef struct RAW_D2CHARINFO {
	// 0x00
	bn_char		AcctName[MAX_ACCTNAME_LEN];	/* account name */
	// 0x10
	bn_char		CharName[MAX_CHARNAME_LEN];	/* char name */
	// 0x20 guess add
	bn_char		RealmIPName[MAX_REALMIPNAME_LEN];	/* realm name */
	// 0x30
	bn_int		token;
	// 0x34
	bn_int		CharLevel;
	// 0x38
	bn_short	CharClass;
	// 0x3A
	bn_short	TickCount;
	// 0x3C
	bn_short	EnterGame;
	// 0x3E
	bn_short	AllowLadder;
	// 0x40
	bn_short	CharLockStatus;
	// 0x42
	// padding?
	bn_short	Padding0;
	// 0x44
	DWORD		EnterTime;
	// 0x48
	DWORD		CharCreateTime;
	// 0x4C
	DWORD		ClientId;
	// 0x50
	WORD		GameId;
	// padding0x52
	WORD		Padding1;
	// 0x54
	struct RAW_D2GAMEINFO	*lpGameInfo;	/* pointer to the GAMEINFO */
	// 0x58
	struct RAW_D2CHARINFO	*prev;
	// 0x5C
	struct RAW_D2CHARINFO	*next;
} D2CHARINFO, *PD2CHARINFO, *LPD2CHARINFO;// total 0x60 bytes

typedef struct RAW_D2GAMEINFO {
	//0x00
	bn_char		GameName[MAX_GAMENAME_LEN];
	//0x10
	bn_char		GamePass[MAX_GAMEPASS_LEN];
	//0x20
	bn_char		GameDesc[MAX_GAMEDESC_LEN];
	//0x40
	bn_char		CreatorAccoutName[MAX_ACCTNAME_LEN];
	//0x50
	bn_char		CreatorCharName[MAX_CHARNAME_LEN];
	//0x60
	bn_char		CreatorIP[MAX_REALMIPNAME_LEN];
	// 0x70
	bn_byte		ladder;		// for 1.10
	// 0x71
	bn_byte		expansion;
	// 0x72
	bn_byte		difficulty;
	// 0x73
	bn_byte		hardcore;
	// 0x74
	bn_byte		reserved;
	// 0x75 padding??
	bn_byte		padding;
	// 0x76
	WORD		GameId;
	// 0x78
	WORD		CharCount;
	// padding 0x7A 0x7B
	WORD		Padding;
	// 0x7C
	DWORD		CreateTime;
	// 0x80
	DWORD		disable;
	// 0x84
	struct RAW_D2CHARINFO	*lpCharInfo;
	// 0x88
	struct RAW_D2GAMEINFO	*prev;
	// 0x8c
	struct RAW_D2GAMEINFO	*next;
} D2GAMEINFO, *PD2GAMEINFO, *LPD2GAMEINFO;


typedef struct RAW_D2GETDATAREQUEST {
	//0x00
	bn_char		AcctName[MAX_ACCTNAME_LEN];	/* account name */
	//0x10
	bn_char		CharName[MAX_CHARNAME_LEN];	/* char name */
	//0x20
	DWORD		ClientId;
	//0x24
	DWORD		Seqno;
	//0x28
	bn_short	TickCount;
	bn_short    Padding;
	//0x2C
	struct RAW_D2GETDATAREQUEST *prev;
	//0x30
	struct RAW_D2GETDATAREQUEST *next;
} D2GETDATAREQUEST, *PD2GETDATAREQUEST, *LPD2GETDATAREQUEST;

typedef struct RAW_MOTDCLIENT {
	struct list_head	list;
	DWORD				ClientId;
} MOTDCLIENT, *PMOTDCLIENT, *LPMOTDCLIENT;

/* macro defination */
#define D2GSIncCurrentGameNumber()		{ currentgamenum++; }
#define D2GSDecCurrentGameNumber()		{ currentgamenum--; }

/* functions */
void D2GSResetGameList(void);
int  D2GSGetCurrentGameNumber(void);
int  D2GSGetCurrentGameStatistic(DWORD *gamenum, DWORD *usernum);
void D2GSDeleteAllCharInGame(D2GAMEINFO *lpGameInfo);
int  D2GSGameListInsert(
	UCHAR* GameName,
	UCHAR* GamePass,
	UCHAR* GameDesc,
	UCHAR* CreatorAccoutName,
	UCHAR* CreatorCharName,
	UCHAR* CreatorIP,
	UCHAR expansion,
	UCHAR difficulty, UCHAR hardcore, UCHAR ladder, WORD wGameId);
int  D2GSGameListDelete(D2GAMEINFO *lpGameInfo);
int  D2GSInsertCharIntoGameInfo(D2GAMEINFO *lpGameInfo, DWORD token, UCHAR *AcctName,
					UCHAR *CharName, UCHAR* RealmIPName, DWORD CharLevel, WORD CharClass, WORD EnterGame);
int  D2GSDeleteCharFromGameInfo(D2GAMEINFO *lpGameInfo, D2CHARINFO *lpCharInfo);
int  D2GSInsertCharIntoPendingList(
	DWORD token,
	UCHAR* AcctName,
	UCHAR* CharName,
	UCHAR* RealmIPName,
	DWORD CharLevel,
	WORD CharClass,
	D2GAMEINFO* lpGame);
int  D2GSDeletePendingChar(D2CHARINFO *lpCharInfo);
int  D2GSInsertGetDataRequest(UCHAR *AcctName, UCHAR *CharName, DWORD dwClientId, DWORD dwSeqno);
int  D2GSDeleteGetDataRequest(D2GETDATAREQUEST *lpGetDataReq);
D2GAMEINFO *D2GSFindGameInfoByGameId(WORD GameId);
D2GAMEINFO *D2GSFindGameInfoByGameName(UCHAR *GameName);
D2CHARINFO *D2GSFindCharInGameByCharName(D2GAMEINFO *lpGame, UCHAR *CharName);
D2CHARINFO *D2GSFindPendingCharByToken(DWORD token);
D2CHARINFO *D2GSFindPendingCharByCharName(UCHAR *CharName);
D2GETDATAREQUEST *D2GSFindGetDataRequestBySeqno(DWORD dwSeqno);
void D2GSPendingCharTimerRoutine(void);
void D2GSGetDataRequestTimerRoutine(void);
void FormatTimeString(long t, u_char *buf, int len, int format);
void D2GSShowGameList(unsigned int ns);
void D2GSShowCharInGame(unsigned int ns, WORD GameId);
void D2GSDisableGameByGameId(unsigned int ns, WORD GameId);
void D2GSEnableGameByGameId(unsigned int ns, WORD GameId);

int chat_message_announce_all(DWORD dwMsgType, const char *msg);
int chat_message_announce_game(DWORD dwMsgType, WORD GameId, const char *msg);
int chat_message_announce_game2(DWORD dwMsgType, D2GAMEINFO *lpGame, const char *msg);
int chat_message_announce_char(DWORD dwMsgType, const char *CharName, const char *msg);
int chat_message_announce_char2(DWORD dwMsgType, DWORD dwClientId, const char *msg);

int D2GSMOTDAdd(DWORD dwClientId);
int D2GSSendMOTD(void);
void D2GSCheckGameLife();
BOOL D2GSSaveAllGames(DWORD dwMilliseconds);
#endif /* INCLUDED_D2GAMELIST_H */