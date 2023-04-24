#ifndef _SC0710_H
#define _SC0710_H

#include "LINUXV4L2.h"

#define SC0710_REGBASE_ENGINE_OFFSET        0x00000100

#define XDMA_MAX_NUM_CHANNELS   (4)

#define XDMA_NUM_DIRECTIONS     (2)

#define XDMA_MAX_CHAN_IRQ       (XDMA_NUM_DIRECTIONS * XDMA_MAX_NUM_CHANNELS)

#define XDMA_MAX_USER_IRQ       (16)

#define XMDA_MAX_NUM_IRQ        (XDMA_MAX_USER_IRQ + XDMA_MAX_CHAN_IRQ)

#define SC0710_SPI_CTRL_DEFAULT			   (SC0710_SPI_CTRL_TRANS_STOP | SC0710_SPI_CTRL_MANUAL_CHIP_SELECT)

#define SC0710_SPI_CTRL_TRANS_STOP			0x00000100

#define SC0710_SPI_CTRL_MANUAL_CHIP_SELECT	0x00000080

#define SC0710_SPI_CTRL_RESET_RX_FIFO		0x00000040

#define SC0710_SPI_CTRL_RESET_TX_FIFO		0x00000020

#define SC0710_SPI_CTRL_SET_TO_MASTER		0x00000004

#define SC0710_SPI_CTRL_ENABLE_SPI			0x00000002

#define SC0710_REGBASE_ENGINE_H2C           0x00000000

#define SC0710_REGBASE_ENGINE_C2H           0x00001000

#define SC0710_REGBASE_IRQ                  0x00002000

#define SC0710_REGBASE_CONFIG               0x00003000

#define SC0710_REGBASE_SGDMA_H2C            0x00004000

#define SC0710_REGBASE_SGDMA_C2H            0x00005000

#define SC0710_REGBASE_BLOCK_OFFSET         0x00001000



#define BIT_N(n)							(1 << n)
#define XDMA_CTRL_RUN_BIT                   (BIT_N(0))
#define XDMA_CTRL_IE_DESC_STOPPED           (BIT_N(1))
#define XDMA_CTRL_IE_DESC_COMPLETED         (BIT_N(2))
#define XDMA_CTRL_IE_ALIGNMENT_MISMATCH     (BIT_N(3))
#define XDMA_CTRL_IE_MAGIC_STOPPED          (BIT_N(4))
#define XDMA_CTRL_IE_INVALID_LENGTH			(BIT_N(5))
#define XDMA_CTRL_IE_IDLE_STOPPED			(BIT_N(6))
#define XDMA_CTRL_IE_READ_ERROR				(0x1F <<  9)
#define XDMA_CTRL_IE_WRITE_ERROR			(0x1F << 14)
#define XDMA_CTRL_IE_DESCRIPTOR_ERROR		(0x1F << 19)
#define XDMA_CTRL_NON_INCR_ADDR             (BIT_N(25))
#define XDMA_CTRL_POLL_MODE                 (BIT_N(26))
#define XDMA_CTRL_RST                       (BIT_N(31))

#define XDMA_CTRL_IE_ALL (XDMA_CTRL_IE_DESC_STOPPED | \
						  XDMA_CTRL_IE_DESC_COMPLETED | \
                          XDMA_CTRL_IE_ALIGNMENT_MISMATCH | \
						  XDMA_CTRL_IE_MAGIC_STOPPED | \
                          XDMA_CTRL_IE_INVALID_LENGTH | \
						  XDMA_CTRL_IE_READ_ERROR | \
						  XDMA_CTRL_IE_WRITE_ERROR | \
                          XDMA_CTRL_IE_DESCRIPTOR_ERROR)

#define XDMA_DESC_MAGIC                     (0xAD4B0000)
#define XDMA_DESC_STOP_BIT                  (BIT_N(0))
#define XDMA_DESC_COMPLETED_BIT             (BIT_N(1))
#define XDMA_DESC_EOP_BIT                   (BIT_N(4))

#ifndef MEDIAEDGE
#define SC0710_DEF_10BITS_METHOD 1
#endif


#pragma pack( push, 1 )

typedef struct _XDMA_DESCRIPTOR
{
    UINT32 control;

    UINT32 numBytes;

    UINT32 srcAddrLo;

    UINT32 srcAddrHi;

    UINT32 dstAddrLo;

    UINT32 dstAddrHi;

    UINT32 nextLo;

    UINT32 nextHi;

} XDMA_DESCRIPTOR;

typedef struct {

    UINT32 status;

    UINT32 length;

    UINT32 reserved_1[ 6 ];

} XDMA_RESULT;

#define TOTAL_VIDEO_SOURCE 2

#define HDMI0          0
#define VGA0           3
#define YPBPR0         2
#define AV0            5
#define SVIDEO0        6
#define AV0_SQ_PAL     8
#define SVIDEO0_SQ_PAL 9

#define PCM_ONLY	   0
#define LINE_1_ONLY	   1
#define PCM_LINE_1     2
#define LINE_2_ONLY    3
#define PCM_LINE_2     4


#define TOTAL_6802		3
#define TOTAL_HDMI		4
#define TOTAL_SDI		1
#define TOTAL_CS42L51	2
#define TOTAL_VIDEO_SOURCE_LC300	(TOTAL_HDMI+TOTAL_SDI)
#define TOTAL_AUDIO_INPUT	8

#define AUDIO_AD_LINE_IN_L		0
#define AUDIO_AD_LINE_IN_R		1
#define AUDIO_AD_LINE_IN_LR		2
#define AUDIO_AD_MIC_IN_L		3
#define AUDIO_AD_MIC_IN_R		4
#define AUDIO_AD_MIC_IN_LR		5
#define AUDIO_AD_MIC_IN_LR_MIX	6


typedef struct
{
	unsigned char  scale;
	unsigned char  bright;
	unsigned char  contrast;
	unsigned char  sat;
	unsigned char  hue;
	unsigned char  DDR_Latch;
	unsigned char  SDR_Latch;
	unsigned char  pading;
}VIDEOSOURCE_LC300;

typedef struct
{
	unsigned char audiolinein;
	unsigned char pcmvolume;
	unsigned char lineinvolume;
	unsigned char noisegate;
}AUDIOSOURCE_LC300;

typedef struct
{
	unsigned short vtotal;
	unsigned short htotal;
	unsigned short vactive;
	unsigned short hactive;
	unsigned char  fps;
	unsigned char  modeflag;
	unsigned char  AudioSampleRate;
	unsigned char  ColorFormat;
	unsigned char  HDCPOnOff;
	unsigned char  HDR2SDR;
	VIDEOSOURCE_LC300	   Vid;

	unsigned char  ColorDepth;
	AUDIOSOURCE_LC300		 Aud;
	unsigned char  padding[3];
}RESOLUCTION_LC300;

typedef union _HISILICON_INTERFACE
{
	struct
	{
		unsigned char selport;
		unsigned char isSupportRGB444;
		unsigned short BitRate;
		RESOLUCTION_LC300 resolution[TOTAL_VIDEO_SOURCE];
		unsigned char IT6663EDID_Mode;
	}function_name;



}MCU_INTERFACE_MULTICH_LC300;



typedef struct {

	unsigned short vtotal;

	unsigned short vstart;

	unsigned short vactive;

	unsigned short htotal;

	unsigned short hstart;

	unsigned short hactive;

	unsigned short searchtableindex;

	unsigned char  VSyncWidth;

	unsigned char  SOGOn;

	unsigned char  HV_Polarity;

	unsigned char  HSYNC_VSYNC_Mode;

	unsigned char  VerticalFreq;

	unsigned char  padding[11];

} ANALOG_SIGNAL;

typedef struct {

	unsigned char  scale;

	unsigned char  bright;

	unsigned char  contrast;

	unsigned char  sat;

	unsigned char  hue;

	unsigned char  sharpness;

	unsigned char  G_Adjust;

	unsigned char  B_Adjust;

} VIDEOSOURCE_MULTICH_AIO;

typedef struct {

	unsigned char audiolinein;

	unsigned char pcmvolume;

	unsigned char lineinvolume;

	unsigned char padding;

} AUDIOSOURCE_MULTICH_AIO;

typedef struct
{
	unsigned short vtotal;

	unsigned short htotal;

	unsigned short vactive;

	unsigned short hactive;

	unsigned char  fps;

	unsigned char  modeflag;

	unsigned char  AudioSampleRate;

	unsigned char  ColorFormat;

	unsigned char  HDCPOnOff;

	unsigned char  HDR2SDR;

	VIDEOSOURCE_MULTICH_AIO		Vid;

	ANALOG_SIGNAL               analog_signal;

	AUDIOSOURCE_MULTICH_AIO		Aud;

	unsigned char               padding[4];

} RESOLUCTION_MULTICH_AIO;

typedef union _MCU_INTERFACE
{
	struct {

		unsigned char  SelPort;

		unsigned char  padding;

		unsigned short BitRate;

		RESOLUCTION_MULTICH_AIO resolution[TOTAL_VIDEO_SOURCE];

		unsigned char VideoInput[TOTAL_VIDEO_SOURCE];

	} function_name;

	unsigned char reg_array[sizeof(RESOLUCTION_MULTICH_AIO) * TOTAL_VIDEO_SOURCE + 24];

} MCU_INTERFACE_MULTICH_AIO;

typedef struct _VIDEOSOURCE
{
        unsigned char scale;

        unsigned char bright;

        unsigned char contrast;

        unsigned char sat;

        unsigned char hue;

        unsigned char r_adjust;

        unsigned char g_adjust;

        unsigned char b_adjust;

} VIDEOSOURCE;

typedef struct
{
	BYTE OnOff;

	BYTE Header;

	BYTE Address;

	BYTE Command;

	BYTE Data1;

	BYTE Data2;

	BYTE Data3;

	BYTE Data4;

} PTZ;

typedef struct
{
	unsigned char AudioLineInSel;

	unsigned char pcmvolume;

	unsigned char lineinvolume;

	unsigned char noisegate;

} AudioCtl;

typedef union _HDRPACKET
{
	struct _TVI_CTRL
	{
		AudioCtl	  audioctl_ex;

		unsigned char padding[26];

	} TVI_CTRL;

	unsigned char     HPB0[30];

} HDRPACKET;

typedef union _PTZ_OTHER
{
    struct _OTHER
    {
		unsigned char	colordepth;

		AudioCtl		audioctl;

		unsigned char	padding[ 3 ];

    } OTHER;

	PTZ            ptz;

} PTZ_OTHER;

typedef struct _MCU_RESOLUCTION
{
		unsigned short vtotal;

		unsigned short htotal;

		unsigned short vactive;

		unsigned short hactive;

		unsigned char  fps;

		unsigned char  modeflag;

		unsigned char  audiosamplerate;

		unsigned char  colorformat;

		unsigned char  hdcponoff;

		unsigned char  hdr2sdr;

		VIDEOSOURCE    vid;

		HDRPACKET      hdrpacket;


		PTZ_OTHER		ptz_other;

} MCU_RESOLUCTION;

typedef union _HDMI20_INTERFACE
{
	struct {

		unsigned char   selport;



		unsigned char   data_port_num;

		unsigned short  bitrate;

		MCU_RESOLUCTION resolution[ 16 ];

	} function_name;

} HDMI20_INTERFACE;

#pragma pack(   )

typedef struct _SC0710_SYS_CFG
{
	CSCMatrix  o_csc_matrix;

	CSCMatrix  o_csc_original;

	ULONG     n_output_video_resolution_fps_1_2;

	ULONG      n_output_video_resolution_cx;

	ULONG      n_output_video_resolution_cxs[ 16 ];

	ULONG      n_output_video_resolution_cy;

	ULONG      n_output_video_resolution_cys[ 16 ];

	ULONG      n_output_video_resolution_fps;

	ULONG      n_output_video_resolution_fpss[ 16 ];

	ULONG      n_output_video_resolution_fps_m;

	ULONG      n_output_video_resolution_fps_ms[ 16 ];

	ULONG	   n_output_video_resolution_fps_base_fps;

	ULONG	   n_output_video_resolution_fps_base_fpss[ 16 ];

	ULONG      n_output_video_resolution_interleaved;

	ULONG      n_output_video_resolution_interleaveds[ 16 ];

	ULONG	   n_output_video_resolution_psf;

	ULONG	   n_output_video_resolution_psfs[ 16 ];

	ULONG	   n_output_video_resolution_is_resets[ 16 ];

	ULONG      n_output_video_resolution_lock;

	ULONG      n_output_video_color_format;

	ULONG      n_output_video_color_bits;

	ULONG      n_output_video_resolution_vbi_lines;

	ULONG      n_output_video_bufsize_header_payload;

	ULONG      n_output_video_bufsize;

	ULONG      n_output_video_resolution_color_range;

	ULONG      n_output_audio_sampling_frequency;

	ULONG	   n_output_audio_resolution_fps_base_fps;

	ULONGLONG  n_output_video_stream_frame_number[ 16 ];

	BYTE*	   p_output_audio_stream_buffer_i2s[ 16 ];

	ULONG      n_output_audio_stream_buffer_size[ 16 ];

	ULONG      n_output_audio_stream_buffer_size_ai;

	ULONG      n_output_audio_stream_buffer_size_ao;

	ULONGLONG  n_output_audio_stream_frame_number[ 16 ];

	LONGLONG   n_output_audio_stream_1mins_diff_duration[ 16 ];

	ULONG	   n_output_audio_resolution_is_resets[ 16 ];

	ULONG      n_output_audio_i2s_format;

	ULONG      n_output_audio_bufsize;

	ULONG	   n_output_multi_sync_channels;

	ULONG      n_input_video_resolution_cx;

	ULONG      n_input_video_resolution_cxs[ 16 ];

	ULONG      n_input_video_resolution_cy;

	ULONG      n_input_video_resolution_cys[ 16 ];

	ULONG      n_input_video_resolution_fps;

	ULONG      n_input_video_resolution_fpss[ 16 ];

	ULONG      n_input_video_resolution_fps_m;

	ULONG      n_input_video_resolution_fps_ms[ 16 ];

	ULONG      n_input_video_resolution_interleaved;

	ULONG      n_input_video_resolution_interleaveds[ 16 ];

	ULONG      n_input_video_resolution_interleaveds_field[ 16 ];

	ULONG      n_input_video_resolution_color_colorimetry;

	ULONG      n_input_video_resolution_color_colorimetrys[ 16 ];

	ULONG      n_input_video_resolution_color_range;

	ULONG      n_input_video_resolution_color_ranges[ 16 ];

	ULONG      n_input_video_resolution_color_type;

	ULONG      n_input_video_resolution_color_types[ 16 ];

	ULONG      n_input_video_resolution_levelb;

	ULONG      n_input_video_resolution_levelbs[ 16 ];

	ULONG	   n_input_video_resolution_psf;

	ULONG	   n_input_video_resolution_psfs[ 16 ];

	ULONG      n_input_video_resolution_3d_stereo_fmt;

	ULONG      n_input_video_resolution_3d_stereo_fmts[ 16 ];

	ULONG      n_input_video_resolution_bitrate;

	ULONG	   n_input_video_resolution_dataport;

	BOOL       b_input_video_resolution_spliter_mode;

	BOOL       b_input_video_resolution_external_sync;

	ULONG      n_input_video_resolution_vbi_lines;

	ULONG      n_input_video_resolution_h_total;

	ULONG      n_input_video_resolution_v_total;

	ULONG      n_input_video_resolution_h_totals[ 16 ];

	ULONG      n_input_video_resolution_v_totals[ 16 ];

	ULONG      n_input_video_resolution_fps_ms_by_p2140[ 16 ];

	ULONG      n_input_video_resolution_hv_polarity;

	ULONG      b_input_video_resolution_vga_h_period_us;

	ULONG      b_input_video_resolution_vga_v_period_us;

	ULONGLONG  b_input_video_resolution_vga_h_period_us_sum;

	ULONGLONG  b_input_video_resolution_vga_v_period_us_sum;

	ULONG      b_input_video_resolution_vga_period_us_counts;

	LONG	   n_input_video_resolution_offset_x_start_vesa[ DEFAULT_MST3367_VESA_SIZE ];

	LONG	   n_input_video_resolution_offset_y_start_vesa[ DEFAULT_MST3367_VESA_SIZE ];

	LONG	   n_input_video_resolution_offset_dirty_vesa[ DEFAULT_MST3367_VESA_SIZE ];

	LONG	   n_input_video_resolution_offset_x_start_eia[ DEFAULT_MST3367_EIA_SIZE ];

	LONG	   n_input_video_resolution_offset_y_start_eia[ DEFAULT_MST3367_EIA_SIZE ];

	LONG	   n_input_video_resolution_offset_dirty_eia[ DEFAULT_MST3367_EIA_SIZE ];

	LONG	   n_input_video_resolution_offset_index;

	LONG	   n_input_video_resolution_offset_x_start_fetch;

	LONG	   n_input_video_resolution_offset_y_start_fetch;

	ULONG      n_input_tvi_signal_locksp[ 32 ];

	BYTE       n_input_video_hdr_packets[ 32 ];



	ULONG      n_input_video_no_audio_packet;

	ULONG      n_input_video_good_adc_phase;

	BOOL	   b_input_video_signal_nintendo_n64;

	ULONG      n_input_video_signal_single_link_under_3G;

	ULONG      n_input_video_signal_dual_link_under_6Gs[ 16 ];

	ULONG      n_input_video_signal_dual_stream_sdis[ 16 ];

	ULONG      n_input_video_signal_dual_link_sdis[ 16 ];

	BOOL       b_dma_length_error;

	BOOL       b_dma_EOP_error;

	BOOL       b_input_video_signal_changed;

	BOOL       b_input_video_signal_changeds[ 16 ];

	ULONG      n_input_audio_sampling_frequency;

	ULONG      n_input_audio_sampling_frequencys[ 16 ];

	ULONG	   n_input_mode_index;

	BYTE	   n_bank_reg;

	ULONG	   n_pcie_generation;

	ULONG      n_pcie_link_width;

	BOOL	   b_is_thermal_support;

	BOOL	   b_is_gv7601_support;

	BOOL	   b_is_tw9910_support;

	BOOL	   b_is_mst3367_support;

	BOOL	   b_is_mst3363_support;

	BOOL       b_is_mcu_support;

	BOOL       b_is_new_12gsdi_support;

	BOOL       b_is_output_card_support;

	ULONG	   n_board_major_version;

	ULONG	   n_board_minor_version;

	ULONG	   n_firmware_major_version;

	ULONG	   n_firmware_minor_version;

	ULONG      n_fpga_version;

	ULONG      n_mcu_version;


	ULONG		n_extend_mcu_version;

	ULONG		n_extend_board_type;

	ULONG      n_firmware_version;

	ULONG	   n_firmware_version_year;

	ULONG	   n_firmware_version_month;

	ULONG	   n_firmware_version_date;

	ULONG	   n_firmware_version_sub;

	BOOL       b_is_background_updated;

	ULONG      n_is_bad_signal_count_vs[16];

	ULONG      n_is_bad_signal_count_a;







	ULONG       n_is_multi_channels_enabled;

	ULONG      n_multi_chdma_remains;



	ULONG	   n_spi_ctrl_reg;

	volatile ULONG n_video_running_state;

	volatile ULONG n_audio_running_state;



	BYTE	   n_SOG_mode;

	BYTE	   n_SOG_vsync_mode;

	BYTE	   n_Interlace_mode;

	ULONG	   n_vsync_width;

	ULONG	   m_RX_RATE;

	BYTE       m_LOCK;

	BYTE       width_multiples[ 4 ];

	BYTE       height_multiples[ 4 ];

	ULONG		n_completedDescCountVideo;

	ULONG		n_completedDescCountAudio;

	ULONG		n_frame_no_swint;



	BYTE *             p_config_bar_address;

    XDMA_CONFIG_REGS * p_regs_config;

    XDMA_IRQ_REGS *    p_regs_interrupt;

    XDMA_ENGINE_REGS * p_regs_xdma_engines[ XDMA_MAX_NUM_CHANNELS ][ XDMA_NUM_DIRECTIONS ];

    XDMA_SGDMA_REGS *  p_regs_xdma_sgdmas[ XDMA_MAX_NUM_CHANNELS ][ XDMA_NUM_DIRECTIONS ];

} SC0710_SYS_CFG;

void SC0710_CLOSE_THREAD_EX( CDevice * pDevice );
#define  SC0710_START_THREAD() { LINUXV4L2_DEBUG( KERN_INFO, "[%02d] SC0710_START_THREAD()\n", pDevice->m_nKsDeviceNumber); down( &(pDevice->m_pControlSemaphore) ); pDevice->m_StopControlPanelAnalysisThread = 0; pDevice->m_RestartControlPanelAnalysisThread = 1; up( &(pDevice->m_pControlSemaphore) ); }
#define  SC0710_CLOSE_THREAD() { SC0710_CLOSE_THREAD_EX( pDevice ); }

#define  SC0710_START_SOFTWARE_INTERRUPT_THREAD() { SC0710_StartSoftwareInterruptThread( pDevice ); }
#define  SC0710_CLOSE_SOFTWARE_INTERRUPT_THREAD() { SC0710_StopSoftwareInterruptThread( pDevice ); }

NTSTATUS SC0710_HwInitialize( CDevice * pDevice );
NTSTATUS SC0710_HwUnInitialize( CDevice * pDevice );


NTSTATUS SC0710_HwVideoComponentsInitialize(CDevice* pDevice, CVideo* pVideo);
NTSTATUS SC0710_HwVideoComponentsUnInitialize(CDevice* pDevice);

NTSTATUS SC0710_HwAudioComponentsInitialize(CDevice* pDevice);
NTSTATUS SC0710_HwAudioComponentsUnInitialize(CDevice* pDevice);

int SC0710_StartControlPanelAnalysisThread( CDevice * pDevice );
int SC0710_StopControlPanelAnalysisThread( CDevice * pDevice );
int SC0710_OnControlPanelAnalysisThread( void* p_context );

NTSTATUS SC0710_PowerDownPeripherals( CDevice * pDevice );

NTSTATUS SC0710_HwProcessAnalogPCIPacket( CDevice * pDevice, ULONG status );
NTSTATUS SC0710_HwProcessAnalogRawVideoFrame(CDevice* pDevice, CVideo* pVideo);


void SC0710_HwProcessAnalogRawVideoFrameMultiCH0( unsigned long p_context );
void SC0710_HwProcessAnalogRawVideoFrameMultiCH1( unsigned long p_context );
void SC0710_HwProcessAnalogRawVideoFrameMultiCH2( unsigned long p_context );
void SC0710_HwProcessAnalogRawVideoFrameMultiCH3( unsigned long p_context );

void MultiCh_MultiCopy_0(struct work_struct* work);
void MultiCh_MultiCopy_1(struct work_struct* work);
void MultiCh_MultiCopy_2(struct work_struct* work);
void MultiCh_MultiCopy_3(struct work_struct* work);


void SC0710_HwProcessAnalogRawAudioFrame( struct work_struct *work );

ULONG SC0710_GetRegister( CDevice * pDevice, ULONG dwIndex );
BOOLEAN SC0710_SetRegister( CDevice * pDevice, ULONG dwIndex, ULONG dwValue );

NTSTATUS SC0710_SetAnalogCrossbarRouteProperty(CDevice* pDevice, ULONG ch, ULONG nVideoInput, ULONG nVideoOutput, ULONG nAudioInput, ULONG nAudioOutput, BOOL THREAD);
NTSTATUS SC0710_EnableHDCP( CDevice * pDevice, ULONG nEnable, ULONG ch );
NTSTATUS SC0710_SetAnalogVideoProcAmpProperties( CDevice * pDevice, ULONG ch, BOOL THREAD );
NTSTATUS SC0710_EableEDIDAccess(CDevice* pDevice, ULONG ch);

BOOLEAN SC0710_AccessSlaveDeviceRegisterS( CDevice * pDevice, ULONG nDevNum, BYTE bDevAddr, BYTE * pTxBuf, ULONG nTxLen, BYTE * pRxBuf, ULONG nRxLen );
BOOLEAN SC0710_AccessI2cRegisterS_NEW( CDevice * pDevice, BYTE bDevAddr, BYTE * pTxBuf, ULONG nTxLen, BYTE * pRxBuf, ULONG nRxLen, ULONG DELAY_100US );

NTSTATUS SC0710_StartSoftwareInterruptThread( CDevice * pDevice );
NTSTATUS SC0710_StopSoftwareInterruptThread( CDevice * pDevice );
NTSTATUS SC0710_OnSoftwareInterruptThread( void* p_context );

BOOLEAN SC0710_DownloadP2140Firmware( CDevice * pDevice, BYTE* pwszFileName );
BOOLEAN SC0710_DownloadC5Firmware_SPI( CDevice * pDevice, BYTE* pwszFileName );
BOOLEAN SC0710_DownloadC5Firmware( CDevice * pDevice, BYTE* pwszFileName );
BOOLEAN SC0710_DownloadC5Firmware_Online( CDevice * pDevice, BYTE* pwszFileName, ULONG powersaving );

BOOLEAN SC0710_DownloadCyclone10Firmware_SPI( CDevice * pDevice );
BOOLEAN SC0720_DownloadC7Firmware_SPI( CDevice * pDevice );
BOOLEAN SC0720_DownloadCyclone10Firmware_SPI( CDevice * pDevice );

NTSTATUS SC0710_EnableHDR2SDR( CDevice * pDevice, ULONG nEnable );

NTSTATUS SC0710_EnableHotPlug( CDevice * pDevice, ULONG nEnable, ULONG ch );

NTSTATUS SC0710_Enable_Dolby( CDevice * pDevice, ULONG nEnable );

void SC0710_PowerSaving( CDevice * pDevice, BOOL THREAD );

void BlkCpy_YUYV_BGR_0( struct work_struct *work );
void BlkCpy_YUYV_BGR_1( struct work_struct *work );
void BlkCpy_YUYV_BGR_2( struct work_struct *work );
void BlkCpy_YUYV_BGR_3( struct work_struct *work );
void BlkCpy_YUYV_BGR_4( struct work_struct *work );
void BlkCpy_YUYV_BGR_5( struct work_struct *work );
void BlkCpy_YUYV_BGR_6( struct work_struct *work );

void BlkCpy_Y_0( struct work_struct *work );
void BlkCpy_Y_1( struct work_struct *work );
void BlkCpy_Y_2( struct work_struct *work );
void BlkCpy_Y_3( struct work_struct *work );
void BlkCpy_Y_4( struct work_struct *work );
void BlkCpy_Y_5( struct work_struct *work );
void BlkCpy_Y_6( struct work_struct *work );

void BlkCpy_C_NV12_0( struct work_struct *work );
void BlkCpy_C_NV12_1( struct work_struct *work );
void BlkCpy_C_NV12_2( struct work_struct *work );
void BlkCpy_C_NV12_3( struct work_struct *work );
void BlkCpy_C_NV12_4( struct work_struct *work );
void BlkCpy_C_NV12_5( struct work_struct *work );
void BlkCpy_C_NV12_6( struct work_struct *work );

void BlkCpy_C_YV12_0( struct work_struct *work );
void BlkCpy_C_YV12_1( struct work_struct *work );
void BlkCpy_C_YV12_2( struct work_struct *work );
void BlkCpy_C_YV12_3( struct work_struct *work );
void BlkCpy_C_YV12_4( struct work_struct *work );
void BlkCpy_C_YV12_5( struct work_struct *work );
void BlkCpy_C_YV12_6( struct work_struct *work );

void BlkCpy_P010_0( struct work_struct *work );
void BlkCpy_P010_1( struct work_struct *work );
void BlkCpy_P010_2( struct work_struct *work );
void BlkCpy_P010_3( struct work_struct *work );
void BlkCpy_P010_4( struct work_struct *work );
void BlkCpy_P010_5( struct work_struct *work );
void BlkCpy_P010_6( struct work_struct *work );



NTSTATUS SC0710_MuteLoopThrough( CDevice * pDevice, ULONG mute );

void MultiChLastLine(CVideo* pVideo, ULONG ch, ULONG cho);

VOID SC0710_background_updated(CDevice* pDevice);


#endif
