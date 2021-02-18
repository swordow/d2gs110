#ifndef INCLUDED_D2CS_D2GS_PROTOCOL_H
#define INCLUDED_D2CS_D2GS_PROTOCOL_H
/* 
 * there is only little comments in this header file
 * check the documents for more details
*/

/* include your own header files here */
#ifdef D2CS
# include "common/bn_type.h"
#endif

#ifdef D2GS
# include "bn_types.h"
#pragma pack(push, pack01, 1)
#endif

typedef struct
{
	bn_short	size;
	bn_short	type;
	bn_int		seqno;			/* seqno, set by the sender */
} t_d2cs_d2gs_header;

typedef struct
{
	t_d2cs_d2gs_header	h;
} t_d2cs_d2gs_generic;

#define D2CS_D2GS_AUTHREQ		0x10
//0x08+0x08 = 0x10 bytes
typedef struct
{
	t_d2cs_d2gs_header	h;
	bn_int			sessionnum;
	bn_int			signlen;
	/* realm name */
	/* key check sum, maybe 128 bytes */
} t_d2cs_d2gs_authreq;

#define D2GS_D2CS_AUTHREPLY		0x11
typedef struct
{
	t_d2cs_d2gs_header	h;
	bn_int			version;
	bn_int			checksum;
	bn_int			randnum;
	bn_int			signlen;
	bn_basic		sign[128];
} t_d2gs_d2cs_authreply;//0x98 bytes

#define D2CS_D2GS_AUTHREPLY		0x11
//0x08 + 0x04 = 0x0C bytes
typedef struct
{
	t_d2cs_d2gs_header	h;
	//0x08
	bn_int			reply;
} t_d2cs_d2gs_authreply;

#define D2CS_D2GS_AUTHREPLY_SUCCEED			0x00
#define D2CS_D2GS_AUTHREPLY_BAD_VERSION		0x01
#define D2CS_D2GS_AUTHREPLY_BAD_CHECKSUM	0x02

#define D2GS_D2CS_SETGSINFO		0x12
typedef struct
{
	t_d2cs_d2gs_header	h;
	bn_int			maxgame;
	bn_int			gameflag;
} t_d2gs_d2cs_setgsinfo;

#define D2CS_D2GS_SETGSINFO		0x12
typedef struct
{
	t_d2cs_d2gs_header      h;
	bn_int                  maxgame;
	bn_int                  gameflag;
} t_d2cs_d2gs_setgsinfo;

#define D2CS_D2GS_ECHOREQ		0x13
typedef struct {
	t_d2cs_d2gs_header	h;
} t_d2cs_d2gs_echoreq;


#define D2GS_D2CS_ECHOREPLY	0x13
typedef struct {
	t_d2cs_d2gs_header	h;
} t_d2gs_d2cs_echoreply;

#define D2CS_D2GS_CONTROL		0x14
typedef struct
{
	t_d2cs_d2gs_header      h;
	bn_int                  cmd;
	bn_int                  value;
} t_d2cs_d2gs_control;
#define D2CS_D2GS_CONTROL_CMD_RESTART	0x01
#define D2CS_D2GS_CONTROL_CMD_SHUTDOWN	0x02
#define D2CS_D2GS_CONTROL_VALUE_DEFAULT	0x00

#define D2CS_D2GS_SETINITINFO		0x15
typedef struct
{
	t_d2cs_d2gs_header	h;
	//0x08
	bn_int			time;
	//0x0C
	bn_int			gs_id;
	//0x10
	bn_int			ac_version;
	//0x14
	/* ac_checksum 1024 bytes*/
} t_d2cs_d2gs_setinitinfo;

#define D2CS_D2GS_SETCONFFILE		0x16
typedef struct
{
	//00
	t_d2cs_d2gs_header	h;
	//08
	bn_int			size;
	//0c
	bn_int			reserved1;
	//10
	/* conf file (null terminated string) */
} t_d2cs_d2gs_setconffile;

#define D2CS_D2GS_CREATEGAMEREQ		0x20
typedef struct
{
	//0x00
	t_d2cs_d2gs_header	h;
	//0x08
	bn_byte			ladder;
	//0x09
	bn_byte			expansion;
	//0x0a
	bn_byte			difficulty;
	//0x0b
	bn_byte			hardcore;
	//0x0c
	/* gamename */
	/* gamepass */
	/* gamedesc */
	/* create by acctname */
	/* create by charname */
	/* create by ip address(string) */
} t_d2cs_d2gs_creategamereq;
#define D2GAME_DIFFICULTY_NORMAL	0
#define D2GAME_DIFFICULTY_NIGHTMARE	1
#define D2GAME_DIFFICULTY_HELL		2

#define D2GS_D2CS_CREATEGAMEREPLY	0x20
typedef struct
{
	t_d2cs_d2gs_header	h;
	//08
	bn_int			result;
	//0c
	bn_int			gameid;
} t_d2gs_d2cs_creategamereply;
#define D2GS_D2CS_CREATEGAME_SUCCEED	0
#define D2GS_D2CS_CREATEGAME_FAILED		1


#define D2CS_D2GS_JOINGAMEREQ		0x21
typedef struct
{
	t_d2cs_d2gs_header	h;
	//0x08
	bn_int			gameid;
	//0x0c
	bn_int			token;
	//0x10
	/* character name */
	/* account name */
	/* client ip address(string) */
} t_d2cs_d2gs_joingamereq;

#define D2GS_D2CS_JOINGAMEREPLY		0x21
typedef struct
{
	t_d2cs_d2gs_header	h;
	bn_int			result;
	bn_int			gameid;
} t_d2gs_d2cs_joingamereply;
#define D2GS_D2CS_JOINGAME_SUCCEED			0
#define D2GS_D2CS_JOINGAME_FAILED			1

#define D2GS_D2CS_UPDATEGAMEINFO	0x22
typedef struct
{
	t_d2cs_d2gs_header	h;
	bn_int			flag;
	bn_int			gameid;
	bn_int			charlevel;
	bn_int			charclass;
	/* charname */
} t_d2gs_d2cs_updategameinfo;
#define D2GS_D2CS_UPDATEGAMEINFO_FLAG_UPDATE	0
#define D2GS_D2CS_UPDATEGAMEINFO_FLAG_ENTER		1
#define D2GS_D2CS_UPDATEGAMEINFO_FLAG_LEAVE		2

#define D2GS_D2CS_CLOSEGAME		0x23
typedef struct
{
	t_d2cs_d2gs_header	h;
	bn_int			gameid;
} t_d2gs_d2cs_closegame;

#ifdef D2GS
#pragma pack(pop, pack01)
#endif

#endif
