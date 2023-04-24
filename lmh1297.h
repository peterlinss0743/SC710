#ifndef __LMH1297_H__
#define __LMH1297_H__

#include "LINUXV4L2.h"

#define __EQ_1297

enum{
	LMH1297_12G = 0x0,
	LMH1297_6G  = 0x1,
	LMH1297_3G  = 0x2,
	LMH1297_HD  = 0x3,
	LMH1297_SD  = 0x4,
	LMH1297_UKN = 0xF,
};

enum{
	LMH1297_ID = 0x6B,
};

enum{
	SHARED_PAGE = 0x0,
	CDR_PAGE    = 0x4,
	EQ_PAGE     = 0x5,
};

enum
{
	nRX_unkown = 0,
	nRX_SD  = 1,
	nRX_HD  = 2,
	nRX_3G  = 3,
	nRX_6G  = 4,
	nRX_12G = 5,
};

enum
{
	RX_MODE_SD = 0x1,
	RX_MODE_HD = 0x2,
	RX_MODE_3G = 0x3,

	RX_MODE_4_HD = 0xAA,
	RX_MODE_4_3G = 0xFF,
};

#define DDI0_TO_DDO0 0x0004
#define DDI0_TO_DDO1 0x0040
#define DDI0_TO_DDO2 0x0400
#define DDI0_TO_DDO3 0x4000

#define DDI0_TO_ALL_OUTPUT (DDI0_TO_DDO0|DDI0_TO_DDO1|DDI0_TO_DDO2|DDI0_TO_DDO3)

void LMH1297_SET_REGISTER_PAGE( CDevice* pDevice, BYTE nPAGE);
void GET_RX_STAT( CDevice* pDevice, WORD *nRATE, BYTE *LOCKED );

void Disable_EQ( CDevice* pDevice );
void Enable_EQ( CDevice* pDevice );

#endif
