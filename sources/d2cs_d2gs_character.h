#ifndef INCLUDED_D2CS_D2GS_CHARACTER_H
#define INCLUDED_D2CS_D2GS_CHARACTER_H

#ifdef D2CS
# include "common/bn_type.h"
#endif

#ifdef D2GS
# include "bn_types.h"
#pragma pack(push, pack01, 1)
#endif 

#define D2CHARINFO_MAGICWORD		0x12345678
#define D2CHARINFO_VERSION			0x00010000
#define D2CHARINFO_PORTRAIT_PADSIZE	30

typedef struct
{
	//00
	bn_int		magicword;		/* static for check */
	//04
	bn_int		version;		/* charinfo file version */
	//08
	bn_int		create_time;	/* character creation time */
	//0c
	bn_int		last_time;		/* character last access time */
	//10
	bn_int		checksum;
	//14
	bn_int		total_play_time;
	//18
	bn_int		reserved[6]; 
	//0x30
	unsigned char	charname[MAX_CHARNAME_LEN];
	//0x40
	unsigned char	account[MAX_ACCTNAME_LEN];
	//0x50
	unsigned char	realmname[MAX_REALMNAME_LEN];
	//0x70
} t_d2charinfo_header;// 0x70 bytes

typedef struct
{
	bn_int		experience;
	bn_int		charstatus;
	bn_int		charlevel;
	bn_int		charclass;
} t_d2charinfo_summary;

typedef struct
{
        bn_short        header;	/* 0x84 0x80 */
        bn_byte         gfx[11];
        bn_byte         class;
        bn_byte         color[11];
        bn_byte         level;
        bn_byte         status;
        bn_byte         u1[3];
		bn_byte			ladder;
        bn_byte         u2[2];
        bn_byte         end;	/* 0x00 */
} t_d2charinfo_portrait;//34bytes

typedef struct
{
	t_d2charinfo_header		header;//112bytes
	t_d2charinfo_portrait	portrait;//34bytes
	bn_byte					pad[D2CHARINFO_PORTRAIT_PADSIZE];//30bytes
	t_d2charinfo_summary	summary;//16bytes
} t_d2charinfo_file;//192bytes

#ifdef D2GS
#pragma pack(pop, pack01)
#endif

#endif
