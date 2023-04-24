#include "upaprom_example.h"

void MySleep( int ms )
{
	DELAY_100NS( (ms * 10000) );
}

BOOLEAN SC0710_AccessSlaveDeviceRegisterS( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE * pTxBuf, ULONG nTxLen, BYTE * pRxBuf, ULONG nRxLen );

BOOLEAN MZ0380_ComboAnalogVideoDecoderRegisterX( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE * pValue, ULONG nBytes, BYTE is_read );



int i2c_write_bytes( CDevice * pDevice, unsigned char devaddr, unsigned char * txbuf, unsigned int txcount )
{
#if defined(AME_SC0710) || defined(AME_SC0720)

	SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	if( p_sys_cfg->n_mcu_version == 0 ) {

		LINUXV4L2_PRINT( KERN_INFO,"i2c_write_bytes, mcu version = 0\n");

		return 0;
	}
	return SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, devaddr >> 1, txbuf, txcount, NULL, 0 );

#else

	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	if( p_sys_cfg->n_mcu_version == 0)
	{
		LINUXV4L2_PRINT( KERN_INFO, "i2c_write_bytes, mcu version = 0 \n");
		return 0;
	}
	return MZ0380_ComboAnalogVideoDecoderRegisterX( pDevice, 0, devaddr, txbuf, txcount, 0 );

#endif


}



int i2c_read_bytes( CDevice * pDevice, unsigned char devaddr, unsigned char * rxbuf, unsigned int rxcount )
{
#if defined(AME_SC0710) || defined(AME_SC0720)

	SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	if( p_sys_cfg->n_mcu_version == 0 ) {

		LINUXV4L2_PRINT( KERN_INFO,"i2c_read_bytes, mcu version = 0\n");

		return 0;
	}
	return SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, devaddr >> 1, NULL, 0, rxbuf, rxcount );

#else

	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	if( p_sys_cfg->n_mcu_version == 0)
	{
		LINUXV4L2_PRINT( KERN_INFO, "i2c_read_bytes, mcu version = 0 \n");
		return 0;
	}
	return MZ0380_ComboAnalogVideoDecoderRegisterX( pDevice, 0,  devaddr, rxbuf, rxcount, 1 );

#endif


}

int mcu_i2c_access(CDevice * pDevice, unsigned char devaddr, unsigned int txcount, unsigned char *txbuf, unsigned int rxcount, unsigned char *rxbuf)
{
	unsigned char i2c_txcount = 0;
	unsigned char i2c_rxcount = 0;
	unsigned char i2c_txbuf[32] = {0};
	unsigned char i2c_rxbuf[32] = {0};

#if defined(AME_SC0710) || defined(AME_SC0720)











	devaddr <<= 1;
	if(txcount != 0)
	{
		if(rxcount != 0)
		{
			i2c_txcount = txcount+3;
			i2c_txbuf[0] = 0x66;
			i2c_txbuf[1] = devaddr|1;
			if(devaddr == 0xaa)	{

				i2c_txbuf[2] = 3;
			}
			else {
				i2c_txbuf[2] = (unsigned char)rxcount;
			}
			RtlCopyMemory( i2c_txbuf+3, txbuf, txcount );

		}
		else
		{
			i2c_txcount = txcount+2;
			i2c_txbuf[0] = 0x66;
			i2c_txbuf[1] = devaddr;
			RtlCopyMemory( i2c_txbuf+2, txbuf, txcount );

		}

		if( !i2c_write_bytes(pDevice, i2c_txbuf[0], i2c_txbuf+1, i2c_txcount-1 ) ) {
			return 0;
		}
	}
	if(rxcount != 0)
	{
		if(txcount != 0)
			MySleep(150);
		i2c_rxcount = (unsigned char)rxcount;
		i2c_txbuf[0] = 0x66;
		RtlFillMemory( i2c_rxbuf, rxcount, 0 );

		if( !i2c_read_bytes(pDevice, i2c_txbuf[0], i2c_rxbuf, i2c_rxcount ) ) {
			return 0;
		}
		RtlCopyMemory( rxbuf, i2c_rxbuf, rxcount );

	}

#else
	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	if( p_sys_cfg->n_mcu_version == 0)
	{
		LINUXV4L2_PRINT( KERN_INFO, "mcu_i2c_access, mcu version = 0 \n");
		return 0;
	}
	devaddr <<= 1;
	if(txcount != 0)
	{
		if(rxcount != 0)
		{
			i2c_txcount = txcount+3;
			i2c_txbuf[0] = 0x66;
			i2c_txbuf[1] = devaddr|1;
			i2c_txbuf[2] = (unsigned char)rxcount;
			RtlCopyMemory( i2c_txbuf+3, txbuf, txcount );

		}
		else
		{
			i2c_txcount = txcount+2;
			i2c_txbuf[0] = 0x66;
			i2c_txbuf[1] = devaddr;
			RtlCopyMemory( i2c_txbuf+2, txbuf, txcount );

		}

		if( !i2c_write_bytes(pDevice, i2c_txbuf[0], i2c_txbuf+1, i2c_txcount-1 ) ) {

			LINUXV4L2_PRINT( KERN_INFO, "mcu_i2c_access() i2c_write_bytes error!!! \n");

			return 0;
		}
	}
	if(rxcount != 0)
	{
		if(txcount != 0)
			MySleep(150);
		i2c_rxcount = (unsigned char)rxcount;
		i2c_txbuf[0] = 0x66;
		RtlFillMemory( i2c_rxbuf, rxcount, 0 );

		if( !i2c_read_bytes(pDevice, i2c_txbuf[0], i2c_rxbuf, i2c_rxcount ) ) {

			LINUXV4L2_PRINT( KERN_INFO, "mcu_i2c_access() i2c_read_bytes error!!! \n");

			return 0;
		}
		RtlCopyMemory( rxbuf, i2c_rxbuf, rxcount );

	}
#endif

	return 1;
}

void SetMCUPolling(CDevice * pDevice, unsigned char stop_polling)
{
	unsigned char txbuf[16];
	unsigned char rxbuf[16];
	ULONG i = 0;

	while(1)
	{
		txbuf[0] = 0x12;
		txbuf[1] = 0x34;
		txbuf[2] = 0x5c;
		txbuf[3] = stop_polling;
		mcu_i2c_access(pDevice,0x55, 4, txbuf, 3, rxbuf);

		LINUXV4L2_PRINT( KERN_INFO, "SetMCUPolling(%d)::AP ROM flag (0x%x, 0x%x, 0x%x) \n", stop_polling , rxbuf[0], rxbuf[1], rxbuf[2]);

		if(rxbuf[0] == 0xaa && rxbuf[1] == 0x55 && rxbuf[2] == stop_polling)
		{
			break;
		}

		i++;

		if(i >= 30) {

			return;
		}


		MySleep(300);
	}
}

int WaitI2CReady_LDROM(CDevice * pDevice, int count)
{
#if 1
	MySleep(15);
	return 0;
#else
	int i;
	unsigned char rxbuf[1];


	i = 0;
	while(1)
	{
	    i2c_read_bytes(pDevice, 0x60, rxbuf, 1);
		if(rxbuf[0] == 0)
		{

		    break;
		}
		i++;
		if(i > 20)
		{
			LINUXV4L2_PRINT( KERN_INFO, "LDROM I2C Time out \n");
			break;
		}
		MySleep(1);
	}
	return 0;
#endif
}


int i2c_access_LDROM(CDevice * pDevice, unsigned char devaddr, unsigned int txcount, unsigned char *txbuf, unsigned int rxcount, unsigned char *rxbuf)
{
	if(txcount != 0)
	{
		if( !i2c_write_bytes( pDevice, 0x6c, txbuf, txcount ) )
		{
				return FALSE;
		}
	}
	if(rxcount != 0)
	{
		WaitI2CReady_LDROM( pDevice, txcount + 2 );
		if( !i2c_read_bytes( pDevice, 0x6c, rxbuf, rxcount ) )
		{
			return FALSE;
		}
		LINUXV4L2_DEBUG( KERN_INFO, "i2c_access_LDROM read 0x%x, 0x%x, 0x%x \n", rxbuf[0], rxbuf[1], rxbuf[2]);
	}
	return TRUE;
}



unsigned char UpdateAP_Tx[64];
unsigned char UpdateAP_Rx[64];
#define CMD_UPDATE_APROM	0xA0
#define CMD_UPDATE_CONFIG	0xA1
#define CMD_ERASE_ALL		0xA2
#define CMD_GET_FWVER		0xA3
#define CMD_RUN_APROM		0xA4
#define CMD_RUN_LDROM		0xA5
#define CMD_RESET			0xA6
#define CMD_PAYLOAD_END		0xA7
#define CMD_GET_ROMINFO		0xA8
#define CMD_GET_DEVICEID	0xA9
#define CMD_GET_FLASHMODE 	0xAA
#define CMD_BLANK_CHECK		0xAB
#define CMD_IDLE_CHECK		0xAC
#define CMD_I2C_TEST		0xAD
#define CMD_DUMMY			0xFF

struct D_CMD_UPDATE_CONFIG
{
	unsigned char commandid;
	unsigned char char_0xaa;
	unsigned char char_0x55;
	unsigned char char_0x44;
	unsigned int  config0;
	unsigned int  config1;
};

struct D_CMD_GET_ROMINFO
{
	unsigned char firmversion;
	unsigned char pdid;
	unsigned char reserve1;
	unsigned char reserve2;
	unsigned int  config0;
	unsigned int  config1;
	unsigned int  aprom_checksum;
	unsigned int  aprom_len;
};

struct D_CMD_UPDATE_APROM
{
	unsigned char commandid;
	unsigned char currentpage;
	unsigned char payloadlen;
	unsigned char payload[61];
};

struct D_CMD_PAYLOAD_END
{
	unsigned char commandid;
	unsigned char payloadlen;
	unsigned char char_0xaa;
	unsigned char char_0x55;
	unsigned int  pagechecksum;
};

#define AP_SIZE		0x8000
#define MCU_PAGE_SIZE	0x200
#define UPDATE_SUCCESS		0
#define FAIL_JMP_LDROM		1
#define FAIL_READ_BIN_FILE	2
#define FAIL_ERASE_APROM	3
#define FAIL_BLANKCHECK		4
#define FAIL_UPDATE_PAGE	5
#define FAIL_CHECKSUM		6
#define FAIL_MCU_NOT_EXIST  7
#define FAIL_JMP_APROM		8
#define FAIL_BUSY_LDROM		9
#define FAIL_PAGE_TRANSFER	10

int UpdateOnePage(CDevice * pDevice, unsigned int pageno, unsigned char *fbuf, unsigned int pagesize)
{


	unsigned int size, i;
	struct D_CMD_UPDATE_APROM s_update;
	struct D_CMD_PAYLOAD_END s_pageend;
	unsigned char *ptemp;

	s_pageend.commandid = CMD_PAYLOAD_END;
	s_pageend.payloadlen = (BYTE)pagesize;
	s_pageend.char_0xaa = 0xaa;
	s_pageend.char_0x55 = 0x55;
	s_pageend.pagechecksum = 0;

	ptemp = fbuf;
	s_update.commandid = CMD_UPDATE_APROM;
	s_update.currentpage = (BYTE)pageno;
	while(1)
	{
		if(pagesize > MAX_I2C_TX_SIZE-3)
			size = MAX_I2C_TX_SIZE-3;
		else
			size = pagesize;
		s_update.payloadlen = (BYTE)size;
		for(i = 0;i < size;i++)
		{
			s_update.payload[i] = *ptemp;
			s_pageend.pagechecksum += *ptemp;
			ptemp++;
		}
		pagesize -= size;

		i2c_access_LDROM(pDevice, 0x36, MAX_I2C_TX_SIZE, (unsigned char *)&s_update, 0, UpdateAP_Rx);
		if(pagesize == 0)
			break;
		s_update.currentpage = 0xff;
		MySleep(2);
	}
	MySleep(10);

#if 1
	i2c_access_LDROM(pDevice, 0x36, sizeof(struct D_CMD_PAYLOAD_END), (unsigned char *)&s_pageend, 2, UpdateAP_Rx);
	MySleep(150);
	i = 0;
	while(1)
	{
		if(UpdateAP_Rx[0] == 0xaa && UpdateAP_Rx[1] == 0x55)
			break;
		else if(UpdateAP_Rx[0] == 0xcc && UpdateAP_Rx[1] == 0xdd)
			break;
		else if(UpdateAP_Rx[0] == 0xee && UpdateAP_Rx[1] == 0xff)
			break;
		else
			MySleep(50);
		i2c_access_LDROM(pDevice, 0x36, 0, UpdateAP_Tx, 2, UpdateAP_Rx);
		i++;
		if(i > 10)
			break;

	}
	if(UpdateAP_Rx[0] == 0xaa && UpdateAP_Rx[1] == 0x55)
	{
		LINUXV4L2_PRINT( KERN_INFO,"page ok \n");
		return 0;
	}
	if(UpdateAP_Rx[0] == 0xcc && UpdateAP_Rx[1] == 0xdd)
	{
		LINUXV4L2_PRINT( KERN_INFO,"chechsum fail \n");
		return 2;
	}
	if(UpdateAP_Rx[0] == 0xee && UpdateAP_Rx[1] == 0xff)
	{
		LINUXV4L2_PRINT( KERN_INFO,"page write error \n");
		return 1;
	}
	return 3;
#else
	i2c_access_LDROM(pDevice, 0x36, sizeof(struct D_CMD_PAYLOAD_END), (unsigned char *)&s_pageend, 0, UpdateAP_Rx);
	i = 0;
	MySleep(100);
		return 0;
#endif
}

int CheckLD_ROM_Idle(CDevice * pDevice)
{
	int i;

	i = 0;
	while(1)
	{
		UpdateAP_Tx[0] = CMD_IDLE_CHECK;
		UpdateAP_Rx[0] = 0x11;
#if 0
		i2c_access_LDROM(pDevice,0x36, 1, UpdateAP_Tx, 3, UpdateAP_Rx);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::CheckLD_ROM_Idle (0x%x, 0x%x, 0x%x)\n", UpdateAP_Rx[0], UpdateAP_Rx[1], UpdateAP_Rx[2]);
		if((UpdateAP_Rx[0] == 0xab && UpdateAP_Rx[1] == 0xcd && UpdateAP_Rx[2] == 0xef) ||
		   (UpdateAP_Rx[0] == 0xab && UpdateAP_Rx[1] == 0xe6 && UpdateAP_Rx[2] == 0xff))
#else
		i2c_access_LDROM(pDevice,0x36, 1, UpdateAP_Tx, 1, UpdateAP_Rx);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::CheckLD_ROM_Idle (0x%x)\n", UpdateAP_Rx[0]);

		if( (UpdateAP_Rx[0] == 0xab) || (UpdateAP_Rx[0] == 0xac) )
#endif
		{
			break;
		}
		MySleep(50);
		i++;
		if(i > 5)
			return  1;
	}
	return 0;
}











int TestI2C(CDevice * pDevice)
{
	int i, j, sendlength;

	i = 0;
	while(1)
	{
		UpdateAP_Tx[0] = CMD_I2C_TEST;
		for(j = 0;j < i;j++)
			memset(UpdateAP_Tx+1, (BYTE)(0xaa+i+j), 16);
		memset(UpdateAP_Rx, 0, 16);
		sendlength = (i&0xf)+2;
		if(sendlength > 16)
			sendlength = 16;
		i2c_access_LDROM(pDevice,0x36, sendlength, UpdateAP_Tx, sendlength, UpdateAP_Rx);
		for(j = 0;j < sendlength;j++)
		{
			if(UpdateAP_Rx[j] != UpdateAP_Tx[j])
				break;
		}
		if(j == sendlength)
				{

		}
		else
		{
			LINUXV4L2_PRINT( KERN_INFO, "CDevice::TestI2C fail %d", sendlength);
		}
		i++;
		if(i > 20000)
			return  1;
	}
	return 0;
}

int UpdateAPROM(CDevice * pDevice,unsigned char *bin_name, unsigned char * read_buf, unsigned int flength)
{

	unsigned char *ptemp;
	unsigned int currentpage;
	unsigned int total_checksum;
	unsigned int i;
	unsigned int tempsize;




	if(CheckLD_ROM_Idle(pDevice) == 1)
	{

		LINUXV4L2_PRINT( KERN_INFO, "NO MCU ....\n");
		return 1;
	}
	else
	{
		LINUXV4L2_PRINT( KERN_INFO, "Already in the LD ROM \n");
	}

	MySleep(500);



	LINUXV4L2_PRINT( KERN_INFO, "In Erase All... \n");
	UpdateAP_Tx[0] = CMD_ERASE_ALL;
	i2c_access_LDROM(pDevice,0x36, 1, UpdateAP_Tx, 0, 0);

#if defined(AME_SC0710) || defined(AME_SC0720)

	MySleep(8000);

#else



	MySleep(10000);

#endif

	ptemp = read_buf;
	total_checksum = 0;
	for(i = 0;i < flength;i++)
		total_checksum += *ptemp++;
    LINUXV4L2_PRINT( KERN_INFO, "Out Erase All... \n");



	currentpage = 0;
	ptemp = read_buf;
	tempsize = flength;
	LINUXV4L2_PRINT( KERN_INFO, "Update File Length %d\n", flength);
	while(1)
	{
		int res;

		if(tempsize >= MCU_PAGE_SIZE)
			res = UpdateOnePage(pDevice,currentpage, ptemp, MCU_PAGE_SIZE);
		else
			res = UpdateOnePage(pDevice,currentpage, ptemp, tempsize);
		if(tempsize == MCU_PAGE_SIZE && flength != 32*1024)
		{


			ptemp = read_buf;
			RtlFillMemory( ptemp, 0x10, 0 );

			res = UpdateOnePage(pDevice,currentpage+1, ptemp, 0x10);
			break;
		}
		if(tempsize < MCU_PAGE_SIZE)
			break;
		tempsize -= MCU_PAGE_SIZE;
		ptemp += MCU_PAGE_SIZE;
		currentpage++;
		LINUXV4L2_PRINT( KERN_INFO, "Update page, current page %d...\n", currentpage);
	}

	MySleep(100);



	LINUXV4L2_PRINT( KERN_INFO, "Run APROM \n");
	UpdateAP_Tx[0] = CMD_RUN_APROM;
	UpdateAP_Rx[0] = 0;
	i2c_access_LDROM(pDevice,0x36, 1, UpdateAP_Tx, 0, 0);
	MySleep(1500);
	MySleep(1500);

	{
		unsigned char aprom[ 3 ] = { 0, 0, 0 };
		unsigned char mcucode[3] = { 0, 0, 0 };
		unsigned int aprom_date;

#if defined(AME_SC0710) || defined(AME_SC0720)

		SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
#else

		MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
#endif

		GetMCUROMVersion(pDevice, aprom, mcucode);
		aprom_date = 0;
		if(mcucode[0] == 0x51 && mcucode[1] == 0x10 && mcucode[2] == 0x27)
			aprom_date = (unsigned int)aprom[0]*10000+aprom[1]*100+aprom[2];
		LINUXV4L2_PRINT( KERN_INFO, "Run APROM date: %d\n", aprom_date);
		p_sys_cfg->n_mcu_version = aprom_date;
		if(aprom_date > 170820)
		{
			int i;

			i = 0;
			while(1)
			{
				unsigned char txbuf[3], rxbuf[3];
				txbuf[0] = 0x12;
				txbuf[1] = 0x34;
				txbuf[2] = 0x5a;
				mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);
				LINUXV4L2_PRINT( KERN_INFO, "CDevice::EnableSecurity (0x%x, 0x%x, 0x%x) \n", rxbuf[0], rxbuf[1], rxbuf[2]);
				if(rxbuf[0] == 0xaa && rxbuf[1] == 0x55 && rxbuf[2] == 0x27)
				break;
				MySleep(200);
				i++;
				if(i > 50)
					break;
			}
		}
		if(aprom_date == 0)
			return 1;
		else
			return UPDATE_SUCCESS;
	}
	return UPDATE_SUCCESS;
}


void GetMCUROMVersion(CDevice * pDevice,unsigned char *aprom, unsigned char *mcucode)
{
	unsigned char txbuf[16];
	unsigned char rxbuf[16];
	int i;





	i = 0;
	while(1)
	{
		txbuf[0] = 0x12;
		txbuf[1] = 0x34;
		txbuf[2] = 0x57;
		mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::AP ROM flag (0x%x, 0x%x, 0x%x) \n", rxbuf[0], rxbuf[1], rxbuf[2]);
		if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x27)
		{
			break;
		}
		if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x28)
		{
			break;
		}
		i++;
		if(i >= 30)
			return;
		MySleep(300);
	}
	mcucode[0] = rxbuf[0];
	mcucode[1] = rxbuf[1];
	mcucode[2] = rxbuf[2];

	while(1)
	{

		txbuf[0] = 0x12;
		txbuf[1] = 0x34;
		txbuf[2] = 0x58;
		mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::APROM version(0x%x, 0x%x, 0x%x) \n", rxbuf[0], rxbuf[1], rxbuf[2]);

		if( rxbuf[ 0 ] == 81 && rxbuf[ 1 ] == 16 && rxbuf[ 2 ] == 39 ) {

			;
		}
		else {

		if(rxbuf[0] >= 17 &&  rxbuf[0] <= 79)
		{
			aprom[0] = rxbuf[0];
			aprom[1] = rxbuf[1];
			aprom[2] = rxbuf[2];
			break;
		}
		}
		MySleep(200);
		i++;
		if(i >= 50)
			return;
	}
	return;
}


void Jump2LDROM(CDevice * pDevice,unsigned char *ldrom)
{
	unsigned char txbuf[32];
	unsigned char rxbuf[32];
	int i;



	i = 0;
	while(1)
		{
		txbuf[0] = 0x03;
		txbuf[1] = 0x0D;
		txbuf[2] = 0x12;
		txbuf[3] = 0x04;
		txbuf[4] = 0x86;
		txbuf[5] = 0x54;
		txbuf[6] = 0x06;
		txbuf[7] = 0xAA;
		mcu_i2c_access(pDevice,0x55, 8, txbuf, 0, rxbuf);
		LINUXV4L2_PRINT( KERN_INFO,"CDevice::Jump2LDROM, wait \n");
		MySleep(1000);






		if(CheckLD_ROM_Idle(pDevice) == 0)
		    break;
		i++;
		if(i > 4)
			break;
	}
	MySleep(1000);

#if 0
	{


		txbuf[0] = CMD_GET_ROMINFO;
		rxbuf[0] = 0;
		i2c_access_LDROM(pDevice,0x36, 1, txbuf, sizeof(struct D_CMD_GET_ROMINFO), (unsigned char *)&rominfo);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::LDROM firmversion 0x%x \n", rominfo.firmversion);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::LDROM pdid 0x%x \n", rominfo.pdid);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::LDROM reserve1 0x%x \n", rominfo.reserve1);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::LDROM reserve2 0x%x \n", rominfo.reserve2);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::LDROM config0 0x%x \n", rominfo.config0);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::LDROM config1 0x%x \n", rominfo.config1);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::LDROM aprom_checksum 0x%x \n", rominfo.aprom_checksum);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::LDROM aprom_len 0x%x \n", rominfo.aprom_len);
	}
	if(rominfo.reserve1 == 0xaa && rominfo.reserve2 == 0x55)
		*ldrom = rominfo.firmversion;
	else
		*ldrom = 0;
#endif
}


int UpdateEDID(CDevice * pDevice, unsigned char *edid_buf)
{
	unsigned char txbuf[8];
	unsigned char rxbuf[8];
	int i = 0;
	int checksum = 0;

	#if defined(AME_SC0710) || defined(AME_SC0720)

	SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#else

	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#endif

	if( p_sys_cfg->n_mcu_version == 0)
	{
		LINUXV4L2_PRINT( KERN_INFO, "UpdateEDID, mcu version = 0 \n");
		return 0;
	}
	i = 0;
	txbuf[0] = 0x12;
	txbuf[1] = 0x34;
	txbuf[2] = 0x59;
	mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);
	LINUXV4L2_PRINT( KERN_INFO, "CDevice::Enter UpdateEDID (0x%x, 0x%x, 0x%x) \n", rxbuf[0], rxbuf[1], rxbuf[2]);
	for(i = 0;i < 50;i++)
	{
		if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x30)
			break;
		MySleep(100);
		mcu_i2c_access(pDevice,0x55, 0, txbuf, 3, rxbuf);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::Enter UpdateEDID %d (0x%x, 0x%x, 0x%x) \n", i, rxbuf[0], rxbuf[1], rxbuf[2]);
	}
#if defined(AME_SC0710) || defined(AME_SC0720)
	MySleep(10);
#endif
	if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x30)
	{
		checksum = 0;
		for(i = 0;i < 256;i += 16)
		{
			int j = 0;
			for(j = 0;j < 16;j++)
				checksum += edid_buf[i+j];
			{
				int ret = i2c_write_bytes(pDevice, 0x66, edid_buf+i, 16);
				LINUXV4L2_PRINT( KERN_INFO, "CDevice::send EDID data (0x%x) ret(0x%x)\n", i, ret);
			}
#if defined(AME_SC0710) || defined(AME_SC0720)
			MySleep(10);
#endif
		}
	}
	LINUXV4L2_PRINT( KERN_INFO, "CDevice::update EDID checksum (0x%x) \n", checksum);
	i = 0;
	while(1)
	{
		MySleep(300);
		txbuf[0] = 0xcc;
		mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::Exit UpdateEDID wait %d (0x%x, 0x%x, 0x%x) \n", i, rxbuf[0], rxbuf[1], rxbuf[2]);
		if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x31)
			break;
		i++;
		if(i == 3)
			break;
	}

	while(1)
	{

		txbuf[0] = 0x12;
		txbuf[1] = 0x34;
		txbuf[2] = 0x58;
		mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::APROM version(0x%x, 0x%x, 0x%x) \n", rxbuf[0], rxbuf[1], rxbuf[2]);
		if(rxbuf[0] >= 17 &&  rxbuf[0] <= 37 && rxbuf[1] <= 12 && rxbuf[2] <= 31)
			break;
		MySleep(200);
		i++;
		if(i >= 50)
			return 1;
	}

	return 1;
}


int ReadEDID(CDevice * pDevice, unsigned char *edid_buf, BYTE id )
{
	unsigned char txbuf[8];
	unsigned char rxbuf[8];
	int i, checksum;

#if defined(AME_SC0710) || defined(AME_SC0720)

	SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

#else

	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

#endif

	if( p_sys_cfg->n_mcu_version == 0)
	{
		LINUXV4L2_PRINT( KERN_INFO, "ReadEDID, mcu version = 0 \n");
		return 0;
	}


	txbuf[0] = 0x12;
	txbuf[1] = 0x34;
	txbuf[2] = id;
	mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);
	LINUXV4L2_PRINT( KERN_INFO, "CDevice::Enter ReadEDID (0x%x, 0x%x, 0x%x) \n", rxbuf[0], rxbuf[1], rxbuf[2]);
	for(i = 0;i < 50;i++)
	{
		if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x35)
			break;
		MySleep(100);
		mcu_i2c_access(pDevice,0x55, 0, txbuf, 3, rxbuf);
		LINUXV4L2_PRINT( KERN_INFO, "CDevice::Enter ReadEDID %d (0x%x, 0x%x, 0x%x) \n", i, rxbuf[0], rxbuf[1], rxbuf[2]);
	}
	LINUXV4L2_PRINT( KERN_INFO, "CDevice::Enter ReadEDID\n");
	MySleep(100);
	for(i = 0;i < 256;i += 16)
	{
		txbuf[0] = 0x55;
		txbuf[1] = (BYTE)i;
		i2c_write_bytes(pDevice, 0x66, txbuf, 2);

#if defined(AME_SC0710) || defined(AME_SC0720)
		MySleep(2);
#endif
		i2c_read_bytes(pDevice, 0x66, edid_buf+i, 16);
		LINUXV4L2_PRINT( KERN_INFO, "Read EDID 0x%x, (0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x) \n", i,
			edid_buf[i], edid_buf[i+1], edid_buf[i+2], edid_buf[i+3], edid_buf[i+4], edid_buf[i+5], edid_buf[i+6], edid_buf[i+7],
			edid_buf[i+8], edid_buf[i+9], edid_buf[i+10], edid_buf[i+11], edid_buf[i+12], edid_buf[i+13], edid_buf[i+14], edid_buf[i+15]);

#if defined(AME_SC0710) || defined(AME_SC0720)
		MySleep(2);
#endif
	}
	checksum = 0;
	for(i = 0;i < 256;i++)
	{
		checksum += edid_buf[i];
	}
	LINUXV4L2_PRINT( KERN_INFO, "read EDID data checksum 0x%x \n", checksum);
	i = 0;
	while(1)
	{
		txbuf[0] = 0xcc;
		mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);
		LINUXV4L2_PRINT( KERN_INFO, "Exit ReadEDID wait %d (0x%x, 0x%x, 0x%x) \n", i, rxbuf[0], rxbuf[1], rxbuf[2]);
		if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x31)
			return 0;
		i++;
		MySleep(100);
		if(i == 3)
			break;
	}

	txbuf[0] = 0x00;
	txbuf[1] = 0x00;
	i2c_write_bytes(pDevice, 0x66, txbuf, 2);
	return 1;
}


int UpdateMACaddress(CDevice * pDevice, unsigned char *mac_buf)
{
	unsigned char txbuf[8] = { 0 };
	unsigned char rxbuf[8] = { 0 };
	int i = 0;
	int checksum = 0;

	#if defined(AME_SC0710) || defined(AME_SC0720)
	{
		SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

		if( p_sys_cfg->n_mcu_version == 0)
		{
			LINUXV4L2_PRINT( KERN_INFO, "UpdateMACaddress, mcu version = 0 \n" );
			return 0;
		}
		i = 0;
		txbuf[0] = 0x12;
		txbuf[1] = 0x34;
		txbuf[2] = 0x64;
		mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);
		LINUXV4L2_PRINT( KERN_INFO, "Enter UpdateMACaddress (0x%x, 0x%x, 0x%x) \n", rxbuf[0], rxbuf[1], rxbuf[2] );
		for(i = 0;i < 50;i++)
		{
			if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x30)
				break;
			MySleep(100);
			mcu_i2c_access(pDevice,0x55, 0, txbuf, 3, rxbuf);
			LINUXV4L2_PRINT( KERN_INFO, "Enter UpdateMACaddress %d (0x%x, 0x%x, 0x%x) \n", i, rxbuf[0], rxbuf[1], rxbuf[2] );
		}

		MySleep(10);

		if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x30)
		{
			int j = 0;
			checksum = 0;
			for(i = 0;i < 256;i += 16)
			{
				for( j = 0;j < 16;j++)
					checksum += mac_buf[i+j];
				i2c_write_bytes(pDevice, 0x66, mac_buf+i, 16);
				LINUXV4L2_PRINT( KERN_INFO, "send MAC data (0x%x) \n", i );

				MySleep(10);
			}
		}
		LINUXV4L2_PRINT( KERN_INFO, "update MAC checksum (0x%x) \n", checksum );
		i = 0;
		while(1)
		{
			MySleep(300);
			txbuf[0] = 0xcc;
			mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);
			LINUXV4L2_PRINT( KERN_INFO, "Exit UpdateMACaddress wait %d (0x%x, 0x%x, 0x%x) \n", i, rxbuf[0], rxbuf[1], rxbuf[2] );
			if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x31)
				break;
			i++;
			if(i == 3)
				break;
		}

		while(1)
		{

			txbuf[0] = 0x12;
			txbuf[1] = 0x34;
			txbuf[2] = 0x58;
			mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);
			LINUXV4L2_PRINT( KERN_INFO, "APROM version(0x%x, 0x%x, 0x%x) \n", rxbuf[0], rxbuf[1], rxbuf[2] );
			if(rxbuf[0] >= 17 &&  rxbuf[0] <= 37 && rxbuf[1] <= 12 && rxbuf[2] <= 31)
				break;
			MySleep(200);
			i++;
			if(i >= 50)
				return 1;
		}
	}
	#endif

	return 1;
}

int ReadMACaddress(CDevice * pDevice, BYTE* pMACAddress )
{
	unsigned char txbuf[8];

	#if defined(AME_SC0710) || defined(AME_SC0720)

	SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	if( p_sys_cfg->n_mcu_version == 0)
	{
		LINUXV4L2_PRINT( KERN_INFO, "ReadMACaddress, mcu version = 0 \n" );
		return 0;
	}
	txbuf[0] = 0x12;
	txbuf[1] = 0x34;
	txbuf[2] = 0x67;
	mcu_i2c_access(pDevice,0x55, 3, txbuf, 6, pMACAddress);
	#endif

	return 1;
}


#if defined(AME_SC0710) || defined(AME_SC0720)
int SC720_ReadMACaddress(CDevice* pDevice, BYTE ch, BYTE* pMACAddress)
{
	unsigned char txbuf[8];

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	if (p_sys_cfg->n_mcu_version == 0)
	{
		LINUXV4L2_PRINT( KERN_INFO, "ReadMACaddress, mcu version = 0 \n");
		return 0;
	}
	txbuf[0] = 0x12;
	txbuf[1] = 0x34;
	txbuf[2] = 0x6C + ch%4;
	mcu_i2c_access(pDevice, 0x55, 3, txbuf, 6, pMACAddress);




	return 1;
}
#endif

int MCU_UPDATE_BOARD_MEMORY( CDevice * pDevice, unsigned char * boardmem_buf )
{
	unsigned char txbuf[ 8 ];

	unsigned char rxbuf[ 8 ];

	unsigned char bmbuf[ 64 ] = { 0 };

	ULONG i = 0;

	#if defined(AME_SC0710) || defined(AME_SC0720)

	SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#else

	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#endif

	if( p_sys_cfg->n_mcu_version == 0 ) {

		LINUXV4L2_PRINT( KERN_INFO, "MCU_UPDATE_BOARD_MEMORY( mcu.version = 0 )\n");

		return 0;
	}
	i = 0;

	txbuf[ 0 ] = 0x12;

	txbuf[ 1 ] = 0x34;

	txbuf[ 2 ] = 0x65;

	mcu_i2c_access( pDevice,0x55, 3, txbuf, 3, rxbuf );



	for( i = 0 ; i < 50 ; i++ ) {

		if( rxbuf[ 0 ] == 0x51 && rxbuf[ 1 ] == 0x10 && rxbuf[ 2 ] == 0x30 ) {

			break;
		}
		MySleep( 100 );

		mcu_i2c_access( pDevice,0x55, 0, txbuf, 3, rxbuf );


	}

	#if defined(AME_SC0710) || defined(AME_SC0720)

	MySleep( 10 );

	#endif

	if( rxbuf[ 0 ] == 0x51 && rxbuf[ 1 ] == 0x10 && rxbuf[ 2 ] == 0x30 ) {

		ULONG encrypt_size = 64;

		memcpy( bmbuf, boardmem_buf, 16 );

		for( i = 0; i < encrypt_size ; i += 16 ) {

			i2c_write_bytes( pDevice, 0x66, &bmbuf[ i ], 16 );

			#if defined(AME_SC0710) || defined(AME_SC0720)

			MySleep( 10 );
			#endif
		}
	}

	i = 0;

	while( 1 ) {

		MySleep( 300 );

		txbuf[ 0 ] = 0xCC;

		mcu_i2c_access( pDevice,0x55, 3, txbuf, 3, rxbuf );

		if( rxbuf[ 0 ] == 0x51 && rxbuf[ 1 ] == 0x10 && rxbuf[ 2 ] == 0x31 ) {

			break;
		}
		i++;

		if( i == 3 ) {

			break;
		}
	}
	while( 1 ) {

		txbuf[ 0 ] = 0x12;

		txbuf[ 1 ] = 0x34;

		txbuf[ 2 ] = 0x58;

		mcu_i2c_access( pDevice,0x55, 3, txbuf, 3, rxbuf );

		if( rxbuf[ 0 ] >= 17 && rxbuf[ 0 ] <= 37 && rxbuf[ 1 ] < 12 && rxbuf[ 2 ] < 31 ) {

			break;
		}
		MySleep( 200 );

		i++;

		if( i >= 50 ) {

			return 1;
		}
	}
	return 1;
}

int MCU_READ_BOARD_MEMORY( CDevice * pDevice, unsigned char * boardmem_buf )
{
	unsigned char txbuf[ 8 ];

	unsigned char rxbuf[ 8 ];

	unsigned char bmbuf[ 64 ] = { 0 };

	ULONG encrypt_size = 64;

	ULONG i = 0;

	#if defined(AME_SC0710) || defined(AME_SC0720)

	SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#else

	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#endif

	if( p_sys_cfg->n_mcu_version == 0 ) {

		LINUXV4L2_PRINT( KERN_INFO, "MCU_READ_BOARD_MEMORY( mcu.version = 0 )\n");

		return 0;
	}
	txbuf[ 0 ] = 0x12;

	txbuf[ 1 ] = 0x34;

	txbuf[ 2 ] = 0x66;

	mcu_i2c_access( pDevice, 0x55, 3, txbuf, 3, rxbuf );

	for( i = 0 ; i < 50 ;i++ ) {

		if( rxbuf[ 0 ] == 0x51 && rxbuf[ 1 ] == 0x10 && rxbuf[ 2 ] == 0x35 ) {

			break;
		}
		MySleep( 100 );

		mcu_i2c_access( pDevice, 0x55, 0, txbuf, 3, rxbuf );
	}
	MySleep( 100 );



	for( i = 0 ; i < encrypt_size; i += 16 ) {

		txbuf[ 0 ] = 0x55;

		txbuf[ 1 ] = (BYTE)(i);

		i2c_write_bytes( pDevice, 0x66, txbuf, 2 );

		#if defined(AME_SC0710) || defined(AME_SC0720)

		MySleep( 2 );
		#endif

		i2c_read_bytes( pDevice, 0x66, &bmbuf[ i ], 16 );

		memcpy( boardmem_buf, bmbuf, 16 );

		#if defined(AME_SC0710) || defined(AME_SC0720)

		MySleep( 2 );
		#endif
	}
	i = 0;

	while( 1 ) {

		txbuf[ 0 ] = 0xcc;

		mcu_i2c_access( pDevice, 0x55, 3, txbuf, 3, rxbuf );

		if( rxbuf[ 0 ] == 0x51 && rxbuf[ 1 ] == 0x10 && rxbuf[ 2 ] == 0x31 ) {

			return 0;
		}
		i++;

		MySleep( 100 );

		if( i == 3 ) {

			break;
		}
	}
	txbuf[ 0 ] = 0x00;

	txbuf[ 1 ] = 0x00;

	i2c_write_bytes( pDevice, 0x66, txbuf, 2 );

	return 1;
}


int VerifyLDROM(CDevice * pDevice)
{
	unsigned char txbuf[16];

	unsigned char rxbuf[16];

	int i;




	txbuf[0] = 0x12;

	txbuf[1] = 0x34;

	txbuf[2] = 0x58;

	mcu_i2c_access( pDevice, 0x55, 3, txbuf, 3, rxbuf );

	i = 0;

	while(1)
	{



		if( (rxbuf[0] >= 15 && rxbuf[0] <= 50) && (rxbuf[1] >= 1 && rxbuf[1] <= 12) && (rxbuf[2] >= 1 && rxbuf[2] <= 31) ) {

			break;
		}

		MySleep(50);

		i++;

		if(i >= 10)
		{


			return 0;

		}

		MySleep(50);

		mcu_i2c_access(pDevice,0x55, 0, txbuf, 3, rxbuf);
	}


	txbuf[0] = 0x03;

	txbuf[1] = 0x0D;

	txbuf[2] = 0x12;

	txbuf[3] = 0x04;

	txbuf[4] = 0x86;

	txbuf[5] = 0x54;

	txbuf[6] = 0x06;

	txbuf[7] = 0xAA;

	mcu_i2c_access(pDevice, 0x55, 8, txbuf, 0, rxbuf);




	MySleep(300);


	txbuf[0] = CMD_IDLE_CHECK;

	txbuf[0] = 0x11;

	i2c_access_LDROM(pDevice,0x36, 1, txbuf, 3, rxbuf);

	i = 0;

	while(1)
	{


		if(rxbuf[0] == 0xab && rxbuf[1] == 0xcd && rxbuf[2] == 0xef) {

			break;
		}
		if(rxbuf[0] == 0xaa && rxbuf[1] == 0xbb && rxbuf[2] == 0xcc) {

			break;
		}

		MySleep(10);

		i++;

		if(i > 10)
		{

			return 0;
		}

		i2c_access_LDROM(pDevice,0x36, 0, txbuf, 3, rxbuf);
	}




	txbuf[0] = CMD_RUN_APROM;

	i2c_access_LDROM(pDevice,0x36, 1, txbuf, 0, 0);

	MySleep(1500);



	txbuf[0] = 0x12;

	txbuf[1] = 0x34;

	txbuf[2] = 0x58;

	mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);

	i = 0;

	while(1)
	{


		if((rxbuf[0] >= 15 && rxbuf[0] <= 40) && (rxbuf[1] >= 1 && rxbuf[1] <= 12) && (rxbuf[2] >= 1 && rxbuf[2] <= 31)) {

			break;
		}

		MySleep(50);

		i++;

		if(i >= 10)
		{

			return 0;

		}

		MySleep(50);

		mcu_i2c_access(pDevice,0x55, 0, txbuf, 3, rxbuf);
	}



	return 1;
}


int Update2ndMCU(CDevice * pDevice, unsigned char addr, unsigned char *bin_buf, int bin_size)
{
	unsigned char txbuf[8];
	unsigned char rxbuf[8];
	int i, send_size;

	#if defined(AME_SC0710) || defined(AME_SC0720)

	SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	#else

	MZ0380_SYS_CFG * p_sys_cfg = (MZ0380_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
	#endif

	if( p_sys_cfg->n_mcu_version == 0)
	{
		LINUXV4L2_PRINT( KERN_INFO, "Update2ndMCU, mcu version = 0 \n");
		return 0;
	}

	i = 0;
	txbuf[0] = 0x12;
	txbuf[1] = 0x34;


	txbuf[2] = addr;
	txbuf[3] = (bin_size&0xff);
	txbuf[4] = (bin_size&0xff00)>>8;
	txbuf[5] = (bin_size&0xff0000)>>16;
	mcu_i2c_access(pDevice,0x55, 6, txbuf, 3, rxbuf);

	LINUXV4L2_PRINT( KERN_INFO, "Enter Update2ndMCU (0x%x, 0x%x, 0x%x) bin_size(0d%d)\n", rxbuf[0], rxbuf[1], rxbuf[2], bin_size );

	for(i = 0;i < 120;i++)
	{
		if(rxbuf[0] == 0xcc && rxbuf[1] == 0x11 && rxbuf[2] == 0x33)
			break;
		MySleep(100);

		mcu_i2c_access(pDevice,0x55, 0, txbuf, 3, rxbuf);

		LINUXV4L2_PRINT( KERN_INFO, "Enter Update2ndMCU %d (0x%x, 0x%x, 0x%x) \n", i, rxbuf[0], rxbuf[1], rxbuf[2]);
	}


	if(rxbuf[0] == 0xcc && rxbuf[1] == 0x11 && rxbuf[2] == 0x33)
	{

		send_size = 0;

		for(i = 0;i < bin_size;i += send_size)
		{

			if((i % 512) == 0 && i != 0)
				MySleep(100);


			if(i <= (bin_size-32))
				send_size = 32;
			else
				send_size = bin_size - i;

			i2c_write_bytes(pDevice, 0x66, bin_buf+i, send_size);

			MySleep(1);

			i2c_read_bytes(pDevice, 0x66, rxbuf, 3);

			LINUXV4L2_PRINT( KERN_INFO, "send Update2ndMCU data (0x%x) send_size(0x%x), rx(0x%x, 0x%x, 0x%x) \n", i, send_size, rxbuf[0], rxbuf[1], rxbuf[2]);
		}
	}

	return 1;
}
