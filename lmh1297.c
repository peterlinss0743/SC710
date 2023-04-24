#include "LINUXV4L2.h"
#include "lmh1297.h"
#include "sc0710.h"

void LMH1297_I2C_WRITE( CDevice* pDevice, BYTE nAddr, BYTE nData )
{
	ULONG nTxLen = 2;

	BYTE pTxBuf[ 2 ] = { nAddr, nData };

	SC0710_AccessI2cRegisterS_NEW( pDevice, 0x2D << 1, pTxBuf, nTxLen, NULL, 0, 0 );
}

BYTE LMH1297_I2C_READ( CDevice* pDevice, BYTE nAddr )
{
	ULONG nTxLen = 1;
	BYTE pTxBuf[ 2 ] = { nAddr, 0x00 };
	ULONG nRxLen = 1;
	BYTE pRxBuf[ 2 ] = { 0x00, 0x00 };

	SC0710_AccessI2cRegisterS_NEW( pDevice, 0x2D << 1, pTxBuf, nTxLen, pRxBuf, nRxLen, 0 );

	return pRxBuf[ 0 ];
}

void LMH1297_I2C_MASK_WRITE( CDevice* pDevice, BYTE nADDr, BYTE nMask, BYTE nData )
{
	BYTE nVal;

	nVal = LMH1297_I2C_READ( pDevice, nADDr ) & ~nMask;

	nVal |= nData & nMask;

	LMH1297_I2C_WRITE( pDevice, nADDr, nVal );



}

BYTE LMH1297_I2C_MASK_READ( CDevice* pDevice, BYTE nADDr, BYTE nMask )
{
	BYTE nVal = 0;

	nVal = LMH1297_I2C_READ( pDevice, nADDr ) & nMask;

	return nVal;
}

void LMH1297_INIT( CDevice* pDevice )
{





	LMH1297_SET_REGISTER_PAGE( pDevice, SHARED_PAGE);
	LMH1297_I2C_MASK_WRITE( pDevice, 0xFE, 0x02, 0x02);
	LMH1297_I2C_MASK_WRITE( pDevice, 0xFE, 0x01, 0x00);
	LMH1297_I2C_MASK_WRITE( pDevice, 0xFE, 0x20, 0x20);
	LMH1297_I2C_MASK_WRITE( pDevice, 0xFE, 0x10, 0x00);

	LMH1297_SET_REGISTER_PAGE( pDevice, EQ_PAGE);
	LMH1297_I2C_MASK_WRITE( pDevice, 0x31, 0x20, 0x20 );
	LMH1297_I2C_MASK_WRITE( pDevice, 0x31, 0x10, 0x00 );





	LMH1297_SET_REGISTER_PAGE( pDevice, CDR_PAGE);
	LMH1297_I2C_MASK_WRITE( pDevice, 0x3F, 0x08, 0x08 );
	LMH1297_I2C_MASK_WRITE( pDevice, 0x1C, 0x0C, 0x08 );
}

void LMH1297_CDR_STATE_MACHINE_RESET( CDevice* pDevice )
{

	LMH1297_SET_REGISTER_PAGE( pDevice, CDR_PAGE);

	LMH1297_I2C_MASK_WRITE( pDevice, 0x0A, 0x0C, 0x0C );

	LMH1297_I2C_MASK_WRITE( pDevice, 0x0A, 0x0C, 0x00 );
}

void LMH1297_SHARED_REGISTER_RESET( CDevice* pDevice )
{
	BYTE nVal;

	LMH1297_SET_REGISTER_PAGE( pDevice, SHARED_PAGE);

	LMH1297_I2C_MASK_WRITE( pDevice, 0x04, 0x40, 0x40 );

	LMH1297_I2C_MASK_WRITE( pDevice, 0xE2, 0x0F, 0x01 );


	do
	{
		nVal = LMH1297_I2C_MASK_READ( pDevice, 0xE2, 0x10 );
	}while( nVal != 0x10 );
}

void LMH1297_CDR_REGISTER_RESET( CDevice* pDevice )
{
	BYTE nVal;

	LMH1297_SET_REGISTER_PAGE( pDevice, CDR_PAGE);

	LMH1297_I2C_MASK_WRITE( pDevice, 0x00, 0x04, 0x04 );

	LMH1297_SET_REGISTER_PAGE( pDevice, SHARED_PAGE);

	LMH1297_I2C_MASK_WRITE( pDevice, 0xE2, 0x0F, 0x01 );


	do
	{
		nVal = LMH1297_I2C_MASK_READ( pDevice, 0xE2, 0x10 );
	}while( nVal != 0x10 );

	LMH1297_SET_REGISTER_PAGE( pDevice, CDR_PAGE);
}

void LMH1297_EQ_REGISTER_RESET( CDevice* pDevice )
{
	BYTE nVal;

	LMH1297_SET_REGISTER_PAGE( pDevice, EQ_PAGE);

	LMH1297_I2C_MASK_WRITE( pDevice, 0x00, 0x04, 0x04 );

	LMH1297_SET_REGISTER_PAGE( pDevice, SHARED_PAGE);

	LMH1297_I2C_MASK_WRITE( pDevice, 0xE2, 0x0F, 0x01 );


	do
	{
		nVal = LMH1297_I2C_MASK_READ( pDevice, 0xE2, 0x10 );
	}while( nVal != 0x10 );

	LMH1297_SET_REGISTER_PAGE( pDevice, EQ_PAGE);
}

BYTE LMH1297_IN0_Carrier_Detect( CDevice* pDevice )
{
	BYTE nVal;


	LMH1297_SET_REGISTER_PAGE( pDevice, EQ_PAGE);

	nVal = LMH1297_I2C_MASK_READ( pDevice, 0x02, 0x40 );

	return ( nVal != 0 );
}

BYTE LMH1297_LOCK_DATA_RATE( CDevice* pDevice )
{
	BYTE nVal, cnt;

	cnt = 0;

	LMH1297_SET_REGISTER_PAGE( pDevice, CDR_PAGE);

	LMH1297_I2C_MASK_WRITE( pDevice, 0x0C, 0xF0, 0x00 );

	while( 1 ){

			nVal = LMH1297_I2C_MASK_READ( pDevice, 0x02, 0x18 );

			if( nVal == 0x18 )
				break;

			cnt++;

			if( cnt > 100 )
				return LMH1297_UKN;


	}

	LMH1297_I2C_MASK_WRITE( pDevice, 0x0C, 0xF0, 0x30 );

	nVal = LMH1297_I2C_MASK_READ( pDevice, 0x02, 0x38 );

	LMH1297_I2C_MASK_WRITE( pDevice, 0x0C, 0xF0, 0x00 );

	return ( nVal >> 3 ) & 0x07;
}

BYTE LMH1297_GET_ID( CDevice* pDevice )
{
	BYTE nVal;

	LMH1297_SET_REGISTER_PAGE( pDevice, SHARED_PAGE);

	nVal = LMH1297_I2C_MASK_READ( pDevice, 0xF1, 0xFF );

	return nVal;
}

void LMH1297_SET_REGISTER_PAGE( CDevice* pDevice, BYTE nPAGE)
{
	LMH1297_I2C_MASK_WRITE( pDevice, 0xFF, 0x07, nPAGE );
}

void LMH1297_RESET( CDevice* pDevice )
{
	LMH1297_SHARED_REGISTER_RESET( pDevice );

	LMH1297_CDR_REGISTER_RESET( pDevice );

	LMH1297_EQ_REGISTER_RESET( pDevice );
}

void set_width_multiples( CDevice* pDevice, BYTE wm_ch0, BYTE wm_ch1, BYTE wm_ch2, BYTE wm_ch3 )
{
	SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	p_sys_cfg->width_multiples[0] = wm_ch0;
	p_sys_cfg->width_multiples[1] = wm_ch1;
	p_sys_cfg->width_multiples[2] = wm_ch2;
	p_sys_cfg->width_multiples[3] = wm_ch3;
}

void set_height_multiples( CDevice* pDevice, BYTE hm_ch0, BYTE hm_ch1, BYTE hm_ch2, BYTE hm_ch3 )
{
	SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	p_sys_cfg->height_multiples[0] = hm_ch0;
	p_sys_cfg->height_multiples[1] = hm_ch1;
	p_sys_cfg->height_multiples[2] = hm_ch2;
	p_sys_cfg->height_multiples[3] = hm_ch3;
}

void set_rx_rate( CDevice* pDevice, BYTE *nRATE_MODE, WORD *nRX_RATE )
{
	BYTE i, rate, first_rate, mode, first_mode;

	for( i = 0; i < 4; i++)
	{
		rate = ( *nRX_RATE >> ( i * 4 )) & 0xF;
		switch( rate )
		{
			case nRX_SD:
				mode = RX_MODE_SD;
				break;
			case nRX_HD:
				mode = RX_MODE_HD;
				break;
			case nRX_3G:
				mode = RX_MODE_3G;
				break;
			default:
				mode = 0;
		}

		if( i == 0 )
		{
			first_rate = rate;
			first_mode = mode;
			*nRATE_MODE |= (( mode & 0x3 ) << ( i * 2 ));
		}
		else
		{
			if( mode )
			{
				*nRATE_MODE |= (( mode & 0x3 ) << ( i * 2 ));
			}
			else
			{
				*nRATE_MODE |= (( first_mode & 0x3 ) << ( i * 2 ));
				*nRX_RATE   |= (( first_rate & 0xF ) << ( i * 4 ));
			}
		}
	}
}

void GET_RX_STAT( CDevice* pDevice, WORD *nRATE, BYTE *LOCKED )
{
#ifndef __EQ_1297
	WORD nGS12070_RATE;
#endif
	BYTE tmp_RATE;
	int i;

	*nRATE = 0;

	*LOCKED = 0;

#ifndef __EQ_1297
	for( i = 0; i < TOTAL_RX; i++ )
	{
		if( m_lmhxxxx->LMHXXXX_IN0_Carrier_Detect( i ))
		{
			*LOCKED |= ( 0x1 << i );



			if( LMH0324_ID == m_lmhxxxx->LMHXXXX_GET_ID( i ) )
			{
				tmp_RATE = m_lmhxxxx->LMHXXXX_GET_RATE( i );

				if( tmp_RATE == LMH0324_SD )
				{
					*nRATE |= ( nRX_SD << ( 4 * i ));
				}
				else if( tmp_RATE == LMH0324_HD_3G )
				{
					m_gspi->GS12070_GET_RATE( &nGS12070_RATE, NULL );

					if( (( nGS12070_RATE >> ( 2 * i )) & 0x3 ) == GS12070_3G_RATE )
					{
						*nRATE |= ( nRX_3G << ( 4 * i ));
					}
					else
					{
						*nRATE |= ( nRX_HD << ( 4 * i ));
					}
				}
			}
			else
			{
				tmp_RATE = m_lmhxxxx->LMHXXXX_LOCK_DATA_RATE( i );

				if( tmp_RATE == LMH1219_SD )
				{
					*nRATE |= ( nRX_SD << ( 4 * i ));
				}
				else if( tmp_RATE == LMH1219_HD )
				{
					*nRATE |= ( nRX_HD << ( 4 * i ));
				}
				else if( tmp_RATE == LMH1219_3G )
				{
					*nRATE |= ( nRX_3G << ( 4 * i ));
				}
				else if( tmp_RATE == LMH1219_6G )
				{
					*nRATE |= ( nRX_6G << ( 4 * i ));
				}
				else if( tmp_RATE == LMH1219_12G )
				{
					*nRATE |= ( nRX_12G << ( 4 * i ));
				}
			}

		}
	}
#else
	i = 0;

	if ( LMH1297_IN0_Carrier_Detect( pDevice ) )
	{
		*LOCKED |= (0x1 << i);

		tmp_RATE = LMH1297_LOCK_DATA_RATE( pDevice );

		if (tmp_RATE == LMH1297_SD)
		{
			*nRATE |= (nRX_SD << (4 * i));
		}
		else if (tmp_RATE == LMH1297_HD)
		{
			*nRATE |= (nRX_HD << (4 * i));
		}
		else if (tmp_RATE == LMH1297_3G)
		{
			*nRATE |= (nRX_3G << (4 * i));
		}
		else if (tmp_RATE == LMH1297_6G)
		{
			*nRATE |= (nRX_6G << (4 * i));
		}
		else if (tmp_RATE == LMH1297_12G)
		{
			*nRATE |= (nRX_12G << (4 * i));
		}

	}
#endif
}

void LMH1297_DISABLE_LOOP_OUT( CDevice* pDevice, BYTE f_disable )
{
	LMH1297_SET_REGISTER_PAGE( pDevice, SHARED_PAGE);
	LMH1297_I2C_MASK_WRITE( pDevice, 0xFE, 0x20, 0x20);
	if( f_disable )
	{
		LMH1297_I2C_MASK_WRITE( pDevice, 0xFE, 0x10, 0x10);
	}
	else
	{
		LMH1297_I2C_MASK_WRITE( pDevice, 0xFE, 0x10, 0x00);
	}
}
void LMH1297_OUT0_SETTING( CDevice* pDevice, BYTE f_reclock, BYTE vod_setting, BYTE de_emphasis_setting )
{
	LINUXV4L2_PRINT( KERN_INFO, "LMH1297_OUT0_SETTING() f_reclock(%d) vod_setting(%d) de_emphasis_setting(%d)\n", f_reclock, vod_setting, de_emphasis_setting );

	LMH1297_SET_REGISTER_PAGE(pDevice, CDR_PAGE);
    LMH1297_I2C_MASK_WRITE(pDevice, 0x09, 0x20, 0x20);
    LMH1297_I2C_MASK_WRITE(pDevice, 0x1C, 0x0C, 0x08);
    if( f_reclock )
    {
            LMH1297_I2C_MASK_WRITE(pDevice, 0x1E, 0xE0, 0x20);
    }
    else
    {
            LMH1297_I2C_MASK_WRITE(pDevice, 0x1E, 0xE0, 0x00);
    }

    LMH1297_SET_REGISTER_PAGE(pDevice, EQ_PAGE);
    LMH1297_I2C_MASK_WRITE(pDevice, 0x30, 0x20, 0x20);
    LMH1297_I2C_MASK_WRITE(pDevice, 0x30, 0x07, vod_setting & 0x07);

    LMH1297_I2C_MASK_WRITE(pDevice, 0x31, 0x40, 0x40);
    LMH1297_I2C_MASK_WRITE(pDevice, 0x31, 0x07, de_emphasis_setting & 0x07);
}

void Disable_EQ( CDevice* pDevice )
{
	LMH1297_SET_REGISTER_PAGE(pDevice, SHARED_PAGE);

	LMH1297_I2C_MASK_WRITE(pDevice, 0xFE, 0x20, 0x20 );
	LMH1297_I2C_MASK_WRITE(pDevice, 0xFE, 0x10, 0x10 );

	LMH1297_SET_REGISTER_PAGE(pDevice, EQ_PAGE);

	LMH1297_I2C_MASK_WRITE(pDevice, 0x1D, 0x80, 0x80 );
	LMH1297_I2C_MASK_WRITE(pDevice, 0x1D, 0x40, 0x40 );

	LMH1297_I2C_MASK_WRITE(pDevice, 0x31, 0x20, 0x20 );
	LMH1297_I2C_MASK_WRITE(pDevice, 0x31, 0x10, 0x10 );

	LMH1297_I2C_MASK_WRITE(pDevice, 0x00, 0x38, 0x38 );
}

void Enable_EQ( CDevice* pDevice )
{
	LMH1297_SET_REGISTER_PAGE(pDevice, SHARED_PAGE);

	LMH1297_I2C_MASK_WRITE(pDevice, 0xFE, 0x20, 0x00 );
	LMH1297_I2C_MASK_WRITE(pDevice, 0xFE, 0x10, 0x00 );

	LMH1297_SET_REGISTER_PAGE(pDevice, EQ_PAGE);

	LMH1297_I2C_MASK_WRITE(pDevice, 0x1D, 0x80, 0x00 );
	LMH1297_I2C_MASK_WRITE(pDevice, 0x1D, 0x40, 0x00 );

	LMH1297_I2C_MASK_WRITE(pDevice, 0x31, 0x20, 0x20 );
	LMH1297_I2C_MASK_WRITE(pDevice, 0x31, 0x10, 0x00 );

	LMH1297_I2C_MASK_WRITE(pDevice, 0x00, 0x38, 0x08 );
}