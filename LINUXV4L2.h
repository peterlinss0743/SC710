#ifndef LINUXV4L2_H
#define LINUXV4L2_H

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/usb.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/videodev2.h>
#include <media/videobuf2-vmalloc.h>
#include <media/videobuf2-dma-sg.h>
#include <media/videobuf2-dma-contig.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/firmware.h>
#include <linux/rtc.h>
#include <linux/workqueue.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
#include <linux/pm_qos.h>
#endif








#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0)
#include <media/v4l2-device.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-ctrls.h>
#include <linux/v4l2-controls.h>
#include <media/v4l2-event.h>
#include <media/videobuf-vmalloc.h>
#include <media/videobuf-dma-sg.h>
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
#include <media/v4l2-common.h>
#include <media/videobuf-vmalloc.h>
#include <media/videobuf-dma-sg.h>
#else
#include <media/v4l2-common.h>
#include <media/video-buf.h>
#include <linux/highmem.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/control.h>
#include <sound/initval.h>
#else
#include <sound/driver.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/control.h>
#include <sound/initval.h>
#endif

#if LINUX_VERSION_CODE >=  KERNEL_VERSION(3,1,0)

typedef u64 DMA_nnBIT_MASK __deprecated;

#define DMA_32BIT_MASK  (DMA_nnBIT_MASK)DMA_BIT_MASK(32)

#endif

#if LINUX_VERSION_CODE <  KERNEL_VERSION(2,6,21)

struct v4l2_encoder_cmd { __u32 cmd; __u32 flags; union { struct { __u32 data[ 8 ]; } raw; }; };

#endif

#if LINUX_VERSION_CODE <  KERNEL_VERSION(2,6,22)

#define list_first_entry( ptr, type, member ) list_entry( (ptr)->next, type, member )

#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)

enum v4l2_bitrate_mode { V4L2_BITRATE_NONE = 0, V4L2_BITRATE_CBR, V4L2_BITRATE_VBR };

struct v4l2_mpeg_compression { __u32 _unsupport_struct; };

#endif

#if LINUX_VERSION_CODE <  KERNEL_VERSION(2,6,26)

#define V4L2_CID_SHARPNESS (V4L2_CID_BASE + 27)

#endif

#if LINUX_VERSION_CODE >=  KERNEL_VERSION(5,11,0)
#define V4L2_CTRL_CLASS_MPEG            V4L2_CTRL_CLASS_CODEC
#define V4L2_CID_MPEG_BASE              V4L2_CID_CODEC_BASE
#endif

#if LINUX_VERSION_CODE >=  KERNEL_VERSION(6,1,0)
#define snd_dma_continuous_data(x)	NULL
#endif

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,1,0)
#define V4L2_CID_MPEG_VIDEO_H264_PROFILE                (V4L2_CID_MPEG_BASE+363)
enum v4l2_mpeg_video_h264_profile {
        V4L2_MPEG_VIDEO_H264_PROFILE_BASELINE                   = 0,
        V4L2_MPEG_VIDEO_H264_PROFILE_CONSTRAINED_BASELINE       = 1,
        V4L2_MPEG_VIDEO_H264_PROFILE_MAIN                       = 2,
        V4L2_MPEG_VIDEO_H264_PROFILE_EXTENDED                   = 3,
        V4L2_MPEG_VIDEO_H264_PROFILE_HIGH                       = 4,
        V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_10                    = 5,
        V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_422                   = 6,
        V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_444_PREDICTIVE        = 7,
        V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_10_INTRA              = 8,
        V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_422_INTRA             = 9,
        V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_444_INTRA             = 10,
        V4L2_MPEG_VIDEO_H264_PROFILE_CAVLC_444_INTRA            = 11,
        V4L2_MPEG_VIDEO_H264_PROFILE_SCALABLE_BASELINE          = 12,
        V4L2_MPEG_VIDEO_H264_PROFILE_SCALABLE_HIGH              = 13,
        V4L2_MPEG_VIDEO_H264_PROFILE_SCALABLE_HIGH_INTRA        = 14,
        V4L2_MPEG_VIDEO_H264_PROFILE_STEREO_HIGH                = 15,
        V4L2_MPEG_VIDEO_H264_PROFILE_MULTIVIEW_HIGH             = 16,
};

#define V4L2_CID_MPEG_VIDEO_H264_LEVEL                  (V4L2_CID_MPEG_BASE+359)
enum v4l2_mpeg_video_h264_level {
        V4L2_MPEG_VIDEO_H264_LEVEL_1_0  = 0,
        V4L2_MPEG_VIDEO_H264_LEVEL_1B   = 1,
        V4L2_MPEG_VIDEO_H264_LEVEL_1_1  = 2,
        V4L2_MPEG_VIDEO_H264_LEVEL_1_2  = 3,
        V4L2_MPEG_VIDEO_H264_LEVEL_1_3  = 4,
        V4L2_MPEG_VIDEO_H264_LEVEL_2_0  = 5,
        V4L2_MPEG_VIDEO_H264_LEVEL_2_1  = 6,
        V4L2_MPEG_VIDEO_H264_LEVEL_2_2  = 7,
        V4L2_MPEG_VIDEO_H264_LEVEL_3_0  = 8,
        V4L2_MPEG_VIDEO_H264_LEVEL_3_1  = 9,
        V4L2_MPEG_VIDEO_H264_LEVEL_3_2  = 10,
        V4L2_MPEG_VIDEO_H264_LEVEL_4_0  = 11,
        V4L2_MPEG_VIDEO_H264_LEVEL_4_1  = 12,
        V4L2_MPEG_VIDEO_H264_LEVEL_4_2  = 13,
        V4L2_MPEG_VIDEO_H264_LEVEL_5_0  = 14,
        V4L2_MPEG_VIDEO_H264_LEVEL_5_1  = 15,
};
#endif

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,24)
enum videobuf_state2 {
         VIDEOBUF_NEEDS_INIT = 0,
         VIDEOBUF_PREPARED   = 1,
         VIDEOBUF_QUEUED     = 2,
         VIDEOBUF_ACTIVE     = 3,
         VIDEOBUF_DONE       = 4,
         VIDEOBUF_ERROR      = 5,
         VIDEOBUF_IDLE       = 6,
};
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,9,0)
#define V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC	0x2000
#endif


typedef          char           CHAR;
typedef          short         SHORT;
typedef			 int			 INT;

typedef          int            LONG;
typedef unsigned int           ULONG;

typedef     long long       LONGLONG;
typedef unsigned char          UCHAR;
typedef unsigned short        USHORT;
typedef unsigned int		    UINT;
typedef unsigned int		  UINT32;
typedef unsigned long long ULONGLONG;
typedef unsigned char           BYTE;
typedef unsigned char          UINT8;
typedef unsigned short          WORD;
typedef unsigned int           DWORD;
typedef unsigned char        BOOLEAN;
typedef void                    VOID;
typedef void *                 PVOID;
typedef unsigned char*		  PUCHAR;



#define DMA_ERROR_1080i 1

#define WOLF 1

#if   WOLF
#define INT_BYTES_A						4096
#define FAST_DETECT						1
#define OUTPUT_REAL_VIDEO_TIME_STAMP	1
#define NO_SIGNAL_NO_OUTPUT				1

#else
#define OUTPUT_IDEAL_VIDEO_TIME_STAMP	1
#define INT_BYTES_A						4096




#endif









#define MT_LINECOPY 1









#ifdef FAST_DETECT
#define DEC_LOOP_CNT 5
#else
#define DEC_LOOP_CNT 10
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
#define V4L2_IN_CAP_PRESETS             0x00000001
#define V4L2_IN_CAP_STD                 0x00000004
#endif


#define LINUXV4L2_DEFAULT_VIDEO_STANDARD	V4L2_STD_PAL_B








#ifdef DEF_NO_LOG

#define LINUXV4L2_PRINT( level, msg...)

#define LINUXV4L2_DEBUG( level, msg...)

#else

	#define LINUXV4L2_PRINT( level, msg... )	printk( level "[LINUXV4L2] : " msg )

	#ifdef DEF_DEBUG

		#define LINUXV4L2_DEBUG( level, msg... )	printk( level "[LINUXV4L2] : " msg )

	#else

		#define LINUXV4L2_DEBUG( level, msg...)

	#endif

#endif


#define LINUXV4L2_VMALLOC_MODULE

#define MIN(x,y) (x<y)?(x):(y)

#define READ_REGISTER_ULONG(x) readl(x)

#define WRITE_REGISTER_ULONG(x,y) writel(y,x)

#define FREE( p ) { if( p ) { kfree( p ); p = NULL; } }
#define FREE_MEMORY( p ) { if( p ) { vfree( p ); p = NULL; } }

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif


#define MAX_SUB_DEVICE_NUM_X_2 32

#define MAX_SUB_DEVICE_NUM     16

#define MAX_DMA_BUFFER_NUM      132



#define MULTI_CH_RING 8

#if (MULTI_CH_RING == 4)
#define SC0710_DEF_MULTI_CHDMA_ROW	64
#else
#define SC0710_DEF_MULTI_CHDMA_ROW	107
#endif


#define SC0710_SPEED_LINE_COPY_BUFFER_SIZE	110592

#define MAX_REG_BUFFER_NUM      6

#define V4L2_CID_SWITCH_SPEED                       (V4L2_CID_BASE + 200)

#define V4L2_CID_SWITCH_TABLE                       (V4L2_CID_BASE + 201)

#define V4L2_CID_HDCP_ENABLE                        (V4L2_CID_BASE + 202)

#define V4L2_CID_HDCP_STATUS                        (V4L2_CID_BASE + 203)

#define V4L2_CID_HV_SCALE_FACTOR                    (V4L2_CID_BASE + 204)

#define V4L2_CID_H264_HV_SCALE_FACTOR               (V4L2_CID_BASE + 205)

#define V4L2_CID_WATCH_DOG_PERIOD                   (V4L2_CID_BASE + 206)

#define V4L2_CID_WATCH_DOG		                    (V4L2_CID_BASE + 207)

#define V4L2_CID_WATCH_DOG_ENABLE                   (V4L2_CID_BASE + 212)



#define V4L2_CID_SPECIAL_CAMERA			            (V4L2_CID_BASE + 214)

#define V4L2_CID_NO_SIGNAL_NO_OUTPUT	            (V4L2_CID_BASE + 215)

#define V4L2_CID_NO_SIGNAL_YUV_PATTERN				(V4L2_CID_BASE + 216)

#define V4L2_CID_FRAME_RATE		                    (V4L2_CID_BASE + 208)

#define V4L2_CID_AUDIO_SAMPLING_FREQ                (V4L2_CID_BASE + 211)

#define V4L2_CID_MOTION_STATUS						(V4L2_CID_BASE + 250)

#define V4L2_CID_MOTION_SENSITIVITY_LOW_THRESHOLD	(V4L2_CID_BASE + 251)

#define V4L2_CID_MOTION_SENSITIVITY_HIGH_THRESHOLD	(V4L2_CID_BASE + 252)

#define V4L2_CID_MOTION_MASK_CX						(V4L2_CID_BASE + 253)

#define V4L2_CID_MOTION_MASK_CY						(V4L2_CID_BASE + 254)

#define V4L2_CID_OSD_POS_X							(V4L2_CID_BASE + 260)

#define V4L2_CID_OSD_POS_Y							(V4L2_CID_BASE + 261)

#define V4L2_CID_OSD_COLOR							(V4L2_CID_BASE + 262)

#define V4L2_CID_OSD_TEXT							(V4L2_CID_BASE + 263)

#define V4L2_CID_GPIO_DIRECTION						(V4L2_CID_BASE + 270)

#define V4L2_CID_GPIO_DATA							(V4L2_CID_BASE + 271)

#define V4L2_CID_UPDATE_FW							(V4L2_CID_BASE + 272)

#define V4L2_CID_TW5864_MOTION_STATUS_00			(V4L2_CID_BASE + 280)
#define V4L2_CID_TW5864_MOTION_STATUS_01			(V4L2_CID_BASE + 281)
#define V4L2_CID_TW5864_MOTION_STATUS_02			(V4L2_CID_BASE + 282)
#define V4L2_CID_TW5864_MOTION_STATUS_03			(V4L2_CID_BASE + 283)
#define V4L2_CID_TW5864_MOTION_STATUS_04			(V4L2_CID_BASE + 284)
#define V4L2_CID_TW5864_MOTION_STATUS_05			(V4L2_CID_BASE + 285)
#define V4L2_CID_TW5864_MOTION_STATUS_06			(V4L2_CID_BASE + 286)
#define V4L2_CID_TW5864_MOTION_STATUS_07			(V4L2_CID_BASE + 287)

#define V4L2_CID_TW5864_MOTION_TEMPORAL_SENSITIVITY	(V4L2_CID_BASE + 290)
#define V4L2_CID_TW5864_MOTION_SPATIAL_SENSITIVITY	(V4L2_CID_BASE + 291)
#define V4L2_CID_TW5864_MOTION_LEVEL_SENSITIVITY	(V4L2_CID_BASE + 292)
#define V4L2_CID_TW5864_MOTION_SPEED				(V4L2_CID_BASE + 293)

#define V4L2_CID_X_RES_DETECTED						(V4L2_CID_BASE + 294)
#define V4L2_CID_Y_RES_DETECTED						(V4L2_CID_BASE + 295)
#define V4L2_CID_FPS_DETECTED						(V4L2_CID_BASE + 296)

#define V4L2_CID_INTERLEAVE_DETECTED				(V4L2_CID_BASE + 297)
#define V4L2_CID_ENC_DEINTERLACE					(V4L2_CID_BASE + 298)

#define V4L2_CID_RESET_I2S							(V4L2_CID_BASE + 299)

#define V4L2_CID_VBI_ENABLE							(V4L2_CID_BASE + 300)

#define V4L2_CID_VBI_LINE							(V4L2_CID_BASE + 301)

#define V4L2_CID_VBI_MIX_MODE						(V4L2_CID_BASE + 348)

#define V4L2_CID_HDMI_MIXER							(V4L2_CID_BASE + 302)

#define V4L2_CID_HANC_TIMECODE_H_M					(V4L2_CID_BASE + 303)
#define V4L2_CID_HANC_TIMECODE_S_F					(V4L2_CID_BASE + 304)

#define V4L2_CID_SET_I2C_REGISTER					(V4L2_CID_BASE + 305)

#define V4L2_CID_ACCESS_I2C_REGISTER				(V4L2_CID_BASE + 306)

#define V4L2_CID_SIZE_NOT_MATCH						(V4L2_CID_BASE + 307)

#define V4L2_CID_INVERT_UV							(V4L2_CID_BASE + 308)

#define V4L2_CID_SET_I2C_CHIPADDR					(V4L2_CID_BASE + 309)

#define V4L2_CID_CAMERA_TYPE						(V4L2_CID_BASE + 310)

#define V4L2_CID_CommOverCableParams				(V4L2_CID_BASE + 311)

#define V4L2_CID_Quad_Link_2_Sample_Interleave		(V4L2_CID_BASE + 312)

#define V4L2_CID_MZ0380_FIRMWARE_STATUS				(V4L2_CID_BASE + 313)

#define V4L2_CID_MZ0380_OUTPUT_ORIGINAL_DMA_VIDEO	(V4L2_CID_BASE + 314)

#define V4L2_CID_COLOR_RANGE						(V4L2_CID_BASE + 315)

#define V4L2_CID_UPDATE_EDID						(V4L2_CID_BASE + 316)

#define V4L2_CID_VIDEO_INPUT_BANDWIDTH				(V4L2_CID_BASE + 317)

#define V4L2_CID_VIDEO_INPUT_EQ						(V4L2_CID_BASE + 318)

#define V4L2_CID_UPDATE_MCU							(V4L2_CID_BASE + 319)

#define V4L2_CID_CommOverCableParams_First			(V4L2_CID_BASE + 320)
#define V4L2_CID_CommOverCableParams_Second			(V4L2_CID_BASE + 321)

#define V4L2_CID_UPDATE_PID							(V4L2_CID_BASE + 322)

#define V4L2_CID_SET_I2C_BANK						(V4L2_CID_BASE + 323)

#define V4L2_CID_TIME_ZONE							(V4L2_CID_BASE + 324)

#define V4L2_CID_720_OUTPUT							(V4L2_CID_BASE + 325)

#define V4L2_CID_TIME_ZONE_HALF_HOUR				(V4L2_CID_BASE + 326)

#define V4L2_CID_DUMP_REGISTER						(V4L2_CID_BASE + 327)

#define V4L2_CID_SET_MEDICAL						(V4L2_CID_BASE + 328)

#define V4L2_CID_SET_BAR_BLOCK						(V4L2_CID_BASE + 329)

#define V4L2_CID_SET_BAR_OFFSET						(V4L2_CID_BASE + 330)

#define V4L2_CID_ACCESS_BAR							(V4L2_CID_BASE + 331)

#define V4L2_CID_HARDWARE_VERSION					(V4L2_CID_BASE + 332)

#define V4L2_CID_HDMI_HDRtoSDR						(V4L2_CID_BASE + 333)

#define V4L2_CID_DEVICE_VIDEO_INPUT_CONFIG			(V4L2_CID_BASE + 334)

#define V4L2_CID_DEVICE_AUDIO_INPUT_CONFIG			(V4L2_CID_BASE + 335)

#define V4L2_CID_OUTPUT_VIDEO_AUDIO					(V4L2_CID_BASE + 336)

#define V4L2_CID_AUDIO_CHANNEL						(V4L2_CID_BASE + 337)

#define V4L2_CID_POWER_SAVING						(V4L2_CID_BASE + 338)

#define V4L2_CID_UPDATE_EXTEND_MCU					(V4L2_CID_BASE + 339)

#define V4L2_CID_MCU_VERSION						(V4L2_CID_BASE + 340)

#define V4L2_CID_EXTEND_MCU_VERSION					(V4L2_CID_BASE + 341)

#define V4L2_CID_C7_VERSION							(V4L2_CID_BASE + 342)

#define V4L2_CID_FW_VERSION							(V4L2_CID_BASE + 343)

#define V4L2_CID_MUTE_LOOPTHROUGH					(V4L2_CID_BASE + 344)

#define V4L2_CID_MAC_ADDRESS_1						(V4L2_CID_BASE + 345)

#define V4L2_CID_MAC_ADDRESS_2						(V4L2_CID_BASE + 346)

#define V4L2_CID_AUTOPHASE							(V4L2_CID_BASE + 347)

#define V4L2_PCIE_CONNECTION						(V4L2_CID_BASE + 349)

#define V4L2_CID_USE_GPUDIRECT						(V4L2_CID_BASE + 350)

#define V4L2_CID_VIDEO_CONNECTIONS					(V4L2_CID_BASE + 351)

#define V4L2_CID_ENABLE_DIRECT						(V4L2_CID_BASE + 352)

#define V4L2_CID_UVC_STATUS							(V4L2_CID_BASE + 353)





#define V4L2_CID_MPEG_VIDEO_QUALITY					(V4L2_CID_MPEG_BASE + 250)

#define V4L2_CID_MPEG_VIDEO_FRAMERATE				(V4L2_CID_MPEG_BASE + 251)

#define V4L2_CID_MPEG_VIDEO_QPSTEP					(V4L2_CID_MPEG_BASE + 252)

#define V4L2_CID_MPEG_VIDEO_OSD						(V4L2_CID_MPEG_BASE + 253)

#define V4L2_CID_MPEG_VIDEO_OSD_LINE				(V4L2_CID_MPEG_BASE + 254)

#define V4L2_CID_MPEG_SET_KEY_FRAME					(V4L2_CID_MPEG_BASE + 255)

#define V4L2_CID_MPEG_VIDEO_OSD_ENABLE				(V4L2_CID_MPEG_BASE + 256)

#define V4L2_CID_MPEG_SET_SLICE						(V4L2_CID_MPEG_BASE + 257)

#define V4L2_CID_MPEG_VIDEO_MAX_QP					(V4L2_CID_MPEG_BASE + 258)

#define V4L2_CID_MPEG_VIDEO_MIN_QP					(V4L2_CID_MPEG_BASE + 259)

#define V4L2_CID_MPEG_VIDEO_OSD_X_POSITION			(V4L2_CID_MPEG_BASE + 260)

#define V4L2_CID_MPEG_VIDEO_OSD_Y_POSITION			(V4L2_CID_MPEG_BASE + 261)

#define V4L2_CID_MPEG_VIDEO_OSD_FONT_SIZE			(V4L2_CID_MPEG_BASE + 262)

#define V4L2_CID_MPEG_VIDEO_OSD_TEXT_COLOR			(V4L2_CID_MPEG_BASE + 263)

#define V4L2_CID_MPEG_XET_DEBUG_EEPROM				(V4L2_CID_MPEG_BASE + 264)
#define V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY		(V4L2_CID_MPEG_BASE + 265)
#define V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_ACCESS	(V4L2_CID_MPEG_BASE + 266)
#define V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_2		(V4L2_CID_MPEG_BASE + 267)
#define V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_3		(V4L2_CID_MPEG_BASE + 268)
#define V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_4		(V4L2_CID_MPEG_BASE + 269)
#define V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_5		(V4L2_CID_MPEG_BASE + 270)

#define V4L2_CID_MPEG_VIDEO_OSD_PICTURE_PARM		(V4L2_CID_MPEG_BASE + 271)
#define V4L2_CID_MPEG_VIDEO_OSD_PICTURE				(V4L2_CID_MPEG_BASE + 272)

#define V4L2_CID_HDR_PACKETS						(V4L2_CID_MPEG_BASE + 273)
#define V4L2_CID_READ_EDID							(V4L2_CID_MPEG_BASE + 274)






#define DELAY_to_PLAY								60


typedef struct _GENNUM_INFO_
{
	WORD m_FRAMES;
	WORD m_SECONDS;
	WORD m_MINUTES;
	WORD m_HOURS;

} _GENNUM_INFO;

typedef union _CSCMatrix
{
    struct
	{
	    BYTE data[ 24 ];

	} b;

	struct
	{
		USHORT CSC_M11, CSC_M12, CSC_M13, CSC_M21, CSC_M22, CSC_M23, CSC_M31, CSC_M32, CSC_M33;

		USHORT CSC_A1, CSC_A2, CSC_A3;

	} w;

} CSCMatrix;

typedef void (*callback)(unsigned long);

typedef struct Interrupt_Container {

	BYTE interrupt_arrived;

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	u64 ts;
	#else
	struct timeval ts;
	#endif


} Interrupt_Container;

#pragma pack( push, 1 )

typedef struct {

	UINT32 identifier;

	UINT32 reserved_1[ 4 ];

	UINT32 msiEnable;

} XDMA_CONFIG_REGS;


typedef struct {

    UINT32 identifier;

    UINT32 userIntEnable;

    UINT32 userIntEnableW1S;

    UINT32 userIntEnableW1C;

    UINT32 channelIntEnable;

    UINT32 channelIntEnableW1S;

    UINT32 channelIntEnableW1C;

    UINT32 reserved_1[ 9 ];

    UINT32 userIntRequest;

    UINT32 channelIntRequest;

    UINT32 userIntPending;

    UINT32 channelIntPending;

    UINT32 reserved_2[ 12 ];

    UINT32 userVector[ 4 ];

    UINT32 reserved_3[ 4 ];

    UINT32 channelVector[ 2 ];

} XDMA_IRQ_REGS;

typedef struct {

    UINT32 identifier;

    UINT32 control;

    UINT32 controlW1S;

    UINT32 controlW1C;

    UINT32 reserved_1[ 12 ];

    UINT32 status;

    UINT32 statusRC;

    UINT32 completedDescCount;

    UINT32 alignments;

    UINT32 reserved_2[ 14 ];

    UINT32 pollModeWbLo;

    UINT32 pollModeWbHi;

    UINT32 intEnableMask;

    UINT32 intEnableMaskW1S;

    UINT32 intEnableMaskW1C;

    UINT32 reserved_3[ 9 ];

    UINT32 perfCtrl;

    UINT32 perfCycLo;

    UINT32 perfCycHi;

    UINT32 perfDatLo;

    UINT32 perfDatHi;

    UINT32 perfPndLo;

    UINT32 perfPndHi;

} XDMA_ENGINE_REGS;

typedef struct {

    UINT32 identifier;

    UINT32 reserved_1[ 31 ];

    UINT32 firstDescLo;

    UINT32 firstDescHi;

    UINT32 firstDescAdj;

} XDMA_SGDMA_REGS, * PXDMA_SGDMA_REGS;

typedef struct {

	unsigned char  magic_pattern[5];


	unsigned short : 2;

	unsigned short channel : 2;

	unsigned short : 1;

	unsigned short line_number_b0 : 5;

	unsigned short : 2;

	unsigned short line_number_b5 : 4;



	unsigned short line_number_b9 : 3;

	unsigned short : 3;

	unsigned short width_b0 : 8;

	unsigned short : 2;


	unsigned char width_b8 : 5;

	unsigned char vid_mode : 3;

} C5_SDI_HEADERS_10BITS, * PC5_SDI_HEADERS_10BITS;

typedef struct {

	unsigned char  magic_pattern[4];

	unsigned short channel : 2;

	unsigned short channel_b2 : 1;

	unsigned short line_number : 12;

	unsigned short channel_b3 : 1;

	unsigned short width : 13;

	unsigned short vid_mode : 3;

} C5_SDI_HEADERS, * PC5_SDI_HEADERS;

typedef struct {

	unsigned short  magic_pattern[ 4 ];

	unsigned char info[ 8 ];



} C5_SDI_HEADERS_P01X, * PC5_SDI_HEADERS_P01X;

#pragma pack(   )

typedef struct CVideoBuffer {


	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
	struct vb2_v4l2_buffer vb;
	#else
	struct vb2_buffer	vb;
	#endif

	struct list_head	list;

	ULONG				input;

} CVideoBuffer;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
#define buffer_state		vb.vb2_buf.state
#define vb2_buffer_cross	vb.vb2_buf
#else
#define buffer_state		vb.state
#define vb2_buffer_cross	vb
#endif

#define MAX_VB 16

typedef struct CAudioBuffer {

	struct snd_dma_buffer * m_pKsBuffer;

} CAudioBuffer;

typedef struct _H264_STREAM_HEADER
{
	DWORD nFrameBufferSize : 24;

	DWORD nFrameType       :  1;

	DWORD nMotionStatus    :  1;

	DWORD nLockStatus      :  1;

	DWORD nReserved        :  5;

} H264_STREAM_HEADER;

typedef struct CVideo {

	struct CDevice *        m_pDevice;
	ULONG                   m_nSubDeviceNumber;
	ULONG                   m_nSubDeviceType;

	ULONG                   m_nStandard;
	ULONG                   m_nLockStatus;
	ULONG                   m_nLockStatusMask_Spliter;
	ULONGLONG               m_nLockAccumulatorCounter_Spliter;
	ULONG					m_nDropDecumulatorCounter_Spliter;
	ULONG					m_nSwitchNumber_Spliter;
	ULONG					m_nSwitchSpeed_Spliter;
	BYTE					m_pSwitchTable_Spliter[ 12 ];

	ULONG                   m_nSrcFrameWidth;
	ULONG                   m_nSrcFrameHeight;


	ULONG                   m_nDstFrameWidth;
	ULONG                   m_nDstFrameHeight;
	ULONG                   m_nDstFramePitch;

	ULONG                   m_nDstFrameSize;

	ULONG                   m_nDstFrameInfoSet;
	ULONG                   m_nS_Parm_Set;

	ULONG                   m_nFramePixelFormat;
	ULONG                   m_nFramePixelField;
	ULONG                   m_nFrameRate;

	ULONGLONG               m_nFrameNumber;
	ULONGLONG               m_nFrameLastLine_cnt;
	ULONGLONG               m_nFrameNumberForQueue;
	UINT                    m_nFrameCopyPolarity;

	ULONG					m_nEncoderMode;
	ULONG					m_nEncoderBitRate;
	ULONG					m_nEncoderQuality;
	ULONG					m_nEncoderGop;
	ULONG					m_nEncoderQPStep;

	spinlock_t              m_sKsBufferListSpinLock;
	struct list_head        m_sKsBufferList;


	ULONG					m_nFirstFrameNumber;

	LONGLONG				m_nLastReferenceClockTime;
	LONGLONG				m_nLastPresentationTime;

	LONGLONG				m_nNowPresentationTime;
	LONGLONG				m_nFirstPresentationTime;

	LONG					m_nLastPresentationTimeMultipler;
	LONGLONG				m_nLastPresentationTimeFps;
	LONGLONG				m_nLastPresentationTimeFpsCounts;
	LONGLONG				m_nLastPresentationTimeFpsErrors;

	ULONG					m_nCustomEncoderSlice;
	ULONG					m_nCustomEncoderKeyFrameArrived;

	struct vb2_queue		vb_vidq;
	struct mutex			mutex;

	volatile ULONG			m_MultiCh_MultiCpy[4];

	struct work_struct		w_multich_multicopy[4];

	spinlock_t				m_sMultiCh_MultiCopySpinLock;

	ULONG					m_native_fmt;

	struct workqueue_struct* wq_multich_multicpy;

	wait_queue_head_t		m_wait_queue_parsing_lines;

	ULONG					m_InNoVideoDPC;

	spinlock_t				m_sNo_Signal_SpinLock;

	LONGLONG				n_output_audio_stream_1mins_diff_duration;
	ULONG					m_nAudioResidue;
	BYTE					m_audio_adjust_buffer[4096 * 4 * 4];
	ULONG					m_previous_video_ring;
	ULONG					m_previous_h264_ring;

	ULONG					m_nCustomAnalogVideoVbiLinesEnableProperty;
	ULONG					m_nCustomAnalogVideoVbiLinesProperty;
	ULONG					m_nCustomAnalogVideoVbiMixModeProperty;

	ULONG					m_strorder;
	ULONG					m_nCustomAnalogOutputVideoAudioProperty;

	struct pm_qos_request 	pm_req;

#if defined(AME_SC0710) || defined(AME_SC0720)

	BYTE*					m_pTmpVBuf[8][2];

	ULONG				  m_Multi_Copy;

	ULONG				  m_Multi_Copy_Err_Cnt;
	ULONG				  m_MultiCh_Copy_Err_Cnt;

	struct workqueue_struct *wq_blkcpy;

	struct work_struct w_blkcpy_yuyv_bgr[ 7 ];

	struct work_struct w_blkcpy_y[ 7 ];

	struct work_struct w_blkcpy_nv12[ 7 ];

	struct work_struct w_blkcpy_yv12[ 7 ];

	struct work_struct w_blkcpy_p010[ 7 ];

#endif


	BYTE						m_valid[256];

	ULONG						m_nInputChanged;


} CVideo;

typedef struct CAudio {

	struct CDevice *           m_pDevice;
	ULONG                      m_nSubDeviceNumber;
	ULONG                      m_nSubDeviceType;
	ULONG                      m_nInput;
	ULONG                      m_nVolume;
	ULONG                      m_nRate;
	ULONG                      m_nChannels;
	ULONG                      m_nSampleBits;
	ULONG                      m_nFrameSize;

	ULONGLONG                  m_nFrameNumber;

	ULONG                      m_nFrameDMANumber;
	ULONG                      m_nFrames;
	struct snd_pcm_substream * m_pKsSubStream;

	BYTE						m_audio_temp[ 3764 ];
	BYTE						m_audio_residue_buffer[ 3764 * 2 ];
	ULONG						m_audio_data_residue;

	LONGLONG				n_output_audio_stream_1mins_diff_duration;
	ULONG					m_nAudioResidue;
	BYTE					m_audio_adjust_buffer[4096 * 4 * 4];

} CAudio;



#define TW6816_MAX_BUFFER 2

typedef struct CDevice {

	struct pci_dev *       m_pKsDevice;

	ULONG                  m_nKsDeviceNumber;
	ULONG                  m_nKsDeviceBusNumber;
	ULONG                  m_nKsDeviceVideoNumber;
	ULONG                  m_nKsDeviceAudioNumber;
	ULONG                  m_nKsDeviceVideoEncoderNumber;
	ULONG                  m_nKsDeviceAudioEncoderNumber;
	struct video_device *  m_pKsVideo                              [ MAX_SUB_DEVICE_NUM ];
	struct snd_card *      m_pKsAudioCard                          [ MAX_SUB_DEVICE_NUM ];
	struct snd_pcm  *      m_pKsAudioPCM                           [ MAX_SUB_DEVICE_NUM ];
	CVideo *               m_pVideo                                [ MAX_SUB_DEVICE_NUM ];
	CAudio *               m_pAudio                                [ MAX_SUB_DEVICE_NUM ];
	volatile ULONG	       m_eVideoStreamState                     [ MAX_SUB_DEVICE_NUM ];
	ULONG				   m_eAudioStreamState                     [ MAX_SUB_DEVICE_NUM ];
	struct video_device *  m_pKsVideoEncoder                       [ MAX_SUB_DEVICE_NUM ];
	struct snd_card *      m_pKsAudioEncoderCard                   [ MAX_SUB_DEVICE_NUM ];
	struct snd_pcm  *      m_pKsAudioEncoderPCM                    [ MAX_SUB_DEVICE_NUM ];
	CVideo *               m_pVideoEncoder                         [ MAX_SUB_DEVICE_NUM ];
	CAudio *               m_pAudioEncoder                         [ MAX_SUB_DEVICE_NUM ];
	ULONG				   m_eVideoEncoderStreamState              [ MAX_SUB_DEVICE_NUM ];
	ULONG				   m_eAudioEncoderStreamState              [ MAX_SUB_DEVICE_NUM ];
	dma_addr_t             m_pDmaBaseCommonPhysicalBuffer          [ MAX_DMA_BUFFER_NUM ];
	BYTE *                 m_pDmaBaseCommonBuffer                  [ MAX_DMA_BUFFER_NUM ];
	ULONG                  m_nDmaBaseCommonBufferSize              [ MAX_DMA_BUFFER_NUM ];
	ULONG                  m_nDmaBaseCommonBufferNumber            [ MAX_DMA_BUFFER_NUM ];
	ULONG                  m_nDmaBaseCommonBufferCustomMiscProperty[ MAX_DMA_BUFFER_NUM ];
	BYTE __iomem *         m_pRegBaseCommonBuffer                  [ MAX_REG_BUFFER_NUM ];
	ULONG                  m_nRegBaseCommonBufferSize              [ MAX_REG_BUFFER_NUM ];
	ULONG			       m_pRegBaseCommonPhysicalBuffer          [ MAX_REG_BUFFER_NUM ];

	struct tasklet_struct  m_sTasklet;
	struct tasklet_struct  m_sTasklet_video;
	struct tasklet_struct  m_sTasklet_audio;

	ULONGLONG              m_nTaskletExtraParameterA;
	ULONGLONG              m_nTaskletExtraParameterB;
	ULONGLONG              m_nTaskletExtraParameterC;

	ULONGLONG              m_nTaskletExtraParameter_a;

	volatile BOOLEAN	   m_hInterruptAccessLock;

	struct semaphore       m_sComponentAccessSemaphore;
	ULONG		           m_nDummy8[8];
	wait_queue_head_t	   m_wait_queue_component;
	ULONG				   m_component_ready;

	volatile ULONG         m_nAnalogCaptureStreamPowerReference;
	volatile ULONG         m_nVideoCaptureStreamPowerReference;
	volatile ULONG         m_nAudioCaptureStreamPowerReference;

	ULONG                  m_nCustomHardwareTopologyProperty;
	ULONG                  m_nCustomVideoStandardProperty;
	ULONG                  m_nCustomAudioStandardProperty;

	ULONG					m_nCustomGpioDirectionProperty;
	ULONG					m_nCustomGpioDataProperty;
	ULONG					m_nCustomGpioSupportProperty;

	ULONG				   m_nCustomSoftwareWatchdogDurationProperty;
	struct task_struct *   m_pControlThread;
	struct task_struct *   m_pSoftwareinterruptThread;
	struct task_struct *   m_pVideoLossThread;
	struct task_struct *   m_pAudioLossThread;

	struct task_struct*		m_pMultiChVideoLossThread;


	#if defined(AME_MZ0380) || defined(AME_SC0710) || defined(AME_SC0720)
	struct workqueue_struct *wq_audio;
	struct work_struct w_audio;
	#endif



	ULONG				   m_ReadyToStopControlThread;
	ULONG				   m_ReadyToStopVideoLossThread;
	ULONG				   m_ReadyToStopAudioLossThread;

	ULONG					m_ReadyToStopMultiChVideoLossThread;

	ULONG				   m_nCustomAnalogVideoScaleOutputProperty;
	PVOID				   m_pCustomSystemConfigProperty;
	ULONG				   m_nAnalogCopyProtMacrovisionProperty;
	ULONG				   m_nAnalogCopyProtMacrovisionPropertys[ 16 ];
	ULONG				   m_nAnalogVideoDecoderStatusProperty;
	ULONG					m_nAnalogVideoDecoderStatusPropertys[ 16 ];
	ULONG				   m_nAnalogCrossbarVideoInputProperty;
	ULONG		          m_nAnalogCrossbarVideoInputPropertys[16];
	ULONG				   m_nAnalogCrossbarAudioInputProperty;
	ULONG		          m_nAnalogCrossbarAudioInputPropertys[16];

	ULONG				   m_nAnalogCrossbarVideoOutputProperty;
	ULONG				   m_nAnalogCrossbarAudioOutputProperty;

	ULONG				   m_nCustomAnalogVideoResolutionProperty;
	ULONG				   m_nCustomAnalogVideoFrameRateProperty;
	ULONG				   m_nCustomAnalogVideoInterleavedProperty;


	ULONG		          m_nCustomEncoderVideoResolutionProperty[ 32 ];
	ULONG		          m_nAnalogVideoCompressionKeyframeRateProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoBitRateModeProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoBitRateProperty[ 32 ];
	ULONG		          m_nAnalogVideoCompressionQualityProperty[ 32 ];
	ULONG		          m_nAnalogVideoCompressionOverrideKeyframeProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoBframesPerKeyframeProperty[ 32 ];

	BOOLEAN				m_bCustomOsdTextEnableProperty;
	BOOLEAN				m_bCustomOsdTextSequenceTypeProperty;
	ULONG				m_nCustomOsdTextLineProperty[ 32 ];
	CHAR				m_pCustomOsdTextStringProperty[ 32 ][ 24 ][ 64 + 1 ];
	BOOLEAN				m_bCustomOsdTextStringRefreshProperty[ 32 ][ 24 ];
	ULONG				m_nCustomOsdTextColorProperty[ 32 ][ 24 ];
	ULONG				m_nCustomOsdTextFontSizeProperty[ 32 ][ 24 ];
	ULONG				m_nCustomOsdTextPositionXProperty[ 32 ][ 24 ];
	ULONG				m_nCustomOsdTextPositionYProperty[ 32 ][ 24 ];

	BOOLEAN					m_bCustomOsdPicturePathRefreshProperty[ 32 ][ 4 ];
	ULONG					m_nCustomOsdPictureParamsProperty[ 32 ][ 4 ][ 4 ];
	CHAR					m_pCustomOsdPicturePathProperty[ 32 ][ 4 ][ 64 + 1 ];
	ULONG					m_nCustomOsdPictureProperty[ 32 ];

	BOOLEAN					m_bCustomOsdRegionMaskRefreshProperty[ 32 ][ 2 ];
	ULONG					m_nCustomOsdRegionMaskParamsProperty[ 32 ][ 2 ][ 4 ];

	ULONG				  m_nCustomAnalogVideoInputEqProperty;
	ULONG				  m_nCustomAnalogVideoInputBandwidthProperty;
	ULONG				  m_nCustomSpecialCameraInputProperty;
	ULONG				  m_nCustomCompanyIvsProperty;
	ULONG				  m_nCustomAnalogVideoSdiSingalStatusProperty;
	ULONG				  m_nCustomAnalogVideoRGBColorConvertColorRangeProperty;
	ULONG				  m_nCustomAnalogVideoNativeColorSpaceProperty;
	ULONG				  m_nCustomAnalogVideoNativeColorSpaceAutoProperty;
	ULONG				  m_nCustomAnalogVideoNativeColorDeepProperty;
	ULONG				  m_nCustomAnalogVideoNativeColorDeepAutoProperty;
	ULONG				  m_nCustomAnalogVideoHDR2SDRProperty;
	ULONG				  m_nCustomAnalogVideoLoopThroughMuteProperty;


	wait_queue_head_t	  m_wait_queue_head;

	wait_queue_head_t     m_wait_queue_control;



	ULONG				  m_MZ0380_ready;
	struct semaphore      m_HWAccessSemaphore;
	ULONG		          m_nDummy5[8];

	struct semaphore      m_pControlSemaphore;
	ULONG		          m_nDummy6[8];

	struct semaphore       m_sRAWVideoFrame;
	ULONG		          m_nDummy7[8];

	spinlock_t			  m_sMultiCopySpinLock;
	spinlock_t			  m_sMultiIppSpinLock;

	ULONG				  m_combine_ready;

	ULONG 				  CFG0;
	ULONG 				  CFGB;
	ULONG 				  CFG2;
	ULONG				  idVendor;
	ULONG				  idProduct;
	ULONG				  bcdDevice;
	ULONG				  iManufacturer;
	ULONG				  iProduct;
	ULONG				  iSerialNumber;

	ULONG					m_nCustomEdidModeSelectProperty;


	ULONG				  m_nCustomMotionStatusProperty[ 32 ][ 8 ];
	ULONG				  m_nCustomMotionTemporalSensitivityProperty[ 32 ];
	ULONG				  m_nCustomMotionSpatialSensitivityProperty[ 32 ];
	ULONG				  m_nCustomMotionLevelSensitivityProperty[ 32 ];
	ULONG				  m_nCustomMotionSpeedProperty[ 32 ];
	ULONG				  m_nCustomSoftwareWatchdogResetProperty;

	ULONG				  m_nAnalogCopyProtMacrovisionEnableProperty;



	ULONG				 m_IRQ_ready;

	ULONG				  m_nCustomAnalogVideo720OutputProperty;
	ULONG		          m_nCustomEncoderVideoProfileProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoLevelProperty[ 32 ];

	ULONG		          m_nCustomPreviewVideoResolutionProperty[ 32 ];
	ULONG				  m_nCustomRegionMaskParamsProperty[ 32 ][ 2 ][ 4 ];
	ULONG		          m_nCustomAnalogVideoDeinterlaceTypeProperty[ 16 ];
	ULONG		          m_nCustomAnalogVideoDenoiseTypeProperty[ 16 ];
	ULONG		          m_nCustomAnalogVideoDemiseTypeProperty[ 16 ];
	ULONG				  m_nCustomAnalogVideoAutoPhaseTypeProperty[ 16 ];
	ULONG		          m_nCustomEncoderVideoEntropyProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoMaxQPProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoMinQPProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoAspectRatioProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoCropProperty[ 4 ][ 32 ];
	ULONG		          m_nCustomEncoderVideoAvgFrameRateProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoSkipFrameRateProperty[ 32 ];

	ULONG		          m_nCustomEncoderVideoPeakBitRateProperty[ 32 ];
	ULONG		          m_nCustomEncoderVideoTroughQualityProperty[ 32 ];

	ULONG				  m_nCustomAnalogVideoSkipFrameRateProperty[ 16 ];
	ULONG				  m_nCustomAnalogVideoAvgFrameRateProperty[ 32 ];

	ULONG				  m_nCustom_H264_Level_Property;

	ULONG				  m_nCustomSoftwareWatchdogEnableProperty;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0)

	struct v4l2_device		v4l2_dev;
#endif

	BOOLEAN				m_pControlThread_already_stop;

	BOOLEAN				m_pVideoLosslThread_already_stop;
	BOOLEAN				m_pAudioLosslThread_already_stop;

	ULONG				H264_main_frame_count;
	ULONG				audio_byte_count;

	ULONG				m_nCustomAnalogVideoVgaHActiveProperty;
	ULONG				m_nCustomAnalogVideoVgaAspectRatioProperty;
	ULONG				m_nCustomCompanyVision4CEProperty;
	ULONG				m_nCustomAnalogVideoColorRangeProperty;

	ULONG				m_MST3367_bank;

	ULONG				n_video_buffer_count;

	ULONG				m_No_Signal_yuv_pattern_property;

	ULONG				m_No_Signal_no_output_property;


	ULONG				m_buf_pos;
	ULONG				m_src_pos;
	BYTE				m_nTLV320ADC3101_PAGES[ 4 ];

	ULONG				m_nCustomAnalogVideoSplitProperty;

	ULONG				m_nCustomAnalogAudioVolumeProperty;
	ULONG				m_nCustomAnalogAudioPinTopologyProperty;
	ULONG		          m_nCustomAnalogAudioPinTopologyPropertys[16];
	ULONG		        m_nCustomAnalogAudioSampleFrequencyProperty;
	ULONG				m_nCustomAnalogAudioDirectSoundPatchProperty;
	ULONG				m_nCustomAnalogAudio8chI2sEnableProperty;

	ULONG				m_nCustomAnalogAudioChannelProperty;

	UINT				m_nKsDeviceAddress;

	struct Interrupt_Container	m_intrrupt_array[ 4 ];


	ULONGLONG					m_pl330b_stopfirmware_time;

	_GENNUM_INFO				m_GENNUM_INFO;
	WORD						m_USER_id;

	ULONG						m_i2c_chipaddr;
	ULONG						m_i2c_register;

	ULONG						m_nCustomAnalogVideoSubEncoderEnableProperty;

	ULONG						m_size_not_match_no_signal_blue_pattern;

	ULONG						m_Invert_UV;

	ULONG						m_nCustomAnalogVideoPinTopologyProperty;

	ULONG						m_nCustomAnalogVideoPinTopologyPropertyEx;

	ULONG						m_nCustomAnalogVideoMultiSyncChannels;
	ULONG						m_nCustomAnalogVideoMultiSyncPositionX;
	ULONG						m_nCustomAnalogVideoMultiSyncPositionY;
	ULONG						m_nCustomAnalogVideoMultiSyncPixelFormat;
	ULONG						m_nCustomAnalogVideoMultiSyncParentType;

	struct CDevice*				m_pLinkDevice;
	ULONG						m_nLinkNum;

	ULONG						m_nCustomAnalogVideoSeamlessSwitchProperty;

	const struct firmware		*fw_version;
	const struct firmware		*fw_firmware;
	const struct firmware		*fw_osd_recording;
	const struct firmware		*logo_no_signal;

	ULONGLONG					m_n_previous_signal_lock_status;
	callback					ptr_my_callback_video;




	ULONG						m_nAnalogCrossbarVideoInputAutoScanProperty;

	ULONG						m_nAnalogCrossbarVideoInputAutoScanTimeOutProperty;

	BYTE*						m_po[8];
	BYTE*						m_po_uv;




	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	u64							m_previous_main_h264_timestamp;
	u64							interrupt_ts;
	u64							interrupt_ts_audio;
	u64							interrupt_ts_audio_first_frame;
	#else
	struct timeval				m_previous_main_h264_timestamp;
	struct timeval				interrupt_ts;
	struct timeval				interrupt_ts_audio;
	struct timeval				interrupt_ts_audio_first_frame;
	#endif

	ULONGLONG					previous_ts[ 16 ];

	ULONGLONG					previous_ts_preview[16];
	ULONGLONG					previous_ts_h264;
	ULONGLONG					previous_ts_audio_int[ 16 ];

	ULONG						m_previous_audio_ring;

	ULONGLONG					previous_ts_surround_video_int;




	ULONG						MZ0380_firmware_statue;

	ULONG						m_nKsMasterNumber;
	ULONG						m_nKsPrimaryNumber;

	ULONG						m_nCustomCompanyMedicalProperty;
	ULONG						m_nCustomCompanyEndoCamProperty;
	ULONG						m_nCustomCompanyAlconProperty;
	ULONG						m_nCustomCompanyJojoroProperty;
	ULONG						m_nCustomCompanyMedxchangeProperty;
	ULONG						m_nCustomAnalogVideoSDILevelBSupportProperty;



	ULONG						m_nOutputOriginalDMAVideo;

	ULONG						m_StopControlPanelAnalysisThread;
	ULONG						m_RestartControlPanelAnalysisThread;

	ULONG						m_nFieldsPolarityHistory;

	LONG						m_time_zone;

	ULONG						m_time_zone_half_hour;

	ULONG						is_native_yuy2;
	ULONG						m_sVideoIntVector;
	ULONG						m_sMZ0380_audioInterruptVector;

	ULONG						m_dwDMAErrorFlags;
	volatile ULONG				  m_nCloneStreamPointerBaseCommonBufferLockNumber[ 8 ][ 32 ];

	ULONG						m_Video_status_ring;
	ULONG						m_Video_field;
	ULONG						m_Audio_status_ring;

	ULONG						m_sc0710_video_statusRC;
	ULONG						m_sc0710_audio_statusRC;

	dma_addr_t					m_desc_phy_addr_v;

	dma_addr_t					m_resu_phy_addr_v;

	dma_addr_t					m_desc_vrt_addr_v;

	dma_addr_t					m_desc_phy_addr_v_c;

	dma_addr_t					m_resu_phy_addr_v_c;

	dma_addr_t					m_desc_vrt_addr_v_c;


	ULONG						m_nCustomAnalogVideoMultiChannelSupportProperty;
	BYTE						m_SC0710_Pack_Multi_Channels_temp_buffer[ (4096+16) * 3 ];

	ULONG						m_nCustomCompanyMediaedgeProperty;

	ULONG						m_nBAR_block;
	ULONG						m_nBAR_offset;

	ULONG						m_nTemperature;

	ULONG						line_count[ 16 ];

	ULONG*						p_linecopy_cfg[ 16 ][MULTI_CH_RING];

	BYTE*						p_Storage[4][4];
	BYTE*						p_raw_v_buf;


	ULONG						m_nCounter_Fill_SG_Addr;
	ULONG						m_nDescriptor_Elements[MAX_VB];
	ULONG						m_nDescriptor_Elements_c[MAX_VB];

	ULONG						debug[ 16 ];

	int							init_video_node_number;
	int							init_audio_node_number;

	spinlock_t					m_sISR_SpinLock;
	spinlock_t					m_sDPC_Audio_SpinLock;
	spinlock_t					m_sDPC_Video_SpinLock;
	ULONG						m_nCustomDolbyProperty;
	ULONG						m_nCustomPowerSavingProperty;

	ULONG						m_nCustomSoftwareInterruptProperty;

	ULONGLONG					m_nVideoInterruptCount;
	ULONGLONG					m_nVideoH264Count;
	ULONGLONG					m_nAudioInterruptCount;
	BYTE						m_audio_arranged_buffer[ 4096 * 8 ];

	ULONG						m_InNoAudioDPC;


	ULONG						m_pMultiChVL_already_stop;

	ULONG						m_nCustomAnalogVideoMultiChannelMaskProperty;

	ULONG						m_nMulti_Instance;

	ULONG						m_nEnableDirect;

	ULONG						m_nUVCRunningStatus;
} CDevice;

#define BCD_TO_BINARY( counts ) (((counts) >> 4) * 10 + ((counts) & 0x0F))


#define TW6805_MAX_VIDEO_DMA_BUFFER_SIZE	(     3624 * 16)
#define TW6805_MAX_AUDIO_DMA_BUFFER_SIZE	(      24 * 128)
#define TW6805_MAX_XXVBI_DMA_BUFFER_SIZE	(             0)
#define TW6805_MAX_HX264_DMA_BUFFER_SIZE	(             0)
#define TW6805_MAX_MPEG4_DMA_BUFFER_SIZE	(             0)
#define TW6805_MAX_MJPEG_DMA_BUFFER_SIZE	(             0)

#define TW5864_MAX_VIDEO_DMA_BUFFER_SIZE	(	 0x00097E00)
#define TW5864_MAX_AUDIO_DMA_BUFFER_SIZE	(             0)
#define TW5864_MAX_XXVBI_DMA_BUFFER_SIZE	(             0)
#define TW5864_MAX_HX264_DMA_BUFFER_SIZE	(	1024 * 1024)
#define TW5864_MAX_MPEG4_DMA_BUFFER_SIZE	(			  0)
#define TW5864_MAX_MJPEG_DMA_BUFFER_SIZE	(             0)

#define MZ0380_HD_VIDEO_DMA_BUFFER_SIZE			(0x0034BD00)

#define MZ0380_MAX_VIDEO_DMA_BUFFER_SIZE		(0x0034BD00)


#define MZ0380_MAX_VIDEO_960H_DMA_BUFFER_SIZE	(0x000CA900)
#define MZ0380_MAX_VIDEO_D1_DMA_BUFFER_SIZE 	(0x000CA900)


#define MZ0380_MAX_AUDIO_DMA_BUFFER_SIZE		(      8192 * 4 * 8)
#define MZ0380_MAX_AUDIO_SD_DMA_BUFFER_SIZE		(	   8192 * 16)

#define MZ0380_HD_VIDEO_DMA_Y_BUFFER_SIZE		(0x00400000)
#define MZ0380_HD_VIDEO_DMA_UV_BUFFER_SIZE		(0x00270000)

#define PL330B_MAX_VIDEO_DMA_BUFFER_SIZE	 (            0)
#define PL330B_MAX_AUDIO_DMA_BUFFER_SIZE	 (            0)
#define PL330B_MAX_VBI_DMA_BUFFER_SIZE		 (            0)
#define PL330B_MAX_MPEG_DMA_BUFFER_SIZE		 (            0)

#define SC0700_MAX_AUDIO_DMA_BUFFER_SIZE	 (   (4096  * 4 ) + (2048  * 8  * 4 ) * (4 ) )

#define SC0710_MAX_DMA_BUFFER_QUANTITY_A_FRAME 29

#define DEFAULT_MST3367_VESA_SIZE	467

#define DEFAULT_MST3367_EIA_SIZE	 26

#define NTSTATUS		int

#define STATUS_SUCCESS							0x00000001
#define STATUS_UNSUCCESSFUL 					0x00000000
#define STATUS_INSUFFICIENT_RESOURCES			(0xc000009a)

#define SUPPORTED_ANALOG_VIDEO_STANDARDS_60HZ	V4L2_STD_525_60
#define SUPPORTED_ANALOG_VIDEO_STANDARDS_50HZ	V4L2_STD_625_50

#define MAX_PATH 260

BOOLEAN DELAY_100NS( ULONG DELAY );

typedef BOOLEAN BOOL;

void RtlCopyMemory( BYTE* pe, BYTE* po, int nFileSize );
void RtlFillMemory(BYTE* pe , int size, BYTE set_value);
void RtlZeroMemory(BYTE* pe , int size);

NTSTATUS OnGetCustomDeviceSerialNumberProperty( CDevice* pDevice, ULONG* pPropertyValue );

NTSTATUS OnGetCustomDebugMacAddressI2cDataProperty( CDevice* pDevice, ULONG address_sub, ULONG* pPropertyValue );
NTSTATUS OnSetCustomDebugMacAddressAccessEnableProperty( CDevice* pDevice );
NTSTATUS OnSetCustomDebugMacAddressI2cSubProperty( ULONG PropertyValue );
NTSTATUS OnSetCustomDebugMacAddressI2cDataProperty( BYTE PropertyValue );

int vivid_subscribe_event(struct v4l2_fh* fh, const struct v4l2_event_subscription* sub);



#define multichip_interrupt_time_gap 15000

#define MINIMUM_INTERVAL_CORRECT_INTERRUPT 8000

extern ULONG TABLE_DEVICE_INPUT_TOPOLOGY[  ][ 4 ];

enum Audio_LineIn_Topology {
         PHONE_JACK			= 0,
         PHONE_JACK_MIC		= 1,
         PCM_PHONE_JACK		= 2,
         PCM_PHONE_JACK_MIC	= 3,
         PIN				= 4,
         PIN_MIC			= 5,
         PCM_PIN			= 6,
         PCM_PIN_MIC		= 7,
};

#if defined(AME_MZ0380) || defined(AME_SC0700) || defined(AME_SC0710) || defined(AME_SC0720)
void common_pci_driver_on_tasklet_audio( unsigned long p_context );
#endif

#define	V4L2_PIX_FMT_P010   v4l2_fourcc('P', '0', '1', '0')
#define	V4L2_PIX_FMT_P016   v4l2_fourcc('P', '0', '1', '6')
#define	V4L2_PIX_FMT_P210   v4l2_fourcc('P', '2', '1', '0')
#define	V4L2_PIX_FMT_P216   v4l2_fourcc('P', '2', '1', '6')

#define	V4L2_PIX_FMT_PLANAR_BGR   v4l2_fourcc('R', 'G', 'B', 'P')
#define	V4L2_PIX_FMT_PLANAR_RGB   v4l2_fourcc('B', 'G', 'R', 'P')

#define MAX_I2C_TX_SIZE		16

#define STORAGE_SIZE		29368128

#define MAX_WIDTH  4096
#define MAX_HEIGHT 2160
#define MIN_WIDTH  16
#define MIN_HEIGHT 16

#define MZ0380_I2C_MCU_ADR7_0X64 0x64
#define MZ0380_I2C_MCU_ADR7_0X32 0x32

#define MULTI_IPP 2
#define MULTI_IPP_DONE 0x030003




#define LOGO_WIDTH 320
#define LOGO_HEIGHT 240

#endif





