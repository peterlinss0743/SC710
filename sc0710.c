#include "LINUXV4L2.h"
#include "property.h"
#include "sc0710.h"

#include "nosg.h"
#include "hdcp.h"

#include "lmh1297.h"
#include "upaprom_example.c"


#ifndef ABS
#define ABS(x) (((x)>=0)?(x):(-(x)))
#endif



















extern struct snd_pcm_hardware LINUXV4L2_SND_PCM_HARDWARE_SC0710;



static const int SC0710_I2C_24C02_ADR7	= 0x000000A2 >> 1;

static const int SC0710_I2C_24C00_ADR7    = 0x000000A0 >> 1;

static const int SC0710_I2C_MCU_ADR7_0X31 = 0x00000031;

static const int SC0710_I2C_MCU_ADR7_0X32 = 0x00000032;

static const int SC0710_I2C_MCU_ADR7_0X33 = 0x00000033;

static const int SC0710_I2C_COMPLETE_BIT = 0x00000001;

static const int SC0710_I2C_SUCCESS_BIT = 0x00000002;



extern CDevice* g_pDevice[128];

ULONG SC0710_GetRegister(CDevice* pDevice, ULONG dwIndex)
{
	return READ_REGISTER_ULONG((ULONG*)(pDevice->m_pRegBaseCommonBuffer[0] + dwIndex));
}

BOOLEAN SC0710_SetRegister(CDevice* pDevice, ULONG dwIndex, ULONG dwValue)
{
	WRITE_REGISTER_ULONG((ULONG*)(pDevice->m_pRegBaseCommonBuffer[0] + dwIndex), (DWORD)(dwValue));

	return TRUE;
}

BOOLEAN SC0710_AccessI2cRegisterS(CDevice* pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE* pTxBuf, ULONG nTxLen, BYTE* pRxBuf, ULONG nRxLen)
{
	DWORD* p_tx_buf = (DWORD*)(pTxBuf);

	DWORD* p_rx_buf = (DWORD*)(pRxBuf);

	BOOLEAN is_success = FALSE;

	ULONG   i = 0;

	if( ((pDevice->idVendor == 0x12AB) && (pDevice->idProduct & 0xFFFF) == 0x0750) ) {

		return TRUE;
	}

	for (i = 0; i < nTxLen; i += 4) {

		ULONG m = i / 4;

		SC0710_SetRegister(pDevice, 0x000000B4 + (m * 4), p_tx_buf[m]);

		if(i > 16) {

			break;
		}
	}
	{	ULONG n_dev_addr = (bDevAddr << 1);

	DWORD dw_i2c_cmd = (n_dev_addr << 24) | (nTxLen << 16) | (nRxLen << 8);

	SC0710_SetRegister(pDevice, 0x000000B0, dw_i2c_cmd | 0x01);
	}
	for (i = 0; i < 100; i++) {

		ULONG R000000C4 = SC0710_GetRegister(pDevice, 0x000000C4);

		if((R000000C4 & SC0710_I2C_COMPLETE_BIT) != 0) {

			is_success = TRUE;

			break;
		}
		DELAY_100NS(10000);
	}
	if(is_success == FALSE) {

		LINUXV4L2_PRINT(KERN_INFO, "SC0710_AccessI2cRegisterS( CH%02X.%04X.%04X.%d.%d ) ERROR\n", pDevice->m_nKsDeviceNumber, nDevNum, bDevAddr, nTxLen, nRxLen);
	}
	for (i = 0; i < nRxLen; i += 4) {

		ULONG m = i / 4;



		p_rx_buf[m] = SC0710_GetRegister(pDevice, 0x00000090 + (m * 4));



		if(i > 24) {

			break;
		}
	}
	{	DWORD dw_i2c_cmd = 0x00000000;

	SC0710_SetRegister(pDevice, 0x000000B0, dw_i2c_cmd);
	}
	DELAY_100NS(100000);

	return is_success;
}

BOOLEAN SC0710_AccessI2cRegisterS_NEW(CDevice* pDevice, BYTE bDevAddr, BYTE* pTxBuf, ULONG nTxLen, BYTE* pRxBuf, ULONG nRxLen, ULONG DELAY_100US)
{
	BOOLEAN is_success = FALSE;

	ULONG i = 0;

	ULONG R00000104 = 0x00000000;

	if(nTxLen > 0) {

		SC0710_SetRegister(pDevice, 0x00003000 + 0x00000100, 0x00000002);

		SC0710_SetRegister(pDevice, 0x00003000 + 0x00000100, 0x00000001);

		SC0710_SetRegister(pDevice, 0x00003000 + 0x00000108, 0x00000100 | bDevAddr);

		for (i = 0; i < nTxLen; i++) {
			int count = 0;
			do {

				R00000104 = SC0710_GetRegister(pDevice, 0x00003000 + 0x00000104);

				if((R00000104 & 0x00000010) == 0x00000000) {

					break;
				}
				DELAY_100NS(10000);

				count++;

			} while (count < 100);

			if(i == nTxLen - 1) {

				if((nTxLen > 0) && (nRxLen > 0)) {

					SC0710_SetRegister(pDevice, 0x00003000 + 0x00000108, 0x00000000 | pTxBuf[i]);
				}
				else {

					SC0710_SetRegister(pDevice, 0x00003000 + 0x00000108, 0x00000200 | pTxBuf[i]);
				}
			}
			else {

				SC0710_SetRegister(pDevice, 0x00003000 + 0x00000108, pTxBuf[i]);
			}
		}
	}
	if((nTxLen > 0) && (nRxLen > 0) && (DELAY_100US > 0)) {

		DELAY_100NS(DELAY_100US * 1000);
	}
	else {



		if(nRxLen) {

			ULONG _delay = 1;

			_delay += nTxLen;

			if(_delay > 10) {

				_delay = 10;
			}

			DELAY_100NS((_delay * 100000));
		}
		else {

			DELAY_100NS(10000);
		}
	}



	if(nTxLen > 0) {

		for (i = 0; i < 10; i++)
		{
			R00000104 = SC0710_GetRegister(pDevice, 0x00003000 + 0x00000104);

			if((R00000104 & 0x00000080) == 0x00000080) {

				is_success = TRUE;

				break;
			}
			DELAY_100NS(10000);
		}
		if((R00000104 & 0x00000080) == 0x00000000) {

			LINUXV4L2_PRINT(KERN_INFO, "I2C WRITE ERROR %d.%d!!\n", nTxLen, nRxLen);

			is_success = FALSE;
		}
	}

	if(nRxLen > 0) {

		SC0710_SetRegister(pDevice, 0x00003000 + 0x00000120, 0x0000000F);

		SC0710_SetRegister(pDevice, 0x00003000 + 0x00000100, 0x00000002);

		SC0710_SetRegister(pDevice, 0x00003000 + 0x00000100, 0x00000000);

		SC0710_SetRegister(pDevice, 0x00003000 + 0x00000108, 0x00000101 | bDevAddr);

		SC0710_SetRegister(pDevice, 0x00003000 + 0x00000108, 0x00000200 | nRxLen);

		SC0710_SetRegister(pDevice, 0x00003000 + 0x00000100, 0x00000001);

		for (i = 0; i < nRxLen; i++) {

			const int n_max_wait_count = 100;

			int n_wait_count = 0;

			for (n_wait_count = 0; n_wait_count < n_max_wait_count; n_wait_count++) {

				R00000104 = SC0710_GetRegister(pDevice, 0x00003000 + 0x00000104);

				if((R00000104 & 0x00000040) == 0x00000000) {

					break;
				}
				else {

					DELAY_100NS(10000);
				}
			}
			if(n_wait_count >= n_max_wait_count) {

				ULONG j = 0;

				for (j = 0; j < nRxLen; j++) {

					pRxBuf[j] = (BYTE)(j);
				}
				break;
			}
			else {

				ULONG R0000010C = SC0710_GetRegister(pDevice, 0x00003000 + 0x0000010C);

				pRxBuf[i] = (BYTE)(R0000010C & 0x000000FF);
			}
		}
	}



	if(nRxLen > 0) {

		for (i = 0; i < 2; i++) {

			R00000104 = SC0710_GetRegister(pDevice, 0x00003000 + 0x00000104);

			if((R00000104 & 0x00000040) == 0x00000040) {

				is_success = TRUE;

				break;
			}
			DELAY_100NS(10000);
		}
		if((R00000104 & 0x00000040) == 0x00) {

			LINUXV4L2_PRINT(KERN_INFO, "I2C READ ERROR %d.%d!!\n", nTxLen, nRxLen);

			is_success = FALSE;
		}
	}
	return is_success;
}

BOOLEAN SC0710_AccessMcuRegisterS(CDevice* pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE* pTxBuf, ULONG nTxLen, BYTE* pRxBuf, ULONG nRxLen)
{
	unsigned int  i2c_proxy_txlen = 0;

	unsigned int  i2c_proxy_rxlen = 0;

	unsigned char i2c_proxy_txbuf[32] = { 0 };

	unsigned char i2c_proxy_rxbuf[32] = { 0 };

	BOOLEAN       is_success = FALSE;

	if(nTxLen > 0) {

		if(nRxLen > 0) {

			i2c_proxy_txlen = nTxLen + 2;

			i2c_proxy_txbuf[0] = (bDevAddr << 1) | 1;

			i2c_proxy_txbuf[1] = (unsigned char)(nRxLen);

			RtlCopyMemory(i2c_proxy_txbuf + 2, pTxBuf, nTxLen);
		}
		else {

			i2c_proxy_txlen = nTxLen + 1;

			i2c_proxy_txbuf[0] = (bDevAddr << 1) | 0;

			RtlCopyMemory(i2c_proxy_txbuf + 1, pTxBuf, nTxLen);
		}
		if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

			(pDevice->iManufacturer == 0x1A) ||

			(pDevice->iManufacturer == 0x1B) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x29) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ||

			(pDevice->iManufacturer == 0x2D)) {

			is_success = SC0710_AccessI2cRegisterS_NEW(pDevice, SC0710_I2C_MCU_ADR7_0X33 << 1, i2c_proxy_txbuf, i2c_proxy_txlen, NULL, 0, 0);
		}
		else {



			is_success = SC0710_AccessI2cRegisterS_NEW(pDevice, SC0710_I2C_MCU_ADR7_0X33 << 1, i2c_proxy_txbuf, i2c_proxy_txlen, NULL, 0, 0);
		}
		DELAY_100NS(2500000);
	}
	if(nRxLen > 0) {

		i2c_proxy_rxlen = nRxLen;

		RtlFillMemory(i2c_proxy_rxbuf, nRxLen, 0x00);

		if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

			(pDevice->iManufacturer == 0x1A) ||

			(pDevice->iManufacturer == 0x1B) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x29) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ||

			(pDevice->iManufacturer == 0x2D)) {

			is_success = SC0710_AccessI2cRegisterS_NEW(pDevice, SC0710_I2C_MCU_ADR7_0X33 << 1, NULL, 0, i2c_proxy_rxbuf, i2c_proxy_rxlen, 0);
		}
		else {



			is_success = SC0710_AccessI2cRegisterS_NEW(pDevice, SC0710_I2C_MCU_ADR7_0X33 << 1, NULL, 0, i2c_proxy_rxbuf, i2c_proxy_rxlen, 0);
		}
		RtlCopyMemory(pRxBuf, i2c_proxy_rxbuf, nRxLen);
	}
	DELAY_100NS(100000);

	return is_success;
}

BOOLEAN SC0710_AccessSlaveDeviceRegisterS(CDevice* pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE* pTxBuf, ULONG nTxLen, BYTE* pRxBuf, ULONG nRxLen)
{
	if(SC0710_I2C_MCU_ADR7_0X32 == bDevAddr ||

		SC0710_I2C_MCU_ADR7_0X33 == bDevAddr ||

		SC0710_I2C_24C02_ADR7    == bDevAddr ||

		SC0710_I2C_24C00_ADR7    == bDevAddr ||

		0x36 == bDevAddr) {

		SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

		if(p_sys_cfg->n_firmware_version < 0x20190301) {

			DELAY_100NS(10000);

			return SC0710_AccessI2cRegisterS(pDevice, nDevNum, bDevAddr, pTxBuf, nTxLen, pRxBuf, nRxLen);
		}
		else {

			return SC0710_AccessI2cRegisterS_NEW(pDevice, bDevAddr << 1, pTxBuf, nTxLen, pRxBuf, nRxLen, 0);
		}
	}
	else {

		return SC0710_AccessMcuRegisterS(pDevice, nDevNum, bDevAddr, pTxBuf, nTxLen, pRxBuf, nRxLen);
	}
	return FALSE;
}

BOOLEAN SC0710_SetGpioValue( CDevice * pDevice, BYTE bBitNum, BYTE bValue )
{
	if( ((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ||

		((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ) {

		unsigned char rxbuf[ 3 ] = { 0 };

		unsigned char txbuf[ 8 ][ 5 ] = { { 0x12, 0x34, 0x91, 0x00, 0x00 },
										  { 0x12, 0x34, 0x91, 0x01, 0x00 },
										  { 0x12, 0x34, 0x91, 0x02, 0x00 },
										  { 0x12, 0x34, 0x91, 0x03, 0x00 },
										  { 0x12, 0x34, 0x91, 0x04, 0x00 },
										  { 0x12, 0x34, 0x91, 0x05, 0x00 },
										  { 0x12, 0x34, 0x95, 0x0F, 0x00 },
										  { 0x12, 0x34, 0x90, 0x00, 0x00 },
		};
		txbuf[ bBitNum ][ 4 ] = bValue;

		SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, 0x55, txbuf[ bBitNum ], 5, rxbuf, 0 ); DELAY_100NS( 1000000 );
	}
	return TRUE;
}

BYTE SC0710_GetGpioValue( CDevice * pDevice, BYTE bBitNum )
{
	if( ((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ||

		((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ) {

		unsigned char rxbuf[ 3 ] = { 0 };

		unsigned char txbuf[ 8 ][ 4 ] = { { 0x12, 0x34, 0x91, 0x00 },
										  { 0x12, 0x34, 0x91, 0x01 },
										  { 0x12, 0x34, 0x91, 0x02 },
										  { 0x12, 0x34, 0x91, 0x03 },
										  { 0x12, 0x34, 0x91, 0x04 },
										  { 0x12, 0x34, 0x91, 0x05 },
										  { 0x12, 0x34, 0x95, 0x0F },
										  { 0x12, 0x34, 0x90, 0x00 },
		};
		SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, 0x55, txbuf[ bBitNum ], 4, rxbuf, 3 ); DELAY_100NS( 1000000 );

		if( rxbuf[ 1 ] == txbuf[ bBitNum ][ 2 ] ) {

			return (BYTE)(rxbuf[ 2 ]);
		}
		else {

			;
		}
	}
	return 0x00;
}

DWORD SC0710_GetGpioValueS( CDevice * pDevice, ULONG bBitMask )
{
	if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||

		((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ||

		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1B)) ||

		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1B)) ||

		((pDevice->bcdDevice == 0x0B04) && (pDevice->iManufacturer == 0x1B)) ||

		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1D)) ||

		((pDevice->bcdDevice == 0x0A04) && (pDevice->iManufacturer == 0x1D)) ||

		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||

		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F)) ||

		((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||

		((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D))) {

		BYTE L_PinMask = (BYTE)((bBitMask & 0x00FF) >> 0);

		BYTE H_PinMask = (BYTE)((bBitMask & 0xFF00) >> 8);

		ULONG PinValue = 0x0000;

		unsigned char rxbuf[ 4 ] = { 0 };

		unsigned char txbuf[ 5 ] = { 0x12, 0x34, 0x98, L_PinMask, H_PinMask };

		SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, 0x55, txbuf, 5, rxbuf, 3 ); DELAY_100NS( 1000000 );

		if( rxbuf[ 0 ] == 0x99 ) {

			PinValue = ((rxbuf[ 1 ] << 8) | (rxbuf[ 2 ] << 0));

			return PinValue;
		}
	}
	return 0x00;
}

BOOLEAN SC0710_SetGpioValueS( CDevice * pDevice, ULONG bBitMask, DWORD dwValue )
{
	if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||

		((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ||

		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1B)) ||

		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1B)) ||

		((pDevice->bcdDevice == 0x0B04) && (pDevice->iManufacturer == 0x1B)) ||

		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1D)) ||

		((pDevice->bcdDevice == 0x0A04) && (pDevice->iManufacturer == 0x1D)) ||

		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||

		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F)) ||

		((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||

		((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D))) {

		BYTE L_PinMask  = (BYTE)((bBitMask & 0x00FF) >> 0);

		BYTE H_PinMask  = (BYTE)((bBitMask & 0xFF00) >> 8);

		BYTE L_PinValue = (BYTE)((dwValue & 0x00FF) >> 0);

		BYTE H_PinValue = (BYTE)((dwValue & 0xFF00) >> 8);

		unsigned char rxbuf[ 4 ] = { 0 };

		unsigned char txbuf[ 7 ] = { 0x12, 0x34, 0x98, L_PinMask, L_PinValue, H_PinMask, H_PinValue };

		SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, 0x55, txbuf, 7, rxbuf, 0 ); DELAY_100NS( 1000000 );
	}
	return TRUE;
}

BOOLEAN	SC0710_SetAnalogVideoDecoderRegister(CDevice* pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE nBank, BYTE bIndex, BYTE nValue)
{
	return FALSE;
}

BYTE SC0710_GetAnalogVideoDecoderRegister(CDevice* pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE nBank, BYTE bIndex)
{
	return FALSE;
}





#define SC0710_TX_FIFO_MAX  256

#define SC0710_RX_FIFO_MAX  252

static const int SC0710_FLASH_BLOCK_SIZE = 0x00010000;

BOOLEAN SC0710_AccessSpiRegisterS(CDevice* pDevice, ULONG nCmd, ULONG* pTxBuf, ULONG nTxLen, ULONG* pRxBuf, ULONG nRxLen)
{
	ULONG i = 0;

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	{

		p_sys_cfg->n_spi_ctrl_reg |= (SC0710_SPI_CTRL_RESET_RX_FIFO | SC0710_SPI_CTRL_RESET_TX_FIFO);

		SC0710_SetRegister(pDevice, 0x00001060, p_sys_cfg->n_spi_ctrl_reg);

		p_sys_cfg->n_spi_ctrl_reg &= ~(SC0710_SPI_CTRL_RESET_RX_FIFO | SC0710_SPI_CTRL_RESET_TX_FIFO);
	}
	{

		SC0710_SetRegister(pDevice, 0x00001070, 0x00000000);
	}
	{

		p_sys_cfg->n_spi_ctrl_reg &= ~SC0710_SPI_CTRL_TRANS_STOP;

		SC0710_SetRegister(pDevice, 0x00001060, p_sys_cfg->n_spi_ctrl_reg);
	}



	SC0710_SetRegister(pDevice, 0x00001068, nCmd);

	for (i = 0; i < nTxLen; i++) {

		SC0710_SetRegister(pDevice, 0x00001068, pTxBuf[i]);
	}



	for (i = 0; i < nRxLen; i++) {

		SC0710_SetRegister(pDevice, 0x00001068, 0x00000000);
	}

	DELAY_100NS(10000);

	{

		ULONG R00001064 = SC0710_GetRegister(pDevice, 0x00001064);
	}
	{

		SC0710_SetRegister(pDevice, 0x00001070, 0x00000001);
	}
	{

		p_sys_cfg->n_spi_ctrl_reg |= SC0710_SPI_CTRL_TRANS_STOP;

		SC0710_SetRegister(pDevice, 0x00001060, p_sys_cfg->n_spi_ctrl_reg);
	}



	if(nRxLen == 0) {

		return TRUE;
	}
	{
		ULONG R00001078 = SC0710_GetRegister(pDevice, 0x00001078);

		if(R00001078 < (nTxLen + nRxLen)) {

			LINUXV4L2_PRINT(KERN_INFO, "SC0710_AccessSpiRegisterS( CH%02X: %d / %d ) ERROR!!\n", pDevice->m_nKsDeviceNumber, R00001078, (nTxLen + nRxLen));
		}
	}



	SC0710_GetRegister(pDevice, 0x0000106C);

	for (i = 0; i < nTxLen; i++) {

		SC0710_GetRegister(pDevice, 0x0000106C);
	}
	for (i = 0; i < nRxLen; i++) {

		pRxBuf[i] = SC0710_GetRegister(pDevice, 0x0000106C);
	}
	return TRUE;
}

BOOLEAN SC0710_AccessSpiRegisterS_PORT2(CDevice* pDevice, ULONG nCmd, ULONG* pTxBuf, ULONG nTxLen, ULONG* pRxBuf, ULONG nRxLen)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	ULONG i = 0;

	{

		p_sys_cfg->n_spi_ctrl_reg |= (SC0710_SPI_CTRL_RESET_RX_FIFO | SC0710_SPI_CTRL_RESET_TX_FIFO);

		SC0710_SetRegister(pDevice, 0x00002060, p_sys_cfg->n_spi_ctrl_reg);

		p_sys_cfg->n_spi_ctrl_reg &= ~(SC0710_SPI_CTRL_RESET_RX_FIFO | SC0710_SPI_CTRL_RESET_TX_FIFO);
	}
	{

		SC0710_SetRegister(pDevice, 0x00002070, 0x00000000);
	}
	{

		p_sys_cfg->n_spi_ctrl_reg &= ~SC0710_SPI_CTRL_TRANS_STOP;

		SC0710_SetRegister(pDevice, 0x00002060, p_sys_cfg->n_spi_ctrl_reg);
	}



	SC0710_SetRegister(pDevice, 0x00002068, nCmd);

	for (i = 0; i < nTxLen; i++) {

		SC0710_SetRegister(pDevice, 0x00002068, pTxBuf[i]);
	}



	for (i = 0; i < nRxLen; i++) {

		SC0710_SetRegister(pDevice, 0x00002068, 0x00000000);
	}

	DELAY_100NS(10000);

	{

		ULONG R00002064 = SC0710_GetRegister(pDevice, 0x00002064);
	}
	{

		SC0710_SetRegister(pDevice, 0x00002070, 0x00000001);
	}
	{

		p_sys_cfg->n_spi_ctrl_reg |= SC0710_SPI_CTRL_TRANS_STOP;

		SC0710_SetRegister(pDevice, 0x00002060, p_sys_cfg->n_spi_ctrl_reg);
	}



	if(nRxLen == 0) {

		return TRUE;
	}
	{
		ULONG R00002078 = SC0710_GetRegister(pDevice, 0x00002078);

		if(R00002078 < (nTxLen + nRxLen)) {

			LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_AccessSpiRegisterS_PORT2() nCmd(0x%x) R00002078(0x%x) nTxLen(0x%x) nRxLen(0x%x) ERROR!!\n", pDevice->m_nKsDeviceNumber, nCmd, R00002078, nTxLen, nRxLen);
		}
	}


	SC0710_GetRegister(pDevice, 0x0000206C);

	for (i = 0; i < nTxLen; i++) {

		SC0710_GetRegister(pDevice, 0x0000206C);
	}
	for (i = 0; i < nRxLen; i++) {

		pRxBuf[i] = SC0710_GetRegister(pDevice, 0x0000206C);
	}
	return TRUE;
}

static const int P2140UpdateCodeAddr_old = 0x00200000;
static const int P2140Timer2CodeAddr_old = 0x003C0000;
static const int P2140GoldenCodeAddr     = 0x00000000;
static const int P2140Timer1CodeAddr     = 0x001BFC00;
static const int P2140UpdateCodeAddr     = 0x001C0000;
static const int P2140Timer2CodeAddr     = 0x00380000;
static const int P2180UpdateCodeAddr     = 0x00240000;
static const int P2180Timer2CodeAddr     = 0x00480000;

ULONG  tx_buf[SC0710_TX_FIFO_MAX + 3];

BOOLEAN SC0710_SetP2140FlashData(CDevice* pDevice, BYTE* pBuffer, ULONG nBufferSize)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	ULONG n_fpga_update_start_addr = P2140UpdateCodeAddr;

	ULONG n_fpga_update_stop_addr = P2140Timer2CodeAddr;

	ULONG i = 0;

	ULONG lengths = 0;

	if( ((pDevice->idVendor == 0x12AB) && (pDevice->idProduct & 0xFFFF) == 0x0720) ) {

		n_fpga_update_start_addr = P2180UpdateCodeAddr;

		n_fpga_update_stop_addr = P2180Timer2CodeAddr;
	}
	else {

		if( p_sys_cfg->n_firmware_version < 0x20190301 ) {

			n_fpga_update_start_addr = P2140UpdateCodeAddr_old;

			n_fpga_update_stop_addr = P2140Timer2CodeAddr_old;
		}
		else {

			n_fpga_update_start_addr = P2140UpdateCodeAddr;

			n_fpga_update_stop_addr = P2140Timer2CodeAddr;
		}
	}
	{
		int n_allowed_write_size = n_fpga_update_stop_addr - n_fpga_update_start_addr;

		int n_erase_size = (nBufferSize + SC0710_FLASH_BLOCK_SIZE - 1) / SC0710_FLASH_BLOCK_SIZE * SC0710_FLASH_BLOCK_SIZE;

		if(n_erase_size > n_allowed_write_size) {

			return FALSE;
		}
	}
	for (i = 0; i < nBufferSize; i += SC0710_FLASH_BLOCK_SIZE) {



		ULONG  address = n_fpga_update_start_addr + i;

		ULONG  tx_buf[3];

		BYTE* po = (BYTE*)(&address);

		LINUXV4L2_PRINT(KERN_INFO, "FIRMWARE.ERASED: %d / %d BYTES", i, nBufferSize);

		tx_buf[0] = po[2];

		tx_buf[1] = po[1];

		tx_buf[2] = po[0];

		SC0710_AccessSpiRegisterS(pDevice, 0x00000006 , NULL, 0, NULL, 0);

		SC0710_AccessSpiRegisterS(pDevice, 0x000000D8 , tx_buf, 3, NULL, 0);
		{
			ULONG status = 0x00000000;

			do {

				SC0710_AccessSpiRegisterS(pDevice, 0x00000005 , NULL, 0, &status, 1);

			} while (status & 0x00000001 );
		}
	}
	{
		int wridx = ((nBufferSize - 1) / SC0710_TX_FIFO_MAX) * SC0710_TX_FIFO_MAX;

		for (; wridx >= 0; wridx -= SC0710_TX_FIFO_MAX) {

			LINUXV4L2_PRINT(KERN_INFO, "FIRMWARE.UPDATE: %d / %d BYTES", wridx, nBufferSize);

			if((ULONG)(wridx + SC0710_TX_FIFO_MAX) > nBufferSize) {

				lengths = nBufferSize - wridx;
			}
			else {

				lengths = SC0710_TX_FIFO_MAX;
			}



			{
				ULONG j = 0;

				ULONG  address = n_fpga_update_start_addr + wridx;



				BYTE* po = (BYTE*)(&address);

				BYTE* pe = (BYTE*)(pBuffer + wridx);

				tx_buf[0] = po[2];

				tx_buf[1] = po[1];

				tx_buf[2] = po[0];

				for (j = 0; j < lengths; j++) {

					tx_buf[j + 3] = pe[j];
				}
				SC0710_AccessSpiRegisterS(pDevice, 0x00000006 , NULL, 0, NULL, 0);

				SC0710_AccessSpiRegisterS(pDevice, 0x00000002 , tx_buf, lengths + 3, NULL, 0);
				{
					ULONG status = 0x00000000;

					do {

						SC0710_AccessSpiRegisterS(pDevice, 0x00000005 , NULL, 0, &status, 1);

					} while (status & 0x00000001 );
				}
			}
		}
	}
	return TRUE;
}

ULONG  rx_buf[SC0710_RX_FIFO_MAX];

BOOLEAN SC0710_GetP2140FlashData(CDevice* pDevice, ULONG nAddr, BYTE* pBuffer, ULONG nBufferSize)
{
	ULONG lengths = 0;

	ULONG i = 0;

	for (i = 0; i < nBufferSize; i += lengths) {

		if((i + SC0710_RX_FIFO_MAX) > nBufferSize) {

			lengths = nBufferSize - i;
		}
		else {

			lengths = SC0710_RX_FIFO_MAX;
		}



		{
			ULONG j = 0;

			ULONG  address = nAddr + i;

			ULONG  tx_buf[3];



			BYTE* po = (BYTE*)(&address);

			BYTE* pe = (BYTE*)(pBuffer + i);

			tx_buf[0] = po[2];

			tx_buf[1] = po[1];

			tx_buf[2] = po[0];

			SC0710_AccessSpiRegisterS(pDevice, 0x00000003 , tx_buf, 3, rx_buf, lengths);

			for (j = 0; j < lengths; j++) {

				pe[j] = (BYTE)(rx_buf[j]);
			}
		}
	}
	return TRUE;
}

BOOLEAN SC0710_StartP2140FlashAccess(CDevice* pDevice)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	{

		SC0710_SetRegister(pDevice, 0x00001040, 0x0000000A);



		p_sys_cfg->n_spi_ctrl_reg = SC0710_SPI_CTRL_DEFAULT;

		p_sys_cfg->n_spi_ctrl_reg |= SC0710_SPI_CTRL_SET_TO_MASTER;

		p_sys_cfg->n_spi_ctrl_reg |= SC0710_SPI_CTRL_ENABLE_SPI;



		SC0710_SetRegister(pDevice, 0x00001060, p_sys_cfg->n_spi_ctrl_reg);
		{
			ULONG status = 0x00000000;

			SC0710_AccessSpiRegisterS(pDevice, 0x00000005 , NULL, 0, &status, 1);
		}
	}
	return TRUE;
}

BOOLEAN SC0710_StopP2140FlashAccess(CDevice* pDevice)
{
	return TRUE;
}

static BYTE C5_PREAMBLE[5] = { 0xFF, 0xFF, 0xFF, 0xBD, 0xB3 };

static const int SC0710_C5_FLASH_START_ADDR = 0x00000000;

static const int SC0710_C5_FLASH_STOP_ADDR = 0x000E0000;

static const int SC0710_C7_FLASH_STOP_ADDR = 0x01000000;

BOOLEAN SC0710_SetC5FlashData(CDevice* pDevice, BYTE* pBuffer, ULONG nBufferSize)
{
	ULONG  lengths = 0;



	ULONG i = 0;

	ULONG n_find_index = 0;

	ULONG n_cx_flash_stop_addr = 0;

	tx_buf[0] = 0;

	tx_buf[1] = 0;

	tx_buf[2] = 0;

	SC0710_AccessSpiRegisterS_PORT2(pDevice, 0x00000074 , tx_buf, 3, NULL, 0);

	for (n_find_index = 0; n_find_index < nBufferSize; n_find_index++) {

		if(memcmp(pBuffer + n_find_index, C5_PREAMBLE, 5) == 0) {



			break;
		}
	}
	if(n_find_index == nBufferSize) {



		return FALSE;
	}


	if(((pDevice->bcdDevice == 0x0204) && (pDevice->iManufacturer == 0x29)) ||

		((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x29)) ||

		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2D)) ||

		((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||

		((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

		n_cx_flash_stop_addr = SC0710_C7_FLASH_STOP_ADDR;
	}
	else {

		n_cx_flash_stop_addr = SC0710_C5_FLASH_STOP_ADDR;
	}

	for (i = 0; i < nBufferSize; i += SC0710_FLASH_BLOCK_SIZE) {

		LINUXV4L2_PRINT(KERN_INFO, "FIRMWARE.ERASED: %d / %d BYTES", i, nBufferSize);



		if(i > n_cx_flash_stop_addr) {



			return FALSE;
		}
		else {


		}
		{
			ULONG status = 0x00000000;

			int error_count = 0;



			BYTE* po = (BYTE*)(&i);

			tx_buf[0] = po[2];

			tx_buf[1] = po[1];

			tx_buf[2] = po[0];

			SC0710_AccessSpiRegisterS_PORT2(pDevice, 0x00000006 , NULL, 0, NULL, 0);

			SC0710_AccessSpiRegisterS_PORT2(pDevice, 0x000000D8 , tx_buf, 3, NULL, 0);

			do {

				SC0710_AccessSpiRegisterS_PORT2(pDevice, 0x00000005 , NULL, 0, &status, 1);

				error_count++;

				if(error_count > 0 && (error_count % 1000) == 0) {

					LINUXV4L2_PRINT(KERN_INFO, "STILL IN ERASE LOOP(%d)", i);
				}

			} while (status & 0x00000001 );
		}
	}
	{
		LONG wridx = ((nBufferSize - 1) / SC0710_TX_FIFO_MAX) * SC0710_TX_FIFO_MAX;

		for (; wridx >= 0; wridx -= SC0710_TX_FIFO_MAX) {

			LINUXV4L2_PRINT(KERN_INFO, "FIRMWARE.UPDATE: %d / %d BYTES", wridx, nBufferSize);

			if((ULONG)(wridx + SC0710_TX_FIFO_MAX) > nBufferSize) {

				lengths = nBufferSize - wridx;
			}
			else {

				lengths = SC0710_TX_FIFO_MAX;
			}



			{	ULONG  address = wridx;

			ULONG j = 0;

			if(address > n_cx_flash_stop_addr) {



				return FALSE;
			}
			else {


			}
			{


				BYTE* po = (BYTE*)(&address);

				BYTE* pe = (BYTE*)(pBuffer + wridx);

				tx_buf[0] = po[2];

				tx_buf[1] = po[1];

				tx_buf[2] = po[0];

				for (j = 0; j < lengths; j++) {

					tx_buf[j + 3] = pe[j];
				}
				SC0710_AccessSpiRegisterS_PORT2(pDevice, 0x00000006 , NULL, 0, NULL, 0);

				SC0710_AccessSpiRegisterS_PORT2(pDevice, 0x00000002 , tx_buf, lengths + 3, NULL, 0);
				{
					ULONG status = 0x00000000;

					int error_count = 0;

					do {

						SC0710_AccessSpiRegisterS_PORT2(pDevice, 0x00000005 , NULL, 0, &status, 1);

						error_count++;

						if(error_count > 0 && (error_count % 1000) == 0) {

							LINUXV4L2_PRINT(KERN_INFO, "STILL IN UPDATE LOOP (%d)", i);
						}

					} while (status & 0x00000001 );
				}
			}
			}
		}
	}
	return TRUE;
}

BOOLEAN SC0710_StartC5FlashAccess(CDevice* pDevice)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	{

		SC0710_SetRegister(pDevice, 0x00002040, 0x0000000A);



		p_sys_cfg->n_spi_ctrl_reg = SC0710_SPI_CTRL_DEFAULT;

		p_sys_cfg->n_spi_ctrl_reg |= SC0710_SPI_CTRL_SET_TO_MASTER;

		p_sys_cfg->n_spi_ctrl_reg |= SC0710_SPI_CTRL_ENABLE_SPI;



		SC0710_SetRegister(pDevice, 0x00002060, p_sys_cfg->n_spi_ctrl_reg);
		{
			ULONG status = 0x00000000;

			SC0710_AccessSpiRegisterS_PORT2(pDevice, 0x00000005 , NULL, 0, &status, 1);
		}
	}
	return TRUE;
}

VOID SC0710_StopC5FlashAccess(CDevice* pDevice)
{
	SC0710_SetRegister(pDevice, 0x00002040, 0x0000000A);
}

BOOL SC0710_IS_BAD_SIGNAL(CDevice* pDevice)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	BOOL is_bad_signal = FALSE;

	if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) { return FALSE; }

	if( pDevice->m_nAnalogVideoDecoderStatusProperty == 0 ) { return FALSE; }

	if(((pDevice->idVendor == 0x12AB) && (pDevice->idProduct & 0xFFFF) == 0x0750)) { return FALSE; }

	if(p_sys_cfg->n_firmware_version >= 0x20190614) {

		ULONG R000000AC = SC0710_GetRegister(pDevice, 0x000000AC);

		if(p_sys_cfg->n_output_video_color_bits == 1 ||

			p_sys_cfg->n_output_video_color_bits == 2) {

			;
		}
		else {

			if( (pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x000E) ||
				(pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x000F) ||

				((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x11)) ||
				((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x17)) ||
				((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x17)) ||
				((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x17)) ||
				((pDevice->bcdDevice == 0x0301) && (pDevice->iManufacturer == 0x17)) ||
				((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x17)) ||
				((pDevice->bcdDevice == 0xB001) && (pDevice->iManufacturer == 0x17)) ) {

				is_bad_signal = (R000000AC & 0x00000002) ? TRUE : FALSE;
			}
			else {

				if((p_sys_cfg->n_output_video_resolution_cy % 2) ||

					(p_sys_cfg->n_output_video_resolution_fps >= 144)) {

					is_bad_signal = (R000000AC & 0x00000002) ? TRUE : FALSE;
				}
				else {

					is_bad_signal = (R000000AC & 0x00000001) ? TRUE : FALSE;
				}
			}
		}
		if(is_bad_signal)
		{



			if(p_sys_cfg->n_input_video_resolution_interleaved)
			{
				CVideo* pVideo = pDevice->m_pVideo[0];

				if(pVideo)
				{
					if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420 || pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YVU420 || pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12 || pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_M420)
					{
						return FALSE;
					}
				}
			}

			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] R000000AC(0x%x)\n", pDevice->m_nKsDeviceNumber, R000000AC);
		}

		if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x13)) ||

			((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x13))) {

			if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4)
			{
				if(p_sys_cfg->n_audio_running_state)
				{
					ULONG audio_sample = (R000000AC >> 16) & 0xFFFF;

					if(audio_sample < 700)
					{
						p_sys_cfg->n_input_video_no_audio_packet = 1;
					}
				}
			}

		}
	}


	return is_bad_signal;
}

BOOL SC0710_IS_BAD_SIGNAL_MULTICH(CDevice* pDevice, ULONG ch)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	BOOL is_bad_signal = FALSE;

	ULONG R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

	R000000EC &= ~3;

	if( ((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0xA201) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
		((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F)) ) {

		if( pDevice->m_nAnalogCrossbarVideoInputProperty == 4 ) {

			R000000EC |= 1;
		}
		else {

			R000000EC |= 0;
		}
	}
	else {

		R000000EC |= ch;
	}

	SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);

	if(p_sys_cfg->n_firmware_version >= 0x20190614) {

		BOOL is_sd_fmt = FALSE;

		ULONG R000000AC = SC0710_GetRegister(pDevice, 0x000000AC);

		if( (p_sys_cfg->n_input_video_resolution_cxs[ ch ] == 720 && p_sys_cfg->n_input_video_resolution_cys[ ch ] == 240) ) {

			is_sd_fmt = TRUE;
		}
		if( p_sys_cfg->n_output_video_color_bits == 1 ||

			p_sys_cfg->n_output_video_color_bits == 2 ||

			is_sd_fmt ) {

			;
		}
		else {

			if( (p_sys_cfg->n_output_video_resolution_cy % 2) ||

				(p_sys_cfg->n_output_video_resolution_fps >= 144) ) {

				is_bad_signal = (R000000AC & 0x00000002) ? TRUE : FALSE;
			}
			else {

				is_bad_signal = (R000000AC & 0x00000001) ? TRUE : FALSE;
			}
		}

		if(is_bad_signal)
		{
			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] CH(%d) R000000AC(0x%x)\n", pDevice->m_nKsDeviceNumber, ch, R000000AC);
		}
	}
	return is_bad_signal;
}



void SC0710_ADJUST_CSC_TABLE(CDevice* pDevice, ULONG nBrightness, ULONG nContrast, ULONG nSaturation, ULONG nHue, int n_in_range, int n_in_color, int n_in_metric, int n_out_color, int n_out_range, int n_out_metric);

NTSTATUS SC0710_SetAnalogVideoProcAmpProperties(CDevice* pDevice, ULONG ch, BOOL THREAD)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	if(THREAD == FALSE) { SC0710_CLOSE_THREAD(); }

	if(p_sys_cfg->b_is_mcu_support) {

		HDMI20_INTERFACE s_rx_hdmi20_buf; memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));
		{
			unsigned char* rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.resolution[0].vid.scale);

			if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

				rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.resolution[ch].vid.scale);
			}
			else {

				rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.resolution[0].vid.scale);
			}
			{
				long long        offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf);

				unsigned char    txbuf[7] = { (int)(offset), (BYTE)(pDevice->m_nCustomAnalogVideoColorRangeProperty & 0x03),

															   (BYTE)(g_n_analog_decoder_brightness[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

															   (BYTE)(g_n_analog_decoder_contrast[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

															   (BYTE)(g_n_analog_decoder_saturation[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

															   (BYTE)(g_n_analog_decoder_hue[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

															   (BYTE)(g_n_analog_decoder_sharpness[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF) };

				LINUXV4L2_DEBUG(KERN_INFO, "[%02d] CH(%d) offset(0d%lld) ColorRange(0x%x) bright(0x%x) contrast(0x%x) saturation(0x%x) hue(0x%x) sharp(0x%x)\n", pDevice->m_nKsDeviceNumber, ch, offset,

					(BYTE)(pDevice->m_nCustomAnalogVideoColorRangeProperty & 0x03),

					(BYTE)(g_n_analog_decoder_brightness[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

					(BYTE)(g_n_analog_decoder_contrast[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

					(BYTE)(g_n_analog_decoder_saturation[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

					(BYTE)(g_n_analog_decoder_hue[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

					(BYTE)(g_n_analog_decoder_sharpness[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF));


				if(((pDevice->bcdDevice == 0xD004) && (pDevice->iManufacturer == 0x1C)) ||
					((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1C)) ||
					((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D))) {

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 7, NULL, 0);
				}
				else if(((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||
					((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D)) ||
					((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
					((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

					unsigned char txbuf[8] = { (int)(offset >> 8), (int)(offset >> 0),

												(BYTE)(pDevice->m_nCustomAnalogVideoColorRangeProperty & 0x03),

												(BYTE)(g_n_analog_decoder_brightness[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

												(BYTE)(g_n_analog_decoder_contrast[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

												(BYTE)(g_n_analog_decoder_saturation[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

												(BYTE)(g_n_analog_decoder_hue[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

												(BYTE)(g_n_analog_decoder_sharpness[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF) };

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 8, NULL, 0);
				}
				else if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x12))) {

					unsigned char txbuf[8] = { (int)(offset), (BYTE)(pDevice->m_nCustomAnalogVideoColorRangeProperty & 0x03),

												(BYTE)(g_n_analog_decoder_brightness[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

												(BYTE)(g_n_analog_decoder_contrast[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

												(BYTE)(g_n_analog_decoder_saturation[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

												(BYTE)(g_n_analog_decoder_hue[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

												(BYTE)(g_n_analog_decoder_sharpness[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

												(BYTE)(g_n_analog_decoder_white_balance[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF) };

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 8, NULL, 0);
				}
				else if (false
					|| ((pDevice->bcdDevice == 0x0301) && (pDevice->iManufacturer == 0x17))
					|| ((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x17))
					|| ((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x1D))
					|| ((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D))
					) {
					MCU_INTERFACE_MULTICH_LC300 s_rx_lc300_buf;

					unsigned char* rxbuf_lc300 = (BYTE*)(&s_rx_lc300_buf.function_name.resolution[ch].Vid.scale);

					long long        offset_lc300 = rxbuf_lc300 - (unsigned char*)(&s_rx_lc300_buf);

					memset(&s_rx_lc300_buf, 0x00, sizeof(s_rx_lc300_buf));

					if (((pDevice->bcdDevice == 0x0301) && (pDevice->iManufacturer == 0x17)) ||
						((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x17))) {

						rxbuf_lc300 = (BYTE*)(&s_rx_lc300_buf.function_name.resolution[4].Vid.scale);

						offset_lc300 = rxbuf_lc300 - (unsigned char*)(&s_rx_lc300_buf);
					}
					{
						unsigned char    txbuf_lc300[7] = { (int)(offset_lc300), (BYTE)(pDevice->m_nCustomAnalogVideoColorRangeProperty & 0x03),

																		(BYTE)(g_n_analog_decoder_brightness[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

																		(BYTE)(g_n_analog_decoder_contrast[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

																		(BYTE)(g_n_analog_decoder_saturation[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

																		(BYTE)(g_n_analog_decoder_hue[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF),

																		(BYTE)(g_n_analog_decoder_sharpness[pDevice->m_nKsDeviceNumber * 1][ch] & 0xFF) };

						LINUXV4L2_DEBUG(KERN_INFO, "ch %d, offset 0x%x, size %d \n", ch, offset_lc300, sizeof(s_rx_lc300_buf));

						SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf_lc300, 7, NULL, 0);
					}
				}
				else {

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 6, NULL, 0);
				}
			}
		}
	}
	if(THREAD == FALSE) { SC0710_START_THREAD(); }

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_SetAnalogVideoDecoderStandardProperty(CDevice* pDevice, ULONG nStandard, BOOL THREAD)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	if(THREAD == FALSE) { SC0710_CLOSE_THREAD(); }

	if(p_sys_cfg->b_is_tw9910_support) {

		if(pDevice->m_nAnalogCrossbarVideoInputProperty == 5 ||

			pDevice->m_nAnalogCrossbarVideoInputProperty == 6) {

			;
		}
	}
	if(THREAD == FALSE) { SC0710_START_THREAD(); }

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_SetAnalogCrossbarRouteProperty(CDevice* pDevice, ULONG ch, ULONG nVideoInput, ULONG nVideoOutput, ULONG nAudioInput, ULONG nAudioOutput, BOOL THREAD)
{
	SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_SetAnalogCrossbarRouteProperty() ch(%d) nVideoInput(%d) m_nCustomAnalogVideoMultiSyncPixelFormat(%d) nAudioInput(%d)\n", pDevice->m_nKsDeviceNumber, ch, nVideoInput, pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat, nAudioInput);

	if(THREAD == FALSE) { SC0710_CLOSE_THREAD(); }

	if(nVideoInput != -1) {

		if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x10)) ||

			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x11))) {

			unsigned char txbuf[2] = { 0x00, 0x00 };

			if(pDevice->m_nAnalogCrossbarVideoInputProperty == 0) {

				txbuf[1] = 0;
			}
			else if(pDevice->m_nAnalogCrossbarVideoInputProperty == 1) {

				txbuf[1] = 3;
			}
			else {

				if(pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat == 0) {



					txbuf[1] = 1;
				}
				else {

					txbuf[1] = 2;
				}
			}
			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);
		}
		if( ((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||

			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||

			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x1101) && (pDevice->iManufacturer == 0x1F)) ||


			((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0501) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0601) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F)) ) {

			if(ch == 0)
			{

				MCU_INTERFACE_MULTICH_AIO s_rx_hdmi20_buf;

				unsigned char* rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.VideoInput[ch]);

				long long       offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf);

				unsigned char   txbuf[4] = { (int)(offset), (BYTE)(nVideoInput & 0x0F), 0x00, 0x00 };

				memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);
			}
		}






















		if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

			ULONG i = 0;

			ULONG counts = pDevice->bcdDevice & 0x00FF;

			if( ((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||
				((pDevice->bcdDevice == 0xB401) && (pDevice->iManufacturer == 0x16)) ||
				((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||
				((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ||
				((pDevice->bcdDevice == 0xB201) && (pDevice->iManufacturer == 0x18)) ||
				((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18)) ) {

				counts = 4;
			}
			for( i = 0 ; i < counts ; i++ ) {

				SC0710_SetAnalogVideoProcAmpProperties( pDevice, i, TRUE );
			}
		}
		else {

			if((pDevice->m_nAnalogCrossbarVideoInputProperty == 0) || (pDevice->m_nAnalogCrossbarVideoInputProperty == 1))
			{
				SC0710_EnableHotPlug(pDevice, FALSE, 0);



				DELAY_100NS(2000000);

				SC0710_EnableHotPlug(pDevice, TRUE, 0);
			}
			SC0710_SetAnalogVideoProcAmpProperties(pDevice, 0, TRUE);
		}
		if( TRUE ) {





			p_sys_cfg->n_input_video_resolution_cxs[ ch ] = 0;

			p_sys_cfg->n_input_video_resolution_cys[ ch ] = 0;

			p_sys_cfg->n_input_video_resolution_fpss[ ch ] = 0;

			p_sys_cfg->n_input_video_resolution_fps_ms[ ch ] = 0;

			p_sys_cfg->n_input_video_resolution_levelbs[ ch ] = 0;

			p_sys_cfg->n_input_video_resolution_psfs[ ch ] = 0;

			p_sys_cfg->n_input_video_resolution_color_colorimetrys[ ch ] = 0;

			p_sys_cfg->n_input_audio_sampling_frequencys[ ch ] = 0;
		}
	}
	if(nAudioInput != -1) {

		BYTE I2S_SEL = 0;

		HDMI20_INTERFACE s_rx_hdmi20_buf;

		memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));

		if(pDevice->m_nAnalogCrossbarVideoInputProperty == 0 ||

			pDevice->m_nAnalogCrossbarVideoInputProperty == 1 ||

			pDevice->m_nAnalogCrossbarVideoInputProperty == 4) {

			if(pDevice->m_nAnalogCrossbarAudioInputProperty == 0) {

				I2S_SEL = 0;
			}
			else {

				if(pDevice->m_nCustomAnalogAudioPinTopologyProperty == 0) {

					I2S_SEL = 1;
				}
				else {

					I2S_SEL = 2;
				}
			}
		}
		if(pDevice->m_nAnalogCrossbarVideoInputProperty == 2 ||

			pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ||

			pDevice->m_nAnalogCrossbarVideoInputProperty == 5 ||

			pDevice->m_nAnalogCrossbarVideoInputProperty == 6) {

			if(pDevice->m_nCustomAnalogAudioPinTopologyProperty == 0) {

				I2S_SEL = 1;
			}
			else {

				I2S_SEL = 2;
			}
		}
		if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

			(pDevice->iManufacturer == 0x1A) ||

			(pDevice->iManufacturer == 0x1B) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x29) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ||

			(pDevice->iManufacturer == 0x2D)) {

			if(pDevice->m_nAnalogCrossbarAudioInputProperty == 0) {

				I2S_SEL = 0;
			}
			else {

				if(pDevice->m_nCustomAnalogAudioPinTopologyProperty == 0) {

					I2S_SEL = 1;
				}
				else {

					I2S_SEL = 2;
				}
			}
		}
		if(((pDevice->iProduct == 0x00) && (pDevice->iManufacturer == 0x15) && (pDevice->bcdDevice == 0x000B)) ||

			((pDevice->bcdDevice == 0xB001) && (pDevice->iManufacturer == 0x17)) ||

			((pDevice->bcdDevice == 0xB201) && (pDevice->iManufacturer == 0x18))) {

			unsigned char* rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.resolution[0].ptz_other.OTHER.audioctl.AudioLineInSel);

			long long       offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf);

			unsigned char   txbuf[2] = { (BYTE)(offset), 0 };

			memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));

			if(I2S_SEL == 0) {

				txbuf[1] = 0;
			}
			else if(I2S_SEL == 1) {

				txbuf[1] = 1;
			}
			else if(I2S_SEL == 2) {

				txbuf[1] = 2;
			}
			else if(I2S_SEL == 3) {

				txbuf[1] = 3;
			}
			else {

				txbuf[1] = 4;
			}
			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);
		}
		if( ((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1B)) ||
			((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1B)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1D)) ||
			((pDevice->bcdDevice == 0x0A04) && (pDevice->iManufacturer == 0x1D)) ||
			((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ) {

			ULONG counts = pDevice->bcdDevice & 0x00FF;

			ULONG ch = 0;

			for( ch = 0 ; ch < counts ; ch++ ) {

				if( pDevice->m_nAnalogCrossbarAudioInputPropertys[ ch ] == 0 ) {

					I2S_SEL = 0;
				}
				else {

					if( pDevice->m_nCustomAnalogAudioPinTopologyPropertys[ ch ] == 0 ) {

						I2S_SEL = 1;
					}
					else if( pDevice->m_nCustomAnalogAudioPinTopologyPropertys[ ch ] == 1 ) {

						I2S_SEL = 2;
					}
					else if( pDevice->m_nCustomAnalogAudioPinTopologyPropertys[ ch ] == 2 ) {

						I2S_SEL = 3;
					}
					else {

						I2S_SEL = 4;
					}
				}
				if( (pDevice->iManufacturer == 0x1D) ) {

					if( I2S_SEL == 2 ) { I2S_SEL = 1; }

					if( I2S_SEL == 4 ) { I2S_SEL = 3; }
				}
				if( ((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ) {

					ULONG R000000B4 = 0x00000000;

					R000000B4 = SC0710_GetRegister( pDevice, 0x000000B4 );










					if( pDevice->m_nAnalogCrossbarAudioInputProperty == 0 ) {

						R000000B4 &= ~(0x0F);
					}
					else {

						R000000B4 |=  (0x0F);
					}
					SC0710_SetRegister( pDevice, 0x000000B4, R000000B4 );
				}
				else {

					unsigned char *  rxbuf = (BYTE *)(&s_rx_hdmi20_buf.function_name.resolution[ ch ].ptz_other.OTHER.audioctl.AudioLineInSel );

					long long        offset = rxbuf - (unsigned char *)(&s_rx_hdmi20_buf);

					unsigned char    txbuf[ 2 ] = { (BYTE)(offset), 0 };

					memset( &s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf) );

					if( I2S_SEL == 0 ) {

						txbuf[ 1 ] = 0;
					}
					else if( I2S_SEL == 1 ) {

						txbuf[ 1 ] = 1;
					}
					else if( I2S_SEL == 2 ) {

						txbuf[ 1 ] = 2;
					}
					else if( I2S_SEL == 3 ) {

						txbuf[ 1 ] = 3;
					}
					else if( I2S_SEL == 4 ) {

						txbuf[ 1 ] = 4;
					}
					else {

						txbuf[ 1 ] = 0;
					}
					SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0 );
				}
			}
		}

		if(((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||
			((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D))) {

			ULONG counts = pDevice->bcdDevice & 0x00FF;
			ULONG ch = 0;
			for (ch = 0; ch < counts; ch++) {

				unsigned char* rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.resolution[ch].ptz_other.OTHER.audioctl.AudioLineInSel);

				long long        offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf);

				unsigned char    txbuf[3] = { (BYTE)(offset >> 8), (BYTE)(offset >> 0), 0 };

				memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));

				if(pDevice->m_nAnalogCrossbarAudioInputPropertys[ch] == 0) {

					I2S_SEL = 0;
				}
				else {

					if(pDevice->m_nCustomAnalogAudioPinTopologyPropertys[ch] == 0) {

						I2S_SEL = 1;
					}
					else if(pDevice->m_nCustomAnalogAudioPinTopologyPropertys[ch] == 1) {

						I2S_SEL = 2;
					}
					else if(pDevice->m_nCustomAnalogAudioPinTopologyPropertys[ch] == 2) {

						I2S_SEL = 3;
					}
					else {

						I2S_SEL = 4;
					}
				}
				txbuf[2] = I2S_SEL;

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 3, NULL, 0);
			}
		}
		if( ((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D))  ||
			((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D))  ||
			((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D)) ) {

			ULONG counts = pDevice->bcdDevice & 0x00FF;

			ULONG ch = 0;

			for( ch = 0 ; ch < counts ; ch++ ) {

				unsigned char *  rxbuf = (BYTE *)(&s_rx_hdmi20_buf.function_name.resolution[ ch ].ptz_other.OTHER.audioctl.AudioLineInSel );

				long long        offset = rxbuf - (unsigned char *)(&s_rx_hdmi20_buf);

				unsigned char    txbuf[ 3 ] = { (BYTE)(offset >> 8), (BYTE)(offset >> 0), 0 };

				memset( &s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf) );

				txbuf[ 2 ] = 0;

				SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 3, NULL, 0 );
			}
		}
		if( ((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0xA201) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x1101) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x1201) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x1301) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0501) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0601) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F)) ) {

			MCU_INTERFACE_MULTICH_AIO s_rx_hdmi20_buf;

			unsigned char * rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.resolution[ ch ].Aud.audiolinein);

			long long       offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf);

			unsigned char   txbuf[ 4 ] = { (int)(offset), (BYTE)(nAudioInput & 0x0F), 0x00, 0x00 };

			memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf) );

			if( pDevice->m_nAnalogCrossbarVideoInputProperty == 2 ||

				pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ||

				pDevice->m_nAnalogCrossbarVideoInputProperty == 5 ||

				pDevice->m_nAnalogCrossbarVideoInputProperty == 6 ) {

				if(pDevice->m_nCustomAnalogAudioPinTopologyProperty == PHONE_JACK)
				{
					txbuf[ 1 ] = 1;
				}
				else if(pDevice->m_nCustomAnalogAudioPinTopologyProperty == PCM_PHONE_JACK)
				{
					txbuf[ 1 ] = 2;
				}
				else if(pDevice->m_nCustomAnalogAudioPinTopologyProperty == PIN)
				{
					txbuf[ 1 ] = 3;
				}
				else if(pDevice->m_nCustomAnalogAudioPinTopologyProperty == PCM_PIN)
				{
					txbuf[ 1 ] = 4;
				}
				else
				{
				}

			}
			SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0 );

			if( ((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F)) ) {

				;
			}
			else {

				ULONG R000000B4 = SC0710_GetRegister( pDevice, 0x000000B4 );

				if( pDevice->m_nAnalogCrossbarVideoInputProperty == 4 ) {

					if( pDevice->m_nAnalogCrossbarAudioInputProperty == 0 ) {

						R000000B4 &= ~0x02;
					}
					else {

						R000000B4 |=  0x02;
					}
				}
				else if( pDevice->m_nAnalogCrossbarVideoInputProperty == 2 ||

						 pDevice->m_nAnalogCrossbarVideoInputProperty == 3 ||

						 pDevice->m_nAnalogCrossbarVideoInputProperty == 5 ||

						 pDevice->m_nAnalogCrossbarVideoInputProperty == 6 ) {

						R000000B4 |=  0x01;
				}
				else {

					if( pDevice->m_nAnalogCrossbarAudioInputProperty == 0 ) {

						R000000B4 &= ~0x01;
					}
					else {

						R000000B4 |=  0x01;
					}
				}
				SC0710_SetRegister( pDevice, 0x000000B4, R000000B4 );
			}
		}

	}
	if(THREAD == FALSE) { SC0710_START_THREAD(); }

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_SetAnalogAudioVoulmeProperty(CDevice* pDevice, ULONG nVolume)
{
	return STATUS_SUCCESS;
}

VOID SC0710_SET_PTZ_DATA( CDevice * pDevice, BYTE ch, BYTE psz_data[ 8 ] )
{
	HDMI20_INTERFACE s_rx_hdmi20_buf;



	unsigned char *  rxbuf = (BYTE *)(&s_rx_hdmi20_buf.function_name.resolution[ ch ].ptz_other.ptz);

	long long        offset = rxbuf - (unsigned char *)(&s_rx_hdmi20_buf);

	unsigned char    txbuf[ 9 ] = { (BYTE)(offset) + 0x01, 0, 0, 0, 0, 0, 0, 0 };

	memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));

	if( ((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
		((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
		((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D)) ) {

		BYTE enable = 0;

		txbuf[ 0 ] = (int)((offset + 0x01) >> 8);

		txbuf[ 1 ] = (int)((offset + 0x01) >> 0);

		RtlCopyMemory( txbuf + 2, psz_data, 7 );

		SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 9, NULL, 0 );

		DELAY_100NS( 50000 );

		txbuf[ 0 ] = (int)((offset) >> 8);

		txbuf[ 1 ] = (int)((offset) >> 0);


		SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, &enable, 1 );

		DELAY_100NS( 50000 );

		txbuf[ 2 ] = (enable | 0x01);

		SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 3, NULL, 0 );
	}
	else {

		BYTE enable = 0;

		txbuf[ 0 ] = (int)((offset + 0x01) >> 0);

		RtlCopyMemory( txbuf + 1, psz_data, 7 );

		SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 8, NULL, 0 );

		DELAY_100NS( 50000 );

		txbuf[ 0 ] = (BYTE)(offset);

		SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, &enable, 1 );

		DELAY_100NS( 50000 );

		txbuf[ 1 ] = (enable | 0x01);

		SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0 );
	}
}

NTSTATUS SC0710_EnableHDCP(CDevice* pDevice, ULONG nEnable, ULONG ch)
{
	HDMI20_INTERFACE s_rx_hdmi20_buf;
	{
		unsigned char* rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.resolution[ch].hdcponoff);

		long long        offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf);





		unsigned char    txbuf[4] = { (int)(offset), 0, 0, 0 };

		unsigned char    reads[2] = { 0, 0 };

		memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));

		if(((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||
			((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D)) ||
			((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
			((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
			((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

			txbuf[ 0 ] = (int)(offset >> 8);

			txbuf[ 1 ] = (int)(offset >> 0);

			SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, reads, 1 );
		}
		else {

			txbuf[ 0 ] = (int)(offset >> 0);

			SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, reads, 1 );
		}
		if(nEnable == 0) {

			txbuf[1] = (reads[0] | 0x01);
		}
		else {

			txbuf[1] = (reads[0] & 0xFE);
		}
		SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);

		LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_EnableHDCP() nEnable(0x%x) ch(0x%x)\n", pDevice->m_nKsDeviceNumber, nEnable, ch);

		SC0710_EnableHotPlug(pDevice, FALSE, ch);



		DELAY_100NS(2000000);

		SC0710_EnableHotPlug(pDevice, TRUE, ch);
	}

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_EnableHotPlug(CDevice* pDevice, ULONG nEnable, ULONG ch)
{
	HDMI20_INTERFACE s_rx_hdmi20_buf;
	{
		unsigned char* rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.resolution[ch].hdcponoff);

		long long        offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf);

		unsigned char    txbuf[4] = { (int)(offset), 0, 0, 0 };

		unsigned char    reads[2] = { 0, 0 };

		SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

		memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));

		if(p_sys_cfg->n_mcu_version >= 200319) {


			if(((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ) {

				return STATUS_SUCCESS;
			}

			if(((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||
				((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D)) ||
				((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
				((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
				((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

				txbuf[ 0 ] = (int)(offset >> 8);

				txbuf[ 1 ] = (int)(offset >> 0);

				SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, reads, 1 );

				if( nEnable == 1 ) {

					txbuf[ 2 ] = (reads[ 0 ] | 0x04);
				}
				else {

					txbuf[ 2 ] = (reads[ 0 ] & 0xFB);
				}
				SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 3, NULL, 0 );
			}
			else {

				txbuf[ 0 ] = (int)(offset >> 0);

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, reads, 1);

				if(nEnable == 1) {

					txbuf[1] = (reads[0] | 0x04);
				}
				else {

					txbuf[1] = (reads[0] & 0xFB);
				}
				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);
			}
			LINUXV4L2_PRINT( KERN_INFO,"[%02d] SC0710_ENABLE_HOTPLUG( %d.%d )", pDevice->m_nKsDeviceNumber, ch, nEnable );
		}
	}
	return STATUS_SUCCESS;
}

NTSTATUS SC0710_MuteLoopThrough(CDevice* pDevice, ULONG mute)
{
	HDMI20_INTERFACE s_rx_hdmi20_buf;

	unsigned char* rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.resolution[0].hdcponoff);

	long long        offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf);

	unsigned char    txbuf[4] = { (int)(offset), 0, 0, 0 };

	unsigned char    reads[2] = { 0, 0 };

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));

	if(p_sys_cfg->n_mcu_version >= 210809) {

		if(((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||
			((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D)) ||
			((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
			((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
			((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

			txbuf[ 0 ] = (int)(offset >> 8);

			txbuf[ 1 ] = (int)(offset >> 0);

			SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, reads, 1 );

			if( mute == 1 ) {

				txbuf[ 2 ] = (reads[ 0 ] | 0x08);
			}
			else {

				txbuf[ 2 ] = (reads[ 0 ] & 0xF7);
			}
			SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 3, NULL, 0 );
		}
		else {

			txbuf[ 0 ] = (int)(offset >> 0);

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, reads, 1);

			if(mute == 1) {

				txbuf[1] = (reads[0] | 0x08);
			}
			else {

				txbuf[1] = (reads[0] & 0xF7);
			}
			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);
		}
		LINUXV4L2_PRINT(KERN_INFO, "SC0710_MuteLoopThrough( %d.%d )", pDevice->m_nKsDeviceNumber, mute);
	}
	return STATUS_SUCCESS;
}

NTSTATUS SC0710_EableEDIDAccess(CDevice* pDevice, ULONG ch)
{
	HDMI20_INTERFACE s_rx_hdmi20_buf;

	unsigned char* rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.selport);

	long long        offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf);

	unsigned char    txbuf[4] = { (int)(offset), 0, 0, 0 };

	memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));

	txbuf[1] = (unsigned char)(ch);

	SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);

	return STATUS_SUCCESS;
}


NTSTATUS SC0710_SetDeviceLEDState(CDevice* pDevice, ULONG nState)
{
	HDMI20_INTERFACE s_rx_hdmi20_buf;

	unsigned char* rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.resolution[0].hdcponoff);

	long long        offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf);

	unsigned char    txbuf[4] = { (int)(offset), 0, 0, 0 };

	unsigned char    read_back[2] = { 0, 0 };

	memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));

	SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, read_back, 1);

	if(nState >= 0x00000001) {

		txbuf[1] = (read_back[0] | 0x02);
	}
	else {

		txbuf[1] = (read_back[0] & ~0x02);
	}
	SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_EnableHDR2SDR(CDevice* pDevice, ULONG nEnable)
{
	HDMI20_INTERFACE s_rx_hdmi20_buf;
	{
		unsigned char* rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.resolution[0].hdr2sdr);

		long long        offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf);

		unsigned char    txbuf[4] = { (BYTE)(offset), (BYTE)(nEnable) ? 1 : 0, 0, 0 };

		memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));

		SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);

		LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_EnableHDR2SDR( %d )\n", pDevice->m_nKsDeviceNumber, nEnable);
	}
	return STATUS_SUCCESS;
}

NTSTATUS SC0710_EnableNative444(CDevice* pDevice, ULONG nMode)
{
	HDMI20_INTERFACE s_rx_hdmi20_buf;

	unsigned char* rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.data_port_num);

	long long        offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf);

	unsigned char    txbuf[4] = { (int)(offset), 0x00, 0x00, 0x00 };

	memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));

	SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf, 1);

	rxbuf[0] &= 0x00000006;

	if(nMode == 0x00000000) {

		txbuf[1] = 0x00;
	}
	if(nMode == 0x00000001) {

		txbuf[1] = 0x04;
	}
	if(nMode >= 0x00000002) {

		txbuf[1] = 0x06;
	}
	if(rxbuf[0] != txbuf[1]) {

		SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);

		DELAY_100NS(1280000);
	}
	LINUXV4L2_PRINT(KERN_INFO, "SC0710_EnableNative444( %d )\n", nMode);

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_EnableScaleDown(CDevice* pDevice)
{
	HDMI20_INTERFACE s_rx_hdmi20_buf;



	unsigned char* rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.resolution[0].ptz_other.OTHER.padding[0]);

	long long        offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf);

	memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));

	{
		unsigned char    txbuf[8] = { (BYTE)(offset), 0x01, 0, 0, 0, 0, 0, 0 };

		SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);
	}
	LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_EnableScaleDown()\n", pDevice->m_nKsDeviceNumber);

	return STATUS_SUCCESS;
}

BOOLEAN SC0710_DownloadP2140Firmware(CDevice* pDevice, BYTE* pwszFileName)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	BYTE* pFileBuffer = NULL;

	BYTE* pWorkBuffer = NULL;

	const struct firmware* fw = NULL;

	int ret = 1;

	ULONG             nFileSize = 0;

	BOOLEAN			  returns = FALSE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	u64 ts = 0;
#else
	struct timeval ts;
#endif

	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	if(p_sys_cfg->n_mcu_version == 0) { return FALSE; }

	if(pDevice->m_pKsVideo[0])
	{
		ret = request_firmware(&fw, pwszFileName, &(pDevice->m_pKsVideo[0]->dev));

		LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadP2140Firmware() fw(0p%p)\n", pDevice->m_nKsDeviceNumber, fw);
	}

	if(ret) {

		LINUXV4L2_PRINT(KERN_ERR, "[%02d] SC0710_DownloadP2140Firmware() Upload failed. (file %s not found?)\n", pDevice->m_nKsDeviceNumber, pwszFileName);

		return FALSE;

	}
	else {

		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_DownloadP2140Firmware() %s\n", pDevice->m_nKsDeviceNumber, pwszFileName);

		if(fw)
		{
			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_DownloadP2140Firmware() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

			if(fw->size > 0)
			{
				nFileSize = fw->size;

				pFileBuffer = vmalloc(nFileSize + 4);

				pWorkBuffer = vmalloc(nFileSize + 4);

				if(pFileBuffer && pWorkBuffer)
				{
					memset(pFileBuffer, 0x00, nFileSize + 4);

					memcpy(pFileBuffer, fw->data, fw->size);
				}
				else
				{
					LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadP2140Firmware() no buffer\n", pDevice->m_nKsDeviceNumber);

					release_firmware(fw);

					FREE_MEMORY(pFileBuffer);

					FREE_MEMORY(pWorkBuffer);

					return FALSE;
				}
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadP2140Firmware() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

				return FALSE;
			}
		}
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
#else
	do_gettimeofday(&ts);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	start_times = (ULONGLONG)(ts / 1000);
#else
	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
#endif

	if(pFileBuffer[0] == 0x00 &&

		pFileBuffer[1] == 0x11 &&

		pFileBuffer[2] == 0x22 &&

		pFileBuffer[3] == 0x33 &&

		pFileBuffer[4] == 0x44 &&

		pFileBuffer[5] == 0x55 &&

		pFileBuffer[6] == 0x66 &&

		pFileBuffer[7] == 0x77 &&

		pFileBuffer[8] == 0x88 &&

		pFileBuffer[9] == 0x99 &&

		pFileBuffer[10] == 0xAA &&

		pFileBuffer[11] == 0xBB &&

		pFileBuffer[12] == 0xCC &&

		pFileBuffer[13] == 0xDD &&

		pFileBuffer[14] == 0xEE &&

		pFileBuffer[15] == 0xFF) {

		SC0710_StartP2140FlashAccess(pDevice);

		{
			ULONG i = 0;

			BYTE* p = pWorkBuffer;

			ULONG n_real_size = nFileSize - 16;

			ULONG n_real_size_2 = n_real_size >> 1;





			RtlCopyMemory(pWorkBuffer, pFileBuffer + 16 + n_real_size_2 + (n_real_size % 2), n_real_size_2);

			RtlCopyMemory(pWorkBuffer + n_real_size_2, pFileBuffer + 16, n_real_size_2 + (n_real_size % 2));

			for (i = 0; i < n_real_size_2; i++) {

				*p = *p ^ 0xA5;

				p++;
			}
			p = pWorkBuffer + n_real_size_2;

			for (i = 0; i < n_real_size_2; i++) {

				*p = *p ^ 0x5A;

				p++;
			}
		}
		SC0710_SetP2140FlashData(pDevice, pWorkBuffer, nFileSize - 16);

		SC0710_StopP2140FlashAccess(pDevice);
	}
	returns = TRUE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
#else
	do_gettimeofday(&ts);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	stop_times = (ULONGLONG)(ts / 1000);
#else
	stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
#endif

	if(returns) {

		LINUXV4L2_PRINT(KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (SUCCESS)\n", (ULONG)(stop_times - start_times), nFileSize);
	}
	else {

		LINUXV4L2_PRINT(KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (FAIL)\n", (ULONG)(stop_times - start_times), nFileSize);
	}



	FREE_MEMORY(pFileBuffer);

	FREE_MEMORY(pWorkBuffer);

	release_firmware(fw);

	return returns;
}


BOOLEAN SC0710_DownloadC5Firmware(CDevice* pDevice, BYTE* pwszFileName)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	BYTE* pFileBuffer = NULL;

	BYTE* pWorkBuffer = NULL;

	const struct firmware* fw = NULL;

	int ret = 1;

	ULONG             nFileSize = 0;

	BOOLEAN			  returns = FALSE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	u64 ts = 0;
#else
	struct timeval ts;
#endif

	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	if(p_sys_cfg->n_mcu_version == 0) { return FALSE; }

	if(pDevice->m_pKsVideo[0])
	{
		ret = request_firmware(&fw, pwszFileName, &(pDevice->m_pKsVideo[0]->dev));

		LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware() fw(0p%p)\n", pDevice->m_nKsDeviceNumber, fw);
	}

	if(ret) {

		LINUXV4L2_PRINT(KERN_ERR, "[%02d] SC0710_DownloadC5Firmware() Upload failed. (file %s not found?)\n", pDevice->m_nKsDeviceNumber, pwszFileName);

		return FALSE;

	}
	else {

		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware() %s\n", pDevice->m_nKsDeviceNumber, pwszFileName);

		if(fw)
		{
			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

			if(fw->size > 0)
			{
				nFileSize = fw->size;

				pFileBuffer = vmalloc(nFileSize + 256);

				pWorkBuffer = vmalloc(nFileSize + 256);

				if(pFileBuffer && pWorkBuffer)
				{
					memset(pFileBuffer, 0x00, nFileSize + 256);

					memcpy(pFileBuffer, fw->data, fw->size);
				}
				else
				{
					LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware() no buffer\n", pDevice->m_nKsDeviceNumber);

					release_firmware(fw);

					FREE_MEMORY(pFileBuffer);

					FREE_MEMORY(pWorkBuffer);

					return FALSE;
				}
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

				return FALSE;
			}
		}
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
#else
	do_gettimeofday(&ts);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	start_times = (ULONGLONG)(ts / 1000);
#else
	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
#endif

	if(pFileBuffer[0] == 0x00 &&

		pFileBuffer[1] == 0x11 &&

		pFileBuffer[2] == 0x22 &&

		pFileBuffer[3] == 0x33 &&

		pFileBuffer[4] == 0x44 &&

		pFileBuffer[5] == 0x55 &&

		pFileBuffer[6] == 0x66 &&

		pFileBuffer[7] == 0x77 &&

		pFileBuffer[8] == 0x88 &&

		pFileBuffer[9] == 0x99 &&

		pFileBuffer[10] == 0xAA &&

		pFileBuffer[11] == 0xBB &&

		pFileBuffer[12] == 0xCC &&

		pFileBuffer[13] == 0xDD &&

		pFileBuffer[14] == 0xEE &&

		pFileBuffer[15] == 0xFF) {

			{
				ULONG i = 0;

				BYTE* p = pWorkBuffer;

				ULONG n_real_size = nFileSize - 16;

				ULONG n_real_size_2 = n_real_size >> 1;





				RtlCopyMemory(pWorkBuffer, pFileBuffer + 16 + n_real_size_2 + (n_real_size % 2), n_real_size_2);

				RtlCopyMemory(pWorkBuffer + n_real_size_2, pFileBuffer + 16, n_real_size_2 + (n_real_size % 2));

				for (i = 0; i < n_real_size_2; i++) {

					*p = *p ^ 0xA5;

					p++;
				}
				p = pWorkBuffer + n_real_size_2;

				for (i = 0; i < n_real_size_2; i++) {

					*p = *p ^ 0x5A;

					p++;
				}
			}
			nFileSize = nFileSize - 16;

			nFileSize = ((nFileSize + 255) / 256) * 256;

			{
				unsigned char txbuf[16] = { 0 };

				unsigned char rxbuf[8] = { 0 };







				int i2c7BitAdr0x33 = 0x66 >> 1;

				txbuf[0] = 0x12;

				txbuf[1] = 0x34;

				txbuf[2] = 0x35;

				txbuf[ 3 ] = (unsigned char)((nFileSize & 0x000000FF) >>  0);

				txbuf[ 4 ] = (unsigned char)((nFileSize & 0x0000FF00) >>  8);

				txbuf[ 5 ] = (unsigned char)((nFileSize & 0x00FF0000) >> 16);

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, 0x55, txbuf, 6, rxbuf, 3);

				{
					int tryCount = 0;

					for (tryCount = 0; tryCount < 50; tryCount++) {

						if(rxbuf[0] == 0xAA && rxbuf[1] == 0x00 && rxbuf[2] == 0x00) {

							break;
						}
						DELAY_100NS(1000000);

						SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, (unsigned char)i2c7BitAdr0x33, NULL, 0, rxbuf, 3);
					}
					if(tryCount == 50) {

						LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware() tryCount(%d)\n", pDevice->m_nKsDeviceNumber, tryCount);

						return 0;
					}
				}
				{
					unsigned int i = 0;

					unsigned char* spi_buf = (unsigned char*)(pWorkBuffer);

					for (i = 0; i < nFileSize; i += MAX_I2C_TX_SIZE) {

						int perWrSize = nFileSize - i;

						if(perWrSize > MAX_I2C_TX_SIZE) {

							perWrSize = MAX_I2C_TX_SIZE;
						}


						SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, (unsigned char)(i2c7BitAdr0x33), spi_buf + i, perWrSize, NULL, 0);

						if((i % (1024 * 8)) == 0) {

							;
						}

						{
							int tryCount = 0;

							while (1) {

								rxbuf[0] = 0;

								rxbuf[1] = 0;

								rxbuf[2] = 0;

								SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, (unsigned char)(i2c7BitAdr0x33), NULL, 0, rxbuf, 3);
								{
									int rx_spi_size = rxbuf[0] | (rxbuf[1] << 8) | (rxbuf[2] << 16);

									if(rx_spi_size == (i + perWrSize)) {

										break;
									}
								}
								DELAY_100NS(100000);

								tryCount++;

								if(tryCount > 100) {

									LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware() tryCount(%d) i(%d) --\n", pDevice->m_nKsDeviceNumber, tryCount, i);

									return 0;
								}
							}
						}
					}
				}
			}
	}
	returns = TRUE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
#else
	do_gettimeofday(&ts);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	stop_times = (ULONGLONG)(ts / 1000);
#else
	stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
#endif

	if(returns) {

		LINUXV4L2_PRINT(KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (SUCCESS)\n", (ULONG)(stop_times - start_times), nFileSize);
	}
	else {

		LINUXV4L2_PRINT(KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (FAIL)\n", (ULONG)(stop_times - start_times), nFileSize);
	}



	FREE_MEMORY(pFileBuffer);

	FREE_MEMORY(pWorkBuffer);

	release_firmware(fw);

	return returns;
}

BOOLEAN SC0710_DownloadC5Firmware_SPI(CDevice* pDevice, BYTE* pwszFileName)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	BYTE* pFileBuffer = NULL;

	BYTE* pWorkBuffer = NULL;

	const struct firmware* fw = NULL;

	int ret = 1;

	ULONG             nFileSize = 0;

	BOOLEAN			  returns = FALSE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	u64 ts = 0;
#else
	struct timeval ts;
#endif

	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	unsigned char txbuf[2] = { 0x03, 0x01 };

	unsigned char rxbuf = 0;

	if(p_sys_cfg->n_mcu_version == 0) { return FALSE; }

	if(pDevice->m_pKsVideo[0])
	{
		ret = request_firmware(&fw, pwszFileName, &(pDevice->m_pKsVideo[0]->dev));

		LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware_SPI() fw(0p%p)\n", pDevice->m_nKsDeviceNumber, fw);
	}

	if(ret) {

		LINUXV4L2_PRINT(KERN_ERR, "[%02d] SC0710_DownloadC5Firmware_SPI() Upload failed. (file %s not found?)\n", pDevice->m_nKsDeviceNumber, pwszFileName);

		return FALSE;

	}
	else {

		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware_SPI() %s\n", pDevice->m_nKsDeviceNumber, pwszFileName);

		if(fw)
		{
			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware_SPI() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

			if(fw->size > 0)
			{
				nFileSize = fw->size;

				pFileBuffer = vmalloc(((nFileSize + 3) / 4) * 4);

				pWorkBuffer = vmalloc(((nFileSize + 3) / 4) * 4);

				if(pFileBuffer && pWorkBuffer)
				{
					memset(pFileBuffer, 0x00, ((nFileSize + 3) / 4) * 4);

					memcpy(pFileBuffer, fw->data, fw->size);
				}
				else
				{
					LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware_SPI() no buffer\n", pDevice->m_nKsDeviceNumber);

					release_firmware(fw);

					FREE_MEMORY(pFileBuffer);

					FREE_MEMORY(pWorkBuffer);

					return FALSE;
				}
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware_SPI() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

				return FALSE;
			}
		}
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
#else
	do_gettimeofday(&ts);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	start_times = (ULONGLONG)(ts / 1000);
#else
	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
#endif

	if(pFileBuffer[0] == 0x00 &&

		pFileBuffer[1] == 0x11 &&

		pFileBuffer[2] == 0x22 &&

		pFileBuffer[3] == 0x33 &&

		pFileBuffer[4] == 0x44 &&

		pFileBuffer[5] == 0x55 &&

		pFileBuffer[6] == 0x66 &&

		pFileBuffer[7] == 0x77 &&

		pFileBuffer[8] == 0x88 &&

		pFileBuffer[9] == 0x99 &&

		pFileBuffer[10] == 0xAA &&

		pFileBuffer[11] == 0xBB &&

		pFileBuffer[12] == 0xCC &&

		pFileBuffer[13] == 0xDD &&

		pFileBuffer[14] == 0xEE &&

		pFileBuffer[15] == 0xFF) {

		ULONG R000000EC = 0x00000000;

		{
			ULONG i = 0;

			BYTE* p = pWorkBuffer;

			ULONG n_real_size = nFileSize - 16;

			ULONG n_real_size_2 = n_real_size >> 1;





			RtlCopyMemory(pWorkBuffer, pFileBuffer + 16 + n_real_size_2 + (n_real_size % 2), n_real_size_2);

			RtlCopyMemory(pWorkBuffer + n_real_size_2, pFileBuffer + 16, n_real_size_2 + (n_real_size % 2));

			for (i = 0; i < n_real_size_2; i++) {

				*p = *p ^ 0xA5;

				p++;
			}
			p = pWorkBuffer + n_real_size_2;

			for (i = 0; i < n_real_size_2; i++) {

				*p = *p ^ 0x5A;

				p++;
			}
		}
		nFileSize = nFileSize - 16;

		if((pDevice->iManufacturer == 0x29) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ||

			(pDevice->iManufacturer == 0x2D)) {

			;
		}
		else {



			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);

			do {

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, &rxbuf, 1);

			} while (rxbuf != 0x02);
		}
		if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

			(pDevice->iManufacturer == 0x1A) ||

			(pDevice->iManufacturer == 0x1B) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x29) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ||

			(pDevice->iManufacturer == 0x2D)) {

			;
		}
		else {

			R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

			R000000EC &= ~0x01;

			SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);
		}
		SC0710_StartC5FlashAccess(pDevice);

		SC0710_SetC5FlashData(pDevice, pWorkBuffer, nFileSize);

		SC0710_StopC5FlashAccess(pDevice);

		if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

			(pDevice->iManufacturer == 0x1A) ||

			(pDevice->iManufacturer == 0x1B) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x29) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ||

			(pDevice->iManufacturer == 0x2D)) {

			;
		}
		else {

			R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

			R000000EC |= 0x01;

			SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);
		}

		if((pDevice->iManufacturer == 0x29) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ||

			(pDevice->iManufacturer == 0x2D)) {

			;
		}
		else {

			txbuf[1] = 0x03;

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);

			do {

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, &rxbuf, 1);

			} while (rxbuf != 0x00);
		}
	}


	returns = TRUE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
#else
	do_gettimeofday(&ts);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	stop_times = (ULONGLONG)(ts / 1000);
#else
	stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
#endif

	if(returns) {

		LINUXV4L2_PRINT(KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (SUCCESS)\n", (ULONG)(stop_times - start_times), nFileSize);
	}
	else {

		LINUXV4L2_PRINT(KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (FAIL)\n", (ULONG)(stop_times - start_times), nFileSize);
	}



	FREE_MEMORY(pFileBuffer);

	FREE_MEMORY(pWorkBuffer);

	release_firmware(fw);

	return returns;
}

BOOLEAN SC0720_DownloadC7Firmware_SPI( CDevice * pDevice )
{
	BOOLEAN returns = FALSE;

	BYTE txbuf_gpio_high[ 6 ] = {  0x12, 0x34, 0x92, 0x0E, 0x01, 0x00 };

	BYTE txbuf_gpio_low[  6 ] = {  0x12, 0x34, 0x92, 0x0E, 0x00, 0x00 };

	BYTE pwsz_ch_12[64] = "SC0720.CS12.FW.HEX";

	BYTE pwsz_ch_34[64] = "SC0720.CS34.FW.HEX";

	ULONG R000000CC = 0x00000000;

	SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, 0x55, txbuf_gpio_high, 5, NULL, 0 );

	DELAY_100NS( 5000000 );




	R000000CC = SC0710_GetRegister( pDevice, 0x000000CC );

	R000000CC &= ~0x3000;

	SC0710_SetRegister( pDevice, 0x000000CC, R000000CC );

	returns = SC0710_DownloadC5Firmware_SPI( pDevice, pwsz_ch_12 );

	if( returns == FALSE ) {

		LINUXV4L2_DEBUG(KERN_INFO, "SC0720_DownloadC7Firmware_SPI( CH01 ) - DOWNLOAD %s FAIL!!\n", pwsz_ch_12);

		goto EXIT;
	}



	R000000CC = SC0710_GetRegister( pDevice, 0x000000CC );

	R000000CC &= ~0x3000;

	R000000CC |=  0x1000;

	SC0710_SetRegister( pDevice, 0x000000CC, R000000CC );

	returns = SC0710_DownloadC5Firmware_SPI( pDevice, pwsz_ch_12 );

	if( returns == FALSE ) {

		LINUXV4L2_DEBUG(KERN_INFO, "SC0720_DownloadC7Firmware_SPI( CH02 ) - DOWNLOAD %s FAIL!!\n", pwsz_ch_12);

		goto EXIT;
	}

	if( ((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D)) ) {

		;
	}
	else {



		R000000CC = SC0710_GetRegister( pDevice, 0x000000CC );

		R000000CC &= ~0x3000;

		R000000CC |=  0x2000;

		SC0710_SetRegister( pDevice, 0x000000CC, R000000CC );

		returns = SC0710_DownloadC5Firmware_SPI( pDevice, pwsz_ch_34 );

		if( returns == FALSE ) {

			LINUXV4L2_DEBUG(KERN_INFO, "SC0720_DownloadC7Firmware_SPI( CH03 ) - DOWNLOAD %s FAIL!!\n", pwsz_ch_34);

			goto EXIT;
		}



		R000000CC = SC0710_GetRegister( pDevice, 0x000000CC );

		R000000CC &= ~0x3000;

		R000000CC |= 0x3000;

		SC0710_SetRegister( pDevice, 0x000000CC, R000000CC );

		returns = SC0710_DownloadC5Firmware_SPI( pDevice, pwsz_ch_34 );

		if( returns == FALSE ) {

			LINUXV4L2_DEBUG(KERN_INFO, "SC0720_DownloadC7Firmware_SPI( CH04 ) - DOWNLOAD %s FAIL!!\n", pwsz_ch_34);

			goto EXIT;
		}
	}

EXIT:

	SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, 0x55, txbuf_gpio_low, 5, NULL, 0 );

	return returns;
}



static const int SC0710_CYCLONE10_FLASH_STOP_ADDR = 0x2000000;

unsigned char sc0710_cyclone10_reverse( unsigned char b )
{
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;

   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;

   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;

   return b;
}

BOOLEAN SC0710_SetCyclone10FlashData( CDevice * pDevice, BYTE * pBuffer, ULONG nBufferSize )
{
	ULONG  lengths = 0;

	ULONG  rx_buf[ 4 ] = { 0x00, 0x00, 0x00, 0x00 };

	SC0710_AccessSpiRegisterS_PORT2( pDevice, 0x0000009E , NULL, 0, rx_buf, 3 );

	LINUXV4L2_PRINT( KERN_INFO,"CYCOLNE10 ID (0x%x) (0x%x) (0x%x)", rx_buf[ 0 ], rx_buf[ 1 ], rx_buf[ 2 ] );

	{
		ULONG i = 0;

		LINUXV4L2_PRINT( KERN_INFO,"THIS FILE MUST BE SWAPPED" );

		for( i = 0 ; i < nBufferSize ; i++ ) {

		   *(pBuffer + i) = sc0710_cyclone10_reverse( *(pBuffer + i) );
		}
	}

	if( (rx_buf[ 0 ] == 0x20) && (rx_buf[ 1 ] == 0xBB) && (rx_buf[ 2 ] == 0x19) ) {

		ULONG  tx_buf[ 4 ];

		ULONG i = 0;

		LONG wridx = ((nBufferSize - 1) / SC0710_TX_FIFO_MAX) * SC0710_TX_FIFO_MAX;

		tx_buf[ 0 ] = 0xEE;

		tx_buf[ 1 ] = 0xAF;

		SC0710_AccessSpiRegisterS_PORT2( pDevice, 0x000000B1 , tx_buf, 2, NULL, 0 );

		for( i = 0 ; i < nBufferSize ; i += SC0710_FLASH_BLOCK_SIZE ) {

			ULONG  tx_buf[ 4 ];

			ULONG status = 0x00000000;

			int error_count = 0;

			BYTE * po = (BYTE *)(&i);

			LINUXV4L2_PRINT( KERN_INFO,"FIRMWARE.ERASED: %d / %d BYTES", i, nBufferSize);



			if( i > SC0710_CYCLONE10_FLASH_STOP_ADDR ) {



				return FALSE;
			}
			else {


			}

			tx_buf[ 0 ] = po[ 3 ];

			tx_buf[ 1 ] = po[ 2 ];

			tx_buf[ 2 ] = po[ 1 ];

			tx_buf[ 3 ] = po[ 0 ];

			SC0710_AccessSpiRegisterS_PORT2( pDevice, 0x00000006 , NULL, 0, NULL, 0 );

			SC0710_AccessSpiRegisterS_PORT2( pDevice, 0x000000DC , tx_buf, 4, NULL, 0 );

			do {

				SC0710_AccessSpiRegisterS_PORT2( pDevice, 0x00000005 , NULL, 0, &status, 1 );

				error_count++;

				if( error_count > 0 && (error_count % 1000) == 0 ) {

					LINUXV4L2_PRINT( KERN_INFO,"STILL IN ERASE LOOP(%d)", i);
				}

			} while( status & 0x00000001  );
		}

		for( ; wridx >= 0 ; wridx -= SC0710_TX_FIFO_MAX ) {

			LINUXV4L2_PRINT( KERN_INFO,"FIRMWARE.UPDATE: %d / %d BYTES", wridx, nBufferSize);

			if( (ULONG)(wridx + SC0710_TX_FIFO_MAX) > nBufferSize ) {

				lengths = nBufferSize - wridx;
			}
			else {

				lengths = SC0710_TX_FIFO_MAX;
			}



			{	ULONG  address = wridx;

				ULONG  tx_buf[ SC0710_TX_FIFO_MAX + 4 ];

				BYTE * po = (BYTE *)(&address);

				BYTE * pe = (BYTE *)(pBuffer + wridx);

				ULONG j = 0;

				ULONG status = 0x00000000;

				int error_count = 0;

				if( address > SC0710_CYCLONE10_FLASH_STOP_ADDR ) {



					return FALSE;
				}
				else {



					;
				}

				tx_buf[ 0 ] = po[ 3 ];

				tx_buf[ 1 ] = po[ 2 ];

				tx_buf[ 2 ] = po[ 1 ];

				tx_buf[ 3 ] = po[ 0 ];

				for( j = 0 ; j < lengths ; j++ ) {

					tx_buf[ j + 4 ] = pe[ j ];
				}
				SC0710_AccessSpiRegisterS_PORT2( pDevice, 0x00000006 , NULL, 0, NULL, 0 );

				SC0710_AccessSpiRegisterS_PORT2( pDevice, 0x00000012 , tx_buf, lengths + 4, NULL, 0 );


				do {

					SC0710_AccessSpiRegisterS_PORT2( pDevice, 0x00000005 , NULL, 0, &status, 1 );

					error_count++;

					if( error_count > 0 && (error_count % 1000) == 0 ) {

						LINUXV4L2_PRINT( KERN_INFO,"STILL IN UPDATE LOOP (%d)", i );
					}

				} while( status & 0x00000001  );
			}
		}
	}
	return TRUE;
}

BOOLEAN SC0710_Cyclone10_Firmware_SPI( CDevice * pDevice, BYTE* pwszFileName )
{
	SC0710_SYS_CFG *  p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	BYTE *            pFileBuffer = NULL;

	BYTE *            pWorkBuffer = NULL;

	const struct firmware* fw = NULL;

	ULONG             nFileSize = 0;

	BOOLEAN			  returns = TRUE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	u64 ts = 0;
#else
	struct timeval ts;
#endif

	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	unsigned char txbuf[ 2 ] = { 0x03, 0x01 };

	unsigned char rxbuf = 0;

	if( p_sys_cfg->n_mcu_version == 0 ) { return FALSE; }

	if(pDevice->m_pKsVideo[0])
	{
		returns = request_firmware(&fw, pwszFileName, &(pDevice->m_pKsVideo[0]->dev));

		LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_Cyclone10_Firmware_SPI() fw(0p%p)\n", pDevice->m_nKsDeviceNumber, fw);
	}

	if(returns) {

		LINUXV4L2_PRINT(KERN_ERR, "[%02d] SC0710_Cyclone10_Firmware_SPI() Upload failed. (file %s not found?)\n", pDevice->m_nKsDeviceNumber, pwszFileName);

		return FALSE;

	}
	else {

		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_Cyclone10_Firmware_SPI() %s\n", pDevice->m_nKsDeviceNumber, pwszFileName);

		if(fw)
		{
			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_Cyclone10_Firmware_SPI() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

			if(fw->size > 0)
			{
				nFileSize = fw->size;

				pFileBuffer = vmalloc(((nFileSize + 3) / 4) * 4);

				pWorkBuffer = vmalloc(((nFileSize + 3) / 4) * 4);

				if(pFileBuffer && pWorkBuffer)
				{
					memset(pFileBuffer, 0x00, ((nFileSize + 3) / 4) * 4);

					memcpy(pFileBuffer, fw->data, fw->size);
				}
				else
				{
					LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_Cyclone10_Firmware_SPI() no buffer\n", pDevice->m_nKsDeviceNumber);

					release_firmware(fw);

					FREE_MEMORY(pFileBuffer);

					FREE_MEMORY(pWorkBuffer);

					return FALSE;
				}
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_Cyclone10_Firmware_SPI() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

				return FALSE;
			}
		}
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
#else
	do_gettimeofday(&ts);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	start_times = (ULONGLONG)(ts / 1000);
#else
	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
#endif

	if( pFileBuffer[  0 ] == 0x00 &&

		pFileBuffer[  1 ] == 0x11 &&

		pFileBuffer[  2 ] == 0x22 &&

		pFileBuffer[  3 ] == 0x33 &&

		pFileBuffer[  4 ] == 0x44 &&

		pFileBuffer[  5 ] == 0x55 &&

		pFileBuffer[  6 ] == 0x66 &&

		pFileBuffer[  7 ] == 0x77 &&

		pFileBuffer[  8 ] == 0x88 &&

		pFileBuffer[  9 ] == 0x99 &&

		pFileBuffer[ 10 ] == 0xAA &&

		pFileBuffer[ 11 ] == 0xBB &&

		pFileBuffer[ 12 ] == 0xCC &&

		pFileBuffer[ 13 ] == 0xDD &&

		pFileBuffer[ 14 ] == 0xEE &&

		pFileBuffer[ 15 ] == 0xFF ) {

		ULONG R000000EC = 0x00000000;

		{	ULONG n_real_size = nFileSize - 16;

			ULONG n_real_size_2 = n_real_size >> 1;

			BYTE * p = pWorkBuffer;

			ULONG i = 0;





			RtlCopyMemory( pWorkBuffer                , pFileBuffer + 16 + n_real_size_2 + (n_real_size % 2), n_real_size_2 );

			RtlCopyMemory( pWorkBuffer + n_real_size_2, pFileBuffer + 16									, n_real_size_2 + (n_real_size % 2) );

			for( i = 0 ; i < n_real_size_2 ; i++ ) {

			   *p = *p ^ 0xA5;

				p++;
			}
			p = pWorkBuffer + n_real_size_2;

			for( i = 0 ; i < n_real_size_2 ; i++ ) {

			   *p = *p ^ 0x5A;

				p++;
			}
		}
		nFileSize = nFileSize - 16;

		if(	(pDevice->iManufacturer == 0x23) ||

			(pDevice->iManufacturer == 0x29) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ) {

			;
		}
		else {



			SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0 );

			do {

				SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, &rxbuf, 1 );

			} while( rxbuf != 0x02 );
		}

		if( (pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

			(pDevice->iManufacturer == 0x1A) ||

			(pDevice->iManufacturer == 0x1B) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x23) ||

			(pDevice->iManufacturer == 0x29) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ||

			(pDevice->iManufacturer == 0x2D) ) {

			;
		}
		else {

			R000000EC = SC0710_GetRegister( pDevice, 0x000000EC );

			R000000EC &= ~0x01;

			SC0710_SetRegister( pDevice, 0x000000EC, R000000EC );
		}

		SC0710_StartC5FlashAccess( pDevice );

		returns = SC0710_SetCyclone10FlashData( pDevice, pWorkBuffer, nFileSize );

		SC0710_StopC5FlashAccess( pDevice );

		if( (pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

			(pDevice->iManufacturer == 0x1A) ||

			(pDevice->iManufacturer == 0x1B) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x23) ||

			(pDevice->iManufacturer == 0x29) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ||

			(pDevice->iManufacturer == 0x2D) ) {

			;
		}
		else {

			R000000EC = SC0710_GetRegister( pDevice, 0x000000EC );

			R000000EC |= 0x01;

			SC0710_SetRegister( pDevice, 0x000000EC, R000000EC );
		}

		if(	(pDevice->iManufacturer == 0x23) ||

			(pDevice->iManufacturer == 0x29) ||

			(pDevice->iManufacturer == 0x2B) ||

			(pDevice->iManufacturer == 0x2D) ) {

			;
		}
		else {

			txbuf[ 1 ] = 0x03;

			SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0 );

			do {

				SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, &rxbuf, 1 );

			} while( rxbuf != 0x00 );
		}
	}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
#else
	do_gettimeofday(&ts);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	stop_times = (ULONGLONG)(ts / 1000);
#else
	stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
#endif

	if( returns ) {

		LINUXV4L2_PRINT( KERN_INFO,"TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (SUCCESS)\n", (ULONG)(stop_times - start_times), nFileSize);
	}
	else {

		LINUXV4L2_PRINT( KERN_INFO,"TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (FAIL)\n", (ULONG)(stop_times - start_times), nFileSize);
	}



	FREE_MEMORY( pFileBuffer );

	FREE_MEMORY( pWorkBuffer );

	return TRUE;
}

BOOLEAN SC0720_DownloadCyclone10Firmware_SPI( CDevice * pDevice )
{
	BOOLEAN returns = FALSE;

	ULONG R000000CC = 0x00000000;

	R000000CC = SC0710_GetRegister( pDevice, 0x000000CC );

	R000000CC &= ~0x3000;

	R000000CC |=  0x1000;

	SC0710_SetRegister( pDevice, 0x000000CC, R000000CC );

	returns = SC0710_Cyclone10_Firmware_SPI( pDevice, "SC0720.CS.FW.HEX" );

	R000000CC = SC0710_GetRegister( pDevice, 0x000000CC );

	R000000CC &= ~0x3000;

	R000000CC |=  0x2000;

	SC0710_SetRegister( pDevice, 0x000000CC, R000000CC );

	R000000CC &= ~0x3000;

	SC0710_SetRegister( pDevice, 0x000000CC, R000000CC );

	if( returns == FALSE ) {

		LINUXV4L2_PRINT( KERN_INFO, "SC0720_DownloadCyclone10Firmware_SPI() - DOWNLOAD %s FAIL!!\n", "SC0720.CS.FW.HEX");

		goto EXIT;
	}

EXIT:

	return returns;
}

BOOLEAN SC400_Cyclone10_Firmware_SPI( CDevice* pDevice, BYTE* pwszFileName )
{
	SC0710_SYS_CFG *  p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	BYTE *            pFileBuffer = NULL;

	BYTE *            pWorkBuffer = NULL;

	const struct firmware* fw = NULL;

	ULONG             nFileSize = 0;

	BOOLEAN			  returns = TRUE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	u64 ts = 0;
#else
	struct timeval ts;
#endif

	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;

	if( p_sys_cfg->n_mcu_version == 0 ) { return FALSE; }

	LINUXV4L2_PRINT( KERN_INFO,"SC400_Cyclone10_Firmware_SPI( \"%s\" )\n", pwszFileName);

	if(pDevice->m_pKsVideo[0])
	{
		returns = request_firmware(&fw, pwszFileName, &(pDevice->m_pKsVideo[0]->dev));

		LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC400_Cyclone10_Firmware_SPI() fw(0p%p)\n", pDevice->m_nKsDeviceNumber, fw);
	}

	if(returns) {

		LINUXV4L2_PRINT(KERN_ERR, "[%02d] SC400_Cyclone10_Firmware_SPI() Upload failed. (file %s not found?)\n", pDevice->m_nKsDeviceNumber, pwszFileName);

		return FALSE;

	}
	else {

		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC400_Cyclone10_Firmware_SPI() %s\n", pDevice->m_nKsDeviceNumber, pwszFileName);

		if(fw)
		{
			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC400_Cyclone10_Firmware_SPI() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

			if(fw->size > 0)
			{
				nFileSize = fw->size;

				pFileBuffer = vmalloc(((nFileSize + 3) / 4) * 4);

				pWorkBuffer = vmalloc(((nFileSize + 3) / 4) * 4);

				if(pFileBuffer && pWorkBuffer)
				{
					memset(pFileBuffer, 0x00, ((nFileSize + 3) / 4) * 4);

					memcpy(pFileBuffer, fw->data, fw->size);
				}
				else
				{
					LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC400_Cyclone10_Firmware_SPI() no buffer\n", pDevice->m_nKsDeviceNumber);

					release_firmware(fw);

					FREE_MEMORY(pFileBuffer);

					FREE_MEMORY(pWorkBuffer);

					return FALSE;
				}
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC400_Cyclone10_Firmware_SPI() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

				return FALSE;
			}
		}
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
#else
	do_gettimeofday(&ts);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	start_times = (ULONGLONG)(ts / 1000);
#else
	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
#endif

	if( pFileBuffer[  0 ] == 0x00 &&

		pFileBuffer[  1 ] == 0x11 &&

		pFileBuffer[  2 ] == 0x22 &&

		pFileBuffer[  3 ] == 0x33 &&

		pFileBuffer[  4 ] == 0x44 &&

		pFileBuffer[  5 ] == 0x55 &&

		pFileBuffer[  6 ] == 0x66 &&

		pFileBuffer[  7 ] == 0x77 &&

		pFileBuffer[  8 ] == 0x88 &&

		pFileBuffer[  9 ] == 0x99 &&

		pFileBuffer[ 10 ] == 0xAA &&

		pFileBuffer[ 11 ] == 0xBB &&

		pFileBuffer[ 12 ] == 0xCC &&

		pFileBuffer[ 13 ] == 0xDD &&

		pFileBuffer[ 14 ] == 0xEE &&

		pFileBuffer[ 15 ] == 0xFF ) {

		{	ULONG n_real_size = nFileSize - 16;

			ULONG n_real_size_2 = n_real_size >> 1;

			BYTE * p = pWorkBuffer;

			ULONG i = 0;





			RtlCopyMemory( pWorkBuffer                , pFileBuffer + 16 + n_real_size_2 + (n_real_size % 2), n_real_size_2 );

			RtlCopyMemory( pWorkBuffer + n_real_size_2, pFileBuffer + 16									, n_real_size_2 + (n_real_size % 2) );

			for( i = 0 ; i < n_real_size_2 ; i++ ) {

			   *p = *p ^ 0xA5;

				p++;
			}
			p = pWorkBuffer + n_real_size_2;

			for( i = 0 ; i < n_real_size_2 ; i++ ) {

			   *p = *p ^ 0x5A;

				p++;
			}
		}
		nFileSize = nFileSize - 16;

		SC0710_StartC5FlashAccess( pDevice );

		returns = SC0710_SetCyclone10FlashData( pDevice, pWorkBuffer, nFileSize );

		SC0710_StopC5FlashAccess( pDevice );
	}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
#else
	do_gettimeofday(&ts);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	stop_times = (ULONGLONG)(ts / 1000);
#else
	stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
#endif

	if( returns ) {

		LINUXV4L2_PRINT( KERN_INFO,"TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (SUCCESS)\n", (ULONG)(stop_times - start_times), nFileSize);
	}
	else {

		LINUXV4L2_PRINT( KERN_INFO,"TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (FAIL)\n", (ULONG)(stop_times - start_times), nFileSize);
	}



	FREE_MEMORY( pFileBuffer );

	FREE_MEMORY( pWorkBuffer );

	return TRUE;
}

BOOLEAN SC0710_DownloadCyclone10Firmware_SPI( CDevice * pDevice )
{
	BOOLEAN returns = FALSE;

	ULONG R000000CC = 0x00000000;

	R000000CC = SC0710_GetRegister( pDevice, 0x000000CC );

	R000000CC &= ~0x3000;

	R000000CC |= 0x1000;

	SC0710_SetRegister( pDevice, 0x000000CC, R000000CC );

	returns = SC400_Cyclone10_Firmware_SPI( pDevice, "SC0710.CS.FW.HEX" );

	R000000CC = SC0710_GetRegister( pDevice, 0x000000CC );

	R000000CC &= ~0x3000;

	R000000CC |= 0x2000;

	SC0710_SetRegister( pDevice, 0x000000CC, R000000CC );

	R000000CC &= ~0x3000;

	SC0710_SetRegister( pDevice, 0x000000CC, R000000CC );

	if( returns == FALSE ) {

		LINUXV4L2_PRINT( KERN_INFO,"SC0720_DownloadCyclone10Firmware_SPI() - DOWNLOAD %s FAIL!!\n", "SC0720.CYCLONE10.FW.HEX");

		goto EXIT;
	}

EXIT:

	return returns;
}



static BYTE c5m2_id[4] = { 0xC2, 0x04, 0x80, 0x80 };

static BYTE c7m2_id[4] = { 0x41, 0x11, 0x20, 0x43 };

static BYTE c7m2_id_2[4] = { 0x41, 0x11, 0x30, 0x43 };

static BYTE c7m2_id_3[4] = { 0xC2, 0x00, 0x80, 0x80 };

#define SPI_CHIP_SEL_HIGH  0x0001

#define SPI_CHIP_SEL_LOW   0x0000

static const int CMD_C5_READ_ID = 0x07;
static const int CMD_C5_READ_STATUS = 0x09;
static const int CMD_C5_CLEAR = 0x70;
static const int CMD_C5_WRITE_INC = 0x41;
static const int CMD_C5_WRITE_EN = 0x4A;
static const int CMD_C5_REFRESH = 0x71;
static const int CMD_C5_WRITE_DIS = 0x4F;
static const int CMD_C5_PROGRAM_SPI = 0x74;

void spi_tx(CDevice* pDevice, ULONG wrbytecnt, unsigned char* wrbuf)
{
	ULONG i = 0;

	ULONG R00002064 = 0;

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);


	if( (pDevice->idVendor == 0x12AB) && ((pDevice->idProduct & 0xFFFF) == 0x0710) &&

		(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x13)) ||
		 ((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x13)) ) )
 	{
		for (i = 0; i < wrbytecnt; i++) {

			SC0710_SetRegister(pDevice, 0x00002068, (ULONG)(wrbuf[i]));

			if(i%0x40==0x3F)
			{
				usleep_range(10, 20);
			}
		}

		{
			int cnt = 0;

			do {

				R00002064 = SC0710_GetRegister(pDevice, 0x00002064);

				cnt ++;

			} while (!( R00002064 & 0x04) && cnt < 100000);

		}

		{

			p_sys_cfg->n_spi_ctrl_reg |= (SC0710_SPI_CTRL_RESET_RX_FIFO | SC0710_SPI_CTRL_RESET_TX_FIFO);

			SC0710_SetRegister(pDevice, 0x00002060, p_sys_cfg->n_spi_ctrl_reg);

			p_sys_cfg->n_spi_ctrl_reg &= ~(SC0710_SPI_CTRL_RESET_RX_FIFO | SC0710_SPI_CTRL_RESET_TX_FIFO);
		}
	}
	else
	{
		for (i = 0; i < wrbytecnt; i++) {

			SC0710_SetRegister(pDevice, 0x00002068, (ULONG)(wrbuf[i]));

			do {

				R00002064 = SC0710_GetRegister(pDevice, 0x00002064);

			} while (R00002064 & 0x01);

			SC0710_GetRegister(pDevice, 0x0000206C);
		}
	}
}
void spi_rx(CDevice* pDevice, ULONG rdbytecnt, unsigned char* rdbuf)
{
	ULONG i = 0;

	ULONG R00002064 = 0;

	for (i = 0; i < rdbytecnt; i++) {

		SC0710_SetRegister(pDevice, 0x00002068, 0x00);

		do {

			R00002064 = SC0710_GetRegister(pDevice, 0x00002064);

		} while (R00002064 & 0x01);

		rdbuf[i] = (unsigned char)(SC0710_GetRegister(pDevice, 0x0000206C));
	}
}

void set_chip_sel(CDevice* pDevice, bool level)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	if(level == true) {

		p_sys_cfg->n_spi_ctrl_reg |= SC0710_SPI_CTRL_TRANS_STOP;

		SC0710_SetRegister(pDevice, 0x00002060, p_sys_cfg->n_spi_ctrl_reg);

		SC0710_SetRegister(pDevice, 0x00002070, SPI_CHIP_SEL_HIGH);
	}
	else {

		{

			p_sys_cfg->n_spi_ctrl_reg |= (SC0710_SPI_CTRL_RESET_RX_FIFO | SC0710_SPI_CTRL_RESET_TX_FIFO);

			SC0710_SetRegister(pDevice, 0x00002060, p_sys_cfg->n_spi_ctrl_reg);

			p_sys_cfg->n_spi_ctrl_reg &= ~(SC0710_SPI_CTRL_RESET_RX_FIFO | SC0710_SPI_CTRL_RESET_TX_FIFO);
		}
		SC0710_SetRegister(pDevice, 0x00002070, SPI_CHIP_SEL_LOW);

		p_sys_cfg->n_spi_ctrl_reg &= ~SC0710_SPI_CTRL_TRANS_STOP;

		SC0710_SetRegister(pDevice, 0x00002060, p_sys_cfg->n_spi_ctrl_reg);
	}
}

BOOLEAN SC0710_DownloadC5Firmware_Online(CDevice* pDevice, BYTE* pwszFileName, ULONG powersaving)
{

	BYTE* pFileBuffer = NULL;

	BYTE* pWorkBuffer = NULL;

	const struct firmware* fw = NULL;

	int ret = 1;

	ULONG             nFileSize = 0;

	BOOLEAN			  returns = FALSE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	u64 ts = 0;
#else
	struct timeval ts;
#endif

	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;





	unsigned char buf[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	if(p_sys_cfg->n_mcu_version == 0) { return FALSE; }

	if(pDevice->m_pKsVideo[0])
	{
		ret = request_firmware(&fw, pwszFileName, &(pDevice->m_pKsVideo[0]->dev));

		LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware_Online() fw(0p%p)\n", pDevice->m_nKsDeviceNumber, fw);
	}

	if(ret) {

		LINUXV4L2_PRINT(KERN_ERR, "[%02d] SC0710_DownloadC5Firmware_Online() Upload failed. (file %s not found?)\n", pDevice->m_nKsDeviceNumber, pwszFileName);

		return FALSE;

	}
	else {

		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware_Online() %s, powersaving(0x%x)\n", pDevice->m_nKsDeviceNumber, pwszFileName, powersaving);

		if(fw)
		{
			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware_Online() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

			if(fw->size > 0)
			{
				nFileSize = fw->size;

				pFileBuffer = vmalloc(((nFileSize + 3) / 4) * 4);

				pWorkBuffer = vmalloc(((nFileSize + 3) / 4) * 4);

				if(pFileBuffer && pWorkBuffer)
				{
					memset(pFileBuffer, 0x00, ((nFileSize + 3) / 4) * 4);

					memcpy(pFileBuffer, fw->data, fw->size);
				}
				else
				{
					LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware_Online() no buffer\n", pDevice->m_nKsDeviceNumber);

					release_firmware(fw);

					FREE_MEMORY(pFileBuffer);

					FREE_MEMORY(pWorkBuffer);

					return FALSE;
				}
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadC5Firmware_Online() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

				return FALSE;
			}
		}
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
#else
	do_gettimeofday(&ts);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	start_times = (ULONGLONG)(ts / 1000);
#else
	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
#endif

	if(pFileBuffer[0] == 0x00 &&

		pFileBuffer[1] == 0x11 &&

		pFileBuffer[2] == 0x22 &&

		pFileBuffer[3] == 0x33 &&

		pFileBuffer[4] == 0x44 &&

		pFileBuffer[5] == 0x55 &&

		pFileBuffer[6] == 0x66 &&

		pFileBuffer[7] == 0x77 &&

		pFileBuffer[8] == 0x88 &&

		pFileBuffer[9] == 0x99 &&

		pFileBuffer[10] == 0xAA &&

		pFileBuffer[11] == 0xBB &&

		pFileBuffer[12] == 0xCC &&

		pFileBuffer[13] == 0xDD &&

		pFileBuffer[14] == 0xEE &&

		pFileBuffer[15] == 0xFF) {

		ULONG i = 0;

		BYTE* p = pWorkBuffer;

		ULONG n_real_size = nFileSize - 16;

		ULONG n_real_size_2 = n_real_size >> 1;

		ULONG count = 0;

		RtlCopyMemory(pWorkBuffer, pFileBuffer + 16 + n_real_size_2 + (n_real_size % 2), n_real_size_2);

		RtlCopyMemory(pWorkBuffer + n_real_size_2, pFileBuffer + 16, n_real_size_2 + (n_real_size % 2));

		for (i = 0; i < n_real_size_2; i++) {

			*p = *p ^ 0xA5;

			p++;
		}
		p = pWorkBuffer + n_real_size_2;

		for (i = 0; i < n_real_size_2; i++) {

			*p = *p ^ 0x5A;

			p++;
		}

		nFileSize = nFileSize - 16;



		RtlFillMemory(buf, 4, 0xFF);

		if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

			(pDevice->iManufacturer == 0x1A) ||

			(pDevice->iManufacturer == 0x1B) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x1E) ||

			(pDevice->iManufacturer == 0x1F)) {

			;
		}
		else {

			ULONG R000000EC = 0;

			R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

			R000000EC &= ~0x01;

			SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);
		}
		DELAY_100NS( 100000 );

		SC0710_StartC5FlashAccess(pDevice);

		set_chip_sel(pDevice, 0);
		set_chip_sel(pDevice, 1);

		set_chip_sel(pDevice, 0);
		buf[0] = CMD_C5_READ_ID;
		spi_tx(pDevice, 4, buf);
		spi_rx(pDevice, 4, buf + 4);
		set_chip_sel(pDevice, 1);

		LINUXV4L2_PRINT(KERN_INFO, "CMD_C5_READ_ID(%x, %x, %x, %x)", *(buf + 4), *(buf + 5), *(buf + 6), *(buf + 7));

		if(memcmp(buf + 4, c5m2_id, 4) == 0) {

			;
		}
		else if(memcmp(buf + 4, c7m2_id_3, 4) == 0) {

			;
		}
		else {

			SC0710_StopC5FlashAccess(pDevice);

			FREE_MEMORY(pFileBuffer);

			FREE_MEMORY(pWorkBuffer);

			if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

				(pDevice->iManufacturer == 0x1A) ||

				(pDevice->iManufacturer == 0x1B) ||

				(pDevice->iManufacturer == 0x1C) ||

				(pDevice->iManufacturer == 0x1D) ||

				(pDevice->iManufacturer == 0x1C) ||

				(pDevice->iManufacturer == 0x1C) ||

				(pDevice->iManufacturer == 0x1D) ||

				(pDevice->iManufacturer == 0x1E) ||

				(pDevice->iManufacturer == 0x1F)) {

				;
			}
			else {

				ULONG R000000EC = 0;

				R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

				R000000EC |= 0x01;

				SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);
			}
			return FALSE;
		}

		DELAY_100NS( 100000 );

		LINUXV4L2_PRINT(KERN_INFO, "START TO REFRESH");

		do {

			count++;

			set_chip_sel(pDevice, 0);
			buf[0] = CMD_C5_REFRESH;
			spi_tx(pDevice, 4, buf);
			set_chip_sel(pDevice, 1);

			DELAY_100NS( 10000 );

			set_chip_sel(pDevice, 0);
			buf[0] = CMD_C5_WRITE_EN;
			spi_tx(pDevice, 4, buf);
			set_chip_sel(pDevice, 1);

			DELAY_100NS( 10000 );

			set_chip_sel(pDevice, 0);
			buf[0] = CMD_C5_CLEAR;
			spi_tx(pDevice, 4, buf);
			set_chip_sel(pDevice, 1);

			DELAY_100NS( 10000 );

			set_chip_sel(pDevice, 0);
			buf[0] = CMD_C5_WRITE_INC;
			spi_tx(pDevice, 4, buf);
			spi_tx(pDevice, nFileSize, pWorkBuffer);
			set_chip_sel(pDevice, 1);

			DELAY_100NS( 10000 );

			RtlFillMemory(buf, 4, 0xFF);

			set_chip_sel(pDevice, 0);
			buf[0] = CMD_C5_READ_STATUS;
			spi_tx(pDevice, 4, buf);
			spi_rx(pDevice, 4, buf + 4);
			set_chip_sel(pDevice, 1);


			LINUXV4L2_PRINT(KERN_INFO, "CMD_READ_STATUS(%x, %x, %x, %x)", *(buf + 4), *(buf + 5), *(buf + 6), *(buf + 7));

			if( count > 10 ) break;

			DELAY_100NS( 100000 );

		} while( !(*(buf + 4) == 0x00 && *(buf + 5) == 0x81 && *(buf + 6) == 0x60 && *(buf + 7) == 0x00) );

		set_chip_sel(pDevice, 0);
		buf[0] = CMD_C5_WRITE_DIS;
		spi_tx(pDevice, 4, buf);
		set_chip_sel(pDevice, 1);

		SC0710_StopC5FlashAccess(pDevice);

		if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

			(pDevice->iManufacturer == 0x1A) ||

			(pDevice->iManufacturer == 0x1B) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x1E) ||

			(pDevice->iManufacturer == 0x1F)) {

			;
		}
		else {

			ULONG R000000EC = 0;

			R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

			R000000EC |= 0x01;

			SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);
		}
	}
	returns = TRUE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
#else
	do_gettimeofday(&ts);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	stop_times = (ULONGLONG)(ts / 1000);
#else
	stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
#endif

	if(returns) {

		LINUXV4L2_PRINT(KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (SUCCESS)\n", (ULONG)(stop_times - start_times), nFileSize);
	}
	else {

		LINUXV4L2_PRINT(KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (FAIL)\n", (ULONG)(stop_times - start_times), nFileSize);
	}



	FREE_MEMORY(pFileBuffer);

	FREE_MEMORY(pWorkBuffer);

	release_firmware(fw);

	return TRUE;
}

BOOLEAN SC0710_DownloadC7Firmware_Online(CDevice* pDevice, BYTE* pwszFileName)
{
	BYTE* pFileBuffer = NULL;

	BYTE* pWorkBuffer = NULL;

	const struct firmware* fw = NULL;

	int ret = 1;

	ULONG             nFileSize = 0;

	BOOLEAN			  returns = FALSE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	u64 ts = 0;
#else
	struct timeval ts;
#endif

	ULONGLONG start_times = 0;

	ULONGLONG stop_times = 0;





	unsigned char buf[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	if(p_sys_cfg->n_mcu_version == 0) { return FALSE; }

	if(pDevice->m_pKsVideo[0])
	{
		ret = request_firmware(&fw, pwszFileName, &(pDevice->m_pKsVideo[0]->dev));

		LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadC7Firmware_Online() fw(0p%p)\n", pDevice->m_nKsDeviceNumber, fw);
	}

	if(ret) {

		LINUXV4L2_PRINT(KERN_ERR, "[%02d] SC0710_DownloadC7Firmware_Online() Upload failed. (file %s not found?)\n", pDevice->m_nKsDeviceNumber, pwszFileName);

		return FALSE;

	}
	else {

		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_DownloadC7Firmware_Online() %s\n", pDevice->m_nKsDeviceNumber, pwszFileName);

		if(fw)
		{
			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_DownloadC7Firmware_Online() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

			if(fw->size > 0)
			{
				nFileSize = fw->size;

				pFileBuffer = vmalloc(((nFileSize + 3) / 4) * 4);

				pWorkBuffer = vmalloc(((nFileSize + 3) / 4) * 4);

				if(pFileBuffer && pWorkBuffer)
				{
					memset(pFileBuffer, 0x00, ((nFileSize + 3) / 4) * 4);

					memcpy(pFileBuffer, fw->data, fw->size);
				}
				else
				{
					LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadC7Firmware_Online() no buffer\n", pDevice->m_nKsDeviceNumber);

					release_firmware(fw);

					FREE_MEMORY(pFileBuffer);

					FREE_MEMORY(pWorkBuffer);

					return FALSE;
				}
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_DownloadC7Firmware_Online() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

				return FALSE;
			}
		}
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
#else
	do_gettimeofday(&ts);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	start_times = (ULONGLONG)(ts / 1000);
#else
	start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
#endif

	if(pFileBuffer[0] == 0x00 &&

		pFileBuffer[1] == 0x11 &&

		pFileBuffer[2] == 0x22 &&

		pFileBuffer[3] == 0x33 &&

		pFileBuffer[4] == 0x44 &&

		pFileBuffer[5] == 0x55 &&

		pFileBuffer[6] == 0x66 &&

		pFileBuffer[7] == 0x77 &&

		pFileBuffer[8] == 0x88 &&

		pFileBuffer[9] == 0x99 &&

		pFileBuffer[10] == 0xAA &&

		pFileBuffer[11] == 0xBB &&

		pFileBuffer[12] == 0xCC &&

		pFileBuffer[13] == 0xDD &&

		pFileBuffer[14] == 0xEE &&

		pFileBuffer[15] == 0xFF) {

			{
				ULONG i = 0;

				BYTE* p = pWorkBuffer;

				ULONG n_real_size = nFileSize - 16;

				ULONG n_real_size_2 = n_real_size >> 1;

				RtlCopyMemory(pWorkBuffer, pFileBuffer + 16 + n_real_size_2 + (n_real_size % 2), n_real_size_2);

				RtlCopyMemory(pWorkBuffer + n_real_size_2, pFileBuffer + 16, n_real_size_2 + (n_real_size % 2));

				for (i = 0; i < n_real_size_2; i++) {

					*p = *p ^ 0xA5;

					p++;
				}
				p = pWorkBuffer + n_real_size_2;

				for (i = 0; i < n_real_size_2; i++) {

					*p = *p ^ 0x5A;

					p++;
				}
			}
			nFileSize = nFileSize - 16;



			RtlFillMemory(buf, 4, 0xFF);

			if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

				(pDevice->iManufacturer == 0x1A) ||

				(pDevice->iManufacturer == 0x1B) ||

				(pDevice->iManufacturer == 0x1C) ||

				(pDevice->iManufacturer == 0x1D) ||

				(pDevice->iManufacturer == 0x1C) ||

				(pDevice->iManufacturer == 0x1C) ||

				(pDevice->iManufacturer == 0x1D) ||

				((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||

				((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D)) ||

				((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x17))) {

				;
			}
			else {

				ULONG R000000EC = 0;

				R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

				R000000EC &= ~0x01;

				SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);
			}

			SC0710_StartC5FlashAccess(pDevice);

			set_chip_sel(pDevice, 0);
			buf[0] = 0xE0;
			spi_tx(pDevice, 4, buf);
			spi_rx(pDevice, 4, buf + 4);
			set_chip_sel(pDevice, 1);

			if(memcmp(buf + 4, c7m2_id, 4) == 0) {

				;
			}
			else if(memcmp(buf + 4, c7m2_id_2, 4) == 0) {

				;
			}
			else {

				SC0710_StopC5FlashAccess(pDevice);

				FREE_MEMORY(pFileBuffer);

				FREE_MEMORY(pWorkBuffer);

				if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

					(pDevice->iManufacturer == 0x1A) ||

					(pDevice->iManufacturer == 0x1B) ||

					(pDevice->iManufacturer == 0x1C) ||

					(pDevice->iManufacturer == 0x1D) ||

					(pDevice->iManufacturer == 0x1C) ||

					(pDevice->iManufacturer == 0x1C) ||

					(pDevice->iManufacturer == 0x1D) ||

					((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||

					((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D)) ||

					((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x17))) {

					;
				}
				else {

					ULONG R000000EC = 0;

					R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

					R000000EC |= 0x01;

					SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);
				}

				return FALSE;
			}
			set_chip_sel(pDevice, 0);
			set_chip_sel(pDevice, 1);

			set_chip_sel(pDevice, 0);
			buf[0] = 0xC6;
			buf[1] = 0x00;
			spi_tx(pDevice, 3, buf);
			set_chip_sel(pDevice, 1);



			set_chip_sel(pDevice, 0);
			buf[0] = 0x3C;
			buf[1] = 0x00;
			spi_tx(pDevice, 4, buf);
			spi_rx(pDevice, 4, buf + 4);
			set_chip_sel(pDevice, 1);

			set_chip_sel(pDevice, 0);
			buf[0] = 0x0E;
			buf[1] = 0x01;
			spi_tx(pDevice, 4, buf);
			set_chip_sel(pDevice, 1);

			DELAY_100NS(50000);



			set_chip_sel(pDevice, 0);
			buf[0] = 0x3C;
			buf[1] = 0X00;
			spi_tx(pDevice, 4, buf);
			spi_rx(pDevice, 4, buf + 4);


			LINUXV4L2_PRINT(KERN_INFO, "CMD_READ_STATUS(%x, %x, %x, %x)", *(buf + 4), *(buf + 5), *(buf + 6), *(buf + 7));

			set_chip_sel(pDevice, 1);

			set_chip_sel(pDevice, 0);
			buf[0] = 0x46;
			buf[1] = 0x00;
			spi_tx(pDevice, 4, buf);
			set_chip_sel(pDevice, 1);



			set_chip_sel(pDevice, 0);
			buf[0] = 0x7A;
			buf[1] = 0x00;
			spi_tx(pDevice, 4, buf);

			{	int i, send_size;

			i = 0;

			while (nFileSize) {

				if(nFileSize >= 4096) {

					send_size = 4096;
				}
				else {

					send_size = nFileSize;
				}
				spi_tx(pDevice, send_size, pWorkBuffer + i);

				i += send_size;

				nFileSize -= send_size;
			}
			}
			set_chip_sel(pDevice, 1);



			set_chip_sel(pDevice, 0);
			buf[0] = 0x26;
			buf[1] = 0x00;
			spi_tx(pDevice, 4, buf);
			set_chip_sel(pDevice, 1);


			RtlFillMemory(buf, 4, 0xFF);

			set_chip_sel(pDevice, 0);
			spi_tx(pDevice, 4, buf);
			set_chip_sel(pDevice, 1);



			set_chip_sel(pDevice, 0);
			buf[0] = 0x3C;
			buf[1] = 0x00;
			spi_tx(pDevice, 4, buf);
			spi_rx(pDevice, 4, buf + 4);
			set_chip_sel(pDevice, 1);

			SC0710_StopC5FlashAccess(pDevice);

			if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

				(pDevice->iManufacturer == 0x1A) ||

				(pDevice->iManufacturer == 0x1B) ||

				(pDevice->iManufacturer == 0x1C) ||

				(pDevice->iManufacturer == 0x1D) ||

				(pDevice->iManufacturer == 0x1C) ||

				(pDevice->iManufacturer == 0x1C) ||

				(pDevice->iManufacturer == 0x1D) ||

				((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||

				((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D)) ||

				((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x17))) {

				;
			}
			else {

				ULONG R000000EC = 0;

				R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

				R000000EC |= 0x01;

				SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);
			}

	}
	returns = TRUE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
#else
	do_gettimeofday(&ts);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	stop_times = (ULONGLONG)(ts / 1000);
#else
	stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
#endif

	if(returns) {

		LINUXV4L2_PRINT(KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (SUCCESS)\n", (ULONG)(stop_times - start_times), nFileSize);
	}
	else {

		LINUXV4L2_PRINT(KERN_INFO, "TOTAL DOWNLOAD TIMES = %08d (%d BYTES) (FAIL)\n", (ULONG)(stop_times - start_times), nFileSize);
	}



	FREE_MEMORY(pFileBuffer);

	FREE_MEMORY(pWorkBuffer);

	release_firmware(fw);

	return TRUE;
}

extern ULONG SC0710_DEF_MULTI_CHDMA_PAGES_SIZE;

extern ULONG SC0710_DEF_MULTI_CHDMA_PAGES;

extern ULONG SC0710_DEF_MULTI_CHDMA_PAGES_SIZE_4M_RESIDUE;

NTSTATUS SC0710_HwStartVideoDMA(CDevice* pDevice, ULONG n_native_vdo444, BOOL is_10bits, CVideo* pVideo)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	ULONG cx = p_sys_cfg->n_input_video_resolution_cx;

	ULONG cy = p_sys_cfg->n_input_video_resolution_cy;

	ULONG interleaved = p_sys_cfg->n_input_video_resolution_interleaved;

	ULONG fps = p_sys_cfg->n_input_video_resolution_fps;

	ULONG m = p_sys_cfg->n_input_video_resolution_fps_m;

	if(interleaved) {

		cy <<= 1;


		fps >>= 1;
	}
	if(cx % 4) {

		cx /= 4;

		cx = 4 * (cx + 1);
	}
	if(cx == 0 || cy == 0 || fps == 0) {

		cx = p_sys_cfg->n_output_video_resolution_cx;

		cy = p_sys_cfg->n_output_video_resolution_cy;

		interleaved = p_sys_cfg->n_output_video_resolution_interleaved;

		fps = p_sys_cfg->n_output_video_resolution_fps;

		m = p_sys_cfg->n_output_video_resolution_fps_m;
	}
	if(cx == 0 || cy == 0 || fps == 0) {

		cx = 1920; cy = 1080; interleaved = 0; fps = 60; m = 0;
	}
	if(pDevice->m_nCustomCompanyMediaedgeProperty)
	{
		cx *= 2;

		cy *= 2;

		fps /= 4;
	}


	{



		BOOL  is_low_bandwidth = FALSE;

		ULONG n_10bits_mode = 0;

		ULONG pcm_buf_size_v = 0;

		ULONG n_vbi_lines = 0;

		ULONG n_header_payload = 0;

		ULONG rings_v = 4;

		dma_addr_t s_desc_phy_addr_v = 0;

		dma_addr_t s_resu_phy_addr_v = 0;


		if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420 || pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YVU420 || pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12 || pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_M420)
		{
			is_low_bandwidth = TRUE;
		}

		if(p_sys_cfg->n_pcie_generation <= 2 && p_sys_cfg->n_pcie_link_width <= 2) {

			if((cx * cy * fps) >= (1920 * 1200 * 60 * 2)) {

				is_low_bandwidth = TRUE;

				is_10bits = TRUE;
			}
		}
		if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x14)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x15)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x17)) ||
			((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x17)) ||
			((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x17)) ||
			((pDevice->bcdDevice == 0x0104) && (pDevice->iManufacturer == 0x17)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x16)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18)) ||
			((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x18)) ||
			((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x18)) ||
			((pDevice->bcdDevice == 0x0402) && (pDevice->iManufacturer == 0x19)) ||
			((pDevice->bcdDevice == 0x0502) && (pDevice->iManufacturer == 0x19))) {

			if((cx * cy * fps) > (4096 * 2160 * 30))
			{
				is_low_bandwidth = TRUE;

				is_10bits = TRUE;
			}


			if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV)
			{
				is_low_bandwidth = FALSE;

				is_10bits = FALSE;
			}
		}
		if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x000E) ||
			(pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x000F) ||
			(pDevice->iProduct == 0x00 && pDevice->iManufacturer == 0x15 && pDevice->bcdDevice == 0x000B) ||
			(pDevice->iProduct == 0x10 && pDevice->iManufacturer == 0xFC && pDevice->bcdDevice == 0xD07C) ||
			(pDevice->iProduct == 0x10 && pDevice->iManufacturer == 0xFC && pDevice->bcdDevice == 0xD07C)) {

			if(n_native_vdo444 == 0x00000001) {

				n_native_vdo444 = 0x00000002;
			}
		}
		if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

			if((pDevice->iManufacturer == 0x1A) ||

				(pDevice->iManufacturer == 0x1B)) {

				n_native_vdo444 = 0x00000000;
			}
			else {

				;
			}
			if(((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0xA201) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
				((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F)) ||
				((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16))) {

				;
			}
			else {

				is_10bits = FALSE;
			}
		}
		if(((pDevice->bcdDevice == 0xD004) && (pDevice->iManufacturer == 0x1C)) ||
			((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1C)) ||
			((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ||
			((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
			((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
			((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

			is_low_bandwidth = TRUE;

			is_10bits = TRUE;
		}
		if(pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty >= 3) {

			is_low_bandwidth = TRUE;

			is_10bits = TRUE;
		}
		if(p_sys_cfg->n_fpga_version <= 250618 && p_sys_cfg->n_fpga_version != 0) {

			n_native_vdo444 = 0x00000000;
		}
		if(p_sys_cfg->n_mcu_version <= 190725) {

			n_native_vdo444 = 0x00000000;
		}
		if(is_10bits) {

			n_native_vdo444 = 0x00000000;

			if(((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0xA201) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
				((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F)) ||
				((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16) &&(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty) )) {

				if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010)
				{
					n_10bits_mode = 5;
				}
			}
			else if(is_low_bandwidth) {

				{
					n_10bits_mode = 3;
				}
			}


			else if((cx * cy * fps) > (4096 * 2160 * 30)) {

#ifdef SC0710_DEF_10BITS_METHOD


				{
					n_10bits_mode = 2;
				}
#else
				n_10bits_mode = 1;
#endif
			}
			else {

				n_10bits_mode = 1;
			}
		}

		if ((pDevice->idVendor == 0x12AB) && ((pDevice->idProduct & 0xFFFF) == 0x0750))
		{
			if (is_10bits) {

				if ((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P210) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P216))
				{
					n_10bits_mode = 6;
				}
				else if ((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P016))
				{
					n_10bits_mode = 1;
				}
				else
				{

				}
			}
			else
			{
				if ((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY))
				{
					n_10bits_mode = 0;
				}

				if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12)
				{
					n_10bits_mode = 4;
				}
			}
		}

		if(pVideo->m_nCustomAnalogVideoVbiLinesEnableProperty > 0 &&

			pVideo->m_nCustomAnalogVideoVbiLinesProperty > 0) {

			n_vbi_lines = pVideo->m_nCustomAnalogVideoVbiLinesProperty;
		}

		LINUXV4L2_PRINT(KERN_INFO, "[%02d] n_vbi_lines(0x%x)\n", pDevice->m_nKsDeviceNumber, n_vbi_lines);



		if(((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||
			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||
			((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18)) ||
			((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2B)) ||
			((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2D))) {

			unsigned char rxbuf[2] = { 0x00, 0x00 };

			unsigned char txbuf[2] = { 0x01, 0x00 };

			BOOL is_reg_updated = FALSE;

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 1, rxbuf, 1);

			usleep_range(5000, 6000);

			txbuf[1] = rxbuf[0];

			if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

				if((txbuf[1] & 0x80) == 0x00) { is_reg_updated = TRUE; }

				txbuf[1] |= 0x80;
			}
			else {

				if((txbuf[1] & 0x80) == 0x80) { is_reg_updated = TRUE; }

				txbuf[1] &= ~0x80;
			}

			if(is_reg_updated) {

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);

			}

			LINUXV4L2_DEBUG(KERN_INFO, "register 1 (0x%x) --", txbuf[1]);
		}
		if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x16)) ||

			((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x18)) ||
			((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x18))) {

			unsigned char rxbuf[2] = { 0x00, 0x00 };

			unsigned char txbuf[2] = { 0x01, 0x00 };

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 1, rxbuf, 1);

			DELAY_100NS(100000);

			txbuf[1] = rxbuf[0];

			txbuf[1] &= ~0x80;

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);
		}

		if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x13)) ||
			((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x13)) ||
			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||
			((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||
			((pDevice->bcdDevice == 0xB401) && (pDevice->iManufacturer == 0x16)) ||
			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||
			((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ||
			((pDevice->bcdDevice == 0xB201) && (pDevice->iManufacturer == 0x18)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18)) ||
			((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x18)) ||
			((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x18)) ) {

			if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4) {

				BOOL is_2si_mode = FALSE;

				BOOL is_reg_updated = FALSE;

				unsigned char txbuf[2] = { 0, 0 };

				unsigned char rxbuf[2] = { 0, 0 };

				BYTE n_device_address = 0;

				n_header_payload = 16;

				n_native_vdo444 = 0x00000000;

				if (p_sys_cfg->n_mcu_version >= 190521) {

					txbuf[0] = 0x01;
				}
				n_device_address = (p_sys_cfg->n_mcu_version >= 190903) ? SC0710_I2C_MCU_ADR7_0X31 : SC0710_I2C_MCU_ADR7_0X32;

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, n_device_address, txbuf, 1, rxbuf, 1);

				txbuf[1] = rxbuf[0];

				LINUXV4L2_PRINT(KERN_INFO, "[%02d] register 1(0x%x) read\n", pDevice->m_nKsDeviceNumber, rxbuf[0]);

				if (p_sys_cfg->n_input_video_signal_single_link_under_3G == 0 &&

					pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat == 1) {

					is_2si_mode = TRUE;
				}
				if (((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x13)) ||
					((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x13)) ||
					((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||
					((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x18)) ||
					((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x18))) {

					if (p_sys_cfg->n_input_video_signal_single_link_under_3G == 0) {

						is_2si_mode = TRUE;
					}
					else {

						is_2si_mode = FALSE;
					}
				}
				if (pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

					n_header_payload = 16;

					if ((txbuf[1] & 0x30) != 0x10) { is_reg_updated = TRUE; }

					txbuf[1] &= ~0x20;

					txbuf[1] |= 0x10;
				}
				else if (is_2si_mode) {

					n_header_payload = 0;

					if ((txbuf[1] & 0x30) != 0x20) { is_reg_updated = TRUE; }

					txbuf[1] |= 0x20;

					txbuf[1] &= ~0x10;
				}
				else {

					if (p_sys_cfg->n_input_video_signal_single_link_under_3G) {

						n_header_payload = 0;

						if ((txbuf[1] & 0x30) != 0x30) { is_reg_updated = TRUE; }

						txbuf[1] |= 0x20;
					}
					else {

						n_header_payload = 16;

						if ((txbuf[1] & 0x30) != 0x10) { is_reg_updated = TRUE; }

						txbuf[1] &= ~0x20;
					}
					txbuf[1] |= 0x10;
				}
				if (is_10bits) {


					if (((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) &&
						(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0)) {


						if ((txbuf[1] & 0x40) != 0x00) { is_reg_updated = TRUE; }

						txbuf[1] &= ~0x40;
					}
					else
					{
						if ((txbuf[1] & 0x40) != 0x40) { is_reg_updated = TRUE; }

						txbuf[1] |= 0x40;
					}
				}
				else {

					if ((txbuf[1] & 0x40) != 0x00) { is_reg_updated = TRUE; }

					txbuf[1] &= ~0x40;
				}
				txbuf[1] |= 0x0F;

				if (is_reg_updated) {

					LINUXV4L2_PRINT(KERN_INFO, "[%02d] register 1(0x%x) write\n", pDevice->m_nKsDeviceNumber, txbuf[1]);

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, n_device_address, txbuf, 2, NULL, 0);
				}

				rxbuf[1] = 0x00;

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, n_device_address, txbuf, 1, rxbuf, 1);

				LINUXV4L2_PRINT(KERN_INFO, "[%02d] register 1(0x%x) read back\n", pDevice->m_nKsDeviceNumber, rxbuf[0]);

				if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x13)) ||
					((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x13))) {


				}
				else
				{
					if (pDevice->m_nCustomDolbyProperty)
					{
						unsigned char tx[2] = { 3, 0 };

						unsigned char rx[2] = { 0, 0 };

						SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, n_device_address, tx, 1, rx, 1);

						tx[1] = rx[0];

						tx[1] |= 0x04;

						SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, n_device_address, tx, 2, NULL, 0);
					}
					else
					{
						unsigned char tx[2] = { 3, 0 };

						unsigned char rx[2] = { 0, 0 };

						SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, n_device_address, tx, 1, rx, 1);

						tx[1] = rx[0];

						tx[1] &= ~0x04;

						SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, n_device_address, tx, 2, NULL, 0);
					}
				}
			}
		}

		if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18))) {

			if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

				if(TRUE) {

					unsigned char txbuf[2] = { 0x3F, 0x0F };

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);
				}
				else {

					unsigned char txbuf[2] = { 0x3F, 0x00 };

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);
				}
			}
		}

		if(((pDevice->bcdDevice == 0x0B04) && (pDevice->iManufacturer == 0x1B))) {

			if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

				if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420 || pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YVU420 || pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12 || pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_M420)
				{
					unsigned char txbuf[2] = { 0x3F, 0x0F };

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);
				}
				else
				{

					unsigned char txbuf[2] = { 0x3F, 0x00 };

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);
				}
			}
		}

		if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

			(pDevice->iManufacturer == 0x1A) ||

			(pDevice->iManufacturer == 0x1B) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1C) ||

			(pDevice->iManufacturer == 0x1D) ||

			(pDevice->iManufacturer == 0x1E) ||

			(pDevice->iManufacturer == 0x1F)) {



			unsigned char txbuf[2] = { 0x01, 0x00 };

			unsigned char rxbuf[2] = { 0, 0 };

			n_header_payload = 16;

			n_native_vdo444 = 0x00000000;

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 1, rxbuf, 1);

			txbuf[1] = rxbuf[0];

			n_header_payload = 16;

			txbuf[1] &= ~0x20;

			txbuf[1] |= 0x10;

			if(pDevice->bcdDevice == 0x0002) {

				txbuf[1] |= 0x03;
			}
			else {

				txbuf[1] |= 0x0F;
			}
			if(is_10bits) {

				txbuf[1] |= 0x40;
			}
			else {

				txbuf[1] &= ~0x40;
			}
			txbuf[1] &= ~0x80;

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);

			rxbuf[1] = 0x00;

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 1, rxbuf, 1);
		}

		if(((pDevice->bcdDevice & 1) == 0x0001) && (pDevice->iManufacturer == 0x15))
		{
			if(p_sys_cfg->n_mcu_version >= 210726)
			{
				BOOL is_reg_updated = FALSE;

				unsigned char txbuf[2] = { 0x01, 0x00 };

				unsigned char rxbuf[2] = { 0, 0 };

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf, 1);

				txbuf[1] = rxbuf[0];

				if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY)
				{
					if((txbuf[1] & 0x8) == 0x00)
					{
						txbuf[1] |= 0x8;

						is_reg_updated = TRUE;
					}
				}
				else
				{
					if(txbuf[1] & 0x8)
					{
						txbuf[1] &= ~0x8;

						is_reg_updated = TRUE;
					}
				}

				if(is_reg_updated)
				{
					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);

					LINUXV4L2_PRINT(KERN_INFO, "[%02d] swap UY (0x%x)\n", pDevice->m_nKsDeviceNumber, txbuf[1]);
				}
			}
		}


		if(n_10bits_mode == 6) {

			pcm_buf_size_v = (cx * cy * 2 * 2);
		}
		else if (n_10bits_mode == 5) {

			pcm_buf_size_v = (cx * cy * 5) / 2;
		}
		else if((n_10bits_mode == 3) ||(n_10bits_mode == 4)) {

			if(n_header_payload == 0) {

				pcm_buf_size_v = (cx * cy * 3) / 2;
			}
			else {

				pcm_buf_size_v = (((cx / 2) + n_header_payload) * (cy) * 3);
			}
		}
		else if(n_10bits_mode == 2) {

			pcm_buf_size_v = (cx * cy * 15) / 8;
		}
		else if(n_10bits_mode == 1) {

			if ((pDevice->idVendor == 0x12AB) && ((pDevice->idProduct & 0xFFFF) == 0x0750))
			{
				pcm_buf_size_v = (cx * cy * 3) / 1;
			}
			else
			{
				if(pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat == 0)
				{
					n_header_payload = 32;

					pcm_buf_size_v = (cx + n_header_payload) * cy * 3;
				}
				else
				{
					pcm_buf_size_v = (cx * cy * 3) / 1;
				}
			}
		}
		else {

			pcm_buf_size_v = (n_native_vdo444 > 0) ? (cx * cy * 3) : ((cx + n_header_payload) * (cy + n_vbi_lines) * 2);
		}
		if(interleaved) {

			pcm_buf_size_v /= 2;
		}

		if((pDevice->iManufacturer == 0x1A) ||
			(pDevice->iManufacturer == 0x1B) ||
			(pDevice->iManufacturer == 0x16) ||
			(pDevice->iManufacturer == 0x18) ||
			(pDevice->iManufacturer == 0x2B)) {

			if(n_vbi_lines > 0) {

				if((pDevice->iManufacturer == 0x1A) ||
					(pDevice->iManufacturer == 0x1B)) {

					unsigned char txbuf[2] = { 0x3B, 0x00 };

					txbuf[0] = 0x3B;

					txbuf[1] = (p_sys_cfg->n_input_video_resolution_interleaveds[0]) ? (BYTE)(n_vbi_lines >> 1) : (BYTE)(n_vbi_lines >> 0);

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);

					txbuf[0] = 0x3C;

					txbuf[1] = (p_sys_cfg->n_input_video_resolution_interleaveds[1]) ? (BYTE)(n_vbi_lines >> 1) : (BYTE)(n_vbi_lines >> 0);

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);

					txbuf[0] = 0x3D;

					txbuf[1] = (p_sys_cfg->n_input_video_resolution_interleaveds[2]) ? (BYTE)(n_vbi_lines >> 1) : (BYTE)(n_vbi_lines >> 0);

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);

					txbuf[0] = 0x3E;

					txbuf[1] = (p_sys_cfg->n_input_video_resolution_interleaveds[3]) ? (BYTE)(n_vbi_lines >> 1) : (BYTE)(n_vbi_lines >> 0);

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);
				}
				else {

					unsigned char txbuf[2] = { 0x3B, 0x00 };

					txbuf[0] = 0x3B;

					txbuf[1] = (p_sys_cfg->n_input_video_resolution_interleaveds[0]) ? (BYTE)(n_vbi_lines >> 2) : (BYTE)(n_vbi_lines >> 1);

					if(pVideo->m_nCustomAnalogVideoVbiMixModeProperty == 1)
					{
						txbuf[1] |= 0x80;
					}
					else
					{
						txbuf[1] &= ~0x80;
					}

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);

					txbuf[0] = 0x3C;

					txbuf[1] = (p_sys_cfg->n_input_video_resolution_interleaveds[1]) ? (BYTE)(n_vbi_lines >> 2) : (BYTE)(n_vbi_lines >> 1);

					if(pVideo->m_nCustomAnalogVideoVbiMixModeProperty == 1)
					{
						txbuf[1] |= 0x80;
					}
					else
					{
						txbuf[1] &= ~0x80;
					}

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);

					txbuf[0] = 0x3D;

					txbuf[1] = (p_sys_cfg->n_input_video_resolution_interleaveds[2]) ? (BYTE)(n_vbi_lines >> 2) : (BYTE)(n_vbi_lines >> 1);

					if(pVideo->m_nCustomAnalogVideoVbiMixModeProperty == 1)
					{
						txbuf[1] |= 0x80;
					}
					else
					{
						txbuf[1] &= ~0x80;
					}

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);

					txbuf[0] = 0x3E;

					txbuf[1] = (p_sys_cfg->n_input_video_resolution_interleaveds[3]) ? (BYTE)(n_vbi_lines >> 2) : (BYTE)(n_vbi_lines >> 1);

					if(pVideo->m_nCustomAnalogVideoVbiMixModeProperty == 1)
					{
						txbuf[1] |= 0x80;
					}
					else
					{
						txbuf[1] &= ~0x80;
					}

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);
				}
			}
			else {

				unsigned char txbuf[2] = { 0x3B, 0x00 };

				txbuf[0] = 0x3B;

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);

				txbuf[0] = 0x3C;

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);

				txbuf[0] = 0x3D;

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);

				txbuf[0] = 0x3E;

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);
			}
		}




		LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwStartVideoDMA( cx = %d, cy = %d, interleaved = %d, fps = %d,  m = %d, pcie = %d.%d, mode = %d, 444 = %d )",

			pDevice->m_nKsDeviceNumber, cx, cy, interleaved, fps, m, p_sys_cfg->n_pcie_generation, p_sys_cfg->n_pcie_link_width, n_10bits_mode, n_native_vdo444);

		{
			s_desc_phy_addr_v = pDevice->m_pDmaBaseCommonPhysicalBuffer[0];

			s_resu_phy_addr_v = pDevice->m_pDmaBaseCommonPhysicalBuffer[0] + 0x100000;

			RtlZeroMemory((BYTE*)pDevice->m_pDmaBaseCommonBuffer[0], 0x200000);

			LINUXV4L2_DEBUG(KERN_INFO, "s_desc_phy_addr_v [llx%llx]", s_desc_phy_addr_v);

			LINUXV4L2_PRINT(KERN_INFO, "pcm_buf_size_v (0d%d)", pcm_buf_size_v);



			{
				ULONG i = 0;
				ULONG j = 0;

				dma_addr_t s_desc_phy_addr_begin = pDevice->m_pDmaBaseCommonPhysicalBuffer[0];

				dma_addr_t s_desc_phy_addr_next = pDevice->m_pDmaBaseCommonPhysicalBuffer[0];

				ULONG dma_addr_index = 1;

				XDMA_DESCRIPTOR* p_desc = (XDMA_DESCRIPTOR*)(pDevice->m_pDmaBaseCommonBuffer[0]);

				s_desc_phy_addr_next += 32;

				if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

					if(((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

						p_sys_cfg->n_is_multi_channels_enabled = 16;
					}
					else if(((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||
						((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D)) ||
						((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
						((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D))) {

						p_sys_cfg->n_is_multi_channels_enabled = 8;
					}
					else if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x12))) {

						p_sys_cfg->n_is_multi_channels_enabled = 2;
					}
					else {

						p_sys_cfg->n_is_multi_channels_enabled = 4;
					}

					rings_v = SC0710_DEF_MULTI_CHDMA_PAGES * SC0710_DEF_MULTI_CHDMA_ROW;

					pcm_buf_size_v = SC0710_DEF_MULTI_CHDMA_PAGES_SIZE;

					for (i = 1; i < (rings_v + 1); i++)
					{
						if(i == rings_v) {

							p_desc->control = XDMA_DESC_MAGIC;

							p_desc->control |= (XDMA_DESC_COMPLETED_BIT);

							p_desc->numBytes = pcm_buf_size_v;

							p_desc->nextHi = (UINT32)((s_desc_phy_addr_begin >> 32) & 0xFFFFFFFF);
							p_desc->nextLo = (UINT32)(s_desc_phy_addr_begin & 0xFFFFFFFF);



							p_desc->srcAddrHi = (UINT32)((s_resu_phy_addr_v >> 32) & 0xFFFFFFFF);
							p_desc->srcAddrLo = (UINT32)(s_resu_phy_addr_v & 0xFFFFFFFF);



							p_desc->dstAddrHi = 0;


							p_desc->dstAddrLo = (UINT32)((pDevice->m_pDmaBaseCommonPhysicalBuffer[rings_v / SC0710_DEF_MULTI_CHDMA_PAGES] + (SC0710_DEF_MULTI_CHDMA_PAGES - 1) * pcm_buf_size_v) & 0xFFFFFFFF) + SC0710_DEF_MULTI_CHDMA_PAGES_SIZE_4M_RESIDUE;






							LINUXV4L2_DEBUG(KERN_INFO, "[%02d] [%08X.%08X.%08X%08X.%08X%08X.%08X%08X]-", i, p_desc->control, p_desc->numBytes,

								p_desc->srcAddrHi, p_desc->srcAddrLo,

								p_desc->dstAddrHi, p_desc->dstAddrLo,

								p_desc->nextHi, p_desc->nextLo);


						}
						else {

							p_desc->control = XDMA_DESC_MAGIC;

							p_desc->control |= (XDMA_DESC_COMPLETED_BIT);

							p_desc->numBytes = pcm_buf_size_v;

							p_desc->srcAddrHi = (UINT32)((s_resu_phy_addr_v >> 32) & 0xFFFFFFFF);
							p_desc->srcAddrLo = (UINT32)(s_resu_phy_addr_v & 0xFFFFFFFF);

							s_resu_phy_addr_v += 32;

							p_desc->dstAddrHi = 0;
							p_desc->dstAddrLo = (UINT32)((pDevice->m_pDmaBaseCommonPhysicalBuffer[((i - 1) / SC0710_DEF_MULTI_CHDMA_PAGES) + 1] + ((i - 1) % SC0710_DEF_MULTI_CHDMA_PAGES) * pcm_buf_size_v) & 0xFFFFFFFF) + SC0710_DEF_MULTI_CHDMA_PAGES_SIZE_4M_RESIDUE;


							p_desc->nextHi = (UINT32)((s_desc_phy_addr_next >> 32) & 0xFFFFFFFF);
							p_desc->nextLo = (UINT32)(s_desc_phy_addr_next & 0xFFFFFFFF);

							s_desc_phy_addr_next += 32;


							p_desc++;
						}
					}
				}
				else {

					p_sys_cfg->n_is_multi_channels_enabled = 0;

					RtlZeroMemory((BYTE*)pDevice->m_nDescriptor_Elements, sizeof(ULONG) * MAX_SUB_DEVICE_NUM);

					for (i = 0; i < rings_v; i++) {


						ULONG quantity_4M_block = (pcm_buf_size_v + 0x3FFFFF) / 0x400000;

						if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x10)) ||
							((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x11))) {

							if(pDevice->m_nAnalogCrossbarVideoInputProperty == 2)
							{
								if(pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat == 0)
								{
									quantity_4M_block = ((cx / 2 * cy * 2 + 0x3FFFFF) / 0x400000) * 2;
								}
							}
						}




						if(n_10bits_mode == 4)
						{

							ULONG M420_DMA_length = 0;

							ULONG line = 0;

							ULONG length = 0;

							ULONG y_in_4M_Block = 0;
							ULONG c_in_4M_Block = 0;

							ULONG residue_y = 0;
							ULONG residue_c = 0;

							ULONG dma_addr_index_y = 1;
							ULONG dma_addr_index_c = 1;

							ULONG pitch = cx;


















							ULONG quantity_4M_Y_block = ((cx * cy) + 0x3FFFFF) / 0x400000;
							ULONG quantity_4M_C_block = ((cx * cy / 2) + 0x3FFFFF) / 0x400000;

							dma_addr_t dma_phy_addr_y = pDevice->m_pDmaBaseCommonPhysicalBuffer[i * (quantity_4M_Y_block + quantity_4M_C_block) + 1];
							dma_addr_t dma_phy_addr_uv = pDevice->m_pDmaBaseCommonPhysicalBuffer[i * (quantity_4M_Y_block + quantity_4M_C_block) + quantity_4M_Y_block + 1];

							dma_addr_t dma_phy_addr = 0;



							if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P016)
							{
								pitch = cx * 2;
							}
							else
							{
								pitch = cx;
							}

							if(i == (rings_v - 1)) {

								while (line < ((cy * 3) >> 1))
								{
									p_desc->control = XDMA_DESC_MAGIC;

									if(((line % 3) == 0) || ((line % 3) == 1))
									{
										length = 2 * pitch;

										dma_phy_addr = dma_phy_addr_y;

										if(residue_y)
										{
											length = residue_y;

											residue_y = 0;
										}
										else if((y_in_4M_Block + length) >= 0x400000)
										{

											length = 0x400000 - y_in_4M_Block;

											residue_y = 2 * pitch - length;
										}
										else
										{

										}

										y_in_4M_Block += length;
									}
									else
									{
										length = pitch;

										dma_phy_addr = dma_phy_addr_uv;

										if(residue_c)
										{
											length = residue_c;

											residue_c = 0;
										}
										else if((c_in_4M_Block + length) >= 0x400000)
										{

											length = 0x400000 - c_in_4M_Block;

											residue_c = pitch - length;
										}
										else
										{

										}

										c_in_4M_Block += length;

										if(line == (((cy * 3) >> 1) - 1))
										{
											p_desc->control |= (XDMA_DESC_COMPLETED_BIT);
										}

									}

									p_desc->numBytes = length;

									M420_DMA_length += p_desc->numBytes;

									pDevice->m_nDescriptor_Elements[i]++;

									p_desc->srcAddrHi = (UINT32)((s_resu_phy_addr_v >> 32) & 0xFFFFFFFF);
									p_desc->srcAddrLo = (UINT32)(s_resu_phy_addr_v & 0xFFFFFFFF);

									s_resu_phy_addr_v += 32;

									p_desc->dstAddrHi = (UINT32)((dma_phy_addr >> 32) & 0xFFFFFFFF);
									p_desc->dstAddrLo = (UINT32)(dma_phy_addr & 0xFFFFFFFF);





									if(line == (((cy * 3) >> 1) - 1))
									{
										p_desc->nextHi = (UINT32)((pDevice->m_pDmaBaseCommonPhysicalBuffer[0] >> 32) & 0xFFFFFFFF);
										p_desc->nextLo = (UINT32)(pDevice->m_pDmaBaseCommonPhysicalBuffer[0] & 0xFFFFFFFF);
									}
									else
									{
										p_desc->nextHi = (UINT32)((s_desc_phy_addr_next >> 32) & 0xFFFFFFFF);
										p_desc->nextLo = (UINT32)(s_desc_phy_addr_next & 0xFFFFFFFF);
									}

									s_desc_phy_addr_next += 32;


									p_desc++;

									if(residue_y)
									{
										if(y_in_4M_Block == 0x400000)
										{
											dma_phy_addr_y = pDevice->m_pDmaBaseCommonPhysicalBuffer[i * (quantity_4M_Y_block + quantity_4M_C_block) + 1 + dma_addr_index_y++];

											y_in_4M_Block = 0;
										}
										else
										{
											LINUXV4L2_DEBUG(KERN_INFO, "[%02d] y_in_4M_Block(0x%x) NOT 0x400000, ERROR\n", pDevice->m_nKsDeviceNumber, y_in_4M_Block);
										}
									}
									else if(residue_c)
									{
										if(c_in_4M_Block == 0x400000)
										{
											dma_phy_addr_uv = pDevice->m_pDmaBaseCommonPhysicalBuffer[i * (quantity_4M_Y_block + quantity_4M_C_block) + quantity_4M_Y_block + 1 + dma_addr_index_c++];

											c_in_4M_Block = 0;
										}
										else
										{
											LINUXV4L2_DEBUG(KERN_INFO, "[%02d] c_in_4M_Block(0x%x) NOT 0x400000, ERROR\n", pDevice->m_nKsDeviceNumber, c_in_4M_Block);
										}
									}
									else
									{
										if(((line % 3) == 0) || ((line % 3) == 1))
										{
											line += 2;

											if(y_in_4M_Block == 0x400000)
											{
												dma_phy_addr_y = pDevice->m_pDmaBaseCommonPhysicalBuffer[i * (quantity_4M_Y_block + quantity_4M_C_block) + 1 + dma_addr_index_y++];

												y_in_4M_Block = 0;
											}
											else
											{
												dma_phy_addr_y += length;
											}
										}
										else
										{
											line++;

											if(c_in_4M_Block == 0x400000)
											{
												dma_phy_addr_uv = pDevice->m_pDmaBaseCommonPhysicalBuffer[i * (quantity_4M_Y_block + quantity_4M_C_block) + quantity_4M_Y_block + 1 + dma_addr_index_c++];

												c_in_4M_Block = 0;
											}
											else
											{
												dma_phy_addr_uv += length;
											}
										}
									}
								}
							}
							else
							{
								while (line < ((cy * 3) >> 1))
								{
									p_desc->control = XDMA_DESC_MAGIC;

									if(((line % 3) == 0) || ((line % 3) == 1))
									{
										length = 2 * pitch;

										dma_phy_addr = dma_phy_addr_y;

										if(residue_y)
										{
											length = residue_y;

											residue_y = 0;
										}
										else if((y_in_4M_Block + length) >= 0x400000)
										{

											length = 0x400000 - y_in_4M_Block;

											residue_y = 2 * pitch - length;
										}
										else
										{

										}

										y_in_4M_Block += length;
									}
									else
									{
										length = pitch;

										dma_phy_addr = dma_phy_addr_uv;

										if(residue_c)
										{
											length = residue_c;

											residue_c = 0;
										}
										else if((c_in_4M_Block + length) >= 0x400000)
										{

											length = 0x400000 - c_in_4M_Block;

											residue_c = pitch - length;
										}
										else
										{

										}

										c_in_4M_Block += length;

										if(line == (((cy * 3) >> 1) - 1))
										{
											p_desc->control |= (XDMA_DESC_COMPLETED_BIT);
										}
									}

									p_desc->numBytes = length;

									M420_DMA_length += p_desc->numBytes;

									pDevice->m_nDescriptor_Elements[i]++;

									p_desc->srcAddrHi = (UINT32)((s_resu_phy_addr_v >> 32) & 0xFFFFFFFF);
									p_desc->srcAddrLo = (UINT32)(s_resu_phy_addr_v & 0xFFFFFFFF);

									s_resu_phy_addr_v += 32;

									p_desc->dstAddrHi = (UINT32)((dma_phy_addr >> 32) & 0xFFFFFFFF);
									p_desc->dstAddrLo = (UINT32)(dma_phy_addr & 0xFFFFFFFF);

									p_desc->nextHi = (UINT32)((s_desc_phy_addr_next >> 32) & 0xFFFFFFFF);
									p_desc->nextLo = (UINT32)(s_desc_phy_addr_next & 0xFFFFFFFF);

									s_desc_phy_addr_next += 32;

									p_desc++;


									if(residue_y)
									{
										if(y_in_4M_Block == 0x400000)
										{
											dma_phy_addr_y = pDevice->m_pDmaBaseCommonPhysicalBuffer[i * (quantity_4M_Y_block + quantity_4M_C_block) + 1 + dma_addr_index_y++];

											y_in_4M_Block = 0;
										}
										else
										{
											LINUXV4L2_DEBUG(KERN_INFO, "[%02d] y_in_4M_Block(0x%x) NOT 0x400000, ERROR\n", pDevice->m_nKsDeviceNumber, y_in_4M_Block);
										}
									}
									else if(residue_c)
									{
										if(c_in_4M_Block == 0x400000)
										{
											dma_phy_addr_uv = pDevice->m_pDmaBaseCommonPhysicalBuffer[i * (quantity_4M_Y_block + quantity_4M_C_block) + quantity_4M_Y_block + 1 + dma_addr_index_c++];

											c_in_4M_Block = 0;
										}
										else
										{
											LINUXV4L2_DEBUG(KERN_INFO, "[%02d] c_in_4M_Block(0x%x) NOT 0x400000, ERROR\n", pDevice->m_nKsDeviceNumber, c_in_4M_Block);
										}
									}
									else
									{
										if(((line % 3) == 0) || ((line % 3) == 1))
										{
											line += 2;

											if(y_in_4M_Block == 0x400000)
											{
												dma_phy_addr_y = pDevice->m_pDmaBaseCommonPhysicalBuffer[i * (quantity_4M_Y_block + quantity_4M_C_block) + 1 + dma_addr_index_y++];

												y_in_4M_Block = 0;
											}
											else
											{
												dma_phy_addr_y += length;
											}
										}
										else
										{
											line++;

											if(c_in_4M_Block == 0x400000)
											{
												dma_phy_addr_uv = pDevice->m_pDmaBaseCommonPhysicalBuffer[i * (quantity_4M_Y_block + quantity_4M_C_block) + quantity_4M_Y_block + 1 + dma_addr_index_c++];

												c_in_4M_Block = 0;
											}
											else
											{
												dma_phy_addr_uv += length;
											}
										}
									}
								}
							}
							if(M420_DMA_length == pcm_buf_size_v)
							{
								LINUXV4L2_PRINT(KERN_INFO, "[%02d] M420_DMA_length(%d) match pcm_buf_size_v(%d)", pDevice->m_nKsDeviceNumber, M420_DMA_length, pcm_buf_size_v);
							}
							else
							{
								LINUXV4L2_PRINT(KERN_INFO, "[%02d] M420_DMA_length(%d) NOT match pcm_buf_size_v(%d)", pDevice->m_nKsDeviceNumber, M420_DMA_length, pcm_buf_size_v);
							}
						}

						else
						{
							if(i == (rings_v - 1)) {

								for (j = 0; j < quantity_4M_block; j++)
								{
									p_desc->control = XDMA_DESC_MAGIC;

									if(j == quantity_4M_block - 1)
									{

										p_desc->control |= (XDMA_DESC_COMPLETED_BIT);

										p_desc->numBytes = pcm_buf_size_v - 0x400000 * j;

										if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_M420))
										{
										}
										else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P016)
										{
											LINUXV4L2_DEBUG(KERN_INFO, "[%02d] P016 TODO\n", pDevice->m_nKsDeviceNumber);
										}
										else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010)
										{
											if((cx == 3840) && (cy == 2160))
											{
												p_desc->numBytes = pcm_buf_size_v - 0x400000 * j + 3904 * 3;
											}
											else if((cx == 4096) && (cy == 2160))
											{
												p_desc->numBytes = pcm_buf_size_v - 0x400000 * j + 1024 * 3;
											}
											else
											{
											}
										}
										else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P210)
										{
											if((cx == 3840) && (cy == 2160))
											{
												p_desc->numBytes = pcm_buf_size_v - 0x400000 * j + 3904 * 3;
											}
											else if((cx == 4096) && (cy == 2160))
											{
												LINUXV4L2_DEBUG(KERN_INFO, "[%02d] P210 TODO\n", pDevice->m_nKsDeviceNumber);
											}
											else
											{
												LINUXV4L2_DEBUG(KERN_INFO, "[%02d] P210 TODO\n", pDevice->m_nKsDeviceNumber);
											}
										}
										else
										{
											if((cx == 3840) && (cy == 2160))
											{
												p_desc->numBytes = pcm_buf_size_v - 0x400000 * j + 2048;
											}
											else if((cx == 4096) && (cy == 2160))
											{
												p_desc->numBytes = pcm_buf_size_v - 0x400000 * j + 4096 * 3;
											}
											else
											{
											}
										}

										if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x10)) ||
											((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x11))) {

											if(pDevice->m_nAnalogCrossbarVideoInputProperty == 2)
											{
												if(pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat == 0)
												{
													p_desc->numBytes = (cx / 2 * cy * 2 - 0x400000) * (j / 2);
												}
											}
										}

										p_desc->nextHi = (UINT32)((s_desc_phy_addr_begin >> 32) & 0xFFFFFFFF);
										p_desc->nextLo = (UINT32)(s_desc_phy_addr_begin & 0xFFFFFFFF);


									}
									else
									{
										p_desc->numBytes = 0x400000;

										if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_M420))
										{
										}
										else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P016)
										{
											LINUXV4L2_DEBUG(KERN_INFO, "[%02d] P016 TODO\n", pDevice->m_nKsDeviceNumber);
										}
										else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010)
										{
											if((cx == 3840) && (cy == 2160))
											{
												p_desc->numBytes = 0x400000 - 3904;
											}
											else if((cx == 4096) && (cy == 2160))
											{
												p_desc->numBytes = 0x400000 - 1024;
											}
											else
											{
											}
										}
										else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P210)
										{
											if((cx == 3840) && (cy == 2160))
											{
												p_desc->numBytes = 0x400000 - 3904;
											}
											else if((cx == 4096) && (cy == 2160))
											{
												LINUXV4L2_DEBUG(KERN_INFO, "[%02d] P210 TODO\n", pDevice->m_nKsDeviceNumber);
											}
											else
											{
												LINUXV4L2_DEBUG(KERN_INFO, "[%02d] P210 TODO\n", pDevice->m_nKsDeviceNumber);
											}
										}
										else
										{
											if((cx == 3840) && (cy == 2160))
											{
												p_desc->numBytes = 0x400000 - 1024;
											}
											else if((cx == 4096) && (cy == 2160))
											{
												p_desc->numBytes = 0x400000 - 4096;
											}
											else
											{
											}
										}

										p_desc->nextHi = (UINT32)((s_desc_phy_addr_next >> 32) & 0xFFFFFFFF);
										p_desc->nextLo = (UINT32)(s_desc_phy_addr_next & 0xFFFFFFFF);

										s_desc_phy_addr_next += 32;
									}

									p_desc->srcAddrHi = (UINT32)((s_resu_phy_addr_v >> 32) & 0xFFFFFFFF);
									p_desc->srcAddrLo = (UINT32)(s_resu_phy_addr_v & 0xFFFFFFFF);

									s_resu_phy_addr_v += 32;

									p_desc->dstAddrHi = (UINT32)((pDevice->m_pDmaBaseCommonPhysicalBuffer[dma_addr_index % SC0710_MAX_DMA_BUFFER_QUANTITY_A_FRAME] >> 32) & 0xFFFFFFFF);
									p_desc->dstAddrLo = (UINT32)(pDevice->m_pDmaBaseCommonPhysicalBuffer[dma_addr_index % SC0710_MAX_DMA_BUFFER_QUANTITY_A_FRAME] & 0xFFFFFFFF);

									dma_addr_index++;






									LINUXV4L2_DEBUG(KERN_INFO, "[%02d] [%08X.%08X.%08X%08X.%08X%08X.%08X%08X]-", i, p_desc->control, p_desc->numBytes,

										p_desc->srcAddrHi, p_desc->srcAddrLo,

										p_desc->dstAddrHi, p_desc->dstAddrLo,

										p_desc->nextHi, p_desc->nextLo);

									p_desc++;

								}
							}
							else {

								for (j = 0; j < quantity_4M_block; j++)
								{
									p_desc->control = XDMA_DESC_MAGIC;

									if(j == quantity_4M_block - 1)
									{

										p_desc->control |= (XDMA_DESC_COMPLETED_BIT);

										p_desc->numBytes = pcm_buf_size_v - 0x400000 * j;

										if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_M420))
										{
										}
										else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P016)
										{
											LINUXV4L2_DEBUG(KERN_INFO, "[%02d] P016 TODO\n", pDevice->m_nKsDeviceNumber);
										}
										else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010)
										{
											if((cx == 3840) && (cy == 2160))
											{
												p_desc->numBytes = pcm_buf_size_v - 0x400000 * j + 3904 * 3;
											}
											else if((cx == 4096) && (cy == 2160))
											{
												p_desc->numBytes = pcm_buf_size_v - 0x400000 * j + 1024 * 3;
											}
											else
											{
											}
										}
										else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P210)
										{
											if((cx == 3840) && (cy == 2160))
											{
												p_desc->numBytes = pcm_buf_size_v - 0x400000 * j + 3904 * 3;
											}
											else if((cx == 4096) && (cy == 2160))
											{
												LINUXV4L2_DEBUG(KERN_INFO, "[%02d] P210 TODO\n", pDevice->m_nKsDeviceNumber);
											}
											else
											{
												LINUXV4L2_DEBUG(KERN_INFO, "[%02d] P210 TODO\n", pDevice->m_nKsDeviceNumber);
											}
										}
										else
										{
											if((cx == 3840) && (cy == 2160))
											{
												p_desc->numBytes = pcm_buf_size_v - 0x400000 * j + 2048;
											}
											else if((cx == 4096) && (cy == 2160))
											{
												p_desc->numBytes = pcm_buf_size_v - 0x400000 * j + 4096 * 3;
											}
											else
											{
											}
										}

										if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x10)) ||
											((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x11))) {

											if(pDevice->m_nAnalogCrossbarVideoInputProperty == 2)
											{
												if(pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat == 0)
												{
													p_desc->numBytes = (cx / 2 * cy * 2 - 0x400000) * (j / 2);
												}
											}
										}

									}
									else
									{
										p_desc->numBytes = 0x400000;

										if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_M420))
										{
										}
										else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P016)
										{
											LINUXV4L2_DEBUG(KERN_INFO, "[%02d] P016 TODO\n", pDevice->m_nKsDeviceNumber);
										}
										else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010)
										{
											if((cx == 3840) && (cy == 2160))
											{
												p_desc->numBytes = 0x400000 - 3904;
											}
											else if((cx == 4096) && (cy == 2160))
											{
												p_desc->numBytes = 0x400000 - 1024;
											}
											else
											{
											}
										}
										else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P210)
										{
											if((cx == 3840) && (cy == 2160))
											{
												p_desc->numBytes = 0x400000 - 3904;
											}
											else if((cx == 4096) && (cy == 2160))
											{
												LINUXV4L2_DEBUG(KERN_INFO, "[%02d] P210 TODO\n", pDevice->m_nKsDeviceNumber);
											}
											else
											{
												LINUXV4L2_DEBUG(KERN_INFO, "[%02d] P210 TODO\n", pDevice->m_nKsDeviceNumber);
											}
										}
										else
										{
											if((cx == 3840) && (cy == 2160))
											{
												p_desc->numBytes = 0x400000 - 1024;
											}
											else if((cx == 4096) && (cy == 2160))
											{
												p_desc->numBytes = 0x400000 - 4096;
											}
											else
											{
											}
										}
									}

									p_desc->srcAddrHi = (UINT32)((s_resu_phy_addr_v >> 32) & 0xFFFFFFFF);
									p_desc->srcAddrLo = (UINT32)(s_resu_phy_addr_v & 0xFFFFFFFF);

									s_resu_phy_addr_v += 32;

									p_desc->dstAddrHi = (UINT32)((pDevice->m_pDmaBaseCommonPhysicalBuffer[dma_addr_index % SC0710_MAX_DMA_BUFFER_QUANTITY_A_FRAME] >> 32) & 0xFFFFFFFF);
									p_desc->dstAddrLo = (UINT32)(pDevice->m_pDmaBaseCommonPhysicalBuffer[dma_addr_index % SC0710_MAX_DMA_BUFFER_QUANTITY_A_FRAME] & 0xFFFFFFFF);



									dma_addr_index++;

									p_desc->nextHi = (UINT32)((s_desc_phy_addr_next >> 32) & 0xFFFFFFFF);
									p_desc->nextLo = (UINT32)(s_desc_phy_addr_next & 0xFFFFFFFF);

									s_desc_phy_addr_next += 32;

									LINUXV4L2_DEBUG(KERN_INFO, "[%02d] [%08X.%08X.%08X%08X.%08X%08X.%08X%08X]", i, p_desc->control, p_desc->numBytes,

										p_desc->srcAddrHi, p_desc->srcAddrLo,

										p_desc->dstAddrHi, p_desc->dstAddrLo,

										p_desc->nextHi, p_desc->nextLo);

									p_desc++;
								}
							}
						}



					}
				}
			}
		}





		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[0][0]->firstDescHi), 0);
		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[0][0]->firstDescLo), 0);
		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[0][0]->firstDescAdj), 0);




		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[1][0]->firstDescHi), 0);
		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[1][0]->firstDescLo), 0);
		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[1][0]->firstDescAdj), 0);




		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[0][1]->firstDescHi), (UINT32)((pDevice->m_pDmaBaseCommonPhysicalBuffer[0] >> 32) & 0xFFFFFFFF));
		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[0][1]->firstDescLo), (UINT32)(pDevice->m_pDmaBaseCommonPhysicalBuffer[0] & 0xFFFFFFFF));
		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[0][1]->firstDescAdj), 0);

		if ((pDevice->idVendor == 0x12AB) && ((pDevice->idProduct & 0xFFFF) == 0x0750))
		{
			WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[2][1]->firstDescHi), (UINT32)((pDevice->m_pDmaBaseCommonPhysicalBuffer[2] >> 32) & 0xFFFFFFFF));
			WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[2][1]->firstDescLo), (UINT32)(pDevice->m_pDmaBaseCommonPhysicalBuffer[2] & 0xFFFFFFFF));
			WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[2][1]->firstDescAdj), 0);
		}



		if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x10)) ||
			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x11)) ||
			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x14)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x14)) ||
			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x15)) ||
			((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x15)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x15)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x17)) ||
			((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x17)) ||
			((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x17)) ||
			((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x19)) ||
			((pDevice->bcdDevice == 0x0402) && (pDevice->iManufacturer == 0x19)) ||
			((pDevice->bcdDevice == 0x0501) && (pDevice->iManufacturer == 0x19)) ||
			((pDevice->bcdDevice == 0x0502) && (pDevice->iManufacturer == 0x19))) {

			SC0710_EnableNative444(pDevice, n_native_vdo444);
		}
		if((pDevice->idVendor == 0x12AB) && ((pDevice->idProduct & 0xFFFF) == 0x0750))
		{
			if (p_sys_cfg->n_firmware_version) {

				LINUXV4L2_PRINT(KERN_INFO, "[%02d] n_10bits_mode(0x%x)\n", pDevice->m_nKsDeviceNumber, n_10bits_mode);

				if (n_10bits_mode == 1) {

					SC0710_SetRegister(pDevice, 0x00006240, 0x80000000);
					SC0710_SetRegister(pDevice, 0x00006244, 0);
					SC0710_SetRegister(pDevice, 0x00006200, 2);
					SC0710_SetRegister(pDevice, 0x000062C0, 0x80000000);
					SC0710_SetRegister(pDevice, 0x000062C4, 0);
					SC0710_SetRegister(pDevice, 0x00006280, 2);

					SC0710_SetRegister(pDevice, 0x000060C0, 4);
					SC0710_SetRegister(pDevice, 0x00006080, 2);
					SC0710_SetRegister(pDevice, 0x000061C0, 2);
					SC0710_SetRegister(pDevice, 0x00006180, 2);
				}
				else if (n_10bits_mode == 6) {

					SC0710_SetRegister(pDevice, 0x00006240, 0x80000000);
					SC0710_SetRegister(pDevice, 0x00006244, 0);
					SC0710_SetRegister(pDevice, 0x00006200, 2);
					SC0710_SetRegister(pDevice, 0x000062C0, 0);
					SC0710_SetRegister(pDevice, 0x000062C4, 0x80000000);
					SC0710_SetRegister(pDevice, 0x00006280, 2);

					SC0710_SetRegister(pDevice, 0x000060C0, 3);
					SC0710_SetRegister(pDevice, 0x00006080, 2);
				}
				else if(n_10bits_mode == 4) {

					SC0710_SetRegister(pDevice, 0x00006240, 0);
					SC0710_SetRegister(pDevice, 0x00006244, 0x80000000);
					SC0710_SetRegister(pDevice, 0x00006200, 2);

					SC0710_SetRegister(pDevice, 0x00006040, 0x80000000);
					SC0710_SetRegister(pDevice, 0x00006044, 0);
					SC0710_SetRegister(pDevice, 0x00006000, 2);

					SC0710_SetRegister(pDevice, 0x000060C0, 1);
					SC0710_SetRegister(pDevice, 0x00006080, 2);

					SC0710_SetRegister(pDevice, 0x00006140, 0);
					SC0710_SetRegister(pDevice, 0x00006144, 0x80000000);
					SC0710_SetRegister(pDevice, 0x00006100, 2);

					SC0710_SetRegister(pDevice, 0x000061C0, 0);
					SC0710_SetRegister(pDevice, 0x00006180, 2);
				}
				else {

					if (((p_sys_cfg->n_input_video_resolution_cx == 720) && (p_sys_cfg->n_input_video_resolution_cy == 240)) ||
						((p_sys_cfg->n_input_video_resolution_cx == 720) && (p_sys_cfg->n_input_video_resolution_cy == 288)) ||
						((p_sys_cfg->n_input_video_resolution_cx == 640) && (p_sys_cfg->n_input_video_resolution_cy == 240)) ||
						((p_sys_cfg->n_input_video_resolution_cx == 768) && (p_sys_cfg->n_input_video_resolution_cy == 288))) {

						SC0710_SetRegister(pDevice, 0x00006240, 0);
						SC0710_SetRegister(pDevice, 0x00006244, 0x80000000);
						SC0710_SetRegister(pDevice, 0x00006200, 2);
						SC0710_SetRegister(pDevice, 0x00006040, 0x80000000);
						SC0710_SetRegister(pDevice, 0x00006044, 0x80000000);
						SC0710_SetRegister(pDevice, 0x00006048, 0);

						SC0710_SetRegister(pDevice, 0x00006000, 2);
						SC0710_SetRegister(pDevice, 0x000060C0, 2);
						SC0710_SetRegister(pDevice, 0x00006080, 2);
					}
					else {

						SC0710_SetRegister(pDevice, 0x00006240, 0);
						SC0710_SetRegister(pDevice, 0x00006244, 0x80000000);
						SC0710_SetRegister(pDevice, 0x00006200, 2);
						SC0710_SetRegister(pDevice, 0x00006040, 0);
						SC0710_SetRegister(pDevice, 0x00006044, 0x80000000);

						SC0710_SetRegister(pDevice, 0x00006000, 2);
						SC0710_SetRegister(pDevice, 0x000060C0, 0);
						SC0710_SetRegister(pDevice, 0x00006080, 2);
					}
				}
			}
		}
		else
		{

			ULONG R000000D0 = SC0710_GetRegister( pDevice, 0x000000D0 );

			ULONG R000000CC = SC0710_GetRegister(pDevice, 0x000000CC);

			R000000D0 &= 0x4;

			if(p_sys_cfg->n_is_multi_channels_enabled) {

				SC0710_SetRegister(pDevice, 0x000000C8, (0));
			}
			else if(is_10bits) {

				if(n_header_payload)
				{
					SC0710_SetRegister(pDevice, 0x000000C8, cy * 3);
				}
				else
				{
					SC0710_SetRegister(pDevice, 0x000000C8, 0);
				}
			}
			else {

				if( ((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1A)) ||
					((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1A)) ||
					((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1B)) ||
					((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1B)) ||
					((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x13)) ||
					((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x13)) ||
					((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||
					((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||
					((pDevice->bcdDevice == 0xB401) && (pDevice->iManufacturer == 0x16)) ||
					((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||
					((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ||
					((pDevice->bcdDevice == 0xB201) && (pDevice->iManufacturer == 0x18)) ||
					((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18)) ||
					((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x18)) ||
					((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x18)) ) {

					if(n_header_payload > 0) {


						SC0710_SetRegister(pDevice, 0x000000C8, (cy * 2) + (n_vbi_lines * 2));
					}
					else {

						if(interleaved)
						{

							SC0710_SetRegister(pDevice, 0x000000C8, (cy) / 2 + (n_vbi_lines));
						}
						else
						{

							SC0710_SetRegister(pDevice, 0x000000C8, (cy * 1) + (n_vbi_lines));
						}
					}
				}
				else
				{
					if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x15)) ||
						((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x15)) ||
						((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x17)) ||
						((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x17)) ||
						((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x17)) ||
						((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x11))) {

						SC0710_SetRegister(pDevice, 0x000000C8, (cy));
					}
					else
					{

						SC0710_SetRegister(pDevice, 0x000000C8, 0);
					}
				}
			}
			LINUXV4L2_PRINT(KERN_INFO, "[%02d] BAR0 register 0xC8, value(0d%d)\n", pDevice->m_nKsDeviceNumber, SC0710_GetRegister(pDevice, 0x000000C8));

			if(p_sys_cfg->n_mcu_version >= 190823) {

				LINUXV4L2_PRINT(KERN_INFO, "[DATA.PORT.CTRL]\n");

				if(p_sys_cfg->n_input_video_resolution_dataport == 1) {

					R000000D0 |= 0x00000010;
				}
				else {

					R000000D0 &= ~0x00000010;
				}
			}
			else {

				LINUXV4L2_PRINT(KERN_INFO, "[150000000]\n");

				if((cx * cy * fps) <= (150000000)) {

					R000000D0 |= 0x00000010;
				}
				else {

					R000000D0 &= ~0x00000010;
				}
			}
			R000000D0 &= ~0x00000060;

			if(is_10bits) {

				if(n_10bits_mode == 6) {

					R000000D0 |= 0x00000000;

					R000000D0 |= 0x00000000;
				}
				else if((n_10bits_mode == 3) || (n_10bits_mode == 4)) {

					R000000D0 |= 0x00000020;

					R000000D0 |= 0x00004000;
				}
				else if(n_10bits_mode == 2) {

					R000000D0 |= 0x00000020;

					R000000D0 |= 0x00000400;
				}
				else if(n_10bits_mode == 5) {

					R000000D0 |= 0x00000400;

					R000000D0 |= (0x01 << 19);

				}
				else {

					R000000D0 |= 0x00000020;




				}
			}
			else {

				if(n_native_vdo444 >= 0x00000002) {

					R000000D0 |= 0x00000060;

					R000000D0 |= 0x00004000;
				}
				else if(n_native_vdo444 == 0x00000001) {

					R000000D0 |= 0x00000040;

					R000000D0 |= 0x00004000;
				}
				else {

					R000000D0 |= 0x00000000;

					R000000D0 |= 0x00004000;
				}
			}
			R000000D0 |= 0x00000100;

			if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

				(pDevice->iManufacturer == 0x1C) ||

				(pDevice->iManufacturer == 0x1D) ||

				(pDevice->iManufacturer == 0x1C) ||

				(pDevice->iManufacturer == 0x1C) ||

				(pDevice->iManufacturer == 0x1D) ||

				(pDevice->iManufacturer == 0x29) ||

				(pDevice->iManufacturer == 0x2D)) {

				if(((pDevice->bcdDevice == 0xD004) && (pDevice->iManufacturer == 0x1C)) ||
					((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1C)) ||
					((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
					((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

					;
				}
				else {

					R000000D0 &= ~0x000000E0;

					if(pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty >= 3) {

						R000000D0 |= 0x00000020;
					}
					else {

						R000000D0 |= 0x00000080;
					}
				}
			}
			if(((pDevice->bcdDevice == 0x0B04) && (pDevice->iManufacturer == 0x1B))) {

				R000000D0 &= ~0x000000E0;

				if(pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty >= 3) {

					R000000D0 |= 0x00000020;
				}
				else {

					R000000D0 |= 0x00000080;
				}
			}
			if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

				(pDevice->iManufacturer == 0x1C) ||

				(pDevice->iManufacturer == 0x1D) ||







				(pDevice->iManufacturer == 0x1E) ||

				(pDevice->iManufacturer == 0x1F)) {

				R000000D0 &= ~0x000000E0;

				if(n_native_vdo444 == 0x00000001) {

					R000000D0 |= 0x00000040;
				}
				else if(n_native_vdo444 == 0x00000002) {

					R000000D0 |= 0x00000060;
				}
				else if(n_10bits_mode == 0x00000003) {

					R000000D0 |= 0x00000020;
				}
				else {

					R000000D0 |= 0x00000080;
				}
			}
			R000000CC &= ~0x00000800;

			SC0710_SetRegister(pDevice, 0x000000D0, R000000D0);


			if((n_10bits_mode == 3) || (n_10bits_mode == 4)) {

				if(fps > 60) {

					R000000CC |= 0x00000800;
				}
				else {

					R000000CC |= 0x00000000;
				}
			}
			else {

				R000000CC |= 0x00000000;
			}
			SC0710_SetRegister(pDevice, 0x000000CC, R000000CC);

			{
				ULONG R000000DC = p_sys_cfg->n_input_video_resolution_bitrate;









				SC0710_SetRegister(pDevice, 0x000000DC, R000000DC);
			}

			R000000D0 |= 0x00000200;

			SC0710_SetRegister(pDevice, 0x000000D0, R000000D0);

			R000000D0 &= ~0x00000200;

			if((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18))
			{
				if(pDevice->m_nCustomDolbyProperty)
					R000000D0 |= 0x00008000;
				else
					R000000D0 &= ~0x00008000;
			}

			if( ((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||
				((pDevice->bcdDevice == 0xB401) && (pDevice->iManufacturer == 0x16)) ||
				((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x13)) ||
				((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x13))) {

				R000000D0 |= 0x00000800;
			}




			if(pDevice->m_nCustomCompanyJojoroProperty) R000000D0 |= 0x00000800;

			SC0710_SetRegister(pDevice, 0x000000D0, R000000D0);

			LINUXV4L2_PRINT(KERN_INFO, "[%02d] BAR0 register 0xD0, value(0x%x)\n", pDevice->m_nKsDeviceNumber, SC0710_GetRegister(pDevice, 0x000000D0));
		}



		{	p_sys_cfg->n_output_video_resolution_cx = cx;

			p_sys_cfg->n_output_video_resolution_cy = cy;

			p_sys_cfg->n_output_video_resolution_interleaved = interleaved;

			p_sys_cfg->n_output_video_resolution_fps = fps;

			p_sys_cfg->n_output_video_resolution_fps_m = m;

			p_sys_cfg->n_output_video_resolution_fps_base_fps = (m == 0) ? (fps * 1000) : (fps * 1000000 / 1001);

			p_sys_cfg->n_output_video_color_format = n_native_vdo444;

			if (n_10bits_mode == 6) {

				p_sys_cfg->n_output_video_color_bits = (is_10bits) ? 6 : 0;
			}
			else if(n_10bits_mode == 5) {

				p_sys_cfg->n_output_video_color_bits = (is_10bits) ? 5 : 0;
			}
			else if(n_10bits_mode == 4) {

				p_sys_cfg->n_output_video_color_bits = (is_10bits) ? 4 : 0;
			}
			else if(n_10bits_mode == 3) {

				p_sys_cfg->n_output_video_color_bits = (is_10bits) ? 3 : 0;
			}
			else if(n_10bits_mode == 2) {

				p_sys_cfg->n_output_video_color_bits = (is_10bits) ? 2 : 0;
			}
			else if (n_10bits_mode == 1) {

				p_sys_cfg->n_output_video_color_bits = (is_10bits) ? 1 : 0;
			}
			else {

				p_sys_cfg->n_output_video_color_bits = (is_10bits) ? 0 : 0;
			}
			p_sys_cfg->n_output_video_bufsize = pcm_buf_size_v;

			p_sys_cfg->n_output_video_bufsize_header_payload = n_header_payload;

			LINUXV4L2_PRINT(KERN_INFO, "[%02d] n_output_video_color_bits(0x%x)\n", pDevice->m_nKsDeviceNumber, p_sys_cfg->n_output_video_color_bits);

		}
	}
	pDevice->m_nVideoInterruptCount = 0;

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_HwStopVideoDMA(CDevice* pDevice)
{
	LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwStopVideoDMA\n", pDevice->m_nKsDeviceNumber);


	RtlZeroMemory( (BYTE*) pDevice->m_nDescriptor_Elements, sizeof(ULONG) * MAX_SUB_DEVICE_NUM );

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_HwStartAudioDMA(CDevice* pDevice, BOOL is_8ch_i2s)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	ULONG freq = p_sys_cfg->n_input_audio_sampling_frequency;

	ULONG rings_a = 8;

	ULONG pcm_buf_size_a = LINUXV4L2_SND_PCM_HARDWARE_SC0710.period_bytes_min * 4;

	dma_addr_t s_desc_phy_addr_a = 0;

	dma_addr_t s_resu_phy_addr_a = 0;

	LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwStartAudioDMA() is_8ch_i2s(0x%x)\n", pDevice->m_nKsDeviceNumber, is_8ch_i2s);

	if(freq == 0) {

		freq = p_sys_cfg->n_output_audio_sampling_frequency;
	}
	if(freq == 0) {

		freq = 48000;
	}

	p_sys_cfg->n_output_audio_sampling_frequency = freq;

	p_sys_cfg->n_output_audio_i2s_format = (is_8ch_i2s) ? 8 : 2;

	pDevice->m_nAudioInterruptCount = 0;

	p_sys_cfg->n_completedDescCountAudio = 0;

	LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwStartAudioDMA() n_output_audio_sampling_frequency(0d%d) n_output_audio_i2s_format(0d%d)\n", pDevice->m_nKsDeviceNumber, p_sys_cfg->n_output_audio_sampling_frequency, p_sys_cfg->n_output_audio_i2s_format);

	if(((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D)) ||
		((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D))) {

		pcm_buf_size_a = LINUXV4L2_SND_PCM_HARDWARE_SC0710.period_bytes_min * 8;
	}

	s_desc_phy_addr_a = pDevice->m_pDmaBaseCommonPhysicalBuffer[129];

	s_resu_phy_addr_a = pDevice->m_pDmaBaseCommonPhysicalBuffer[129] + 0x1000;

	RtlZeroMemory((BYTE*)pDevice->m_pDmaBaseCommonBuffer[129], 0x2000);

	{
		ULONG i = 0;

		dma_addr_t s_desc_phy_addr_begin = 0;

		dma_addr_t s_desc_phy_addr_next = 0;

		dma_addr_t s_memory_phy_addrs[8] = { pDevice->m_pDmaBaseCommonPhysicalBuffer[130],

												pDevice->m_pDmaBaseCommonPhysicalBuffer[130],

												pDevice->m_pDmaBaseCommonPhysicalBuffer[130],

												pDevice->m_pDmaBaseCommonPhysicalBuffer[130],

												pDevice->m_pDmaBaseCommonPhysicalBuffer[130],

												pDevice->m_pDmaBaseCommonPhysicalBuffer[130],

												pDevice->m_pDmaBaseCommonPhysicalBuffer[130],

												pDevice->m_pDmaBaseCommonPhysicalBuffer[130],
		};

		s_desc_phy_addr_begin = pDevice->m_pDmaBaseCommonPhysicalBuffer[129];

		s_desc_phy_addr_next = pDevice->m_pDmaBaseCommonPhysicalBuffer[129];

		LINUXV4L2_DEBUG(KERN_INFO, "s_desc_phy_addr_a [llx%llx]", s_desc_phy_addr_a);

		LINUXV4L2_DEBUG(KERN_INFO, "pcm_buf_size_a (0d%d)", pcm_buf_size_a);

		s_memory_phy_addrs[0] += (INT_BYTES_A * 8 * 0);

		s_memory_phy_addrs[1] += (INT_BYTES_A * 8 * 1);

		s_memory_phy_addrs[2] += (INT_BYTES_A * 8 * 2);

		s_memory_phy_addrs[3] += (INT_BYTES_A * 8 * 3);

		s_memory_phy_addrs[4] += (INT_BYTES_A * 8 * 4);

		s_memory_phy_addrs[5] += (INT_BYTES_A * 8 * 5);

		s_memory_phy_addrs[6] += (INT_BYTES_A * 8 * 6);

		s_memory_phy_addrs[7] += (INT_BYTES_A * 8 * 7);

		for (i = 0; i < rings_a; i++) {

			XDMA_DESCRIPTOR* p_desc = NULL;

			p_desc = (XDMA_DESCRIPTOR*)(pDevice->m_pDmaBaseCommonBuffer[129] + (32 * i));

			s_desc_phy_addr_next += 32;

			if(i == (rings_a - 1)) {

				p_desc->control = XDMA_DESC_MAGIC | XDMA_DESC_COMPLETED_BIT;

				p_desc->numBytes = pcm_buf_size_a;

				p_desc->srcAddrHi = (UINT32)((s_resu_phy_addr_a >> 32) & 0xFFFFFFFF);

				p_desc->srcAddrLo = (UINT32)(s_resu_phy_addr_a & 0xFFFFFFFF);

				p_desc->dstAddrHi = (UINT32)((s_memory_phy_addrs[i % 8] >> 32) & 0xFFFFFFFF);

				p_desc->dstAddrLo = (UINT32)(s_memory_phy_addrs[i % 8] & 0xFFFFFFFF);

				p_desc->nextHi = (UINT32)((s_desc_phy_addr_begin >> 32) & 0xFFFFFFFF);

				p_desc->nextLo = (UINT32)(s_desc_phy_addr_begin & 0xFFFFFFFF);

				LINUXV4L2_DEBUG(KERN_INFO, "[%02d] [%08X.%08X.%08X%08X.%08X%08X.%08X%08X]-", i, p_desc->control, p_desc->numBytes,

					p_desc->srcAddrHi, p_desc->srcAddrLo,

					p_desc->dstAddrHi, p_desc->dstAddrLo,

					p_desc->nextHi, p_desc->nextLo);
			}
			else {

				p_desc->control = XDMA_DESC_MAGIC | XDMA_DESC_COMPLETED_BIT;

				p_desc->numBytes = pcm_buf_size_a;

				p_desc->srcAddrHi = (UINT32)((s_resu_phy_addr_a >> 32) & 0xFFFFFFFF);

				p_desc->srcAddrLo = (UINT32)(s_resu_phy_addr_a & 0xFFFFFFFF);

				p_desc->dstAddrHi = (UINT32)((s_memory_phy_addrs[i % 8] >> 32) & 0xFFFFFFFF);

				p_desc->dstAddrLo = (UINT32)(s_memory_phy_addrs[i % 8] & 0xFFFFFFFF);

				p_desc->nextHi = (UINT32)((s_desc_phy_addr_next >> 32) & 0xFFFFFFFF);

				p_desc->nextLo = (UINT32)(s_desc_phy_addr_next & 0xFFFFFFFF);

				LINUXV4L2_DEBUG(KERN_INFO, "[%02d] [%08X.%08X.%08X%08X.%08X%08X.%08X%08X]", i, p_desc->control, p_desc->numBytes,

					p_desc->srcAddrHi, p_desc->srcAddrLo,

					p_desc->dstAddrHi, p_desc->dstAddrLo,

					p_desc->nextHi, p_desc->nextLo);
			}
			s_resu_phy_addr_a += 32;
		}
	}

	WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[1][1]->firstDescHi), (UINT32)((pDevice->m_pDmaBaseCommonPhysicalBuffer[129] >> 32) & 0xFFFFFFFF));
	WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[1][1]->firstDescLo), (UINT32)(pDevice->m_pDmaBaseCommonPhysicalBuffer[129] & 0xFFFFFFFF));


	WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_sgdmas[1][1]->firstDescAdj), 0);

	p_sys_cfg->n_output_audio_bufsize = pcm_buf_size_a;

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_HwStopAudioDMA(CDevice* pDevice)
{
	return STATUS_SUCCESS;
}

NTSTATUS SC0710_HwStartVideoInterrupt(CDevice* pDevice)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwStartVideoInterrupt()", pDevice->m_nKsDeviceNumber);

	if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty)
	{
		pDevice->m_nTaskletExtraParameterA = 0;
		pDevice->m_nTaskletExtraParameterB = 0;
	}
	else
	{
		if(pDevice->m_nCustomSoftwareInterruptProperty)
		{

		}
		else
		{
			if( (pDevice->idVendor == 0x12AB) && (pDevice->idProduct & 0xFFFF) == 0x0750 ) {

				WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_interrupt->channelIntEnableW1S), 0x00000002);
			}
			else
			{
				WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_interrupt->channelIntEnableW1S), 0x00000004);
			}
		}
	}





	WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[0][1]->controlW1S), XDMA_CTRL_RUN_BIT);

	if ((pDevice->idVendor == 0x12AB) && (pDevice->idProduct & 0xFFFF) == 0x0750)
	{

		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[2][1]->controlW1S), XDMA_CTRL_RUN_BIT);
	}




	DELAY_100NS( 100000 );
	{
		ULONG R000000D0 = SC0710_GetRegister(pDevice, 0x000000D0);

		if( (pDevice->idVendor == 0x12AB) && (pDevice->idProduct & 0xFFFF) == 0x0750 ) {

			SC0710_SetRegister( pDevice, 0x000000B0, 0x00000012 );

			R000000D0 = 0x00000010;
		}
		else {

			R000000D0 |= 0x00000001;

		}

		SC0710_SetRegister(pDevice, 0x000000D0, R000000D0);
	}
	return STATUS_SUCCESS;
}

NTSTATUS SC0710_HwStopVideoInterrupt(CDevice* pDevice)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwStopVideoInterrupt() n_video_running_state(0x%x) n_audio_running_state(0x%x)", pDevice->m_nKsDeviceNumber, p_sys_cfg->n_video_running_state, p_sys_cfg->n_audio_running_state);

	if((p_sys_cfg->n_video_running_state == 0) && (p_sys_cfg->n_audio_running_state == 0))
	{
		ULONG R000000D0 = SC0710_GetRegister(pDevice, 0x000000D0);

		if( ((pDevice->idVendor == 0x12AB) && (pDevice->idProduct & 0xFFFF) == 0x0750) ) {

			SC0710_SetRegister( pDevice, 0x000000B0, 0x00000000 );

			R000000D0  = 0x00000000;
		}
		else {

			R000000D0 &= 0xFFFFFFFE;
		}

		SC0710_SetRegister(pDevice, 0x000000D0, R000000D0);
	}
	WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[0][1]->controlW1C), XDMA_CTRL_RUN_BIT);

	if ((pDevice->idVendor == 0x12AB) && (pDevice->idProduct & 0xFFFF) == 0x0750)
	{

		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[2][1]->controlW1C), XDMA_CTRL_RUN_BIT);
	}












	if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty)
	{
		pDevice->m_nTaskletExtraParameterA = 0;
		pDevice->m_nTaskletExtraParameterB = 0;
	}
	else
	{
		if(pDevice->m_nCustomSoftwareInterruptProperty)
		{

		}
		else
		{
			if( (pDevice->idVendor == 0x12AB) && (pDevice->idProduct & 0xFFFF) == 0x0750 ) {

				WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_interrupt->channelIntEnableW1C), 0x00000002);
			}
			else
			{
				WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_interrupt->channelIntEnableW1C), 0x00000004);
			}
		}
	}

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_HwStartAudioInterrupt(CDevice* pDevice)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	LINUXV4L2_PRINT(KERN_INFO, "SC0710_HwStartAudioInterrupt( CH%02X )", pDevice->m_nKsDeviceNumber);

	WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_interrupt->channelIntEnableW1S), 0x00000008);









	WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[1][1]->controlW1S), XDMA_CTRL_RUN_BIT);



	{
		ULONG R000000D0 = SC0710_GetRegister(pDevice, 0x000000D0);

		R000000D0 |= 0x00000001;

		if((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D))
		{

			R000000D0 |= 0x00000004;

			LINUXV4L2_PRINT(KERN_INFO, "SC0710_HwStartAudioInterrupt( CH%02X ), D0:0X%08X", pDevice->m_nKsDeviceNumber, R000000D0);
		}

		SC0710_SetRegister(pDevice, 0x000000D0, R000000D0);
	}
	return STATUS_SUCCESS;
}

NTSTATUS SC0710_HwStopAudioInterrupt(CDevice* pDevice)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwStopAudioInterrupt() n_video_running_state(0x%x) n_audio_running_state(0x%x)", pDevice->m_nKsDeviceNumber, p_sys_cfg->n_video_running_state, p_sys_cfg->n_audio_running_state);

	if((p_sys_cfg->n_video_running_state == 0) && (p_sys_cfg->n_audio_running_state == 0))
	{
		ULONG R000000D0 = SC0710_GetRegister(pDevice, 0x000000D0);

		R000000D0 &= 0xFFFFFFFE;

		if ((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D))
		{

			R000000D0 &= 0xFFFFFFFB;

			LINUXV4L2_PRINT(KERN_INFO, "SC0710_HwStopAudioInterrupt( CH%02X ), D0:0X%08X", pDevice->m_nKsDeviceNumber, R000000D0);
		}

		SC0710_SetRegister(pDevice, 0x000000D0, R000000D0);
	}


	WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[1][1]->controlW1C), XDMA_CTRL_RUN_BIT);









	WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_interrupt->channelIntEnableW1C), 0x00000008);

	return STATUS_SUCCESS;
}


NTSTATUS SC0710_HwVideoComponentsInitialize(CDevice* pDevice, CVideo* pVideo)
{
	if(pDevice->m_nVideoCaptureStreamPowerReference != 0) {

		pDevice->m_nVideoCaptureStreamPowerReference++;
		pDevice->m_nAnalogCaptureStreamPowerReference++;


	}
	else {
		pDevice->m_nVideoCaptureStreamPowerReference = 1;
		pDevice->m_nAnalogCaptureStreamPowerReference = 1;
	}

	LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_HwVideoComponentsInitialize( %d )\n", pDevice->m_nKsDeviceNumber, pDevice->m_nVideoCaptureStreamPowerReference);

	if(pDevice->m_nVideoCaptureStreamPowerReference == 1) {

		SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);











		ULONG n_native_vdo444 = 0x00000000;

		BOOL is_10bits = FALSE;



		SC0710_CLOSE_THREAD();

		if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24))
		{
			pDevice->m_nCustomAnalogVideoNativeColorDeepProperty = 0;
		}
		else if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P016) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P210))
		{
			pDevice->m_nCustomAnalogVideoNativeColorDeepProperty = 1;
		}
		else
		{
			pDevice->m_nCustomAnalogVideoNativeColorDeepProperty = 1;
		}

		if((pDevice->m_pVideo[0]->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pDevice->m_pVideo[0]->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24))
		{
			pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty = 1;

			pDevice->m_nCustomAnalogVideoNativeColorSpaceAutoProperty = 0x00000001;
		}

		else
		{
			pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty = 0;
		}

		if(pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty == 1) {

			if(pDevice->m_nCustomAnalogVideoNativeColorSpaceAutoProperty >= 0x00000002 ||

				pDevice->m_nCustomAnalogVideoNativeColorSpaceAutoProperty == 0x00000000) {

				n_native_vdo444 = 0x00000002;
			}
			else {

				n_native_vdo444 = 0x00000001;
			}
		}
		if(pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty >= 2) {

			if(pDevice->m_nCustomAnalogVideoNativeColorSpaceAutoProperty >= 0x00000002) {

				n_native_vdo444 = 0x00000002;
			}
			else if(pDevice->m_nCustomAnalogVideoNativeColorSpaceAutoProperty == 0x00000001) {

				n_native_vdo444 = 0x00000001;
			}
			else {

				n_native_vdo444 = 0x00000000;
			}
		}
		if(pDevice->m_nCustomAnalogVideoNativeColorDeepProperty == 1) {

			is_10bits = TRUE;
		}
		if(pDevice->m_nCustomAnalogVideoNativeColorDeepProperty >= 2) {

			is_10bits = (pDevice->m_nCustomAnalogVideoNativeColorDeepAutoProperty > 0) ? TRUE : FALSE;
		}
		if(is_10bits) {

			n_native_vdo444 = 0x00000000;
		}
		if(TRUE) {

			SC0710_HwStopVideoInterrupt(pDevice);

			SC0710_HwStopVideoDMA(pDevice);



			p_sys_cfg->n_video_running_state = 0x00000001;

			SC0710_HwStartVideoDMA(pDevice, n_native_vdo444, is_10bits, pVideo);

			if(TRUE) {

				p_sys_cfg->n_is_bad_signal_count_vs[0] = 0;



				p_sys_cfg->n_multi_chdma_remains = 0;









				p_sys_cfg->b_input_video_signal_changed = FALSE;

				p_sys_cfg->b_dma_length_error = FALSE;

				p_sys_cfg->b_dma_EOP_error = FALSE;
			}

			pDevice->m_nDmaBaseCommonBufferNumber[0] = 0;

			{
				ULONG i = 0;
				for(i = 0; i < 16; i++)
				{
					p_sys_cfg->n_output_video_stream_frame_number[ i ] = 0;
				}
			}


			SC0710_HwStartVideoInterrupt(pDevice);

			if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty)
			{
				SC0710_START_SOFTWARE_INTERRUPT_THREAD();
			}
		}

		if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty == 0)
		{
			{
				if(!pDevice->m_No_Signal_no_output_property)
				{
					StartVideoLossThread(pDevice);
				}
			}
		}
		else
		{

			if(!pDevice->m_No_Signal_no_output_property)
			{
				StartMultiChVideoLossThread(pDevice);
			}
		}

		if((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x17)) {


		}
		else
		{
			StartAudioLossThread(pDevice);
		}

		SC0710_START_THREAD();
	}

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_HwVideoComponentsUnInitialize(CDevice* pDevice)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_HwVideoComponentsUnInitialize() m_nVideoCaptureStreamPowerReference(0x%x)\n", pDevice->m_nKsDeviceNumber, pDevice->m_nVideoCaptureStreamPowerReference);

	if(pDevice->m_nVideoCaptureStreamPowerReference == 0) return STATUS_SUCCESS;

	if(pDevice->m_nVideoCaptureStreamPowerReference != 1) {

		pDevice->m_nVideoCaptureStreamPowerReference--;
		pDevice->m_nAnalogCaptureStreamPowerReference--;

		return STATUS_SUCCESS;
	}
	pDevice->m_nVideoCaptureStreamPowerReference = 0;
	pDevice->m_nAnalogCaptureStreamPowerReference = 0;

	if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty == 0)
	{
		{
			if(!pDevice->m_No_Signal_no_output_property)
			{
				StopVideoLossThread(pDevice);
			}
		}
	}
	else
	{
		if(!pDevice->m_No_Signal_no_output_property)
		{
			StopMultiChVideoLossThread(pDevice);
		}
	}

	if ((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x17)) {


	}
	else
	{
		StopAudioLossThread(pDevice);
	}

	SC0710_CLOSE_THREAD();


	p_sys_cfg->n_video_running_state = 0x00000000;

	if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty)
	{
		SC0710_CLOSE_SOFTWARE_INTERRUPT_THREAD();
	}

	SC0710_HwStopVideoInterrupt(pDevice);

	SC0710_HwStopVideoDMA(pDevice);

	{

		SC0710_EnableHDR2SDR(pDevice, pDevice->m_nCustomAnalogVideoHDR2SDRProperty);
	}

	SC0710_START_THREAD();

	return SC0710_PowerDownPeripherals(pDevice);
}

NTSTATUS SC0710_HwAudioComponentsInitialize(CDevice* pDevice)
{
	BOOL is_8ch_i2s = TRUE;

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	if(pDevice->m_nAudioCaptureStreamPowerReference != 0) {

		pDevice->m_nAudioCaptureStreamPowerReference++;


	}
	else {
		pDevice->m_nAudioCaptureStreamPowerReference = 1;
	}

	LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwAudioComponentsInitialize( %d )\n", pDevice->m_nKsDeviceNumber, pDevice->m_nAudioCaptureStreamPowerReference);

	if(pDevice->m_nCustomAnalogAudio8chI2sEnableProperty > 0) {

		is_8ch_i2s = TRUE;
	}

	if(pDevice->m_nAudioCaptureStreamPowerReference == 1) {

		SC0710_HwStopAudioInterrupt(pDevice);

		SC0710_HwStopAudioDMA(pDevice);



		p_sys_cfg->n_audio_running_state = 0x00000001;

		{
			ULONG i = 0;
			for(i = 0; i < 16; i++)
			{
				p_sys_cfg->n_output_audio_stream_frame_number[ i ] = 0;

				p_sys_cfg->n_output_audio_stream_buffer_size[ i ] = 0;

				p_sys_cfg->n_output_audio_stream_1mins_diff_duration[ i ] = 0;
			}
		}

		SC0710_HwStartAudioDMA(pDevice, is_8ch_i2s);

		SC0710_HwStartAudioInterrupt(pDevice);

		pDevice->m_nDmaBaseCommonBufferNumber[1] = 0;

		if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty == 0)
		{

		}
	}

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_HwAudioComponentsUnInitialize(CDevice* pDevice)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwAudioComponentsUnInitialize() m_nAudioCaptureStreamPowerReference(0x%x)\n", pDevice->m_nKsDeviceNumber, pDevice->m_nAudioCaptureStreamPowerReference);

	if(pDevice->m_nAudioCaptureStreamPowerReference == 0) return STATUS_SUCCESS;

	if(pDevice->m_nAudioCaptureStreamPowerReference != 1) {

		pDevice->m_nAudioCaptureStreamPowerReference--;

		return STATUS_SUCCESS;
	}
	pDevice->m_nAudioCaptureStreamPowerReference = 0;

	if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty == 0)
	{

	}


	p_sys_cfg->n_audio_running_state = 0x00000000;

	SC0710_HwStopAudioInterrupt(pDevice);

	SC0710_HwStopAudioDMA(pDevice);





	return SC0710_PowerDownPeripherals(pDevice);
}

NTSTATUS SC0710_HwUnInitialize(CDevice* pDevice)
{
	LINUXV4L2_DEBUG(KERN_INFO, "SC0710_HwUnInitialize()\n");

	SC0710_StopControlPanelAnalysisThread(pDevice);

	{
		SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

		ULONG i = 0;
		for(i = 0; i < 16; i++)
		{
			FREE(p_sys_cfg->p_output_audio_stream_buffer_i2s[ i ]);

			p_sys_cfg->n_output_audio_stream_buffer_size[ i ] = 0;
		}
	}

	g_pDevice[pDevice->m_nKsDeviceNumber] = NULL;

	FREE(pDevice->m_pCustomSystemConfigProperty);

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_HwInitialize(CDevice* pDevice)
{
	ULONG i = 0;

	SC0710_SYS_CFG* p_sys_cfg = NULL;

	if((pDevice->idVendor == 0x10EE && (pDevice->idProduct & 0xFFF0) == 0x7020) ||

		(pDevice->idVendor == 0x12AB && (pDevice->idProduct & 0xFFFF) == 0x0710) ||

		(pDevice->idVendor == 0x12AB && (pDevice->idProduct & 0xFFFF) == 0x0720) ||

		(pDevice->idVendor == 0x12AB && (pDevice->idProduct & 0xFFFF) == 0x0750) ||

		(pDevice->idVendor == 0x12AB && (pDevice->idProduct & 0xFFFF) == 0x0410)) {

		;
	}
	else {



		LINUXV4L2_PRINT(KERN_ERR, "WRONG ID idVendor(0x%x) idProduct(0x%x)\n", pDevice->idVendor, pDevice->idProduct);

		return STATUS_UNSUCCESSFUL;
	}

	if(pDevice)
	{
		g_pDevice[pDevice->m_nKsDeviceNumber] = pDevice;

		pDevice->m_pCustomSystemConfigProperty = (SC0710_SYS_CFG*)kmalloc(sizeof(SC0710_SYS_CFG), GFP_KERNEL);

		if(pDevice->m_pCustomSystemConfigProperty == NULL)
		{
			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_HwInitialize() no m_pCustomSystemConfigProperty, error\n", pDevice->m_nKsDeviceNumber);

			return FALSE;
		}
	}
	else
	{
		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_HwInitialize() no pDevice, error\n", pDevice->m_nKsDeviceNumber);

		return FALSE;
	}

	memset(pDevice->m_pCustomSystemConfigProperty, 0, sizeof(SC0710_SYS_CFG));

	p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	LINUXV4L2_PRINT(KERN_INFO, "SC0710_HwInitialize( %d.%d.IV:%04X.IP:%04X.BD:%04X.IM:%04X )\n", pDevice->m_nKsDeviceNumber, pDevice->m_nKsDeviceBusNumber, pDevice->idVendor, pDevice->idProduct, pDevice->bcdDevice, pDevice->iManufacturer);

	init_waitqueue_head(&pDevice->m_wait_queue_control);

	if( pDevice->idVendor == 0x12AB && pDevice->idProduct == 0x0750 ) {

		ULONG CFG80 = 0x00000000;

		pci_read_config_dword(pDevice->m_pKsDevice, 0x80, &CFG80);

		p_sys_cfg->n_pcie_generation = (CFG80 >> 16) & 0x0F;

		p_sys_cfg->n_pcie_link_width = (CFG80 >> 20) & 0x0F;

		LINUXV4L2_PRINT(KERN_INFO, "SC0750_HwInitialize() CFG80(0x%x) generation(0x%x) width(0x%x)\n", CFG80, p_sys_cfg->n_pcie_generation, p_sys_cfg->n_pcie_link_width);
	}
	else {

		ULONG CFG70 = 0x00000000;

		pci_read_config_dword(pDevice->m_pKsDevice, 0x70, &CFG70);

		p_sys_cfg->n_pcie_generation = (CFG70 >> 16) & 0x0F;

		p_sys_cfg->n_pcie_link_width = (CFG70 >> 20) & 0x0F;

		LINUXV4L2_PRINT(KERN_INFO, "SC0710_HwInitialize() CFG70(0x%x) generation(0x%x) width(0x%x)\n", CFG70, p_sys_cfg->n_pcie_generation, p_sys_cfg->n_pcie_link_width);
	}
	if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x10)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x11))) {

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x13)) ||

		((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x13)) ) {

		pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 0;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x23))) {

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1A)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1A)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1B)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1B)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1B)) ||
		((pDevice->bcdDevice == 0xB004) && (pDevice->iManufacturer == 0x1B)) ||
		((pDevice->bcdDevice == 0x0B04) && (pDevice->iManufacturer == 0x1B)) ||
		((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x1B)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2B)) ||
		((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B))) {

		if((pDevice->bcdDevice & 0x00FF) >= 0x00000002) {

			pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;
		}
		pDevice->m_nAnalogCrossbarVideoInputProperty = 4;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x12)) ||

		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1C)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1C)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1D)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1D)) ||
		((pDevice->bcdDevice == 0x0A04) && (pDevice->iManufacturer == 0x1D)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1D)) ||
		((pDevice->bcdDevice == 0x0B04) && (pDevice->iManufacturer == 0x1D)) ||
		((pDevice->bcdDevice == 0xB004) && (pDevice->iManufacturer == 0x1D)) ||
		((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x1D)) ||
		((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D)) ||
		((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x1D)) ||
		((pDevice->bcdDevice == 0x0204) && (pDevice->iManufacturer == 0x29)) ||
		((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x29)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2D)) ||
		((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D))) {

		if((pDevice->bcdDevice & 0x00FF) >= 0x00000002) {

			pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;
		}
		pDevice->m_nAnalogCrossbarVideoInputProperty = 0;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1C))) {

		pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;

		pDevice->m_nAnalogCrossbarVideoInputProperty = 0;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}

	if(((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
		((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F))) {

		pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;

		pDevice->m_nAnalogCrossbarVideoInputPropertys[0] = 0;

		pDevice->m_nAnalogCrossbarVideoInputPropertys[1] = 4;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if(((pDevice->bcdDevice == 0xA201) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x1E))) {

		pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;

		pDevice->m_nAnalogCrossbarVideoInputPropertys[0] = 4;

		pDevice->m_nAnalogCrossbarVideoInputPropertys[1] = 4;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1E))) {

		if((pDevice->bcdDevice & 0x00FF) >= 0x00000002) {

			pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;
		}
		pDevice->m_nAnalogCrossbarVideoInputPropertys[0] = 4;

		pDevice->m_nAnalogCrossbarVideoInputPropertys[1] = 0;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F))) {

		if((pDevice->bcdDevice & 0x00FF) >= 0x00000002) {

			pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;
		}
		pDevice->m_nAnalogCrossbarVideoInputPropertys[0] = pDevice->m_nAnalogCrossbarVideoInputProperty;

		pDevice->m_nAnalogCrossbarVideoInputPropertys[1] = 4;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if(((pDevice->bcdDevice == 0xD004) && (pDevice->iManufacturer == 0x1C)) ||
		((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1C)) ||
		((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ||
		((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
		((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
		((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

		if((pDevice->bcdDevice & 0x00FF) >= 0x00000002) {

			pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;
		}
		pDevice->m_nAnalogCrossbarVideoInputProperty = 0;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x14)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x14)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x15)) ||
		((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x15)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x15)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x17)) ||
		((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x17)) ||
		((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x17)) ||
		((pDevice->bcdDevice == 0x0301) && (pDevice->iManufacturer == 0x17)) ||
		((pDevice->bcdDevice == 0xB001) && (pDevice->iManufacturer == 0x17)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x17)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x17)) ||
		((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x17)) ||
		((pDevice->bcdDevice == 0x0104) && (pDevice->iManufacturer == 0x17)) ) {

		pDevice->m_nAnalogCrossbarVideoInputProperty = 0;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||
		((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||
		((pDevice->bcdDevice == 0xB401) && (pDevice->iManufacturer == 0x16)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x16)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0xB201) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x18))) {

		pDevice->m_nAnalogCrossbarVideoInputProperty = 4;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||
		((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||

		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ||

		((pDevice->bcdDevice == 0xB201) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18)) ) {



	}
	if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0102) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0104) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0108) && (pDevice->iManufacturer == 0x19))) {

		if(pDevice->bcdDevice & 0x0F00) {

			LINUXV4L2_PRINT(KERN_INFO, "[SDVOE.FIBER]\n");
		}
		else {

			LINUXV4L2_PRINT(KERN_INFO, "[SDVOE.CAT]\n");
		}
		pDevice->m_nAnalogCrossbarVideoInputProperty = 0;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if(((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0202) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0204) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0208) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0301) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0302) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0308) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0402) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0501) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0502) && (pDevice->iManufacturer == 0x19)) ||
		((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x29)) ||
		((pDevice->bcdDevice == 0x0202) && (pDevice->iManufacturer == 0x29)) ||
		((pDevice->bcdDevice == 0x0204) && (pDevice->iManufacturer == 0x29)) ||
		((pDevice->bcdDevice == 0x0301) && (pDevice->iManufacturer == 0x29)) ||
		((pDevice->bcdDevice == 0x0302) && (pDevice->iManufacturer == 0x29)) ||
		((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x29))) {

		if((pDevice->bcdDevice & 0x0F00) == 0x0300 ||
			(pDevice->bcdDevice & 0x0F00) == 0x0500) {

			LINUXV4L2_PRINT(KERN_INFO, "[AVP.FIBER]\n");
		}
		else {

			LINUXV4L2_PRINT(KERN_INFO, "[AVP.CAT]\n");
		}
		pDevice->m_nAnalogCrossbarVideoInputProperty = 0;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x000E) ||
		(pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x000F)) {

		pDevice->m_nAnalogCrossbarVideoInputProperty = 0;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if((pDevice->iProduct == 0x00 && pDevice->iManufacturer == 0x15 && pDevice->bcdDevice == 0x000B)) {

		pDevice->m_nAnalogCrossbarVideoInputProperty = 0;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if((pDevice->iProduct == 0x10 && pDevice->iManufacturer == 0xFC && pDevice->bcdDevice == 0xD07C) ||
		(pDevice->iProduct == 0x10 && pDevice->iManufacturer == 0xFC && pDevice->bcdDevice == 0xD07C)) {

		pDevice->m_nAnalogCrossbarVideoInputProperty = 0;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011)) {

		pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;

		pDevice->m_nAnalogCrossbarVideoInputProperty = 0;

		p_sys_cfg->b_is_mcu_support = TRUE;
	}
	if( ((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x17)) ||
		((pDevice->bcdDevice == 0x0104) && (pDevice->iManufacturer == 0x17)) ) {

		BYTE chip_id = 0x00;

		BYTE ven_id = 0x00;

		BYTE dev_id = 0x00;

		{	unsigned char txbuf[2] = { 0xFD, 0 };

		unsigned char rxbuf[2] = { 0, 0 };

		SC0710_AccessI2cRegisterS_NEW(pDevice, 0x90, txbuf, 1, rxbuf, 1, 0);

		chip_id = rxbuf[0];
		}
		{	unsigned char txbuf[2] = { 0xFE, 0 };

		unsigned char rxbuf[2] = { 0, 0 };

		SC0710_AccessI2cRegisterS_NEW(pDevice, 0x90, txbuf, 1, rxbuf, 1, 0);

		ven_id = rxbuf[0];
		}
		{	unsigned char txbuf[2] = { 0xFF, 0 };

		unsigned char rxbuf[2] = { 0, 0 };

		SC0710_AccessI2cRegisterS_NEW(pDevice, 0x90, txbuf, 1, rxbuf, 1, 0);

		dev_id = rxbuf[0] & 0xF0;
		}


		if(chip_id == 0x50 && ven_id == 0x50 && dev_id == 0x90) {

			p_sys_cfg->b_is_thermal_support = TRUE;
		}
	}



	p_sys_cfg->p_config_bar_address = NULL;

	for (i = 0; i < 2; i++) {

		ULONG IRQ00 = READ_REGISTER_ULONG((ULONG*)(pDevice->m_pRegBaseCommonBuffer[i] + SC0710_REGBASE_IRQ + 0x00000000));

		ULONG CFG00 = READ_REGISTER_ULONG((ULONG*)(pDevice->m_pRegBaseCommonBuffer[i] + SC0710_REGBASE_CONFIG + 0x00000000));

		LINUXV4L2_PRINT(KERN_INFO, "[%02d] [BAR:%02d, IRQ:%08X, CFG:%08X]\n", pDevice->m_nKsDeviceNumber, i, IRQ00, CFG00);

		if(i == 0) {

			ULONG R000000C4 = READ_REGISTER_ULONG((ULONG*)(pDevice->m_pRegBaseCommonBuffer[0] + 0x000000C4));

			R000000C4 &= 0xFFF0FFFF;

			R000000C4 |= 0x000F0000;

			WRITE_REGISTER_ULONG((ULONG*)(pDevice->m_pRegBaseCommonBuffer[0] + 0x000000C4), R000000C4);
		}
		if((IRQ00 & 0xFFF00000) == 0x1FC00000 &&

			(CFG00 & 0xFFF00000) == 0x1FC00000) {

			p_sys_cfg->p_config_bar_address = (BYTE*)(pDevice->m_pRegBaseCommonBuffer[i]);

			LINUXV4L2_PRINT(KERN_INFO, "[%02d] It is bar %d\n", pDevice->m_nKsDeviceNumber, i);

			p_sys_cfg->n_firmware_version = CFG00 & 0x000000FF;



			break;
		}
	}
	if(p_sys_cfg->p_config_bar_address) {



		p_sys_cfg->p_regs_interrupt = (XDMA_IRQ_REGS*)(p_sys_cfg->p_config_bar_address + SC0710_REGBASE_IRQ);

		for (i = 0; i < XDMA_NUM_DIRECTIONS; i++) {

			ULONG ch = 0;
			for (ch = 0; ch < XDMA_MAX_NUM_CHANNELS; ch++) {

				ULONG offset = (i * SC0710_REGBASE_BLOCK_OFFSET) + (ch * SC0710_REGBASE_ENGINE_OFFSET);

				p_sys_cfg->p_regs_xdma_engines[ch][i] = (XDMA_ENGINE_REGS*)(p_sys_cfg->p_config_bar_address + SC0710_REGBASE_ENGINE_H2C + offset);

				p_sys_cfg->p_regs_xdma_sgdmas[ch][i] = (XDMA_SGDMA_REGS*)(p_sys_cfg->p_config_bar_address + SC0710_REGBASE_SGDMA_H2C + offset);
				{
					ULONG ENG00 = READ_REGISTER_ULONG((ULONG*)(p_sys_cfg->p_config_bar_address + SC0710_REGBASE_ENGINE_H2C + offset + 0x00000000));

					ULONG SGD00 = READ_REGISTER_ULONG((ULONG*)(p_sys_cfg->p_config_bar_address + SC0710_REGBASE_SGDMA_H2C + offset + 0x00000000));

					LINUXV4L2_PRINT(KERN_INFO, "[%02d] [DIR:%02d, ENG:%08X, ID:%08X.%08X]\n", pDevice->m_nKsDeviceNumber, i, ch, ENG00, SGD00);
				}
			}
		}


		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[0][0]->intEnableMaskW1S), XDMA_CTRL_IE_ALL);


		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[0][0]->controlW1S), XDMA_CTRL_IE_ALL);


		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[1][0]->intEnableMaskW1S), XDMA_CTRL_IE_ALL);


		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[1][0]->controlW1S), XDMA_CTRL_IE_ALL);


		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[0][1]->intEnableMaskW1S), XDMA_CTRL_IE_ALL | XDMA_CTRL_IE_IDLE_STOPPED);


		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[0][1]->controlW1S), XDMA_CTRL_IE_ALL | XDMA_CTRL_IE_IDLE_STOPPED);


		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[1][1]->intEnableMaskW1S), XDMA_CTRL_IE_ALL | XDMA_CTRL_IE_IDLE_STOPPED);


		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[1][1]->controlW1S), XDMA_CTRL_IE_ALL | XDMA_CTRL_IE_IDLE_STOPPED);


		if( (pDevice->idVendor == 0x12AB) && (pDevice->idProduct & 0xFFFF) == 0x0750 )
		{
			WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[2][1]->intEnableMaskW1S), XDMA_CTRL_IE_ALL | XDMA_CTRL_IE_IDLE_STOPPED);


			WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_xdma_engines[2][1]->controlW1S), XDMA_CTRL_IE_ALL | XDMA_CTRL_IE_IDLE_STOPPED);
		}


		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_interrupt->userVector[0]), 0);


		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_interrupt->userVector[1]), 0);


		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_interrupt->userVector[2]), 0);


		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_interrupt->userVector[3]), 0);


		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_interrupt->channelVector[0]), 0);


		WRITE_REGISTER_ULONG(&(p_sys_cfg->p_regs_interrupt->channelVector[1]), 0);
	}
	else
	{
		LINUXV4L2_PRINT(KERN_ERR, "[%02d] SC0710_HwInitialize() no p_config_bar_address\n", pDevice->m_nKsDeviceNumber);

		return FALSE;
	}

	if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x000E) ||
		(pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x000F)) {

		pDevice->m_nCustomCompanyMedicalProperty = 1;
	}
	if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x10)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x11))) {

		pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat = 0;
	}
	else
	{
		pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat = 1;
	}

	p_sys_cfg->n_video_running_state = 0x00000000;

	p_sys_cfg->n_audio_running_state = 0x00000000;



	{	unsigned char txbuf[4] = { 0x12, 0x34, 0x57, 0 };

		unsigned char rxbuf[4] = { 0, 0, 0, 0 };

		int n_mcu_version = 0;

		for (i = 0; i < 10; i++) {

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, 0x55, txbuf, 3, rxbuf, 3);

			LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwInitialize( %02d: MCU.RD = %02X.%02X.%02X )\n", pDevice->m_nKsDeviceNumber, i, rxbuf[0], rxbuf[1], rxbuf[2]);

			if((rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x27) ||

				(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] >= 0x28)) {

				n_mcu_version = 0x00000001;

				break;
			}
			else {

				n_mcu_version = 0x00000000;
			}
			DELAY_100NS(1000000);
		}
		p_sys_cfg->n_mcu_version = n_mcu_version;
	}
	{	unsigned char txbuf[4] = { 0x12, 0x34, 0x58, 0 };

		unsigned char rxbuf[4] = { 0, 0, 0, 0 };

		int n_mcu_version = 0;

		int n_mcu_found = 0;

		for (i = 0; i < 10; i++) {

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, 0x55, txbuf, 3, rxbuf, 3);

			n_mcu_version = (rxbuf[0] * 10000) + (rxbuf[1] * 100) + (rxbuf[2]);

			LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwInitialize( %02d: MCU(%d) )\n", pDevice->m_nKsDeviceNumber, i, n_mcu_version);



			if((rxbuf[0] >= 19 && rxbuf[0] <= 29) && (rxbuf[1] >= 1 && rxbuf[1] <= 12) && (rxbuf[2] >= 1 && rxbuf[2] <= 31)) {

				n_mcu_found = 1;

				break;
			}
			DELAY_100NS(10000000);
		}
		if(n_mcu_found) {

			p_sys_cfg->n_mcu_version = n_mcu_version;
		}
		else {

			p_sys_cfg->n_mcu_version = 0;
		}
	}


	if((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x15)) {


		unsigned char txbuf[4] = { 0x12, 0x34, 0x6B, 0 };

		unsigned char rxbuf[4] = { 0, 0, 0, 0 };

		ULONG extend_mcu_version = 0;

		ULONG n_extend_mcu_found = 0;

		ULONG i = 0;

		for (i = 0; i < 10; i++) {

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, 0x55, txbuf, 3, rxbuf, 3);

			extend_mcu_version = (rxbuf[0] * 10000) + (rxbuf[1] * 100) + (rxbuf[2]);

			LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwInitialize( %02d: extend board MCU(%d) )\n", pDevice->m_nKsDeviceNumber, i, extend_mcu_version);



			if((rxbuf[0] >= 19 && rxbuf[0] <= 29) && (rxbuf[1] >= 1 && rxbuf[1] <= 12) && (rxbuf[2] >= 1 && rxbuf[2] <= 31)) {

				n_extend_mcu_found = 1;

				break;
			}
			DELAY_100NS(10000000);
		}
		if(n_extend_mcu_found) {

			p_sys_cfg->n_extend_mcu_version = extend_mcu_version;
		}
		else {

			p_sys_cfg->n_extend_mcu_version = 0;
		}


		{	unsigned char txbuf[4] = { 0x12, 0x34, 0x6C, 0 };

			unsigned char rxbuf[4] = { 0, 0, 0, 0 };

			LONG n_extend_board_type = 0;

			LONG n_extend_board_found = 0;

			ULONG i = 0;

			for (i = 0; i < 10; i++) {

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, 0x55, txbuf, 3, rxbuf, 3);

				n_extend_board_type = (rxbuf[0] * 10000) + (rxbuf[1] * 100) + (rxbuf[2]);

				LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwInitialize( %02d: extend board type = %d )\n", pDevice->m_nKsDeviceNumber, i, n_extend_board_type);



				if((rxbuf[0] >= 19 && rxbuf[0] <= 29) && (rxbuf[1] >= 1 && rxbuf[1] <= 12) && (rxbuf[2] >= 1 && rxbuf[2] <= 31)) {

					n_extend_board_found = 1;

					break;
				}
				DELAY_100NS(10000000);
			}
			if(n_extend_board_found) {

				p_sys_cfg->n_extend_board_type = n_extend_board_type;
			}
			else {

				p_sys_cfg->n_extend_board_type = 0;
			}
		}
	}
	INIT_WORK(&pDevice->w_audio, SC0710_HwProcessAnalogRawAudioFrame );

	{
		ULONG i = 0;
		for(i = 0; i < 16; i++)
		{
			p_sys_cfg->p_output_audio_stream_buffer_i2s[ i ] = kmalloc( 1920000, GFP_KERNEL );

			p_sys_cfg->n_output_audio_stream_buffer_size[ i ] = 0;
		}
	}


	#ifdef BG_UPDATE

	p_sys_cfg->b_is_background_updated = TRUE;

	#else
	SC0710_background_updated(pDevice);
	#endif


	SC0710_StartControlPanelAnalysisThread(pDevice);

	return SC0710_PowerDownPeripherals(pDevice);
}

NTSTATUS SC0710_PowerDownPeripherals(CDevice* pDevice)
{
	LINUXV4L2_DEBUG(KERN_INFO, "SC0710_PowerDownPeripherals()\n");

	return STATUS_SUCCESS;
}


NTSTATUS SC0710_HwProcessAnalogPCIPacket(CDevice* pDevice, ULONG status)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	CVideo* pVideo = pDevice->m_pVideo[0];



	unsigned long flags = 0x00000000;

	ULONG show_signal_video = 0;

	if(!pVideo->m_nCustomAnalogOutputVideoAudioProperty)
	{
		return STATUS_SUCCESS;
	}

	if(!pDevice->m_nAnalogVideoDecoderStatusProperty)
	{
		return STATUS_SUCCESS;
	}

	spin_lock_irqsave(&pVideo->m_sNo_Signal_SpinLock, flags);

	if(status & 0x00000001)
	{
		pDevice->m_nDmaBaseCommonBufferNumber[0]++;
	}

	if(pVideo->m_InNoVideoDPC)
	{
		show_signal_video = 0;
	}
	else
	{
		show_signal_video = 1;
	}

	spin_unlock_irqrestore(&pVideo->m_sNo_Signal_SpinLock, flags);

	if(show_signal_video)
	{

		if(status & 0x00000001)
		{
			if(pDevice->m_nVideoCaptureStreamPowerReference != 0)
			{
				ULONG do_callback[2] = { 0, 0};



				if(p_sys_cfg->n_frame_no_swint == 0)
				{



				}

				{
					BYTE* po = NULL;

					po = pDevice->m_pDmaBaseCommonBuffer[((pDevice->m_nVideoInterruptCount - 1) % 4) + 1];

					pDevice->m_Video_field = po[0] & 0x01;


					if(pDevice->m_Video_field == 1)
					{
						do_callback[0] = 1;

						if(pDevice->m_nMulti_Instance) do_callback[1] = 1;
					}


					if(pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[0] == p_sys_cfg->n_input_video_resolution_fps)
					{
						do_callback[0] = 1;
					}

					if(pDevice->m_nMulti_Instance)
					{
						if(pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[1] == p_sys_cfg->n_input_video_resolution_fps)
						{
							do_callback[1] = 1;
						}
					}
				}

#ifdef INTERLACE_FIELD_MODE
				{
				}
#endif



				if(p_sys_cfg->n_input_video_resolution_interleaved) {

					if(pDevice->m_nVideoInterruptCount > 1) {

						if(p_sys_cfg->n_video_running_state)
						{
							if(pDevice->m_eVideoStreamState[0] && do_callback[0]) SC0710_HwProcessAnalogRawVideoFrame(pDevice, pDevice->m_pVideo[0]);

							if(pDevice->m_nMulti_Instance) if(pDevice->m_eVideoStreamState[1] && do_callback[1]) SC0710_HwProcessAnalogRawVideoFrame(pDevice, pDevice->m_pVideo[1]);
						}
						else
						{
							LINUXV4L2_DEBUG(KERN_INFO, "[%02d] not run state, cb\n", pDevice->m_nKsDeviceNumber);
						}
					}
				}
				else {

					if(pDevice->m_nVideoInterruptCount > 0) {

						if(pDevice->m_combine_ready == 1)
						{
							LINUXV4L2_DEBUG(KERN_INFO, "[%02d] m_combine_ready still 1, cb --\n", pDevice->m_nKsDeviceNumber);
						}
						else
						{

						}

						pDevice->m_combine_ready = 1;

						if(p_sys_cfg->n_video_running_state)
						{
							if(pDevice->m_eVideoStreamState[0]) SC0710_HwProcessAnalogRawVideoFrame(pDevice, pDevice->m_pVideo[0]);

							if(pDevice->m_nMulti_Instance) if(pDevice->m_eVideoStreamState[1]) SC0710_HwProcessAnalogRawVideoFrame(pDevice, pDevice->m_pVideo[1]);

						}
						else
						{
							pDevice->m_combine_ready = 0;

							LINUXV4L2_DEBUG(KERN_INFO, "[%02d] not run state, cb\n", pDevice->m_nKsDeviceNumber);
						}
					}
				}
			}
		}
	}

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_PACK_MULTI_CHANNELS_PACKETS(CDevice* pDevice, ULONG n_sub_ch, ULONG cxo, ULONG cyo, ULONG cwo, ULONG cxe, ULONG cye, ULONG cwe, BYTE* pe, ULONG n_dma_bufsize, ULONG preview_number)
{
	ULONG line_count[4] = { 0, 0, 0, 0 };

	ULONG scanned_data_size = 0;

	ULONG cross_4M_boundary = 0;

	ULONG residue = 0;

	PC5_SDI_HEADERS p_head = NULL;

	ULONG current_4M_block = 1;

	ULONG total_4M_block = n_dma_bufsize / 0x400000 + 1;

	ULONG dma_buffer_number = 0;

	if(total_4M_block == 5)
	{
		dma_buffer_number = (((preview_number - 1) % 4) * 5) + 1;
	}
	else
	{
		dma_buffer_number = (((preview_number - 1) % 4) * 4) + 1;
	}

	p_head = (PC5_SDI_HEADERS)(pDevice->m_pDmaBaseCommonBuffer[dma_buffer_number]);

	while (scanned_data_size <= n_dma_bufsize) {



		if((p_head->magic_pattern[0] == 0xFF) &&

			(p_head->magic_pattern[1] == 0xFF) &&

			(p_head->magic_pattern[2] == 0xFF) &&

			(p_head->magic_pattern[3] == 0x00)) {

			ULONG   ch = p_head->channel;

			ULONG   line = p_head->line_number;

			ULONG   n_data_width = p_head->width << 1;

			BYTE* p_start = (BYTE*)(p_head);

			ULONG* p_end = (ULONG*)(p_start + 8 + n_data_width);

			ULONG   offsets = 0;

			{

			}


			if(((scanned_data_size + 3840 + sizeof(PC5_SDI_HEADERS) * 2) >= (0x400000 * current_4M_block)) && (cross_4M_boundary == 0))
			{

				residue = 0x400000 * current_4M_block - scanned_data_size;



				RtlZeroMemory(pDevice->m_SC0710_Pack_Multi_Channels_temp_buffer, sizeof(pDevice->m_SC0710_Pack_Multi_Channels_temp_buffer));

				memcpy(pDevice->m_SC0710_Pack_Multi_Channels_temp_buffer, p_start, residue);

				memcpy(pDevice->m_SC0710_Pack_Multi_Channels_temp_buffer + residue, pDevice->m_pDmaBaseCommonBuffer[dma_buffer_number + 1], 4096 + sizeof(PC5_SDI_HEADERS));

				p_head = (PC5_SDI_HEADERS)(pDevice->m_SC0710_Pack_Multi_Channels_temp_buffer);

				cross_4M_boundary = 1;

				continue;
			}

			if(pDevice->m_nCustomCompanyMediaedgeProperty > 0) {

				if(n_sub_ch != ch) {





					if(cross_4M_boundary == 1)
					{
						p_start = pDevice->m_pDmaBaseCommonBuffer[++dma_buffer_number] + n_data_width + 16 - residue;



						cross_4M_boundary = 0;

						current_4M_block++;
					}
					else
					{
						p_start += (8) + (n_data_width)+(8);
					}

					p_head = (PC5_SDI_HEADERS)(p_start);

					scanned_data_size += (n_data_width + sizeof(PC5_SDI_HEADERS) * 2);

					continue;
				}
				offsets += (line - 1) * (cwe << 1);
			}
			else {

				offsets = (ch < 2) ? (0) : (cwe * cye);

				offsets += (ch % 2) * (cxo);

				offsets += (line - 1) * (cwe << 1);
			}

			if(ch > 3) {



				return STATUS_SUCCESS;
			}
			if(line > 1080) {



				return STATUS_SUCCESS;
			}


			if(line == 1)
			{
				LINUXV4L2_DEBUG(KERN_INFO, "ch(%d) line(%x)  %x %x %x %x %x %x %x %x %x %x %x\n", ch, line, *(p_start + 8), *(p_start + 10), *(p_start + 12), *(p_start + 14), *(p_start + 16), *(p_start + 18), *(p_start + 20), *(p_start + 22), *(p_start + 24), *(p_start + 26), *(p_start + 28));
			}

			if(pDevice->m_nCustomCompanyMediaedgeProperty > 0) {

				if(n_data_width > (cxo * 2)) {



					return STATUS_SUCCESS;
				}
			}
			else
			{
				if(n_data_width > cxo) {



					return STATUS_SUCCESS;
				}
			}

			if((offsets + n_data_width) > (n_dma_bufsize)) {



				return STATUS_SUCCESS;
			}
			RtlCopyMemory(pe + offsets, p_start + 8, n_data_width);



			if(p_end[0] == 0xFFFFFFFF &&

				p_end[1] == 0xFFFFFFFF) {

				line_count[ch]++;
			}
			else {

				break;
			}






			if(cross_4M_boundary == 1)
			{
				p_start = pDevice->m_pDmaBaseCommonBuffer[++dma_buffer_number] + n_data_width + 16 - residue;



				cross_4M_boundary = 0;

				current_4M_block++;
			}
			else
			{
				p_start += (8) + (n_data_width)+(8);
			}

			p_head = (PC5_SDI_HEADERS)(p_start);

			scanned_data_size += (n_data_width + sizeof(PC5_SDI_HEADERS) * 2);
		}
		else {



			break;
		}
	}

	if(line_count[n_sub_ch % 4] != 1080)
	{

	}

	return STATUS_SUCCESS;
}
NTSTATUS SC0710_SHRINK_16BIT_TO_10BIT(CDevice* pDevice, ULONG src_length, BYTE* p_src, ULONG dest_length, BYTE* p_dest)
{



	ULONG i = 0;

	for (i = 0; i < src_length; i += 8)
	{
		USHORT* p_16bit = (USHORT*)(p_src + i);

		BYTE* p_10bit = p_dest + (i / 8) * 5;

		if((i / 8) * 5 < dest_length)
		{
			*(p_10bit + 0) = (*p_16bit >> 6) & 0xFF;

			*(p_10bit + 1) = (*p_16bit >> 14) & 0x03;

			p_16bit++;

			*(p_10bit + 1) |= ((*p_16bit >> 6) & 0x3F) << 2;

			*(p_10bit + 2) = (*p_16bit >> 12) & 0x0F;

			p_16bit++;

			*(p_10bit + 2) |= ((*p_16bit >> 6) & 0x0F) << 4;

			*(p_10bit + 3) = (*p_16bit >> 10) & 0x3F;

			p_16bit++;

			*(p_10bit + 3) |= ((*p_16bit >> 6) & 0x03) << 6;

			*(p_10bit + 4) = (*p_16bit >> 8) & 0xFF;
		}
	}

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_PACK_MULTI_CHANNELS_PACKETS_P01X(CDevice* pDevice, ULONG n_sub_ch, ULONG cxo, ULONG cyo, ULONG cwo, ULONG cxe, ULONG cye, ULONG cwe, BYTE* pe, ULONG n_dma_bufsize, ULONG preview_number)
{
	ULONG line_count[4] = { 0, 0, 0, 0 };

	ULONG scanned_data_size = 0;

	ULONG cross_4M_boundary = 0;

	ULONG residue = 0;

	PC5_SDI_HEADERS_P01X p_head = NULL;

	ULONG current_4M_block = 1;

	ULONG total_4M_block = (n_dma_bufsize + 0x3FFFFF) / 0x400000;

	ULONG dma_buffer_number = 0;

	dma_buffer_number = (((preview_number - 1) % 4) * total_4M_block) + 1;

	if((dma_buffer_number == 1) || (dma_buffer_number == 7) || (dma_buffer_number == 13) || (dma_buffer_number == 19))
	{



	}
	else
	{
		LINUXV4L2_PRINT(KERN_INFO, "[%02d] wrong dma_buffer_number(%d)\n", pDevice->m_nKsDeviceNumber, dma_buffer_number);

		return FALSE;
	}

	p_head = (PC5_SDI_HEADERS_P01X)(pDevice->m_pDmaBaseCommonBuffer[dma_buffer_number]);

	while (scanned_data_size <= n_dma_bufsize) {



		if((p_head->magic_pattern[0] == 0xFFC0) &&

			(p_head->magic_pattern[1] == 0xFFC0) &&

			(p_head->magic_pattern[2] == 0xFFC0) &&

			(p_head->magic_pattern[3] == 0x0000)) {

			ULONG   ch = p_head->info[1] & 0x3;

			ULONG   line = ((p_head->info[1] >> 3) & 0x1F) | (p_head->info[3] & 0x7F) << 5;



			ULONG   n_vid_mode_color = (p_head->info[7] >> 5) & 0x1;

			ULONG   n_data_width = (p_head->info[5] | ((p_head->info[7] & 0x1F) << 8)) << 1;

			BYTE* p_start = (BYTE*)(p_head);

			ULONG* p_end = (ULONG*)(p_start + 16 + n_data_width);

			ULONG   offsets = 0;


			if(((scanned_data_size + n_data_width + sizeof(C5_SDI_HEADERS_P01X) * 2) >= (0x400000 * current_4M_block)) && (cross_4M_boundary == 0))
			{

				residue = 0x400000 * current_4M_block - scanned_data_size;



				RtlZeroMemory(pDevice->m_SC0710_Pack_Multi_Channels_temp_buffer, sizeof(pDevice->m_SC0710_Pack_Multi_Channels_temp_buffer));

				memcpy(pDevice->m_SC0710_Pack_Multi_Channels_temp_buffer, p_start, residue);

				memcpy(pDevice->m_SC0710_Pack_Multi_Channels_temp_buffer + residue, pDevice->m_pDmaBaseCommonBuffer[dma_buffer_number + 1], 4096 + sizeof(C5_SDI_HEADERS_P01X));

				p_head = (PC5_SDI_HEADERS_P01X)(pDevice->m_SC0710_Pack_Multi_Channels_temp_buffer);

				cross_4M_boundary = 1;

				continue;
			}

			if(pDevice->m_nCustomCompanyMediaedgeProperty > 0) {

				if(n_sub_ch != ch) {





					if(cross_4M_boundary == 1)
					{
						p_start = pDevice->m_pDmaBaseCommonBuffer[++dma_buffer_number] + n_data_width + 32 - residue;



						cross_4M_boundary = 0;

						current_4M_block++;
					}
					else
					{
						p_start += (16) + (n_data_width)+(16);
					}

					p_head = (PC5_SDI_HEADERS_P01X)(p_start);

					scanned_data_size += (n_data_width + sizeof(C5_SDI_HEADERS_P01X) * 2);

					continue;
				}

				if(n_vid_mode_color == 0)
				{
#ifdef P010_PACKED
					{
						offsets = (line - 1) * ((cwe * 10) >> 3);
					}
#else
					{
						offsets = (line - 1) * (cwe * 2);
					}
#endif
				}
				else
				{
#ifdef P010_PACKED
					{
						line /= 2;

						offsets = ((cwe * 10) >> 3) * cye + (line - 1) * ((cwe * 10) >> 3);
					}
#else
					{
						line /= 2;

						offsets = cwe * 2 * cye + (line - 1) * (cwe * 2);
					}
#endif
				}
			}
			else {

				offsets = (ch < 2) ? (0) : (cwe * cye);

				offsets += (ch % 2) * (cxo);

				offsets += (line - 1) * (cwe << 1);
			}

			if(ch > 3) {



				return STATUS_SUCCESS;
			}

			if(n_vid_mode_color == 0)
			{
				if((line > 1080) || (line == 0)) {



					return STATUS_SUCCESS;
				}
			}
			else
			{
				if((line > 540) || (line == 0)) {



					return STATUS_SUCCESS;
				}
			}

			if(pDevice->m_nCustomCompanyMediaedgeProperty > 0) {

				if(n_data_width != (cxo * 2)) {



					return STATUS_SUCCESS;
				}
			}
			else
			{
				if(n_data_width > cxo) {



					return STATUS_SUCCESS;
				}
			}

			if((offsets + n_data_width) > (n_dma_bufsize)) {



				return STATUS_SUCCESS;
			}

#ifdef P010_PACKED
			{
				SC0710_SHRINK_16BIT_TO_10BIT(pDevice, n_data_width, p_start + 16, (cxo * 10) >> 3, pe + offsets);
			}
#else
			{
				RtlCopyMemory(pe + offsets, p_start + 16, n_data_width);
			}
#endif



			if(p_end[0] == 0xFFC0FFC0 &&

				p_end[1] == 0xFFC0FFC0 &&

				p_end[2] == 0xFFC0FFC0 &&

				p_end[3] == 0xFFC0FFC0) {

				line_count[ch]++;

			}
			else {



				break;
			}






			if(cross_4M_boundary == 1)
			{
				p_start = pDevice->m_pDmaBaseCommonBuffer[++dma_buffer_number] + n_data_width + 32 - residue;

				cross_4M_boundary = 0;

				current_4M_block++;
			}
			else
			{
				p_start += (16) + (n_data_width)+(16);
			}

			p_head = (PC5_SDI_HEADERS_P01X)(p_start);

			scanned_data_size += (n_data_width + sizeof(C5_SDI_HEADERS_P01X) * 2);
		}
		else {



			break;
		}
	}

	if(line_count[n_sub_ch % 4] != 1080)
	{

	}

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_HwProcessAnalogRawVideoFrame(CDevice* pDevice, CVideo* pVideo)
{
#if defined(AME_SC0710) || defined(AME_SC0720)


	pDevice->m_combine_ready = 0;

	if(pDevice->m_eVideoStreamState[pVideo->m_nSubDeviceNumber] == 1)
	{
		CVideoBuffer* pVideoBuffer = NULL;

		unsigned long flags = 0x00000000;

		BOOLEAN is_completes = FALSE;

		ULONG real_fps = 0;

		ULONG dma_bytes = 0;

		ULONG video_frame = 0;

		SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

		ULONG cx = p_sys_cfg->n_output_video_resolution_cx;

		ULONG cy = p_sys_cfg->n_output_video_resolution_cy;

		ULONG is_skip = FALSE;

		ULONG bit = 0;

		if(pVideo == NULL) { return FALSE; }

		{
			if((pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF) != 0x77FFFFFF)
			{
				real_fps = pDevice->m_nCustomAnalogVideoFrameRateProperty;

				if(real_fps && pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber])
				{
					if(pDevice->m_nCustomAnalogVideoInterleavedProperty)
					{
						real_fps /= 2;
					}



					CALCULATE_FPS(pDevice, pVideo, real_fps, (pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF));

					if(real_fps <= (pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF))
					{
						is_skip = FALSE;
					}
					else
					{
						bit = pDevice->m_nVideoInterruptCount % real_fps;



						is_skip = (pDevice->m_pVideo[pVideo->m_nSubDeviceNumber]->m_valid[bit] == 0) ? TRUE : FALSE;
					}
				}
			}
		}

		if(is_skip)
		{

		}
		else
		{
			{
				spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);

				if(pVideo->m_Multi_Copy)
				{

					pVideo->m_Multi_Copy_Err_Cnt++;

					spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);

					LINUXV4L2_PRINT(KERN_ERR, "[%02d] m_Multi_Copy( 0x%x ), Err_Cnt(%d) SKIP\n", pDevice->m_nKsDeviceNumber, pVideo->m_Multi_Copy, pVideo->m_Multi_Copy_Err_Cnt);

					if(pVideo->m_Multi_Copy_Err_Cnt > 2)
					{


					}
					else
					{
						return FALSE;
					}
				}
				else
				{
					pVideo->m_Multi_Copy_Err_Cnt = 0;

					spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
				}

				{
					ULONG result_index = 0;

					ULONG quantity_4M_block = (p_sys_cfg->n_output_video_bufsize + 0x3FFFFF) / 0x400000;

					BYTE* p_result = NULL;

					if(p_sys_cfg->n_output_video_color_bits == 4) {

						ULONG i = 0;

						BYTE* p_result = NULL;

						ULONG elements_quantity_ahead = 0;

						BYTE* start_position = NULL;

						ULONG M420_DMA_order = (pDevice->m_nVideoInterruptCount - 1) % 4;


						for (i = 0; i < M420_DMA_order; i++)
						{
							elements_quantity_ahead += pDevice->m_nDescriptor_Elements[i];
						}



						start_position = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + elements_quantity_ahead * 32;



						for (i = 0; i < pDevice->m_nDescriptor_Elements[M420_DMA_order]; i++)
						{

							p_result = start_position + i * 32;

							dma_bytes += ((XDMA_RESULT*)p_result)->length;

							if(((XDMA_RESULT*)p_result)->status & 1)
							{
								if(i != (pDevice->m_nDescriptor_Elements[M420_DMA_order] - 1))
								{
									LINUXV4L2_DEBUG(KERN_INFO, "[%02d] EOP, i(0d%d) M420_DMA_order(%d)\n", pDevice->m_nKsDeviceNumber, i, M420_DMA_order);


									p_sys_cfg->b_dma_EOP_error = TRUE;
								}
							}
						}

					}
					else
					{
						for (result_index = 0; result_index < quantity_4M_block; result_index++)
						{
							p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + ((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block * 32 + result_index * 32;

							if(((XDMA_RESULT*)p_result)->length > 0x400000)
							{
								LINUXV4L2_PRINT(KERN_INFO, "[%02d] result_index(%d) length(%d) status(0x%x)-\n", pDevice->m_nKsDeviceNumber, result_index, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);

								return FALSE;
							}
							else
							{
								dma_bytes += ((XDMA_RESULT*)p_result)->length;
							}
						}
					}

					if(dma_bytes != p_sys_cfg->n_output_video_bufsize)
					{
						if(pDevice->m_nTaskletExtraParameterC & 0x00000080)
						{

						}
						else if((pDevice->m_nAnalogCopyProtMacrovisionProperty == 1) && (pDevice->m_nAnalogCopyProtMacrovisionEnableProperty == 0))
						{

						}
						else
						{
							LINUXV4L2_DEBUG(KERN_INFO, "[%02d] m_nSubDeviceNumber(%d) length(%d) ERROR-\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber, dma_bytes);


							p_sys_cfg->b_dma_length_error = TRUE;
						}
					}
				}

				if(p_sys_cfg->b_dma_length_error || p_sys_cfg->b_dma_EOP_error)
				{

					return FALSE;
				}
			}


			spin_lock_irqsave(&pVideo->m_sKsBufferListSpinLock, flags);


			if(list_empty(&pVideo->m_sKsBufferList) == 0) {

				if(pVideo->m_sKsBufferList.next)
				{
					pVideoBuffer = list_entry(pVideo->m_sKsBufferList.next, CVideoBuffer, list);

					if(pVideoBuffer)
					{
						if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
						{


							#ifdef CONFIG_X86_64
							if((((PVOID)(pVideoBuffer->list.next)) == LIST_POISON1) || (((PVOID)(pVideoBuffer->list.prev)) == LIST_POISON2) || (((ULONGLONG)(pVideoBuffer->list.next) & 0xFFFF000000000000) == 0xdead000000000000) || (((ULONGLONG)(pVideoBuffer->list.prev) & 0xFFFF000000000000) == 0xdead000000000000) || (((ULONGLONG)(pVideoBuffer->list.next) == 0x8080808080808080)) || (((ULONGLONG)(pVideoBuffer->list.prev)) == 0x8080808080808080))
							#else
							if((((PVOID)(pVideoBuffer->list.next)) == LIST_POISON1) || (((PVOID)(pVideoBuffer->list.prev)) == LIST_POISON2) || (((ULONG)(pVideoBuffer->list.next)) == 0xdead000000000100) || (((ULONG)(pVideoBuffer->list.prev)) == 0xdead000000000200) || (((ULONG)(pVideoBuffer->list.next) == 0x8080808080808080)) || (((ULONG)(pVideoBuffer->list.prev)) == 0x8080808080808080))
							#endif
							{

								LINUXV4L2_PRINT(KERN_INFO, "[%02d] error, next(%p) prev(%p)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->list.next, pVideoBuffer->list.prev);
								LINUXV4L2_PRINT(KERN_INFO, "[%02d] error, LIST_POISON1(%p) LIST_POISON2(%p)\n", pDevice->m_nKsDeviceNumber, LIST_POISON1, LIST_POISON2);

								pVideoBuffer = NULL;
							}
							else
							{

								if(p_sys_cfg->n_output_video_bufsize_header_payload)
								{
									if(pVideo->m_nCustomAnalogVideoVbiLinesEnableProperty > 0 && pVideo->m_nCustomAnalogVideoVbiLinesProperty > 0)
									{
										video_frame = dma_bytes - (p_sys_cfg->n_output_video_bufsize_header_payload * (cy / 2 + pVideo->m_nCustomAnalogVideoVbiLinesProperty / 2) * 4);
									}
									else
									{
										video_frame = dma_bytes - (p_sys_cfg->n_output_video_bufsize_header_payload * (cy / 2) * 4);
									}
								}
								else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010)
								{

									if(p_sys_cfg->n_output_video_color_bits == 1)
									{
										video_frame = dma_bytes;
									}
									else
									{
										video_frame = dma_bytes * 16 / 10;
									}
								}
								else
								{
									if(p_sys_cfg->n_input_video_resolution_interleaved)
									{
										video_frame = dma_bytes * 2;
									}
									else
									{
										video_frame = dma_bytes;
									}
								}


								if((vb2_get_plane_payload(&pVideoBuffer->vb2_buffer_cross, 0) == video_frame) || (pDevice->m_nAnalogVideoDecoderStatusProperty == 0))
								{
									if(pDevice->debug[0]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] GOT BUFFER\n", pDevice->m_nKsDeviceNumber);



									list_del(&pVideoBuffer->list);
								}
								else
								{
									LINUXV4L2_DEBUG(KERN_INFO, "[%02d] vb2_get_plane_payload() vb2_get_plane_payload(%ld) video_frame(%d) NOT MATCH\n", pDevice->m_nKsDeviceNumber, vb2_get_plane_payload(&pVideoBuffer->vb2_buffer_cross, 0), video_frame);

									pVideoBuffer = NULL;
								}
							}
						}
						else
						{
							LINUXV4L2_DEBUG(KERN_INFO, "[%02d] pVideoBuffer state error(0x%x)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state);

							pVideoBuffer = NULL;
						}
					}
					else
					{
						LINUXV4L2_DEBUG(KERN_INFO, "[%02d] list_entry return NULL\n", pDevice->m_nKsDeviceNumber);
					}
				}
				else
				{
					LINUXV4L2_DEBUG(KERN_INFO, "[%02d] m_sKsBufferList.next is NULL\n", pDevice->m_nKsDeviceNumber);
				}
			}
			else {

				LINUXV4L2_DEBUG(KERN_INFO, "[%02d] m_nSubDeviceNumber(%d) no video buffer!\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber);

				pVideoBuffer = NULL;
			}

			spin_unlock_irqrestore(&pVideo->m_sKsBufferListSpinLock, flags);
		}

		if(pVideoBuffer)
		{
			BYTE* pe = vb2_plane_vaddr(&pVideoBuffer->vb2_buffer_cross, 0);

			if(pe == NULL) { LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_HwProcessAnalogRawVideoFrame() pe == NULL \n", pDevice->m_nKsDeviceNumber); vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR); return FALSE; }

			if(pVideoBuffer->buffer_state != VB2_BUF_STATE_ACTIVE) { LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state error(%x)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state); vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR); return FALSE; }

			if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
			{
				{
					ULONG show_pattern = 0;

					if(pDevice->m_nTaskletExtraParameterC & 0x00000080)
					{
						show_pattern = 1;
					}

					if(p_sys_cfg->n_is_bad_signal_count_vs[0] > 0)
					{
						p_sys_cfg->n_is_bad_signal_count_vs[0]--;

						show_pattern = 1;
					}

					if(pDevice->m_No_Signal_no_output_property) show_pattern = 0;

					{
						BYTE* po = NULL;

						ULONG j = 0;

						ULONG cxe = pVideo->m_nDstFrameWidth;

						ULONG cye = pVideo->m_nDstFrameHeight;

						ULONG  interleaved = (p_sys_cfg->n_output_video_resolution_interleaved);

						if(vb2_get_plane_payload(&pVideoBuffer->vb2_buffer_cross, 0) >= p_sys_cfg->n_output_video_bufsize)
						{
							if((cx >= 640) && (cy >= 240))
							{
								if((pDevice->m_nAnalogCopyProtMacrovisionProperty == 1) && (pDevice->m_nAnalogCopyProtMacrovisionEnableProperty == 0))
								{
									if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
									{
										if((cxe >= 720) && (cye >= 480))
										{
											ULONG vertical = 0;

											if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY))
											{
												BYTE* logo = HDCP_LOGO_YUY2;

												BYTE* start = pe + cxe * 2 * (cye / 2 - 100) + cxe - 320;

												memset(pe, 0x80, cxe * cye * 2);

												for (vertical = 0; vertical < 240; vertical++)
												{
													memcpy(start, logo, 640);

													start += cxe * 2;
													logo += 640;
												}
											}
											else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12)
											{
											}
											else
											{
												BYTE* logo = HDCP_LOGO_Y;

												BYTE* start = pe + cxe * (cye / 2 - 100) + cxe / 2 - 160;

												memset(pe, 0x80, (cxe * cye * 12) >> 3);

												for (vertical = 0; vertical < 240; vertical++)
												{
													memcpy(start, logo, 320);

													start += cxe;
													logo += 320;
												}
											}
										}
									}
									else
									{
										LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error,  \n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state);
									}
								}
								else
								{
									ULONG  n_header_payload = p_sys_cfg->n_output_video_bufsize_header_payload;

									if(n_header_payload > 0)
									{
										ULONG  n_dma_bufsize = p_sys_cfg->n_output_video_bufsize;

										ULONG  swint = p_sys_cfg->n_frame_no_swint;

										if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY))
										{
											SC0710_PACK_MULTI_CHANNELS_PACKETS(pDevice, swint, cx, cy, cx, cxe, cye, cxe, pe, n_dma_bufsize, pDevice->m_nVideoInterruptCount);
										}
										else if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P016))
										{
											SC0710_PACK_MULTI_CHANNELS_PACKETS_P01X(pDevice, swint, cx, cy, cx, cxe, cye, cxe, pe, n_dma_bufsize, pDevice->m_nVideoInterruptCount);
										}
										else
										{
										}
									}
									else
									{
										ULONG quantity_4M_block = (cx * cy * 2 + 0x3FFFFF) / 0x400000;

										if(cx == 4096 && cy == 2160)
										{
											ULONG data_copied = 0;

											if(pVideo->m_Multi_Copy)
											{
												LINUXV4L2_PRINT(KERN_ERR, "[%02d] pVideo->m_Multi_Copy(0x%x) NOT ZERO - [FAIL]\n", pDevice->m_nKsDeviceNumber, pVideo->m_Multi_Copy);
											}

											if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY))
											{
												if(video_frame <= (cxe * cye * 2))
												{
													quantity_4M_block = (cx * cy * 2 + 0x3FFFFF) / 0x400000;

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);


														pVideo->m_nFrameNumberForQueue = pDevice->m_nVideoInterruptCount;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0] = pe;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1] = (BYTE*)pVideoBuffer;


														if(p_sys_cfg->n_output_video_color_bits == 4)
														{
															ULONG quantity_4M_Y_block = ((cx * cy) + 0x3FFFFF) / 0x400000;
															ULONG quantity_4M_C_block = ((cx * cy / 2) + 0x3FFFFF) / 0x400000;

															for (j = 0; j < quantity_4M_Y_block; j++)
															{

																pVideo->m_Multi_Copy |= (1 << j);

																{

																	ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_y[j % quantity_4M_Y_block]);

																	if(qw == 0) {

																		LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																		pVideo->m_Multi_Copy &= ~(1 << j);
																	}
																}
															}
															for (j = 0; j < quantity_4M_C_block; j++)
															{

																pVideo->m_Multi_Copy |= (1 << (j + quantity_4M_Y_block));

																{

																	ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_nv12[j % quantity_4M_C_block]);

																	if(qw == 0) {

																		LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work C() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																		pVideo->m_Multi_Copy &= ~(1 << (j + quantity_4M_Y_block));

																	}
																}
															}
														}
														else
														{
															for (j = 0; j < quantity_4M_block; j++)
															{

																pVideo->m_Multi_Copy |= (1 << j);

																{

																	ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_yuyv_bgr[j % quantity_4M_block]);

																	if(qw == 0) {

																		LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																		pVideo->m_Multi_Copy &= ~(1 << j);
																	}
																}
															}
														}

														spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error, j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);


														if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
													}
												}

												return TRUE;
											}
											else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P016)
											{
											}
											else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010)
											{
												if(video_frame <= (cxe * cye * 3))
												{
													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);


														pVideo->m_nFrameNumberForQueue = pDevice->m_nVideoInterruptCount;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0] = pe;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1] = (BYTE*)pVideoBuffer;


														if(p_sys_cfg->n_output_video_color_bits == 1)
														{
															ULONG quantity_4M_block = ((cx * cy * 3) + 0x3FFFFF) / 0x400000;

															for (j = 0; j < quantity_4M_block; j++)
															{

																pVideo->m_Multi_Copy |= (1 << j);

																{

																	ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_yuyv_bgr[j % quantity_4M_block]);

																	if(qw == 0) {

																		LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																		pVideo->m_Multi_Copy &= ~(1 << j);
																	}
																}
															}
														}
														else
														{
															ULONG quantity_4M_block = ((cx * cy * 15 >> 3) + 0x3FFFFF) / 0x400000;

															for (j = 0; j < quantity_4M_block; j++)
															{

																pVideo->m_Multi_Copy |= (1 << j);

																{

																	ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_p010[j % quantity_4M_block]);

																	if(qw == 0) {

																		LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																		pVideo->m_Multi_Copy &= ~(1 << j);
																	}
																}
															}
														}

														spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error, j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);


														if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
													}
												}
												else
												{
													LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range, video_frame(%d)-\n", pDevice->m_nKsDeviceNumber, video_frame);
												}

												return TRUE;
											}
											else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12)
											{
												if(video_frame <= ((cxe * cye * 12) >> 3))
												{
													ULONG quantity_4M_Y_block = ((cx * cy) + 0x3FFFFF) / 0x400000;
													ULONG quantity_4M_C_block = ((cx * cy / 2) + 0x3FFFFF) / 0x400000;

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);


														pVideo->m_nFrameNumberForQueue = pDevice->m_nVideoInterruptCount;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0] = pe;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1] = (BYTE*)pVideoBuffer;

														for (j = 0; j < quantity_4M_Y_block; j++)
														{

															pVideo->m_Multi_Copy |= (1 << j);

															{

																ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_y[j % quantity_4M_Y_block]);

																if(qw == 0) {

																	LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																	pVideo->m_Multi_Copy &= ~(1 << j);
																}
															}
														}
														for (j = 0; j < quantity_4M_C_block; j++)
														{

															pVideo->m_Multi_Copy |= (1 << (j + quantity_4M_Y_block));

															{

																ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_nv12[j % quantity_4M_C_block]);

																if(qw == 0) {

																	LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work C() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																	pVideo->m_Multi_Copy &= ~(1 << (j + quantity_4M_Y_block));

																}
															}
														}

														spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error, j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);


														if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
													}
												}

												return TRUE;
											}
											else if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24))
											{

												quantity_4M_block = (cx * cy * 3 + 0x3FFFFF) / 0x400000;

												for (j = 0; j < quantity_4M_block; j++)
												{

													BYTE* p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + ((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block * 32 + j * 32;

													if(((XDMA_RESULT*)p_result)->length > 0x400000)
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] j(%d) length(%d) status(0x%x)\n", pDevice->m_nKsDeviceNumber, j, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);

														continue;
													}

													po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block) + 1 + j];

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														if((data_copied + ((XDMA_RESULT*)p_result)->length) <= (cxe * cye * 3))
														{
															memcpy(pe, po, ((XDMA_RESULT*)p_result)->length);

															pe += ((XDMA_RESULT*)p_result)->length;

															data_copied += ((XDMA_RESULT*)p_result)->length;
														}
														else
														{
															LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range(%d) data_copied(%d)\n", pDevice->m_nKsDeviceNumber, ((XDMA_RESULT*)p_result)->length, data_copied);
														}
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error,  j(%d)-\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);


													}
												}
											}
											else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_M420)
											{

												quantity_4M_block = ((cx * cy * 12 >> 3) + 0x3FFFFF) / 0x400000;

												for (j = 0; j < quantity_4M_block; j++)
												{

													BYTE* p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + ((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block * 32 + j * 32;

													if(((XDMA_RESULT*)p_result)->length > 0x400000)
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] j(%d) length(%d) status(0x%x)\n", pDevice->m_nKsDeviceNumber, j, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);

														continue;
													}

													po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block) + 1 + j];

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														if((data_copied + ((XDMA_RESULT*)p_result)->length) <= (cxe * cye * 3))
														{
															memcpy(pe, po, ((XDMA_RESULT*)p_result)->length);

															pe += ((XDMA_RESULT*)p_result)->length;

															data_copied += ((XDMA_RESULT*)p_result)->length;
														}
														else
														{
															LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range(%d) data_copied(%d)\n", pDevice->m_nKsDeviceNumber, ((XDMA_RESULT*)p_result)->length, data_copied);
														}
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error,  j(%d)-\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);


													}
												}
											}
											else
											{
												ULONG quantity_2M_C_block = ((cx * cy / 2) + 0x1FFFFF) / 0x200000;

												if(video_frame <= ((cxe * cye * 12) >> 3))
												{
													ULONG quantity_4M_Y_block = ((cx * cy) + 0x3FFFFF) / 0x400000;

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);


														pVideo->m_nFrameNumberForQueue = pDevice->m_nVideoInterruptCount;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0] = pe;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1] = (BYTE*)pVideoBuffer;

														for (j = 0; j < quantity_4M_Y_block; j++)
														{

															pVideo->m_Multi_Copy |= (1 << j);

															{

																ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_y[j % quantity_4M_Y_block]);

																if(qw == 0) {

																	LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																	pVideo->m_Multi_Copy &= ~(1 << j);
																}
															}
														}

														for (j = 0; j < quantity_2M_C_block; j++)
														{

															pVideo->m_Multi_Copy |= (1 << (j + quantity_4M_Y_block));

															{

																ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_yv12[j % quantity_2M_C_block]);

																if(qw == 0) {

																	LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work C() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																	pVideo->m_Multi_Copy &= ~(1 << (j + quantity_4M_Y_block));

																}
															}
														}

														spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error, j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);


														if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
													}
												}

												return TRUE;
											}
										}
										else if(cx == 3840 && cy == 2160)
										{
											ULONG data_copied = 0;

											if(pVideo->m_Multi_Copy)
											{
												LINUXV4L2_PRINT(KERN_ERR, "[%02d] pVideo->m_Multi_Copy(0x%x) NOT ZERO - [FAIL]\n", pDevice->m_nKsDeviceNumber, pVideo->m_Multi_Copy);
											}

											if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY))
											{
												if(video_frame <= (cxe * cye * 2))
												{

													if(p_sys_cfg->n_output_video_color_bits == 4)
													{
														ULONG quantity_4M_Y_block = ((cx * cy) + 0x3FFFFF) / 0x400000;
														ULONG quantity_4M_C_block = ((cx * cy / 2) + 0x3FFFFF) / 0x400000;

														if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
														{
															spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);


															pVideo->m_nFrameNumberForQueue = pDevice->m_nVideoInterruptCount;
															pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0] = pe;
															pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1] = (BYTE*)pVideoBuffer;

															for (j = 0; j < quantity_4M_Y_block; j++)
															{

																pVideo->m_Multi_Copy |= (1 << j);

																{

																	ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_y[j % quantity_4M_Y_block]);

																	if(qw == 0) {

																		LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																		pVideo->m_Multi_Copy &= ~(1 << j);
																	}
																}
															}
															for (j = 0; j < quantity_4M_C_block; j++)
															{

																pVideo->m_Multi_Copy |= (1 << (j + quantity_4M_Y_block));

																{

																	ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_nv12[j % quantity_4M_C_block]);

																	if(qw == 0) {

																		LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work C() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																		pVideo->m_Multi_Copy &= ~(1 << (j + quantity_4M_Y_block));

																	}
																}
															}

															spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
														}
														else
														{
															LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error, j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);


															if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);

														}

													}
													else
													{


														quantity_4M_block = (video_frame + 0x3FFFFF) / 0x400000;

														if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
														{

															if(pDevice->debug[0]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] start queue_work\n", pDevice->m_nKsDeviceNumber);

															spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);


															pVideo->m_nFrameNumberForQueue = pDevice->m_nVideoInterruptCount;
															pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0] = pe;
															pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1] = (BYTE*)pVideoBuffer;

															for (j = 0; j < quantity_4M_block; j++)
															{

																pVideo->m_Multi_Copy |= (1 << j);

																{

																	ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_yuyv_bgr[j % quantity_4M_block]);

																	if(qw == 0) {

																		LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																		pVideo->m_Multi_Copy &= ~(1 << j);
																	}
																}
															}

															spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
														}
														else
														{
															LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error, j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);


															if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
														}
													}
												}

												return TRUE;
											}
											else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P016)
											{
											}
											else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010)
											{
												if(video_frame <= (cxe * cye * 3))
												{
													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);


														pVideo->m_nFrameNumberForQueue = pDevice->m_nVideoInterruptCount;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0] = pe;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1] = (BYTE*)pVideoBuffer;


														if(p_sys_cfg->n_output_video_color_bits == 1)
														{
															ULONG quantity_4M_block = ((cx * cy * 3) + 0x3FFFFF) / 0x400000;

															for (j = 0; j < quantity_4M_block; j++)
															{

																pVideo->m_Multi_Copy |= (1 << j);

																{

																	ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_yuyv_bgr[j % quantity_4M_block]);

																	if(qw == 0) {

																		LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																		pVideo->m_Multi_Copy &= ~(1 << j);
																	}
																}
															}
														}
														else
														{
															ULONG quantity_4M_block = ((cx * cy * 15 >> 3) + 0x3FFFFF) / 0x400000;

															for (j = 0; j < quantity_4M_block; j++)
															{

																pVideo->m_Multi_Copy |= (1 << j);

																{

																	ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_p010[j % quantity_4M_block]);

																	if(qw == 0) {

																		LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																		pVideo->m_Multi_Copy &= ~(1 << j);
																	}
																}
															}
														}

														spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error, j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);


														if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
													}
												}
												else
												{
													LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range, video_frame(%d)-\n", pDevice->m_nKsDeviceNumber, video_frame);
												}



												return TRUE;
											}
											else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P210)
											{
												ULONG cwo = cx;
												ULONG cwe = cx;
												ULONG cxo = cx;
												BYTE* pe_y = pe;
												BYTE* pe_c = pe + (cwe * cye * 2);

												ULONG n_src_pitch = (cwo * 5) / 4;

												ULONG n_dst_pitch = (cwe * 2);

												ULONG pixels = cxo / 4;

												quantity_4M_block = (((cx * cy * 5) >> 1) + 0x3FFFFF) / 0x400000;

												for (j = 0; j < quantity_4M_block; j++)
												{
													po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block) + 1 + j];

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														ULONG x = 0;

														BYTE* po_y = po;

														ULONG vertical_in_4M_Block = 0;

														ULONG vertical = 0;

														if(j <= 3)
														{
															vertical_in_4M_Block = 873;
														}
														else
														{
															vertical_in_4M_Block = 828;
														}

														for (vertical = 0; vertical < vertical_in_4M_Block; vertical += 3)
														{
															BYTE* po_y_line1 = (BYTE*)(po_y);

															BYTE* po_y_line2 = (BYTE*)(po_y + n_src_pitch);

															BYTE* po_c_line1 = (BYTE*)(po_y_line2 + n_src_pitch);

															USHORT* pe_y_line1 = (USHORT*)(pe_y);

															USHORT* pe_y_line2 = (USHORT*)(pe_y + n_dst_pitch);

															USHORT* pe_c_line1 = (USHORT*)(pe_c);

															for (x = 0; x < pixels; x++) {

																USHORT* p0 = (USHORT*)(po_y_line1 + 0);

																USHORT* p1 = (USHORT*)(po_y_line1 + 1);

																USHORT* p2 = (USHORT*)(po_y_line1 + 2);

																USHORT* p3 = (USHORT*)(po_y_line1 + 3);

																USHORT* p4 = (USHORT*)(po_y_line2 + 0);

																USHORT* p5 = (USHORT*)(po_y_line2 + 1);

																USHORT* p6 = (USHORT*)(po_y_line2 + 2);

																USHORT* p7 = (USHORT*)(po_y_line2 + 3);

																USHORT* c0 = (USHORT*)(po_c_line1 + 0);

																USHORT* c1 = (USHORT*)(po_c_line1 + 1);

																USHORT* c2 = (USHORT*)(po_c_line1 + 2);

																USHORT* c3 = (USHORT*)(po_c_line1 + 3);

																pe_y_line1[0] = (p0[0] & 0x03FF) << 6;

																pe_y_line1[1] = (p1[0] & 0x0FFC) << 4;

																pe_y_line1[2] = (p2[0] & 0x3FF0) << 2;

																pe_y_line1[3] = (p3[0] & 0xFFC0) << 0;

																pe_y_line2[0] = (p4[0] & 0x03FF) << 6;

																pe_y_line2[1] = (p5[0] & 0x0FFC) << 4;

																pe_y_line2[2] = (p6[0] & 0x3FF0) << 2;

																pe_y_line2[3] = (p7[0] & 0xFFC0) << 0;

																pe_c_line1[0] = (c0[0] & 0x03FF) << 6;

																pe_c_line1[1] = (c1[0] & 0x0FFC) << 4;

																pe_c_line1[2] = (c2[0] & 0x3FF0) << 2;

																pe_c_line1[3] = (c3[0] & 0xFFC0) << 0;

																po_y_line1 += 5;

																po_y_line2 += 5;

																po_c_line1 += 5;

																pe_y_line1 += 4;

																pe_y_line2 += 4;

																pe_c_line1 += 4;
															}

															po_y += (n_src_pitch * 3);

															pe_y += (n_dst_pitch * 2);

															pe_c += (n_dst_pitch * 1);
														}
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error,  j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);
													}
												}
												LINUXV4L2_PRINT(KERN_INFO, "[%02d] complete P210 3840x2160\n", pDevice->m_nKsDeviceNumber);
											}
											else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12)
											{
												if(video_frame <= ((cxe * cye * 12) >> 3))
												{
													ULONG quantity_4M_Y_block = ((cx * cy) + 0x3FFFFF) / 0x400000;
													ULONG quantity_4M_C_block = ((cx * cy / 2) + 0x3FFFFF) / 0x400000;

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);


														pVideo->m_nFrameNumberForQueue = pDevice->m_nVideoInterruptCount;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0] = pe;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1] = (BYTE*)pVideoBuffer;

														for (j = 0; j < quantity_4M_Y_block; j++)
														{

															pVideo->m_Multi_Copy |= (1 << j);

															{

																ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_y[j % quantity_4M_Y_block]);

																if(qw == 0) {

																	LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																	pVideo->m_Multi_Copy &= ~(1 << j);
																}
															}
														}
														for (j = 0; j < quantity_4M_C_block; j++)
														{

															pVideo->m_Multi_Copy |= (1 << (j + quantity_4M_Y_block));

															{

																ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_nv12[j % quantity_4M_C_block]);

																if(qw == 0) {

																	LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work C() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																	pVideo->m_Multi_Copy &= ~(1 << (j + quantity_4M_Y_block));

																}
															}
														}

														spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error, j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);


														if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
													}
												}

												return TRUE;
											}
											else if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24))
											{
												quantity_4M_block = (cx * cy * 3 + 0x3FFFFF) / 0x400000;

												for (j = 0; j < quantity_4M_block; j++)
												{

													BYTE* p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + ((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block * 32 + j * 32;

													if(((XDMA_RESULT*)p_result)->length > 0x400000)
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] j(%d) length(%d) status(0x%x)\n", pDevice->m_nKsDeviceNumber, j, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);

														continue;
													}

													po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block) + 1 + j];

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														if((data_copied + ((XDMA_RESULT*)p_result)->length) <= (cxe * cye * 3))
														{
															memcpy(pe, po, ((XDMA_RESULT*)p_result)->length);

															pe += ((XDMA_RESULT*)p_result)->length;

															data_copied += ((XDMA_RESULT*)p_result)->length;
														}
														else
														{
															LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range(%d) data_copied(%d)\n", pDevice->m_nKsDeviceNumber, ((XDMA_RESULT*)p_result)->length, data_copied);
														}
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error,  j(%d)-\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);
													}
												}
											}
											else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_M420)
											{
												quantity_4M_block = ((cx * cy * 12 >> 3) + 0x3FFFFF) / 0x400000;

												for (j = 0; j < quantity_4M_block; j++)
												{

													BYTE* p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + ((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block * 32 + j * 32;

													if(((XDMA_RESULT*)p_result)->length > 0x400000)
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] j(%d) length(%d) status(0x%x)\n", pDevice->m_nKsDeviceNumber, j, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);

														continue;
													}

													po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block) + 1 + j];

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														if((data_copied + ((XDMA_RESULT*)p_result)->length) <= (cxe * cye * 3))
														{
															memcpy(pe, po, ((XDMA_RESULT*)p_result)->length);

															pe += ((XDMA_RESULT*)p_result)->length;

															data_copied += ((XDMA_RESULT*)p_result)->length;
														}
														else
														{
															LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range(%d) data_copied(%d)\n", pDevice->m_nKsDeviceNumber, ((XDMA_RESULT*)p_result)->length, data_copied);
														}
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error,  j(%d)-\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);
													}
												}
											}
											else
											{
												ULONG quantity_2M_C_block = ((cx * cy / 2) + 0x1FFFFF) / 0x200000;

												if(video_frame <= ((cxe * cye * 12) >> 3))
												{
													ULONG quantity_4M_Y_block = ((cx * cy) + 0x3FFFFF) / 0x400000;

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);


														pVideo->m_nFrameNumberForQueue = pDevice->m_nVideoInterruptCount;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0] = pe;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1] = (BYTE*)pVideoBuffer;

														for (j = 0; j < quantity_4M_Y_block; j++)
														{

															pVideo->m_Multi_Copy |= (1 << j);

															{

																ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_y[j % quantity_4M_Y_block]);

																if(qw == 0) {

																	LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																	pVideo->m_Multi_Copy &= ~(1 << j);
																}
															}
														}

														for (j = 0; j < quantity_2M_C_block; j++)
														{

															pVideo->m_Multi_Copy |= (1 << (j + quantity_4M_Y_block));

															{

																ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_yv12[j % quantity_2M_C_block]);
																if(qw == 0) {

																	LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work C() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																	pVideo->m_Multi_Copy &= ~(1 << (j + quantity_4M_Y_block));

																}
															}
														}
														spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error, j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);


														if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
													}
												}

												return TRUE;
											}
										}
										else if((cx == 2048 && cy == 2560) || (cx == 2560 && cy == 2048))
										{
											ULONG data_copied = 0;

											if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY))
											{
												quantity_4M_block = ((cx * cy * 2) + 0x3FFFFF) / 0x400000;

												for (j = 0; j < quantity_4M_block; j++)
												{

													BYTE* p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + ((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block * 32 + j * 32;

													if(((XDMA_RESULT*)p_result)->length > 0x400000)
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] j(%d) length(%d) status(0x%x)-\n", pDevice->m_nKsDeviceNumber, j, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);

														continue;
													}

													po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block) + 1 + j];

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														if((data_copied + ((XDMA_RESULT*)p_result)->length) <= (cxe * cye * 2))
														{
															memcpy(pe, po, ((XDMA_RESULT*)p_result)->length);

															pe += ((XDMA_RESULT*)p_result)->length;

															data_copied += ((XDMA_RESULT*)p_result)->length;
														}
														else
														{
															LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range(%d) data_copied(%d)-\n", pDevice->m_nKsDeviceNumber, ((XDMA_RESULT*)p_result)->length, data_copied);
														}
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error,  j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);
													}
												}
											}
											if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24))
											{
												quantity_4M_block = ((cx * cy * 3) + 0x3FFFFF) / 0x400000;

												for (j = 0; j < quantity_4M_block; j++)
												{
													BYTE* p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + ((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block * 32 + j * 32;

													if(((XDMA_RESULT*)p_result)->length > 0x400000)
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] j(%d) length(%d) status(0x%x)-\n", pDevice->m_nKsDeviceNumber, j, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);

														continue;
													}

													po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block) + 1 + j];

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														if((data_copied + ((XDMA_RESULT*)p_result)->length) <= (cxe * cye * 3))
														{
															memcpy(pe, po, ((XDMA_RESULT*)p_result)->length);

															pe += ((XDMA_RESULT*)p_result)->length;

															data_copied += ((XDMA_RESULT*)p_result)->length;
														}
														else
														{
															LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range(%d) data_copied(%d)-\n", pDevice->m_nKsDeviceNumber, ((XDMA_RESULT*)p_result)->length, data_copied);
														}
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error,  j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);
													}
												}
											}
										}
										else if(cx == 2048 && cy == 2048)
										{
											ULONG data_copied = 0;

											if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY))
											{
												quantity_4M_block = ((cx * cy * 2) + 0x3FFFFF) / 0x400000;

												for (j = 0; j < quantity_4M_block; j++)
												{

													BYTE* p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + ((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block * 32 + j * 32;



													if(((XDMA_RESULT*)p_result)->status & 1)
													{
														if(j != 1)
														{
															LINUXV4L2_DEBUG(KERN_INFO, "[%02d] j(%d) length(%d) status(0x%x)-\n", pDevice->m_nKsDeviceNumber, j, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);
														}
													}




													if(((XDMA_RESULT*)p_result)->length > 0x400000)
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] j(%d) length(%d) status(0x%x)-\n", pDevice->m_nKsDeviceNumber, j, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);

														continue;
													}





													po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block) + 1 + j];

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														if((data_copied + ((XDMA_RESULT*)p_result)->length) <= (cxe * cye * 2))
														{
															memcpy(pe, po, ((XDMA_RESULT*)p_result)->length);

															pe += ((XDMA_RESULT*)p_result)->length;

															data_copied += ((XDMA_RESULT*)p_result)->length;
														}
														else
														{
															LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range(%d) data_copied(%d)-\n", pDevice->m_nKsDeviceNumber, ((XDMA_RESULT*)p_result)->length, data_copied);
														}
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error,  j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);
													}
												}
											}
											else if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24))
											{
												quantity_4M_block = ((cx * cy * 3) + 0x3FFFFF) / 0x400000;

												for (j = 0; j < quantity_4M_block; j++)
												{


													BYTE* p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + ((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block * 32 + j * 32;






													if(((XDMA_RESULT*)p_result)->length > 0x400000)
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] j(%d) length(%d) status(0x%x)-\n", pDevice->m_nKsDeviceNumber, j, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);

														continue;
													}





													po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block) + 1 + j];

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														if((data_copied + ((XDMA_RESULT*)p_result)->length) <= (cxe * cye * 3))
														{
															memcpy(pe, po, ((XDMA_RESULT*)p_result)->length);

															pe += ((XDMA_RESULT*)p_result)->length;

															data_copied += ((XDMA_RESULT*)p_result)->length;
														}
														else
														{
															LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range(%d) data_copied(%d)-\n", pDevice->m_nKsDeviceNumber, ((XDMA_RESULT*)p_result)->length, data_copied);
														}
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error,  j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);
													}
												}
											}
											else
											{
												quantity_4M_block = (((cx * cy * 12) >> 3) + 0x3FFFFF) / 0x400000;

												LINUXV4L2_PRINT(KERN_INFO, "[%02d]	not implemented yet\n", pDevice->m_nKsDeviceNumber);
											}
										}
										else if(p_sys_cfg->n_output_video_bufsize > 0x400000)
										{
											ULONG data_copied = 0;

											if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY))
											{
												quantity_4M_block = ((cx * cy * 2) + 0x3FFFFF) / 0x400000;

												for (j = 0; j < quantity_4M_block; j++)
												{

													BYTE* p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + ((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block * 32 + j * 32;



													if(((XDMA_RESULT*)p_result)->status & 1)
													{
														if(j != 1)
														{
															LINUXV4L2_DEBUG(KERN_INFO, "[%02d] j(%d) length(%d) status(0x%x)-\n", pDevice->m_nKsDeviceNumber, j, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);
														}
													}




													if(((XDMA_RESULT*)p_result)->length > 0x400000)
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] j(%d) length(%d) status(0x%x)-\n", pDevice->m_nKsDeviceNumber, j, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);

														continue;
													}

													po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block) + 1 + j];

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														if((data_copied + ((XDMA_RESULT*)p_result)->length) <= (cxe * cye * 2))
														{
															memcpy(pe, po, ((XDMA_RESULT*)p_result)->length);

															pe += ((XDMA_RESULT*)p_result)->length;

															data_copied += ((XDMA_RESULT*)p_result)->length;
														}
														else
														{
															LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range(%d) data_copied(%d)-\n", pDevice->m_nKsDeviceNumber, ((XDMA_RESULT*)p_result)->length, data_copied);
														}
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error,  j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);
													}
												}
											}
											else if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24))
											{
												quantity_4M_block = ((cx * cy * 3) + 0x3FFFFF) / 0x400000;

												for (j = 0; j < quantity_4M_block; j++)
												{


													BYTE* p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + ((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block * 32 + j * 32;






													if(((XDMA_RESULT*)p_result)->length > 0x400000)
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] j(%d) length(%d) status(0x%x)-\n", pDevice->m_nKsDeviceNumber, j, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);

														continue;
													}

													po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block) + 1 + j];

													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														if((data_copied + ((XDMA_RESULT*)p_result)->length) <= (cxe * cye * 3))
														{
															memcpy(pe, po, ((XDMA_RESULT*)p_result)->length);

															pe += ((XDMA_RESULT*)p_result)->length;

															data_copied += ((XDMA_RESULT*)p_result)->length;
														}
														else
														{
															LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range(%d) data_copied(%d)-\n", pDevice->m_nKsDeviceNumber, ((XDMA_RESULT*)p_result)->length, data_copied);
														}
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error,  j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);
													}
												}

											}
											else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010)
											{
												if(video_frame <= (cxe * cye * 3))
												{
													if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
													{
														spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);


														pVideo->m_nFrameNumberForQueue = pDevice->m_nVideoInterruptCount;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0] = pe;
														pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1] = (BYTE*)pVideoBuffer;


														if(p_sys_cfg->n_output_video_color_bits == 1)
														{
															ULONG quantity_4M_block = ((cx * cy * 3) + 0x3FFFFF) / 0x400000;

															for (j = 0; j < quantity_4M_block; j++)
															{

																pVideo->m_Multi_Copy |= (1 << j);

																{

																	ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_yuyv_bgr[j % quantity_4M_block]);

																	if(qw == 0) {

																		LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																		pVideo->m_Multi_Copy &= ~(1 << j);
																	}
																}
															}
														}
														else
														{
															ULONG quantity_4M_block = ((cx * cy * 15 >> 3) + 0x3FFFFF) / 0x400000;

															for (j = 0; j < quantity_4M_block; j++)
															{

																pVideo->m_Multi_Copy |= (1 << j);

																{

																	ULONG qw = queue_work(pVideo->wq_blkcpy, &pVideo->w_blkcpy_p010[j % quantity_4M_block]);

																	if(qw == 0) {

																		LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() j(0x%x)- [FAIL]\n", pDevice->m_nKsDeviceNumber, j);

																		pVideo->m_Multi_Copy &= ~(1 << j);
																	}
																}
															}
														}

														spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
													}
													else
													{
														LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error, j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);


														if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
													}
												}
												else
												{
													LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range, video_frame(%d)-\n", pDevice->m_nKsDeviceNumber, video_frame);
												}



												return TRUE;

											}
											else
											{
												LINUXV4L2_PRINT(KERN_INFO, "[%02d]	not implemented yet\n", pDevice->m_nKsDeviceNumber);
											}
										}
										else
										{
											BYTE* po_previous = pDevice->m_pDmaBaseCommonBuffer[((pDevice->m_nVideoInterruptCount + 2) % 4) + 1];

											po = pDevice->m_pDmaBaseCommonBuffer[((pDevice->m_nVideoInterruptCount - 1) % 4) + 1];



											if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
											{
												if(interleaved)
												{


													if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY))
													{


#ifdef INTERLACE_FIELD_MODE

														if(vb2_get_plane_payload(&pVideoBuffer->vb2_buffer_cross, 0) == cx * cy * 2)
														{
															memcpy(pe, po, cx * cy * 2);
														}

#else

														ULONG vertical = 0;

														BYTE* p_dst = pe;
														if(pDevice->m_nVideoInterruptCount == 1)
														{
															for (vertical = 0; vertical < cy / 2; vertical++)
															{
																memcpy(p_dst, po, cx * 2);

																p_dst += cx * 2;

																memcpy(p_dst, po, cx * 2);

																p_dst += cx * 2;

																po += (cx * 2);
															}
														}
														else
														{
															ULONG yuy2_line_size = cx * 2;

															if(po[0] & 0x01)
															{
																for (vertical = 0; vertical < cy / 2; vertical++)
																{
																	memcpy(p_dst, po_previous, yuy2_line_size);

																	p_dst += yuy2_line_size;

																	memcpy(p_dst, po, yuy2_line_size);

																	p_dst += yuy2_line_size;

																	po += yuy2_line_size;

																	po_previous += yuy2_line_size;
																}
															}
															else
															{
																for (vertical = 0; vertical < cy / 2; vertical++)
																{
																	memcpy(p_dst, po, yuy2_line_size);

																	p_dst += yuy2_line_size;

																	memcpy(p_dst, po_previous, yuy2_line_size);

																	p_dst += yuy2_line_size;

																	po += yuy2_line_size;

																	po_previous += yuy2_line_size;
																}
															}
														}

#endif
													}
													else if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24))
													{


														ULONG vertical = 0;

														if(pDevice->m_nVideoInterruptCount == 1)
														{
															for (vertical = 0; vertical < cy / 2; vertical++)
															{
																memcpy(pe, po, cx * 3);

																pe += cx * 3;

																memcpy(pe, po, cx * 3);

																pe += cx * 3;

																po += (cx * 3);
															}
														}
														else
														{
															ULONG yuy2_line_size = cx * 3;

															if(po[0] & 0x01)
															{
																for (vertical = 0; vertical < cy / 2; vertical++)
																{
																	memcpy(pe, po_previous, yuy2_line_size);

																	pe += yuy2_line_size;

																	memcpy(pe, po, yuy2_line_size);

																	pe += yuy2_line_size;

																	po += yuy2_line_size;

																	po_previous += yuy2_line_size;
																}
															}
															else
															{
																for (vertical = 0; vertical < cy / 2; vertical++)
																{
																	memcpy(pe, po, yuy2_line_size);

																	pe += yuy2_line_size;

																	memcpy(pe, po_previous, yuy2_line_size);

																	pe += yuy2_line_size;

																	po += yuy2_line_size;

																	po_previous += yuy2_line_size;
																}
															}
														}
													}
													else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12)
													{
														ULONG vertical = 0;
														BYTE* p_dst = pe;

														if(pDevice->m_nVideoInterruptCount == 1)
														{
														}
														else
														{


															BYTE* src_po = po;

															BYTE* src_po_previous = po_previous;

															if(po[0] & 0x01)
															{



																for (vertical = 0; vertical < (cy * 3) / 4; vertical++)
																{
																	if((vertical % 3) == 2)
																	{
																	}
																	else
																	{
																		src_po_previous = po_previous + cx * vertical;

																		src_po = po + cx * vertical;

																		memcpy(p_dst, src_po_previous, cx);

																		p_dst += cx;

																		memcpy(p_dst, src_po, cx);

																		p_dst += cx;
																	}
																}


																for (vertical = 0; vertical < (cy * 3) / 4; vertical++)
																{
																	BYTE* src = po;

																	if((vertical % 3) == 2)
																	{
																		src = po_previous + cx * vertical;

																		memcpy(p_dst, src, cx);

																		p_dst += cx;

																		src = po + cx * vertical;

																		memcpy(p_dst, src, cx);

																		p_dst += cx;
																	}
																}
															}
															else
															{



																for (vertical = 0; vertical < (cy * 3) / 4; vertical++)
																{
																	if((vertical % 3) == 2)
																	{
																	}
																	else
																	{
																		src_po_previous = po + cx * vertical;

																		src_po = po_previous + cx * vertical;

																		memcpy(p_dst, src_po_previous, cx);

																		p_dst += cx;

																		memcpy(p_dst, src_po, cx);

																		p_dst += cx;
																	}
																}


																for (vertical = 0; vertical < (cy * 3) / 4; vertical++)
																{
																	BYTE* src = po;

																	if((vertical % 3) == 2)
																	{
																		src = po + cx * vertical;

																		memcpy(p_dst, src, cx);

																		p_dst += cx;

																		src = po_previous + cx * vertical;

																		memcpy(p_dst, src, cx);

																		p_dst += cx;
																	}
																}
															}
														}

													}
													else
													{
														ULONG vertical = 0;


														if(pDevice->m_nVideoInterruptCount == 1)
														{
														}
														else
														{
															BYTE* src_po = po;

															BYTE* src_po_previous = po_previous;

															if(po[0] & 0x01)
															{
																for (vertical = 0; vertical < (cy * 3) / 4; vertical++)
																{
																	if((vertical % 3) == 2)
																	{
																	}
																	else
																	{
																		src_po_previous = po_previous + cx * vertical;

																		src_po = po + cx * vertical;

																		memcpy(pe, src_po_previous, cx);

																		pe += cx;

																		memcpy(pe, src_po, cx);

																		pe += cx;
																	}
																}


																for (vertical = 0; vertical < (cy * 3) / 4; vertical++)
																{
																	if((vertical % 3) == 2)
																	{
																		ULONG horizontal = 0;

																		BYTE* src = po_previous + cx * vertical;

																		if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
																		{
																			src = po + cx * vertical;
																		}
																		else
																		{
																			src = po + cx * vertical + 1;
																		}

																		for (horizontal = 0; horizontal < cx; horizontal += 2)
																		{
																			*pe++ = *src;

																			src += 2;
																		}

																		src = po + cx * vertical;

																		if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
																		{
																			src = po_previous + cx * vertical;
																		}
																		else
																		{
																			src = po_previous + cx * vertical + 1;
																		}

																		for (horizontal = 0; horizontal < cx; horizontal += 2)
																		{
																			*pe++ = *src;

																			src += 2;
																		}
																	}
																}

																for (vertical = 0; vertical < (cy * 3) / 4; vertical++)
																{
																	if((vertical % 3) == 2)
																	{
																		ULONG horizontal = 0;

																		BYTE* src = po_previous + cx * vertical;

																		if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
																		{
																			src = po + cx * vertical + 1;
																		}
																		else
																		{
																			src = po + cx * vertical;
																		}

																		for (horizontal = 0; horizontal < cx; horizontal += 2)
																		{
																			*pe++ = *src;

																			src += 2;
																		}

																		src = po + cx * vertical;

																		if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
																		{
																			src = po_previous + cx * vertical + 1;
																		}
																		else
																		{
																			src = po_previous + cx * vertical;
																		}

																		for (horizontal = 0; horizontal < cx; horizontal += 2)
																		{
																			*pe++ = *src;

																			src += 2;
																		}
																	}
																}
															}
															else
															{

																for (vertical = 0; vertical < (cy * 3) / 4; vertical++)
																{
																	if((vertical % 3) == 2)
																	{
																	}
																	else
																	{
																		src_po_previous = po_previous + cx * vertical;

																		src_po = po + cx * vertical;

																		memcpy(pe, src_po, cx);

																		pe += cx;

																		memcpy(pe, src_po_previous, cx);

																		pe += cx;
																	}
																}


																for (vertical = 0; vertical < (cy * 3) / 4; vertical++)
																{
																	if((vertical % 3) == 2)
																	{
																		ULONG horizontal = 0;

																		BYTE* src = po + cx * vertical;

																		if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
																		{
																			src = po_previous + cx * vertical;
																		}
																		else
																		{
																			src = po_previous + cx * vertical + 1;
																		}

																		for (horizontal = 0; horizontal < cx; horizontal += 2)
																		{
																			*pe++ = *src;

																			src += 2;
																		}

																		src = po_previous + cx * vertical;

																		if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
																		{
																			src = po + cx * vertical;
																		}
																		else
																		{
																			src = po + cx * vertical + 1;
																		}

																		for (horizontal = 0; horizontal < cx; horizontal += 2)
																		{
																			*pe++ = *src;

																			src += 2;
																		}
																	}
																}

																for (vertical = 0; vertical < (cy * 3) / 4; vertical++)
																{
																	if((vertical % 3) == 2)
																	{
																		ULONG horizontal = 0;

																		BYTE* src = po + cx * vertical;

																		if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
																		{
																			src = po_previous + cx * vertical + 1;
																		}
																		else
																		{
																			src = po_previous + cx * vertical;
																		}

																		for (horizontal = 0; horizontal < cx; horizontal += 2)
																		{
																			*pe++ = *src;

																			src += 2;
																		}

																		src = po_previous + cx * vertical;

																		if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
																		{
																			src = po + cx * vertical + 1;
																		}
																		else
																		{
																			src = po + cx * vertical;
																		}

																		for (horizontal = 0; horizontal < cx; horizontal += 2)
																		{
																			*pe++ = *src;

																			src += 2;
																		}
																	}
																}
															}
														}
													}
												}
												else
												{
													if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY))
													{
														ULONG cwo = cx;

														ULONG vertical = 0;

														if(cwo % 4) {

															cwo /= 4;

															cwo = 4 * (cwo + 1);


															for (vertical = 0; vertical < cy; vertical++)
															{
																memcpy(pe, po, cx * 2);

																pe += cx * 2;

																po += cwo * 2;
															}
														}
														else
														{
															memcpy(pe, po, vb2_get_plane_payload(&pVideoBuffer->vb2_buffer_cross, 0));
														}
													}
													else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12)
													{


														ULONG vertical = 0;

														BYTE* src = po;

														ULONG cwo = cx;



														if(cwo % 4) {

															cwo /= 4;

															cwo = 4 * (cwo + 1);


															for (vertical = 0; vertical < ((cy * 12) >> 3); vertical += 3)
															{
																memcpy(pe, src, cx * 2);

																pe += cx * 2;

																src += cwo * 3;
															}

															src = po + cwo * 2;

															for (vertical = 0; vertical < ((cy * 12) >> 3); vertical += 3)
															{
																memcpy(pe, src, cx);

																pe += cx;

																src += cwo * 3;
															}

														}
														else
														{
															BYTE* p_dst = pe;

															for (vertical = 0; vertical < ((cy * 12) >> 3); vertical += 3)
															{
																memcpy(p_dst, src, cx * 2);

																p_dst += cx * 2;

																src += cx * 3;
															}

															src = po + cx * 2;

															for (vertical = 0; vertical < ((cy * 12) >> 3); vertical += 3)
															{
																memcpy(p_dst, src, cx);

																p_dst += cx;

																src += cx * 3;
															}
														}
													}
													else if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24))
													{
														ULONG data_copied = 0;

														quantity_4M_block = ((cx * cy * 3) + 0x3FFFFF) / 0x400000;

														for (j = 0; j < quantity_4M_block; j++)
														{

															BYTE* p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + ((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block * 32 + j * 32;

															if(((XDMA_RESULT*)p_result)->length > 0x400000)
															{
																LINUXV4L2_PRINT(KERN_INFO, "[%02d] j(%d) length(%d) status(0x%x)\n", pDevice->m_nKsDeviceNumber, j, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);

																continue;
															}

															po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block) + 1 + j];

															if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
															{
																if((data_copied + ((XDMA_RESULT*)p_result)->length) <= (cxe * cye * 3))
																{
																	memcpy(pe, po, ((XDMA_RESULT*)p_result)->length);

																	pe += ((XDMA_RESULT*)p_result)->length;

																	data_copied += ((XDMA_RESULT*)p_result)->length;
																}
																else
																{
																	LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range(%d) data_copied(%d)\n", pDevice->m_nKsDeviceNumber, ((XDMA_RESULT*)p_result)->length, data_copied);
																}
															}
															else
															{
																LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error,  j(%d)-\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);
															}
														}
													}
													else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010)
													{

														if(video_frame <= (cxe * cye * 3))
														{
															if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
															{

																if(p_sys_cfg->n_output_video_color_bits == 1)
																{
																	memcpy(pe, po, vb2_get_plane_payload(&pVideoBuffer->vb2_buffer_cross, 0));
																}
																else
																{
																	LINUXV4L2_PRINT(KERN_INFO, "[%02d]	not implemented yet\n", pDevice->m_nKsDeviceNumber);
																}
															}
															else
															{
																LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error, j(%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, j);
															}
														}
														else
														{
															LINUXV4L2_PRINT(KERN_INFO, "[%02d] length out of range, video_frame(%d)-\n", pDevice->m_nKsDeviceNumber, video_frame);
														}



													}
													else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_M420)
													{




														memcpy(pe, po, vb2_get_plane_payload(&pVideoBuffer->vb2_buffer_cross, 0));
													}
													else
													{


														ULONG vertical = 0;

														BYTE* src = po;

														ULONG cwo = cx;



														if(cwo % 4) {

															cwo /= 4;

															cwo = 4 * (cwo + 1);


															for (vertical = 0; vertical < ((cy * 12) >> 3); vertical += 3)
															{
																memcpy(pe, src, cx * 2);

																pe += cx * 2;

																src += cwo * 3;
															}


															for (vertical = 0; vertical < ((cy * 12) >> 3); vertical += 3)
															{
																ULONG horizontal = 0;

																if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
																{
																	src = po + cwo * (vertical + 2);
																}
																else
																{
																	src = po + cwo * (vertical + 2) + 1;
																}

																for (horizontal = 0; horizontal < cx; horizontal += 2)
																{
																	*pe++ = *src;

																	src += 2;
																}
																}


															for (vertical = 0; vertical < ((cy * 12) >> 3); vertical += 3)
															{
																ULONG horizontal = 0;

																if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
																{
																	src = po + cwo * (vertical + 2) + 1;
																}
																else
																{
																	src = po + cwo * (vertical + 2);
																}

																for (horizontal = 0; horizontal < cx; horizontal += 2)
																{
																	*pe++ = *src;

																	src += 2;
																}
															}
															}
														else
														{
															BYTE* p_dst = pe;


															for (vertical = 0; vertical < ((cy * 12) >> 3); vertical += 3)
															{
																memcpy(p_dst, src, cx * 2);

																p_dst += cx * 2;

																src += cx * 3;
															}



															for (vertical = 0; vertical < ((cy * 12) >> 3); vertical++)
															{
																ULONG horizontal = 0;

																if((vertical % 3) == 2)
																{
																	if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
																	{
																		src = po + cx * vertical;
																	}
																	else
																	{
																		src = po + cx * vertical + 1;
																	}

																	for (horizontal = 0; horizontal < cx; horizontal += 2)
																	{
																		*p_dst++ = *src;

																		src += 2;
																	}
																}
															}


															for (vertical = 0; vertical < ((cy * 12) >> 3); vertical++)
															{
																ULONG horizontal = 0;

																if((vertical % 3) == 2)
																{
																	if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
																	{
																		src = po + cx * vertical + 1;
																	}
																	else
																	{
																		src = po + cx * vertical;
																	}

																	for (horizontal = 0; horizontal < cx; horizontal += 2)
																	{
																		*p_dst++ = *src;

																		src += 2;
																	}
																}
															}

														}
													}
												}
											}
											else
											{
												LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error,  \n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state);
											}
										}
									}
								}
							}
							else
							{
								LINUXV4L2_PRINT(KERN_INFO, "[%02d] too small \n", pDevice->m_nKsDeviceNumber);
							}
						}
					}
				}


				is_completes = TRUE;
			}

			if(is_completes)
			{

				if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
				{
					ULONGLONG diff = 0;

					ULONG fps = 0;

					ULONGLONG now_ts = 0;

					#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
					pVideoBuffer->vb.vb2_buf.timestamp = ktime_get_ns();
					#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
					do_gettimeofday(&pVideoBuffer->vb.timestamp);
					#else
					do_gettimeofday(&pVideoBuffer->vb.v4l2_buf.timestamp);
					#endif

					#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
					now_ts = pVideoBuffer->vb.vb2_buf.timestamp / 1000;
					#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
					now_ts = pVideoBuffer->vb.timestamp.tv_sec * 1000000 + pVideoBuffer->vb.timestamp.tv_usec;
					#else
					now_ts = pVideoBuffer->vb.v4l2_buf.timestamp.tv_sec * 1000000 + pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec;
					#endif

					p_sys_cfg->n_output_video_stream_frame_number[ 0 ]++;

					diff = now_ts - pDevice->previous_ts_preview[pVideo->m_nSubDeviceNumber];

					if((pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF) == 0x77FFFFFF)
					{
						fps = real_fps;
					}
					else
					{
						fps = pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF;
					}

					if(pDevice->m_nTaskletExtraParameterC & 0x00000080)
					{
						pDevice->m_nTaskletExtraParameterC &= ~0x00000080;
					}
					else
					{
						if(fps > 1)
						{
							if((ABS(diff) > ((1000000 + fps * 5000) / fps)) || (ABS(diff) < ((1000000 - fps * 5000) / fps)))
							{
								if(pDevice->m_nAnalogVideoDecoderStatusProperty) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] diff(llu%llu)(us) s_parm(%d) real_fps(%d)--\n", pDevice->m_nKsDeviceNumber, ABS(diff), fps, real_fps);
							}
						}
					}
					pDevice->previous_ts_preview[pVideo->m_nSubDeviceNumber] = now_ts;

					update_video_time_stamp(pDevice, pVideo, pVideoBuffer);


					#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
					pVideoBuffer->vb.sequence = pVideo->m_nFrameNumber++;

					#else
					pVideoBuffer->vb.v4l2_buf.sequence = pVideo->m_nFrameNumber++;
					#endif

					#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
					pVideoBuffer->vb.field = pDevice->m_Video_field;
					#else
					pVideoBuffer->vb.v4l2_buf.field = pDevice->m_Video_field;
					#endif



					vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_DONE);

					if( pDevice->debug[0] ) LINUXV4L2_PRINT( KERN_INFO, "[%02d] finish a frame, sequence(0d%d)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->vb.sequence );

					if((pDevice->m_nVideoInterruptCount % 10000) == 0)
					{
						if(pDevice->debug[1])
						{
							ULONG fps = pDevice->m_nCustomAnalogVideoFrameRateProperty;

							ULONG freq = pDevice->m_nCustomAnalogAudioSampleFrequencyProperty;

							LINUXV4L2_DEBUG(KERN_INFO, "[%02d] VIDEO(llu%llu) AUDIO(llu%llu) from ISR\n", pDevice->m_nKsDeviceNumber, pDevice->m_nVideoInterruptCount, pDevice->m_nAudioInterruptCount);

							if(fps > 20 && freq)
							{
								if(INT_BYTES_A == 4096) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] V - A (%lld) ms from ISR\n", pDevice->m_nKsDeviceNumber, ( pDevice->m_nVideoInterruptCount * (1000 + p_sys_cfg->n_input_video_resolution_fps_m) / fps ) - ( pDevice->m_nAudioInterruptCount * 64 * 480 / (3*freq/100)) );
								if(INT_BYTES_A == 1920) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] V - A (%lld) ms from ISR\n", pDevice->m_nKsDeviceNumber, (pDevice->m_nVideoInterruptCount * (1000 + p_sys_cfg->n_input_video_resolution_fps_m) / fps) - (pDevice->m_nAudioInterruptCount * 10 * 480 / (freq / 100)));
							}
						}
					}
				}
				else
				{
					LINUXV4L2_PRINT(KERN_ERR, "[%02d] pe state error(%x) ==\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state);

					if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
				}
			}
		}
	}

#endif

	return TRUE;
}


void SC0710_HwProcessAnalogRawAudioFrame( struct work_struct *work )
{
	struct CDevice* pDevice = container_of(work, struct CDevice, w_audio);

	CAudio * pAudio = NULL;

	CAudio * pAudios[ MAX_SUB_DEVICE_NUM_X_2 ] = { NULL };

	BYTE* po = NULL;

	BYTE* pe = NULL;

	ULONG show_signal_audio = 0;

	ULONG i = 0;

	ULONG j = 0;

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	ULONG status = pDevice->m_nTaskletExtraParameter_a;



	pDevice->m_nTaskletExtraParameter_a = 0;

	memset( pAudios, 0x00, MAX_SUB_DEVICE_NUM_X_2 * sizeof(CAudio*) );

	for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

		if( pDevice->m_eAudioStreamState[ i ] == 0x00000000 ) { continue ; }

		if( pDevice->m_pAudio[ i ] == NULL ) { continue ; }

		pAudios[ i ] = pDevice->m_pAudio[ i ];
	}

	if(status & 0x00000010) pDevice->m_nDmaBaseCommonBufferNumber[1]++;

	if(pDevice->m_InNoAudioDPC)
	{
		show_signal_audio = 0;
	}
	else
	{
		show_signal_audio = 1;
	}

	if(show_signal_audio == 0) return;

	if((status & 0x00000010) || (status == 0xEEEEEEEE)) {


		for( i = 0; i < pDevice->m_nKsDeviceAudioNumber; i++ )
		{
			ULONG ch = 0;

			ULONG fps = 0;

			ULONG freq = 48000;

			ULONG video_arrived = 0;

			pAudio = pAudios[ i ];

			if( pAudio == NULL )
			{
				continue;
			}

			ch = i;

			fps = p_sys_cfg->n_input_video_resolution_fpss[ch];

			freq = p_sys_cfg->n_input_audio_sampling_frequencys[ch];

			if(freq == 0)
			{
				freq = 48000;
			}


			if((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D))
			{
				video_arrived = 1;
			}

			if(pDevice->m_pVideo[i])
			{
				CVideo* pVideo = pDevice->m_pVideo[i];

				if(pVideo->m_nFrameNumber)
				{
					video_arrived = 1;
				}
			}

			j = pDevice->m_nAudioInterruptCount;

			if((pDevice->m_eAudioStreamState[i] & 1) && video_arrived)
			{

				switch (j % 8) {

					case 0: po = pDevice->m_pDmaBaseCommonBuffer[130] + (INT_BYTES_A * 8 * 7); break;

					case 1: po = pDevice->m_pDmaBaseCommonBuffer[130] + (INT_BYTES_A * 8 * 0); break;

					case 2: po = pDevice->m_pDmaBaseCommonBuffer[130] + (INT_BYTES_A * 8 * 1); break;

					case 3: po = pDevice->m_pDmaBaseCommonBuffer[130] + (INT_BYTES_A * 8 * 2); break;

					case 4: po = pDevice->m_pDmaBaseCommonBuffer[130] + (INT_BYTES_A * 8 * 3); break;

					case 5: po = pDevice->m_pDmaBaseCommonBuffer[130] + (INT_BYTES_A * 8 * 4); break;

					case 6: po = pDevice->m_pDmaBaseCommonBuffer[130] + (INT_BYTES_A * 8 * 5); break;

					case 7: po = pDevice->m_pDmaBaseCommonBuffer[130] + (INT_BYTES_A * 8 * 6); break;
				}


				if(pDevice->m_nCustomAnalogAudio8chI2sEnableProperty)
				{
					p_sys_cfg->n_output_audio_stream_buffer_size_ai = LINUXV4L2_SND_PCM_HARDWARE_SC0710.period_bytes_min * 4;

					p_sys_cfg->n_output_audio_stream_buffer_size_ao = LINUXV4L2_SND_PCM_HARDWARE_SC0710.period_bytes_min * 4;

				}
				else
				{
					ULONG n = 0;

					ULONG size = LINUXV4L2_SND_PCM_HARDWARE_SC0710.period_bytes_min;

					ULONG size_quad = size / 4;

					DWORD* px = (DWORD*)(po + ch * 4);

					DWORD* py = (DWORD*)(pDevice->m_audio_arranged_buffer);

					p_sys_cfg->n_output_audio_stream_buffer_size_ai = LINUXV4L2_SND_PCM_HARDWARE_SC0710.period_bytes_min;

					p_sys_cfg->n_output_audio_stream_buffer_size_ao = LINUXV4L2_SND_PCM_HARDWARE_SC0710.period_bytes_min;


					if((pDevice->bcdDevice == 0x0301) && (pDevice->iManufacturer == 0x17))
					{
						px = (DWORD*)(po + ch * 4);
					}



					if((pDevice->bcdDevice == 0xD004) && (pDevice->iManufacturer == 0x1C))
					{



						SHORT* px_s = (SHORT*)(po + ch * 2);

						SHORT* py_s = (SHORT*)(py);

						if(ch == 1)
						{
							px_s = (SHORT*)(po + ch * 2 + 2);
						}

						if(ch == 2)
						{
							px_s = (SHORT*)(po + ch * 2 - 2);
						}

						for (n = 0; n < size_quad; n++) {




								*py_s++ = *px_s;
								*py_s++ = *px_s;









							px_s += 8;

						}
					}
					else if(((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

						USHORT* p_dst = (USHORT*)(py);

						USHORT* pl = (USHORT*)(po + 0);

						size_quad = (4096 * 8) / 32;

						pl = (USHORT*)(po + ch * 2);

						if((ch == 1) || (ch == 5) || (ch == 9) || (ch == 13)) {

							pl = (USHORT*)(po + ch * 2 + 2);
						}
						if((ch == 2) || (ch == 6) || (ch == 10) || (ch == 14)) {

							pl = (USHORT*)(po + ch * 2 - 2);
						}
						for (n = 0; n < size_quad; n++) {

							*p_dst++ = *pl;

							*p_dst++ = *pl;

							pl += 16;
						}
					}
					else if((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D)) {








						const static DWORD RESERVED_MAGIC_KEY[2] = {0XDD22EE11,0XBB44CC33};

						ULONG isShifted = 0;

						{
							DWORD ch_audio[8];

							DWORD *cc = (DWORD*)(po);

							for ( n = 0; n < 8; n ++ )
							{
								ch_audio[n] = *(cc+n);
							}



							if (ch_audio[2] == RESERVED_MAGIC_KEY[0] && ch_audio[3] == RESERVED_MAGIC_KEY[1])
							{
								isShifted = 1;
							}

						}

						size_quad = 1024;

						{
							ULONG new_ch = (ch + 4 * isShifted) % 8;

							px = (DWORD*)(po + new_ch * 4);

							for (n = 0; n < size_quad; n++)
							{
								*py++ = *px;

								px += 8;
							}
						}
						if (ch == 4 || ch == 5)
						{
							fps = 60;
						}
					}
					else
					{

						for (n = 0; n < size_quad; n++) {

							*py++ = *px;

							px += 4;

						}
					}
					po = pDevice->m_audio_arranged_buffer;
				}

				if(fps && freq)
				{

					LONGLONG v_base_fps = 0;

					LONGLONG a_base_fps = 0;

					LONGLONG v_total_duration = 0;


					LONGLONG a_total_duration = 0;

					LONGLONG diff = 0;

					RtlCopyMemory( p_sys_cfg->p_output_audio_stream_buffer_i2s[ ch ] + p_sys_cfg->n_output_audio_stream_buffer_size[ ch ], po, p_sys_cfg->n_output_audio_stream_buffer_size_ai );

					p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] += p_sys_cfg->n_output_audio_stream_buffer_size_ai;

					v_base_fps = p_sys_cfg->n_output_video_resolution_fps_base_fpss[ ch ];

					if( p_sys_cfg->n_input_video_resolution_interleaveds[ ch ] == 1 )
					{
						v_base_fps /= 2;
					}

					if(v_base_fps)
					{
						v_total_duration = (p_sys_cfg->n_output_video_stream_frame_number[ ch ] * 1000000000) / (v_base_fps);
					}

					a_base_fps = freq;

					if(a_base_fps)
					{
						a_total_duration = (p_sys_cfg->n_output_audio_stream_frame_number[ ch ] * 4096 * 1000000) / (a_base_fps * 4);
					}

					if( p_sys_cfg->n_input_video_resolution_fps_ms[ ch ] == 0 ) {

						LONGLONG delta = (p_sys_cfg->n_output_video_stream_frame_number[ ch ] * 1000000);

						LONGLONG _fps = p_sys_cfg->n_output_video_resolution_fpss[ ch ];

						if(_fps) delta /= _fps;

						v_total_duration = delta;
					}
					else {



						LONGLONG delta = (p_sys_cfg->n_output_video_stream_frame_number[ ch ] * 1000000);

						LONGLONG _fps = p_sys_cfg->n_output_video_resolution_fpss[ ch ];

						delta *= 1001;



						if(_fps) delta /= (_fps * 1000);

						v_total_duration = delta;
					}

					if(a_base_fps)
					{
						if(pDevice->m_nCustomAnalogAudio8chI2sEnableProperty)
						{
							a_total_duration += (p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] * (1000000 / 4)) / (a_base_fps * 4);
						}
						else
						{
							a_total_duration += (p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] * 1000000) / (a_base_fps * 4);
						}
					}

					diff = (v_total_duration > a_total_duration) ? (v_total_duration - a_total_duration) : (a_total_duration - v_total_duration);

					if( v_total_duration > 60000000 ) {

						if( p_sys_cfg->n_output_audio_stream_1mins_diff_duration[ ch ] == 0 ) {

							p_sys_cfg->n_output_audio_stream_1mins_diff_duration[ ch ] = diff;

							if( p_sys_cfg->n_output_audio_stream_1mins_diff_duration[ ch ] < 40000 ) {

								p_sys_cfg->n_output_audio_stream_1mins_diff_duration[ ch ] = 40000;
							}
							LINUXV4L2_DEBUG( KERN_INFO,"[%02d] ch(%d) 1mins_diff(%d) us\n", pDevice->m_nKsDeviceNumber, i, p_sys_cfg->n_output_audio_stream_1mins_diff_duration[ ch ] );
						}
						if( diff > (p_sys_cfg->n_output_audio_stream_1mins_diff_duration[ ch ] + 25000) ) {

							if( v_total_duration > a_total_duration ) {

								{ RtlCopyMemory( p_sys_cfg->p_output_audio_stream_buffer_i2s[ ch ] + p_sys_cfg->n_output_audio_stream_buffer_size[ ch ], p_sys_cfg->p_output_audio_stream_buffer_i2s[ ch ] + p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] - 16, 16 ); }

								p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] += 16;



								if( pDevice->debug[ 2 ] ) LINUXV4L2_DEBUG( KERN_INFO, "[%02d] ch(%d) +++++16 %lld, %lld %lld, %lld %lld (%d)", pDevice->m_nKsDeviceNumber, ch, diff, p_sys_cfg->n_output_video_stream_frame_number[ ch ], p_sys_cfg->n_output_audio_stream_frame_number[ ch ], v_total_duration, a_total_duration, p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] );
							}
							else if( p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] >= 16 ) {

								p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] -= 16;



								if( pDevice->debug[ 2 ] ) LINUXV4L2_DEBUG( KERN_INFO, "[%02d] ch(%d) ----16 %lld, %lld %lld, %lld %lld (%d)", pDevice->m_nKsDeviceNumber, ch, diff, p_sys_cfg->n_output_video_stream_frame_number[ ch ], p_sys_cfg->n_output_audio_stream_frame_number[ ch ], v_total_duration, a_total_duration, p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] );
							}
						}
					}


					if(p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] < p_sys_cfg->n_output_audio_stream_buffer_size_ai)
					{
						LINUXV4L2_DEBUG( KERN_INFO,"[%02d] ch(%d) postpone audio\n", pDevice->m_nKsDeviceNumber, ch );
					}



					if(pDevice->m_pVideo[ch])
					{
						if((pDevice->m_pVideo[ch]->m_nFrameNumber % 60) == 0)
						{
							if(pDevice->debug[1])
							{
								if(fps > 20 && freq)
								{
									LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) V - A (%lld) ms to AP, v(%lld) a(%lld)\n", pDevice->m_nKsDeviceNumber, ch, (v_total_duration - a_total_duration) / 1000, p_sys_cfg->n_output_video_stream_frame_number[ ch ], p_sys_cfg->n_output_audio_stream_frame_number[ ch ] );
								}
							}
						}
					}
				}
				else
				{
					LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) No fps(%d) && freq(%d)\n", pDevice->m_nKsDeviceNumber, ch, fps, freq);

					p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] = p_sys_cfg->n_output_audio_stream_buffer_size_ai;
				}

				while(p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] >= p_sys_cfg->n_output_audio_stream_buffer_size_ai)
				{
					if(pAudio->m_pKsSubStream)
					{
						if(pAudio->m_pKsSubStream->runtime)
						{
							if(pAudio->m_pKsSubStream->runtime->dma_area)
							{
								pe = pAudio->m_pKsSubStream->runtime->dma_area;

								pe += (pAudio->m_nFrameNumber % (pAudio->m_nFrames)) * pAudio->m_nFrameSize;

								if(status == 0xEEEEEEEE)
								{
									LINUXV4L2_DEBUG(KERN_INFO, "[%02d] fake audio\n", pDevice->m_nKsDeviceNumber);

									memset(pe, 0x00, p_sys_cfg->n_output_audio_stream_buffer_size_ai);
								}
								else
								{

									memcpy(pe, p_sys_cfg->p_output_audio_stream_buffer_i2s[ ch ], p_sys_cfg->n_output_audio_stream_buffer_size_ai);


									memcpy(p_sys_cfg->p_output_audio_stream_buffer_i2s[ ch ], p_sys_cfg->p_output_audio_stream_buffer_i2s[ ch ] + p_sys_cfg->n_output_audio_stream_buffer_size_ai,

										p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] - p_sys_cfg->n_output_audio_stream_buffer_size_ai);


								}
								p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] -= p_sys_cfg->n_output_audio_stream_buffer_size_ai;

								p_sys_cfg->n_output_audio_stream_frame_number[ ch ]++;


								if (((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D)) && (ch == 4 || ch == 5)) {


								}
								else
								{
									#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
									ULONGLONG now_ts = pDevice->interrupt_ts_audio / 1000;
									#else
									ULONGLONG now_ts = pDevice->interrupt_ts_audio.tv_sec * 1000000 + pDevice->interrupt_ts_audio.tv_usec;
									#endif

									ULONGLONG diff = now_ts - pDevice->previous_ts_audio_int[ch];

									if(INT_BYTES_A == 4096)
									{
										if( ABS(diff) > (21333 + 21000) || ABS(diff) < (21333 - 10000) )
										{
											LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) audio diff(0d%lld) us\n", pDevice->m_nKsDeviceNumber, ch, ABS(diff));
										}
									}
									if(INT_BYTES_A == 1920)
									{
										if( ABS(diff) > (10000 + 7000) || ABS(diff) < (10000 - 7000) )
										{
											LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) audio diff(0d%lld) us\n", pDevice->m_nKsDeviceNumber, ch, ABS(diff));
										}
									}

									pDevice->previous_ts_audio_int[ch] = now_ts;
								}

								pAudio->m_nFrameNumber = (pAudio->m_nFrameNumber + 1) % (pAudio->m_nFrames);

								snd_pcm_period_elapsed( pAudio->m_pKsSubStream );

								if(pDevice->debug[ch + 4]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) audio success, m_nFrameNumber(0d%d)\n", pDevice->m_nKsDeviceNumber, ch, pAudio->m_nFrameNumber);

								if(p_sys_cfg->n_output_audio_stream_buffer_size[ ch ] >= p_sys_cfg->n_output_audio_stream_buffer_size_ai)
								{

									LINUXV4L2_DEBUG( KERN_INFO,"[%02d] ch(%d) create audio\n", pDevice->m_nKsDeviceNumber, ch );

									usleep_range( 3000, 3200 );
								}
							}
							else
							{
								LINUXV4L2_DEBUG(KERN_INFO, "[%02d] no dma_area\n", pDevice->m_nKsDeviceNumber);

								break;
							}
						}
						else
						{
							LINUXV4L2_DEBUG(KERN_INFO, "[%02d] no runtime\n", pDevice->m_nKsDeviceNumber);

							break;
						}
					}
					else
					{
						LINUXV4L2_DEBUG(KERN_INFO, "[%02d] no m_pKsSubStream\n", pDevice->m_nKsDeviceNumber);

						break;
					}
				}
			}
			else
			{

			}
		}
	}
}

int SC0710_StartControlPanelAnalysisThread(CDevice* pDevice)
{
	if(pDevice->m_pControlThread == NULL)
	{
		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_StartControlPanelAnalysisThread( ) \n", pDevice->m_nKsDeviceNumber);

		pDevice->m_pControlThread = kthread_run(SC0710_OnControlPanelAnalysisThread, pDevice, "SC0710_OnControlPanelAnalysisThread");

		if(IS_ERR(pDevice->m_pControlThread)) {

			int ret = PTR_ERR(pDevice->m_pControlThread);

			pDevice->m_pControlThread = NULL;

			LINUXV4L2_DEBUG(KERN_ERR, "[%02d] SC0710_StartControlPanelAnalysisThread( ) FAIL(0x%x)\n", pDevice->m_nKsDeviceNumber, ret);

			return ret;
		}
	}
	else
	{
		LINUXV4L2_DEBUG(KERN_ERR, "[%02d] SC0710_StartControlPanelAnalysisThread( ) already exist, error, m_pControlThread(0p%p)\n", pDevice->m_nKsDeviceNumber, pDevice->m_pControlThread);
	}

	return STATUS_SUCCESS;
}

void SC0710_CLOSE_THREAD_EX(CDevice* pDevice)
{
	int ret = 0;

	down(&(pDevice->m_pControlSemaphore));

	pDevice->m_StopControlPanelAnalysisThread = 1;

	up(&(pDevice->m_pControlSemaphore));

	LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_CLOSE_THREAD_EX() m_pControlThread_already_stop(%d)---\n", pDevice->m_nKsDeviceNumber, pDevice->m_pControlThread_already_stop);

	ret = wait_event_timeout(pDevice->m_wait_queue_control, pDevice->m_pControlThread_already_stop == 1, msecs_to_jiffies(3000));

	if(ret <= 0) {

		LINUXV4L2_PRINT(KERN_ERR, "[%02d] SC0710_CLOSE_THREAD_EX() time out, ret(0d%d)---------------------------\n", pDevice->m_nKsDeviceNumber, ret);

	}
	else {

		if(pDevice->m_pControlThread_already_stop == 0)
		{
			LINUXV4L2_PRINT(KERN_ERR, "[%02d] SC0710_CLOSE_THREAD_EX() m_pControlThread_already_stop, error----------------------\n", pDevice->m_nKsDeviceNumber);
		}
		else
		{

		}
	}

	LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_CLOSE_THREAD_EX() ---end\n", pDevice->m_nKsDeviceNumber);
}

int SC0710_StopControlPanelAnalysisThread(CDevice* pDevice)
{
	if(pDevice->m_pControlThread) {

		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_StopControlPanelAnalysisThread( ) \n", pDevice->m_nKsDeviceNumber);

		kthread_stop(pDevice->m_pControlThread);

		pDevice->m_pControlThread = NULL;
	}
	else
	{
		LINUXV4L2_DEBUG(KERN_ERR, "[%02d] SC0710_StopControlPanelAnalysisThread( ) not exist, error\n", pDevice->m_nKsDeviceNumber);
	}

	return STATUS_SUCCESS;
}

int SC0710_OnControlPanelAnalysisThread(void* p_context)
{
	CDevice* pDevice = (CDevice*)p_context;

#if defined(AME_SC0710) || defined(AME_SC0720)

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);



	ULONG counts = 0;

	ULONG i = 0;

	ULONG unstables[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_OnControlPanelAnalysisThread( ) \n", pDevice->m_nKsDeviceNumber);

	while (kthread_should_stop() == FALSE) {

		if(pDevice->m_StopControlPanelAnalysisThread)
		{
			pDevice->m_pControlThread_already_stop = TRUE;

			wake_up(&pDevice->m_wait_queue_control);

			DELAY_100NS(10000);

			continue;
		}

		if(pDevice->m_nCustomPowerSavingProperty)
		{
			if(pDevice->debug[0]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] PowerSaving, skip detection( ) \n", pDevice->m_nKsDeviceNumber);

			DELAY_100NS(10000);

			continue;
		}

		pDevice->m_pControlThread_already_stop = FALSE;

		#ifdef BG_UPDATE
		if(p_sys_cfg->b_is_background_updated)
		{
			SC0710_background_updated(pDevice);
		}
		#endif

		{

			HDMI20_INTERFACE s_rx_hdmi20_buf;

			ULONG ch = 0;

			ULONG nums = 1;

			ULONG is_new_signal = 0;

			if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

					if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1A)) ||
						((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1B)) ||
						((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1C)) ||
						((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1D)) ||
						((pDevice->bcdDevice == 0xD002) && (pDevice->iManufacturer == 0x1C)) ||
						((pDevice->bcdDevice == 0xC002) && (pDevice->iManufacturer == 0x1C)) ||
						((pDevice->bcdDevice == 0xC002) && (pDevice->iManufacturer == 0x1D))) {

						nums = 2;
					}
					else if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1E)) ||
						((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F))) {

						nums = 2;
					}
					else if( ((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||
							 ((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D)) ||
							 ((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
							 ((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ) {

						nums = 8;
					}
					else if(((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

						nums = 16;
					}
					else if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1C)) ||
							 ((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
							 ((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
							 ((pDevice->bcdDevice == 0xA201) && (pDevice->iManufacturer == 0x1E)) ||
							 ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
							 ((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
							 ((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x1E)) ||
							 ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
							 ((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F)) ||
							 ((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1F)) ||
							 ((pDevice->bcdDevice == 0x1101) && (pDevice->iManufacturer == 0x1F)) ||
							 ((pDevice->bcdDevice == 0x1201) && (pDevice->iManufacturer == 0x1F)) ||
							 ((pDevice->bcdDevice == 0x1301) && (pDevice->iManufacturer == 0x1F)) ||
							 ((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x1F)) ||
							 ((pDevice->bcdDevice == 0x0501) && (pDevice->iManufacturer == 0x1F)) ||
							 ((pDevice->bcdDevice == 0x0601) && (pDevice->iManufacturer == 0x1F)) ) {

						nums = 1;
					}
					else {

						nums = 4;
					}
				}
			else {

				nums = 1;
			}

			memset(&s_rx_hdmi20_buf, 0x00, sizeof(s_rx_hdmi20_buf));

			for (ch = 0; ch < nums; ch++)
			{
				unsigned char* rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.selport);



				long long        offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf) + (ch * 60) + ((ch > 0) ? 4 : 0);

				unsigned char    txbuf[5] = { (int)(offset), 0, 0, 0, 0 };

				ULONG			x = 0;

				ULONG			y = 0;

				ULONG			fps = 0;

				ULONG			m = 0;

				ULONG			interleaved = 0;

				ULONG           levelb = 0;

				ULONG			colorrange = 0;

				ULONG			colorimetry = 0;

				ULONG			freq = 0;

				ULONG			hdcp = 0;

				ULONG           bps = 0;

				ULONG			dataport = 0;

				int				n_audio_sample_rate = 0;

				int				n_flag = 0;

				BOOL is_hdr_updated = TRUE;

				BOOL is_3d_updated = TRUE;

				ULONG psf = 0;

				if((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x17))
				{
					if(pDevice->m_nKsDeviceNumber == 1)
					{
						unsigned char txbuf[2] = { 0, 0 };

						unsigned char rxbuf[2] = { 0, 0 };

						SC0710_AccessI2cRegisterS_NEW(pDevice, 0x90, txbuf, 1, rxbuf, 1, 0);

						pDevice->m_nTemperature = rxbuf[0];


					}
				}

				#ifdef AME_SC0710
				if((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) {

					WORD nRX_RATE = 0;

					BYTE nRX_LOCK = 0;

					GET_RX_STAT(pDevice, &nRX_RATE, &nRX_LOCK);



					if(p_sys_cfg->m_RX_RATE != nRX_RATE) {

						p_sys_cfg->m_RX_RATE = nRX_RATE;

						txbuf[0] = 0xAA;

						txbuf[1] = 0x5A;

						txbuf[2] = 0xA5;

						txbuf[3] = (BYTE)(nRX_RATE);

						txbuf[4] = nRX_LOCK;

						SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X33, txbuf, 5, rxbuf, 0);




					}
				}
				#endif

				if(((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||
					((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D)) ||
					((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
					((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

					txbuf[0] = (int)(offset >> 8);

					txbuf[1] = (int)(offset >> 0);

					if(ch == 0) { SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, rxbuf + 0, (4) + (22)); }

					if(ch >= 1) { SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, rxbuf + 4, (0) + (22)); }
				}
				else if(((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0xA201) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
					((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F))) {

					ULONG R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

					R000000EC &= ~0x03;

					if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4) {

						txbuf[0] = (int)(offset);

						txbuf[1] = 0;

						SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf + 0, (4) + (0));

						txbuf[0] = (int)(rxbuf - (unsigned char*)(&s_rx_hdmi20_buf) + 64);

						txbuf[1] = 0;

						SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf + 4, (0) + (22));

						R000000EC |= 0x01;
					}
					else {

						txbuf[0] = (int)(offset);

						txbuf[1] = 0;

						SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf + 0, (4) + (22));

						R000000EC |= 0x00;
					}
					SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);
				}

				else if( false
					|| ((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x1D))
					|| ((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D))
				) {

					MCU_INTERFACE_MULTICH_LC300 s_rx_hdmi20_buf_lc300;

					unsigned char* rxbuf_lc300 = (BYTE*)(&s_rx_hdmi20_buf_lc300.function_name.selport);

					long long        offset_lc300 = rxbuf_lc300 - (unsigned char*)(&s_rx_hdmi20_buf_lc300) + (ch * 30) + ((ch > 0) ? 4 : 0);

					memset(&s_rx_hdmi20_buf_lc300, 0x00, sizeof(s_rx_hdmi20_buf_lc300));

					txbuf[0] = (BYTE)offset_lc300;

					if(ch == 0) {

						SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf_lc300 + 0, (4) + (22));

						pDevice->m_nUVCRunningStatus = s_rx_hdmi20_buf_lc300.function_name.selport;
					}
					else {

						SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf_lc300 + 4, (0) + (22));
					}

					RtlCopyMemory(rxbuf, rxbuf_lc300, 22);
				}


				else if (false
					|| ((pDevice->bcdDevice == 0x0301) && (pDevice->iManufacturer == 0x17))
					|| ((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x17))
				) {

					MCU_INTERFACE_MULTICH_LC300 s_rx_hdmi20_buf_lc300;

					unsigned char* rxbuf_lc300 = (BYTE*)(&s_rx_hdmi20_buf_lc300.function_name.selport);

					long long        offset_lc300 = rxbuf_lc300 - (unsigned char*)(&s_rx_hdmi20_buf_lc300) + (4 * 30) + 4;

					memset(&s_rx_hdmi20_buf_lc300, 0x00, sizeof(s_rx_hdmi20_buf_lc300));

					txbuf[0] = 0;


					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf_lc300, 4);

					txbuf[0] = (BYTE)offset_lc300;

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf_lc300 + 4, (0) + (22));

					RtlCopyMemory(rxbuf, rxbuf_lc300, 22);

				}
				else if (((pDevice->idVendor == 0x12AB) && (pDevice->idProduct & 0xFFFF) == 0x0750)) {

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf + 0, (4) + (22));

					if (p_sys_cfg->n_firmware_version) {

						s_rx_hdmi20_buf.function_name.resolution[0].hactive = (unsigned short)(SC0710_GetRegister(pDevice, 0x00000098));

						s_rx_hdmi20_buf.function_name.resolution[0].vactive = (unsigned short)(SC0710_GetRegister(pDevice, 0x0000009C));


					}
				}
				else {







					txbuf[0] = (int)(offset);

					txbuf[1] = 0;

					if(pDevice->m_StopControlPanelAnalysisThread) { LINUXV4L2_DEBUG(KERN_INFO, "[%02d] continue\n", pDevice->m_nKsDeviceNumber); continue; }

					if(ch == 0) { SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf + 0, (4) + (22)); }

					if(ch >= 1) { SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf + 4, (0) + (22)); }
				}





				if((s_rx_hdmi20_buf.function_name.resolution[0].hactive > 0) &&

					(s_rx_hdmi20_buf.function_name.resolution[0].vactive > 0) &&

					(s_rx_hdmi20_buf.function_name.resolution[0].hactive != 0xFFFF) &&

					(s_rx_hdmi20_buf.function_name.resolution[0].vactive != 0xFFFF)) {

					BOOL is_recalu_fps = FALSE;

					x = s_rx_hdmi20_buf.function_name.resolution[0].hactive;

					y = s_rx_hdmi20_buf.function_name.resolution[0].vactive;

					if(x % 2)
					{
						x -= 1;
					}

					if(y % 2)
					{
						y -= 1;
					}

					fps = s_rx_hdmi20_buf.function_name.resolution[0].fps;

					dataport = s_rx_hdmi20_buf.function_name.data_port_num & 0x00000001;

					m = 0;

					interleaved = 0;

					colorimetry = 0;

					freq = 48000;

					hdcp = 0;

					n_audio_sample_rate = s_rx_hdmi20_buf.function_name.resolution[0].audiosamplerate;

					n_flag = s_rx_hdmi20_buf.function_name.resolution[0].modeflag;

					bps = s_rx_hdmi20_buf.function_name.bitrate;


					if((n_flag & 0x02) == 0x02) {

						m = 1;
					}
					else {

						m = 0;
					}

					if((n_flag & 0x01) == 0x01) {

						USHORT R00019 = 0x0000;

						USHORT R0001A = 0x0000;

						unsigned char tx[2] = { 0x37, 0 };

						unsigned char rx[4] = { 0, 0, 0, 0 };

						interleaved = 1;


						if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4)
						{
							if(pDevice->m_nCustomCompanyJojoroProperty)
							{
								if(pDevice->m_StopControlPanelAnalysisThread) { LINUXV4L2_DEBUG(KERN_INFO, "[%02d] continue\n", pDevice->m_nKsDeviceNumber); continue; }

								SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, tx, 1, rx, 4);

								R00019 = rx[1] << 8 | rx[0];

								R0001A = rx[3] << 8 | rx[2];



								if(fps == 60) {

									if((R00019 == 0x4685) && (R0001A == 0x0100)) {


										x = 1920; y = 540; fps = 60; levelb = 0; psf = 1;
									}
									else if((R00019 == 0x4785) && (R0001A == 0x0100)) {


										x = 1920; y = 540; fps = 60; levelb = 0; psf = 1;
									}
									else if((R00019 == 0x4689) && (R0001A == 0x0200)) {


										x = 1920; y = 540; fps = 60; levelb = 0; psf = 1;
									}
									else if((R00019 == 0x4789) && (R0001A == 0x0200)) {


										x = 1920; y = 540; fps = 60; levelb = 0; psf = 1;
									}
									else {


									}
								}


								if(fps == 50) {

									if((R00019 == 0x4585) && (R0001A == 0x0100)) {


										x = 1920; y = 540; fps = 50; levelb = 0; psf = 1;
									}
									else if((R00019 == 0x4589) && (R0001A == 0x0200)) {


										x = 1920; y = 540; fps = 50; levelb = 0; psf = 1;
									}
									else {

									}
								}

								if(fps == 48) {

									if((R00019 == 0x4285) && (R0001A == 0x0100)) {


										x = 1920; y = 540; fps = 48; levelb = 0; psf = 1;
									}
									else if((R00019 == 0x4385) && (R0001A == 0x0100)) {


										x = 1920; y = 540; fps = 48; levelb = 0; psf = 1;
									}
									else if((R00019 == 0x4289) && (R0001A == 0x0200)) {


										x = 1920; y = 540; fps = 48; levelb = 0; psf = 1;
									}
									else if((R00019 == 0x4389) && (R0001A == 0x0200)) {


										x = 1920; y = 540; fps = 48; levelb = 0; psf = 1;
									}
									else
									{
									}
								}
							}
						}
					}
					else {

						interleaved = 0;
					}

					if(((pDevice->iProduct == 0x1C) && (pDevice->iManufacturer == 0xFA) && (pDevice->bcdDevice == 0x000E)) ||
						((pDevice->iProduct == 0x1C) && (pDevice->iManufacturer == 0xFA) && (pDevice->bcdDevice == 0x000F)) ||
						((pDevice->iProduct == 0x10) && (pDevice->iManufacturer == 0xFC) && (pDevice->bcdDevice == 0xD07C)) ||
						((pDevice->iProduct == 0x10) && (pDevice->iManufacturer == 0xFC) && (pDevice->bcdDevice == 0xD07C))) {

						is_recalu_fps = TRUE;
					}
					if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1C)) ||
						((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1C)) ||




						((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1E) && (ch == 1)) ||
						((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F) && (ch == 1)) ||
						((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x10)) ||
						((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x11)) ||
						((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x15)) ||
						((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x15)) ||
						((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x15)) ||
						((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x17)) ||
						((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x17)) ||
						((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x17)) ||
						((pDevice->bcdDevice == 0x0301) && (pDevice->iManufacturer == 0x17)) ||
						((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x17)) ||
						((pDevice->bcdDevice == 0xB001) && (pDevice->iManufacturer == 0x17)) ||
						((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x17)) ||
						((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x17)) ||
						((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x17)) ||
						((pDevice->bcdDevice == 0x0104) && (pDevice->iManufacturer == 0x17)) ||
						((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x19)) ||
						((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x19)) ||
						((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x19)) ||
						((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x19)) ||
						((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x19)) ||
						((pDevice->bcdDevice == 0x0102) && (pDevice->iManufacturer == 0x19)) ||
						((pDevice->bcdDevice == 0x0104) && (pDevice->iManufacturer == 0x19)) ||
						((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x19)) ||
						((pDevice->bcdDevice == 0x0108) && (pDevice->iManufacturer == 0x19))) {



						is_recalu_fps = TRUE;
					}
					if(is_recalu_fps) {

						ULONG R000000D4 = 0;

						if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1C)) ||
							((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1C)) ||




							((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1E) && (ch == 1)) ||
							((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F) && (ch == 1))) {



							if((x != 0) && (y != 0) && (fps != 0)) {

								ULONG R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

								R000000EC &= ~3;

								R000000EC |= ch;

								SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);
							}
						}
						R000000D4 = SC0710_GetRegister(pDevice, 0x000000D4);



						if((R000000D4 > 0) && (R000000D4 < 2700000)) {

							ULONGLONG fps_1000_o = 27000000000L / R000000D4;

							fps = (ULONG)((fps_1000_o + 500) / 1000);



							if(interleaved) {

								if(pDevice->m_nAnalogVideoDecoderStatusPropertys[ch] > 0) {

									if((s_rx_hdmi20_buf.function_name.resolution[0].htotal != p_sys_cfg->n_input_video_resolution_h_totals[ch]) ||

										((s_rx_hdmi20_buf.function_name.resolution[0].vtotal < p_sys_cfg->n_input_video_resolution_v_totals[ch] - 3) ||

											(s_rx_hdmi20_buf.function_name.resolution[0].vtotal > p_sys_cfg->n_input_video_resolution_v_totals[ch] + 3))) {

										#define FRAME_CLOCK		2

										const ULONG VSYNC_COUNT_27MHZ_1080I60[FRAME_CLOCK] = { 450000 / 1, 450450 / 1 };

										const ULONG VSYNC_COUNT_27MHZ_1080I120[FRAME_CLOCK] = { 450000 / 2, 450450 / 2 };

										const ULONG VSYNC_COUNT_27MHZ_480I60[FRAME_CLOCK] = { 450000 / 1, 450450 / 1 };

										const ULONG VSYNC_COUNT_27MHZ_480I120[FRAME_CLOCK] = { 450000 / 2, 450450 / 2 };

										const ULONG VSYNC_COUNT_27MHZ_480I240[FRAME_CLOCK] = { 450000 / 4, 450450 / 4 };

										ULONG R000000D4_2nd = 0;

										ULONG R000000D4_avg = R000000D4;

										ULONG closest_index = 0;

										ULONG closest_diff = 0;

										ULONG i = 0;

										p_sys_cfg->n_input_video_resolution_h_totals[ch] = s_rx_hdmi20_buf.function_name.resolution[0].htotal;

										p_sys_cfg->n_input_video_resolution_v_totals[ch] = s_rx_hdmi20_buf.function_name.resolution[0].vtotal;



										if((fps == 60) || (fps == 120) || (fps == 240)) {

											ULONG   n_temp_frame_count = 0;

											ULONG* p_temp_vsync_count_27mhz = NULL;

											if(fps == 60) {

												DELAY_100NS(180000);
											}
											if(fps == 120) {

												DELAY_100NS(90000);
											}
											if(fps == 240) {

												DELAY_100NS(45000);
											}
											for (i = 0; i < 20; i++) {

												R000000D4_2nd = SC0710_GetRegister(pDevice, 0x000000D4);

												if((R000000D4_2nd > (R000000D4 + 100)) ||

													(R000000D4_2nd < (R000000D4 - 100))) {

													R000000D4_avg = (R000000D4_2nd + R000000D4) / 2;

													break;
												}
												DELAY_100NS(10000);
											}

											if((y == 240) && (fps == 60)) {

												n_temp_frame_count = FRAME_CLOCK;

												p_temp_vsync_count_27mhz = (ULONG*)(&VSYNC_COUNT_27MHZ_480I60[0]);
											}
											if((y == 240) && (fps == 120)) {

												n_temp_frame_count = FRAME_CLOCK;

												p_temp_vsync_count_27mhz = (ULONG*)(&VSYNC_COUNT_27MHZ_480I120[0]);
											}
											if((y == 240) && (fps == 240)) {

												n_temp_frame_count = FRAME_CLOCK;

												p_temp_vsync_count_27mhz = (ULONG*)(&VSYNC_COUNT_27MHZ_480I240[0]);
											}
											if((y == 540) && (fps == 60)) {

												n_temp_frame_count = FRAME_CLOCK;

												p_temp_vsync_count_27mhz = (ULONG*)(&VSYNC_COUNT_27MHZ_1080I60[0]);
											}
											if((y == 540) && (fps == 120)) {

												n_temp_frame_count = FRAME_CLOCK;

												p_temp_vsync_count_27mhz = (ULONG*)(&VSYNC_COUNT_27MHZ_1080I120[0]);
											}
											for (i = 0; i < n_temp_frame_count; i++) {

												if(i == 0) {

													closest_index = 0;

													closest_diff = abs(R000000D4_avg - p_temp_vsync_count_27mhz[i]);
												}
												else {

													if((ULONG)(abs(R000000D4_avg - p_temp_vsync_count_27mhz[i])) < closest_diff) {

														closest_index = i;

														closest_diff = abs(R000000D4_avg - p_temp_vsync_count_27mhz[i]);
													}
												}
											}
											if(closest_index % 2) {

												m = 1;
											}
											else {

												m = 0;
											}
											p_sys_cfg->n_input_video_resolution_fps_ms_by_p2140[ch] = m;
										}
										else {

											p_sys_cfg->n_input_video_resolution_fps_ms_by_p2140[ch] = m = 0;
										}
									}
									else {

										m = p_sys_cfg->n_input_video_resolution_fps_ms_by_p2140[ch];
									}
								}
							}
							else {

								ULONG fps_1000_e = (fps * 1000000) / 1001;

								if(((fps_1000_e - 10) < (fps_1000_o)) &&

									((fps_1000_e + 10) > (fps_1000_o)) &&

									((fps % 6) == 0)) {

									m = 1;
								}
								else {

									m = 0;
								}
							}
						}
					}

					if (((pDevice->bcdDevice == 0xD004) && (pDevice->iManufacturer == 0x1C)) ||
						((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1C)) ||
						((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ||
						((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
						((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
						((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

						if ((n_flag & 0x04) == 0x04) {

							p_sys_cfg->n_input_tvi_signal_locksp[ch] = 1;
						}
						else {

							p_sys_cfg->n_input_tvi_signal_locksp[ch] = 0;
						}
					}
					else {

						if((n_flag & 0x04) == 0x04) {

							colorrange = 2;
						}
						else {

							colorrange = 1;
						}
					}
					{	switch (n_flag & 0x30) {

							case 0x30: colorimetry = 3; break;

							case 0x20: colorimetry = 1; break;

							case 0x10: colorimetry = 2; break;

							case 0x00: {

								colorimetry = (x == 720) ? 1 : 2;

								break;
							}
						}
					}
					if((n_flag & 0x08) == 0x08) {

						if(pDevice->m_nCustomCompanyMedicalProperty == 0x00000001) {

							hdcp = 0;
						}
						else {

							hdcp = 1;
						}
					}
					else {

						hdcp = 0;
					}
					if(n_audio_sample_rate == 0x01) {

						freq = 32000;
					}
					else if(n_audio_sample_rate == 0x02) {

						freq = 44100;
					}
					else if(n_audio_sample_rate == 0x03) {

						freq = 48000;
					}
					else if(n_audio_sample_rate == 0x04) {

						freq = 88200;
					}
					else if(n_audio_sample_rate == 0x05) {

						freq = 96000;
					}
					else if(n_audio_sample_rate == 0x06) {

						freq = 176400;
					}
					else if(n_audio_sample_rate == 0x07) {

						freq = 192000;
					}
					if(freq > 0) {

						freq = 48000;
					}
					if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

						(pDevice->iManufacturer == 0x1C) ||

						(pDevice->iManufacturer == 0x1D) ||

						(pDevice->iManufacturer == 0x1C) ||

						(pDevice->iManufacturer == 0x1C) ||

						(pDevice->iManufacturer == 0x1D) ||

						(pDevice->iManufacturer == 0x29) ||

						(pDevice->iManufacturer == 0x2D) ||

						(pDevice->iManufacturer == 0x2D)) {

						if(((pDevice->bcdDevice == 0xD004) && (pDevice->iManufacturer == 0x1C)) ||

							((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1C)) ||

							((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ||

							((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||

							((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||

							((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

							is_hdr_updated = FALSE;

							is_3d_updated = FALSE;
						}
						else {

							is_hdr_updated = FALSE;

							is_3d_updated = TRUE;
						}
						freq = 48000;
					}


					#ifndef FAST_DETECT
					if(is_hdr_updated && ch == 0) {

						unsigned char* rxbuf = (BYTE*)(&s_rx_hdmi20_buf.function_name.resolution[0].hdrpacket);

						long long        offset = rxbuf - (unsigned char*)(&s_rx_hdmi20_buf);

						unsigned char    txbuf[4] = { (int)(offset), 0, 0, 0 };

						unsigned char* HDR = rxbuf;

						BOOLEAN         ret = FALSE;

						txbuf[0] = (int)(offset);

						ret = SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf, 16);

						if(ret) {

							txbuf[0] = (int)(offset + 16);

							ret = SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf + 16, 16);

							if(ret) {

								RtlCopyMemory(p_sys_cfg->n_input_video_hdr_packets, HDR, 32);
							}
							else {

								;
							}
						}
					}
					#endif

					if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1C)) ||
						((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1C)) ||
						((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1C)) ||
						((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x1D)) ||
						((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D)) ||
						((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1D)) ||
						((pDevice->bcdDevice == 0x0A04) && (pDevice->iManufacturer == 0x1D))) {

						ULONG R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

						if((pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty >= 3) ||

							(n_flag & 0x40) == 0x40) {

							R000000EC |= (1 << ((ch * 2) + 8));
						}
						else {

							R000000EC &= ~(1 << ((ch * 2) + 8));
						}
						SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);
					}
					if( ((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2D)) ) {

						ULONG R000000EC = SC0710_GetRegister( pDevice, 0x000000EC );

						R000000EC &= ~(3 << ((ch * 2) + 8));

						if( pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty >= 3 ) {

							R000000EC |=  (1 << ((ch * 2) + 8));
						}
						else {

							R000000EC &= ~(1 << ((ch * 2) + 8));
						}
						SC0710_SetRegister( pDevice, 0x000000EC, R000000EC );
					}
					if( ((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
						((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||

						((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
						((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||

						((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
						((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F)) ) {

						ULONG n_native_vdo444 = 0x00000000;

						ULONG n_10bits_mode = 0x00000000;

						if( pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty == 1 ) {

							if( pDevice->m_nCustomAnalogVideoNativeColorSpaceAutoProperty >= 0x00000002 ||

								pDevice->m_nCustomAnalogVideoNativeColorSpaceAutoProperty == 0x00000000 ) {



								n_native_vdo444 = 0x00000000;
							}
							else {

								n_native_vdo444 = 0x00000001;
							}
						}
						if( pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty >= 2 ) {

							if( pDevice->m_nCustomAnalogVideoNativeColorSpaceAutoProperty >= 0x00000002 ) {



								n_native_vdo444 = 0x00000000;
							}
							else if( pDevice->m_nCustomAnalogVideoNativeColorSpaceAutoProperty == 0x00000001 ) {

								n_native_vdo444 = 0x00000001;
							}
							else {

								n_native_vdo444 = 0x00000000;
							}
						}
						if( pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty >= 3 ) {

							n_10bits_mode = 0x00000003;
						}
						{
							ULONG R000000EC = SC0710_GetRegister( pDevice, 0x000000EC );

							if( p_sys_cfg->n_input_video_signal_dual_stream_sdis[ ch ] ) {

								R000000EC &= ~(15 << ((ch * 2) + 8));
							}
							else {

								R000000EC &= ~( 7 << ((ch * 2) + 8));

								if( n_native_vdo444 == 0x00000001 ) {

									R000000EC |=  (1 << ((ch * 2) +  9));
								}
								else if( n_native_vdo444 == 0x00000002 ) {

									R000000EC |=  (1 << ((ch * 2) +  8));
								}
								else if( n_10bits_mode == 0x00000003 ) {

									R000000EC |=  (1 << ((ch * 2) + 10));
								}
								else {

									R000000EC &= ~(7 << ((ch * 2) +  8));
								}
							}
							SC0710_SetRegister( pDevice, 0x000000EC, R000000EC );
						}
					}

				}

				if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x13)) ||
					((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x13)) ||
					((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||
					((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||
					((pDevice->bcdDevice == 0xB401) && (pDevice->iManufacturer == 0x16)) ||
					((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||
					((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ||
					((pDevice->bcdDevice == 0xB201) && (pDevice->iManufacturer == 0x18)) ||
					((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18)) ||
					((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x18)) ||
					((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x18)) ) {

					if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4)
					{
						if(((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||
							((pDevice->bcdDevice == 0xB401) && (pDevice->iManufacturer == 0x16)) ||
							((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||
							((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18))) {

							BOOL is_reg_updated = FALSE;

							unsigned char txbuf[2] = { 0, 0 };

							unsigned char rxbuf[2] = { 0, 0 };

							if(p_sys_cfg->n_mcu_version >= 220315) {

								txbuf[0] = 0x01;

								SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 1, rxbuf, 1);

								txbuf[1] = rxbuf[0];


								if((txbuf[1] & 0x0F) != (pDevice->m_nCustomAnalogVideoMultiChannelMaskProperty)) {

									txbuf[1] &= 0xF0;

									txbuf[1] |= (pDevice->m_nCustomAnalogVideoMultiChannelMaskProperty & 0x0F);

									is_reg_updated = TRUE;
								}

								if(is_reg_updated) {

									SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X32, txbuf, 2, NULL, 0);
								}
							}
						}

						if(ch == 0) {

							if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

								x /= 2;

								y /= 2;
							}
							else {

								unsigned char txbuf[2] = { 2, 0 };

								unsigned char rxbuf[2] = { 0, 0 };

								BYTE n_device_address = (p_sys_cfg->n_mcu_version >= 190903) ? SC0710_I2C_MCU_ADR7_0X31 : SC0710_I2C_MCU_ADR7_0X32;

								if(pDevice->m_StopControlPanelAnalysisThread) { LINUXV4L2_DEBUG(KERN_INFO, "[%02d] continue\n", pDevice->m_nKsDeviceNumber); continue; }

								SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, n_device_address, txbuf, 1, rxbuf, 1);

								if((rxbuf[0] & 0x2F) == 0x21) {

									p_sys_cfg->n_input_video_signal_single_link_under_3G = 1;
								}
								else {

									p_sys_cfg->n_input_video_signal_single_link_under_3G = 0;
								}
								if(p_sys_cfg->n_input_video_signal_single_link_under_3G) {

									if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||
										((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x13)) ||
										((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x13)) ||
										psf ) {



										;
									}
									else {

										x /= 2;

										y /= 2;
									}
								}
							}
						}
					}


					if(((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||

						((pDevice->bcdDevice == 0xB401) && (pDevice->iManufacturer == 0x16)) ||

						((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ||

						((pDevice->bcdDevice == 0xB201) && (pDevice->iManufacturer == 0x18)) ||

						((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18))) {

						if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

							BYTE n_device_address = (p_sys_cfg->n_mcu_version >= 190903) ? SC0710_I2C_MCU_ADR7_0X31 : SC0710_I2C_MCU_ADR7_0X32;

							unsigned char txbuf[2] = { 0x03, 0x00 };

							unsigned char rxbuf[2] = { 0, 0 };

							BOOL is_reg_updated = FALSE;

							if(ch == 0 ||

								ch == 2) {


								SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, n_device_address, txbuf, 1, rxbuf, 1);
							}
							if(ch == 0) {

								if(x >= 3840 && y >= 2160) {

									p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[0] = 1;

									p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[1] = 1;

									if((rxbuf[0] & 0x01) == 0x00) { is_reg_updated = TRUE; }

									txbuf[1] = rxbuf[0] | 0x01;
								}
								else {

									p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[0] = 0;

									p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[1] = 0;

									if((rxbuf[0] & 0x01) == 0x01) { is_reg_updated = TRUE; }

									txbuf[1] = rxbuf[0] & 0xFE;
								}
							}
							if(ch == 2) {

								if(x >= 3840 && y >= 2160) {

									p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[2] = 1;

									p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[3] = 1;

									if((rxbuf[0] & 0x02) == 0x00) { is_reg_updated = TRUE; }

									txbuf[1] = rxbuf[0] | 0x02;
								}
								else {

									p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[2] = 0;

									p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[3] = 0;

									if((rxbuf[0] & 0x02) == 0x02) { is_reg_updated = TRUE; }

									txbuf[1] = rxbuf[0] & 0xFD;
								}
							}
							if(ch == 0 ||

								ch == 2) {

								if(is_reg_updated) {


									SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, n_device_address, txbuf, 2, NULL, 0);
								}
							}





						}
					}
				}

				if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1C)) ||
					((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1C)) ||
					((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1C)) ||
					((pDevice->bcdDevice == 0xD004) && (pDevice->iManufacturer == 0x1C)) ||
					((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1C)) ||
					((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0x0A04) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ||

					((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0xA201) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
					((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F)) ||
					((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F)) ||
					((pDevice->bcdDevice == 0x0204) && (pDevice->iManufacturer == 0x29)) ||
					((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x29)) ||
					((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2B)) ||
					((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2D))) {



					if((x != 0) && (y != 0) && (fps != 0)) {

						BOOL is_bad_signal = FALSE;

						is_bad_signal = SC0710_IS_BAD_SIGNAL_MULTICH(pDevice, ch);

						if(is_bad_signal) {

							if(((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
								((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
								((pDevice->bcdDevice == 0xA201) && (pDevice->iManufacturer == 0x1E)) ||
								((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
								((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
								((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x1E)) ||
								((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
								((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F))) {

								if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4) {

									ULONG R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

									R000000EC &= ~(1 << (1 + 4));

									SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);

									R000000EC |= (1 << (1 + 4));

									SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);

									LINUXV4L2_PRINT(KERN_INFO, "[CH01] [CHANNEL RESET]\n");
								}
								else {

									ULONG R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

									R000000EC &= ~(1 << (0 + 4));

									SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);

									R000000EC |= (1 << (0 + 4));

									SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);

									LINUXV4L2_PRINT(KERN_INFO, "[CH00] [CHANNEL RESET]\n");
								}
							}
							else {

								ULONG R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

								R000000EC &= ~(1 << (ch + 4));

								SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);

								R000000EC |= (1 << (ch + 4));

								SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);

								LINUXV4L2_PRINT(KERN_INFO, "[CH%02d] [CHANNEL RESET]\n", ch);
							}
						}
					}
				}

				if(((x == 2826) && (y == 2312)) || ((x == 1798) && (y == 1284)))
				{
					LINUXV4L2_PRINT(KERN_INFO, "invalid MCU_FORMAT_DETECT( %d x %d x %d x %d x %d x %d x %d x %d )\n", pDevice->m_nKsDeviceNumber, ch, (ULONG)(x), (ULONG)(y), (fps), (m), (freq), (hdcp));

					counts++;

					DELAY_100NS(10000);

					continue;
				}

				if((x != 0) && (y != 0) && ((x * y) <= (7680 * 4320)) && (fps != 0)) {

					if(p_sys_cfg->n_input_video_resolution_cxs[ch] != x ||

						p_sys_cfg->n_input_video_resolution_cys[ch] != y ||

						p_sys_cfg->n_input_video_resolution_fpss[ch] != fps ||

						p_sys_cfg->n_input_video_resolution_fps_ms[ch] != m ||

						p_sys_cfg->n_input_video_resolution_levelbs[ch] != levelb ||

						p_sys_cfg->n_input_video_resolution_color_colorimetrys[ch] != colorimetry ||

						p_sys_cfg->n_input_audio_sampling_frequencys[ch] != freq ||

						p_sys_cfg->n_input_video_resolution_psfs[ch] != psf) {

						#ifdef FAST_DETECT
						if(unstables[ch]++ > 0) {
						#else
						if(unstables[ch]++ > 5) {
						#endif

							LINUXV4L2_PRINT(KERN_INFO, "[%02d] MCU_FORMAT_DETECT() ch(%d) %d/%d, %d/%d, %d/%d, %d/%d, %d/%d, %d/%d, %d/%d, %d/%d  \n", pDevice->m_nKsDeviceNumber, ch,
								p_sys_cfg->n_input_video_resolution_cxs[ch], x,
								p_sys_cfg->n_input_video_resolution_cys[ch], y,
								p_sys_cfg->n_input_video_resolution_fpss[ch], fps,
								p_sys_cfg->n_input_video_resolution_fps_ms[ch], m,
								p_sys_cfg->n_input_video_resolution_levelbs[ch], levelb,
								p_sys_cfg->n_input_video_resolution_color_colorimetrys[ch], colorimetry,
								p_sys_cfg->n_input_audio_sampling_frequencys[ch], freq,
								p_sys_cfg->n_input_video_resolution_psfs[ch], psf);

							unstables[ch] = 0;

							p_sys_cfg->n_input_video_resolution_cx = x;

							p_sys_cfg->n_input_video_resolution_cy = y;

							p_sys_cfg->n_input_video_resolution_fps = fps;

							if(pDevice->m_nCustomCompanyMediaedgeProperty == 1)
							{
								p_sys_cfg->n_input_video_resolution_cx /= 2;

								p_sys_cfg->n_input_video_resolution_cy /= 2;

								p_sys_cfg->n_input_video_resolution_fps *= 4;
							}

							p_sys_cfg->n_input_video_resolution_fps_m = m;

							p_sys_cfg->n_input_video_resolution_levelb = levelb;

							p_sys_cfg->n_input_video_resolution_cxs[ch] = x;

							p_sys_cfg->n_input_video_resolution_cys[ch] = y;

							p_sys_cfg->n_input_video_resolution_fpss[ch] = fps;

							p_sys_cfg->n_input_video_resolution_fps_ms[ch] = m;

							p_sys_cfg->n_input_video_resolution_levelbs[ch] = levelb;







							if(p_sys_cfg->n_input_video_resolution_cy == 240 ||

								p_sys_cfg->n_input_video_resolution_cy == 288 ||

								p_sys_cfg->n_input_video_resolution_cy == 540) {

								p_sys_cfg->n_input_video_resolution_interleaved = 1;

								p_sys_cfg->n_input_video_resolution_interleaveds[ch] = 1;
							}
							else {

								p_sys_cfg->n_input_video_resolution_interleaved = 0;

								p_sys_cfg->n_input_video_resolution_interleaveds[ch] = 0;
							}
							p_sys_cfg->n_input_video_resolution_color_colorimetry = colorimetry;

							p_sys_cfg->n_input_video_resolution_color_range = colorrange;

							p_sys_cfg->n_input_video_resolution_color_type = 1;

							p_sys_cfg->n_input_video_resolution_bitrate = bps;

							p_sys_cfg->n_input_video_resolution_dataport = dataport;

							p_sys_cfg->b_input_video_signal_changed = TRUE;

							p_sys_cfg->n_input_audio_sampling_frequency = freq;

							p_sys_cfg->n_output_video_resolution_cxs[ch] = x;

							p_sys_cfg->n_output_video_resolution_cys[ch] = y;

							p_sys_cfg->n_output_video_resolution_fpss[ch] = fps;

							p_sys_cfg->n_output_video_resolution_fps_base_fpss[ch] = (m == 0) ? (fps * 1000) : (fps * 1000000 / 1001);

							if(p_sys_cfg->n_input_video_resolution_interleaveds[ch] == 1)
							{
								p_sys_cfg->n_output_video_resolution_fpss[ch] = (fps / 2);

								p_sys_cfg->n_output_video_resolution_fps_base_fpss[ch] = (m == 0) ? ((fps / 2) * 1000) : ((fps / 2) * 1000000 / 1001);
							}

							p_sys_cfg->n_output_video_resolution_fps_ms[ch] = m;

							p_sys_cfg->n_output_video_resolution_interleaveds[ch] = p_sys_cfg->n_input_video_resolution_interleaveds[ch];

							p_sys_cfg->n_input_video_resolution_color_colorimetrys[ch] = colorimetry;

							p_sys_cfg->n_input_video_resolution_color_ranges[ch] = colorrange;

							p_sys_cfg->n_input_video_resolution_color_types[ch] = 1;

							p_sys_cfg->n_input_audio_sampling_frequencys[ch] = freq;

							p_sys_cfg->n_input_video_resolution_psf = psf;

							p_sys_cfg->n_input_video_resolution_psfs[ch] = psf;

							p_sys_cfg->n_output_video_resolution_psf = psf;

							p_sys_cfg->n_output_video_resolution_psfs[ch] = psf;

						}
						if(unstables[ch] > 0) {

							p_sys_cfg->n_is_bad_signal_count_vs[ch] = 1000;
						}
						else {

							p_sys_cfg->n_is_bad_signal_count_vs[ch] = fps / 8;
						}


						is_new_signal = 1;
					}
					else
					{
						unstables[ch] = 0;

						if(p_sys_cfg->n_is_bad_signal_count_vs[ch] > 30) {

							p_sys_cfg->n_is_bad_signal_count_vs[ch] = 30;
						}
					}






					if(unstables[ch] == 0) {

						if(p_sys_cfg->n_input_video_resolution_fps > 50) {

							pDevice->m_nCustomVideoStandardProperty = V4L2_STD_NTSC_M;
						}
						else if(p_sys_cfg->n_input_video_resolution_fps > 30) {

							pDevice->m_nCustomVideoStandardProperty = V4L2_STD_PAL_B;
						}
						else if(p_sys_cfg->n_input_video_resolution_fps > 25) {

							pDevice->m_nCustomVideoStandardProperty = V4L2_STD_NTSC_M;
						}
						else {

							pDevice->m_nCustomVideoStandardProperty = V4L2_STD_PAL_B;
						}
						pDevice->m_nCustomAnalogVideoResolutionProperty = (p_sys_cfg->n_input_video_resolution_cx << 16) |

							(p_sys_cfg->n_input_video_resolution_cy << 0);

						pDevice->m_nCustomAnalogVideoFrameRateProperty = (p_sys_cfg->n_input_video_resolution_fps);

						pDevice->m_nCustomAnalogVideoInterleavedProperty = (p_sys_cfg->n_input_video_resolution_interleaved);

						pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = p_sys_cfg->n_input_audio_sampling_frequency;

						pDevice->m_nAnalogVideoDecoderStatusProperty = 1;

						pDevice->m_nAnalogVideoDecoderStatusPropertys[ch] = 1;

						pDevice->m_nAnalogCopyProtMacrovisionProperty = hdcp;

						pDevice->m_nAnalogCopyProtMacrovisionPropertys[ch] = hdcp;



						#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)
						if(is_new_signal)
						{
							is_new_signal = 0;

							if (pDevice->m_pKsVideo[0])
							{
								static const struct v4l2_event ev_fmt = {
									.type = V4L2_EVENT_SOURCE_CHANGE,
									.u.src_change.changes = V4L2_EVENT_SRC_CH_RESOLUTION,
								};

								v4l2_event_queue(pDevice->m_pKsVideo[0], &ev_fmt);

								LINUXV4L2_PRINT(KERN_INFO, "[%02d] event issued\n", pDevice->m_nKsDeviceNumber);

							}
						}
						#endif
					}
				}
				else {

					p_sys_cfg->n_input_video_resolution_cx = 0;

					p_sys_cfg->n_input_video_resolution_cy = 0;

					p_sys_cfg->n_input_video_resolution_fps = 0;

					p_sys_cfg->n_input_video_resolution_fps_m = 0;

					p_sys_cfg->n_input_video_resolution_levelb = 0;

					p_sys_cfg->n_input_video_resolution_interleaved = 0;

					p_sys_cfg->n_input_video_resolution_color_colorimetry = 0;

					p_sys_cfg->n_input_video_resolution_color_range = 0;

					p_sys_cfg->n_input_video_resolution_color_type = 0;

					p_sys_cfg->n_input_video_resolution_bitrate = 0;

					p_sys_cfg->n_input_video_resolution_dataport = dataport;

					p_sys_cfg->n_input_audio_sampling_frequency = 0;



					p_sys_cfg->n_input_video_resolution_cxs[ch] = 0;

					p_sys_cfg->n_input_video_resolution_cys[ch] = 0;

					p_sys_cfg->n_input_video_resolution_fpss[ch] = 0;

					p_sys_cfg->n_input_video_resolution_fps_ms[ch] = 0;

					p_sys_cfg->n_input_video_resolution_levelbs[ch] = 0;

					p_sys_cfg->n_input_video_resolution_interleaveds[ch] = 0;

					p_sys_cfg->n_input_video_resolution_interleaveds_field[ch] = 0;

					p_sys_cfg->n_input_video_resolution_color_colorimetrys[ch] = 0;

					p_sys_cfg->n_input_video_resolution_color_ranges[ch] = 0;

					p_sys_cfg->n_input_video_resolution_color_types[ch] = 0;

					p_sys_cfg->n_input_video_resolution_h_totals[ch] = 0;

					p_sys_cfg->n_input_video_resolution_v_totals[ch] = 0;

					p_sys_cfg->n_input_video_resolution_fps_ms_by_p2140[ch] = 0;

					p_sys_cfg->n_input_audio_sampling_frequencys[ch] = 0;

					pDevice->m_nCustomAnalogVideoResolutionProperty = 0;

					pDevice->m_nCustomAnalogVideoFrameRateProperty = 0;

					pDevice->m_nCustomAnalogVideoInterleavedProperty = 0;

					pDevice->m_nCustomAnalogAudioSampleFrequencyProperty = 0;

					pDevice->m_nAnalogVideoDecoderStatusProperty = 0;

					pDevice->m_nAnalogVideoDecoderStatusPropertys[ch] = 0;

					pDevice->m_nAnalogCopyProtMacrovisionProperty = 0;

					pDevice->m_nAnalogCopyProtMacrovisionPropertys[ch] = 0;

					pDevice->m_nDmaBaseCommonBufferNumber[0] = 0;
					pDevice->m_nDmaBaseCommonBufferNumber[1] = 0;

					p_sys_cfg->n_input_video_resolution_psf = 0;

					p_sys_cfg->n_input_video_resolution_psfs[ch] = 0;

				}

				if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||

					((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1C)) ||
					((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1C)) ||
					((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1C)) ||
					((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0x0A04) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0x0B04) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0xB004) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0xD004) && (pDevice->iManufacturer == 0x1C)) ||
					((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1C)) ||
					((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
					((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F)) ||
					((pDevice->bcdDevice == 0x0204) && (pDevice->iManufacturer == 0x29)) ||
					((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x29)) ||
					((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2B)) ||
					((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2D)) ||
					((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
					((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D)) ||
					((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||
					((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D))) {

					BOOL is_tvi_products_1 = FALSE;

					BOOL is_tvi_products_2 = FALSE;

					BOOL is_n8_products = FALSE;

					ULONG R000000EC = 0;

					if(((pDevice->bcdDevice == 0xD004) && (pDevice->iManufacturer == 0x1C)) ||
						((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1C)) ||
						((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ||
						((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D))) {

						is_tvi_products_1 = TRUE;
					}
					if(((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
						((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

						is_tvi_products_2 = TRUE;
					}
					if(((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||
						((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D))) {

						is_n8_products = TRUE;
					}
					R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

					if(is_tvi_products_2) {

						if(pDevice->m_nAnalogVideoDecoderStatusPropertys[ch] > 0) {

							ULONG group = ch / 4;

							R000000EC &= ~0x03;

							R000000EC |= group;
						}
					}
					else if(is_n8_products) {

						if(pDevice->m_nAnalogVideoDecoderStatusPropertys[ch] > 0) {

							ULONG group = (ch / 4) | 0x30;

							R000000EC &= ~0x03;

							R000000EC |= group;
						}
					}
					else {

						if(pDevice->m_nAnalogVideoDecoderStatusPropertys[ch] > 0) {

							if((R000000EC & (1 << (ch + 4))) == 0) {

								LINUXV4L2_PRINT(KERN_INFO, "ENABLE CH%02d", ch);
							}
							if(((pDevice->bcdDevice == 0x0204) && (pDevice->iManufacturer == 0x29)) ||
								((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x29)) ||
								((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2B)) ||
								((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2D))) {

								R000000EC &= ~0x03;

								R000000EC |= ch;
							}
							if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F))) {

								if(((p_sys_cfg->n_input_video_resolution_cxs[ch] == 720) && (p_sys_cfg->n_input_video_resolution_cys[ch] == 240)) ||

									((p_sys_cfg->n_input_video_resolution_cxs[ch] == 720) && (p_sys_cfg->n_input_video_resolution_cys[ch] == 288))) {

									R000000EC |= (1 << (ch + 20));
								}
								else {

									R000000EC &= ~(1 << (ch + 20));
								}
								R000000EC &= ~(0x3 << (ch * 2 + 8));

								if(p_sys_cfg->n_input_video_resolution_cxs[ch] *

									p_sys_cfg->n_input_video_resolution_cys[ch] *

									p_sys_cfg->n_input_video_resolution_fpss[ch] > 65000000) {

									R000000EC |= 1 << (ch * 2 + 8);
								}
								else {

									;
								}
							}
							R000000EC |= (1 << (ch + 4));
						}
						else {

							if(R000000EC & (1 << (ch + 4))) {

								LINUXV4L2_PRINT(KERN_INFO, "DISABLE CH%02d", ch);
							}
							R000000EC &= ~(1 << (ch + 4));
						}
					}
					if(is_tvi_products_1) {

						if((pDevice->m_nAnalogVideoDecoderStatusPropertys[ch] > 0) &&



							(y < 1080)) {

							R000000EC &= ~(3 << (ch * 2 + 8));
						}
						else {

							R000000EC &= ~(3 << (ch * 2 + 8));

							R000000EC |= (1 << (ch * 2 + 8));
						}
					}
					if(is_tvi_products_2) {

						ULONG group = 0;

						for (group = 0; group < 4; group++) {

							R000000EC &= ~(3 << (group * 2 + 8));

							R000000EC |= (1 << (group * 2 + 8));
						}
					}
					SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);
				}

				if(((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0xA201) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x1E)) ||
					((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
					((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F))) {

					ULONG R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

					R000000EC &= ~0x003000C0;

					if(p_sys_cfg->n_output_video_color_bits == 5) {

						R000000EC |= (0x03 << 24);
					}
					else {

						R000000EC &= 0xFCFFFFFF;
					}

					if(pDevice->m_nAnalogVideoDecoderStatusPropertys[ch] > 0) {

						if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4) {

							R000000EC &= ~0x10;

							if((R000000EC & (1 << (1 + 4))) == 0) {

								LINUXV4L2_PRINT(KERN_INFO, "ENABLE CH01");

								R000000EC |= 0x20;
							}
							if(((p_sys_cfg->n_input_video_resolution_cxs[ch] == 720) && (p_sys_cfg->n_input_video_resolution_cys[ch] == 240)) ||

								((p_sys_cfg->n_input_video_resolution_cxs[ch] == 720) && (p_sys_cfg->n_input_video_resolution_cys[ch] == 288))) {

								R000000EC |= (1 << (1 + 20));
							}
							else {

								R000000EC &= ~(1 << (1 + 20));
							}
						}
						else {

							R000000EC &= ~0x20;

							if((R000000EC & (1 << (0 + 4))) == 0) {

								LINUXV4L2_PRINT(KERN_INFO, "ENABLE CH00");

								R000000EC |= 0x10;
							}
							if(((p_sys_cfg->n_input_video_resolution_cxs[ch] == 720) && (p_sys_cfg->n_input_video_resolution_cys[ch] == 240)) ||

								((p_sys_cfg->n_input_video_resolution_cxs[ch] == 720) && (p_sys_cfg->n_input_video_resolution_cys[ch] == 288))) {

								R000000EC |= (1 << (0 + 20));
							}
							else {

								R000000EC &= ~(1 << (0 + 20));
							}
						}
					}
					else {

						if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4) {

							R000000EC &= ~0x10;

							if(R000000EC & (1 << (1 + 4))) {

								LINUXV4L2_PRINT(KERN_INFO, "DISABLE CH01");

								R000000EC &= ~0x20;
							}
						}
						else {

							R000000EC &= ~0x20;

							if(R000000EC & (1 << (0 + 4))) {

								LINUXV4L2_PRINT(KERN_INFO, "DISABLE CH00");

								R000000EC &= ~0x10;
							}
						}
					}
					SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);
				}

				if(pDevice->m_pVideo[ch])
				{
					CVideo* pVideo = pDevice->m_pVideo[ch];

					if(pDevice->m_RestartControlPanelAnalysisThread == 1)
					{
						pDevice->m_RestartControlPanelAnalysisThread = 0;

						if(pVideo->m_nInputChanged)
						{
							pVideo->m_nInputChanged = 0;

							LINUXV4L2_PRINT(KERN_INFO, "[%02d] ch(%d) clear m_nInputChanged", pDevice->m_nKsDeviceNumber, ch);
						}
					}
				}

			}

		}

		{
			BOOL is_bad_signal = SC0710_IS_BAD_SIGNAL(pDevice);


			if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty == 0 &&

				pDevice->m_nAnalogCaptureStreamPowerReference > 0 &&

				p_sys_cfg->n_video_running_state > 0 &&

				(p_sys_cfg->b_input_video_signal_changed || is_bad_signal || p_sys_cfg->b_dma_length_error || p_sys_cfg->b_dma_EOP_error)) {

				ULONG n_native_vdo444 = 0x00000000;

				BOOL  is_10bits = FALSE;

				BOOL  is_reset = TRUE;

				LINUXV4L2_PRINT(KERN_INFO, "[%02d] [FIRMWARE RESET][changed=%d, reset=%d, length_err=%d, EOP_err=%d]\n", pDevice->m_nKsDeviceNumber, p_sys_cfg->b_input_video_signal_changed, is_bad_signal, p_sys_cfg->b_dma_length_error, p_sys_cfg->b_dma_EOP_error);

				p_sys_cfg->b_input_video_signal_changed = FALSE;

				p_sys_cfg->b_dma_length_error = FALSE;

				p_sys_cfg->b_dma_EOP_error = FALSE;

				if((pDevice->m_pVideo[0]->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pDevice->m_pVideo[0]->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY) || (pDevice->m_pVideo[0]->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pDevice->m_pVideo[0]->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24))
				{
					pDevice->m_nCustomAnalogVideoNativeColorDeepProperty = 0;
				}
				else if((pDevice->m_pVideo[0]->m_nFramePixelFormat == V4L2_PIX_FMT_P010) || (pDevice->m_pVideo[0]->m_nFramePixelFormat == V4L2_PIX_FMT_P016) || (pDevice->m_pVideo[0]->m_nFramePixelFormat == V4L2_PIX_FMT_P210))
				{
					pDevice->m_nCustomAnalogVideoNativeColorDeepProperty = 1;
				}
				else
				{
					pDevice->m_nCustomAnalogVideoNativeColorDeepProperty = 1;
				}

				if((pDevice->m_pVideo[0]->m_nFramePixelFormat == V4L2_PIX_FMT_RGB24) || (pDevice->m_pVideo[0]->m_nFramePixelFormat == V4L2_PIX_FMT_BGR24))
				{
					pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty = 1;

					pDevice->m_nCustomAnalogVideoNativeColorSpaceAutoProperty = 0x00000001;
				}

				else
				{
					pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty = 0;
				}

				if(pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty == 1) {

					if(pDevice->m_nCustomAnalogVideoNativeColorSpaceAutoProperty >= 0x00000002 ||

						pDevice->m_nCustomAnalogVideoNativeColorSpaceAutoProperty == 0x00000000) {

						n_native_vdo444 = 0x00000002;
					}
					else {

						n_native_vdo444 = 0x00000001;
					}
				}
				if(pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty >= 2) {

					if(pDevice->m_nCustomAnalogVideoNativeColorSpaceAutoProperty >= 0x00000002) {

						n_native_vdo444 = 0x00000002;
					}
					else if(pDevice->m_nCustomAnalogVideoNativeColorSpaceAutoProperty == 0x00000001) {

						n_native_vdo444 = 0x00000001;
					}
					else {

						n_native_vdo444 = 0x00000000;
					}
				}
				if(pDevice->m_nCustomAnalogVideoNativeColorDeepProperty == 1) {

					is_10bits = TRUE;
				}
				if(pDevice->m_nCustomAnalogVideoNativeColorDeepProperty >= 2) {

					is_10bits = (pDevice->m_nCustomAnalogVideoNativeColorDeepAutoProperty > 0) ? TRUE : FALSE;
				}
				if(is_10bits) {

					n_native_vdo444 = 0x00000000;
				}
				if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

					is_reset = FALSE;
				}

				if(pDevice->m_nCustomCompanyJojoroProperty)
				{
					if(pDevice->m_nAnalogVideoDecoderStatusProperty == 0)
					{
						is_reset = FALSE;
					}
				}

				if(is_reset) {

					if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty)
					{
						SC0710_CLOSE_SOFTWARE_INTERRUPT_THREAD();
					}

					SC0710_HwStopVideoInterrupt(pDevice);


					SC0710_HwStopVideoDMA(pDevice);



					SC0710_HwStartVideoDMA(pDevice, n_native_vdo444, is_10bits, pDevice->m_pVideo[0]);

					if(is_bad_signal == FALSE) {

						p_sys_cfg->n_is_bad_signal_count_vs[0] = 0;

						p_sys_cfg->n_is_bad_signal_count_a = 0;

						p_sys_cfg->n_multi_chdma_remains = 0;








					}
					else {

						p_sys_cfg->n_multi_chdma_remains = 0;
					}

					SC0710_HwStartVideoInterrupt(pDevice);

					if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty)
					{
						SC0710_START_SOFTWARE_INTERRUPT_THREAD();
					}
				}
			}
		}

		counts++;

		pDevice->m_pControlThread_already_stop = TRUE;

		wake_up(&pDevice->m_wait_queue_control);

		for (i = 0; i < DEC_LOOP_CNT; i++) {

			if(kthread_should_stop()) {

				break;
			}

			if(pDevice->m_StopControlPanelAnalysisThread) { LINUXV4L2_DEBUG(KERN_INFO, "[%02d] break\n", pDevice->m_nKsDeviceNumber); break; }

			DELAY_100NS(100000);
		}
	}

	pDevice->m_pControlThread_already_stop = TRUE;

	wake_up(&pDevice->m_wait_queue_control);

	LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_OnControlPanelAnalysisThread( exit ) - %p\n", pDevice->m_nKsDeviceNumber, pDevice->m_pControlThread);

#endif

	return 0;

}

NTSTATUS SC0710_StartSoftwareInterruptThread(CDevice* pDevice)
{
	if(pDevice->m_pSoftwareinterruptThread == NULL)
	{
		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_StartSoftwareInterruptThread( ) \n", pDevice->m_nKsDeviceNumber);

		pDevice->m_pSoftwareinterruptThread = kthread_run(SC0710_OnSoftwareInterruptThread, pDevice, "SC0710_OnSoftwareInterruptThread");

		if(IS_ERR(pDevice->m_pSoftwareinterruptThread)) {

			int ret = PTR_ERR(pDevice->m_pSoftwareinterruptThread);

			pDevice->m_pSoftwareinterruptThread = NULL;

			LINUXV4L2_DEBUG(KERN_ERR, "[%02d] SC0710_StartSoftwareInterruptThread( ) FAIL(0x%x)\n", pDevice->m_nKsDeviceNumber, ret);

			return ret;
		}
	}
	else
	{
		LINUXV4L2_DEBUG(KERN_ERR, "[%02d] SC0710_StartSoftwareInterruptThread( ) already exist, m_pSoftwareinterruptThread(0p%p)\n", pDevice->m_nKsDeviceNumber, pDevice->m_pSoftwareinterruptThread);
	}

	return STATUS_SUCCESS;

}

NTSTATUS SC0710_StopSoftwareInterruptThread(CDevice* pDevice)
{
	if(pDevice->m_pSoftwareinterruptThread) {

		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_StopSoftwareInterruptThread( ) \n", pDevice->m_nKsDeviceNumber);

		kthread_stop(pDevice->m_pSoftwareinterruptThread);

		pDevice->m_pSoftwareinterruptThread = NULL;
	}
	else
	{
		LINUXV4L2_DEBUG(KERN_ERR, "[%02d] SC0710_StopControlPanelAnalysisThread( ) not exist\n", pDevice->m_nKsDeviceNumber);
	}

	return STATUS_SUCCESS;
}

NTSTATUS SC0710_UNPACK_MULTI_CHDMA_PAGES_EX(CDevice* pDevice, ULONG n_start_page_no, ULONG n_stop_page_no);



NTSTATUS SC0710_OnSoftwareInterruptThread(void* p_context)
{
	CDevice* pDevice = (CDevice*)p_context;

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	XDMA_ENGINE_REGS* p_regs_xdma_engine_video = (XDMA_ENGINE_REGS*)(p_sys_cfg->p_regs_xdma_engines[0][1]);









	ULONGLONG n_start_time_v = 0;











	ULONG     n_timeouts_v = 0;



	ULONG     n_fake_mask_v = 0;



	ULONG     n_completed_desc_count_v = 0;



	BOOL      is_run_v = FALSE;



	ULONG     rings_v = 4;



	ULONG     n_frame_no_vs_last = 0;

	p_sys_cfg->n_frame_no_swint = -1;

	LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_OnSoftwareInterruptThread() m_nCustomAnalogVideoMultiChannelSupportProperty(0x%x)\n", pDevice->m_nKsDeviceNumber, pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty);

	set_user_nice(current, -20);



	while (kthread_should_stop() == FALSE) {

		BOOL  is_stream_loss_v = FALSE;



		BOOL  is_come_in_vs[4] = { FALSE, FALSE, FALSE, FALSE };



		BOOL  is_fake_in_v = FALSE;



		ULONG n_frame_no_vs[4] = { 0, 0, 0, 0 };

		ULONG counts_v = READ_REGISTER_ULONG(&(p_regs_xdma_engine_video->completedDescCount));









		if(is_run_v == FALSE) {

			is_run_v = TRUE;

			n_completed_desc_count_v = counts_v;
		}
		else {

			if(pDevice->m_nCustomCompanyMediaedgeProperty > 0) {

				ULONG R000000D8 = (SC0710_GetRegister(pDevice, 0x000000D8) & 0xFFFF0000) >> 16;



				CVideo* pVideo = pDevice->m_pVideo[0];

				if(pVideo)
				{
					if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV)
					{
						if(p_sys_cfg->n_frame_no_swint == 0 && R000000D8 >= 1024) { p_sys_cfg->n_frame_no_swint = 1; n_frame_no_vs[0] = n_frame_no_vs_last; is_come_in_vs[0] = TRUE; }

						if(p_sys_cfg->n_frame_no_swint == 1 && R000000D8 >= 2048) { p_sys_cfg->n_frame_no_swint = 2; n_frame_no_vs[0] = n_frame_no_vs_last; is_come_in_vs[0] = TRUE; }

						if(p_sys_cfg->n_frame_no_swint == 2 && R000000D8 >= 3072) { p_sys_cfg->n_frame_no_swint = 3; n_frame_no_vs[0] = n_frame_no_vs_last; is_come_in_vs[0] = TRUE; }
					}
					else if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P016))
					{
						if(p_sys_cfg->n_frame_no_swint == 0 && R000000D8 >= 1500) { p_sys_cfg->n_frame_no_swint = 1; n_frame_no_vs[0] = n_frame_no_vs_last; is_come_in_vs[0] = TRUE; }

						if(p_sys_cfg->n_frame_no_swint == 1 && R000000D8 >= 3000) { p_sys_cfg->n_frame_no_swint = 2; n_frame_no_vs[0] = n_frame_no_vs_last; is_come_in_vs[0] = TRUE; }

						if(p_sys_cfg->n_frame_no_swint == 2 && R000000D8 >= 4500) { p_sys_cfg->n_frame_no_swint = 3; n_frame_no_vs[0] = n_frame_no_vs_last; is_come_in_vs[0] = TRUE; }
					}
					else
					{
					}
				}
			}
			else if(n_completed_desc_count_v != counts_v)
			{
				if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0)
				{
					if(p_sys_cfg->n_video_running_state)
					{
						ULONG from = n_completed_desc_count_v;

						ULONG to = counts_v - 1;

						SC0710_UNPACK_MULTI_CHDMA_PAGES_EX(pDevice, from, to);

						n_completed_desc_count_v = counts_v;

						{
							ULONG i = from;
							for (i = from; i <= to; i++)
							{
								BYTE* p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + (i % (SC0710_DEF_MULTI_CHDMA_PAGES * SC0710_DEF_MULTI_CHDMA_ROW)) * 32;

								if(((XDMA_RESULT*)p_result)->length != SC0710_DEF_MULTI_CHDMA_PAGES_SIZE)
								{
									LINUXV4L2_PRINT(KERN_INFO, "[%02d] i(%d) length(%d) status(0x%x)\n", pDevice->m_nKsDeviceNumber, i, ((XDMA_RESULT*)p_result)->length, ((XDMA_RESULT*)p_result)->status);
								}
							}
						}
					}
				}
				else
				{
					if(pDevice->m_nCustomSoftwareInterruptProperty)
					{
						pDevice->m_nTaskletExtraParameterA |= 1;

						LINUXV4L2_DEBUG(KERN_INFO, "[%02d] V m_nTaskletExtraParameterA(0x%x) swint\n", pDevice->m_nKsDeviceNumber, pDevice->m_nTaskletExtraParameterA);

						tasklet_hi_schedule(&(pDevice->m_sTasklet_video));
					}
					n_completed_desc_count_v = counts_v;
				}
			}
			else
			{

			}
		}

		if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

			;
		}
		else {

			if(is_come_in_vs[0]) { n_timeouts_v = 0; }
			else { n_timeouts_v++; }



			if(n_timeouts_v >= 100) { is_stream_loss_v = TRUE; }


		}



		if(is_come_in_vs[0] ||

			is_come_in_vs[1] ||

			is_come_in_vs[2] ||

			is_come_in_vs[3]







			) {


				{
					ULONG status = 0x00000000;

					ULONG arguments = 0x00000000;

					if(n_fake_mask_v > 0 && is_come_in_vs[0]) {

						n_fake_mask_v--;

						is_fake_in_v = TRUE;
					}

					if(is_come_in_vs[0]) { status |= 0x00000001; arguments += (n_frame_no_vs[0] % rings_v) << 0; arguments += (p_sys_cfg->n_frame_no_swint) << 24; }

					if(is_come_in_vs[1]) { status |= 0x00000002; arguments += (n_frame_no_vs[1] % rings_v) << 4; }

					if(is_come_in_vs[2]) { status |= 0x00000004; arguments += (n_frame_no_vs[2] % rings_v) << 8; }

					if(is_come_in_vs[3]) { status |= 0x00000008; arguments += (n_frame_no_vs[3] % rings_v) << 12; }


					pDevice->m_nTaskletExtraParameterA |= status;





					if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty == 1)
					{
					}
					else
					{
						if(status & 0x1)
						{
							tasklet_hi_schedule(&(pDevice->m_sTasklet_video));
						}
					}
				}
		}

		usleep_range(200, 250);



		if(is_come_in_vs[0]) {

			n_start_time_v = 0;

			n_timeouts_v = 0;
		}
	}
	LINUXV4L2_DEBUG(KERN_INFO, "[%02d] SC0710_OnSoftwareInterruptThread( exit ) - %p\n", pDevice->m_nKsDeviceNumber, pDevice->m_pSoftwareinterruptThread);

	return 0;

}


void MultiChLastLine(CVideo* pVideo, ULONG ch, ULONG cho)
{
	ULONG correct_line_total = 0;

	struct CDevice* pDevice = pVideo->m_pDevice;

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);




	if(pVideo->m_native_fmt == 1)
	{
		correct_line_total = (cho * 12) >> 3;
	}
	else
	{
		if(p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[ch]) {

			correct_line_total = cho * 2;
		}
		else
		{
			correct_line_total = cho;
		}
	}

	if(pDevice->debug[ch + 4]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] MultiChLastLine() ch(%d) correct_line_total(%d)\n", pDevice->m_nKsDeviceNumber, ch, correct_line_total);

	if((pDevice->line_count[ch] != correct_line_total) && (pVideo->m_InNoVideoDPC != 1))
	{
		if(p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[ch]) {

			if(pDevice->line_count[ch] == (correct_line_total - 1)) {

				;
			}
			else {

				LINUXV4L2_DEBUG(KERN_INFO, "[%02d] line_count[ %d ](%d) ideal(%d)\n", pDevice->m_nKsDeviceNumber, ch, pDevice->line_count[ch], correct_line_total);

				pVideo->m_nFrameLastLine_cnt++;

				pDevice->line_count[ch] = 0;
			}
		}
		else {

			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] line_count[ %d ](%d) ideal(%d)\n", pDevice->m_nKsDeviceNumber, ch, pDevice->line_count[ch], correct_line_total);

			pVideo->m_nFrameLastLine_cnt++;

			pDevice->line_count[ch] = 0;
		}
	}
	else
	{
		ULONG wait_count_res_720 = 0;

		ULONG is_skip = FALSE;

		ULONG bit = 0;

		pVideo->m_nFrameLastLine_cnt++;

		if(pDevice->debug[ch + 4]) LINUXV4L2_DEBUG( KERN_INFO, "[%02d] correct line_count[ %d ](%d) -\n", pDevice->m_nKsDeviceNumber, ch, pDevice->line_count[ ch ]  );

		if((pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF) != 0x77FFFFFF)
		{
			ULONG real_fps = p_sys_cfg->n_input_video_resolution_fpss[pVideo->m_nSubDeviceNumber];

			if(real_fps && pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber])
			{
				if(p_sys_cfg->n_input_video_resolution_interleaveds[pVideo->m_nSubDeviceNumber])
				{
					real_fps /= 2;
				}



				CALCULATE_FPS(pDevice, pVideo, real_fps, (pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF));

				if(real_fps <= (pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF))
				{
					is_skip = FALSE;
				}
				else
				{
					bit = pVideo->m_nFrameLastLine_cnt % real_fps;

					is_skip = (pDevice->m_pVideo[pVideo->m_nSubDeviceNumber]->m_valid[bit] == 0) ? TRUE : FALSE;
				}
			}
		}
		if(is_skip)
		{
			if(pDevice->debug[ch + 4]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] m_nSubDeviceNumber(%d) skip frame, bit(0d%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber, bit);

			pDevice->line_count[ch] = 0;
		}
		else
		{


			if(p_sys_cfg->n_input_video_resolution_cxs[ch] == 720)
			{
				while (pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder] & 0x00FFFF)
				{
					usleep_range(250, 300);

					wait_count_res_720++;

					if(pDevice->debug[ch + 4]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) wait_count_res_720(0x%x)\n", pDevice->m_nKsDeviceNumber, ch, wait_count_res_720);

					if(wait_count_res_720 > 20)	break;
				}
			}

			if(pDevice->m_eVideoStreamState[ch])
			{

				if(pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder] != 0)
				{
					pVideo->m_MultiCh_Copy_Err_Cnt++;

					if(pDevice->debug[ch + 4]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) m_MultiCh_MultiCpy(0x%x) NOT ZERO, m_MultiCh_Copy_Err_Cnt(%d)\n", pDevice->m_nKsDeviceNumber, ch, pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder], pVideo->m_MultiCh_Copy_Err_Cnt);

					if(pVideo->m_MultiCh_Copy_Err_Cnt > 2)
					{


					}
					else
					{
						return;
					}
				}
				else
				{
					pVideo->m_MultiCh_Copy_Err_Cnt = 0;
				}
				{

					CVideoBuffer* pVideoBuffer = NULL;

					unsigned long flags = 0x00000000;

					spin_lock_irqsave(&pVideo->m_sKsBufferListSpinLock, flags);


					if(list_empty(&pVideo->m_sKsBufferList) == 0) {

						if(pVideo->m_sKsBufferList.next)
						{
							pVideoBuffer = list_entry(pVideo->m_sKsBufferList.next, CVideoBuffer, list);

							if(pVideoBuffer)
							{
								if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
								{


									#ifdef CONFIG_X86_64
									if((((PVOID)(pVideoBuffer->list.next)) == LIST_POISON1) || (((PVOID)(pVideoBuffer->list.prev)) == LIST_POISON2) || (((ULONGLONG)(pVideoBuffer->list.next) & 0xFFFF000000000000) == 0xdead000000000000) || (((ULONGLONG)(pVideoBuffer->list.prev) & 0xFFFF000000000000) == 0xdead000000000000) || (((ULONGLONG)(pVideoBuffer->list.next) == 0x8080808080808080)) || (((ULONGLONG)(pVideoBuffer->list.prev)) == 0x8080808080808080))
									#else
									if((((PVOID)(pVideoBuffer->list.next)) == LIST_POISON1) || (((PVOID)(pVideoBuffer->list.prev)) == LIST_POISON2) || (((ULONG)(pVideoBuffer->list.next)) == 0xdead000000000100) || (((ULONG)(pVideoBuffer->list.prev)) == 0xdead000000000200) || (((ULONG)(pVideoBuffer->list.next) == 0x8080808080808080)) || (((ULONG)(pVideoBuffer->list.prev)) == 0x8080808080808080))
									#endif
									{

										LINUXV4L2_PRINT(KERN_INFO, "[%02d] error, next(%p) prev(%p)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->list.next, pVideoBuffer->list.prev);
										LINUXV4L2_PRINT(KERN_INFO, "[%02d] error, LIST_POISON1(%p) LIST_POISON2(%p)\n", pDevice->m_nKsDeviceNumber, LIST_POISON1, LIST_POISON2);

										pVideoBuffer = NULL;
									}
									else
									{
										ULONG cx = p_sys_cfg->n_input_video_resolution_cxs[ch];

										ULONG cy = p_sys_cfg->n_input_video_resolution_cys[ch];

										if(p_sys_cfg->n_input_video_resolution_interleaveds[ch])
										{
											cy <<= 1;
										}

										if((vb2_get_plane_payload(&pVideoBuffer->vb2_buffer_cross, 0) >= (cx * cy * 12 >> 3)) || (pVideo->m_InNoVideoDPC == 1))
										{


											list_del(&pVideoBuffer->list);
										}
										else
										{
											LINUXV4L2_DEBUG(KERN_INFO, "[%02d] vb2_get_plane_payload() vb2_get_plane_payload(%ld) video_frame(%d) NOT MATCH\n", pDevice->m_nKsDeviceNumber, vb2_get_plane_payload(&pVideoBuffer->vb2_buffer_cross, 0), (cx * cy * 12 >> 3));

											pVideoBuffer = NULL;
										}
									}
								}
								else
								{
									LINUXV4L2_DEBUG(KERN_INFO, "[%02d] pVideoBuffer state error(0x%x)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state);

									pVideoBuffer = NULL;
								}
							}
							else
							{
								LINUXV4L2_DEBUG(KERN_INFO, "[%02d] list_entry return NULL\n", pDevice->m_nKsDeviceNumber);
							}
						}
						else
						{
							LINUXV4L2_DEBUG(KERN_INFO, "[%02d] m_sKsBufferList.next is NULL\n", pDevice->m_nKsDeviceNumber);
						}
					}
					else {

						if(pDevice->debug[ch + 4]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] no video buffer!\n", pDevice->m_nKsDeviceNumber);

						pVideoBuffer = NULL;
					}

					spin_unlock_irqrestore(&pVideo->m_sKsBufferListSpinLock, flags);

					if(pVideoBuffer)
					{

						if((pVideo->m_InNoVideoDPC == 1) || (pDevice->m_nAnalogCopyProtMacrovisionPropertys[ch] == 1))
						{

							BYTE* pe = vb2_plane_vaddr(&pVideoBuffer->vb2_buffer_cross, 0);

							if(pe == NULL) { LINUXV4L2_DEBUG(KERN_INFO, "[%02d] MultiChLastLine() pe == NULL \n", pDevice->m_nKsDeviceNumber); vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR); return; }

							if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY))
							{

							}
							else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12)
							{
							}
							else
							{

							}

							if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
							{

								#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
								pVideoBuffer->vb.vb2_buf.timestamp = ktime_get_ns();
								#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
								do_gettimeofday(&pVideoBuffer->vb.timestamp);
								#else
								do_gettimeofday(&pVideoBuffer->vb.v4l2_buf.timestamp);
								#endif

								update_video_time_stamp(pDevice, pVideo, pVideoBuffer);

								#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
								pVideoBuffer->vb.sequence = pVideo->m_nFrameNumber++;
								#else
								pVideoBuffer->vb.v4l2_buf.sequence = pVideo->m_nFrameNumber++;
								#endif

								p_sys_cfg->n_output_video_stream_frame_number[ ch ]++;

								#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
								pVideoBuffer->vb.field = pDevice->m_Video_field;
								#else
								pVideoBuffer->vb.v4l2_buf.field = pDevice->m_Video_field;
								#endif

								vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_DONE);


							}
							else
							{
								LINUXV4L2_PRINT(KERN_INFO, "[%02d] pe state(%x) error, next(0p%p) prev(0p%p)\n", pDevice->m_nKsDeviceNumber, pVideoBuffer->buffer_state, pVideoBuffer->list.next, pVideoBuffer->list.prev);


								if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
							}
						}
						else
						{

							ULONG j = 0;

							spin_lock_irqsave(&pVideo->m_sMultiCh_MultiCopySpinLock, flags);



							pVideo->m_nFrameNumberForQueue = pVideo->m_nFrameLastLine_cnt;

							pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % MULTI_CH_RING][1] = (BYTE*)pVideoBuffer;


							if(pDevice->debug[ch + 4]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] start ch(%d) m_strorder(%d) queue_work -\n", pDevice->m_nKsDeviceNumber, ch, pVideo->m_strorder);



							for (j = 0; j < 4; j++)
							{

								pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder] |= (1 << j);

								if(pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder] & (1 << j))
								{
									ULONG qw = queue_work(pVideo->wq_multich_multicpy, &pVideo->w_multich_multicopy[j]);

									if(qw == 0) {

										LINUXV4L2_PRINT(KERN_ERR, "[%02d] queue_work() ch(0x%x) w_multich_multicopy[%d]  (%d)- [FAIL]\n", pDevice->m_nKsDeviceNumber, ch, j, pVideo->m_strorder);

										pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder] &= ~(1 << j);
									}
								}
							}

							spin_unlock_irqrestore(&pVideo->m_sMultiCh_MultiCopySpinLock, flags);
						}
					}

				}
			}
			else
			{

			}

			pDevice->line_count[ch] = 0;
		}
	}
}

#ifdef MT_LINECOPY
typedef struct _SC0710_LINECOPY_SPEEDUP
{
	BYTE* po;

	BYTE* pe;

	ULONG     sz;

} SC0710_LINECOPY_SPEEDUP;

#endif


NTSTATUS SC0710_UNPACK_MULTI_CHDMA_PAGES(CDevice* pDevice, ULONG n_start_page_no, ULONG n_stop_page_no, ULONG cxo[16], ULONG cyo[16], ULONG cwo[16], ULONG cxe[16], ULONG cye[16], ULONG cwe[16], BYTE* po, BYTE* pe[16], ULONG n_dma_bufsize, ULONG is_10bits);

NTSTATUS SC0710_UNPACK_MULTI_CHDMA_PAGES_EX(CDevice* pDevice, ULONG n_start_page_no, ULONG n_stop_page_no)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	ULONG i = 0;
	ULONG j = 0;
	NTSTATUS status = STATUS_SUCCESS;
	BYTE* po = NULL;

	ULONG cxo[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	ULONG cyo[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	ULONG cwo[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	ULONG cxe[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	ULONG cye[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	ULONG cwe[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	BYTE* pe[16] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

	ULONG  n_dma_bufsize = 0;

	ULONG c = 0;

	ULONG is_10bits = FALSE;

	if(p_sys_cfg->n_output_video_color_bits == 5) {

		is_10bits = TRUE;
	}


	for (c = 0; c < 16; c++) {

		cxo[c] = p_sys_cfg->n_input_video_resolution_cxs[c];

		cyo[c] = p_sys_cfg->n_input_video_resolution_cys[c];

		cwo[c] = p_sys_cfg->n_input_video_resolution_cxs[c];

		cxe[c] = p_sys_cfg->n_input_video_resolution_cxs[c];

		cye[c] = p_sys_cfg->n_input_video_resolution_cys[c];

		cwe[c] = p_sys_cfg->n_input_video_resolution_cxs[c];
	}

	n_start_page_no %= (SC0710_DEF_MULTI_CHDMA_PAGES * SC0710_DEF_MULTI_CHDMA_ROW);

	n_stop_page_no %= (SC0710_DEF_MULTI_CHDMA_PAGES * SC0710_DEF_MULTI_CHDMA_ROW);


	i = (n_start_page_no / SC0710_DEF_MULTI_CHDMA_PAGES) % SC0710_DEF_MULTI_CHDMA_ROW;

	j = (n_stop_page_no / SC0710_DEF_MULTI_CHDMA_PAGES) % SC0710_DEF_MULTI_CHDMA_ROW;

	if(i != j) {

		ULONG n_start_last_page_no = ((i + 1) * SC0710_DEF_MULTI_CHDMA_PAGES) - (1);

		ULONG n_stop_1st_page_no = j * SC0710_DEF_MULTI_CHDMA_PAGES;

		{
			ULONG from = n_start_page_no;

			ULONG to = n_start_last_page_no;



			ULONG remains = p_sys_cfg->n_multi_chdma_remains; p_sys_cfg->n_multi_chdma_remains = 0;

			if(to >= from) {

				n_dma_bufsize = (to - from + 1) * (p_sys_cfg->n_output_video_bufsize);

				n_dma_bufsize += remains;

				po = (pDevice->m_pDmaBaseCommonBuffer[(i % SC0710_DEF_MULTI_CHDMA_ROW) + 1]);



				po += SC0710_DEF_MULTI_CHDMA_PAGES_SIZE_4M_RESIDUE;


				po += (from % SC0710_DEF_MULTI_CHDMA_PAGES) * (p_sys_cfg->n_output_video_bufsize);

				po -= (remains);



				status = SC0710_UNPACK_MULTI_CHDMA_PAGES(pDevice, from, to, cxo, cyo, cwo, cxe, cye, cwe, po, pe, n_dma_bufsize, is_10bits);
			}
			else {

				LINUXV4L2_PRINT(KERN_INFO, "[CHDMA.A.%d][%03d ~ %03d] bufsize = %d / %d\n", i, from, to, n_dma_bufsize, remains);
			}
		}


		while (((n_start_last_page_no + 1) % (SC0710_DEF_MULTI_CHDMA_PAGES * SC0710_DEF_MULTI_CHDMA_ROW)) != n_stop_1st_page_no)
		{
			ULONG from = (n_start_last_page_no + 1) % (SC0710_DEF_MULTI_CHDMA_PAGES * SC0710_DEF_MULTI_CHDMA_ROW);

			ULONG to = from + (SC0710_DEF_MULTI_CHDMA_PAGES - 1);

			ULONG remains = p_sys_cfg->n_multi_chdma_remains; p_sys_cfg->n_multi_chdma_remains = 0;



			if(to >= from) {

				n_dma_bufsize = (to - from + 1) * (p_sys_cfg->n_output_video_bufsize);

				n_dma_bufsize += remains;

				po = (pDevice->m_pDmaBaseCommonBuffer[(++i % SC0710_DEF_MULTI_CHDMA_ROW) + 1]);



				po += SC0710_DEF_MULTI_CHDMA_PAGES_SIZE_4M_RESIDUE;

				po -= (remains);



				status = SC0710_UNPACK_MULTI_CHDMA_PAGES(pDevice, from, to, cxo, cyo, cwo, cxe, cye, cwe, po, pe, n_dma_bufsize, is_10bits);
			}
			else {

				LINUXV4L2_PRINT(KERN_INFO, "[CHDMA.D.%d][%03d ~ %03d] bufsize = %d / %d\n", i, from, to, n_dma_bufsize, remains);
			}

			{


				ULONG i_n_start_last_page_no = (n_start_last_page_no / SC0710_DEF_MULTI_CHDMA_PAGES) % SC0710_DEF_MULTI_CHDMA_ROW;

				if(i_n_start_last_page_no > j)
				{

				}
				else
				{
					if(n_start_last_page_no >= n_stop_page_no)
					{
						LINUXV4L2_PRINT(KERN_INFO, "[CHDMA.C.%d][%03d ~ %03d] n_start_last_page_no(0d%d) > n_stop_page_no(0d%d)\n", i, from, to, n_start_last_page_no, n_stop_page_no);

						return FALSE;
					}
				}
			}

			n_start_last_page_no += SC0710_DEF_MULTI_CHDMA_PAGES;

			n_start_last_page_no %= (SC0710_DEF_MULTI_CHDMA_PAGES * SC0710_DEF_MULTI_CHDMA_ROW);
		}
		{	ULONG from = n_stop_1st_page_no;

		ULONG to = n_stop_page_no;

		ULONG remains = p_sys_cfg->n_multi_chdma_remains; p_sys_cfg->n_multi_chdma_remains = 0;



		if(to >= from) {

			n_dma_bufsize = (to - from + 1) * (p_sys_cfg->n_output_video_bufsize);

			n_dma_bufsize += remains;

			po = (pDevice->m_pDmaBaseCommonBuffer[(j % SC0710_DEF_MULTI_CHDMA_ROW) + 1]);



			po += SC0710_DEF_MULTI_CHDMA_PAGES_SIZE_4M_RESIDUE;

			po += (from % SC0710_DEF_MULTI_CHDMA_PAGES) * (p_sys_cfg->n_output_video_bufsize);

			po -= (remains);



			status = SC0710_UNPACK_MULTI_CHDMA_PAGES(pDevice, from, to, cxo, cyo, cwo, cxe, cye, cwe, po, pe, n_dma_bufsize, is_10bits);
		}
		else {

			LINUXV4L2_PRINT(KERN_INFO, "[CHDMA.B.%d][%03d ~ %03d] bufsize = %d / %d\n", j, from, to, n_dma_bufsize, remains);
		}
		}
	}
	else {

		ULONG from = n_start_page_no;

		ULONG to = n_stop_page_no;

		ULONG remains = p_sys_cfg->n_multi_chdma_remains; p_sys_cfg->n_multi_chdma_remains = 0;

		if(to >= from) {

			n_dma_bufsize = (to - from + 1) * (p_sys_cfg->n_output_video_bufsize);

			n_dma_bufsize += remains;

			po = (pDevice->m_pDmaBaseCommonBuffer[(i % SC0710_DEF_MULTI_CHDMA_ROW) + 1]);



			po += SC0710_DEF_MULTI_CHDMA_PAGES_SIZE_4M_RESIDUE;


			po += (from % SC0710_DEF_MULTI_CHDMA_PAGES) * (p_sys_cfg->n_output_video_bufsize);

			po -= (remains);



			status = SC0710_UNPACK_MULTI_CHDMA_PAGES(pDevice, from, to, cxo, cyo, cwo, cxe, cye, cwe, po, pe, n_dma_bufsize, is_10bits);
		}
		else {

			LINUXV4L2_PRINT(KERN_INFO, "[CHDMA.X.%d][%03d ~ %03d] bufsize = %d / %d\n", i, from, to, n_dma_bufsize, remains);
		}
	}

	return status;
}
ULONG PARSE_HEAD(BYTE* po_begin, ULONG is_10bits)
{
	if(po_begin) {



		if(is_10bits) {

			PC5_SDI_HEADERS_10BITS p_head = (PC5_SDI_HEADERS_10BITS)(po_begin);

			if((p_head->magic_pattern[0] == 0xFF) &&

				(p_head->magic_pattern[1] == 0xFF) &&

				(p_head->magic_pattern[2] == 0xFF) &&

				(p_head->magic_pattern[3] == 0x3F) &&

				(p_head->magic_pattern[4] == 0x00)) {

				return TRUE;
			}
		}
		else {

			PC5_SDI_HEADERS p_head = (PC5_SDI_HEADERS)(po_begin);

			if((p_head->magic_pattern[0] == 0xFF) &&

				(p_head->magic_pattern[1] == 0xFF) &&

				(p_head->magic_pattern[2] == 0xFF) &&

				(p_head->magic_pattern[3] == 0x00)) {

				return TRUE;
			}
		}
	}
	return FALSE;
}

ULONG PARSE_CHANNEL(BYTE* po_begin, SC0710_SYS_CFG* p_sys_cfg, ULONG is_10bits)
{
	ULONG ch = 0;

	if(po_begin && p_sys_cfg) {

		if(is_10bits) {

			PC5_SDI_HEADERS_10BITS p_head = (PC5_SDI_HEADERS_10BITS)(po_begin);

			ch = p_head->channel;
		}
		else {

			PC5_SDI_HEADERS p_head = (PC5_SDI_HEADERS)(po_begin);

			if(p_sys_cfg->n_is_multi_channels_enabled > 0) {

				if(p_sys_cfg->n_is_multi_channels_enabled == 16) {

					ch = (p_head->channel_b3 << 3) | (p_head->channel_b2 << 2) | (p_head->channel << 0);

					ch %= 16;
				}
				else if(p_sys_cfg->n_is_multi_channels_enabled == 8) {

					ch = (p_head->channel_b2 << 2) | (p_head->channel << 0);

					ch %= 8;
				}
				else {

					ch = (p_head->channel);

					ch %= 4;
				}
			}
		}
	}
	return ch;
}

ULONG PARSE_LINE_NUMBER(BYTE* po_begin, ULONG is_10bits)
{
	if(po_begin) {

		if(is_10bits) {

			PC5_SDI_HEADERS_10BITS p_head = (PC5_SDI_HEADERS_10BITS)(po_begin);

			return (p_head->line_number_b9 << 9) | (p_head->line_number_b5 << 5) | (p_head->line_number_b0 << 0);
		}
		else {

			PC5_SDI_HEADERS p_head = (PC5_SDI_HEADERS)(po_begin);

			return p_head->line_number;
		}
	}
	return 0;
}

ULONG PARSE_VID_MODE(BYTE* po_begin, ULONG is_10bits)
{
	if(po_begin) {

		if(is_10bits) {

			PC5_SDI_HEADERS_10BITS p_head = (PC5_SDI_HEADERS_10BITS)(po_begin);

			return p_head->vid_mode;
		}
		else {

			PC5_SDI_HEADERS p_head = (PC5_SDI_HEADERS)(po_begin);

			return p_head->vid_mode;
		}
	}
	return 0;
}

ULONG PARSE_DATA_WIDTH_PIXEL(BYTE* po_begin, ULONG is_10bits)
{
	ULONG n_data_width = 0;

	if(po_begin) {

		if(is_10bits) {

			PC5_SDI_HEADERS_10BITS p_head = (PC5_SDI_HEADERS_10BITS)(po_begin);

			n_data_width = (p_head->width_b8 << 8) | (p_head->width_b0 << 0);
		}
		else {

			PC5_SDI_HEADERS p_head = (PC5_SDI_HEADERS)(po_begin);

			n_data_width = p_head->width;

		}
	}
	return n_data_width;
}

NTSTATUS SC0710_UNPACK_MULTI_CHDMA_PAGES(CDevice* pDevice, ULONG n_start_page_no, ULONG n_stop_page_no, ULONG cxo[16], ULONG cyo[16], ULONG cwo[16], ULONG cxe[16], ULONG cye[16], ULONG cwe[16], BYTE* po, BYTE* pe[16], ULONG n_dma_bufsize, ULONG is_10bits)
{
	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);



	ULONG n_header_size = (is_10bits) ? 10 : 8;

	ULONG n_padding_size = (is_10bits) ? 6 : 8;

	ULONG n_padding_mask_0 = (is_10bits) ? 0x0000FFFF : 0xFFFFFFFF;

	ULONG n_padding_mask_1 = (is_10bits) ? 0xFFFFFFFF : 0xFFFFFFFF;

	BYTE* po_begin = (BYTE*)(po);

	BYTE* po_end = NULL;

	BYTE* po_end_wo_hdr = (BYTE*)(po + n_dma_bufsize - n_header_size);

	BOOL   is_yuv420_support = FALSE;

	ULONG  n_max_width = (is_10bits) ? 5120 : 4096;

	ULONG  n_max_height = 2160;

	ULONG  n_no_head_cnt = 0;

	if(n_dma_bufsize > 0x7000000) { LINUXV4L2_PRINT(KERN_INFO, "n_dma_bufsize(0x%x) too large---, from(0x%x) to(0x%x)\n", n_dma_bufsize, n_start_page_no, n_stop_page_no); return STATUS_UNSUCCESSFUL; }

	if((pDevice->iManufacturer == 0x1A) ||

		(pDevice->iManufacturer == 0x1B)) {

		if(((pDevice->bcdDevice == 0x0B04) && (pDevice->iManufacturer == 0x1B))) {

			is_yuv420_support = TRUE;

			n_max_width = 4096;

			n_max_height = 2160;
		}
		else {

			is_yuv420_support = TRUE;

			n_max_width = 2048;

			n_max_height = 1200;
		}
	}
	if((pDevice->iManufacturer == 0x1C) ||

		(pDevice->iManufacturer == 0x1D)) {

		if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1C)) ||

			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1C)) ||

			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1D)) ||

			((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1D)) ||

			((pDevice->bcdDevice == 0x0B04) && (pDevice->iManufacturer == 0x1D)) ||

			((pDevice->bcdDevice == 0xB004) && (pDevice->iManufacturer == 0x1D))) {

			is_yuv420_support = TRUE;

			n_max_width = 4096;

			n_max_height = 2160;
		}
		else if(((pDevice->bcdDevice == 0xD004) && (pDevice->iManufacturer == 0x1C)) ||

			((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1C)) ||

			((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ||

			((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D))) {

			is_yuv420_support = TRUE;

			n_max_width = 4096;

			n_max_height = 2160;
		}
		else {

			is_yuv420_support = FALSE;

			n_max_width = 2048;

			n_max_height = 1200;
		}
	}
	if((pDevice->iManufacturer == 0x23) ||

		(pDevice->iManufacturer == 0x29) ||

		(pDevice->iManufacturer == 0x2B) ||

		(pDevice->iManufacturer == 0x2D) ||

		(pDevice->iManufacturer == 0x2B) ||

		(pDevice->iManufacturer == 0x2D) ||

		(pDevice->iManufacturer == 0x2D)) {

		if(((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B))) {

			is_yuv420_support = TRUE;

			n_max_width = 2048;

			n_max_height = 1200;
		}
		else {

			is_yuv420_support = TRUE;

			n_max_width = 4096;

			n_max_height = 2160;
		}
	}
	if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18))) {

		is_yuv420_support = TRUE;
	}
	if(((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F))) {

		is_yuv420_support = TRUE;
	}
	if((pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011)) {

		is_yuv420_support = TRUE;
	}
	if(pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty >= 3) {

		is_yuv420_support = TRUE;
	}

	while (po_begin < po_end_wo_hdr) {




		if(PARSE_HEAD(po_begin, is_10bits)) {


			ULONG ch_max = 3;

			ULONG ch = 0;

			ULONG   line = PARSE_LINE_NUMBER(po_begin, is_10bits);

			ULONG   n_vid_mode_fmt = (PARSE_VID_MODE(po_begin, is_10bits) >> 1);

			ULONG   n_vid_mode_color = (PARSE_VID_MODE(po_begin, is_10bits) & 0x01);

			ULONG   n_native_fmt = 0;

			ULONG   n_data_width = 0;

			BYTE* p_start = po_begin;

			ULONG* p_stop = NULL;

			BYTE*   offsets = NULL;

			ULONG   cho = 0;

			ULONG   r = 0;

			ULONG	n_vbi_lines = 0;

			CVideo* pVideo = pDevice->m_pVideo[ch];

			if(p_sys_cfg->n_is_multi_channels_enabled > 0) {

				if(p_sys_cfg->n_is_multi_channels_enabled == 16) {

					ch = PARSE_CHANNEL(po_begin, p_sys_cfg, is_10bits);

					ch_max = 15;
				}
				else if(p_sys_cfg->n_is_multi_channels_enabled == 8) {

					ch = PARSE_CHANNEL(po_begin, p_sys_cfg, is_10bits);

					ch_max = 7;
				}
				else {

					ch = PARSE_CHANNEL(po_begin, p_sys_cfg, is_10bits);

					ch_max = 3;
				}
			}

			if(((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0xA201) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x1E)) ||
				((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
				((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F))) {

				ch = 0;

				ch_max = 3;
			}

			cho = cyo[ch];

			if(is_10bits) {

				n_data_width = PARSE_DATA_WIDTH_PIXEL(po_begin, is_10bits) * 1;

				n_data_width = (n_native_fmt == 1) ? (n_data_width) : (n_data_width << 1);

				n_data_width = (n_data_width * 10) / 8;
			}
			else if(is_yuv420_support && n_vid_mode_fmt == 1) {

				n_native_fmt = 1;

				n_data_width = PARSE_DATA_WIDTH_PIXEL(po_begin, is_10bits) * 1;
			}
			else if(n_vid_mode_fmt == 2 ||

				n_vid_mode_fmt == 3) {

				n_native_fmt = n_vid_mode_fmt;

				n_data_width = PARSE_DATA_WIDTH_PIXEL(po_begin, is_10bits) * 3;
			}
			else {

				n_native_fmt = 0;

				n_data_width = PARSE_DATA_WIDTH_PIXEL(po_begin, is_10bits) * 2;
			}

			if(pVideo)
			{
				if(pVideo->m_nCustomAnalogVideoVbiLinesEnableProperty > 0 &&

					pVideo->m_nCustomAnalogVideoVbiLinesProperty > 0) {

					n_vbi_lines = pVideo->m_nCustomAnalogVideoVbiLinesProperty;
				}
			}

			if(p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[ch]) {

				if(ch == 0) { ch = 0; if(!(line & 0x01)) { offsets += cxo[0]; r = 1; } line = (line - 1) >> 1; line <<= 1; line += 1; }

				if(ch == 1) { ch = 0; if(!(line & 0x01)) { offsets += cxo[0]; r = 1; } line = (line - 1) >> 1; line <<= 1; line += 2; }

				if(ch == 2) { ch = 2; if(!(line & 0x01)) { offsets += cxo[2]; r = 1; } line = (line - 1) >> 1; line <<= 1; line += 1; }

				if(ch == 3) { ch = 2; if(!(line & 0x01)) { offsets += cxo[2]; r = 1; } line = (line - 1) >> 1; line <<= 1; line += 2; }

				cho = cyo[ch];

			}

			n_no_head_cnt = 0;


			if((line == 1) || (line == cho) || (line == (cho - 1)) || (line == cho*2) || (line == (cho*2 -1)))
			{
				if(pDevice->debug[ch + 4])
				{
					LINUXV4L2_PRINT(KERN_INFO, "---ch:%d, line:%d, data:%d, fmt:%d, color:%d, cho:%d, n_vbi_lines:%d", ch, line, n_data_width, n_vid_mode_fmt, n_vid_mode_color, cho, n_vbi_lines);
				}
			}



			if(n_native_fmt == 1) {

				if((n_data_width > ((n_max_width * 1) >> 0)) ||







					(n_data_width < ((400 * 1) >> 0))) {



					po_begin += 4;

					continue;
				}
			}
			else if(n_vid_mode_fmt == 2 ||

				n_vid_mode_fmt == 3) {

				if((n_data_width > ((n_max_width * 3) >> 0)) ||







					(n_data_width < ((400 * 3) >> 0))) {



					po_begin += 4;

					continue;
				}
			}
			else {

				if((n_data_width > ((n_max_width * 2) >> 0)) ||







					(n_data_width < ((400 * 2) >> 0))) {



					po_begin += 4;

					continue;
				}
			}


			p_stop = (ULONG*)(p_start + n_header_size + n_data_width);

			po_end = (BYTE*)(po + n_dma_bufsize);

			if((BYTE*)(p_stop + 2) > (po_end)) {



				break;
			}
			if(((p_stop[0] & n_padding_mask_0) != n_padding_mask_0) ||

				((p_stop[1] & n_padding_mask_1) != n_padding_mask_1)) {



				po_begin += 4;

				continue;
			}


			if(p_sys_cfg->n_input_video_resolution_interleaveds[ch] && cho > 0) {


				if(line == 1)
				{
					p_sys_cfg->n_input_video_resolution_interleaveds_field[ch] = *(p_start + n_header_size) & 1;
				}


				if(p_sys_cfg->n_input_video_resolution_interleaveds_field[ch] && (line <= cho))
				{
					line += cho;

					if(pDevice->debug[ch + 4])
					{

					}
				}
			}

			if(ch > ch_max) {



				po_begin += 4;

				continue;
			}

			if((line > n_max_height) || (line == 0)) {



				po_begin += 4;

				continue;
			}
			if(((p_stop[0] & n_padding_mask_0) == n_padding_mask_0) &&

				((p_stop[1] & n_padding_mask_1) == n_padding_mask_1)) {

#ifndef MT_LINECOPY

				pDevice->line_count[ch]++;
#endif
			}
			else {

				po_begin += 4;

				continue;
			}


			if(p_sys_cfg->n_input_video_resolution_interleaveds[ch]) {

				if(pDevice->m_pVideo[ch]->m_nDstFrameHeight == p_sys_cfg->n_input_video_resolution_cys[ch])
				{

				}
				else
				{
					cho <<= 1;
				}
			}

			if(n_native_fmt == 1) {

				CVideo* pVideo = pDevice->m_pVideo[ch];

				pVideo->m_native_fmt = n_native_fmt;

				if(pVideo)
				{


					if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YVU420) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12))
					{
						if(

							(cwe[ch] <= n_max_width) &&





							(cwe[ch] >= 400) &&

							(cho > 0)) {

							SC0710_LINECOPY_SPEEDUP* p_linecopy_speedup = NULL;

							if(p_sys_cfg->n_input_video_resolution_interleaveds[ch])
							{
								LINUXV4L2_PRINT(KERN_INFO, "interleaved no 420 format");
							}
							else
							{
								if(n_vid_mode_color == 0) {

									offsets += (line - 1) * (cwo[ch]);
								}
								else {


									line = ((line - 1) / 2) * 2 + 1;

									offsets += ((line - 1) / 2) * (cwo[ch]) + (cwo[ch] * cyo[ch]);
								}
							}

#ifdef MT_LINECOPY

							if(n_vid_mode_color == 0) {

								p_linecopy_speedup = (SC0710_LINECOPY_SPEEDUP*)(pDevice->p_linecopy_cfg[ch][pVideo->m_nFrameLastLine_cnt % MULTI_CH_RING] + 1 + (line - 1) * sizeof(SC0710_LINECOPY_SPEEDUP) / 4);
							}
							else
							{
								p_linecopy_speedup = (SC0710_LINECOPY_SPEEDUP*)(pDevice->p_linecopy_cfg[ch][pVideo->m_nFrameLastLine_cnt % MULTI_CH_RING] + 1 + cyo[ch] * sizeof(SC0710_LINECOPY_SPEEDUP) / 4 + (line - 1) * sizeof(SC0710_LINECOPY_SPEEDUP) / 8);
							}

							pDevice->line_count[ch]++;

							p_linecopy_speedup->pe = offsets;

							p_linecopy_speedup->po = p_start + n_header_size;

							p_linecopy_speedup->sz = ((n_vid_mode_fmt & 1) << 31) | ((n_vid_mode_color & 1) << 30) | (line << 16) | n_data_width;


							if((line >= (cho - 1)) && (n_vid_mode_color == 1))
							{
								if((pVideo->m_InNoVideoDPC != 1) && pDevice->m_eVideoStreamState[ch] && pDevice->m_nAnalogVideoDecoderStatusPropertys[ch] && pVideo->m_nCustomAnalogOutputVideoAudioProperty) { MultiChLastLine(pVideo, ch, cho); }
								else
								{
									if(pDevice->debug[ch + 4]) LINUXV4L2_DEBUG(KERN_ERR, "[%02d] CH(%d) m_InNoVideoDPC(0x%x) m_nAnalogVideoDecoderStatusPropertys[ch](0x%x)\n", pDevice->m_nKsDeviceNumber, ch, pVideo->m_InNoVideoDPC, pDevice->m_nAnalogVideoDecoderStatusPropertys[ch]);
								}
							}
#else
#endif
						}
					}




				}
			}
			else {

				CVideo* pVideo = pDevice->m_pVideo[ch];

				pVideo->m_native_fmt = n_native_fmt;

				if(pDevice->m_nMulti_Instance)
				{
					pDevice->m_pVideo[ch + 4]->m_native_fmt = n_native_fmt;
				}

				if(pVideo)
				{


					{


						if(

							(cwe[ch] <= n_max_width) &&

							(cwe[ch] >= 400) &&

							(cho > 0)) {

							SC0710_LINECOPY_SPEEDUP* p_linecopy_speedup = NULL;

							BOOL is_completed = FALSE;

							ULONG n_correct_line_count = cho + n_vbi_lines;

							if(p_sys_cfg->n_input_video_resolution_interleaveds[ch])
							{
								{



									if(line == 1)
									{
										offsets += 0;
									}
									else if(line <= (cho / 2))
									{
										offsets += (line - 1) * 2 * (cwo[ch] << 1);
									}
									else
									{
										ULONG reset_field_line = line - (cho / 2);

										offsets += (reset_field_line * 2 - 1) * (cwe[ch] << 1);
									}
								}
							}
							else
							{
								if(is_10bits) {

									offsets += (line - 1) * (((cwe[ch] * 2) * 10) / 8);
								}
								else
								{
									offsets += (line - 1) * (cwe[ch] << 1);
								}
							}

#ifdef MT_LINECOPY


							if(p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[ch])
							{
								p_linecopy_speedup = (SC0710_LINECOPY_SPEEDUP*)(pDevice->p_linecopy_cfg[ch][pVideo->m_nFrameLastLine_cnt % MULTI_CH_RING] + 1 + ((line - 1) * 2 + r) * sizeof(SC0710_LINECOPY_SPEEDUP) / 4);
							}
							else
							{
								p_linecopy_speedup = (SC0710_LINECOPY_SPEEDUP*)(pDevice->p_linecopy_cfg[ch][pVideo->m_nFrameLastLine_cnt % MULTI_CH_RING] + 1 + (line - 1) * sizeof(SC0710_LINECOPY_SPEEDUP) / 4);
							}

							pDevice->line_count[ch]++;

							p_linecopy_speedup->pe = offsets;

							p_linecopy_speedup->po = p_start + n_header_size;

							p_linecopy_speedup->sz = ((n_vid_mode_fmt & 1) << 31) | ((n_vid_mode_color & 1) << 30) | (line << 16) | n_data_width;

							if(p_sys_cfg->n_input_video_resolution_cxs[ch] == 1366)
							{
								p_linecopy_speedup->sz = ((n_vid_mode_fmt & 1) << 31) | ((n_vid_mode_color & 1) << 30) | (line << 16) | 2732;
							}




							if(p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[ch]) {

								if(((line == (cho + n_vbi_lines)) && (r == 1)) ||

									((line == (cho + n_vbi_lines - 1)) && (r == 1))) {

									is_completed = (r) ? TRUE : FALSE;
								}
							}
							else {



								if(line == (cho + n_vbi_lines)) {

									is_completed = TRUE;
								}
							}
							if(is_completed) {

								if((pVideo->m_InNoVideoDPC != 1) && pDevice->m_eVideoStreamState[ch] && pDevice->m_nAnalogVideoDecoderStatusPropertys[ch] && pVideo->m_nCustomAnalogOutputVideoAudioProperty) { MultiChLastLine(pVideo, ch, n_correct_line_count); }
								else
								{
									if(pDevice->debug[ch + 4]) LINUXV4L2_DEBUG(KERN_ERR, "[%02d] CH(%d) m_InNoVideoDPC(0x%x) m_nAnalogVideoDecoderStatusPropertys[ch](0x%x)\n", pDevice->m_nKsDeviceNumber, ch, pVideo->m_InNoVideoDPC, pDevice->m_nAnalogVideoDecoderStatusPropertys[ch]);
								}
							}
#else
#endif
						}
						else
						{
							if(pDevice->debug[ch + 4]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] cwe[ %d ](0d%d) cho(0d%d) ERROR \n", pDevice->m_nKsDeviceNumber, ch, cwe[ch], cho);
						}
					}




				}
			}



			po_begin = p_start + (n_header_size) + (n_data_width) + (n_padding_size);
		}
		else {

			po_begin++;

			n_no_head_cnt++;


			if(n_no_head_cnt > 8000)
			{
				if(pDevice->debug[0 + 4]) LINUXV4L2_PRINT(KERN_INFO, "NO HEAD \n");

				p_sys_cfg->n_multi_chdma_remains = 0;

				return STATUS_SUCCESS;
			}
		}
	}



	if(po_end > po_begin) {

		ULONG remains = (ULONG)(po_end - po_begin);





		if(remains < SC0710_DEF_MULTI_CHDMA_PAGES_SIZE_4M_RESIDUE) {

			if((n_stop_page_no % SC0710_DEF_MULTI_CHDMA_PAGES) == (SC0710_DEF_MULTI_CHDMA_PAGES - 1)) {



				ULONG j = (n_stop_page_no + 1) / (SC0710_DEF_MULTI_CHDMA_PAGES);

				BYTE* p_next_page = pDevice->m_pDmaBaseCommonBuffer[(j % SC0710_DEF_MULTI_CHDMA_ROW) + 1];



				p_next_page += SC0710_DEF_MULTI_CHDMA_PAGES_SIZE_4M_RESIDUE;

				RtlCopyMemory(p_next_page - remains, po_begin, remains);

				p_sys_cfg->n_multi_chdma_remains = remains;
			}
			else {

				p_sys_cfg->n_multi_chdma_remains = remains;
			}
		}
		else {


		}
	}

	return STATUS_SUCCESS;
}

void BlkCpy_YUYV_BGR(struct work_struct* work, ULONG portion)
{

#if defined(AME_SC0710) || defined(AME_SC0720)

	ULONG i = portion;

	BOOLEAN is_copy_allowed = FALSE;

	unsigned long flags = 0x00000000;

	CVideo* pVideo = container_of(work, struct CVideo, w_blkcpy_yuyv_bgr[i % 7]);

	struct CDevice* pDevice = pVideo->m_pDevice;

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	ULONG real_fps = pDevice->m_nCustomAnalogVideoFrameRateProperty;

	ULONG quantity_4M_block = (p_sys_cfg->n_output_video_bufsize + 0x3FFFFF) / 0x400000;

	CVideoBuffer* pVideoBuffer = (CVideoBuffer*)pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1];

	ULONG work_issued_completed = 0;

	BYTE* po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block) + 1 + i];
	BYTE* p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + ((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block * 32 + i * 32;
	ULONG result_index = 0;
	ULONG size = ((XDMA_RESULT*)p_result)->length;

	BYTE* pe = pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0];

	if(pDevice->debug[0]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] BlockCopy start, i(%d) m_nSubDeviceNumber(%d) size(%d)\n", pDevice->m_nKsDeviceNumber, i, pVideo->m_nSubDeviceNumber, size );


	{
		ULONG j = 0;
		for (j = 0; j < quantity_4M_block; j++)
		{
			work_issued_completed |= (0x010001 << j);
		}
	}

	spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);

	if(pVideo->m_Multi_Copy & (1 << i))
	{
		if(pVideoBuffer)
		{
			if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
			{
				if(pe)
				{
					pVideo->m_Multi_Copy |= (1 << (i + 8));

					is_copy_allowed = TRUE;
				}
				else
				{
					LINUXV4L2_PRINT(KERN_INFO, "[%02d] NO pe, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, i);
				}
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] %s pe state(%x) error, i(%d) m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, __func__, pVideoBuffer->buffer_state, i, pVideo->m_nSubDeviceNumber);


				pVideo->m_Multi_Copy = 0;

				if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
			}
		}
		else
		{
			LINUXV4L2_PRINT(KERN_INFO, "[%02d] NO pVideoBuffer, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, i);
		}
	}
	else
	{
		LINUXV4L2_PRINT(KERN_INFO, "[%02d] m_Multi_Copy(0x%x) WORK NOT QUEUED, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_Multi_Copy, i);
	}
	spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);

	if(is_copy_allowed)
	{

		if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_UYVY))
		{
			for (result_index = 0; result_index < i; result_index++)
			{
				p_result = pDevice->m_pDmaBaseCommonBuffer[0] + 0x100000 + ((pDevice->m_nVideoInterruptCount - 1) % 4) * quantity_4M_block * 32 + result_index * 32;

				pe += ((XDMA_RESULT*)p_result)->length;
			}

			if(pVideo->m_nCustomAnalogVideoVbiLinesEnableProperty)
			{
				if(i == 0)
				{
					if(pDevice->debug[0]) LINUXV4L2_DEBUG(KERN_INFO, "%x %x %x %x %x %x %x %x %x %x %x --\n", *(po + 0), *(po + 0 + 2), *(po + 0 + 4), *(po + 0 + 6), *(po + 0 + 8), *(po + 0 + 10), *(po + 0 + 12), *(po + 0 + 14),

						*(po + 0 + 16), *(po + 0 + 18), *(po + 0 + 20));
				}
			}
			RtlCopyMemory(pe, po, size);
		}

		spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);

		pVideo->m_Multi_Copy &= ~(1 << (i + 8));

		pVideo->m_Multi_Copy |= (1 << (i + 16));

		if (pVideo->m_Multi_Copy == work_issued_completed)
		{


			if (pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
			{
				ULONGLONG diff = 0;

				ULONG fps = 0;

				ULONGLONG now_ts = 0;

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				pVideoBuffer->vb.vb2_buf.timestamp = ktime_get_ns();
				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				do_gettimeofday(&pVideoBuffer->vb.timestamp);
				#else
				do_gettimeofday(&pVideoBuffer->vb.v4l2_buf.timestamp);
				#endif

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				now_ts = pVideoBuffer->vb.vb2_buf.timestamp / 1000;
				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				now_ts = pVideoBuffer->vb.timestamp.tv_sec * 1000000 + pVideoBuffer->vb.timestamp.tv_usec;
				#else
				now_ts = pVideoBuffer->vb.v4l2_buf.timestamp.tv_sec * 1000000 + pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec;
				#endif

				diff = now_ts - pDevice->previous_ts_preview[pVideo->m_nSubDeviceNumber];

				if ((pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF) == 0x77FFFFFF)
				{
					fps = real_fps;
				}
				else
				{
					fps = pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF;
				}

				if (fps > 1)
				{
					if ((ABS(diff) > ((1000000 + fps * 10000) / fps)) || (ABS(diff) < ((1000000 - fps * 10000) / fps)))
					{
						if (pDevice->m_nAnalogVideoDecoderStatusProperty) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] diff(llu%llu)(us) s_parm(%d) real_fps(%d)--i(%d)\n", pDevice->m_nKsDeviceNumber, ABS(diff), fps, real_fps, i);
					}
				}


				pDevice->previous_ts_preview[pVideo->m_nSubDeviceNumber] = now_ts;

				update_video_time_stamp(pDevice, pVideo, pVideoBuffer);

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				pVideoBuffer->vb.sequence = pVideo->m_nFrameNumber++;
				#else
				pVideoBuffer->vb.v4l2_buf.sequence = pVideo->m_nFrameNumber++;
				#endif

				p_sys_cfg->n_output_video_stream_frame_number[0]++;


				vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_DONE);

				if (pDevice->debug[0]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] finish a frame, i(%d) m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, i, pVideo->m_nSubDeviceNumber);
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] %s pe state(%x) error, i(%d) m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, __func__, pVideoBuffer->buffer_state, i, pVideo->m_nSubDeviceNumber);


				if (pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
			}


			pVideo->m_Multi_Copy = 0;
		}
		spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
	}


#endif
}

void BlkCpy_YUYV_BGR_0(struct work_struct* work)
{
	BlkCpy_YUYV_BGR(work, 0);
}
void BlkCpy_YUYV_BGR_1(struct work_struct* work)
{
	BlkCpy_YUYV_BGR(work, 1);
}
void BlkCpy_YUYV_BGR_2(struct work_struct* work)
{
	BlkCpy_YUYV_BGR(work, 2);
}
void BlkCpy_YUYV_BGR_3(struct work_struct* work)
{
	BlkCpy_YUYV_BGR(work, 3);
}
void BlkCpy_YUYV_BGR_4(struct work_struct* work)
{
	BlkCpy_YUYV_BGR(work, 4);
}
void BlkCpy_YUYV_BGR_5(struct work_struct* work)
{
	BlkCpy_YUYV_BGR(work, 5);
}
void BlkCpy_YUYV_BGR_6(struct work_struct* work)
{
	BlkCpy_YUYV_BGR(work, 6);
}


void BlkCpy_Y(struct work_struct* work, ULONG portion)
{

#if defined(AME_SC0710) || defined(AME_SC0720)

	ULONG i = portion;

	BOOLEAN is_copy_allowed = FALSE;

	unsigned long flags = 0x00000000;

	CVideo* pVideo = container_of(work, struct CVideo, w_blkcpy_y[i % 7]);

	struct CDevice* pDevice = pVideo->m_pDevice;


	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	ULONG real_fps = pDevice->m_nCustomAnalogVideoFrameRateProperty;

	ULONG cx = p_sys_cfg->n_output_video_resolution_cx;

	ULONG cy = p_sys_cfg->n_output_video_resolution_cy;

	ULONG quantity_4M_Y_block = ((cx * cy) + 0x3FFFFF) / 0x400000;
	ULONG quantity_4M_C_block = ((cx * cy / 2) + 0x3FFFFF) / 0x400000;

	ULONG size = (i < (quantity_4M_Y_block - 1)) ? 0x400000 : (cx * cy - 0x400000 * (quantity_4M_Y_block - 1));

	CVideoBuffer* pVideoBuffer = (CVideoBuffer*)pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1];

	ULONG work_issued_completed = 0;

	BYTE* po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * (quantity_4M_Y_block + quantity_4M_C_block)) + 1 + i];

	BYTE* pe = pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0];

	if(pDevice->debug[0]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] BlockCopyY start, i(%d) m_nSubDeviceNumber(%d) size(%d)\n", pDevice->m_nKsDeviceNumber, i, pVideo->m_nSubDeviceNumber, size);

	{
		ULONG j = 0;
		for (j = 0; j < (quantity_4M_Y_block + quantity_4M_C_block); j++)
		{
			work_issued_completed |= (0x010001 << j);
		}
	}

	spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);

	if(pVideo->m_Multi_Copy & (1 << i))
	{
		if(pVideoBuffer)
		{
			if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
			{
				if(pe)
				{
					pVideo->m_Multi_Copy |= (1 << (i + 8));

					is_copy_allowed = TRUE;
				}
				else
				{
					LINUXV4L2_PRINT(KERN_INFO, "[%02d] NO pe, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, i);
				}
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] %s pe state(%x) error, i(%d) m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, __func__, pVideoBuffer->buffer_state, i, pVideo->m_nSubDeviceNumber);


				if(pVideo->m_Multi_Copy)
				{
					pVideo->m_Multi_Copy = 0;

					if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
				}
			}
		}
		else
		{
			LINUXV4L2_PRINT(KERN_INFO, "[%02d] NO pVideoBuffer, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, i);
		}
	}
	else
	{
		LINUXV4L2_PRINT(KERN_INFO, "[%02d] m_Multi_Copy(0x%x) WORK NOT QUEUED, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_Multi_Copy, i);
	}
	spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);

	if(is_copy_allowed)
	{

		if(p_sys_cfg->n_output_video_color_bits == 4)
		{
			pe += i * 0x400000;

			RtlCopyMemory(pe, po, size);
		}

		spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);

		pVideo->m_Multi_Copy &= ~(1 << (i + 8));

		pVideo->m_Multi_Copy |= (1 << (i + 16));

		if(pVideo->m_Multi_Copy == work_issued_completed)
		{
			if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
			{
				ULONGLONG diff = 0;

				ULONG fps = 0;

				ULONGLONG now_ts = 0;

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				pVideoBuffer->vb.vb2_buf.timestamp = ktime_get_ns();
				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				do_gettimeofday(&pVideoBuffer->vb.timestamp);
				#else
				do_gettimeofday(&pVideoBuffer->vb.v4l2_buf.timestamp);
				#endif

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				now_ts = pVideoBuffer->vb.vb2_buf.timestamp / 1000;
				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				now_ts = pVideoBuffer->vb.timestamp.tv_sec * 1000000 + pVideoBuffer->vb.timestamp.tv_usec;
				#else
				now_ts = pVideoBuffer->vb.v4l2_buf.timestamp.tv_sec * 1000000 + pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec;
				#endif

				diff = now_ts - pDevice->previous_ts_preview[pVideo->m_nSubDeviceNumber];

				if((pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF) == 0x77FFFFFF)
				{
					fps = real_fps;
				}
				else
				{
					fps = pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF;
				}

				if(fps > 1)
				{
					if((ABS(diff) > ((1000000 + fps * 3000) / fps)) || (ABS(diff) < ((1000000 - fps * 3000) / fps)))
					{
						if(pDevice->m_nAnalogVideoDecoderStatusProperty) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] diff(llu%llu)(us) s_parm(%d) real_fps(%d)--i(%d)\n", pDevice->m_nKsDeviceNumber, ABS(diff), fps, real_fps, i);
					}
				}


				pDevice->previous_ts_preview[pVideo->m_nSubDeviceNumber] = now_ts;

				update_video_time_stamp(pDevice, pVideo, pVideoBuffer);

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				pVideoBuffer->vb.sequence = pVideo->m_nFrameNumber++;
				#else
				pVideoBuffer->vb.v4l2_buf.sequence = pVideo->m_nFrameNumber++;
				#endif

				p_sys_cfg->n_output_video_stream_frame_number[ 0 ]++;

				vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_DONE);

				if(pDevice->debug[0]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] finish a 420 frame, i(%d) m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, i, pVideo->m_nSubDeviceNumber);
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] %s pe state(%x) error, i(%d) m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, __func__, pVideoBuffer->buffer_state, i, pVideo->m_nSubDeviceNumber);


				if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
			}


			pVideo->m_Multi_Copy = 0;
		}

		spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
	}


#endif
}

void BlkCpy_Y_0(struct work_struct* work)
{
	BlkCpy_Y(work, 0);
}
void BlkCpy_Y_1(struct work_struct* work)
{
	BlkCpy_Y(work, 1);
}
void BlkCpy_Y_2(struct work_struct* work)
{
	BlkCpy_Y(work, 2);
}

void BlkCpy_Y_3(struct work_struct* work)
{

#ifdef AME_SC0710

#endif
}

void BlkCpy_Y_4(struct work_struct* work)
{

#ifdef AME_SC0710

#endif
}

void BlkCpy_Y_5(struct work_struct* work)
{

#ifdef AME_SC0710

#endif
}

void BlkCpy_Y_6(struct work_struct* work)
{

#ifdef AME_SC0710

#endif
}



void BlkCpy_C_NV12(struct work_struct* work,ULONG portion)
{

#if defined(AME_SC0710) || defined(AME_SC0720)

	ULONG i = portion;

	BOOLEAN is_copy_allowed = FALSE;

	unsigned long flags = 0x00000000;

	CVideo* pVideo = container_of(work, struct CVideo, w_blkcpy_nv12[i % 7]);

	struct CDevice* pDevice = pVideo->m_pDevice;


	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	ULONG real_fps = pDevice->m_nCustomAnalogVideoFrameRateProperty;

	ULONG cx = p_sys_cfg->n_output_video_resolution_cx;

	ULONG cy = p_sys_cfg->n_output_video_resolution_cy;

	ULONG quantity_4M_Y_block = ((cx * cy) + 0x3FFFFF) / 0x400000;
	ULONG quantity_4M_C_block = ((cx * cy / 2) + 0x3FFFFF) / 0x400000;

	ULONG size = (i < (quantity_4M_C_block - 1)) ? 0x400000 : (cx * cy / 2 - 0x400000 * (quantity_4M_C_block - 1));

	CVideoBuffer* pVideoBuffer = (CVideoBuffer*)pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1];

	ULONG work_issued_completed = 0;

	BYTE* po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * (quantity_4M_Y_block + quantity_4M_C_block)) + quantity_4M_Y_block + 1 + i];

	BYTE* pe = pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0];

	if(pDevice->debug[0]) LINUXV4L2_DEBUG( KERN_INFO, "[%02d] BlockCopyC start, i(%d) m_nSubDeviceNumber(%d) size(%d)\n", pDevice->m_nKsDeviceNumber, i, pVideo->m_nSubDeviceNumber, size );

	{
		ULONG j = 0;
		for (j = 0; j < (quantity_4M_Y_block + quantity_4M_C_block); j++)
		{
			work_issued_completed |= (0x010001 << j);
		}
	}

	spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);

	if(pVideo->m_Multi_Copy & (1 << (i + quantity_4M_Y_block)))
	{
		if(pVideoBuffer)
		{
			if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
			{
				if(pe)
				{
					pVideo->m_Multi_Copy |= (1 << ((i + quantity_4M_Y_block) + 8));

					is_copy_allowed = TRUE;
				}
				else
				{
					LINUXV4L2_PRINT(KERN_INFO, "[%02d] NO pe, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, i);
				}
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] %s pe state(%x) error, i(%d) m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, __func__, pVideoBuffer->buffer_state, i, pVideo->m_nSubDeviceNumber);


				if(pVideo->m_Multi_Copy)
				{
					pVideo->m_Multi_Copy = 0;

					if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
				}
			}
		}
		else
		{
			LINUXV4L2_PRINT(KERN_INFO, "[%02d] NO pVideoBuffer, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, i);
		}
	}
	else
	{
		LINUXV4L2_PRINT(KERN_INFO, "[%02d] m_Multi_Copy(0x%x) WORK NOT QUEUED, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_Multi_Copy, i);
	}
	spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);

	if(is_copy_allowed)
	{

		if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12)
		{
			pe += (cx * cy) + i * 0x400000;

			RtlCopyMemory(pe, po, size);
		}

		spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);

		pVideo->m_Multi_Copy &= ~(1 << ((i + quantity_4M_Y_block) + 8));

		pVideo->m_Multi_Copy |= (1 << ((i + quantity_4M_Y_block) + 16));

		if(pVideo->m_Multi_Copy == work_issued_completed)
		{

			if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
			{
				ULONGLONG diff = 0;

				ULONG fps = 0;

				ULONGLONG now_ts = 0;

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				pVideoBuffer->vb.vb2_buf.timestamp = ktime_get_ns();
				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				do_gettimeofday(&pVideoBuffer->vb.timestamp);
				#else
				do_gettimeofday(&pVideoBuffer->vb.v4l2_buf.timestamp);
				#endif

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				now_ts = pVideoBuffer->vb.vb2_buf.timestamp / 1000;
				#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				now_ts = pVideoBuffer->vb.timestamp.tv_sec * 1000000 + pVideoBuffer->vb.timestamp.tv_usec;
				#else
				now_ts = pVideoBuffer->vb.v4l2_buf.timestamp.tv_sec * 1000000 + pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec;
				#endif

				diff = now_ts - pDevice->previous_ts_preview[pVideo->m_nSubDeviceNumber];

				if((pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF) == 0x77FFFFFF)
				{
					fps = real_fps;
				}
				else
				{
					fps = pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF;
				}

				if(fps > 1)
				{
					if((ABS(diff) > ((1000000 + fps * 3000) / fps)) || (ABS(diff) < ((1000000 - fps * 3000) / fps)))
					{
						if(pDevice->m_nAnalogVideoDecoderStatusProperty) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] diff(llu%llu)(us) s_parm(%d) real_fps(%d)--i(%d)\n", pDevice->m_nKsDeviceNumber, ABS(diff), fps, real_fps, i);
					}
				}


				pDevice->previous_ts_preview[pVideo->m_nSubDeviceNumber] = now_ts;

				update_video_time_stamp(pDevice, pVideo, pVideoBuffer);

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				pVideoBuffer->vb.sequence = pVideo->m_nFrameNumber++;
				#else
				pVideoBuffer->vb.v4l2_buf.sequence = pVideo->m_nFrameNumber++;
				#endif
				p_sys_cfg->n_output_video_stream_frame_number[ 0 ]++;

				vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_DONE);

				if(pDevice->debug[0]) LINUXV4L2_PRINT( KERN_INFO, "[%02d] finish a NV12 frame, i(%d)\n", pDevice->m_nKsDeviceNumber, i );
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] %s pe state(%x) error, i(%d) m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, __func__, pVideoBuffer->buffer_state, i, pVideo->m_nSubDeviceNumber);


				if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
			}


			pVideo->m_Multi_Copy = 0;
		}
		spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
	}


#endif
}

void BlkCpy_C_NV12_0(struct work_struct* work)
{
	BlkCpy_C_NV12(work, 0);
}
void BlkCpy_C_NV12_1(struct work_struct* work)
{
	BlkCpy_C_NV12(work, 1);
}

void BlkCpy_C_NV12_2(struct work_struct* work)
{

#ifdef AME_SC0710

#endif
}

void BlkCpy_C_NV12_3(struct work_struct* work)
{

#ifdef AME_SC0710

#endif
}

void BlkCpy_C_NV12_4(struct work_struct* work)
{

#ifdef AME_SC0710

#endif
}

void BlkCpy_C_NV12_5(struct work_struct* work)
{

#ifdef AME_SC0710

#endif
}

void BlkCpy_C_NV12_6(struct work_struct* work)
{

#ifdef AME_SC0710

#endif
}

void BlkCpy_C_YV12(struct work_struct* work, ULONG portion)
{

#if defined(AME_SC0710) || defined(AME_SC0720)

	ULONG i = portion;

	BOOLEAN is_copy_allowed = FALSE;

	unsigned long flags = 0x00000000;

	CVideo* pVideo = container_of(work, struct CVideo, w_blkcpy_yv12[i % 7]);

	struct CDevice* pDevice = pVideo->m_pDevice;


	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	ULONG real_fps = pDevice->m_nCustomAnalogVideoFrameRateProperty;

	ULONG cx = p_sys_cfg->n_output_video_resolution_cx;

	ULONG cy = p_sys_cfg->n_output_video_resolution_cy;

	ULONG quantity_4M_Y_block = ((cx * cy) + 0x3FFFFF) / 0x400000;
	ULONG quantity_4M_C_block = ((cx * cy / 2) + 0x3FFFFF) / 0x400000;

	ULONG quantity_2M_C_block = ((cx * cy / 2) + 0x1FFFFF) / 0x200000;

	ULONG size = (i < (quantity_2M_C_block - 1)) ? 0x200000 : (cx * cy / 2 - 0x200000 * (quantity_2M_C_block - 1));

	BYTE* po = pDevice->m_pDmaBaseCommonBuffer[(((pDevice->m_nVideoInterruptCount - 1) % 4) * (quantity_4M_Y_block + quantity_4M_C_block)) + quantity_4M_Y_block + 1 + (i / 2)] + 0x200000 * (i % 2);
	BYTE* pe = pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0];

	CVideoBuffer* pVideoBuffer = (CVideoBuffer*)pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1];

	ULONG work_issued_completed = 0;

	if(pDevice->debug[0]) LINUXV4L2_PRINT( KERN_INFO, "[%02d] BlockCopyC start, i(%d) m_nSubDeviceNumber(%d) size(%d)\n", pDevice->m_nKsDeviceNumber, i, pVideo->m_nSubDeviceNumber, size );

	{
		ULONG j = 0;
		for (j = 0; j < (quantity_4M_Y_block + quantity_2M_C_block); j++)
		{
			work_issued_completed |= (0x010001 << j);
		}
	}

	spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);

	if(pVideo->m_Multi_Copy & (1 << (i + quantity_4M_Y_block)))
	{
		if(pVideoBuffer)
		{
			if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
			{
				if(pe)
				{
					pVideo->m_Multi_Copy |= (1 << ((i + quantity_4M_Y_block) + 8));

					is_copy_allowed = TRUE;
				}
				else
				{
					LINUXV4L2_PRINT(KERN_INFO, "[%02d] NO pe, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, i);
				}
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] %s pe state(%x) error, i(%d) m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, __func__, pVideoBuffer->buffer_state, i, pVideo->m_nSubDeviceNumber);


				if(pVideo->m_Multi_Copy)
				{
					pVideo->m_Multi_Copy = 0;

					if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
				}
			}
		}
		else
		{
			LINUXV4L2_PRINT(KERN_INFO, "[%02d] NO pVideoBuffer, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, i);
		}
	}
	else
	{
		LINUXV4L2_PRINT(KERN_INFO, "[%02d] m_Multi_Copy(0x%x) WORK NOT QUEUED, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_Multi_Copy, i);
	}
	spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);

	if(is_copy_allowed)
	{


		BYTE* pe_v = NULL;
		BYTE* pe_u = NULL;
		ULONG c_count = 0;

		pe += (cx * cy) + i * 0x200000 / 2;


		if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
		{
			pe_u = pe;
			pe_v = pe + (cx * cy) / 4;

			for (c_count = 0; c_count < size; c_count += 2)
			{
				*pe_u++ = *po++;
				*pe_v++ = *po++;
			}
		}
		else
		{
			pe_v = pe;
			pe_u = pe + (cx * cy) / 4;

			for (c_count = 0; c_count < size; c_count += 2)
			{
				*pe_u++ = *po++;
				*pe_v++ = *po++;
			}
		}

		spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);

		pVideo->m_Multi_Copy &= ~(1 << ((i + quantity_4M_Y_block) + 8));

		pVideo->m_Multi_Copy |= (1 << ((i + quantity_4M_Y_block) + 16));

		if(pVideo->m_Multi_Copy == work_issued_completed)
		{


			if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
			{
				ULONGLONG diff = 0;

				ULONG fps = 0;

				ULONGLONG now_ts = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				pVideoBuffer->vb.vb2_buf.timestamp = ktime_get_ns();
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				do_gettimeofday(&pVideoBuffer->vb.timestamp);
#else
				do_gettimeofday(&pVideoBuffer->vb.v4l2_buf.timestamp);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				now_ts = pVideoBuffer->vb.vb2_buf.timestamp / 1000;
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				now_ts = pVideoBuffer->vb.timestamp.tv_sec * 1000000 + pVideoBuffer->vb.timestamp.tv_usec;
#else
				now_ts = pVideoBuffer->vb.v4l2_buf.timestamp.tv_sec * 1000000 + pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec;
#endif

				diff = now_ts - pDevice->previous_ts_preview[pVideo->m_nSubDeviceNumber];

				if((pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF) == 0x77FFFFFF)
				{
					fps = real_fps;
				}
				else
				{
					fps = pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF;
				}

				if(fps > 1)
				{
					if((ABS(diff) > ((1000000 + fps * 3000) / fps)) || (ABS(diff) < ((1000000 - fps * 3000) / fps)))
					{
						if(pDevice->m_nAnalogVideoDecoderStatusProperty) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] diff(llu%llu)(us) s_parm(%d) real_fps(%d)--i(%d)\n", pDevice->m_nKsDeviceNumber, ABS(diff), fps, real_fps, i);
					}
				}

				pDevice->previous_ts_preview[pVideo->m_nSubDeviceNumber] = now_ts;

				update_video_time_stamp(pDevice, pVideo, pVideoBuffer);

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				pVideoBuffer->vb.sequence = pVideo->m_nFrameNumber++;
				#else
				pVideoBuffer->vb.v4l2_buf.sequence = pVideo->m_nFrameNumber++;
				#endif

				p_sys_cfg->n_output_video_stream_frame_number[ 0 ]++;

				vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_DONE);


			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] %s pe state(%x) error, i(%d) m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, __func__, pVideoBuffer->buffer_state, i, pVideo->m_nSubDeviceNumber);


				if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
			}

			pVideo->m_Multi_Copy = 0;
		}
		spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
	}


#endif
}

void BlkCpy_C_YV12_0(struct work_struct* work)
{
	BlkCpy_C_YV12(work, 0);
}

void BlkCpy_C_YV12_1(struct work_struct* work)
{
	BlkCpy_C_YV12(work, 1);
}

void BlkCpy_C_YV12_2(struct work_struct* work)
{
	BlkCpy_C_YV12(work, 2);
}

void BlkCpy_C_YV12_3(struct work_struct* work)
{

#ifdef AME_SC0710

#endif
}

void BlkCpy_C_YV12_4(struct work_struct* work)
{

#ifdef AME_SC0710

#endif
}

void BlkCpy_C_YV12_5(struct work_struct* work)
{

#ifdef AME_SC0710

#endif
}

void BlkCpy_C_YV12_6(struct work_struct* work)
{

#ifdef AME_SC0710

#endif
}

void BlkCpy_P010(struct work_struct* work, ULONG portion)
{
#if defined(AME_SC0710) || defined(AME_SC0720)

	ULONG i = portion;

	BOOLEAN is_copy_allowed = FALSE;

	unsigned long flags = 0x00000000;

	CVideo* pVideo = container_of(work, struct CVideo, w_blkcpy_p010[i % 7]);

	struct CDevice* pDevice = pVideo->m_pDevice;


	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	ULONG real_fps = pDevice->m_nCustomAnalogVideoFrameRateProperty;

	ULONG cx = p_sys_cfg->n_output_video_resolution_cx;

	ULONG cy = p_sys_cfg->n_output_video_resolution_cy;

	ULONG quantity_4M_block = (((cx * cy * 15) >> 3) + 0x3FFFFF) / 0x400000;

	BYTE* pe = pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][0];

	CVideoBuffer* pVideoBuffer = (CVideoBuffer*)pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % 4][1];

	ULONG work_issued_completed = 0;



	{
		ULONG j = 0;
		for (j = 0; j < quantity_4M_block; j++)
		{
			work_issued_completed |= (0x010001 << j);
		}
	}

	spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);

	if(pVideo->m_Multi_Copy & (1 << i))
	{
		if(pVideoBuffer)
		{
			if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
			{
				if(pe)
				{
					pVideo->m_Multi_Copy |= (1 << (i + 8));

					is_copy_allowed = TRUE;
				}
				else
				{
					LINUXV4L2_PRINT(KERN_INFO, "[%02d] NO pe, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, i);
				}
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] %s pe state(%x) error, i(%d) m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, __func__, pVideoBuffer->buffer_state, i, pVideo->m_nSubDeviceNumber);
			}
		}
		else
		{
			LINUXV4L2_PRINT(KERN_INFO, "[%02d] NO pVideoBuffer, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, i);
		}
	}
	else
	{
		LINUXV4L2_PRINT(KERN_INFO, "[%02d] m_Multi_Copy(0x%x) WORK NOT QUEUED, ERROR i(%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_Multi_Copy, i);
	}
	spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);

	if(is_copy_allowed)
	{
		ULONG cwo = cx;
		ULONG cwe = cx;
		ULONG cxo = cx;

		ULONG cye = pVideo->m_nDstFrameHeight;

		ULONG c_line_quantity_in_4M_block = ((cxo == 3840) ? 291 : 273);

		BYTE* pe_y = pe + i * c_line_quantity_in_4M_block * 4 * cwo;

		BYTE* pe_c = pe + (cwe * cye * 2) + i * c_line_quantity_in_4M_block * 2 * cwo;

		ULONG n_src_pitch = (cwo * 5) / 4;

		ULONG n_dst_pitch = (cwe * 2);

		ULONG pixels = cxo / 4;

		if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
		{
			ULONG x = 0;

			BYTE* po = NULL;

			BYTE* po_y = NULL;

			ULONG vertical_in_4M_Block = 0;

			ULONG vertical = 0;

			if(cxo == 3840)
			{
				if(i <= 2)
				{
					vertical_in_4M_Block = 873;
				}
				else
				{
					vertical_in_4M_Block = 621;
				}
			}
			else
			{
				if(i <= 2)
				{
					vertical_in_4M_Block = 819;
				}
				else
				{
					vertical_in_4M_Block = 783;
				}
			}


			po = pDevice->m_pDmaBaseCommonBuffer[(((pVideo->m_nFrameNumberForQueue - 1) % 4) * quantity_4M_block) + 1 + i];
			po_y = po;

			for (vertical = 0; vertical < vertical_in_4M_Block; vertical += 3)
			{
				BYTE* po_y_line1 = (BYTE*)(po_y);

				BYTE* po_y_line2 = (BYTE*)(po_y + n_src_pitch);

				BYTE* po_c_line1 = (BYTE*)(po_y_line2 + n_src_pitch);

				USHORT* pe_y_line1 = (USHORT*)(pe_y);

				USHORT* pe_y_line2 = (USHORT*)(pe_y + n_dst_pitch);

				USHORT* pe_c_line1 = (USHORT*)(pe_c);

				for (x = 0; x < pixels; x++) {

					USHORT* p0 = (USHORT*)(po_y_line1 + 0);

					USHORT* p1 = (USHORT*)(po_y_line1 + 1);

					USHORT* p2 = (USHORT*)(po_y_line1 + 2);

					USHORT* p3 = (USHORT*)(po_y_line1 + 3);

					USHORT* p4 = (USHORT*)(po_y_line2 + 0);

					USHORT* p5 = (USHORT*)(po_y_line2 + 1);

					USHORT* p6 = (USHORT*)(po_y_line2 + 2);

					USHORT* p7 = (USHORT*)(po_y_line2 + 3);

					USHORT* c0 = (USHORT*)(po_c_line1 + 0);

					USHORT* c1 = (USHORT*)(po_c_line1 + 1);

					USHORT* c2 = (USHORT*)(po_c_line1 + 2);

					USHORT* c3 = (USHORT*)(po_c_line1 + 3);

					pe_y_line1[0] = (p0[0] & 0x03FF) << 6;

					pe_y_line1[1] = (p1[0] & 0x0FFC) << 4;

					pe_y_line1[2] = (p2[0] & 0x3FF0) << 2;

					pe_y_line1[3] = (p3[0] & 0xFFC0) << 0;

					pe_y_line2[0] = (p4[0] & 0x03FF) << 6;

					pe_y_line2[1] = (p5[0] & 0x0FFC) << 4;

					pe_y_line2[2] = (p6[0] & 0x3FF0) << 2;

					pe_y_line2[3] = (p7[0] & 0xFFC0) << 0;

					pe_c_line1[0] = (c0[0] & 0x03FF) << 6;

					pe_c_line1[1] = (c1[0] & 0x0FFC) << 4;

					pe_c_line1[2] = (c2[0] & 0x3FF0) << 2;

					pe_c_line1[3] = (c3[0] & 0xFFC0) << 0;

					po_y_line1 += 5;

					po_y_line2 += 5;

					po_c_line1 += 5;

					pe_y_line1 += 4;

					pe_y_line2 += 4;

					pe_c_line1 += 4;
				}

				po_y += (n_src_pitch * 3);

				pe_y += (n_dst_pitch * 2);

				pe_c += (n_dst_pitch * 1);
			}
		}
		else
		{
			LINUXV4L2_PRINT(KERN_INFO, "[%02d] %s pe state(%x) error, i(%d) m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, __func__, pVideoBuffer->buffer_state, i, pVideo->m_nSubDeviceNumber);
		}


		spin_lock_irqsave(&pDevice->m_sMultiCopySpinLock, flags);

		pVideo->m_Multi_Copy &= ~(1 << (i + 8));

		pVideo->m_Multi_Copy |= (1 << (i + 16));

		if(pVideo->m_Multi_Copy == work_issued_completed)
		{


			if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
			{
				ULONGLONG diff = 0;

				ULONG fps = 0;

				ULONGLONG now_ts = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				pVideoBuffer->vb.vb2_buf.timestamp = ktime_get_ns();
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				do_gettimeofday(&pVideoBuffer->vb.timestamp);
#else
				do_gettimeofday(&pVideoBuffer->vb.v4l2_buf.timestamp);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				now_ts = pVideoBuffer->vb.vb2_buf.timestamp / 1000;
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				now_ts = pVideoBuffer->vb.timestamp.tv_sec * 1000000 + pVideoBuffer->vb.timestamp.tv_usec;
#else
				now_ts = pVideoBuffer->vb.v4l2_buf.timestamp.tv_sec * 1000000 + pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec;
#endif

				diff = now_ts - pDevice->previous_ts_preview[pVideo->m_nSubDeviceNumber];

				if((pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF) == 0x77FFFFFF)
				{
					fps = real_fps;
				}
				else
				{
					fps = pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF;
				}

				if(fps > 1)
				{
					if((ABS(diff) > ((1000000 + fps * 3000) / fps)) || (ABS(diff) < ((1000000 - fps * 3000) / fps)))
					{
						if(pDevice->m_nAnalogVideoDecoderStatusProperty) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] diff(llu%llu)(us) s_parm(%d) real_fps(%d)--i(%d)\n", pDevice->m_nKsDeviceNumber, ABS(diff), fps, real_fps, i);
					}
				}

				pDevice->previous_ts_preview[pVideo->m_nSubDeviceNumber] = now_ts;

				update_video_time_stamp(pDevice, pVideo, pVideoBuffer);

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
				pVideoBuffer->vb.sequence = pVideo->m_nFrameNumber++;
				#else
				pVideoBuffer->vb.v4l2_buf.sequence = pVideo->m_nFrameNumber++;
				#endif

				p_sys_cfg->n_output_video_stream_frame_number[ 0 ]++;

				vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_DONE);


			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] %s pe state(%x) error, i(%d) m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, __func__, pVideoBuffer->buffer_state, i, pVideo->m_nSubDeviceNumber);


			}

			pVideo->m_Multi_Copy = 0;
		}
		spin_unlock_irqrestore(&pDevice->m_sMultiCopySpinLock, flags);
	}


#endif
}

void BlkCpy_P010_0(struct work_struct* work)
{
	BlkCpy_P010(work, 0);
}

void BlkCpy_P010_1(struct work_struct* work)
{
	BlkCpy_P010(work, 1);
}

void BlkCpy_P010_2(struct work_struct* work)
{
	BlkCpy_P010(work, 2);
}

void BlkCpy_P010_3(struct work_struct* work)
{
	BlkCpy_P010(work, 3);
}

void BlkCpy_P010_4(struct work_struct* work)
{
}

void BlkCpy_P010_5(struct work_struct* work)
{
}

void BlkCpy_P010_6(struct work_struct* work)
{
}



void SC0710_PowerSaving(CDevice* pDevice, BOOL THREAD)
{
#if defined(AME_SC0710)

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	if(THREAD == FALSE) { SC0710_CLOSE_THREAD(); }

	LINUXV4L2_PRINT(KERN_INFO, "[%02d] bcdDevice(0x%x) iManufacturer(0x%x) n_mcu_version(0d%d) n_firmware_version(0d%d)", pDevice->m_nKsDeviceNumber, pDevice->bcdDevice, pDevice->iManufacturer, p_sys_cfg->n_mcu_version, p_sys_cfg->n_firmware_version);


	if(pDevice->m_nCustomPowerSavingProperty)
	{
		if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||

			((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||

			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x15))) {

			if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16) && (p_sys_cfg->n_mcu_version >= 210630)) ||

				((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16) && (p_sys_cfg->n_mcu_version >= 210622)) ||

				((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x15) && (p_sys_cfg->n_mcu_version >= 210617) && (p_sys_cfg->n_firmware_version >= 20210524))) {

				if(pDevice->iManufacturer == 0x16)
				{
					SC0710_DownloadC5Firmware_Online(pDevice, "SC0710.FWB.HEX", pDevice->m_nCustomPowerSavingProperty);

					LINUXV4L2_PRINT(KERN_INFO, "[%02d] POWER DOWN C5", pDevice->m_nKsDeviceNumber);
				}

				if((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16))
				{
					Disable_EQ(pDevice);

					LINUXV4L2_PRINT(KERN_INFO, "[%02d] POWER DOWN EQ", pDevice->m_nKsDeviceNumber);
				}

				{
					unsigned char txbuf[2] = { 2, 0 };

					unsigned char rxbuf[2] = { 0, 0 };

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, 0x32, txbuf, 1, rxbuf, 1);

					txbuf[1] = rxbuf[0];

					txbuf[1] &= 0xFE;

					txbuf[1] |= pDevice->m_nCustomPowerSavingProperty;

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, 0x32, txbuf, 2, NULL, 0);

					LINUXV4L2_PRINT(KERN_INFO, "[%02d] POWER DOWN MCU", pDevice->m_nKsDeviceNumber);

				}

				{
					ULONG R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

					R000000EC &= ~2;

					R000000EC |= (pDevice->m_nCustomPowerSavingProperty << 1);

					SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);

					LINUXV4L2_PRINT(KERN_INFO, "[%02d] POWER DOWN X", pDevice->m_nKsDeviceNumber);
				}
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] NO POWERSAVING SUPPORTED", pDevice->m_nKsDeviceNumber);
			}
		}
	}
	else
	{
		if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||

			((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||

			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x15))) {

			if(((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16) && (p_sys_cfg->n_mcu_version >= 210630)) ||

				((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16) && (p_sys_cfg->n_mcu_version >= 210622)) ||

				((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x15) && (p_sys_cfg->n_mcu_version >= 210617) && (p_sys_cfg->n_firmware_version >= 20210524))) {

				ULONG R000000EC = SC0710_GetRegister(pDevice, 0x000000EC);

				R000000EC &= ~2;

				R000000EC |= (pDevice->m_nCustomPowerSavingProperty << 1);

				SC0710_SetRegister(pDevice, 0x000000EC, R000000EC);

				LINUXV4L2_PRINT(KERN_INFO, "[%02d] POWER UP X, delay 300ms", pDevice->m_nKsDeviceNumber);

				DELAY_100NS(3000000);

				{
					BYTE txbuf[2] = { 2, 0 };

					BYTE rxbuf[2] = { 0, 0 };

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, 0x32, txbuf, 1, rxbuf, 1);

					txbuf[1] = rxbuf[0];

					txbuf[1] &= 0xFE;

					txbuf[1] |= (BYTE)pDevice->m_nCustomPowerSavingProperty;

					SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, 0x32, txbuf, 2, NULL, 0);

					LINUXV4L2_PRINT(KERN_INFO, "[%02d] POWER UP MCU", pDevice->m_nKsDeviceNumber);
				}

				if((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16))
				{
					Enable_EQ(pDevice);

					LINUXV4L2_PRINT(KERN_INFO, "[%02d] POWER UP EQ", pDevice->m_nKsDeviceNumber);
				}

				if(pDevice->iManufacturer == 0x16)
				{
					SC0710_DownloadC5Firmware_Online(pDevice, "SC0710.FWB.HEX", pDevice->m_nCustomPowerSavingProperty);

					LINUXV4L2_PRINT(KERN_INFO, "[%02d] POWER UP C5", pDevice->m_nKsDeviceNumber);
				}
			}
			else
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] NO POWERSAVING SUPPORTED", pDevice->m_nKsDeviceNumber);
			}
		}
	}

	if(THREAD == FALSE) { SC0710_START_THREAD(); }
#endif
}

#define COPY_4K_NV12 1
extern BYTE NOSG_LOGO_YV12[115200];

void MultiCh_MultiCopy(struct work_struct* work, ULONG portion)
{


#if defined(AME_SC0710) || defined(AME_SC0720)

	ULONG i = portion;

	struct CVideo* pVideo = container_of(work, struct CVideo, w_multich_multicopy[i]);

	ULONG ch = 0;

	ULONG cfg = 0;

	CVideoBuffer* pVideoBuffer = NULL;

	BYTE* pe = NULL;

	unsigned long flags = 0x00000000;

	ULONG* p_linecopy_cfg = NULL;

	struct CDevice* pDevice = pVideo->m_pDevice;

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	ULONG cx = 0;

	ULONG cy = 0;

	ULONG vertical_line = 0;

	SC0710_LINECOPY_SPEEDUP* p_linecopy_speedup = NULL;

	ULONG ch_quantity = (pDevice->bcdDevice & 0xF);

	if(ch_quantity)
	{
		ch = pVideo->m_nSubDeviceNumber % ch_quantity;
	}


	cx = pVideo->m_nDstFrameWidth;

	cy = pVideo->m_nDstFrameHeight;

	pVideoBuffer = (CVideoBuffer*)pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % MULTI_CH_RING][1];

	if(pVideo == NULL)
	{
		LINUXV4L2_PRINT(KERN_ERR, "MultiCh_MultiCopy_%d() pVideo == NULL\n", i);

		return;
	}

	if(pVideoBuffer == NULL)
	{
		LINUXV4L2_PRINT(KERN_ERR, "MultiCh_MultiCopy_%d() pVideoBuffer == NULL\n", i);

		return;
	}

	p_linecopy_cfg = pDevice->p_linecopy_cfg[ch][(pVideo->m_nFrameNumberForQueue - 1) % MULTI_CH_RING];

	p_linecopy_speedup = (SC0710_LINECOPY_SPEEDUP*)(p_linecopy_cfg + 1);


	if(pVideo->m_native_fmt == 1)
	{
		p_linecopy_speedup += (i <= 2) ? (i * cy / 2) : (cy + cy / 4);

		vertical_line = (i <= 1) ? (cy / 2) : (cy / 4);

		if(i == 3)
		{
			if(cy % 4)
			{
				vertical_line += (cy / 2) % 2;
			}
		}
	}
	else
	{

		if(p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[ch])
		{

			p_linecopy_speedup += (i * (cy / 2));

			vertical_line = (cy / 2);
		}
		else
		{
			p_linecopy_speedup += (i * (cy / 4));

			vertical_line = (cy / 4);

			if(i == 3)
			{
				if(cy % 4)
				{
					vertical_line += cy % 4;
				}
			}
		}
	}

	if(pDevice->debug[ch + 4]) LINUXV4L2_DEBUG(KERN_INFO, "MultiCh_MultiCopy start ch(0d%d) i(0d%d) m_native_fmt(0d%d) vertical_line(0d%d) p_linecopy_speedup(0x%llx)\n",

		ch, i, pVideo->m_native_fmt, vertical_line, p_linecopy_speedup);

	spin_lock_irqsave(&pVideo->m_sMultiCh_MultiCopySpinLock, flags);

	if(pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder] & (1 << i))
	{

	}
	else
	{
		LINUXV4L2_PRINT(KERN_ERR, "[%02d] MultiCh_MultiCopy_0(0x%x) WORK NOT QUEUED, ERROR ch(%d) i(%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder], ch, i);
	}


	pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder] |= (1 << (i + 8));



	spin_unlock_irqrestore(&pVideo->m_sMultiCh_MultiCopySpinLock, flags);

	pe = vb2_plane_vaddr(&pVideoBuffer->vb2_buffer_cross, 0);
	if(pe)
	{
		#ifdef COPY_4K_NV12
		for (cfg = 0; cfg < vertical_line; cfg++)
		{
			ULONG	n_vid_mode_fmt = (p_linecopy_speedup->sz >> 31) & 0x1;

			ULONG   n_vid_mode_color = (p_linecopy_speedup->sz >> 30) & 0x1;

			ULONG	line = (p_linecopy_speedup->sz >> 16) & 0x3FFF;

			ULONG	n_data_width = (p_linecopy_speedup->sz) & 0xFFFF;







			if((pDevice->m_eVideoStreamState[ch] == 0) || (pVideoBuffer->buffer_state != VB2_BUF_STATE_ACTIVE)) { if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR); return; }

			if(n_vid_mode_fmt == 1)
			{



				if(p_linecopy_speedup->po == NULL)
				{
					LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) cfg(%d) po == NULL\n", pDevice->m_nKsDeviceNumber, ch, cfg);

					break;

				}
				else if(n_data_width != cx)
				{
					LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) cfg(%d) n_data_width(0d%d) error, correct(%d)\n", pDevice->m_nKsDeviceNumber, ch, cfg, n_data_width, pVideo->m_nDstFrameWidth);

					break;
				}
				else
				{
					if(n_vid_mode_color == 1)
					{
						BYTE* pe_c = (BYTE*) ((ULONGLONG)((cx * cy) + (line - 1) / 2 * cx));

						if(p_linecopy_speedup->pe != pe_c)
						{
							LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) cfg(%d) pe(0x%llx) error, correct(0x%llx) C\n", pDevice->m_nKsDeviceNumber, ch, cfg, p_linecopy_speedup->pe, pe_c);

							break;
						}
						else
						{
							if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12)
							{
								RtlCopyMemory(pe + (ULONGLONG)(p_linecopy_speedup->pe), p_linecopy_speedup->po, n_data_width);
							}
							else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YVU420)
							{
								ULONG horizontal = 0;

								BYTE* pe_v = NULL;

								BYTE* pe_u = NULL;

								pe_v = pe + cx * cy + (line - 1) * cx / 4;

								pe_u = pe_v + cx * cy / 4;

								for (horizontal = 0; horizontal < n_data_width; horizontal += 2)
								{
									*pe_u++ = *(p_linecopy_speedup->po)++;
									*pe_v++ = *(p_linecopy_speedup->po)++;
								}
							}
							else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
							{
								ULONG horizontal = 0;

								BYTE* pe_v = NULL;

								BYTE* pe_u = NULL;

								pe_u = pe + cx * cy + (line - 1) * cx / 4;

								pe_v = pe_u + cx * cy / 4;

								for (horizontal = 0; horizontal < n_data_width; horizontal += 2)
								{
									*pe_u++ = *(p_linecopy_speedup->po)++;
									*pe_v++ = *(p_linecopy_speedup->po)++;
								}
							}
							else
							{
								LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) UNSUPPORTED FORMAT(0x%x)\n", pDevice->m_nKsDeviceNumber, ch, pVideo->m_nFramePixelFormat);
							}
						}
					}

					if(n_vid_mode_color == 0)
					{
						if(p_linecopy_speedup->pe != (BYTE*)(ULONGLONG)((line - 1) * cx))
						{
							LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) cfg(%d) pe(0d%d) error, correct(%d) Y\n", pDevice->m_nKsDeviceNumber, ch, cfg, p_linecopy_speedup->pe, (line - 1) * cx);

							break;
						}
						else
						{
							if((pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YVU420) || (pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420))
							{
								RtlCopyMemory(pe + (ULONGLONG)(p_linecopy_speedup->pe), p_linecopy_speedup->po, n_data_width);
							}
							else
							{
								LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) UNSUPPORTED FORMAT(0x%x)\n", pDevice->m_nKsDeviceNumber, ch, pVideo->m_nFramePixelFormat);
							}
						}
					}
				}
			}
			else
			{

				ULONG offsets = 0;

				ULONG correct_data_width = cx * 2;

				if(p_sys_cfg->n_input_video_resolution_interleaveds[ch])
				{





					{
						if((cfg + i * cy / 4) < (cy / 2))
						{


							offsets = (cfg + i * cy / 4) * 2 * cx * 2;
						}
						else
						{




							ULONG reset_field_line = (cfg + i * cy / 4) + 1 - (cy / 2);

							offsets = (reset_field_line * 2 - 1) * cx * 2;
						}
					}
				}
				else
				{

					if(p_sys_cfg->n_input_video_signal_dual_link_under_6Gs[ch])
					{
						offsets = i * cx * 2 * (cy / 4) + (cfg / 2) * cx * 2 + (cfg % 2) * cx;

						correct_data_width = cx;
					}
					else
					{
						if(p_sys_cfg->n_output_video_color_bits == 5)
						{
							offsets = i * (((cx * 2) * 10) / 8) * (cy / 4) + cfg * (((cx * 2) * 10) / 8);

							correct_data_width = (correct_data_width * 10) >> 3;
						}
						else
						{
							offsets = i * cx * 2 * (cy / 4) + cfg * cx * 2;
						}
					}
				}


				if(p_linecopy_speedup->pe != (BYTE*)(ULONGLONG)offsets)
				{
					LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) i(%d) cfg(%d) pe(0d%d) error, correct(%d)\n", pDevice->m_nKsDeviceNumber, ch, i, cfg, p_linecopy_speedup->pe, offsets);

					break;
				}
				else if(p_linecopy_speedup->po == NULL)
				{
					LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) cfg(%d) po == NULL\n", pDevice->m_nKsDeviceNumber, ch, cfg);

					break;
				}
				else if(n_data_width != correct_data_width)
				{
					LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) i(%d) cfg(%d) n_data_width(0d%d) error, correct(%d)\n", pDevice->m_nKsDeviceNumber, ch, i, cfg, n_data_width, correct_data_width);

					break;
				}
				else
				{
					if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUYV)
					{
						RtlCopyMemory(pe + (ULONGLONG)(p_linecopy_speedup->pe), p_linecopy_speedup->po, n_data_width);
					}
					else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YUV420)
					{
						ULONG horizontal = 0;

						BYTE* pe_y = pe + (ULONGLONG)(p_linecopy_speedup->pe) / 2;

						BYTE* pe_u = pe + cx * cy + (ULONGLONG)(p_linecopy_speedup->pe) / 8;

						BYTE* pe_v = pe_u + (cx * cy) / 4;


						for (horizontal = 0; horizontal < n_data_width; horizontal += 2)
						{
							*pe_y++ = *(p_linecopy_speedup->po + horizontal);
						}

						if(cx)
						{
							if((((ULONGLONG)(p_linecopy_speedup->pe) / (cx * 2)) % 2) == 0)
							{

								for (horizontal = 0; horizontal < n_data_width; horizontal += 4)
								{
									*pe_u++ = *(p_linecopy_speedup->po + horizontal + 1);
								}


								for (horizontal = 0; horizontal < n_data_width; horizontal += 4)
								{
									*pe_v++ = *(p_linecopy_speedup->po + horizontal + 3);
								}
							}
						}
					}
					else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_YVU420)
					{
						ULONG horizontal = 0;

						BYTE* pe_y = pe + (ULONGLONG)(p_linecopy_speedup->pe) / 2;

						BYTE* pe_v = pe + cx * cy + (ULONGLONG)(p_linecopy_speedup->pe) / 8;

						BYTE* pe_u = pe_v + (cx * cy) / 4;


						for (horizontal = 0; horizontal < n_data_width; horizontal += 2)
						{
							*pe_y++ = *(p_linecopy_speedup->po + horizontal);
						}

						if(cx)
						{
							if((((ULONGLONG)(p_linecopy_speedup->pe) / (cx * 2)) % 2) == 0)
							{

								for (horizontal = 0; horizontal < n_data_width; horizontal += 4)
								{
									*pe_v++ = *(p_linecopy_speedup->po + horizontal + 3);
								}


								for (horizontal = 0; horizontal < n_data_width; horizontal += 4)
								{
									*pe_u++ = *(p_linecopy_speedup->po + horizontal + 1);
								}
							}
						}
					}
					else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_NV12)
					{
						ULONG horizontal = 0;

						BYTE* pe_y = pe + (ULONGLONG)(p_linecopy_speedup->pe) / 2;

						BYTE* pe_uv = pe + cx * cy + (ULONGLONG)(p_linecopy_speedup->pe) / 4;


						for (horizontal = 0; horizontal < n_data_width; horizontal += 2)
						{
							*pe_y++ = *(p_linecopy_speedup->po + horizontal);
						}

						if(cx)
						{
							if((((ULONGLONG)(p_linecopy_speedup->pe) / (cx * 2)) % 2) == 0)
							{

								for (horizontal = 0; horizontal < n_data_width; horizontal += 2)
								{
									*pe_uv++ = *(p_linecopy_speedup->po + horizontal + 1);
								}
							}
						}
					}
					else if(pVideo->m_nFramePixelFormat == V4L2_PIX_FMT_P010)
					{


						ULONG n_dst_pitch = cx * 2 ;

						ULONG pixels = n_data_width / 5;

						ULONG y_offset = (i * vertical_line + cfg);

						ULONG uv_offset = (i * vertical_line + cfg) / 2 + cy;

						USHORT *pe_y = (USHORT*) (pe + y_offset * n_dst_pitch);

						BYTE * po_line = (BYTE *)(p_linecopy_speedup->po);

						ULONG x = 0;

						if(cfg%2)
						{
							for( x = 0 ; x < pixels ; x++ )
							{

								USHORT * p0 = (USHORT *)(po_line + 0);

								USHORT * p1 = (USHORT *)(po_line + 2);

								*pe_y++ = (p0[ 0 ] & 0x03FF) << 6;

								*pe_y++ = (p1[ 0 ] & 0x3FF0) << 2;

								po_line += 5;
							}
						}
						else
						{
							USHORT *pe_uv = (USHORT*) (pe + uv_offset * n_dst_pitch);

							for( x = 0 ; x < pixels ; x++ )
							{

								{
									USHORT * p0 = (USHORT *)(po_line + 0);

									USHORT * p1 = (USHORT *)(po_line + 2);

									*pe_y++ = (p0[ 0 ] & 0x03FF) << 6;

									*pe_y++ = (p1[ 0 ] & 0x3FF0) << 2;
								}


								{
									USHORT * pc0 = (USHORT *)(po_line + 1);

									USHORT * pc1 = (USHORT *)(po_line + 3);

									*pe_uv++ = (pc0[ 0 ] & 0x0FFC) << 4;

									*pe_uv++ = (pc1[ 0 ] & 0xFFC0) << 0;
								}

								po_line += 5;
							}
						}
					}
					else
					{
						LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) UNSUPPORTED FORMAT(0x%x)\n", pDevice->m_nKsDeviceNumber, ch, pVideo->m_nFramePixelFormat);
					}
				}
			}

			p_linecopy_speedup++;

		}
		#endif
	}
	else
	{
		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] ch(%d) NO pe(0x%x)\n", pDevice->m_nKsDeviceNumber, ch, i);
	}

	if(pDevice->debug[ch + 4]) LINUXV4L2_DEBUG(KERN_INFO, "MultiCh_MultiCopy end ch(0d%d) i(0d%d)\n", ch, i);

	spin_lock_irqsave(&pVideo->m_sMultiCh_MultiCopySpinLock, flags);

	pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder] &= ~(1 << (i + 8));

	pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder] |= (1 << (i + 16));

	if(pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder] == 0x0F000F)
	{
		ULONGLONG diff = 0;

		ULONGLONG now_ts = 0;

		ULONG fps = 0;

		ULONG real_fps = p_sys_cfg->n_output_video_resolution_fpss[ch];

		if((pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF) == 0x77FFFFFF)
		{
			fps = real_fps;
		}
		else
		{
			fps = pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF;
		}



		if(pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE)
		{






			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)

			pVideoBuffer->vb.vb2_buf.timestamp = ktime_get_ns();

			now_ts = pVideoBuffer->vb.vb2_buf.timestamp / 1000;

			#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)

			do_gettimeofday(&pVideoBuffer->vb.timestamp);

			now_ts = pVideoBuffer->vb.timestamp.tv_sec * 1000000 + pVideoBuffer->vb.timestamp.tv_usec;
			#else
			do_gettimeofday(&pVideoBuffer->vb.v4l2_buf.timestamp);

			now_ts = pVideoBuffer->vb.v4l2_buf.timestamp.tv_sec * 1000000 + pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec;
			#endif

			diff = now_ts - pDevice->previous_ts[ch];

			if(fps > 1)
			{
				if((ABS(diff) > ((1000000 + fps * 10000) / fps)) || (ABS(diff) < ((1000000 - fps * 10000) / fps)))
				{
					LINUXV4L2_DEBUG( KERN_INFO,"[%02d] ch(%d) diff(llu%llu)(us) fps(%d) --\n", pDevice->m_nKsDeviceNumber, ch, ABS(diff), fps );







				}
			}
			pDevice->previous_ts[ch] = now_ts;


			update_video_time_stamp(pDevice, pVideo, pVideoBuffer);

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
			pVideoBuffer->vb.sequence = pVideo->m_nFrameNumber++;
			#else
			pVideoBuffer->vb.v4l2_buf.sequence = pVideo->m_nFrameNumber++;
			#endif

			p_sys_cfg->n_output_video_stream_frame_number[ ch ]++;

			if((pDevice->m_eVideoStreamState[ch] == 1) && (pVideoBuffer->buffer_state == VB2_BUF_STATE_ACTIVE))
			{
				vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_DONE);
			}
			else
			{
				vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
			}
			if(pDevice->debug[ch + 4]) LINUXV4L2_PRINT(KERN_INFO, "[%02d] ch(%d) i(%d) finish a frame, sequence(%lld)\n", pDevice->m_nKsDeviceNumber, ch, i, pVideo->m_nFrameNumber);

		}
		else
		{
			LINUXV4L2_PRINT(KERN_ERR, "[%02d] ch(%d) WRONG STATE(0x%x)\n", pDevice->m_nKsDeviceNumber, ch, pVideoBuffer->buffer_state);

			if(pVideoBuffer) vb2_buffer_done(&pVideoBuffer->vb2_buffer_cross, VB2_BUF_STATE_ERROR);
		}


		RtlFillMemory((BYTE*)pDevice->p_linecopy_cfg[ch][(pVideo->m_nFrameNumberForQueue - 1) % MULTI_CH_RING], SC0710_SPEED_LINE_COPY_BUFFER_SIZE, 0x00);

		pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder] = 0;

		pVideo->m_pTmpVBuf[(pVideo->m_nFrameNumberForQueue - 1) % MULTI_CH_RING][1] = NULL;

		spin_unlock_irqrestore(&pVideo->m_sMultiCh_MultiCopySpinLock, flags);

		wake_up(&(pVideo->m_wait_queue_parsing_lines));

	}
	else
	{
		spin_unlock_irqrestore(&pVideo->m_sMultiCh_MultiCopySpinLock, flags);
	}



#endif
}

void MultiCh_MultiCopy_0(struct work_struct* work)
{
	MultiCh_MultiCopy(work, 0);
}
void MultiCh_MultiCopy_1(struct work_struct* work)
{
	MultiCh_MultiCopy(work, 1);
}
void MultiCh_MultiCopy_2(struct work_struct* work)
{
	MultiCh_MultiCopy(work, 2);
}
void MultiCh_MultiCopy_3(struct work_struct* work)
{
	MultiCh_MultiCopy(work, 3);
}


NTSTATUS SC0710_Enable_Dolby(CDevice* pDevice, ULONG dolby)
{
	LINUXV4L2_PRINT(KERN_INFO, "SC0710_Enable_Dolby() dolby(0x%x)\n", dolby);

	if((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18))
	{
		ULONG R000000D0 = SC0710_GetRegister(pDevice, 0x000000D0);

		if(dolby)
			R000000D0 |= 0x00008000;
		else
			R000000D0 &= ~0x00008000;

		SC0710_SetRegister(pDevice, 0x000000D0, R000000D0);

		if(dolby)
		{
			unsigned char tx[2] = { 3, 0 };

			unsigned char rx[2] = { 0, 0 };

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, tx, 1, rx, 1);

			tx[1] = rx[0];

			tx[1] |= 0x04;

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, tx, 2, NULL, 0);
		}
		else
		{
			unsigned char tx[2] = { 3, 0 };

			unsigned char rx[2] = { 0, 0 };

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, tx, 1, rx, 1);

			tx[1] = rx[0];

			tx[1] &= ~0x04;

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, tx, 2, NULL, 0);
		}
	}
	return STATUS_SUCCESS;
}

VOID SC0710_background_updated(CDevice* pDevice)
{
	ULONG i = 0;

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	BOOL is_fpga_exist = TRUE;

	p_sys_cfg->b_is_background_updated = FALSE;

	LINUXV4L2_DEBUG(KERN_INFO, "[%02d] start background\n", pDevice->m_nKsDeviceNumber);

	if (pDevice->iManufacturer == 0x1B) {

		BYTE buf[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

		SC0710_StartC5FlashAccess(pDevice);

		set_chip_sel(pDevice, 0);

		set_chip_sel(pDevice, 1);

		set_chip_sel(pDevice, 0);

		buf[0] = CMD_C5_READ_ID;

		spi_tx(pDevice, 4, buf);

		spi_rx(pDevice, 4, buf + 4);

		set_chip_sel(pDevice, 1);

		SC0710_StopC5FlashAccess(pDevice);

		if (memcmp(buf + 4, c5m2_id, 4) == 0) {


			SC0710_DownloadC5Firmware_Online(pDevice, "SC0710.FWA.HEX", 0);
		}
		else {

			;
		}
	}
	if (pDevice->iManufacturer == 0x1A ||

		pDevice->iManufacturer == 0x1B) {

#if 0
		BYTE buf[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

		SC0710_StartC5FlashAccess(pDevice);

		set_chip_sel(pDevice, 0);

		set_chip_sel(pDevice, 1);

		set_chip_sel(pDevice, 0);

		buf[0] = CMD_C5_READ_ID;

		spi_tx(pDevice, 4, buf);

		spi_rx(pDevice, 4, buf + 4);

		set_chip_sel(pDevice, 1);

		SC0710_StopC5FlashAccess(pDevice);

		if (RtlCompareMemory(buf + 4, c7m2_id_3, 4) == 4) {

			WCHAR pwsz[64] = L"SC0710.FWE.HEX";

			SC0710_DownloadC5Firmware_Online(pDevice, pwsz);
		}
		else {

			;
		}
#else
		if ((pDevice->bcdDevice & 0xF) == 0x0002) {


			SC0710_DownloadC5Firmware_Online(pDevice, "SC0710.FWE.HEX", 0);
		}
		if ((pDevice->bcdDevice & 0xF) == 0x0004) {


			SC0710_DownloadC5Firmware_Online(pDevice, "SC0710.FWA.HEX", 0);
		}
#endif
	}

	if (((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||
		((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||
		((pDevice->bcdDevice == 0xB401) && (pDevice->iManufacturer == 0x16)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x16)) ||

		((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0xB201) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x18))) {

		SC0710_DownloadC5Firmware_Online(pDevice, "SC0710.FWB.HEX", 0);
	}
	if ((pDevice->iManufacturer == 0x1C) ||

		(pDevice->iManufacturer == 0x1D) ||

		(pDevice->iManufacturer == 0x1C) ||

		(pDevice->iManufacturer == 0x1C) ||

		(pDevice->iManufacturer == 0x1D)) {

		if (((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1D)) ||
			((pDevice->bcdDevice == 0x0B04) && (pDevice->iManufacturer == 0x1D))) {

			SC0710_DownloadC7Firmware_Online(pDevice, "SC0710.FWC.HEX");

		}
		else {

			is_fpga_exist = FALSE;
		}
	}
	if ((pDevice->iManufacturer == 0x1E) ||

		(pDevice->iManufacturer == 0x1F)) {

		if (((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0xA201) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x1101) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x1201) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x1301) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0501) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0601) && (pDevice->iManufacturer == 0x1F))) {

			is_fpga_exist = FALSE;
		}
		else {

			SC0710_DownloadC5Firmware_Online(pDevice, "SC0710.FWD.HEX", 0);
		}
	}
	if (((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||
		((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D))) {



		ULONG R000000CC = 0x00000000;

		R000000CC = SC0710_GetRegister(pDevice, 0x000000CC);

		R000000CC &= ~(1 << 12);

		SC0710_SetRegister(pDevice, 0x000000CC, R000000CC);

		SC0710_DownloadC7Firmware_Online(pDevice, "SC072A.HEX");

		DELAY_100NS(1000000);

		R000000CC |= (1 << 12);

		SC0710_SetRegister(pDevice, 0x000000CC, R000000CC);

		SC0710_DownloadC7Firmware_Online(pDevice, "SC072A.HEX");

		DELAY_100NS(10000000);


	}
	if (((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x17))) {



		SC0710_DownloadC7Firmware_Online(pDevice, "SC0710.FWF.HEX");

		DELAY_100NS(10000000);


	}

	if (((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x13)) ||

		((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x13))) {

		if (pDevice->m_nAnalogCrossbarVideoInputProperty == 0) {



			SC0710_DownloadC5Firmware_Online(pDevice, "SC0710.FWG.HEX", 0);


			DELAY_100NS(2000000);
		}
		if (pDevice->m_nAnalogCrossbarVideoInputProperty == 4) {



			SC0710_DownloadC5Firmware_Online(pDevice, "SC0710.FWH.HEX", 0);



			DELAY_100NS(2000000);
		}
	}


	if (false
		|| ((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D))
		|| ((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x17))
		|| ((pDevice->idVendor == 0x12AB) && (pDevice->idProduct & 0xFFFF) == 0x0750)
		) {
		is_fpga_exist = FALSE;
	}
	if (is_fpga_exist)
	{
		ULONG is_fw_download = 0;

		do {

			unsigned char txbuf[4] = { 0x12, 0x34, 0x5B, 0 };

			unsigned char rxbuf[4] = { 0, 0, 0, 0 };

			int n_fpga_version = 0;

			int n_fpga_found = 0;

			for (i = 0; i < 10; i++) {

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, 0x55, txbuf, 3, rxbuf, 3);

				n_fpga_version = (rxbuf[0] * 10000) + (rxbuf[1] * 100) + (rxbuf[2]);

				LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwInitialize( %02d: C7(%d)\n", pDevice->m_nKsDeviceNumber, i, n_fpga_version);

				if (n_fpga_version == 240449) {

					n_fpga_version = 0;

					n_fpga_found = 0;

					break;
				}

				if ((rxbuf[0] >= 0x19 && rxbuf[0] <= 0x29) &&

					(rxbuf[1] >= 0x01 && rxbuf[1] <= 0x12) &&

					(rxbuf[2] >= 0x01 && rxbuf[2] <= 0x31)) {

					n_fpga_found = 1;

					break;
				}
				DELAY_100NS(10000000);
			}
			if (n_fpga_found) {

				p_sys_cfg->n_fpga_version = n_fpga_version;
			}
			else {

				p_sys_cfg->n_fpga_version = 0;

				if (is_fw_download++ == 0) {
#if 0
					if (pDevice->iManufacturer == 0x18) {

						WCHAR pwsz[64] = L"\\??\\C:\\WINDOWS\\SC0710\\SC0710.FWB.HEX";

						SC0710_DownloadM2Firmware(pDevice, pwsz);
					}
#else
					break;
#endif
				}
				else {

					break;
				}
			}

		} while (p_sys_cfg->n_fpga_version == 0);
	}


































	if (p_sys_cfg->n_mcu_version > 0) {

		ULONG R00000008 = SC0710_GetRegister(pDevice, 0x00000008);

		p_sys_cfg->n_firmware_version = R00000008;

		p_sys_cfg->n_firmware_version_year = (R00000008 >> 24) & 0x000000FF;

		p_sys_cfg->n_firmware_version_month = (R00000008 >> 16) & 0x000000FF;

		p_sys_cfg->n_firmware_version_date = (R00000008 >> 8) & 0x000000FF;

		p_sys_cfg->n_firmware_version_sub = (R00000008 >> 0) & 0x000000FF;

		LINUXV4L2_PRINT(KERN_INFO, "[%02d] SC0710_HwInitialize( xlx(0x%x)\n", pDevice->m_nKsDeviceNumber, R00000008);
	}



	if (((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||
		((pDevice->bcdDevice == 0xB401) && (pDevice->iManufacturer == 0x16)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0xB201) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0x0204) && (pDevice->iManufacturer == 0x29)) ||
		((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x29)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2B)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2D))) {

		unsigned char rxbuf[2] = { 0x00, 0x00 };

		unsigned char txbuf[2] = { 0x01, 0x00 };

		BOOL is_reg_updated = FALSE;

		SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 1, rxbuf, 1);

		usleep_range(5000, 6000);

		txbuf[1] = rxbuf[0];

		if (pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

			if ((txbuf[1] & 0x80) == 0x00) { is_reg_updated = TRUE; }

			txbuf[1] &= ~0x20;

			txbuf[1] |= 0x90;
		}
		else {

			if ((txbuf[1] & 0x80) == 0x80) { is_reg_updated = TRUE; }

			txbuf[1] &= ~0x80;
		}

		if (is_reg_updated) {

			SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);

		}

		LINUXV4L2_DEBUG(KERN_INFO, "txbuf[ 1 ] (0x%x)", txbuf[1]);
	}
	if (((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x13)) ||
		((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x13)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||


		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x16)) ||

		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x18))) {

		unsigned char rxbuf[2] = { 0x00, 0x00 };

		unsigned char txbuf[2] = { 0x01, 0x00 };

		SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 1, rxbuf, 1);

		DELAY_100NS(100000);

		txbuf[1] = rxbuf[0];

		txbuf[1] &= ~0x80;

		SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, SC0710_I2C_MCU_ADR7_0X31, txbuf, 2, NULL, 0);
	}

	if ((pDevice->iManufacturer == 0x1C) ||

		(pDevice->iManufacturer == 0x1D) ||

		(pDevice->iManufacturer == 0x1E) ||

		(pDevice->iManufacturer == 0x1F) ||

		(pDevice->iManufacturer == 0x2D)) {

		if (pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0) {

			ULONG i = 0;

			ULONG counts = (pDevice->bcdDevice & 0xFF);

			for (i = 0; i < counts; i++) {

				if (pDevice->m_nCustomCompanyMedicalProperty == 0) {

					SC0710_EnableHDCP(pDevice, 1, i);
				}
				else {

					SC0710_EnableHDCP(pDevice, 0, i);
				}
			}
		}
		else {

			if (pDevice->m_nCustomCompanyMedicalProperty == 0) {

				SC0710_EnableHDCP(pDevice, 1, 0);
			}
			else {

				SC0710_EnableHDCP(pDevice, 0, 0);
			}
		}
	}
	else {

		if ((pDevice->m_nAnalogCrossbarVideoInputProperty == 0) || (pDevice->m_nAnalogCrossbarVideoInputProperty == 1))
		{
			if (pDevice->m_nCustomCompanyMedicalProperty == 0) {

				SC0710_EnableHDCP(pDevice, 1, 0);
			}
			else {

				SC0710_EnableHDCP(pDevice, 0, 0);
			}
		}
	}
	{

		SC0710_EnableHDR2SDR(pDevice, pDevice->m_nCustomAnalogVideoHDR2SDRProperty);
	}

	if (((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x15))) {

		SC0710_EnableScaleDown(pDevice);
	}

	if (((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x17))) {

		SC0710_EnableScaleDown(pDevice);
	}
	if (((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x17))) {

		SC0710_MuteLoopThrough(pDevice, pDevice->m_nCustomAnalogVideoLoopThroughMuteProperty);
	}

	if (((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0xA201) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
		((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F)) ||
		((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1F)) ||
		((pDevice->bcdDevice == 0x1101) && (pDevice->iManufacturer == 0x1F)) ||
		((pDevice->bcdDevice == 0x1201) && (pDevice->iManufacturer == 0x1F)) ||
		((pDevice->bcdDevice == 0x1301) && (pDevice->iManufacturer == 0x1F)) ||
		((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x1F)) ||
		((pDevice->bcdDevice == 0x0501) && (pDevice->iManufacturer == 0x1F)) ||
		((pDevice->bcdDevice == 0x0601) && (pDevice->iManufacturer == 0x1F)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F))) {

		SC0710_SetAnalogCrossbarRouteProperty(pDevice, 0, pDevice->m_nAnalogCrossbarVideoInputPropertys[0], pDevice->m_nAnalogCrossbarVideoOutputProperty, pDevice->m_nAnalogCrossbarAudioInputProperty, pDevice->m_nAnalogCrossbarAudioOutputProperty, TRUE);

		SC0710_SetAnalogCrossbarRouteProperty(pDevice, 1, pDevice->m_nAnalogCrossbarVideoInputPropertys[1], pDevice->m_nAnalogCrossbarVideoOutputProperty, pDevice->m_nAnalogCrossbarAudioInputProperty, pDevice->m_nAnalogCrossbarAudioOutputProperty, TRUE);
	}
	else {

		SC0710_SetAnalogCrossbarRouteProperty(pDevice, 0, pDevice->m_nAnalogCrossbarVideoInputProperty, pDevice->m_nAnalogCrossbarVideoOutputProperty, pDevice->m_nAnalogCrossbarAudioInputProperty, pDevice->m_nAnalogCrossbarAudioOutputProperty, TRUE);
	}

}




