#ifndef __UPAPROM_H__
#define __UPAPROM_H__
#include "LINUXV4L2.h"

void MySleep( int ms );
int i2c_write_bytes( CDevice * pDevice, unsigned char devaddr, unsigned char *txbuf, unsigned int txcount);
int i2c_read_bytes( CDevice * pDevice, unsigned char devaddr, unsigned char *rxbuf, unsigned int rxcount);
int mcu_i2c_access(CDevice * pDevice, unsigned char devaddr, unsigned int txcount, unsigned char *txbuf, unsigned int rxcount, unsigned char *rxbuf);
int WaitI2CReady_LDROM(CDevice * pDevice, int count);
int i2c_access_LDROM(CDevice * pDevice, unsigned char devaddr, unsigned int txcount, unsigned char *txbuf, unsigned int rxcount, unsigned char *rxbuf);
void GetMCUROMVersion(CDevice * pDevice, unsigned char *aprom, unsigned char *mcucode);
void Jump2LDROM(CDevice * pDevice, unsigned char *ldrom);
int UpdateEDIDContent(CDevice * pDevice, unsigned char *buf);
int i2c_access_LDROM(CDevice * pDevice, unsigned char devaddr, unsigned int txcount, unsigned char *txbuf, unsigned int rxcount, unsigned char *rxbuf);
int UpdateOnePage(CDevice * pDevice, unsigned int pageno, unsigned char *fbuf, unsigned int pagesize);
int CheckLD_ROM_Idle(CDevice * pDevice);
int TestI2C(CDevice * pDevice);
int UpdateAPROM(CDevice * pDevice,unsigned char *bin_name, unsigned char * read_buf, unsigned int flength);
void GetMCUROMVersion(CDevice * pDevice,unsigned char *aprom, unsigned char *mcucode);
int UpdateEDID(CDevice * pDevice, unsigned char *edid_buf);
int ReadEDID(CDevice * pDevice, unsigned char *edid_buf, BYTE id );
int Update2ndMCU(CDevice * pDevice, unsigned char addr, unsigned char *bin_buf, int bin_size);
int UpdateMACaddress(CDevice * pDevice, unsigned char *mac_buf);
int ReadMACaddress(CDevice * pDevice, BYTE* pMACAddress );

#endif
