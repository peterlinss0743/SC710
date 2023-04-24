#include "LINUXV4L2.h"
#include "property.h"
#include "sc0710.h"
#include "version.h"
#include "upaprom_example.h"

static int  __init       common_module_on_init( void );
static void __exit       common_module_on_exit( void );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)
#define __devinit        __section(.devinit.text) __cold notrace
#define __devexit        __section(.devexit.text) __exitused __cold notrace
#endif

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
	static int      common_pci_driver_on_probe( struct pci_dev * p_pci_dev, const struct pci_device_id * p_pci_device_id );
	static void     common_pci_driver_on_remove( struct pci_dev * p_pci_dev );
	#else
	static int  __devinit    common_pci_driver_on_probe( struct pci_dev * p_pci_dev, const struct pci_device_id * p_pci_device_id );
	static void __devexit    common_pci_driver_on_remove( struct pci_dev * p_pci_dev );
	#endif

static int               common_pci_driver_on_suspend( struct pci_dev * p_pci_dev, pm_message_t s_pm_message_state );
static int               common_pci_driver_on_resume( struct pci_dev * p_pci_dev );

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)
	static irqreturn_t       common_pci_driver_on_interrupt_2_6_19( int n_irq, void * p_context );
	#else
	static irqreturn_t       common_pci_driver_on_interrupt_2_6_18( int n_irq, void * p_context, struct pt_regs * p_regs );
	#endif

	#if defined(AME_MZ0380) || defined(AME_SC0700) || defined(AME_SC0710) || defined(AME_SC0720)
	static void				 common_pci_driver_on_tasklet_video( unsigned long p_context );
	#endif

static int               common_video_device_vidioc_querycap( struct file * p_file, void * p_priv, struct v4l2_capability * p_cap );
static int               common_video_device_vidioc_querystd( struct file * p_file, void * p_priv, v4l2_std_id * s );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
static int               common_video_device_vidioc_s_std( struct file * p_file, void * p_priv, v4l2_std_id  s );
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static int               common_video_device_vidioc_s_std( struct file * p_file, void * p_priv, v4l2_std_id * s );
#else
static int               common_video_device_vidioc_s_std( struct file * p_file, void * p_priv, v4l2_std_id  s );
#endif

static int               common_video_device_vidioc_g_std( struct file * p_file, void * p_priv, v4l2_std_id * s );
static int               common_video_device_vidioc_enum_input( struct file * p_file, void * p_priv, struct v4l2_input * p_input );
static int				 common_video_device_vidioc_enumaudio(struct file *p_file, void *priv, struct v4l2_audio *a);

static int               common_video_device_vidioc_s_input( struct file * p_file, void * p_priv, unsigned int i );
static int               common_video_device_vidioc_g_input( struct file * p_file, void * p_priv, unsigned int * i );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
static int				 common_video_device_vidioc_s_audio( struct file *file, void *fh, const struct v4l2_audio *a);
#else
static int				 common_video_device_vidioc_s_audio( struct file *file, void *fh, struct v4l2_audio *a );
#endif

static int				 common_video_device_vidioc_g_audio( struct file *p_file, void *fh, struct v4l2_audio *a);



static int               common_video_device_vidioc_s_parm( struct file * p_file, void * p_priv, struct v4l2_streamparm *sp );
static int               common_video_device_vidioc_g_parm( struct file * p_file, void * p_priv, struct v4l2_streamparm *sp );

static int				 common_video_device_vidioc_enum_frameintervals(struct file *p_file, void *fh, struct v4l2_frmivalenum *fival);
static int				 common_video_device_vidioc_enum_framesizes(struct file *p_file, void *fh, struct v4l2_frmsizeenum *fsize);

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
static int				 common_video_device_vidioc_cropcap(struct file *p_file, void *fh, struct v4l2_cropcap *cropcap);
#endif



static int               common_video_device_vidioc_enum_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_fmtdesc * p_fmtdesc );
static int               common_video_device_vidioc_try_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_format * p_fmt );
static int               common_video_device_vidioc_s_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_format * p_fmt );
static int               common_video_device_vidioc_g_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_format * p_fmt );
static int               common_video_device_vidioc_queryctrl( struct file * p_file, void * p_priv, struct v4l2_queryctrl * p_queryctrl );
static int				 common_video_device_vidioc_querymenu( struct file * p_file, void * p_priv, struct v4l2_querymenu *a);

static int               common_video_device_vidioc_s_ctrl( struct file * p_file, void * p_priv, struct v4l2_control * p_control );
static int               common_video_device_vidioc_g_ctrl( struct file * p_file, void * p_priv, struct v4l2_control * p_control );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)

static int               common_video_device_vidioc_s_ext_ctrls( struct file * p_file, void * p_priv, struct v4l2_ext_controls * p_ext_controls );
static int               common_video_device_vidioc_g_ext_ctrls( struct file * p_file, void * p_priv, struct v4l2_ext_controls * p_ext_controls );
static int				 common_video_device_vidioc_try_ext_ctrls( struct file *p_file, void * p_priv, struct v4l2_ext_controls * p_ext_controls);

#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
static int				vid_cap_queue_setup(struct vb2_queue *vq, unsigned int *nbuffers, unsigned int *nplanes,unsigned int sizes[], struct device *alloc_ctxs[]);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
static int				vid_cap_queue_setup(struct vb2_queue *vq, const void *parg, unsigned int *nbuffers, unsigned int *nplanes,unsigned int sizes[], void *alloc_ctxs[]);
#else
static int				vid_cap_queue_setup(struct vb2_queue *vq, const struct v4l2_format *fmt, unsigned int *nbuffers, unsigned int *nplanes,unsigned int sizes[], void *alloc_ctxs[]);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)
static void				vid_cap_stop_streaming(struct vb2_queue *vq);
#else
static int				vid_cap_stop_streaming(struct vb2_queue *vq);
#endif


#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
static void				vid_cap_buf_finish(struct vb2_buffer *vb);
static void				vid_cap_buf_cleanup(struct vb2_buffer *vb);
#else
static void				vivi_lock(struct vb2_queue *vq);
static void				vivi_unlock(struct vb2_queue *vq);
#endif



static int				vid_cap_buf_prepare(struct vb2_buffer *vb);
static void				vid_cap_buf_queue(struct vb2_buffer *vb);
static int				vid_cap_start_streaming(struct vb2_queue *vq, unsigned int count);


int						Initialize_pVideo( CVideo* pVideo, CDevice* pDevice, int i);

static int               common_snd_pcm_ops_open( struct snd_pcm_substream * p_substream );
static int               common_snd_pcm_ops_close( struct snd_pcm_substream * p_substream );
static int               common_snd_pcm_ops_hw_params( struct snd_pcm_substream * p_substream, struct snd_pcm_hw_params * p_params );
static int               common_snd_pcm_ops_hw_free( struct snd_pcm_substream * p_substream );
static int               common_snd_pcm_ops_prepare( struct snd_pcm_substream * p_substream );
static int               common_snd_pcm_ops_trigger( struct snd_pcm_substream * p_substream, int cmd );
static snd_pcm_uframes_t common_snd_pcm_ops_pointer( struct snd_pcm_substream * p_substream );
static int               common_snd_kcontrol_new_volume_info( struct snd_kcontrol * p_kcontrol, struct snd_ctl_elem_info * p_info );
static int               common_snd_kcontrol_new_volume_get( struct snd_kcontrol * p_kcontrol, struct snd_ctl_elem_value * p_value );
static int               common_snd_kcontrol_new_volume_put( struct snd_kcontrol * p_kcontrol, struct snd_ctl_elem_value * p_value );



static struct pci_device_id LINUXV4L2_PCI_DEVICE_ID_TABLE[ ] = {



#ifdef AME_SC0710
	{ 0x1797, 0x6801, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0x1797, 0x6802, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },

	{ 0x1797, 0x6804, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0x1797, 0x6805, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },

	{ 0x1797, 0x6810, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0x1797, 0x6811, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0x1797, 0x6812, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0x1797, 0x6813, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },

	{ 0x1797, 0x6814, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0x1797, 0x6815, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0x1797, 0x6816, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
	{ 0x1797, 0x6817, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },

	{ 0x1797, 0x5864, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },

	{ 0x12AB, 0x0710, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },

#endif

	{ 0x0000, }
};




#ifdef AME_SC0710
static struct pci_driver linuxv4l2_pci_driversc0710 = {

	.name     = "LINUXV4L2SC0710",
	.id_table =  LINUXV4L2_PCI_DEVICE_ID_TABLE,
	.probe    =  common_pci_driver_on_probe,
	.remove   =  common_pci_driver_on_remove,
	.suspend  =  common_pci_driver_on_suspend,
	.resume   =  common_pci_driver_on_resume,
};
#endif

static const struct v4l2_file_operations LINUXV4L2_FILE_OPERATIONS =
{
	.owner		= THIS_MODULE,
	.open           = v4l2_fh_open,
	.release        = vb2_fop_release,
	.read           = vb2_fop_read,
	.poll		= vb2_fop_poll,
	.unlocked_ioctl = video_ioctl2,
	.mmap           = vb2_fop_mmap,
};

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)

static const struct v4l2_ioctl_ops LINUXV4L2_V4L2_IOCTL_OPS =
{
	.vidioc_querycap         = common_video_device_vidioc_querycap,
	.vidioc_querystd         = common_video_device_vidioc_querystd,
	.vidioc_s_std            = common_video_device_vidioc_s_std,
	.vidioc_g_std            = common_video_device_vidioc_g_std,

	.vidioc_enum_input       = common_video_device_vidioc_enum_input,
	.vidioc_s_input          = common_video_device_vidioc_s_input,
	.vidioc_g_input          = common_video_device_vidioc_g_input,
	.vidioc_enum_fmt_vid_cap = common_video_device_vidioc_enum_fmt_cap,
	.vidioc_try_fmt_vid_cap  = common_video_device_vidioc_try_fmt_cap,
	.vidioc_s_fmt_vid_cap    = common_video_device_vidioc_s_fmt_cap,
	.vidioc_g_fmt_vid_cap    = common_video_device_vidioc_g_fmt_cap,
	.vidioc_queryctrl        = common_video_device_vidioc_queryctrl,
	.vidioc_querymenu		 = common_video_device_vidioc_querymenu,
	.vidioc_s_ctrl           = common_video_device_vidioc_s_ctrl,
	.vidioc_g_ctrl           = common_video_device_vidioc_g_ctrl,
	.vidioc_s_ext_ctrls		 = common_video_device_vidioc_s_ext_ctrls,
	.vidioc_g_ext_ctrls		 = common_video_device_vidioc_g_ext_ctrls,
	.vidioc_try_ext_ctrls	 = common_video_device_vidioc_try_ext_ctrls,
	.vidioc_reqbufs          = vb2_ioctl_reqbufs,
	.vidioc_create_bufs		 = vb2_ioctl_create_bufs,
	.vidioc_prepare_buf		 = vb2_ioctl_prepare_buf,
	.vidioc_querybuf         = vb2_ioctl_querybuf,
	.vidioc_qbuf             = vb2_ioctl_qbuf,
	.vidioc_dqbuf            = vb2_ioctl_dqbuf,

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)
	.vidioc_expbuf			 = vb2_ioctl_expbuf,
	#endif

	.vidioc_streamon         = vb2_ioctl_streamon,
	.vidioc_streamoff        = vb2_ioctl_streamoff,
	.vidioc_s_audio          = common_video_device_vidioc_s_audio,
	.vidioc_g_audio          = common_video_device_vidioc_g_audio,
	.vidioc_enumaudio		 = common_video_device_vidioc_enumaudio,

	.vidioc_s_parm          = common_video_device_vidioc_s_parm,
	.vidioc_g_parm          = common_video_device_vidioc_g_parm,
	.vidioc_enum_frameintervals = common_video_device_vidioc_enum_frameintervals,
	.vidioc_enum_framesizes	= common_video_device_vidioc_enum_framesizes,

	#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
	.vidioc_cropcap			= common_video_device_vidioc_cropcap,
	#endif

	.vidioc_subscribe_event = vivid_subscribe_event,
};
#endif

#if (LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18)) || (LINUX_VERSION_CODE == KERNEL_VERSION(2,6,19))

static struct v4l2_tvnorm LINUXV4L2_V4L2_TVNORM[ 2 ] =
{
	{	.name = "V4L2_STD_525_60",
		.id   = V4L2_STD_525_60,
	},
	{	.name = "V4L2_STD_625_50",
		.id   = V4L2_STD_625_50,
	},
};

#endif

static const struct vb2_ops vivi_video_qops = {
	.queue_setup		= vid_cap_queue_setup,
	.buf_prepare		= vid_cap_buf_prepare,
	.buf_queue			= vid_cap_buf_queue,
	.start_streaming	= vid_cap_start_streaming,
	.stop_streaming		= vid_cap_stop_streaming,
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
	.buf_finish			= vid_cap_buf_finish,
	.buf_cleanup			= vid_cap_buf_cleanup,
	.wait_prepare		= vb2_ops_wait_prepare,
	.wait_finish		= vb2_ops_wait_finish,
	#else
	.wait_prepare		= vivi_unlock,
	.wait_finish		= vivi_lock,
	#endif
};

static struct snd_kcontrol_new LINUXV4L2_SND_KCONTROL_NEW_VOLUME = {

	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.access = SNDRV_CTL_ELEM_ACCESS_READWRITE | SNDRV_CTL_ELEM_ACCESS_TLV_READ,
	.name  = "VOLUME",
	.info  = common_snd_kcontrol_new_volume_info,
	.get   = common_snd_kcontrol_new_volume_get,
	.put   = common_snd_kcontrol_new_volume_put,
};

static struct snd_pcm_ops LINUXV4L2_SND_PCM_OPS =
{
	.open      = common_snd_pcm_ops_open,
	.close     = common_snd_pcm_ops_close,
	.ioctl     = snd_pcm_lib_ioctl,
	.hw_params = common_snd_pcm_ops_hw_params,
	.hw_free   = common_snd_pcm_ops_hw_free,
	.prepare   = common_snd_pcm_ops_prepare,
	.trigger   = common_snd_pcm_ops_trigger,
	.pointer   = common_snd_pcm_ops_pointer,


};



struct snd_pcm_hardware LINUXV4L2_SND_PCM_HARDWARE_SC0710 =
{
	.info             = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_MMAP_VALID | SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER,
	.formats          = SNDRV_PCM_FMTBIT_S16_LE,
	.rates            = SNDRV_PCM_RATE_48000,
	.rate_min         = 32000,
	.rate_max         = 48000,

	.channels_min     = 2,
	.channels_max     = 8,
	.buffer_bytes_max = INT_BYTES_A * 4 * 64,
	.period_bytes_min = INT_BYTES_A,
	.period_bytes_max = INT_BYTES_A,
	.periods_max      = 64,
	.periods_min      = 64,
};


ULONG g_n_bus_number[ 32 ] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

static ULONG LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ 26 ][ 22 ] =
{
	{ 0x1797, 0x6801, 128 , 100 , 128 , 128 ,  16 , 1 , 120 , 2 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1797, 0x6802, 128 , 100 , 128 , 128 ,  16 , 1 , 120 , 2 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1797, 0x6804, 128 , 100 , 128 , 128 ,  16 , 1 , 120 , 2 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1797, 0x6805, 128 , 100 , 128 , 128 ,  16 , 1 , 120 , 2 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1797, 0x6810, 128 , 100 , 128 , 128 ,  16 , 1 , 120 , 2 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1797, 0x6811, 128 , 100 , 128 , 128 ,  16 , 1 , 120 , 2 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1797, 0x6812, 128 , 100 , 128 , 128 ,  16 , 1 , 120 , 2 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1797, 0x6813, 128 , 100 , 128 , 128 ,  16 , 1 , 120 , 2 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1797, 0x6814, 128 , 100 , 128 , 128 ,  16 , 1 , 120 , 2 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1797, 0x6815, 128 , 100 , 128 , 128 ,  16 , 1 , 120 , 2 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1797, 0x6816, 128 , 100 , 128 , 128 ,  16 , 1 , 120 , 2 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1797, 0x6817, 128 , 100 , 128 , 128 ,  16 , 1 , 120 , 2 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x14F1, 0x8210, 128 , 128 , 128 , 128 , 128 , 1 , 256 , 0 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x14F1, 0x5851, 128 , 128 , 128 , 128 , 128 , 1 , 256 , 0 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1131, 0x7160, 128 , 128 , 128 , 128 , 128 , 1 , 256 , 0 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1999, 0xA04C, 128 , 128 , 128 , 128 ,  16 , 1 , 256 , 0 , 0x00000000 , 0x00000000 , 0x00000003 , 0x00000001 , 0x3FFFFFFF , 0x00FFFFFF , 0x00000008 , 0x00000008 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x3086, 0x0101, 128 , 128 , 128 , 128 , 128 , 1 , 256 , 0 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1797, 0x5864, 128 , 128 , 128 , 128 , 128 , 1 , 256 , 0 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x0907, 0x0100, 128 , 128 , 128 , 128 , 128 , 1 , 256 , 0 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x12AB, 0x0380, 128 , 128 , 128 , 128 , 128 , 1 , 256 , 0 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x1AF2, 0xA001, 128 , 128 , 128 , 128 , 128 , 1 , 256 , 0 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x12AB, 0x0700, 128 , 128 , 128 , 128 , 128 , 1 , 256 , 0 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x12AB, 0x0701, 128 , 128 , 128 , 128 , 128 , 1 , 256 , 0 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x12AB, 0x0710, 128 , 128 , 128 , 128 , 128 , 1 , 256 , 0 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x12AB, 0x0720, 128 , 128 , 128 , 128 , 128 , 1 , 256 , 0 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
	{ 0x12AB, 0x0750, 128 , 128 , 128 , 128 , 128 , 1 , 256 , 0 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000 , 0x00000000  },
};

static ULONG LINUXV4L2_V4L2_QUERYCTRL_ENABLE[ 26 ][ 22 ] =
{
	{ 0x1797, 0x6801, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1797, 0x6802, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1797, 0x6804, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1797, 0x6805, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1797, 0x6810, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1797, 0x6811, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1797, 0x6812, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1797, 0x6813, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1797, 0x6814, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1797, 0x6815, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1797, 0x6816, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1797, 0x6817, 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x14F1, 0x8210, 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x14F1, 0x5851, 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1131, 0x7160, 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1999, 0xA04C, 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1  },
	{ 0x3086, 0x0101, 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1797, 0x5864, 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x0907, 0x0100, 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x12AB, 0x0380, 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x1AF2, 0xA001, 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x12AB, 0x0700, 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x12AB, 0x0701, 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x12AB, 0x0710, 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x12AB, 0x0720, 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
	{ 0x12AB, 0x0750, 1 , 1 , 1 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1  },
};



static struct v4l2_queryctrl LINUXV4L2_V4L2_QUERYCTRL[ ] =
{
	{	.id            =  V4L2_CID_BRIGHTNESS,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "BRIGHTNESS",
		.minimum       =  0,
		.maximum       =  255,
		.step          =  1,
		.default_value =  128,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_CONTRAST,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "CONTRAST",
		.minimum       =  0,
		.maximum       =  255,
		.step          =  1,
		.default_value =  128,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_SATURATION,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "SATURATION",
		.minimum       =  0,
		.maximum       =  255,
		.step          =  1,
		.default_value =  128,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_HUE,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "HUE",
		.minimum       =  0,
		.maximum       =  255,
		.step          =  1,
		.default_value =  128,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_SHARPNESS,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "SHARPNESS",
		.minimum       =  0,
		.maximum       =  255,
		.step          =  1,
		.default_value =  128,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_AUTOGAIN,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "AUTO GAIN",
		.minimum       =  0,
		.maximum       =  1,
		.step          =  1,
		.default_value =  1,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_GAIN,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "GAIN",
		.minimum       =  0,
		.maximum       =  511,
		.step          =  1,
		.default_value =  256,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_SWITCH_SPEED,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "SWITCH SPEED",
		.minimum       =  0,
		.maximum       =  2,
		.step          =  1,
		.default_value =  1,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_SWITCH_TABLE,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "SWITCH TABLE",
		.minimum       =  0x00000000,
		.maximum       =  0x00FFFFFF,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_MOTION_STATUS,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "MOTION STATUS",
		.minimum       =  0,
		.maximum       =  1,
		.step          =  1,
		.default_value =  0,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_MOTION_SENSITIVITY_LOW_THRESHOLD,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "MOTION SENSITIVITY (TH.L)",
		.minimum       =  0,
		.maximum       =  127,
		.step          =  1,
		.default_value =  64,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_MOTION_SENSITIVITY_HIGH_THRESHOLD,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "MOTION SENSITIVITY (TH.H)",
		.minimum       =  0,
		.maximum       =  127,
		.step          =  1,
		.default_value =  64,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_MOTION_MASK_CX,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "MOTION MASK (CX)",
		.minimum       =  0x00000000,
		.maximum       =  0x3FFFFFFF,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_MOTION_MASK_CY,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "MOTION MASK (CY)",
		.minimum       =  0x00000000,
		.maximum       =  0x00FFFFFF,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_OSD_POS_X,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "OSD POS (X)",
		.minimum       =  0,
		.maximum       =  255,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_OSD_POS_Y,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "OSD POS (Y)",
		.minimum       =  0,
		.maximum       =  255,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_OSD_COLOR,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "OSD COLOR",
		.minimum       =  0,
		.maximum       =  15,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_OSD_TEXT,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "OSD TEXT",
		.minimum       =  0x00000000,
		.maximum       =  0x000000FF,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_GPIO_DIRECTION,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "GPIO DIRECTION",
		.minimum       =  0x00000000,
		.maximum       =  0xFFFFFFFF,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
	{	.id            =  V4L2_CID_GPIO_DATA,
		.type          =  V4L2_CTRL_TYPE_INTEGER,
		.name          = "GPIO DATA",
		.minimum       =  0x00000000,
		.maximum       =  0xFFFFFFFF,
		.step          =  1,
		.default_value =  0x00000000,
		.flags         =  0,
	},
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)

extern struct video_device * video_devdata( struct file * p_file );

static inline void * video_get_drvdata( struct video_device * p_dev ) { return p_dev->priv; }

static inline void video_set_drvdata( struct video_device * p_dev, void * p_data ) { p_dev->priv = p_data; }

#endif



#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)

#define init_MUTEX(sem)         sema_init(sem, 1)
#define init_MUTEX_LOCKED(sem)  sema_init(sem, 0)

#endif

UINT g_n_analog_decoder_brightness                    [ 64 ][ 4 ] = { { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 } };

UINT g_n_analog_decoder_contrast                      [ 64 ][ 4 ] = { { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 } };

UINT g_n_analog_decoder_saturation                    [ 64 ][ 4 ] = { { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 } };

UINT g_n_analog_decoder_hue                           [ 64 ][ 4 ] = { { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 } };

UINT g_n_analog_decoder_sharpness                     [ 64 ][ 4 ] = { { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 } };

UINT g_n_analog_decoder_white_balance[64][4] = { { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 }, { 128, 128, 128, 128 } };

UINT g_n_analog_decoder_gain_auto                     [ 64 ] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

UINT g_n_analog_decoder_gain                          [ 64 ] = { 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256 };



UINT g_n_analog_decoder_lock_status                   [ 64 ] = { 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102 };


UINT g_n_multiplexer_motion_status                    [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

UINT g_n_multiplexer_motion_sensitivity_low_threshold [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

UINT g_n_multiplexer_motion_sensitivity_high_threshold[ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

UINT g_n_multiplexer_motion_mask_cx                   [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

UINT g_n_multiplexer_motion_mask_cy                   [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


UINT g_n_encoder_osd_pos_x                            [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

UINT g_n_encoder_osd_pos_y                            [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

UINT g_n_encoder_osd_color                            [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

CHAR  g_p_encoder_osd_text                             [ 64 ][ 64 + 1 ] = { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" };


UINT g_n_gpio_direction                               [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

UINT g_n_gpio_data                                    [ 64 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

CDevice * g_pDevice[ 128 ] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

							   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

							   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

							   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

							   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

							   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

							   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

							   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

#if defined(AME_SC0710) || defined(AME_SC0720)

static ULONG g_n_mac_address_sub = 0x00;

static BYTE  g_b_mac_address_data[ 256 ];

#endif



BOOLEAN DELAY_100NS( ULONG DELAY )
{

#if defined(AME_MZ0380) || defined(AME_QP0203) || defined(AME_QP0204) || defined(AME_SC0700) || defined(AME_SC0710) || defined(AME_SC0720)

	if( DELAY > 10000)
	{
		schedule_timeout_interruptible( msecs_to_jiffies( DELAY/10000 ) );
	}
	else
	{
		schedule_timeout_interruptible( msecs_to_jiffies( 1 ) );
	}

#else

	udelay( DELAY / 10 );

#endif

	return TRUE;
}
void RtlFillMemory(BYTE* pe , int size, BYTE set_value)
{
	memset(pe, set_value, size);
}

void RtlZeroMemory(BYTE* pe , int size)
{
	memset(pe, 0x00, size);
}

void RtlCopyMemory( BYTE* pe, BYTE* po, int nFileSize )
{
	memcpy(pe, po, nFileSize);
}




static int Init_pci_driver( void * p_context )
{
	int ret = 0;


	ret = pci_register_driver( p_context );

	if( ret != 0 ) {

		LINUXV4L2_DEBUG( KERN_ERR, "Init_pci_driver() - pci_register_driver() - [FAIL]\n" );

		return ret;
	}
	return 0;
}

spinlock_t             m_sSurroundSpinLock[4];

spinlock_t             m_s2CHSpinLock[4];

spinlock_t             m_sPciConfigSpinLock;

struct semaphore       m_sComponentAccessSemaphore;



ULONG TABLE_DEVICE_INPUT_TOPOLOGY[  ][ 4 ] = {


	{ 0x12AB0710, 0xFFFF0FFF, 0x0000014C, 0x00000001 },

	{ 0x12AB0711, 0xFFFF0FFF, 0x0000010C, 0x00000001 },

	{ 0x12AB0712, 0xFFFF0FFF, 0x00000004, 0x00000001 },

	{ 0x12AB0713, 0xFFFF0FFF, 0x00000044, 0x00000001 },

	{ 0x12AB0715, 0xFFFF0FFF, 0x00000004, 0x00000001 },

	{ 0x12AB0716, 0xFFFF0FFF, 0x00000040, 0x00000001 },

	{ 0x12AB0717, 0xFFFF0FFF, 0x00000004, 0x00000001 },


	{ 0x12AB0718, 0xFFFF0FFF, 0x00000040, 0x00000001 },


	{ 0x12AB0719, 0xFFFF0FFF, 0x00000004, 0x00000001 },

	{ 0x12AB071A, 0xFFFF0FFF, 0x00000040, 0x00000001 },

	{ 0x12AB071B, 0xFFFF0FFF, 0x00000040, 0x00000001 },

	{ 0x12AB071C, 0xFFFF0FFF, 0x00000004, 0x00000001 },

	{ 0x12AB071D, 0xFFFF0FFF, 0x00000004, 0x00000001 },

	{ 0x12AB071E, 0xFFFF0FFF, 0x00000044, 0x00000001 },

	{ 0x12AB071F, 0xFFFF0FFF, 0x0000007F, 0x00000001 },

	{ 0x12AB0723, 0xFFFF0FFF, 0x00000100, 0x00000001 },

	{ 0x12AB0729, 0xFFFF0FFF, 0x00000004, 0x00000001 },

	{ 0x12AB072B, 0xFFFF0FFF, 0x00000040, 0x00000001 },

	{ 0x12AB072D, 0xFFFF0FFF, 0x00000004, 0x00000001 },

	{ 0x000E1CFA, 0xFFFFFFFF, 0x00000004, 0x00000001 },

	{ 0x000F1CFA, 0xFFFFFFFF, 0x00000004, 0x00000001 },

	{ 0x00111CFA, 0xFFFFFFFF, 0x00000004, 0x00000001 },

	{ 0xD07C10FC, 0xFFFFFFFF, 0x00000004, 0x00000001 },

};

const char colorbar[8][3] = { {235, 128, 128},
								{210, 16, 146},
								{170, 166, 16},
								{145, 54, 34},
								{107, 202, 221},
								{82, 90, 240},
								{41, 240, 110},
								{16, 128, 128}
							  };

ULONG SC0710_DEF_MULTI_CHDMA_PAGES_SIZE = 1390000;

ULONG MULTI_CH_ROW_SIZE = 0x400000;

ULONG SC0710_DEF_MULTI_CHDMA_PAGES = 3;

ULONG SC0710_DEF_MULTI_CHDMA_PAGES_SIZE_4M_RESIDUE = 24304;

static uint multichannel = 0x00000000;

static uint dolby = 0x00000000;

static uint powersaving = 0x00000000;

static uint softwareint = 0x00000000;

static uint autoscan = 0x00000000;

static uint autophase = 0x00000000;

static uint multiinstance = 0x00000000;

module_param( multichannel, uint, S_IRUGO );

module_param( powersaving, uint, S_IRUGO );

module_param( softwareint, uint, S_IRUGO );

module_param( autoscan, uint, S_IRUGO );

module_param( autophase, uint, S_IRUGO );

module_param( multiinstance, uint, S_IRUGO );



#define DRIVER_VERSION 1183

static int __init common_module_on_init( void )
{
	int ret = -EINVAL;

	struct task_struct* init_thread = NULL;

	LINUXV4L2_PRINT( KERN_INFO, "common_module_on_init(), 1.1.0.120.%d, 2023.04.18 PAL\n", DRIVER_VERSION );

#ifdef CONFIG_ARM64
	LINUXV4L2_PRINT( KERN_INFO, "CONFIG_ARM64\n" );
#endif

#ifdef CONFIG_DMA_CMA
	LINUXV4L2_PRINT(KERN_INFO, "CONFIG_DMA_CMA\n");
	LINUXV4L2_PRINT(KERN_INFO, "add cma=64M@0-4G in booting parameter\n");
#endif

#ifdef CONFIG_PCI_MMCONFIG
	LINUXV4L2_PRINT( KERN_INFO, "CONFIG_PCI_MMCONFIG\n" );
#endif



	SC0710_DEF_MULTI_CHDMA_PAGES_SIZE = 1390000;

	MULTI_CH_ROW_SIZE = 0x400000;

	SC0710_DEF_MULTI_CHDMA_PAGES = (MULTI_CH_ROW_SIZE / SC0710_DEF_MULTI_CHDMA_PAGES_SIZE);

	SC0710_DEF_MULTI_CHDMA_PAGES_SIZE_4M_RESIDUE = (MULTI_CH_ROW_SIZE - (SC0710_DEF_MULTI_CHDMA_PAGES * SC0710_DEF_MULTI_CHDMA_PAGES_SIZE));

	LINUXV4L2_PRINT(KERN_INFO, "SC0710_DEF_MULTI_CHDMA_PAGES_SIZE(0d%d)\n", SC0710_DEF_MULTI_CHDMA_PAGES_SIZE);
	LINUXV4L2_PRINT(KERN_INFO, "MULTI_CH_ROW_SIZE(0d%d)\n", MULTI_CH_ROW_SIZE);
	LINUXV4L2_PRINT(KERN_INFO, "SC0710_DEF_MULTI_CHDMA_PAGES(0d%d)\n", SC0710_DEF_MULTI_CHDMA_PAGES);
	LINUXV4L2_PRINT(KERN_INFO, "SC0710_DEF_MULTI_CHDMA_PAGES_SIZE_4M_RESIDUE(0d%d)\n", SC0710_DEF_MULTI_CHDMA_PAGES_SIZE_4M_RESIDUE);


#ifdef WEBRTC
	LINUXV4L2_PRINT( KERN_INFO, "WEBRTC\n" );
#endif

#ifdef ENABLE_8CHS
	LINUXV4L2_PRINT( KERN_INFO, "ENABLE_8CHS\n" );
#endif

#ifdef USER_POINTER_IO
	LINUXV4L2_PRINT( KERN_INFO, "USER_POINTER_IO, only support original video data format\n" );
#endif


#ifdef WOLF
	LINUXV4L2_PRINT( KERN_INFO, "WOLF\n" );
#endif

#ifdef INTERLACE_FIELD_MODE
	LINUXV4L2_PRINT( KERN_INFO, "INTERLACE_FIELD_MODE\n" );
#endif

#ifdef OUTPUT_IDEAL_VIDEO_TIME_STAMP
	LINUXV4L2_PRINT( KERN_INFO, "OUTPUT_IDEAL_VIDEO_TIME_STAMP\n" );
#endif

#ifdef OUTPUT_REAL_VIDEO_TIME_STAMP
	LINUXV4L2_PRINT( KERN_INFO, "OUTPUT_REAL_VIDEO_TIME_STAMP\n" );
#endif



#ifdef USE_MSI
	LINUXV4L2_PRINT( KERN_INFO, "USE_MSI\n" );
#endif

#ifdef CONFIG_X86_32
	#pragma message("THIS OS IS 32 BIT")
#endif


#ifdef RECLOCK_DISABLED
	LINUXV4L2_PRINT( KERN_INFO, "RECLOCK_DISABLED\n" );
#endif


#ifdef FAST_DETECT
	LINUXV4L2_PRINT(KERN_INFO, "FAST_DETECT\n");
#endif

#ifdef NO_SIGNAL_NO_OUTPUT
	LINUXV4L2_PRINT(KERN_INFO, "NO_SIGNAL_NO_OUTPUT\n");
#endif

#ifdef BG_UPDATE
	LINUXV4L2_PRINT(KERN_INFO, "BG_UPDATE\n");
#endif



	LINUXV4L2_PRINT( KERN_INFO, "multichannel(0x%x)\n", multichannel );

	LINUXV4L2_PRINT( KERN_INFO, "powersaving(0x%x)\n", powersaving );

	LINUXV4L2_PRINT( KERN_INFO, "softwareint(0x%x)\n", softwareint );

	LINUXV4L2_PRINT( KERN_INFO, "autoscan(0x%x)\n", autoscan );

	LINUXV4L2_PRINT( KERN_INFO, "autophase(0x%x)\n", autophase );

	LINUXV4L2_PRINT( KERN_INFO, "multiinstance(0x%x)\n", multiinstance );

	LINUXV4L2_DEBUG( KERN_INFO, "sizeof(unsigned long)(0x%x)\n", sizeof(unsigned long) );
	LINUXV4L2_DEBUG( KERN_INFO, "sizeof(unsigned long long)(0x%x)\n", sizeof(unsigned long long) );



	spin_lock_init( &m_sSurroundSpinLock[0] );
	spin_lock_init( &m_sSurroundSpinLock[1] );
	spin_lock_init( &m_sSurroundSpinLock[2] );
	spin_lock_init( &m_sSurroundSpinLock[3] );

	spin_lock_init( &m_s2CHSpinLock[0] );
	spin_lock_init( &m_s2CHSpinLock[1] );
	spin_lock_init( &m_s2CHSpinLock[2] );
	spin_lock_init( &m_s2CHSpinLock[3] );

	spin_lock_init( &m_sPciConfigSpinLock );

	init_MUTEX( &m_sComponentAccessSemaphore );



#ifdef AME_SC0710

	init_thread = kthread_run( Init_pci_driver, &linuxv4l2_pci_driversc0710, "Init_pci_driver" );

#endif


	if( IS_ERR( init_thread ) ) {

		LINUXV4L2_PRINT( KERN_ERR, "common_module_on_init() - kthread_run() - [FAIL]\n" );

		return ret;
	}
	return 0;
}

static void __exit common_module_on_exit( void )
{
	LINUXV4L2_DEBUG( KERN_INFO, "common_module_on_exit()\n" );



#ifdef AME_SC0710
	pci_unregister_driver( &linuxv4l2_pci_driversc0710 );
#endif

}



static ULONG g_n_counter_of_board_number         = 0x00000000;
static ULONG g_n_counter_of_video_number         = 0x00000000;
static ULONG g_n_counter_of_audio_number         = 0x00000000;
static ULONG g_n_counter_of_video_encoder_number = 0x00000000;
static ULONG g_n_counter_of_audio_encoder_number = 0x00000000;


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
static int common_pci_driver_on_probe( struct pci_dev * p_pci_dev, const struct pci_device_id * p_pci_device_id )
#else
static int __devinit common_pci_driver_on_probe(struct pci_dev* p_pci_dev, const struct pci_device_id* p_pci_device_id)
#endif
{
	CDevice* pDevice = NULL;

	CHAR psz[256];

	int ret = 0;

	int i = 0;

	USHORT w_vender_id = 0x0000;

	USHORT w_device_id = 0x0000;

	USHORT w_subsystem_vender_id = 0x0000;

	USHORT w_subsystem_id = 0x0000;

	down(&m_sComponentAccessSemaphore);

	pci_read_config_word(p_pci_dev, 0x00, &w_vender_id);

	pci_read_config_word(p_pci_dev, 0x02, &w_device_id);

	pci_read_config_word(p_pci_dev, 0x2C, &w_subsystem_vender_id);

	pci_read_config_word(p_pci_dev, 0x2E, &w_subsystem_id);

	LINUXV4L2_PRINT(KERN_INFO, "[%02d] common_pci_driver_on_probe( %02X:%06X:%04X:%04X:%04X:%04X (%04X:%04X:%04X:%04X) )\n", (int)(g_n_counter_of_board_number), p_pci_dev->bus->number, p_pci_dev->class, p_pci_dev->vendor, p_pci_dev->device, p_pci_dev->subsystem_vendor, p_pci_dev->subsystem_device, w_vender_id, w_device_id, w_subsystem_vender_id, w_subsystem_id);


	LINUXV4L2_PRINT(KERN_INFO, "[%02d] common_pci_driver_on_probe() bus(0x%x)\n", (int)(g_n_counter_of_board_number), p_pci_dev->bus);

	LINUXV4L2_PRINT(KERN_INFO, "[%02d] common_pci_driver_on_probe() devfn(0x%x)\n", (int)(g_n_counter_of_board_number), p_pci_dev->devfn);

	LINUXV4L2_PRINT(KERN_INFO, "[%02d] common_pci_driver_on_probe() slot(0x%x)\n", (int)(g_n_counter_of_board_number), PCI_SLOT(p_pci_dev->devfn));

	LINUXV4L2_PRINT(KERN_INFO, "[%02d] common_pci_driver_on_probe() fn(0x%x) ??\n", (int)(g_n_counter_of_board_number), PCI_FUNC(p_pci_dev->devfn));


	LINUXV4L2_PRINT(KERN_INFO, "[%02d] common_pci_driver_on_probe() primary(0x%x)\n", (int)(g_n_counter_of_board_number), p_pci_dev->bus->primary);



	LINUXV4L2_PRINT(KERN_INFO, "[%02d] common_pci_driver_on_probe() self->vendor(0x%x) self->device(0x%x) \n", (int)(g_n_counter_of_board_number), p_pci_dev->bus->self->vendor, p_pci_dev->bus->self->device);

	LINUXV4L2_PRINT(KERN_INFO, "[%02d] common_pci_driver_on_probe() pm_cap(0x%x)\n", (int)(g_n_counter_of_board_number), p_pci_dev->pm_cap);

	LINUXV4L2_PRINT(KERN_INFO, "[%02d] common_pci_driver_on_probe() current_state(0x%x)\n", (int)(g_n_counter_of_board_number), p_pci_dev->current_state);

	pDevice = (CDevice*)(kmalloc(sizeof(struct CDevice), GFP_KERNEL));

	if (pDevice == NULL) {

		LINUXV4L2_DEBUG(KERN_ERR, "[%02d] common_pci_driver_on_probe() - kmalloc() size(0d%d)- [FAIL]\n", g_n_counter_of_board_number, sizeof(struct CDevice));

		ret = -ENOMEM;

		goto common_pci_driver_on_probe_fail;
	}
	else {

		pci_set_drvdata(p_pci_dev, (PVOID)(pDevice));

		memset(pDevice, 0, sizeof(struct CDevice));

		pci_read_config_dword(p_pci_dev, 0x10, &pDevice->m_nKsDeviceAddress);

		LINUXV4L2_PRINT(KERN_INFO, "[%02d] common_pci_driver_on_probe() - w_bar0(0x%x)\n", (int)(g_n_counter_of_board_number), pDevice->m_nKsDeviceAddress);


		spin_lock_init(&(pDevice->m_sISR_SpinLock));

		spin_lock_init(&(pDevice->m_sDPC_Video_SpinLock));

		spin_lock_init(&(pDevice->m_sDPC_Audio_SpinLock));



		init_MUTEX(&(pDevice->m_HWAccessSemaphore));

		init_MUTEX(&(pDevice->m_pControlSemaphore));

		init_MUTEX(&(pDevice->m_sRAWVideoFrame));

		spin_lock_init(&(pDevice->m_sMultiCopySpinLock));
		spin_lock_init(&(pDevice->m_sMultiIppSpinLock));



		pDevice->m_pKsDevice = p_pci_dev;

		pDevice->m_nKsDeviceBusNumber = p_pci_dev->bus->number;

		pDevice->m_nKsPrimaryNumber = p_pci_dev->bus->primary;


		pDevice->m_nKsDeviceNumber = g_n_counter_of_board_number;

		pDevice->m_nKsDeviceVideoNumber = 0x00000000;

		pDevice->m_nKsDeviceAudioNumber = 0x00000000;

		pDevice->m_nKsDeviceVideoEncoderNumber = 0x00000000;

		pDevice->m_nKsDeviceAudioEncoderNumber = 0x00000000;

		pDevice->m_nTaskletExtraParameterA = 0x00000000;

		pDevice->m_nTaskletExtraParameterB = 0x00000000;

		pDevice->m_nTaskletExtraParameterC = 0x00000000;

		pDevice->m_sMZ0380_audioInterruptVector = 0;

		pDevice->m_hInterruptAccessLock = FALSE;

		pDevice->m_nAnalogCaptureStreamPowerReference = 0x00000000;

		pDevice->m_nVideoCaptureStreamPowerReference = 0x00000000;

		pDevice->m_nAudioCaptureStreamPowerReference = 0x00000000;

		pDevice->m_nCustomGpioDirectionProperty = 0;

		pDevice->m_nCustomGpioDataProperty = 0;

		pDevice->m_nCustomGpioSupportProperty = 0;

		pDevice->m_nCustomSoftwareWatchdogDurationProperty = 180;

		pDevice->m_nCustomAnalogVideoScaleOutputProperty = 1;

		pDevice->m_nAnalogCopyProtMacrovisionProperty = 0;

		pDevice->m_nAnalogVideoDecoderStatusProperty = 0;

		pDevice->init_video_node_number = 0;
		pDevice->init_audio_node_number = 0;

		pDevice->m_nAnalogCrossbarVideoInputProperty = 4;

		if (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0710)
		{
			pDevice->m_nAnalogCrossbarVideoInputProperty = 0;
		}

		LINUXV4L2_PRINT(KERN_INFO, "m_nAnalogCrossbarVideoInputProperty(0d%d)\n", pDevice->m_nAnalogCrossbarVideoInputProperty);

		pDevice->m_nAnalogCrossbarAudioInputProperty = 0;

		pDevice->m_nAnalogCrossbarVideoInputAutoScanProperty = autoscan;

		pDevice->m_nAnalogCrossbarVideoInputAutoScanTimeOutProperty = 3000;

		pDevice->m_nCustomAnalogVideoAutoPhaseTypeProperty[0] = autophase;

		pDevice->m_pCustomSystemConfigProperty = NULL;


		pDevice->m_nCustomAnalogVideoFrameRateProperty = 0;

		pDevice->m_nCustomAnalogVideoInterleavedProperty = 1;

		pDevice->m_nCustomSoftwareWatchdogResetProperty = 0;

		pDevice->m_nAnalogCopyProtMacrovisionEnableProperty = 0;

		pDevice->m_nCustomAnalogVideoVgaHActiveProperty = 640;

		pDevice->m_nCustomAnalogVideoVgaAspectRatioProperty = 0;

		pDevice->m_nCustomCompanyVision4CEProperty = 0;

		pDevice->m_nCustomAnalogVideoColorRangeProperty = 0x10000;

		pDevice->m_MST3367_bank = 0xFF;

		pDevice->n_video_buffer_count = 0;

		pDevice->m_nCustomCompanyIvsProperty = 0;

		pDevice->m_nCustomSpecialCameraInputProperty = 0;

		pDevice->m_No_Signal_yuv_pattern_property = 0;

		#ifdef NO_SIGNAL_NO_OUTPUT
		pDevice->m_No_Signal_no_output_property = 1;
		#else
		pDevice->m_No_Signal_no_output_property = 0;
		#endif

		pDevice->m_nCustomAnalogVideoSplitProperty = 1;

		pDevice->m_nCustomAnalogAudioVolumeProperty = 128;

		pDevice->m_nCustomAnalogAudioDirectSoundPatchProperty = 0;

		pDevice->m_nTLV320ADC3101_PAGES[0] = 0x00;
		pDevice->m_nTLV320ADC3101_PAGES[1] = 0x00;
		pDevice->m_nTLV320ADC3101_PAGES[2] = 0x00;
		pDevice->m_nTLV320ADC3101_PAGES[3] = 0x00;

		pDevice->m_ReadyToStopControlThread = 0;

		pDevice->m_ReadyToStopVideoLossThread = 0;

		pDevice->m_ReadyToStopAudioLossThread = 0;

		#ifdef WOLF
		pDevice->m_nCustomCompanyMedicalProperty = 0;
		#else
		pDevice->m_nCustomCompanyMedicalProperty = 1;
		#endif

		LINUXV4L2_PRINT(KERN_INFO, "m_nCustomCompanyMedicalProperty(0d%d)\n", pDevice->m_nCustomCompanyMedicalProperty);

		pDevice->m_nCustomAnalogVideoHDR2SDRProperty = 1;

		LINUXV4L2_PRINT(KERN_INFO, "m_nCustomAnalogVideoHDR2SDRProperty(0d%d)\n", pDevice->m_nCustomAnalogVideoHDR2SDRProperty);

		for (i = 0; i < MAX_SUB_DEVICE_NUM; i++) {

			pDevice->m_pKsVideo[i] = NULL;

			pDevice->m_pKsAudioCard[i] = NULL;

			pDevice->m_pKsAudioPCM[i] = NULL;

			pDevice->m_pVideo[i] = NULL;

			pDevice->m_pAudio[i] = NULL;

			pDevice->m_eVideoStreamState[i] = 0x00000000;

			pDevice->m_eAudioStreamState[i] = 0x00000000;
		}

		for (i = 0; i < MAX_SUB_DEVICE_NUM; i++) {

			pDevice->m_pKsVideoEncoder[i] = NULL;

			pDevice->m_pKsAudioEncoderCard[i] = NULL;

			pDevice->m_pKsAudioEncoderPCM[i] = NULL;

			pDevice->m_pVideoEncoder[i] = NULL;

			pDevice->m_pAudioEncoder[i] = NULL;

			pDevice->m_eVideoEncoderStreamState[i] = 0x00000000;

			pDevice->m_eAudioEncoderStreamState[i] = 0x00000000;

			pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[ i ] = 0x77FFFFFF;

		}
		for (i = 0; i < MAX_DMA_BUFFER_NUM; i++) {

			pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[i] = 0;

			pDevice->m_nDmaBaseCommonBufferNumber[i] = 0;

			pDevice->m_nDmaBaseCommonBufferSize[i] = 0;

			pDevice->m_pDmaBaseCommonBuffer[i] = NULL;

			pDevice->m_pDmaBaseCommonPhysicalBuffer[i] = 0x00000000;
		}
		for (i = 0; i < MAX_REG_BUFFER_NUM; i++) {

			pDevice->m_nRegBaseCommonBufferSize[i] = 0;

			pDevice->m_pRegBaseCommonBuffer[i] = NULL;
		}



		pDevice->m_nCustomHardwareTopologyProperty = 0x00000000;

		pDevice->m_nCustomVideoStandardProperty = LINUXV4L2_DEFAULT_VIDEO_STANDARD;

		pDevice->m_nCustomAudioStandardProperty = 0x00000000;

		pDevice->m_IRQ_ready = 1;

		pDevice->m_nCustomAnalogVideoSubEncoderEnableProperty = 0;

		pDevice->m_size_not_match_no_signal_blue_pattern = 0;

		pDevice->m_Invert_UV = 0;

		pDevice->m_nCustomAnalogVideoPinTopologyProperty = 0;

		pDevice->fw_version = 0;
		pDevice->fw_firmware = 0;

		pDevice->m_nOutputOriginalDMAVideo = 0;

		pDevice->m_nCustomAnalogVideoInputEqProperty = 1;

		pDevice->m_nCustomAnalogVideoInputBandwidthProperty = 1;

		init_MUTEX(&pDevice->m_sComponentAccessSemaphore);

		init_waitqueue_head(&pDevice->m_wait_queue_component);

		pDevice->m_component_ready = 1;

		pDevice->is_native_yuy2 = 0;


		pDevice->m_nCustomEdidModeSelectProperty = 0;

		pDevice->m_nCustomCompanyMediaedgeProperty = 0;

		pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 0;


		pDevice->m_nCustomAnalogVideoSDILevelBSupportProperty = 1;

		pDevice->m_nCustomAnalogAudioChannelProperty = 2;

		pDevice->m_nCustomDolbyProperty = dolby;

		pDevice->m_nCustomPowerSavingProperty = powersaving;

		pDevice->m_nCustomSoftwareInterruptProperty = softwareint;

		pDevice->m_nCustomAnalogAudio8chI2sEnableProperty = 0;

		pDevice->m_nCustomAnalogVideoMultiChannelMaskProperty = 0xF;

		pDevice->m_dwDMAErrorFlags = 0x00000000;

		pDevice->m_nMulti_Instance = multiinstance;

		pDevice->m_nEnableDirect = 0;
	}


	pDevice->CFG0 = 0x00000000; pci_read_config_dword(p_pci_dev, 0x00, &(pDevice->CFG0));

	pDevice->CFGB = 0x00000000; pci_read_config_dword(p_pci_dev, 0x2C, &(pDevice->CFGB));

	pDevice->CFG2 = 0x00000000; pci_read_config_dword(p_pci_dev, 0x08, &(pDevice->CFG2));

	pDevice->idVendor = (USHORT)((pDevice->CFG0 & 0x0000FFFF) >> 0);
	pDevice->idProduct = (USHORT)((pDevice->CFG0 & 0xFFFF0000) >> 16);
	pDevice->bcdDevice = (USHORT)((pDevice->CFGB & 0xFFFF0000) >> 16);

	ret = pci_enable_device(p_pci_dev);

	if (ret != 0) {

		LINUXV4L2_DEBUG(KERN_ERR, "[%02d] common_pci_driver_on_probe() - pci_enable_device() - [FAIL]\n", (int)(g_n_counter_of_board_number));

		goto common_pci_driver_on_probe_fail;
	}

	pDevice->iManufacturer = (BYTE)((pDevice->CFGB & 0x000000FF) >> 0);
	pDevice->iProduct = (BYTE)((pDevice->CFGB & 0x0000FF00) >> 8);
	pDevice->iSerialNumber = (BYTE)((pDevice->CFG2 & 0x000000FF) >> 0);

	LINUXV4L2_DEBUG(KERN_INFO, "    idVendor           = %04X\n", pDevice->idVendor);
	LINUXV4L2_DEBUG(KERN_INFO, "    idProduct          = %04X\n", pDevice->idProduct);
	LINUXV4L2_DEBUG(KERN_INFO, "    bcdDevice          = %04X\n", pDevice->bcdDevice);
	LINUXV4L2_DEBUG(KERN_INFO, "    iManufacturer      = %02X\n", pDevice->iManufacturer);
	LINUXV4L2_DEBUG(KERN_INFO, "    iProduct           = %02X\n", pDevice->iProduct);
	LINUXV4L2_DEBUG(KERN_INFO, "    iSerialNumber      = %02X\n", pDevice->iSerialNumber);

#if defined(AME_SC0710) || defined(AME_SC0720)

	if (((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1A)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1A)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1B)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1B)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x1B)) ||
		((pDevice->bcdDevice == 0xB004) && (pDevice->iManufacturer == 0x1B)) ||
		((pDevice->bcdDevice == 0x0B04) && (pDevice->iManufacturer == 0x1B)) ||
		((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x1B)) ||
		((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2B)) ||
		((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B))) {

		if ((pDevice->bcdDevice & 0x00FF) >= 0x00000002) {

			pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;
		}
	}
	if (((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x12)) ||

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

		if ((pDevice->bcdDevice & 0x00FF) >= 0x00000002) {

			pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;
		}
	}
	if (((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1C))) {

		pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;

	}
	if (((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
		((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F))) {

		pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;
	}
	if (((pDevice->bcdDevice == 0xA201) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x1E))) {

		pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;

	}
	if (((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1E)) ||
		((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F))) {

		if ((pDevice->bcdDevice & 0x00FF) >= 0x00000002) {

			pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;
		}
	}
	if (((pDevice->bcdDevice == 0xD004) && (pDevice->iManufacturer == 0x1C)) ||
		((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1C)) ||
		((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ||
		((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
		((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
		((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

		if ((pDevice->bcdDevice & 0x00FF) >= 0x00000002) {

			pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;
		}
	}

	if ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1C)) {

		pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = 1;
	}

	if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||
		((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||

		((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||
		((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) )

	{
		pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = multichannel;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() - m_nCustomAnalogVideoMultiChannelSupportProperty(%d)\n", (int)(g_n_counter_of_board_number), pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty );
	#endif

	pci_set_master( p_pci_dev );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)

	if( ((p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0710) && (pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty == 0)) ||

		((p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0720) && (pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x23)) ||

		(p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0750) )

	{
		ret = dma_set_mask(&p_pci_dev->dev, DMA_BIT_MASK(64) );
	}
	else
	{
		ret = dma_set_mask(&p_pci_dev->dev, DMA_32BIT_MASK);
	}

	if( ret != 0 ) {

		LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - pci_dma_supported() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

		goto common_pci_driver_on_probe_fail;
	}
#else
	ret = pci_dma_supported( p_pci_dev, DMA_32BIT_MASK );

	if( ret == 0 ) {

		LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - pci_dma_supported() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

		goto common_pci_driver_on_probe_fail;
	}
#endif
	ret = pci_request_regions( p_pci_dev, "LINUXV4L2" );

	if( ret != 0 ) {

		LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - pci_request_regions() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

		goto common_pci_driver_on_probe_fail;
	}

	if( (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0710) || (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0720) )
	{
		if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
		{
			ULONG i = 1;

			LINUXV4L2_PRINT(KERN_INFO, "[%02d] MULTI_CH_RING(0d%d)\n", pDevice->m_nKsDeviceNumber, MULTI_CH_RING);

			pDevice->m_nDmaBaseCommonBufferSize[0] = 0x400000;

			for( i = 1; i <= SC0710_DEF_MULTI_CHDMA_ROW; i++ )
			{
				pDevice->m_nDmaBaseCommonBufferSize[ i ] = MULTI_CH_ROW_SIZE;
			}
		}
		else
		{
			pDevice->m_nDmaBaseCommonBufferSize[ 0 ] = 0x400000;


			pDevice->m_nDmaBaseCommonBufferSize[ 1 ] = 0x400000;


			pDevice->m_nDmaBaseCommonBufferSize[ 2 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 3 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 4 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 5 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 6 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 7 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 8 ] = 0x400000;

			pDevice->m_nDmaBaseCommonBufferSize[ 9 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 10 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 11 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 12 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 13 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 14 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 15 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 16 ] = 0x400000;

			pDevice->m_nDmaBaseCommonBufferSize[ 17 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 18 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 19 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 20 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 21 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 22 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 23 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 24 ] = 0x400000;

			pDevice->m_nDmaBaseCommonBufferSize[ 25 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 26 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 27 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 28 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 29 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 30 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 31 ] = 0x400000;
			pDevice->m_nDmaBaseCommonBufferSize[ 32 ] = 0x400000;
		}



		pDevice->m_nDmaBaseCommonBufferSize[129] = 0x2000;
		pDevice->m_nDmaBaseCommonBufferSize[130] = 0x80000;
	}


	for( i = 0 ; i < MAX_DMA_BUFFER_NUM ; i++ ) {

		if( pDevice->m_nDmaBaseCommonBufferSize[ i ] != 0 ) {



			pDevice->m_pDmaBaseCommonBuffer[ i ] = dma_alloc_coherent( &p_pci_dev->dev, pDevice->m_nDmaBaseCommonBufferSize[ i ], &pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ], GFP_KERNEL);

			if( pDevice->m_pDmaBaseCommonBuffer[ i ] == NULL ) {

				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - dma_alloc_coherent() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

				ret = -ENOMEM;

				goto common_pci_driver_on_probe_fail;
			}

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_nDmaBaseCommonBufferSize[%d] (0d%d)\n", (int)(g_n_counter_of_board_number), i, pDevice->m_nDmaBaseCommonBufferSize[ i ] );
			#ifdef CONFIG_64BIT
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_pDmaBaseCommonBuffer[%d] (llx%llx)\n", (int)(g_n_counter_of_board_number), i, pDevice->m_pDmaBaseCommonBuffer[ i ] );
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_pDmaBaseCommonPhysicalBuffer[%d] (llx%llx)\n", (int)(g_n_counter_of_board_number), i, pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ] );
			#else
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_pDmaBaseCommonBuffer[%d] (0p%p)\n", (int)(g_n_counter_of_board_number), i, pDevice->m_pDmaBaseCommonBuffer[ i ] );
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_pDmaBaseCommonPhysicalBuffer[%d] (llx%llx)\n", (int)(g_n_counter_of_board_number), i, pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ] );
			#endif
			memset( pDevice->m_pDmaBaseCommonBuffer[ i ], 0x00, pDevice->m_nDmaBaseCommonBufferSize[ i ] );
		}
	}



	if( (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0710) || (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0720) || (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0750) )
	{
		UINT CFG08 = 0;
		UINT CFG10 = 0;
		UINT CFG14 = 0;
		UINT CFG18 = 0;
		UINT CFG1C = 0;
		UINT CFG20 = 0;
		UINT CFG24 = 0;
		UINT CFG50 = 0;

		pci_read_config_dword( p_pci_dev, 0x08, &CFG08 );
		pci_read_config_dword( p_pci_dev, 0x10, &CFG10 );
		pci_read_config_dword( p_pci_dev, 0x14, &CFG14 );
		pci_read_config_dword( p_pci_dev, 0x18, &CFG18 );
		pci_read_config_dword( p_pci_dev, 0x1C, &CFG1C );
		pci_read_config_dword( p_pci_dev, 0x20, &CFG20 );
		pci_read_config_dword( p_pci_dev, 0x24, &CFG24 );
		pci_read_config_dword( p_pci_dev, 0x50, &CFG50 );

		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG08 (0x%x)\n", CFG08 );
		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG10 (0x%x)\n", CFG10 );
		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG14 (0x%x)\n", CFG14 );
		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG18 (0x%x)\n", CFG18 );
		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG1C (0x%x)\n", CFG1C );
		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG20 (0x%x)\n", CFG20 );
		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG24 (0x%x)\n", CFG24 );
		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_probe() CFG50 (0x%x)\n", CFG50 );

		pDevice->m_nRegBaseCommonBufferSize[ 0 ] = pci_resource_len( p_pci_dev, 0 );

		if( pDevice->m_nRegBaseCommonBufferSize[ 0 ] != 0 ) {

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_nRegBaseCommonBufferSize[ 0 ] (0d%d)\n", (int)(g_n_counter_of_board_number), pDevice->m_nRegBaseCommonBufferSize[ 0 ] );

			pDevice->m_pRegBaseCommonPhysicalBuffer[ 0 ] = pci_resource_start( p_pci_dev, 0 );

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_pRegBaseCommonPhysicalBuffer[ 0 ] (llx%llx)\n", (int)(g_n_counter_of_board_number), pDevice->m_pRegBaseCommonPhysicalBuffer[ 0 ] );

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
			pDevice->m_pRegBaseCommonBuffer[ 0 ] = ioremap( pci_resource_start( p_pci_dev, 0 ), pDevice->m_nRegBaseCommonBufferSize[ 0 ] );
			#else
			pDevice->m_pRegBaseCommonBuffer[ 0 ] = ioremap_nocache( pci_resource_start( p_pci_dev, 0 ), pDevice->m_nRegBaseCommonBufferSize[ 0 ] );
			#endif

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_pRegBaseCommonBuffer[ 0 ] (llx%llx)\n", (int)(g_n_counter_of_board_number), pDevice->m_pRegBaseCommonBuffer[ 0 ] );

			if( pDevice->m_pRegBaseCommonBuffer[ 0 ] == NULL ) {

				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - ioremap() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

				ret = -EIO;

				goto common_pci_driver_on_probe_fail;
			}
		}

		pDevice->m_nRegBaseCommonBufferSize[ 1 ] = pci_resource_len( p_pci_dev, 1 );

		if( pDevice->m_nRegBaseCommonBufferSize[ 1 ] != 0 ) {

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_nRegBaseCommonBufferSize[ 1 ] (0d%d)\n", (int)(g_n_counter_of_board_number), pDevice->m_nRegBaseCommonBufferSize[ 1 ] );

			pDevice->m_pRegBaseCommonPhysicalBuffer[ 1 ] = pci_resource_start( p_pci_dev, 1 );

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_pRegBaseCommonPhysicalBuffer[ 1 ] (llx%llx)\n", (int)(g_n_counter_of_board_number), pDevice->m_pRegBaseCommonPhysicalBuffer[ 1 ] );

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
			pDevice->m_pRegBaseCommonBuffer[ 1 ] = ioremap( pci_resource_start( p_pci_dev, 1 ), pDevice->m_nRegBaseCommonBufferSize[ 1 ] );
			#else
			pDevice->m_pRegBaseCommonBuffer[ 1 ] = ioremap_nocache( pci_resource_start( p_pci_dev, 1 ), pDevice->m_nRegBaseCommonBufferSize[ 1 ] );
			#endif

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() m_pRegBaseCommonBuffer[ 1 ] (llx%llx)\n", (int)(g_n_counter_of_board_number), pDevice->m_pRegBaseCommonBuffer[ 1 ] );

			if( pDevice->m_pRegBaseCommonBuffer[ 1 ] == NULL ) {

				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - ioremap() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

				ret = -EIO;

				goto common_pci_driver_on_probe_fail;
			}
		}
	}

	if( p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0710 )
	{
		pDevice->m_nKsDeviceVideoNumber = 1; pDevice->m_nKsDeviceAudioNumber = 1; pDevice->m_nKsDeviceVideoEncoderNumber = 0; pDevice->m_nKsDeviceAudioEncoderNumber = 0 ; sprintf( psz, "SC0710" );

		if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
		{
			pDevice->m_nKsDeviceVideoNumber = pDevice->m_nKsDeviceAudioNumber = (pDevice->bcdDevice & 0xF);

			if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||
				((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||
				((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ||
				((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18)) )
			{
				pDevice->m_nKsDeviceVideoNumber = pDevice->m_nKsDeviceAudioNumber = 4;

			}

		}

		if((pDevice->bcdDevice == 0x0301) && (pDevice->iManufacturer == 0x17))
		{
			pDevice->m_nKsDeviceAudioNumber = 3;
		}
		if ((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D))
		{
			pDevice->m_nKsDeviceAudioNumber = 6;
		}


		pDevice->m_nKsDeviceVideoNumber = pDevice->m_nKsDeviceVideoNumber + pDevice->m_nKsDeviceVideoNumber * pDevice->m_nMulti_Instance;

		pDevice->m_nKsDeviceAudioNumber = pDevice->m_nKsDeviceAudioNumber + pDevice->m_nKsDeviceAudioNumber * pDevice->m_nMulti_Instance;
	}

	if( p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0720 ) {

		pDevice->m_nKsDeviceVideoNumber = pDevice->m_nKsDeviceAudioNumber = (pDevice->bcdDevice & 0x1F);

		pDevice->m_nKsDeviceVideoEncoderNumber = 0;

		pDevice->m_nKsDeviceAudioEncoderNumber = 0 ;

		sprintf( psz, "SC0720" );
	}

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0)
			ret = v4l2_device_register(&p_pci_dev->dev, &pDevice->v4l2_dev);

    if (ret < 0)
	{
		LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - v4l2_device_register() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

		goto common_pci_driver_on_probe_fail;
	}
	#endif

	for( i = 0 ; i < pDevice->m_nKsDeviceVideoNumber ; i++ ) {

		pDevice->m_pKsVideo[ i ] = video_device_alloc();

		if( pDevice->m_pKsVideo[ i ] == NULL ) {

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - video_device_alloc() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			ret = -ENOMEM;

			goto common_pci_driver_on_probe_fail;
		}
		else {


			CVideo * pVideo = NULL;

			sprintf( pDevice->m_pKsVideo[ i ]->name, "%s:RAW %02d.%02d %08x", psz, g_n_counter_of_board_number, i, ((pDevice->bcdDevice << 16 ) & 0xFFFF0000 ) | ((pDevice->iProduct  <<  8) & 0xFF00) | ((pDevice->iManufacturer << 0) & 0x00FF) );

			pVideo = (CVideo *)(kmalloc( sizeof(struct CVideo), GFP_KERNEL ));

			if( pVideo == NULL ) {

				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - kmalloc() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

				ret = -ENOMEM;

				goto common_pci_driver_on_probe_fail;
			}
			else {
				BYTE SWITCH_CHANNEL_TABLE[ 13 ] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };

				memset( pVideo, 0, sizeof(struct CVideo) );

				pVideo->m_nSubDeviceType                  = 1;

				ret = Initialize_pVideo( pVideo, pDevice, i );


				memcpy( pVideo->m_pSwitchTable_Spliter, SWITCH_CHANNEL_TABLE, 12 );

				pVideo->m_nLockStatusMask_Spliter = SWITCH_CHANNEL_TABLE[ 12 ];

				pVideo->m_nSwitchSpeed_Spliter = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ 0 ][ 7 + 2 ];

				if( ret == FALSE )
				{
					goto common_pci_driver_on_probe_fail;
				}

				pDevice->m_pKsVideo[ i ]->minor = -1;
				pDevice->m_pKsVideo[ i ]->tvnorms = V4L2_STD_525_60 | V4L2_STD_625_50;
				pDevice->m_pKsVideo[ i ]->release = video_device_release;
				pDevice->m_pKsVideo[ i ]->fops = &LINUXV4L2_FILE_OPERATIONS;
				pDevice->m_pKsVideo[ i ]->ioctl_ops = &LINUXV4L2_V4L2_IOCTL_OPS;




				pDevice->m_pKsVideo[ i ]->v4l2_dev = &pDevice->v4l2_dev;

				pDevice->m_pKsVideo[ i ]->queue = &pVideo->vb_vidq;

				#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,3,0)
				pDevice->m_pKsVideo[ i ]->device_caps = V4L2_CAP_STREAMING | V4L2_CAP_AUDIO | V4L2_CAP_VIDEO_CAPTURE;
				#endif

				pDevice->m_pVideo[ i ] = pVideo;

				pDevice->m_pKsVideo[ i ]->lock = &pVideo->mutex;

				#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,16,0)
				set_bit(V4L2_FL_USE_FH_PRIO, &pDevice->m_pKsVideo[ i ]->flags);
				#endif

				video_set_drvdata( pDevice->m_pKsVideo[ i ], pVideo );
			}

		}
		{
			int video_node_number = (int)(g_n_counter_of_video_number) + (int)(g_n_counter_of_video_encoder_number) + i + pDevice->init_video_node_number;


			#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
			ret = video_register_device( pDevice->m_pKsVideo[ i ], VFL_TYPE_VIDEO, video_node_number );
			#else

				ret = video_register_device( pDevice->m_pKsVideo[ i ], VFL_TYPE_GRABBER, video_node_number );

			#endif

			if( ret != 0 ) {

				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - video_register_device() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

				goto common_pci_driver_on_probe_fail;
			}
			else
			{
				LINUXV4L2_PRINT( KERN_INFO, "[%02d] common_pci_driver_on_probe() - video_register_device() - [success]\n", (int)(g_n_counter_of_board_number) );
			}
		}
		#ifdef AME_SC0710

		if( i == 0 )
		{
			if( p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0710 )
			{
				if( FALSE == SC0710_HwInitialize( pDevice ) ) { ret = -EIO; goto common_pci_driver_on_probe_fail; }
			}
		}
		#endif
	}


	g_n_counter_of_video_number += pDevice->m_nKsDeviceVideoNumber;

	#if defined(AME_SC0710) || defined(AME_SC0720)
	if ((p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0710) || (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0720) || (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0750))
	{
		if (pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty == 1)
		{
			ULONG i = 0;
			ULONG j = 0;

			for (j = 0; j < 16; j++)
			{
				for (i = 0; i < MULTI_CH_RING; i++)
				{
					pDevice->p_linecopy_cfg[j][i] = (ULONG*)vmalloc(SC0710_SPEED_LINE_COPY_BUFFER_SIZE);

					if (pDevice->p_linecopy_cfg[j][i] == NULL) {

						LINUXV4L2_DEBUG(KERN_ERR, "[%02d] common_pci_driver_on_probe() - pDevice->p_linecopy_cfg() - [FAIL]\n", (int)(g_n_counter_of_board_number));

						goto common_pci_driver_on_probe_fail;
					}
				}
			}
		}

		if (pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty == 1)
		{
			ULONG i = 0;
			ULONG j = 0;

			for (j = 0; j < 4; j++)
			{
				for (i = 0; i < 4; i++)
				{
					pDevice->p_Storage[j][i] = (BYTE*)vmalloc(STORAGE_SIZE);

					if (pDevice->p_Storage[j][i] == NULL) {

						LINUXV4L2_DEBUG(KERN_ERR, "[%02d] common_pci_driver_on_probe() - pDevice->p_Storage() - [FAIL]\n", (int)(g_n_counter_of_board_number));

						goto common_pci_driver_on_probe_fail;
					}
				}
			}
		}
		{
			tasklet_init(&(pDevice->m_sTasklet_video), common_pci_driver_on_tasklet_video, (unsigned long)(pDevice));

			tasklet_init(&(pDevice->m_sTasklet_audio), common_pci_driver_on_tasklet_audio, (unsigned long)(pDevice));
		}
	}
	#endif

	for( i = 0 ; i < pDevice->m_nKsDeviceAudioNumber ; i++ ) {

		CHAR psz_audio_name[ 16 ] = "PCM #00";

		#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)

			ret = snd_card_new( &pDevice->m_pKsDevice->dev, -1 + pDevice->init_audio_node_number, psz, THIS_MODULE, sizeof(CAudio), &pDevice->m_pKsAudioCard[ i ] );

			if( ret != 0 ) {

				LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - snd_card_new() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

				goto common_pci_driver_on_probe_fail;
			}

		#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,31)

			#ifdef CENTOS_3_10_0_862

					ret = snd_card_new( &pDevice->m_pKsDevice->dev, -1, psz, THIS_MODULE, sizeof(CAudio), &pDevice->m_pKsAudioCard[ i ] );


					if( ret != 0 ) {

						LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - snd_card_new() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

						goto common_pci_driver_on_probe_fail;
					}
			#else
				ret = snd_card_create( -1 + pDevice->init_audio_node_number , NULL, THIS_MODULE, sizeof(CAudio), &pDevice->m_pKsAudioCard[ i ] );

				if( ret != 0 ) {

					LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - snd_card_create() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

					goto common_pci_driver_on_probe_fail;
				}
				else
				{
					LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_pci_driver_on_probe() - snd_card_create() - [success]\n", (int)(g_n_counter_of_board_number) );
				}
			#endif
		#else

			pDevice->m_pKsAudioCard[ i ] = snd_card_new( -1 + pDevice->init_audio_node_number, NULL, THIS_MODULE, sizeof(CAudio) );

		#endif

		if( pDevice->m_pKsAudioCard[ i ] == NULL ) {

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - snd_card_create() / snd_card_new() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			ret = -ENOMEM;



			continue ;
		}
		else {

			sprintf( pDevice->m_pKsAudioCard[ i ]->driver, "LINUXV4L2" );

			if( (pDevice->bcdDevice == 0x0301) && (pDevice->iManufacturer == 0x17)) {

				if(i==0)
				{
					sprintf( pDevice->m_pKsAudioCard[ i ]->shortname, "%s, Audio Capture #%02d.%02d", psz, (int)( (g_n_counter_of_board_number/8) * 4 + g_n_counter_of_board_number % 4 ), (int)(i) );

					sprintf( pDevice->m_pKsAudioCard[ i ]->longname, "%s, Audio Capture #%02d.%02d (Long Name)", psz, (int)((g_n_counter_of_board_number/8) * 4 + g_n_counter_of_board_number % 4), (int)(i) );

					sprintf( pDevice->m_pKsAudioCard[ i ]->mixername, "%s, Audio Capture #%02d.%02d (Mixer Name)", psz, (int)((g_n_counter_of_board_number/8) * 4 + g_n_counter_of_board_number % 4), (int)(i) );
				}
				else if(i == 1)
				{
					sprintf( psz, "SC0710" );

					sprintf( pDevice->m_pKsAudioCard[ i ]->shortname, "%s, Analog Audio #%02d.%02d", psz, (int)( (g_n_counter_of_board_number/8) * 4 + g_n_counter_of_board_number % 4 ), (int)(i) );

					sprintf( pDevice->m_pKsAudioCard[ i ]->longname, "%s, Analog Audio #%02d.%02d (Long Name)", psz, (int)((g_n_counter_of_board_number/8) * 4 + g_n_counter_of_board_number % 4), (int)(i) );

					sprintf( pDevice->m_pKsAudioCard[ i ]->mixername, "%s, Analog Audio #%02d.%02d (Mixer Name)", psz, (int)((g_n_counter_of_board_number/8) * 4 + g_n_counter_of_board_number % 4), (int)(i) );
				}
				else if(i == 2)
				{
					sprintf( psz, "SC0710" );

					sprintf( pDevice->m_pKsAudioCard[ i ]->shortname, "%s, Analog Audio #%02d.%02d", psz, (int)( (g_n_counter_of_board_number/8) * 4 + g_n_counter_of_board_number % 4 ), (int)(i) );

					sprintf( pDevice->m_pKsAudioCard[ i ]->longname, "%s, Analog Audio #%02d.%02d (Long Name)", psz, (int)((g_n_counter_of_board_number/8) * 4 + g_n_counter_of_board_number % 4), (int)(i) );

					sprintf( pDevice->m_pKsAudioCard[ i ]->mixername, "%s, Analog Audio #%02d.%02d (Mixer Name)", psz, (int)((g_n_counter_of_board_number/8) * 4 + g_n_counter_of_board_number % 4), (int)(i) );
				}
				else
				{
				}
			}
			else if ((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D)) {

				if (i <= 3)
				{
					sprintf(pDevice->m_pKsAudioCard[i]->shortname, "%s, Audio Capture #%02d.%02d", psz, (int)((g_n_counter_of_board_number / 8) * 4 + g_n_counter_of_board_number % 4), (int)(i));

					sprintf(pDevice->m_pKsAudioCard[i]->longname, "%s, Audio Capture #%02d.%02d (Long Name)", psz, (int)((g_n_counter_of_board_number / 8) * 4 + g_n_counter_of_board_number % 4), (int)(i));

					sprintf(pDevice->m_pKsAudioCard[i]->mixername, "%s, Audio Capture #%02d.%02d (Mixer Name)", psz, (int)((g_n_counter_of_board_number / 8) * 4 + g_n_counter_of_board_number % 4), (int)(i));
				}
				else if (i > 3)
				{
					sprintf(psz, "SC0710");

					sprintf(pDevice->m_pKsAudioCard[i]->shortname, "%s, Analog Audio #%02d.%02d", psz, (int)((g_n_counter_of_board_number / 8) * 4 + g_n_counter_of_board_number % 4), (int)(i));

					sprintf(pDevice->m_pKsAudioCard[i]->longname, "%s, Analog Audio #%02d.%02d (Long Name)", psz, (int)((g_n_counter_of_board_number / 8) * 4 + g_n_counter_of_board_number % 4), (int)(i));

					sprintf(pDevice->m_pKsAudioCard[i]->mixername, "%s, Analog Audio #%02d.%02d (Mixer Name)", psz, (int)((g_n_counter_of_board_number / 8) * 4 + g_n_counter_of_board_number % 4), (int)(i));
				}
			}
			else
			{
				sprintf( pDevice->m_pKsAudioCard[ i ]->shortname, "%s, Audio Capture #%02d.%02d", psz, (int)(g_n_counter_of_board_number), (int)(i) );

				sprintf( pDevice->m_pKsAudioCard[ i ]->longname, "%s, Audio Capture #%02d.%02d (Long Name)", psz, (int)(g_n_counter_of_board_number), (int)(i) );

				sprintf( pDevice->m_pKsAudioCard[ i ]->mixername, "%s, Audio Capture #%02d.%02d (Mixer Name)", psz, (int)(g_n_counter_of_board_number), (int)(i) );
			}
		}

		pDevice->m_pAudio[ i ] = (CAudio *)(pDevice->m_pKsAudioCard[ i ]->private_data);

		pDevice->m_pAudio[ i ]->m_pDevice = pDevice;
		pDevice->m_pAudio[ i ]->m_nSubDeviceNumber = i;
		pDevice->m_pAudio[ i ]->m_nSubDeviceType = 0x00000001;
		pDevice->m_pAudio[ i ]->m_nInput = 0;
		pDevice->m_pAudio[ i ]->m_nVolume = 128;
		pDevice->m_pAudio[ i ]->m_nRate = 48000;
		pDevice->m_pAudio[ i ]->m_nChannels = 2;
		pDevice->m_pAudio[ i ]->m_nSampleBits = 16;
		pDevice->m_pAudio[ i ]->m_nFrameSize = 1920;
		pDevice->m_pAudio[ i ]->m_nFrameNumber = 0;
		pDevice->m_pAudio[ i ]->m_nFrames = 64;
		pDevice->m_pAudio[ i ]->m_pKsSubStream = NULL;

		pDevice->m_pAudio[ i ]->m_nFrameDMANumber = 0;

		snd_card_set_dev( pDevice->m_pKsAudioCard[ i ], &p_pci_dev->dev );



		ret = snd_ctl_add( pDevice->m_pKsAudioCard[ i ], snd_ctl_new1( &LINUXV4L2_SND_KCONTROL_NEW_VOLUME, pDevice->m_pAudio[ i ] ) );

		if( ret != 0 ) {

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - snd_ctl_add() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			goto common_pci_driver_on_probe_fail;
		}

		sprintf( psz_audio_name, "PCM #%02d", (int)(g_n_counter_of_audio_number + i) );


		ret = snd_pcm_new( pDevice->m_pKsAudioCard[ i ], psz_audio_name, 0x00000000, 0, 1, &(pDevice->m_pKsAudioPCM[ i ]) );

		if( ret != 0 ) {

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - snd_pcm_new() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			goto common_pci_driver_on_probe_fail;
		}
		else {





			sprintf( pDevice->m_pKsAudioPCM[ i ]->name, "%s", psz_audio_name );

			pDevice->m_pKsAudioPCM[ i ]->info_flags = 0x00000000;


			if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) )
			{
				snd_pcm_lib_preallocate_pages_for_all( pDevice->m_pKsAudioPCM[ i ], SNDRV_DMA_TYPE_CONTINUOUS, snd_dma_continuous_data(GFP_KERNEL), LINUXV4L2_SND_PCM_HARDWARE_SC0710.buffer_bytes_max, LINUXV4L2_SND_PCM_HARDWARE_SC0710.buffer_bytes_max );
			}

			snd_pcm_set_ops( pDevice->m_pKsAudioPCM[ i ], SNDRV_PCM_STREAM_CAPTURE, &LINUXV4L2_SND_PCM_OPS );
		}



		ret = snd_card_register( pDevice->m_pKsAudioCard[ i ] );

		if( ret != 0 ) {

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - snd_card_register() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

			goto common_pci_driver_on_probe_fail;
		}
		else
		{
			LINUXV4L2_PRINT( KERN_INFO, "[%02d] common_pci_driver_on_probe() - snd_card_register() - [SUCCESS]\n", (int)(g_n_counter_of_board_number) );
		}
	}

	g_n_counter_of_audio_number += pDevice->m_nKsDeviceAudioNumber;



	for( i = 0 ; i < pDevice->m_nKsDeviceAudioEncoderNumber ; i++ ) {

		;
	}

	g_n_counter_of_audio_encoder_number += pDevice->m_nKsDeviceAudioEncoderNumber;



	if( (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0710) ||

		(p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0720) ||

		(p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0750) ) {

			int msi_num = pci_alloc_irq_vectors( p_pci_dev, 1, 4, PCI_IRQ_MSI | PCI_IRQ_MSIX | PCI_IRQ_AFFINITY);

			if( msi_num < 0 ) {

				LINUXV4L2_PRINT( KERN_ERR, "[%02d] common_pci_driver_on_probe() - pci_alloc_irq_vectors() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

				goto common_pci_driver_on_probe_fail;
			}
			else
			{
				LINUXV4L2_PRINT( KERN_INFO, "THIS KERNEL SUPPORTS MSI(%d)\n", msi_num );
			}

			pDevice->m_IRQ_ready = request_irq( pci_irq_vector( p_pci_dev, 0), common_pci_driver_on_interrupt_2_6_19, IRQF_SHARED, "LINUXV4L2", (PVOID)(pDevice) );
	}
	else
	{
		pDevice->m_IRQ_ready = request_irq( p_pci_dev->irq, common_pci_driver_on_interrupt_2_6_19, IRQF_SHARED, "LINUXV4L2", (PVOID)(pDevice) );
	}


	if( pDevice->m_IRQ_ready != 0 ) {

		LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_pci_driver_on_probe() - request_irq() - [FAIL]\n", (int)(g_n_counter_of_board_number) );

		goto common_pci_driver_on_probe_fail;
	}
	else
	{
		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] common_pci_driver_on_probe() - request_irq() - [SUCCESS]\n", g_n_counter_of_board_number);
	}

	g_n_counter_of_board_number++;

	for( i = 0; i < 32 ; i++ )
	{

		ULONG j = 0;

		for( j = 0; j < 8; j++ )
		{
			pDevice->m_nCustomMotionStatusProperty[ i ][ j ] = 0;
		}

		pDevice->m_nCustomMotionTemporalSensitivityProperty[ i ] = 0;

		pDevice->m_nCustomMotionSpatialSensitivityProperty[ i ] = 0;

		pDevice->m_nCustomMotionLevelSensitivityProperty[ i ] = 0;

		pDevice->m_nCustomMotionSpeedProperty[ i ] = 0;
	}

	#if defined(AME_MZ0380) || defined(AME_SC0710) || defined(AME_SC0720)
	pDevice->wq_audio = alloc_workqueue("wq_audio", WQ_UNBOUND | WQ_HIGHPRI | WQ_CPU_INTENSIVE | __WQ_ORDERED, 0);

	if (pDevice->wq_audio == NULL) {

		LINUXV4L2_PRINT( KERN_ERR, "[%02d] alloc_workqueue() - alloc_workqueue() wq_audio - [FAIL]\n", (int)(g_n_counter_of_board_number) );

		goto common_pci_driver_on_probe_fail;
	}
	#endif

	LINUXV4L2_DEBUG( KERN_INFO, " common_pci_driver_on_probe() end\n" );

	up( &m_sComponentAccessSemaphore );

	return 0;

common_pci_driver_on_probe_fail:

	for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

		if( pDevice->m_pKsAudioCard[ i ] ) {

			snd_card_free( pDevice->m_pKsAudioCard[ i ] );

			pDevice->m_pKsAudioCard[ i ] = NULL;
		}
		if( pDevice->m_pKsAudioPCM[ i ] ) {

			pDevice->m_pKsAudioPCM[ i ] = NULL;
		}
		if( pDevice->m_pAudio[ i ] ) {

			pDevice->m_pAudio[ i ] = NULL;
		}

		if( pDevice->m_pKsVideo[ i ] ) {

			video_set_drvdata( pDevice->m_pKsVideo[ i ], NULL );

			if( -1 != pDevice->m_pKsVideo[ i ]->minor ) { video_unregister_device( pDevice->m_pKsVideo[ i ] ); }

			if( -1 == pDevice->m_pKsVideo[ i ]->minor ) { video_device_release( pDevice->m_pKsVideo[ i ] ); }

			pDevice->m_pKsVideo[ i ] = NULL;
		}


		#if defined(AME_MZ0380) || defined(AME_SC0710) || defined(AME_SC0720)
		if( pDevice->wq_audio )
		{
			 destroy_workqueue( pDevice->wq_audio );

			 pDevice->wq_audio = NULL;
		}
		#endif

		if (pDevice->m_pVideo[i]) {


#if defined(AME_SC0710) || defined(AME_SC0720)
			if (pDevice->m_pVideo[i]->wq_multich_multicpy)
			{
				destroy_workqueue(pDevice->m_pVideo[i]->wq_multich_multicpy);

				pDevice->m_pVideo[i]->wq_multich_multicpy = NULL;
			}

			if ( pDevice->m_pVideo[i]->wq_blkcpy )
			{
				 destroy_workqueue( pDevice->m_pVideo[i]->wq_blkcpy );

				 pDevice->m_pVideo[i]->wq_blkcpy = NULL;
			}
#endif

			FREE( pDevice->m_pVideo[ i ] );
		}
	}

	v4l2_device_unregister(&pDevice->v4l2_dev);

	for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

		if( pDevice->m_pKsAudioEncoderCard[ i ] ) {

			snd_card_free( pDevice->m_pKsAudioEncoderCard[ i ] );

			pDevice->m_pKsAudioEncoderCard[ i ] = NULL;
		}
		if( pDevice->m_pKsAudioEncoderPCM[ i ] ) {

			pDevice->m_pKsAudioEncoderPCM[ i ] = NULL;
		}
		if( pDevice->m_pAudioEncoder[ i ] ) {

			pDevice->m_pAudioEncoder[ i ] = NULL;
		}

		if( pDevice->m_pKsVideoEncoder[ i ] ) {

			video_set_drvdata( pDevice->m_pKsVideoEncoder[ i ], NULL );

			if( -1 != pDevice->m_pKsVideoEncoder[ i ]->minor ) { video_unregister_device( pDevice->m_pKsVideoEncoder[ i ] ); }

			if( -1 == pDevice->m_pKsVideoEncoder[ i ]->minor ) { video_device_release( pDevice->m_pKsVideoEncoder[ i ] ); }

			pDevice->m_pKsVideoEncoder[ i ] = NULL;
		}

		if( pDevice->m_pVideoEncoder[ i ] ) {

			FREE( pDevice->m_pVideoEncoder[ i ] );
		}
	}


	#if defined(AME_MZ0380) || defined(AME_SC0700) || defined(AME_SC0710) || defined(AME_SC0720)

	if(pDevice->m_IRQ_ready == 0)
	{

		if( (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0710) ||

			(p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0720) ||

			(p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0750) ) {

			#ifdef CONFIG_PCI_MSI
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0)

				free_irq( pci_irq_vector( p_pci_dev, 0), (PVOID)(pDevice) );

				pci_free_irq_vectors( p_pci_dev );

				#endif
			#endif
		}
		else
		{
			free_irq( p_pci_dev->irq, (PVOID)(pDevice) );
		}
	}
	#endif

	if( pDevice ) {

		tasklet_kill( &(pDevice->m_sTasklet) );


		#if defined(AME_SC0710) || defined(AME_SC0720)
		if ((p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0710) || (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0720) || (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0750))
		{
			if (pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty == 1)
			{
				ULONG i = 0;
				ULONG j = 0;

				for (j = 0; j < 16; j++)
				{
					for (i = 0; i < MULTI_CH_RING; i++)
					{
						FREE_MEMORY(pDevice->p_linecopy_cfg[j][i]);
					}
				}
			}

			if (pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty == 1)
			{
				ULONG i = 0;
				ULONG j = 0;

				for (j = 0; j < 4; j++)
				{
					for (i = 0; i < 4; i++)
					{
						FREE_MEMORY(pDevice->p_Storage[j][i]);
					}
				}
			}

			{
				tasklet_kill(&(pDevice->m_sTasklet_video));

				tasklet_kill(&(pDevice->m_sTasklet_audio));
			}
		}
		#endif

		for( i = 0 ; i < MAX_REG_BUFFER_NUM ; i++ ) {

			if( pDevice->m_pRegBaseCommonBuffer[ i ] ) {

				iounmap( pDevice->m_pRegBaseCommonBuffer[ i ] );

				pDevice->m_nRegBaseCommonBufferSize[ i ] = 0;

				pDevice->m_pRegBaseCommonBuffer[ i ] = NULL;

				pDevice->m_pRegBaseCommonPhysicalBuffer[ i ] = 0;
			}
		}
		for( i = 0 ; i < MAX_DMA_BUFFER_NUM ; i++ ) {

			if( pDevice->m_pDmaBaseCommonBuffer[ i ] ) {

				dma_free_coherent( &p_pci_dev->dev, pDevice->m_nDmaBaseCommonBufferSize[ i ], pDevice->m_pDmaBaseCommonBuffer[ i ], pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ] );

				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ i ] = 0;

				pDevice->m_nDmaBaseCommonBufferNumber[ i ] = 0;

				pDevice->m_nDmaBaseCommonBufferSize[ i ] = 0;

				pDevice->m_pDmaBaseCommonBuffer[ i ] = NULL;

				pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ] = 0x00000000;
			}
		}
	}

	pci_release_regions( p_pci_dev );

	pci_disable_device( p_pci_dev );

	pci_set_drvdata( p_pci_dev, NULL );


	FREE( pDevice );

	up( &m_sComponentAccessSemaphore );

	return ret;
}

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
	static void common_pci_driver_on_remove( struct pci_dev * p_pci_dev )
	#else
	static void __devexit common_pci_driver_on_remove( struct pci_dev * p_pci_dev )
	#endif
{

	CDevice* pDevice = pci_get_drvdata(p_pci_dev);

	unsigned int i = 0;

	LINUXV4L2_DEBUG(KERN_INFO, "[%02d] common_pci_driver_on_remove()\n", pDevice->m_nKsDeviceNumber);

#ifdef AME_SC0710
	if (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0710) { if (FALSE == SC0710_HwUnInitialize(pDevice)) { ; } }
#endif

#ifdef AME_SC0720
	if (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0720) { if (FALSE == SC0710_HwUnInitialize(pDevice)) { ; } }
	if (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0750) { if (FALSE == SC0710_HwUnInitialize(pDevice)) { ; } }
#endif

	for (i = 0; i < MAX_SUB_DEVICE_NUM; i++) {

		if (pDevice->m_pKsAudioCard[i]) {

			snd_card_free(pDevice->m_pKsAudioCard[i]);

			pDevice->m_pKsAudioCard[i] = NULL;
		}
		if (pDevice->m_pKsAudioPCM[i]) {

			pDevice->m_pKsAudioPCM[i] = NULL;
		}
		if (pDevice->m_pAudio[i]) {

			pDevice->m_pAudio[i] = NULL;
		}

		if (pDevice->m_pKsVideo[i]) {

			video_set_drvdata(pDevice->m_pKsVideo[i], NULL);

			if (-1 != pDevice->m_pKsVideo[i]->minor) { video_unregister_device(pDevice->m_pKsVideo[i]); }

			if (-1 == pDevice->m_pKsVideo[i]->minor) { video_device_release(pDevice->m_pKsVideo[i]); }

			pDevice->m_pKsVideo[i] = NULL;
		}


		#if defined(AME_MZ0380) || defined(AME_SC0710) || defined(AME_SC0720)
		if ( pDevice->wq_audio )
		{
			 destroy_workqueue( pDevice->wq_audio );

			 pDevice->wq_audio = NULL;
		}
		#endif



		if (pDevice->m_pVideo[i]) {


			#if defined(AME_SC0710) || defined(AME_SC0720)
			if (pDevice->m_pVideo[i]->wq_multich_multicpy)
			{
				destroy_workqueue(pDevice->m_pVideo[i]->wq_multich_multicpy);

				pDevice->m_pVideo[i]->wq_multich_multicpy = NULL;
			}

			if ( pDevice->m_pVideo[i]->wq_blkcpy )
			{
				 destroy_workqueue( pDevice->m_pVideo[i]->wq_blkcpy );

				 pDevice->m_pVideo[i]->wq_blkcpy = NULL;
			}
			#endif

			FREE(pDevice->m_pVideo[i]);
		}
	}

	v4l2_device_unregister(&pDevice->v4l2_dev);

	for (i = 0; i < MAX_SUB_DEVICE_NUM; i++) {



		if (pDevice->m_pKsAudioEncoderCard[i]) {



			snd_card_free(pDevice->m_pKsAudioEncoderCard[i]);

			pDevice->m_pKsAudioEncoderCard[i] = NULL;
		}
		if (pDevice->m_pKsAudioEncoderPCM[i]) {

			pDevice->m_pKsAudioEncoderPCM[i] = NULL;
		}
		if (pDevice->m_pAudioEncoder[i]) {

			pDevice->m_pAudioEncoder[i] = NULL;
		}

		if (pDevice->m_pKsVideoEncoder[i]) {

			video_set_drvdata(pDevice->m_pKsVideoEncoder[i], NULL);

			if (-1 != pDevice->m_pKsVideoEncoder[i]->minor) { video_unregister_device(pDevice->m_pKsVideoEncoder[i]); }

			if (-1 == pDevice->m_pKsVideoEncoder[i]->minor) { video_device_release(pDevice->m_pKsVideoEncoder[i]); }

			pDevice->m_pKsVideoEncoder[i] = NULL;
		}

		if (pDevice->m_pVideoEncoder[i]) {

			FREE(pDevice->m_pVideoEncoder[i]);
		}
	}

	#if defined(AME_MZ0380) || defined(AME_SC0700) || defined(AME_SC0710) || defined(AME_SC0720)

	if(pDevice->m_IRQ_ready == 0)
	{

		if( (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0710) ||

			(p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0720) ||

			(p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0750) ) {

			#ifdef CONFIG_PCI_MSI
				#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0)

				free_irq( pci_irq_vector( p_pci_dev, 0), (PVOID)(pDevice) );

				pci_free_irq_vectors( p_pci_dev );

				#endif
			#endif
		}
		else
		{
			free_irq( p_pci_dev->irq, (PVOID)(pDevice) );
		}
	}
	#endif


	if (pDevice) {

		tasklet_kill(&(pDevice->m_sTasklet));


#if defined(AME_SC0710) || defined(AME_SC0720)
		if ((p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0710) || (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0720) || (p_pci_dev->vendor == 0x12AB && p_pci_dev->device == 0x0750))
		{
			if (pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty == 1)
			{
				ULONG i = 0;
				ULONG j = 0;

				for (j = 0; j < 16; j++)
				{
					for (i = 0; i < MULTI_CH_RING; i++)
					{
						FREE_MEMORY(pDevice->p_linecopy_cfg[j][i]);
					}
				}
			}

			if (pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty == 1)
			{
				ULONG i = 0;
				ULONG j = 0;

				for (j = 0; j < 4; j++)
				{
					for (i = 0; i < 4; i++)
					{
						FREE_MEMORY(pDevice->p_Storage[j][i]);
					}
				}
			}

			{
				tasklet_kill(&(pDevice->m_sTasklet_video));

				tasklet_kill(&(pDevice->m_sTasklet_audio));
			}
		}
#endif

		for( i = 0 ; i < MAX_REG_BUFFER_NUM ; i++ ) {

			if( pDevice->m_pRegBaseCommonBuffer[ i ] ) {

				iounmap( pDevice->m_pRegBaseCommonBuffer[ i ] );

				pDevice->m_nRegBaseCommonBufferSize[ i ] = 0;

				pDevice->m_pRegBaseCommonBuffer[ i ] = NULL;

				pDevice->m_pRegBaseCommonPhysicalBuffer[ i ] = 0;
			}
		}
		for( i = 0 ; i < MAX_DMA_BUFFER_NUM ; i++ ) {

			if( pDevice->m_pDmaBaseCommonBuffer[ i ] ) {

				dma_free_coherent( &p_pci_dev->dev, pDevice->m_nDmaBaseCommonBufferSize[ i ], pDevice->m_pDmaBaseCommonBuffer[ i ], pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ] );

				pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ i ] = 0;

				pDevice->m_nDmaBaseCommonBufferNumber[ i ] = 0;

				pDevice->m_nDmaBaseCommonBufferSize[ i ] = 0;

				pDevice->m_pDmaBaseCommonBuffer[ i ] = NULL;

				pDevice->m_pDmaBaseCommonPhysicalBuffer[ i ] = 0x00000000;
			}
		}
	}

	pci_release_regions( p_pci_dev );

	pci_disable_device( p_pci_dev );

	pci_set_drvdata( p_pci_dev, NULL );


	FREE( pDevice );

}

static int common_pci_driver_on_suspend( struct pci_dev * p_pci_dev, pm_message_t s_pm_message_state )
{
	LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_suspend()\n" );

	return 0;
}

static int common_pci_driver_on_resume( struct pci_dev * p_pci_dev )
{
	LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_resume()\n" );

	return 0;
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19)

static irqreturn_t common_pci_driver_on_interrupt_2_6_19( int n_irq, void * p_context )

#else

static irqreturn_t common_pci_driver_on_interrupt_2_6_19( int n_irq, void * p_context ) { return IRQ_HANDLED; }

static irqreturn_t common_pci_driver_on_interrupt_2_6_18( int n_irq, void * p_context, struct pt_regs * p_regs )

#endif

{
	CDevice * pDevice = (CDevice *)(p_context);

	if( pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_pci_driver_on_interrupt_2_6_19() pDevice == NULL\n" );

		return 0;
	}

	#if defined(AME_SC0710) || defined(AME_SC0720)
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) )
	{
		unsigned long flags = 0x00000000;

		spin_lock_irqsave( &pDevice->m_sISR_SpinLock, flags );
		{


			#define SC0710_REGBASE_IRQ				0x00002000

			#define SC0710_REGBASE_BLOCK_OFFSET		0x00001000

			SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

			XDMA_IRQ_REGS * p_regs_interrupt = (XDMA_IRQ_REGS *)(p_sys_cfg->p_config_bar_address + SC0710_REGBASE_IRQ);

			XDMA_ENGINE_REGS * p_regs_xdma_engine_video = (XDMA_ENGINE_REGS *)(p_sys_cfg->p_config_bar_address + SC0710_REGBASE_BLOCK_OFFSET);

			XDMA_ENGINE_REGS * p_regs_xdma_engine_audio = (XDMA_ENGINE_REGS *)(p_sys_cfg->p_config_bar_address + SC0710_REGBASE_BLOCK_OFFSET + SC0710_REGBASE_ENGINE_OFFSET);


			ULONG status1 = READ_REGISTER_ULONG( &(p_regs_interrupt->channelIntRequest) );


			ULONG status2 = READ_REGISTER_ULONG( &(p_regs_interrupt->userIntRequest) );

			if( status1 ) {

				WRITE_REGISTER_ULONG( &(p_regs_interrupt->channelIntEnableW1C), status1 );
			}
			if( status2 ) {

				WRITE_REGISTER_ULONG( &(p_regs_interrupt->userIntEnableW1C), status2 );
			}


			if( status1 == 0 && status2 == 0 ) {

				goto SC0710_EXIT;
			}

			if( status1 || status2 ) {

				if( status1 & 0x00000002 )
				{
					ULONG counts = READ_REGISTER_ULONG(&(p_regs_xdma_engine_video->completedDescCount));

					pDevice->m_nVideoInterruptCount++;

					if (pDevice->debug[0]) LINUXV4L2_DEBUG(KERN_INFO, "[%02d] V, status1(0x%x) InterruptCount(0d%d) desc counts(0d%d) hwint", pDevice->m_nKsDeviceNumber, status1, pDevice->m_nVideoInterruptCount, counts);

					if (pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty) LINUXV4L2_PRINT(KERN_ERR, "[%02d] V, status1(0x%x) hwint err, multich", pDevice->m_nKsDeviceNumber, status1);

					#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
					pDevice->interrupt_ts = ktime_get_ns();
					#else
					do_gettimeofday( &pDevice->interrupt_ts );
					#endif

					spin_lock_irqsave( &pDevice->m_sDPC_Video_SpinLock, flags );

					pDevice->m_nTaskletExtraParameterA = 1;

					spin_unlock_irqrestore( &pDevice->m_sDPC_Video_SpinLock, flags );

					p_sys_cfg->n_frame_no_swint = 0;

					{
						ULONG statusRC = READ_REGISTER_ULONG(&(p_regs_xdma_engine_video->statusRC));

						pDevice->m_sc0710_video_statusRC = statusRC;
					}

					if(pDevice->m_pVideo[0]->m_nCustomAnalogOutputVideoAudioProperty && p_sys_cfg->n_video_running_state)
					{
						tasklet_hi_schedule( &(pDevice->m_sTasklet_video) );
					}
					else
					{
						if( p_sys_cfg->n_video_running_state )
						{
							WRITE_REGISTER_ULONG( &(p_regs_interrupt->channelIntEnableW1S), 2 );
						}
					}
				}

				if( status1 & 0x00000004 )
				{
					ULONG counts = READ_REGISTER_ULONG( &(p_regs_xdma_engine_video->completedDescCount) );

					pDevice->m_nVideoInterruptCount++;

					if( pDevice->debug[ 0 ] ) LINUXV4L2_DEBUG( KERN_INFO, "[%02d] V, status1(0x%x) InterruptCount(0d%d) desc counts(0d%d) hwint", pDevice->m_nKsDeviceNumber, status1, pDevice->m_nVideoInterruptCount, counts );

					if (pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty) LINUXV4L2_PRINT(KERN_ERR, "[%02d] V, status1(0x%x) hwint err, multich", pDevice->m_nKsDeviceNumber, status1);

					#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
					pDevice->interrupt_ts = ktime_get_ns();
					#else
					do_gettimeofday( &pDevice->interrupt_ts );
					#endif

					spin_lock_irqsave( &pDevice->m_sDPC_Video_SpinLock, flags );

					pDevice->m_nTaskletExtraParameterA = 1;

					spin_unlock_irqrestore( &pDevice->m_sDPC_Video_SpinLock, flags );

					p_sys_cfg->n_frame_no_swint = 0;

					{
						ULONG statusRC = READ_REGISTER_ULONG( &(p_regs_xdma_engine_video->statusRC) );


						pDevice->m_sc0710_video_statusRC = statusRC;
					}

					if(pDevice->m_pVideo[0]->m_nCustomAnalogOutputVideoAudioProperty && p_sys_cfg->n_video_running_state)
					{
						tasklet_hi_schedule( &(pDevice->m_sTasklet_video) );
					}
					else
					{
						if( p_sys_cfg->n_video_running_state )
						{
							WRITE_REGISTER_ULONG( &(p_regs_interrupt->channelIntEnableW1S), 4 );
						}
					}
				}

				if( status1 & 0x00000008 )
				{
					if( pDevice->debug[ 0 ] ) LINUXV4L2_DEBUG( KERN_INFO, "[%02d] A, status1(0x%x) hwint", pDevice->m_nKsDeviceNumber, status1 );

					pDevice->m_nAudioInterruptCount++;

					#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
					pDevice->interrupt_ts_audio = ktime_get_ns();
					#else
					do_gettimeofday( &pDevice->interrupt_ts_audio );
					#endif

					spin_lock_irqsave( &pDevice->m_sDPC_Audio_SpinLock, flags );

					pDevice->m_nTaskletExtraParameter_a = 0x10;

					spin_unlock_irqrestore( &pDevice->m_sDPC_Audio_SpinLock, flags );


					{
						ULONG statusRC = READ_REGISTER_ULONG(&(p_regs_xdma_engine_audio->statusRC));


						pDevice->m_sc0710_audio_statusRC = statusRC;
					}


					if(pDevice->m_pVideo[0]->m_nCustomAnalogOutputVideoAudioProperty && p_sys_cfg->n_audio_running_state)
					{
						tasklet_hi_schedule( &(pDevice->m_sTasklet_audio) );
					}
					else
					{
						if( p_sys_cfg->n_audio_running_state )
						{
							WRITE_REGISTER_ULONG( &(p_regs_interrupt->channelIntEnableW1S), 0x8 );
						}
					}
				}
			}
		}
SC0710_EXIT:

		spin_unlock_irqrestore( &pDevice->m_sISR_SpinLock, flags );


		return IRQ_HANDLED;
	}
	#endif


	return IRQ_HANDLED;
}




#if LINUX_VERSION_CODE < KERNEL_VERSION(3,17,0)
#define V4L2_CAP_EXT_PIX_FORMAT         0x00200000
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0)
#define V4L2_CAP_DEVICE_CAPS            0x80000000
#endif

static int common_video_device_vidioc_querycap( struct file * p_file, void * p_priv, struct v4l2_capability * p_cap )
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_querycap() pVideo(0p%p)\n", pVideo );

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_querycap() pVideo == NULL\n" );

		return -EINVAL;
	}
	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_querycap() pVideo->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pVideo->m_pDevice;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_querycap()\n", pDevice->m_nKsDeviceNumber );

	memset( p_cap, 0, sizeof(struct v4l2_capability) );


	#ifdef AME_SC0710
	strcpy( p_cap->driver, "LXV4L2D_SC0710" );
	#endif

	#ifdef AME_SC0720
	strcpy( p_cap->driver, "LXV4L2D_SC0720" );
	#endif

	if( pVideo->m_nSubDeviceType == 0x00000001 )
	{
		if( pDevice->m_pKsVideo[pVideo->m_nSubDeviceNumber] )
		{
			if( pDevice->m_pKsVideo[pVideo->m_nSubDeviceNumber]->name )
			{
				strcpy( p_cap->card, pDevice->m_pKsVideo[pVideo->m_nSubDeviceNumber]->name );
			}
		}
	}
	else
	{
		if( pDevice->m_pKsVideoEncoder[pVideo->m_nSubDeviceNumber] )
		{
			if( pDevice->m_pKsVideoEncoder[pVideo->m_nSubDeviceNumber]->name )
			{
				strcpy( p_cap->card, pDevice->m_pKsVideoEncoder[pVideo->m_nSubDeviceNumber]->name );
			}
		}
	}

	if( pDevice )
	{

		CHAR psz[ 10 ];

		memset( psz, 0, sizeof(psz) );

		strcat( p_cap->bus_info, pDevice->m_pKsDevice->bus->name );

		sprintf( psz, " %08x", pDevice->idVendor << 16 | pDevice->idProduct  );

		strcat( p_cap->bus_info, psz );
	}

	p_cap->version =  (((DRIVER_VERSION/100) << 16) + (((DRIVER_VERSION%100)/10) << 8)) + (DRIVER_VERSION%10);

	p_cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING | V4L2_CAP_AUDIO | V4L2_CAP_DEVICE_CAPS;

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
	p_cap->device_caps = V4L2_CAP_STREAMING |	V4L2_CAP_AUDIO | V4L2_CAP_VIDEO_CAPTURE;
	#endif

	p_cap->reserved[ 1 ] = (pDevice->m_nTemperature << 16) | DRIVER_VERSION;


	if( pDevice )
	{
		p_cap->reserved[ 2 ] = (pDevice->iProduct << 24) | (pDevice->iManufacturer << 16) | pDevice->bcdDevice;
	}


	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_querycap() end\n", pDevice->m_nKsDeviceNumber );

	return 0;
}

static int common_video_device_vidioc_querystd( struct file * p_file, void * p_priv, v4l2_std_id * s )
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_querystd() pVideo == NULL\n" );

		return -EINVAL;
	}
	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_querystd() pVideo->m_pDevice == NULL\n" );

		return -EINVAL;
	}
	pDevice = pVideo->m_pDevice;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_querystd( %08X )\n", pDevice->m_nKsDeviceNumber, (unsigned int)(pVideo->m_nStandard) );

 	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_querystd( %08X )\n", pDevice->m_nKsDeviceNumber, (unsigned int)(pDevice->m_nCustomVideoStandardProperty) );

   *s = (v4l2_std_id)(pVideo->m_nStandard);

   *s = (v4l2_std_id)(pDevice->m_nCustomVideoStandardProperty);

	return 0;
}

static int common_video_device_vidioc_g_std( struct file * p_file, void * p_priv, v4l2_std_id * s )
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_g_std() pVideo == NULL\n" );

		return 0;
	}
	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_g_std() pVideo->m_pDevice == NULL\n" );

		return 0;
	}
	pDevice = pVideo->m_pDevice;

	*s = pDevice->m_nCustomVideoStandardProperty;

	return 0;
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)

static int common_video_device_vidioc_s_std( struct file * p_file, void * p_priv, v4l2_std_id s )
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	int std_err = 0;

	struct pci_dev *p_pci_dev = 0;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_std() pVideo == NULL\n" );

		return 0;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_std() pVideo->m_pDevice == NULL\n" );

		return 0;
	}

	pDevice = pVideo->m_pDevice;

	if( pDevice->m_pKsDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_std() pDevice->m_pKsDevice == NULL\n" );

		return 0;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_std( llx%llx ) 1\n", pDevice->m_nKsDeviceNumber, s );

	p_pci_dev = (struct pci_dev *)pDevice->m_pKsDevice;

	if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) && (pDevice->m_nAnalogCrossbarVideoInputProperty == 4) )
	{


		std_err = 0;
	}
	else
	{
		pDevice->m_nCustomVideoStandardProperty = (ULONG)(s);

		pVideo->m_nStandard = (ULONG)(s);
	}

	if((unsigned int)(s) & V4L2_STD_NTSC)
	{
		if((unsigned int)(s) & V4L2_STD_PAL)
		{
			std_err = 1;
		}
		else
		{
			std_err = 0;
		}

	}
	if((unsigned int)(s) & V4L2_STD_PAL)
	{
		if((unsigned int)(s) & V4L2_STD_NTSC)
		{
			std_err = 1;
		}
		else
		{
			std_err = 0;
		}

	}

	if(std_err)

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
static int common_video_device_vidioc_s_std( struct file * p_file, void * p_priv, v4l2_std_id * s )
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	struct pci_dev *p_pci_dev = NULL;

	int std_err = 0;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_std() pVideo == NULL\n" );

		return 0;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_std() pVideo->m_pDevice == NULL\n" );

		return 0;
	}

	pDevice = pVideo->m_pDevice;

	if( pDevice->m_pKsDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_std() pDevice->m_pKsDevice == NULL\n" );

		return 0;
	}

	p_pci_dev = pDevice->m_pKsDevice;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_std( %08X ) 2\n", pDevice->m_nKsDeviceNumber, (unsigned int)(*s) );

	if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) && (pDevice->m_nAnalogCrossbarVideoInputProperty == 4) )
	{


		std_err = 0;
	}
	else
	{
		pDevice->m_nCustomVideoStandardProperty = (ULONG)(*s);

		pVideo->m_nStandard = (ULONG)(*s);
	}

	if((unsigned int)(*s) & V4L2_STD_NTSC)
	{
		if((unsigned int)(*s) & V4L2_STD_PAL)
		{
			std_err = 1;
		}
		else
		{
			std_err = 0;
		}

	}
	if((unsigned int)(*s) & V4L2_STD_PAL)
	{
		if((unsigned int)(*s) & V4L2_STD_NTSC)
		{
			std_err = 1;
		}
		else
		{
			std_err = 0;
		}

	}
	if(std_err)

#else

static int common_video_device_vidioc_s_std( struct file * p_file, void * p_priv, v4l2_std_id s )
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_std() pVideo == NULL\n" );

		return 0;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_std() pVideo->m_pDevice == NULL\n" );

		return 0;
	}

	pDevice = pVideo->m_pDevice;

	if( pDevice->m_pKsDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_std() pDevice->m_pKsDevice == NULL\n" );

		return 0;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_std( %08X ) 3\n", pDevice->m_nKsDeviceNumber, s );

	struct pci_dev *p_pci_dev = pDevice->m_pKsDevice;

	int std_err = 0;

	if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) && (pDevice->m_nAnalogCrossbarVideoInputProperty == 4) )
	{


		std_err = 0;
	}
	else
	{
		pDevice->m_nCustomVideoStandardProperty = (ULONG)(s);

		pVideo->m_nStandard = (ULONG)(s);
	}

	if((unsigned int)(s) & V4L2_STD_NTSC)
	{
		if((unsigned int)(s) & V4L2_STD_PAL)
		{
			std_err = 1;
		}
		else
		{
			std_err = 0;
		}

	}
	if((unsigned int)(s) & V4L2_STD_PAL)
	{
		if((unsigned int)(s) & V4L2_STD_NTSC)
		{
			std_err = 1;
		}
		else
		{
			std_err = 0;
		}

	}


#endif
	{
		pDevice->m_nCustomVideoStandardProperty = LINUXV4L2_DEFAULT_VIDEO_STANDARD;

		pVideo->m_nStandard = LINUXV4L2_DEFAULT_VIDEO_STANDARD;

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_std( %08X ) force to default\n", pDevice->m_nKsDeviceNumber, pVideo->m_nStandard );

	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_std( %08X ) 4\n", pDevice->m_nKsDeviceNumber, pVideo->m_nStandard );

	return 0;
}

static int common_video_device_vidioc_enumaudio(struct file *p_file, void *priv, struct v4l2_audio *a)
{
	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	CDevice * pDevice = 0;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_enumaudio() pVideo == NULL\n" );

		return 0;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_enumaudio() pVideo->m_pDevice == NULL\n" );

		return 0;
	}

	pDevice = pVideo->m_pDevice;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] vidioc_enumaudio( %02d )\n", pDevice->m_nKsDeviceNumber, a->index );

	if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0700) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0701) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) ||

		(pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) ) {

        if (a->index == 0)
		{
			strcpy(a->name, "embedded");

			a->capability = V4L2_AUDCAP_STEREO;

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] vidioc_enumaudio() index(0d%d) capability(0x%x)\n", pDevice->m_nKsDeviceNumber, a->index, a->capability );

			return 0;
		}

        if (a->index == 1)
		{

			strcpy(a->name, "Line in");

			a->capability = V4L2_AUDCAP_STEREO;

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] vidioc_enumaudio() index(0d%d) capability(0x%x) ---\n", pDevice->m_nKsDeviceNumber, a->index, a->capability );
			return 0;
		}

	}

	return -EINVAL;
}

static int common_video_device_vidioc_enum_input( struct file * p_file, void * p_priv, struct v4l2_input * p_input )
{
	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	CDevice * pDevice = NULL;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_enum_input() pVideo == NULL\n" );

		return 0;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_enum_input() pVideo->m_pDevice == NULL\n" );

		return 0;
	}

	pDevice = pVideo->m_pDevice;



	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ||
		(pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) {

		if( p_input->index <= 5 )
		{

			p_input->type = V4L2_INPUT_TYPE_CAMERA;

			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50;



			p_input->status = ((pVideo->m_nLockStatus >> 0) & (0x00000001))? 0x0: 0x102;

			strcpy( p_input->name, "#00 COMPOSITE INPUT" );

			return 0;
		}
	}


	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) )
	{
		ULONG ch_quantity = 1;

		SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

		if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
		{
			ch_quantity = (pDevice->bcdDevice & 0xF);
		}

		if( p_input->index == 0 ) {

			p_input->type = V4L2_INPUT_TYPE_CAMERA;

			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50;

			if(pDevice->m_nAnalogCrossbarVideoInputProperty == 0)
			{
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusPropertys[ (pVideo->m_nSubDeviceNumber) % ch_quantity ] & 0x01) ? 0x0 : 0x102);

			}
			else
			{
				p_input->status = 0x102;
			}

			if(pVideo->m_nInputChanged) { p_input->status = 0x102; }

			if( pDevice->m_nAnalogCopyProtMacrovisionProperty )
			{
				p_input->status |= V4L2_IN_ST_MACROVISION;
			}
			else
			{
				p_input->status &= ~V4L2_IN_ST_MACROVISION;
			}

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
			p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
			#endif

			strcpy( p_input->name, "HDMI INPUT(0)" );

			return 0;
		}
		if( p_input->index == 1 ) {

			p_input->type = V4L2_INPUT_TYPE_CAMERA;

			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50;

			if(pDevice->m_nAnalogCrossbarVideoInputProperty == 1)
			{
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusPropertys[ (pVideo->m_nSubDeviceNumber) % ch_quantity ] & 0x01) ? 0x0 : 0x102);
			}
			else
			{
				p_input->status = 0x102;
			}

			if(pVideo->m_nInputChanged) { p_input->status = 0x102; }

			if( pDevice->m_nAnalogCopyProtMacrovisionProperty )
			{
				p_input->status |= V4L2_IN_ST_MACROVISION;
			}
			else
			{
				p_input->status &= ~V4L2_IN_ST_MACROVISION;
			}

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
			p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
			#endif

			strcpy( p_input->name, "DVI-D INPUT(1)" );

			return 0;
		}

		if( p_input->index == 2 ) {

			p_input->type = V4L2_INPUT_TYPE_CAMERA;

			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50;

			if(pDevice->m_nAnalogCrossbarVideoInputProperty == 2)
			{
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusPropertys[ (pVideo->m_nSubDeviceNumber) % ch_quantity ] & 0x01) ? 0x0 : 0x102);
			}
			else
			{
				p_input->status = 0x102;
			}

			if(pVideo->m_nInputChanged) { p_input->status = 0x102; }

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
			p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
			#endif

			if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x10)) ||
				 ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x11)) ) {

				if( pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat == 1 )
				{
					strcpy( p_input->name, "DP SST INPUT(2)" );
				}
				else
				{
					strcpy( p_input->name, "DP MST INPUT(2)" );
				}
			}
			else
			{
				strcpy( p_input->name, "COMPONENT(YPbPr) INPUT(2)" );
			}

			return 0;
		}

		if( p_input->index == 3 ) {

			p_input->type = V4L2_INPUT_TYPE_CAMERA;

			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50;

			if(pDevice->m_nAnalogCrossbarVideoInputProperty == 3)
			{
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusPropertys[ (pVideo->m_nSubDeviceNumber) % ch_quantity ] & 0x01) ? 0x0 : 0x102);
			}
			else
			{
				p_input->status = 0x102;
			}

			if(pVideo->m_nInputChanged) { p_input->status = 0x102; }

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
			p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
			#endif

			strcpy( p_input->name, "DVI-A INPUT(3)" );

			return 0;
		}
		if( p_input->index == 4 ) {

			p_input->type = V4L2_INPUT_TYPE_CAMERA;

			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50;

			if(pDevice->m_nAnalogCrossbarVideoInputProperty == 4)
			{
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusPropertys[ (pVideo->m_nSubDeviceNumber) % ch_quantity ] & 0x01) ? 0x0 : 0x102);
			}
			else
			{
				p_input->status = 0x102;
			}

			if (((pDevice->bcdDevice == 0xD004) && (pDevice->iManufacturer == 0x1C)) ||
				((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1C)) ||
				((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ||
				((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
				((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
				((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

				if (p_sys_cfg->n_input_tvi_signal_locksp[pVideo->m_nSubDeviceNumber % 16] == 0) {

					p_input->status = 0x102;
				}
			}

			if(pVideo->m_nInputChanged) { p_input->status = 0x102; }

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
			p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
			#endif

			strcpy( p_input->name, "SDI INPUT(4)" );

			return 0;
		}
		if( p_input->index == 5 ) {

			p_input->type = V4L2_INPUT_TYPE_CAMERA;

			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50;

			if(pDevice->m_nAnalogCrossbarVideoInputProperty == 5)
			{
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusPropertys[ (pVideo->m_nSubDeviceNumber) % ch_quantity ] & 0x01) ? 0x0 : 0x102);
			}
			else
			{
				p_input->status = 0x102;
			}

			if (((pDevice->bcdDevice == 0xD004) && (pDevice->iManufacturer == 0x1C)) ||
				((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1C)) ||
				((pDevice->bcdDevice == 0xC004) && (pDevice->iManufacturer == 0x1D)) ||
				((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x1D)) ||
				((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
				((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D))) {

				if (p_sys_cfg->n_input_tvi_signal_locksp[pVideo->m_nSubDeviceNumber % 16] == 0) {

					p_input->status = 0x102;
				}
			}

			if(pVideo->m_nInputChanged) { p_input->status = 0x102; }

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
			p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
			#endif

			strcpy( p_input->name, "COMPOSITE INPUT(5)" );

			return 0;
		}
		if( p_input->index == 6 ) {

			p_input->type = V4L2_INPUT_TYPE_CAMERA;

			p_input->std = V4L2_STD_525_60 | V4L2_STD_625_50;

			if(pDevice->m_nAnalogCrossbarVideoInputProperty == 6)
			{
				p_input->status = ((pDevice->m_nAnalogVideoDecoderStatusPropertys[ (pVideo->m_nSubDeviceNumber) % ch_quantity ] & 0x01) ? 0x0 : 0x102);
			}
			else
			{
				p_input->status = 0x102;
			}

			if(pVideo->m_nInputChanged) { p_input->status = 0x102; }

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
			p_input->capabilities = V4L2_IN_CAP_PRESETS | V4L2_IN_CAP_STD;
			#endif

			strcpy( p_input->name, "S-CONNECTOR INPUT(6)" );

			return 0;
		}
	}

return -EINVAL;
}

static int common_video_device_vidioc_s_input( struct file * p_file, void * p_priv, unsigned int i )
{
	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	CDevice* pDevice = pVideo->m_pDevice;

	ULONG input = i;

	ULONG force_setcrosssbar = 0;

	down( &m_sComponentAccessSemaphore );

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_input( 0x%x )\n", pDevice->m_nKsDeviceNumber, i );

	if( input & 0x80000000 )
	{
		if( input & V4L2_IN_ST_MACROVISION )
		{
			pDevice->m_nCustomCompanyMedicalProperty = 0;


			if( input & 0x04000000 )
			{
				pDevice->m_nAnalogCopyProtMacrovisionEnableProperty = 1;
			}
			else
			{
				pDevice->m_nAnalogCopyProtMacrovisionEnableProperty = 0;
			}
		}
		else
		{
			pDevice->m_nCustomCompanyMedicalProperty = 1;

			pDevice->m_nAnalogCopyProtMacrovisionEnableProperty = 0;
		}
		LINUXV4L2_PRINT( KERN_INFO, "[%02d] common_video_device_vidioc_s_input() Medical(0x%x) MacrovisionEnable(0x%x)\n", pDevice->m_nKsDeviceNumber, pDevice->m_nCustomCompanyMedicalProperty, pDevice->m_nAnalogCopyProtMacrovisionEnableProperty );

		if( (input & 0xF) == 4 )
		{
			if( (input & 0x100) == 0x100 )
			{
				pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat = 0;
			}
			else
			{
				pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat = 1;
			}
			LINUXV4L2_PRINT( KERN_INFO, "[%02d] common_video_device_vidioc_s_input() %s\n", pDevice->m_nKsDeviceNumber, pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat ? "2SI" : "QSD" );

			if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||
				((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ) {

				if( (input & 0x200) == 0x200 )
				{
					pDevice->m_nCustomCompanyMediaedgeProperty = 1;
				}
				else
				{
					pDevice->m_nCustomCompanyMediaedgeProperty = 0;
				}

				LINUXV4L2_PRINT( KERN_INFO, "[%02d] common_video_device_vidioc_s_input() Mediaedge(%d)\n", pDevice->m_nKsDeviceNumber, pDevice->m_nCustomCompanyMediaedgeProperty );
			}
		}
		else
		{
			if( (input & 0x100) == 0x100 )
			{
				if( pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat != 1 )
				{
					force_setcrosssbar = 1;
				}

				pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat = 1;
			}
			else
			{
				if( pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat != 0 )
				{
					force_setcrosssbar = 1;
				}

				pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat = 0;

			}
			LINUXV4L2_PRINT( KERN_INFO, "[%02d] common_video_device_vidioc_s_input() %s\n", pDevice->m_nKsDeviceNumber, pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat ? "SST" : "MST" );
		}

		pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty = (input >> 11) & 0x3;

		LINUXV4L2_PRINT(KERN_INFO, "[%02d] common_video_device_vidioc_s_input() m_nCustomAnalogVideoNativeColorSpaceProperty(0x%x)\n", pDevice->m_nKsDeviceNumber, pDevice->m_nCustomAnalogVideoNativeColorSpaceProperty);

		if( (input & 0x30) == 0x00 )
		{
			if( pDevice->m_nCustomAnalogVideoColorRangeProperty != 0x10000 )
			{
				pDevice->m_nCustomAnalogVideoColorRangeProperty = 0x10000;

				force_setcrosssbar = 1;
			}
		}
		else if( (input & 0x30) == 0x10 )
		{
			if( pDevice->m_nCustomAnalogVideoColorRangeProperty != 0x10101 )
			{
				pDevice->m_nCustomAnalogVideoColorRangeProperty = 0x10101;

				force_setcrosssbar = 1;
			}
		}
		else if( (input & 0x30) == 0x20 )
		{
			if( pDevice->m_nCustomAnalogVideoColorRangeProperty != 0x10022 )
			{
				pDevice->m_nCustomAnalogVideoColorRangeProperty = 0x10022;

				force_setcrosssbar = 1;
			}
		}
		else
		{
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_input() unsupported color range(0x%x)\n", pDevice->m_nKsDeviceNumber, input & 0x30 );
		}

		LINUXV4L2_PRINT( KERN_INFO, "[%02d] common_video_device_vidioc_s_input() color range(0x%x)\n", pDevice->m_nKsDeviceNumber, pDevice->m_nCustomAnalogVideoColorRangeProperty );


		if( (input & 0xC0) == 0x00 )
		{
			pDevice->m_nCustomEdidModeSelectProperty = 0;
		}
		else if( (input & 0xC0) == 0x40 )
		{
			pDevice->m_nCustomEdidModeSelectProperty = 1;
		}
		else if( (input & 0xC0) > 0x40 )
		{
			pDevice->m_nCustomEdidModeSelectProperty = 2;
		}
		else
		{
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_input() unsupported edid mode(0x%x)\n", pDevice->m_nKsDeviceNumber, input & 0xC0 );
		}

		LINUXV4L2_PRINT( KERN_INFO, "[%02d] common_video_device_vidioc_s_input() edid mode(0x%x)\n", pDevice->m_nKsDeviceNumber, pDevice->m_nCustomEdidModeSelectProperty );


		#ifdef AME_SC0710
		if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 )
		{
			SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

			if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||
				((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||

				((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||
				((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) )

			{
				ULONG update_reg = 0;

				if (pDevice->m_nCustomAnalogVideoMultiChannelMaskProperty != ((input >> 20) & 0xF))
				{

					pDevice->m_nCustomAnalogVideoMultiChannelMaskProperty = (input >> 20) & 0xF;
				}

				if (pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty != ((input >> 19) & 0x1))
				{

					pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty = (input >> 19) & 0x1;

					update_reg++;
				}

				if (update_reg)
				{
					p_sys_cfg->b_is_background_updated = TRUE;
				}

				LINUXV4L2_PRINT(KERN_INFO, "[%02d] common_video_device_vidioc_s_input() m_nCustomAnalogVideoMultiChannelSupportProperty(0x%x) m_nCustomAnalogVideoMultiChannelMaskProperty(0x%x)\n",

					pDevice->m_nKsDeviceNumber, pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty, pDevice->m_nCustomAnalogVideoMultiChannelMaskProperty);
			}

			if( (pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x000E) ||
				(pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x000F) ) {

				pDevice->m_nCustomCompanyMedicalProperty = 1;
			}

			if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0 ) {

				ULONG i = 0;

				SC0710_CLOSE_THREAD();

				for( i = 0 ; i < 4 ; i++ ) {

					if( pDevice->m_nCustomCompanyMedicalProperty == 0 ) {

						SC0710_EnableHDCP( pDevice, 1, i );
					}
					else {

						SC0710_EnableHDCP( pDevice, 0, i );
					}
				}

				SC0710_START_THREAD();
			}
			else {

				if( ((input & 0xF) == 0) || ((input & 0xF) == 1) )
				{
					if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x13)) ||
						((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x13))  )

					{

					}
					else
					{
						SC0710_CLOSE_THREAD();

						if( pDevice->m_nCustomCompanyMedicalProperty == 0 ) {

							SC0710_EnableHDCP( pDevice, 1, 0 );
						}
						else {

							SC0710_EnableHDCP( pDevice, 0, 0 );
						}

						SC0710_START_THREAD();
					}
				}
			}
		}
		#endif
	}

	input &= 0xF;

	if(input == 0xF)
	{
		pDevice->m_nAnalogCrossbarVideoInputAutoScanProperty = 1;
	}
	else
	{
		pDevice->m_nAnalogCrossbarVideoInputAutoScanProperty = 0;
	}

	#if defined(AME_SC0710) || defined(AME_SC0720)
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) ||
		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) ||
		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) ) {

		if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0)
		{
			if((pDevice->bcdDevice & 0x00FF) > 1)
			{
				if(input != pDevice->m_nAnalogCrossbarVideoInputPropertys[pVideo->m_nSubDeviceNumber % 4])
				{
					force_setcrosssbar = 1;
				}
			}
			else
			{
				if(input != pDevice->m_nAnalogCrossbarVideoInputProperty)
				{
					force_setcrosssbar = 1;
				}
			}
		}
		else
		{
			if(input != pDevice->m_nAnalogCrossbarVideoInputProperty)
			{
				force_setcrosssbar = 1;
			}
		}
	}
	#else
	if(input != pDevice->m_nAnalogCrossbarVideoInputProperty)
	{
		force_setcrosssbar = 1;
	}
	#endif

	if(force_setcrosssbar)
	{
		pDevice->m_nAnalogCrossbarVideoInputProperty = input;

		pDevice->m_nAnalogCrossbarVideoInputPropertys[pVideo->m_nSubDeviceNumber % 4] = input;

			#ifdef AME_SC0710
			if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 )
			{
				SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

				if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x13)) ||
					((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x13))  ) {

					ULONG i = 0;

					LINUXV4L2_PRINT(KERN_INFO, "[%02d] force resolution 0\n", pDevice->m_nKsDeviceNumber);

					pVideo->m_nInputChanged = 1;

					SC0710_CLOSE_THREAD();

					LINUXV4L2_DEBUG(KERN_INFO, "[%02d] clear resolution\n", pDevice->m_nKsDeviceNumber);

					pDevice->m_nAnalogVideoDecoderStatusProperty = 0;

					for (i = 0; i < 16; i++) {

						p_sys_cfg->n_input_video_resolution_cxs[i] = 0;

						p_sys_cfg->n_input_video_resolution_cys[i] = 0;

						p_sys_cfg->n_input_video_resolution_fpss[i] = 0;

						p_sys_cfg->n_input_video_resolution_fps_ms[i] = 0;

						p_sys_cfg->n_input_video_resolution_levelbs[i] = 0;

						p_sys_cfg->n_input_video_resolution_psfs[i] = 0;

						p_sys_cfg->n_input_video_resolution_color_colorimetrys[i] = 0;

						p_sys_cfg->n_input_audio_sampling_frequencys[i] = 0;

						pDevice->m_nAnalogVideoDecoderStatusPropertys[i] = 0;
					}

					p_sys_cfg->b_is_background_updated = TRUE;

					SC0710_START_THREAD();
				}
				else
				{
					SC0710_SetAnalogCrossbarRouteProperty( pDevice, pVideo->m_nSubDeviceNumber % 4, pDevice->m_nAnalogCrossbarVideoInputProperty, 0, pDevice->m_nAnalogCrossbarAudioInputProperty, 0, FALSE);
				}
			}
			#endif

	}
	else
	{
		LINUXV4L2_PRINT( KERN_INFO, "[%02d] same input(%d)\n", pDevice->m_nKsDeviceNumber, pDevice->m_nAnalogCrossbarVideoInputProperty );
	}

	up( &m_sComponentAccessSemaphore );

	return 0;
}

static int common_video_device_vidioc_g_input( struct file * p_file, void * p_priv, unsigned int * i )
{
	CDevice* pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_g_input() pVideo == NULL\n" );

		return -EINVAL;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_g_input() pVideo->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pVideo->m_pDevice;



   *i = pDevice->m_nAnalogCrossbarVideoInputProperty;

	return 0;
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
static int common_video_device_vidioc_s_audio( struct file * p_file, void *fh, const struct v4l2_audio *a)
#else
static int common_video_device_vidioc_s_audio( struct file * p_file, void *fh, struct v4l2_audio *a )
#endif
{
	CDevice* pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	BOOL reset = FALSE;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_audio() pVideo == NULL\n" );

		return -EINVAL;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_audio() pVideo->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pVideo->m_pDevice;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_audio() index(0x%x)\n", pDevice->m_nKsDeviceNumber, a->index );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_audio() mode(0x%x)\n", pDevice->m_nKsDeviceNumber, a->mode );

	if( ( a->index & 0xF) > 2 )
	{
		return -EINVAL;
	}

	down( &m_sComponentAccessSemaphore );

	if( (( (a->index) >> 4 ) & 0xF) != pDevice->m_nCustomAnalogAudioPinTopologyProperty )
	{
		pDevice->m_nCustomAnalogAudioPinTopologyProperty = ( (a->index) >> 4 ) & 0xF;

		reset = TRUE;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_audio() m_nCustomAnalogAudioPinTopologyProperty(0d%d)\n", pDevice->m_nKsDeviceNumber, pDevice->m_nCustomAnalogAudioPinTopologyProperty );

	if( (a->index & 0xF) != pDevice->m_nAnalogCrossbarAudioInputProperty )
	{
		reset = TRUE;
	}

	if( reset )
	{
		pDevice->m_nAnalogCrossbarAudioInputProperty = (a->index & 0xF);

		#if defined(AME_SC0710) || defined(AME_SC0720)
		if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710)
		{
			SC0710_SetAnalogCrossbarRouteProperty( pDevice, pVideo->m_nSubDeviceNumber % 4, -1, -1, pDevice->m_nAnalogCrossbarAudioInputProperty, 0, FALSE);
		}
		#endif
	}

	if( a->index & 0x80000000 )
	{
		if( (a->index & 0x80000100) == 0x80000100)
		{
			pDevice->debug[ 0 ] = 1;
		}
		else
		{
			pDevice->debug[ 0 ] = 0;
		}

		if ((a->index & 0x80000200) == 0x80000200)
		{
			pDevice->debug[ 1 ] = 1;
		}
		else
		{
			pDevice->debug[ 1 ] = 0;
		}

		if ((a->index & 0x80000400) == 0x80000400)
		{
			pDevice->debug[ 2 ] = 1;
		}
		else
		{
			pDevice->debug[ 2 ] = 0;
		}

		if ((a->index & 0x80000800) == 0x80000800)
		{
			pDevice->debug[ 3 ] = 1;
		}
		else
		{
			pDevice->debug[ 3 ] = 0;
		}

		if ((a->index & 0x80001000) == 0x80001000)
		{
			pDevice->debug[4] = 1;
		}
		else
		{
			pDevice->debug[4] = 0;
		}

		if ((a->index & 0x80002000) == 0x80002000)
		{
			pDevice->debug[5] = 1;
		}
		else
		{
			pDevice->debug[5] = 0;
		}

		if ((a->index & 0x80004000) == 0x80004000)
		{
			pDevice->debug[6] = 1;
		}
		else
		{
			pDevice->debug[6] = 0;
		}

		if ((a->index & 0x80008000) == 0x80008000)
		{
			pDevice->debug[7] = 1;
		}
		else
		{
			pDevice->debug[7] = 0;
		}


		if ((a->index & 0x80010000) == 0x80010000)
		{
			pDevice->debug[8] = 1;
		}
		else
		{
			pDevice->debug[8] = 0;
		}

		if ((a->index & 0x80020000) == 0x80020000)
		{
			pDevice->debug[9] = 1;
		}
		else
		{
			pDevice->debug[9] = 0;
		}

		if ((a->index & 0x80040000) == 0x80040000)
		{
			pDevice->debug[10] = 1;
		}
		else
		{
			pDevice->debug[10] = 0;
		}

		if ((a->index & 0x80080000) == 0x80080000)
		{
			pDevice->debug[11] = 1;
		}
		else
		{
			pDevice->debug[11] = 0;
		}


		if ((a->index & 0x80100000) == 0x80100000)
		{
			pDevice->debug[12] = 1;
		}
		else
		{
			pDevice->debug[12] = 0;
		}

		if ((a->index & 0x80200000) == 0x80200000)
		{
			pDevice->debug[13] = 1;
		}
		else
		{
			pDevice->debug[13] = 0;
		}

		if ((a->index & 0x80400000) == 0x80400000)
		{
			pDevice->debug[14] = 1;
		}
		else
		{
			pDevice->debug[14] = 0;
		}

		if ((a->index & 0x80800000) == 0x80800000)
		{
			pDevice->debug[15] = 1;
		}
		else
		{
			pDevice->debug[15] = 0;
		}
	}


	up( &m_sComponentAccessSemaphore );

	return 0;
}
static int common_video_device_vidioc_g_audio(struct file *p_file, void *fh, struct v4l2_audio *a)
{
	CDevice* pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_g_audio() pVideo == NULL\n" );

		return -EINVAL;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_g_audio() pVideo->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pVideo->m_pDevice;

	a->index = (pDevice->m_nCustomAnalogAudioPinTopologyProperty << 4) | pDevice->m_nAnalogCrossbarAudioInputProperty;

	return 0;
}
static int common_video_device_vidioc_s_parm( struct file * p_file, void * p_priv, struct v4l2_streamparm *sp )
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_parm() pVideo == NULL\n" );

		return -EINVAL;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_parm() pVideo->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pVideo->m_pDevice;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_parm() m_nSubDeviceNumber(%d) denominator(%d) numerator(%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber, sp->parm.capture.timeperframe.denominator, sp->parm.capture.timeperframe.numerator);

	if (sp->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	if( (sp->parm.capture.timeperframe.numerator > 0) && (sp->parm.capture.timeperframe.denominator > 0) && (sp->parm.capture.timeperframe.denominator > sp->parm.capture.timeperframe.numerator) )
	{
		ULONG modified_numerator = sp->parm.capture.timeperframe.numerator;

		if( modified_numerator == 1001)
		{
			modified_numerator = 1000;
		}

		if((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))

		{
			if(pVideo->m_nSubDeviceType == 1)
			{
				if(modified_numerator)
				{
					pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] = sp->parm.capture.timeperframe.denominator / modified_numerator;
				}

				LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_parm() denominator(0d%d) numerator(0d%d) m_nCustomAnalogVideoAvgFrameRateProperty[ %d ](0d%d)\n", pDevice->m_nKsDeviceNumber,

					sp->parm.capture.timeperframe.denominator, sp->parm.capture.timeperframe.numerator, pVideo->m_nSubDeviceNumber, pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] );
			}
		}
		else
		{
			if( pVideo->m_nSubDeviceType == 0x00000001 )
			{
				pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[ 0 ] = sp->parm.capture.timeperframe.denominator / modified_numerator;
				pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[ 0 ] |= 0x88000000;

				LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_parm() numerator(0d%d) denominator(0d%d) m_nCustomAnalogVideoAvgFrameRateProperty[ 0 ](0x%x)\n", pDevice->m_nKsDeviceNumber, sp->parm.capture.timeperframe.numerator, sp->parm.capture.timeperframe.denominator, pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[ 0 ] );
			}
			else
			{
				pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ 0 ] = sp->parm.capture.timeperframe.denominator / modified_numerator;
				pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ 0 ] |= 0x88000000;

				LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_parm() numerator(0d%d) denominator(0d%d) m_nCustomEncoderVideoAvgFrameRateProperty[ 0 ](0x%x)\n", pDevice->m_nKsDeviceNumber, sp->parm.capture.timeperframe.numerator, sp->parm.capture.timeperframe.denominator, pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ 0 ] );
			}
		}
	}
	else
	{
		LINUXV4L2_DEBUG( KERN_ERR, "[%02d] common_video_device_vidioc_s_parm() error value, numerator(0d%d) denominator(0d%d)\n", pDevice->m_nKsDeviceNumber, sp->parm.capture.timeperframe.numerator, sp->parm.capture.timeperframe.denominator );

		return -EINVAL;
	}

	return 0;
}
static int common_video_device_vidioc_g_parm( struct file * p_file, void * p_priv, struct v4l2_streamparm *sp )
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	#if defined(AME_SC0710) || defined(AME_SC0720)

	SC0710_SYS_CFG * p_sys_cfg = 0;

	#endif

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_g_parm() pVideo == NULL\n" );

		return -EINVAL;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_g_parm() pVideo->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pVideo->m_pDevice;

	sp->parm.capture.capability |= V4L2_CAP_TIMEPERFRAME;

	#if defined(AME_SC0710) || defined(AME_SC0720)

	p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#endif

	if (sp->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	{
		{
			if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
			{
				ULONG ch_quantity = (pDevice->bcdDevice & 0xF);

				sp->parm.capture.timeperframe.denominator = p_sys_cfg->n_input_video_resolution_fpss[pVideo->m_nSubDeviceNumber % ch_quantity] * 1000;

				if( p_sys_cfg->n_input_video_resolution_interleaveds[pVideo->m_nSubDeviceNumber % ch_quantity] == 1 )
				{
					sp->parm.capture.timeperframe.denominator /= 2;
				}

				if(p_sys_cfg->n_input_video_resolution_fps_ms[pVideo->m_nSubDeviceNumber % ch_quantity] == 1)
				{
					sp->parm.capture.timeperframe.numerator = 1001;
				}
				else
				{
					sp->parm.capture.timeperframe.numerator = 1000;
				}
			}
			else
			{
				sp->parm.capture.timeperframe.denominator = p_sys_cfg->n_input_video_resolution_fps * 1000;


				if( p_sys_cfg->n_input_video_resolution_interleaved == 1 )
				{
					#ifndef INTERLACE_FIELD_MODE
					sp->parm.capture.timeperframe.denominator /= 2;
					#endif
				}

				if(p_sys_cfg->n_input_video_resolution_fps_m == 1)
				{
					sp->parm.capture.timeperframe.numerator = 1001;
				}
				else
				{
					sp->parm.capture.timeperframe.numerator = 1000;
				}
			}
		}
	}

	return 0;
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)

static int common_video_device_vidioc_enum_frameintervals(struct file *p_file, void *fh, struct v4l2_frmivalenum *fival)
{
	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	CDevice* pDevice = NULL;

	#if defined(AME_SC0710) || defined(AME_SC0720)

	SC0710_SYS_CFG * p_sys_cfg = 0;

	#endif

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_enum_frameintervals() pVideo == NULL\n" );

		return -EINVAL;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_enum_frameintervals() pVideo->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pVideo->m_pDevice;

	if (fival->index > 4)
		return -EINVAL;

	#if defined(AME_SC0710) || defined(AME_SC0720)

	p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

	#endif

	{
		fival->type = V4L2_FRMIVAL_TYPE_DISCRETE;

		{

			if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
			{
				ULONG ch_quantity = (pDevice->bcdDevice & 0xF);

				if(p_sys_cfg->n_input_video_resolution_fps_ms[pVideo->m_nSubDeviceNumber % ch_quantity] == 1)
				{
					fival->discrete.numerator = 1001;
				}
				else
				{
					fival->discrete.numerator = 1000;
				}
			}
			else
			{
				if(p_sys_cfg->n_input_video_resolution_fps_m == 1)
				{
					fival->discrete.numerator = 1001;
				}
				else
				{
					fival->discrete.numerator = 1000;
				}
			}


			if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
			{
				ULONG ch_quantity = (pDevice->bcdDevice & 0xF);

				if (pDevice->m_nAnalogVideoDecoderStatusPropertys[pVideo->m_nSubDeviceNumber % ch_quantity])
				{
					fival->discrete.denominator = p_sys_cfg->n_input_video_resolution_fpss[pVideo->m_nSubDeviceNumber % ch_quantity] * 1000;
				}
				else
				{
					fival->discrete.denominator = 60 * 1000;
				}

				if( p_sys_cfg->n_input_video_resolution_interleaveds[pVideo->m_nSubDeviceNumber % ch_quantity] == 1 )
				{
					fival->discrete.denominator /= 2;
				}
			}
			else
			{
				if(pDevice->m_nAnalogVideoDecoderStatusProperty)
				{
					fival->discrete.denominator = p_sys_cfg->n_input_video_resolution_fps * 1000;
				}
				else
				{
					fival->discrete.denominator = 60 * 1000;
				}

				if( p_sys_cfg->n_input_video_resolution_interleaved == 1 )
				{
					#ifndef INTERLACE_FIELD_MODE
					fival->discrete.denominator /= 2;
					#endif
				}
			}

			if( fival->index == 1 )
			{
				fival->discrete.denominator /= 2;
			}

			if( fival->index == 2 )
			{
				fival->discrete.denominator /= 4;
			}

			if( fival->index == 3 )
			{
				fival->discrete.denominator /= 16;
			}

			if( fival->index == 4 )
			{
				fival->discrete.denominator =  (fival->discrete.denominator * 10) / 24;

			}
		}
	}



	return 0;

}

struct SCALE_TABLE
{
	ULONG width;
	ULONG height;
};

#ifdef WEBRTC

#define SCALE_SIZE 5
struct SCALE_TABLE scale_tb[SCALE_SIZE] = { {0,0}, {4096, 2160}, {3840, 2160}, {1920, 1200}, {1920, 1080}  };

#else

#define SCALE_SIZE 30
struct SCALE_TABLE scale_tb[SCALE_SIZE] = { {0,0}, {4096, 2160}, {3840, 2160}, {1920, 1200}, {1920, 1080}, {1680, 1050}, {1280, 1024}, {1280, 960}, {1280, 720}, {1024, 768},

											{1024, 576}, {864, 486}, {854, 480}, {800, 600}, {768, 576}, {768, 432}, {720, 576}, {720, 486}, {720, 480}, {720, 405},

											{640, 480}, {640, 360},	{400, 300}, {384, 288}, {384, 216}, {352, 288}, {352, 240}, {320, 288}, {320, 240}, {160, 120} };

#endif


static const struct v4l2_frmsize_discrete frame_sizes[] = {
	{ 8192, 4320 },
	{ 7680, 4320 },
	{ 4096, 2160 },
	{ 3840, 2160 },
	{ 2560, 2048 },
	{ 2048, 2048 },
	{ 1920, 1200 },
	{ 1920, 1080 },
	{ 1600, 1200 },
	{ 1600, 900 },
	{ 1400, 1050 },
	{ 1360, 768 },
	{ 1280, 1024 },
	{ 1280, 960 },
	{ 1280, 768 },
	{ 1280, 720 },
	{ 1024, 768 },
	{ 800, 600 },
	{ 720, 576 },
	{ 720, 480 },
	{ 640, 480 }
};

static int common_video_device_vidioc_enum_framesizes(struct file *p_file, void *fh, struct v4l2_frmsizeenum *fsize)
{
	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	CDevice* pDevice = NULL;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_enum_framesizes() pVideo == NULL\n" );

		return -EINVAL;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_enum_framesizes() pVideo->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pVideo->m_pDevice;

	#ifdef AME_SC0710
	if(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 )
	{
		if( fsize->index > (sizeof(frame_sizes) / sizeof(frame_sizes[0]) - 1) )
			return -EINVAL;
	}
	#endif

	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;

	if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
	{
		#if defined(AME_SC0710) || defined(AME_SC0720)

		SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

		ULONG ch_quantity = (pDevice->bcdDevice & 0xF);

		if (pDevice->m_nAnalogVideoDecoderStatusPropertys[pVideo->m_nSubDeviceNumber % ch_quantity])
		{
			fsize->discrete.width = p_sys_cfg->n_input_video_resolution_cxs[pVideo->m_nSubDeviceNumber % ch_quantity];

			fsize->discrete.height = p_sys_cfg->n_input_video_resolution_cys[pVideo->m_nSubDeviceNumber % ch_quantity];

			if( p_sys_cfg->n_input_video_resolution_interleaveds[pVideo->m_nSubDeviceNumber % ch_quantity] == 1 )
			{
				fsize->discrete.height *= 2;
			}
		}
		else
		{
			fsize->discrete.width = 1920;

			fsize->discrete.height = 1080;
		}
		#endif
	}
	else
	{
		fsize->discrete.width = frame_sizes[fsize->index].width;

		fsize->discrete.height = frame_sizes[fsize->index].height;
	}

	return 0;
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
static int common_video_device_vidioc_cropcap(struct file *p_file, void *fh, struct v4l2_cropcap *cropcap)
{
	CDevice* pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	int type = cropcap->type;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_cropcap() pVideo == NULL\n" );

		return -EINVAL;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_cropcap() pVideo->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pVideo->m_pDevice;

	memset(cropcap, 0, sizeof(*cropcap));

	cropcap->type = type;

	if (cropcap->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;


	cropcap->bounds.top = cropcap->bounds.left = 0;
	cropcap->defrect.top = cropcap->defrect.left = 0;

	if(pDevice->m_nCustomAnalogVideoResolutionProperty)
	{
		cropcap->bounds.width = ( pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;
		cropcap->bounds.height = ( pDevice->m_nCustomAnalogVideoResolutionProperty ) & 0xFFFF;

		if( pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
		{
			cropcap->bounds.height *= 2;
		}


		cropcap->defrect.width = ( pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;
		cropcap->defrect.height = ( pDevice->m_nCustomAnalogVideoResolutionProperty ) & 0xFFFF;
		if( pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
		{
			cropcap->defrect.height *= 2;
		}
	}
	else
	{
		cropcap->bounds.width = pVideo->m_nDstFrameWidth;

		cropcap->bounds.height = pVideo->m_nDstFrameHeight;

		cropcap->defrect.width = pVideo->m_nDstFrameWidth;

		cropcap->defrect.height = pVideo->m_nDstFrameHeight;
	}

	cropcap->pixelaspect.numerator = 1;
	cropcap->pixelaspect.denominator = 1;

	return 0;
}
#endif

#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,1,0)
#define V4L2_PIX_FMT_H264 v4l2_fourcc( 'H', '2', '6', '4' )
#endif

static int common_video_device_vidioc_enum_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_fmtdesc * p_fmtdesc )
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_enum_fmt_cap() pVideo == NULL\n" );

		return -EINVAL;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_enum_fmt_cap() pVideo->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pVideo->m_pDevice;



	if( p_fmtdesc->type == V4L2_BUF_TYPE_VIDEO_CAPTURE ) {

		if( pVideo->m_nSubDeviceType == 0x00000001 ) {

			if( p_fmtdesc->index == 0 ) {

				if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 ) { sprintf( p_fmtdesc->description, "%s", "YUYV 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUYV;	}

				return 0;
			}

			#ifndef WEBRTC //ndef

			if( p_fmtdesc->index == 1 )
			{
				if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 )
				{
					sprintf( p_fmtdesc->description, "%s", "NV12 4:2:0" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_NV12;

					return 0;
				}
			}
			if( p_fmtdesc->index == 2 )
			{
				if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 )
				{
					sprintf( p_fmtdesc->description, "%s", "YV12 4:2:0" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YVU420;

					return 0;
				}

			}
			if( p_fmtdesc->index == 3 )
			{

				if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 )
				{
					sprintf( p_fmtdesc->description, "%s", "I420 4:2:0" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_YUV420;

					return 0;
				}

			}
			if( p_fmtdesc->index == 4 )
			{
				if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 )
				{
					sprintf( p_fmtdesc->description, "%s", "RGB24 4:4:4" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_RGB24;

					return 0;
				}
			}
			if( p_fmtdesc->index == 5 )
			{
				if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 )
				{
					sprintf( p_fmtdesc->description, "%s", "BGR24 4:4:4" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_BGR24;

					return 0;
				}
			}
			if( p_fmtdesc->index == 6 )
			{
				if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 )
				{
					sprintf( p_fmtdesc->description, "%s", "UYVY 4:2:2" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_UYVY;

					return 0;
				}
			}
			#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,0,0)
			if( p_fmtdesc->index == 7 )
			{
				if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0701))
				{
					sprintf( p_fmtdesc->description, "%s", "P010 10bit 4:2:0" ); p_fmtdesc->pixelformat = V4L2_PIX_FMT_P010;

					return 0;
				}
			}
			if (p_fmtdesc->index == 8)
			{
				if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0701))
				{
					sprintf(p_fmtdesc->description, "%s", "P210 10bit 4:2:2"); p_fmtdesc->pixelformat = V4L2_PIX_FMT_P210;

					return 0;
				}
			}
			if (p_fmtdesc->index == 9)
			{
				if (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710)
				{
					sprintf(p_fmtdesc->description, "%s", "M420 8bit 4:2:0"); p_fmtdesc->pixelformat = V4L2_PIX_FMT_M420;

					return 0;
				}
			}
			#endif

			#endif

		}
	}
	return -EINVAL;
}


static int common_video_device_vidioc_try_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_format * p_fmt )
{
	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	CDevice* pDevice = NULL;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_try_fmt_cap() pVideo == NULL\n" );

		return -EINVAL;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_try_fmt_cap() pVideo->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice  = pVideo->m_pDevice;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_try_fmt_cap() m_nSubDeviceNumber(%d) ( T:%02d, F:%02d )\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber, p_fmt->type, p_fmt->fmt.pix.field );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_try_fmt_cap() m_nSubDeviceNumber(%d) width(%d) height(%d) pixelformat(%x)\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber, p_fmt->fmt.pix.width, p_fmt->fmt.pix.height, p_fmt->fmt.pix.pixelformat );

	if( p_fmt->type == V4L2_BUF_TYPE_VIDEO_CAPTURE ) {

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] try_fmt- %08X:%03dX%03d:%d:%d, field(%d)\n", pDevice->m_nKsDeviceNumber, p_fmt->fmt.pix.pixelformat, p_fmt->fmt.pix.width, p_fmt->fmt.pix.height, p_fmt->fmt.pix.bytesperline, p_fmt->fmt.pix.sizeimage, p_fmt->fmt.pix.field );

		if( p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_UYVY ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420  ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_GREY  ||

			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YUV420  ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_M420  ||

			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_P010 ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_P016 ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_P210 ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_NV12 ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_RGB24 ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_BGR24 ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_PLANAR_BGR ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_PLANAR_RGB ||
			p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_H264 ) {

			if( p_fmt->fmt.pix.field == V4L2_FIELD_ANY ) { p_fmt->fmt.pix.field = V4L2_FIELD_INTERLACED; }

			if( (p_fmt->fmt.pix.width > 7680) || (p_fmt->fmt.pix.height > 4320) )
			{
				LINUXV4L2_PRINT( KERN_INFO, "[%02d] common_video_device_vidioc_try_fmt_cap too large, error, width(%d) height(%d)\n", pDevice->m_nKsDeviceNumber, p_fmt->fmt.pix.width, p_fmt->fmt.pix.height );

				return -EINVAL;
			}

			if((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720))
			{

					if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
					{
						#if defined(AME_SC0710) || defined(AME_SC0720)
						SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

						ULONG ch_quantity = (pDevice->bcdDevice & 0xF);

						if (pDevice->m_nAnalogVideoDecoderStatusPropertys[pVideo->m_nSubDeviceNumber % ch_quantity])
						{
							p_fmt->fmt.pix.width = p_sys_cfg->n_input_video_resolution_cxs[pVideo->m_nSubDeviceNumber % ch_quantity];

							p_fmt->fmt.pix.height = p_sys_cfg->n_input_video_resolution_cys[pVideo->m_nSubDeviceNumber % ch_quantity];

							if (pVideo->m_nCustomAnalogVideoVbiLinesEnableProperty)
							{
								p_fmt->fmt.pix.height += pVideo->m_nCustomAnalogVideoVbiLinesProperty;
							}

							if( p_sys_cfg->n_input_video_resolution_interleaveds[pVideo->m_nSubDeviceNumber % ch_quantity] == 1 )
							{
								p_fmt->fmt.pix.height *= 2;
							}
						}
						else
						{
							p_fmt->fmt.pix.width = 1920;

							p_fmt->fmt.pix.height = 1080;
						}
						#endif
					}
					else
					{
						if( pDevice->m_nAnalogVideoDecoderStatusProperty)
						{
							p_fmt->fmt.pix.width = ( pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;

							p_fmt->fmt.pix.height = ( pDevice->m_nCustomAnalogVideoResolutionProperty ) & 0xFFFF;

							if(pVideo->m_nCustomAnalogVideoVbiLinesEnableProperty )
							{
								p_fmt->fmt.pix.height += pVideo->m_nCustomAnalogVideoVbiLinesProperty;
							}

							if( pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
							{
								#ifndef INTERLACE_FIELD_MODE
								p_fmt->fmt.pix.height *= 2;
								#endif
							}
						}
						else
						{
							p_fmt->fmt.pix.width = 1920;

							p_fmt->fmt.pix.height = 1080;
						}
					}

				p_fmt->fmt.pix.field = V4L2_FIELD_NONE;
			}

			if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) {

				if( p_fmt->fmt.pix.width  <  320 ) { p_fmt->fmt.pix.width  =  320; }

				if (p_fmt->fmt.pix.height < 60) { p_fmt->fmt.pix.height = 60; }

			}
			if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) {

				if( p_fmt->fmt.pix.width  <  320 ) { p_fmt->fmt.pix.width  =  320; }

				if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) && (pDevice->m_nAnalogCrossbarVideoInputProperty <= 4) )
				{
					if (p_fmt->fmt.pix.height < 60) { p_fmt->fmt.pix.height = 60; }
				}
				else
				{
					if( p_fmt->fmt.pix.height <  288 ) { p_fmt->fmt.pix.height =  288; }
				}
			}

			if( p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420  || p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YUV420 || p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_NV12 || p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_M420) {

				p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width );
				p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 12) >> 3;
			}
			else if( (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_RGB24) || (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_BGR24) || (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_PLANAR_BGR) || (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_PLANAR_RGB))
			{
				if( (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_PLANAR_BGR) || (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_PLANAR_RGB))
				{
					p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width);
					p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 3);
					p_fmt->fmt.pix.width =  ( pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;
				}
				else
				{
					p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width) * 3;
					p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height);
				}
			}
			else if( p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_P010 )
			{
				#ifdef P010_PACKED
				{
					p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width * 10) >> 3;
					p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 12) >> 3;
				}
				#else
				{
					p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width * 2);
					p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 12) >> 3;
				}
				#endif
			}
			else if( p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_P016 )
			{
				p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width * 2);
				p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 12) >> 3;
			}
			else if( p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_P210 )
			{
				#ifdef SC700_P210_PACKED
					p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width / 6) * 8;
					p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 2);
				#else
					p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width * 2);
					p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 2);
				#endif
			}
			else {

				p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width * 2);
				p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height);
			}

			if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) {

				if( p_fmt->fmt.pix.width >= 768 && p_fmt->fmt.pix.height >= 480 ) { pVideo->m_nSrcFrameWidth = 768; pVideo->m_nSrcFrameHeight = 480; } else

				if( p_fmt->fmt.pix.width >= 720 && p_fmt->fmt.pix.height >= 480 ) { pVideo->m_nSrcFrameWidth = 720; pVideo->m_nSrcFrameHeight = 480; } else

				if( p_fmt->fmt.pix.width >= 720 && p_fmt->fmt.pix.height <  480 ) { pVideo->m_nSrcFrameWidth = 720; pVideo->m_nSrcFrameHeight = 240; } else

				if( p_fmt->fmt.pix.width >= 704 && p_fmt->fmt.pix.height >= 480 ) { pVideo->m_nSrcFrameWidth = 704; pVideo->m_nSrcFrameHeight = 480; } else

				if( p_fmt->fmt.pix.width >= 704 && p_fmt->fmt.pix.height <  480 ) { pVideo->m_nSrcFrameWidth = 704; pVideo->m_nSrcFrameHeight = 240; } else

				if( p_fmt->fmt.pix.width >= 640 && p_fmt->fmt.pix.height >= 480 ) { pVideo->m_nSrcFrameWidth = 640; pVideo->m_nSrcFrameHeight = 480; } else

				if( p_fmt->fmt.pix.width >= 640 && p_fmt->fmt.pix.height <  480 ) { pVideo->m_nSrcFrameWidth = 640; pVideo->m_nSrcFrameHeight = 240; } else

				if( p_fmt->fmt.pix.width >= 384 && p_fmt->fmt.pix.height >= 240 ) { pVideo->m_nSrcFrameWidth = 384; pVideo->m_nSrcFrameHeight = 240; } else

				if( p_fmt->fmt.pix.width >= 360 && p_fmt->fmt.pix.height >= 240 ) { pVideo->m_nSrcFrameWidth = 360; pVideo->m_nSrcFrameHeight = 240; } else

				if( p_fmt->fmt.pix.width >= 352 && p_fmt->fmt.pix.height >= 240 ) { pVideo->m_nSrcFrameWidth = 352; pVideo->m_nSrcFrameHeight = 240; } else

				if( p_fmt->fmt.pix.width >= 320 && p_fmt->fmt.pix.height >= 240 ) { pVideo->m_nSrcFrameWidth = 320; pVideo->m_nSrcFrameHeight = 240; }
			}
			if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_625_50 ) {

				if( p_fmt->fmt.pix.width >= 768 && p_fmt->fmt.pix.height >= 576 ) { pVideo->m_nSrcFrameWidth = 768; pVideo->m_nSrcFrameHeight = 576; } else

				if( p_fmt->fmt.pix.width >= 720 && p_fmt->fmt.pix.height >= 576 ) { pVideo->m_nSrcFrameWidth = 720; pVideo->m_nSrcFrameHeight = 576; } else

				if( p_fmt->fmt.pix.width >= 720 && p_fmt->fmt.pix.height <  576 ) { pVideo->m_nSrcFrameWidth = 720; pVideo->m_nSrcFrameHeight = 288; } else

				if( p_fmt->fmt.pix.width >= 704 && p_fmt->fmt.pix.height >= 576 ) { pVideo->m_nSrcFrameWidth = 704; pVideo->m_nSrcFrameHeight = 576; } else

				if( p_fmt->fmt.pix.width >= 704 && p_fmt->fmt.pix.height <  576 ) { pVideo->m_nSrcFrameWidth = 704; pVideo->m_nSrcFrameHeight = 288; } else

				if( p_fmt->fmt.pix.width >= 640 && p_fmt->fmt.pix.height >= 576 ) { pVideo->m_nSrcFrameWidth = 640; pVideo->m_nSrcFrameHeight = 576; } else

				if( p_fmt->fmt.pix.width >= 640 && p_fmt->fmt.pix.height <  576 ) { pVideo->m_nSrcFrameWidth = 640; pVideo->m_nSrcFrameHeight = 288; } else

				if( p_fmt->fmt.pix.width >= 384 && p_fmt->fmt.pix.height >= 288 ) { pVideo->m_nSrcFrameWidth = 384; pVideo->m_nSrcFrameHeight = 288; } else

				if( p_fmt->fmt.pix.width >= 360 && p_fmt->fmt.pix.height >= 288 ) { pVideo->m_nSrcFrameWidth = 360; pVideo->m_nSrcFrameHeight = 288; } else

				if( p_fmt->fmt.pix.width >= 352 && p_fmt->fmt.pix.height >= 288 ) { pVideo->m_nSrcFrameWidth = 352; pVideo->m_nSrcFrameHeight = 288; } else

				if( p_fmt->fmt.pix.width >= 320 && p_fmt->fmt.pix.height >= 288 ) { pVideo->m_nSrcFrameWidth = 320; pVideo->m_nSrcFrameHeight = 288; }
			}
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] try_fmt-end %08X:%03dX%03d:%d:%d, field(%d)\n", pDevice->m_nKsDeviceNumber, p_fmt->fmt.pix.pixelformat, p_fmt->fmt.pix.width, p_fmt->fmt.pix.height, p_fmt->fmt.pix.bytesperline, p_fmt->fmt.pix.sizeimage, p_fmt->fmt.pix.field );

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] - RESET!!\n", pDevice->m_nKsDeviceNumber);
			return 0;

		}

	}
    return -EINVAL;
}

static int common_video_device_vidioc_s_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_format * p_fmt )
{
	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	int ret = -EINVAL;

	CDevice* pDevice = NULL;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_fmt_cap() pVideo == NULL\n" );

		return -EINVAL;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_fmt_cap() pVideo->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pVideo->m_pDevice;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_fmt_cap( T:%02d )\n", pDevice->m_nKsDeviceNumber, p_fmt->type );

	{
		if( p_fmt->type == V4L2_BUF_TYPE_VIDEO_CAPTURE ) {

			ret = common_video_device_vidioc_try_fmt_cap( p_file, p_priv, p_fmt );

			if( ret != 0 ) { return ret; }

			LINUXV4L2_PRINT( KERN_INFO, "[%02d] s_fmt- %08X:%03dX%03d:%d:%d, field(%d) m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, p_fmt->fmt.pix.pixelformat, p_fmt->fmt.pix.width, p_fmt->fmt.pix.height, p_fmt->fmt.pix.bytesperline, p_fmt->fmt.pix.sizeimage, p_fmt->fmt.pix.field, pVideo->m_nSubDeviceNumber );

			if( p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV ||
				p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_UYVY ||
				p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420  ||
				p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_GREY  ||

				p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YUV420  ||
				p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_M420  ||

				p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_P010 ||
				p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_P016 ||
				p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_P210 ||
				p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_NV12 ||
				p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_RGB24 ||
				p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_BGR24 ||
				p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_PLANAR_BGR ||
				p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_PLANAR_RGB ||
				p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_H264 ) {

				pVideo->m_nDstFrameWidth    = p_fmt->fmt.pix.width;
				pVideo->m_nDstFrameHeight   = p_fmt->fmt.pix.height;
				pVideo->m_nDstFramePitch    = p_fmt->fmt.pix.bytesperline;
				pVideo->m_nDstFrameSize     = p_fmt->fmt.pix.sizeimage;
				pVideo->m_nFramePixelFormat = p_fmt->fmt.pix.pixelformat;
				pVideo->m_nFramePixelField  = p_fmt->fmt.pix.field;

				pVideo->m_nDstFrameInfoSet  = 1;

				LINUXV4L2_PRINT( KERN_INFO, "[%02d] MATCH!! (SRC:%d:%d) (DST:%d:%d) (SIZE: %d)m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSrcFrameWidth, pVideo->m_nSrcFrameHeight, pVideo->m_nDstFrameWidth, pVideo->m_nDstFrameHeight, pVideo->m_nDstFrameSize, pVideo->m_nSubDeviceNumber );

				if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

					(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

					(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

					(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

					(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

					(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

					(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) ) {

					int index = 0;
					if( ((pDevice->iProduct & 0x0F) == 0x02) || ((pDevice->iProduct & 0x0F) == 0x03)  ) {

						if( (pDevice->iManufacturer & 0x0F) == 0x0A ) {


							index = ((pVideo->m_nSubDeviceNumber < 8) ? pVideo->m_nSubDeviceNumber: pVideo->m_nSubDeviceNumber + 8);
						}
						else {

							index = ((pVideo->m_nSubDeviceNumber < 4) ? pVideo->m_nSubDeviceNumber: pVideo->m_nSubDeviceNumber + 4);
						}
					}

					if( pVideo->m_nSubDeviceType == 0x00000001 ) {

						pDevice->m_nCustomPreviewVideoResolutionProperty[ index ]  =   0x88000000 | (pVideo->m_nDstFrameWidth) << 12 | (pVideo->m_nDstFrameHeight);

					}

					if( pVideo->m_nSubDeviceType == 0x00000002)
					{
						pDevice->m_nCustomEncoderVideoResolutionProperty[ index ]  =   0x88000000 | (pVideo->m_nDstFrameWidth) << 12 | (pVideo->m_nDstFrameHeight);

						if( pVideo->m_nS_Parm_Set == 0 )
						{
							ULONG fps = pDevice->m_nCustomAnalogVideoFrameRateProperty;


							if( fps )
							{
								if(pDevice->m_nCustomAnalogVideoInterleavedProperty )
								{
									fps /= 2;
								}
							}
							else
							{
								LINUXV4L2_PRINT( KERN_ERR, "[%02d] common_video_device_vidioc_s_fmt_cap() fps 0\n", pDevice->m_nKsDeviceNumber);
							}

							pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ index ] = fps;

							if(pDevice->m_nAnalogCaptureStreamPowerReference != 0 )
							{
								pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ index ] |= 0x88000000;
							}

							LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_s_fmt_cap() m_nCustomEncoderVideoAvgFrameRateProperty[ %d ](0x%x) h264\n", pDevice->m_nKsDeviceNumber, index, pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ index ] );
						}
					}
				}


				return 0;
			}
		}
	}

	return -EINVAL;
}

static int common_video_device_vidioc_g_fmt_cap( struct file * p_file, void * p_priv, struct v4l2_format * p_fmt )
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_g_fmt_cap() pVideo == NULL\n" );

		return -EINVAL;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_g_fmt_cap() pVideo->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pVideo->m_pDevice;

	if( p_fmt->type == V4L2_BUF_TYPE_VIDEO_CAPTURE ) {


		{
			if( pVideo->m_nDstFrameInfoSet == 0 )
			{
				if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
				{
					#if defined(AME_SC0710) || defined(AME_SC0720)

					ULONG ch_quantity = (pDevice->bcdDevice & 0xF);

					if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720))
					{
						SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

						pVideo->m_nDstFrameWidth = p_sys_cfg->n_input_video_resolution_cxs[pVideo->m_nSubDeviceNumber % ch_quantity];

						pVideo->m_nDstFrameHeight = p_sys_cfg->n_input_video_resolution_cys[pVideo->m_nSubDeviceNumber % ch_quantity];

						if (p_sys_cfg->n_input_video_resolution_interleaveds[pVideo->m_nSubDeviceNumber % ch_quantity] == 1)
						{
							pVideo->m_nDstFrameHeight *= 2;
						}
					}
					#endif
				}
				else
				{
					if( pDevice->m_nCustomAnalogVideoResolutionProperty != 0 )
					{
						pVideo->m_nDstFrameWidth                  = ( pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;


						pVideo->m_nDstFrameHeight                 = ( pDevice->m_nCustomAnalogVideoResolutionProperty ) & 0xFFFF;

						if( pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
						{
							pVideo->m_nDstFrameHeight *= 2;
						}
					}
				}

				pVideo->m_nDstFramePitch                  = pVideo->m_nDstFrameWidth;


				#if defined(AME_SC0700) || defined(AME_SC0710) || defined(AME_SC0720)
				pVideo->m_nDstFrameSize                   = pVideo->m_nDstFrameWidth * pVideo->m_nDstFrameHeight * 2;
				#else
				pVideo->m_nDstFrameSize                   = pVideo->m_nDstFrameWidth * pVideo->m_nDstFrameHeight * 12 >> 3;
				#endif
			}
		}


		p_fmt->fmt.pix.colorspace   = V4L2_COLORSPACE_SMPTE170M;
		p_fmt->fmt.pix.width        = pVideo->m_nDstFrameWidth;
		p_fmt->fmt.pix.height       = pVideo->m_nDstFrameHeight;
		p_fmt->fmt.pix.bytesperline = pVideo->m_nDstFramePitch;
		p_fmt->fmt.pix.sizeimage    = pVideo->m_nDstFrameSize;
		p_fmt->fmt.pix.pixelformat  = pVideo->m_nFramePixelFormat;
		p_fmt->fmt.pix.field        = pVideo->m_nFramePixelField;
		p_fmt->fmt.pix.priv         = 0;

		if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0700) ||
			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0701) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) ) {


			{
				if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
				{
					#if defined(AME_SC0710) || defined(AME_SC0720)

					ULONG ch_quantity = (pDevice->bcdDevice & 0xF);

					if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720))
					{
						SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

						p_fmt->fmt.pix.width = p_sys_cfg->n_input_video_resolution_cxs[pVideo->m_nSubDeviceNumber % ch_quantity];

						p_fmt->fmt.pix.height = p_sys_cfg->n_input_video_resolution_cys[pVideo->m_nSubDeviceNumber % ch_quantity];

						if (p_sys_cfg->n_input_video_resolution_interleaveds[pVideo->m_nSubDeviceNumber % ch_quantity] == 1)
						{
							p_fmt->fmt.pix.height *= 2;

							p_fmt->fmt.pix.field = V4L2_FIELD_INTERLACED;
						}
						else
						{
							p_fmt->fmt.pix.field = V4L2_FIELD_NONE;
						}
					}
					#endif
				}
				else
				{
					if(pDevice->m_nCustomAnalogVideoResolutionProperty != 0)
					{
						p_fmt->fmt.pix.width        = ( pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;

						p_fmt->fmt.pix.height       = ( pDevice->m_nCustomAnalogVideoResolutionProperty ) & 0xFFFF;

						if( pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
						{
							#ifndef INTERLACE_FIELD_MODE
							p_fmt->fmt.pix.height *= 2;
							#endif

							#if defined(AME_SC0700) || defined(AME_SC0710) || defined(AME_SC0720)
							{
								SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

								if (p_sys_cfg->n_input_video_resolution_psf)
								{
									p_fmt->fmt.pix.field = V4L2_FIELD_INTERLACED_TB;
								}
								else
								{
									p_fmt->fmt.pix.field = V4L2_FIELD_INTERLACED;
								}
							}
							#else
								p_fmt->fmt.pix.field = V4L2_FIELD_INTERLACED;
							#endif
						}
						else
						{

							p_fmt->fmt.pix.field        = V4L2_FIELD_NONE;
						}

						#ifdef REPORT_4k_RESOLUTION
						p_fmt->fmt.pix.width = 3840;
						p_fmt->fmt.pix.height = 2160;
						p_fmt->fmt.pix.field = V4L2_FIELD_NONE;
						#endif
					}
				}

				#if defined(AME_SC0710) || defined(AME_SC0720)
				if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
				{
					SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);


					if( p_sys_cfg->n_input_video_resolution_color_colorimetry == 2 )
					{
						p_fmt->fmt.pix.colorspace   = V4L2_COLORSPACE_REC709;
					}
					else if( p_sys_cfg->n_input_video_resolution_color_colorimetry == 3 )
					{
						#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0)
						p_fmt->fmt.pix.colorspace   = V4L2_COLORSPACE_BT2020;
						#else
						p_fmt->fmt.pix.colorspace   = V4L2_COLORSPACE_REC709;
						#endif
					}
					else
					{
						p_fmt->fmt.pix.colorspace   = V4L2_COLORSPACE_SMPTE170M;
					}
				}
				#else
				if(p_fmt->fmt.pix.width >= 3840)
				{

					#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0)
					p_fmt->fmt.pix.colorspace   = V4L2_COLORSPACE_BT2020;
					#else
					p_fmt->fmt.pix.colorspace   = V4L2_COLORSPACE_REC709;
					#endif
				}
				else if(p_fmt->fmt.pix.width >= 1280)
				{
					p_fmt->fmt.pix.colorspace   = V4L2_COLORSPACE_REC709;
				}
				else
				{
					p_fmt->fmt.pix.colorspace   = V4L2_COLORSPACE_SMPTE170M;
				}
				#endif

				if( (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV) || (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_UYVY) )
				{
					p_fmt->fmt.pix.bytesperline = p_fmt->fmt.pix.width * 2;

					p_fmt->fmt.pix.sizeimage    = p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height;
				}
				else if( p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_P010 )
				{
					#ifdef P010_PACKED
					{
						p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width * 10) >> 3;

						p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 12) >> 3;
					}
					#else
					{
						p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width * 2);

						p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 12) >> 3;
					}
					#endif
				}
				else if( p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_P016 )
				{
					p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width * 2);

					p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 12) >> 3;
				}
				else if( p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_P210 )
				{
					#ifdef SC700_P210_PACKED
						p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width / 6) * 8;
						p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 2);
					#else

						p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width * 2);
						p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 2);
					#endif
				}
				else if( (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_RGB24) || (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_BGR24) || (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_PLANAR_BGR) || (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_PLANAR_RGB))
				{




					if( (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_PLANAR_BGR) || (p_fmt->fmt.pix.pixelformat == V4L2_PIX_FMT_PLANAR_RGB))
					{
						p_fmt->fmt.pix.bytesperline = p_fmt->fmt.pix.width;

						p_fmt->fmt.pix.sizeimage    = p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 3;

						p_fmt->fmt.pix.width =  ( pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;
					}
					else
					{
						p_fmt->fmt.pix.bytesperline = p_fmt->fmt.pix.width * 3;

						p_fmt->fmt.pix.sizeimage    = p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height;
					}
				}
				else
				{
					p_fmt->fmt.pix.bytesperline = (p_fmt->fmt.pix.width );

					p_fmt->fmt.pix.sizeimage = (p_fmt->fmt.pix.bytesperline * p_fmt->fmt.pix.height * 12) >> 3;
				}
			}

			if( pVideo->m_nSubDeviceType == 0x00000001)
			{
			}
			else
			{
				p_fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_H264;
			}
		}


		#ifdef FFMPEG_V1_1_5
		p_fmt->fmt.pix.width = 0;
		p_fmt->fmt.pix.height = 0;
		#endif






		return 0;
	}
    return -EINVAL;
}
static int common_video_device_vidioc_querymenu( struct file * p_file, void * p_priv, struct v4l2_querymenu *a)
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_querymenu() pVideo == NULL\n" );

		return 0;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_querymenu() pVideo->m_pDevice == NULL\n" );

		return 0;
	}

	pDevice = pVideo->m_pDevice;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_querymenu( %08X )\n", pDevice->m_nKsDeviceNumber, a->id );

	return 0;
}

static int common_video_device_vidioc_queryctrl( struct file * p_file, void * p_priv, struct v4l2_queryctrl * p_queryctrl )
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	ULONG i = 0;

	ULONG j = 0;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_queryctrl() pVideo == NULL\n" );

		return 0;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_queryctrl() pVideo->m_pDevice == NULL\n" );

		return 0;
	}
	pDevice = pVideo->m_pDevice;



	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ) { j =  0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6802) ) { j =  1; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ) { j =  2; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6805) ) { j =  3; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ) { j =  4; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ) { j =  5; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ) { j =  6; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) { j =  7; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6814) ) { j =  8; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6815) ) { j =  9; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6816) ) { j = 10; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6817) ) { j = 11; }
	if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x8210) ) { j = 12; }
	if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x5851) ) { j = 13; }
	if( (pDevice->m_pKsDevice->vendor == 0x1131 && pDevice->m_pKsDevice->device == 0x7160) ) { j = 14; }





	if( (pDevice->m_pKsDevice->vendor == 0x1999 && pDevice->m_pKsDevice->device == 0xA04C) ) { j = 15; }
	if( (pDevice->m_pKsDevice->vendor == 0x3086 && pDevice->m_pKsDevice->device == 0x0101) ) { j = 16; }

	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) ) { j = 17; }
	if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ) { j = 18; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ) { j = 19; }
	if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) ) { j = 20; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0700) ) { j = 21; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0701) ) { j = 22; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) ) { j = 23; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) ) { j = 24; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) ) { j = 25; }

	for( i = 0 ; i < ARRAY_SIZE(LINUXV4L2_V4L2_QUERYCTRL); i++ ) {

		if( p_queryctrl->id == LINUXV4L2_V4L2_QUERYCTRL[ i ].id ) {

			if( LINUXV4L2_V4L2_QUERYCTRL_ENABLE[ j ][ i + 2 ] == 1 ) {

				memcpy( p_queryctrl, &(LINUXV4L2_V4L2_QUERYCTRL[ i ]), sizeof(struct v4l2_queryctrl) );

				p_queryctrl->default_value = LINUXV4L2_V4L2_QUERYCTRL_DEFAULT[ j ][ i + 2 ];

				return 0;
			}
		}
	}
    return -EINVAL;
}

NTSTATUS OnSetCustomDebugEepromPidVidProperty(  CVideo * pVideo, ULONG pPropertyValue );
NTSTATUS OnGetCustomDebugEepromPidVidProperty( CVideo * pVideo, ULONG* pPropertyValue );

static int common_video_device_vidioc_g_ctrl( struct file * p_file, void * p_priv, struct v4l2_control * p_control )
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	ULONG i = 0;

	ULONG j = 0;

	ULONG ch_quantity = 1;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_g_ctrl() pVideo == NULL\n" );

		return 0;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_g_ctrl() pVideo->m_pDevice == NULL\n" );

		return 0;
	}

	pDevice = pVideo->m_pDevice;

	if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
	{
		ch_quantity = (pDevice->bcdDevice & 0xF);
	}




	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6802) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6805) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6814) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6815) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6816) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6817) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x8210) ) { i = pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x5851) ) { i = pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pDevice->m_pKsDevice->vendor == 0x1131 && pDevice->m_pKsDevice->device == 0x7160) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1999 && pDevice->m_pKsDevice->device == 0xA04C) ) { i = pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pDevice->m_pKsDevice->vendor == 0x3086 && pDevice->m_pKsDevice->device == 0x0101) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) ) { i = pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0700) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0701) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }

	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6802) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6805) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6814) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6815) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6816) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6817) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x8210) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x5851) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1131 && pDevice->m_pKsDevice->device == 0x7160) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1999 && pDevice->m_pKsDevice->device == 0xA04C) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x3086 && pDevice->m_pKsDevice->device == 0x0101) ) { j = pVideo->m_nSubDeviceNumber % 4; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0700) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0701) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) ) { j = pVideo->m_nSubDeviceNumber % ch_quantity; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) ) { j = pVideo->m_nSubDeviceNumber % ch_quantity; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) ) { j = pVideo->m_nSubDeviceNumber % ch_quantity; }

	if( p_control->id == V4L2_CID_BRIGHTNESS ) {

		p_control->value = g_n_analog_decoder_brightness[ i ][ j ] & 0xF7FFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_CONTRAST ) {

		p_control->value = g_n_analog_decoder_contrast[ i ][ j ] & 0xF7FFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_SATURATION ) {

		p_control->value = g_n_analog_decoder_saturation[ i ][ j ] & 0xF7FFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_HUE ) {

		p_control->value = g_n_analog_decoder_hue[ i ][ j ] & 0xF7FFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_SHARPNESS ) {

		p_control->value = g_n_analog_decoder_sharpness[ i ][ j ] & 0xF7FFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_AUTOGAIN ) {

		p_control->value = g_n_analog_decoder_gain_auto[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_GAIN ) {

		p_control->value = g_n_analog_decoder_gain[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if (p_control->id == V4L2_CID_MIN_BUFFERS_FOR_CAPTURE) {

		p_control->value = 4;

		LINUXV4L2_PRINT( KERN_INFO, "[%02d] V4L2_CID_MIN_BUFFERS_FOR_CAPTURE( %08X )\n", pDevice->m_nKsDeviceNumber, p_control->value);

		return 0;
	}
	if( p_control->id == V4L2_CID_SWITCH_SPEED ) {

		p_control->value = pVideo->m_nSwitchSpeed_Spliter & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_SWITCH_TABLE ) {

		p_control->value = 0x00000000;

		for( i = 0 ; i < 12 ; i++ ) {

			p_control->value |= (ULONG)(pVideo->m_pSwitchTable_Spliter[ i ] & 0x03) << (i * 2);
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_MOTION_STATUS ) {

		p_control->value = g_n_multiplexer_motion_status[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_SENSITIVITY_LOW_THRESHOLD ) {

		p_control->value = g_n_multiplexer_motion_sensitivity_low_threshold[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_SENSITIVITY_HIGH_THRESHOLD ) {

		p_control->value = g_n_multiplexer_motion_sensitivity_high_threshold[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_MASK_CX ) {

		p_control->value = g_n_multiplexer_motion_mask_cx[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_MASK_CY ) {

		p_control->value = g_n_multiplexer_motion_mask_cy[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_OSD_POS_X ) {

		p_control->value = g_n_encoder_osd_pos_x[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_OSD_POS_Y ) {

		p_control->value = g_n_encoder_osd_pos_y[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_OSD_COLOR ) {

		p_control->value = g_n_encoder_osd_color[ i ] & 0x7FFFFFFF;

		return 0;
	}
	if( p_control->id == V4L2_CID_GPIO_DIRECTION ) {




























		if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) )
		{
			if( pDevice->iManufacturer == 0x51 ||

				 pDevice->iManufacturer == 0xC1 ||

				 pDevice->iManufacturer == 0x52 ||

				 pDevice->iManufacturer == 0xC2 ) {

				g_n_gpio_direction[ i ] = (WORD)(pDevice->m_nCustomGpioDirectionProperty & 0xFFFF);
			}
		}



		p_control->value = g_n_gpio_direction[ i ];

		return 0;
	}
	if( p_control->id == V4L2_CID_GPIO_DATA ) {

		p_control->value = g_n_gpio_data[ i ];

		return 0;
	}

	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_00 ) {

		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) )
		{
			p_control->value = (pDevice->m_nCustomMotionStatusProperty[pVideo->m_nSubDeviceNumber][ 0 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_01 ) {

		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) )
		{

			p_control->value = (pDevice->m_nCustomMotionStatusProperty[pVideo->m_nSubDeviceNumber][ 1 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_02 ) {

		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) )
		{

			p_control->value = (pDevice->m_nCustomMotionStatusProperty[pVideo->m_nSubDeviceNumber][ 2 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_03 ) {

		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) )
		{

			p_control->value = (pDevice->m_nCustomMotionStatusProperty[pVideo->m_nSubDeviceNumber][ 3 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_04 ) {

		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) )
		{

			p_control->value = (pDevice->m_nCustomMotionStatusProperty[pVideo->m_nSubDeviceNumber][ 4 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_05 ) {

		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) )
		{

			p_control->value = (pDevice->m_nCustomMotionStatusProperty[pVideo->m_nSubDeviceNumber][ 5 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_06 ) {

		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) )
		{

			p_control->value = (pDevice->m_nCustomMotionStatusProperty[pVideo->m_nSubDeviceNumber][ 6 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_STATUS_07 ) {

		if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) )
		{

			p_control->value = (pDevice->m_nCustomMotionStatusProperty[pVideo->m_nSubDeviceNumber][ 7 ]) & 0xFFFFFFFF;
		}

		return 0;

	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_TEMPORAL_SENSITIVITY ) {

		if( pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864 )
		{

			p_control->value = pDevice->m_nCustomMotionTemporalSensitivityProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF;

		}
		return 0;
	}

	if( p_control->id == V4L2_CID_TW5864_MOTION_SPATIAL_SENSITIVITY ) {

		if( pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864 )
		{

			p_control->value = pDevice->m_nCustomMotionSpatialSensitivityProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF;

		}
		return 0;
	}

	if( p_control->id == V4L2_CID_TW5864_MOTION_LEVEL_SENSITIVITY ) {

		if( pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864 )
		{

			p_control->value = pDevice->m_nCustomMotionLevelSensitivityProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF;

		}
		return 0;
	}

	if( p_control->id == V4L2_CID_TW5864_MOTION_SPEED ) {

		if( pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864 )
		{

			p_control->value = pDevice->m_nCustomMotionSpeedProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF;

		}
		return 0;
	}

	if( p_control->id == V4L2_CID_FRAME_RATE ) {

		p_control->value = pDevice->m_nCustomAnalogVideoFrameRateProperty & 0x77FFFFFF;


		if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) )
		{
			if( pVideo->m_nSubDeviceType == 0x00000001)
			{
				p_control->value = pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[ 0 ];
			}
		}

		if( pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001 )
		{
			p_control->value = pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[ 0 ];
		}

		return 0;
	}

	if( p_control->id == V4L2_CID_WATCH_DOG ) {

		p_control->value = pDevice->m_nCustomSoftwareWatchdogResetProperty & 0x77FFFFFF;

		return 0;
	}

	if( p_control->id == V4L2_CID_X_RES_DETECTED ) {

		p_control->value = ( pDevice->m_nCustomAnalogVideoResolutionProperty >> 16 ) & 0xFFFF;

		return 0;
	}

	if( p_control->id == V4L2_CID_Y_RES_DETECTED ) {

		p_control->value = ( pDevice->m_nCustomAnalogVideoResolutionProperty ) & 0xFFFF;

		if( pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
		{
			p_control->value *= 2;
		}

		return 0;
	}

	if( p_control->id == V4L2_CID_FPS_DETECTED ) {

		p_control->value = ( pDevice->m_nCustomAnalogVideoFrameRateProperty ) & 0xFFFF;

		if( pDevice->m_nCustomAnalogVideoInterleavedProperty == 1 )
		{
			p_control->value /= 2;
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_HDCP_ENABLE ) {

		p_control->value = pDevice->m_nAnalogCopyProtMacrovisionEnableProperty & 1;

		return 0;
	}

	if( p_control->id == V4L2_CID_HDCP_STATUS ) {

		{
			p_control->value = pDevice->m_nAnalogCopyProtMacrovisionProperty & 0x1;
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_AUDIO_SAMPLING_FREQ ) {

		p_control->value = ( pDevice->m_nCustomAnalogAudioSampleFrequencyProperty ) & 0xFFFF;


		#if defined(AME_SC0710) || defined(AME_SC0720)
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720))
		{
			SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

			if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0 )
			{
				ULONG ch_quantity = (pDevice->bcdDevice & 0xF);

				p_control->value = p_sys_cfg->n_input_audio_sampling_frequencys[pVideo->m_nSubDeviceNumber % ch_quantity];

				LINUXV4L2_DEBUG( KERN_INFO, "[%02d] V4L2_CID_AUDIO_SAMPLING_FREQ() ch(%d) freq(0d%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber, p_sys_cfg->n_input_audio_sampling_frequencys[pVideo->m_nSubDeviceNumber % ch_quantity] );
			}

			p_control->value |= p_sys_cfg->n_input_video_no_audio_packet << 24;

		}
		#endif

		return 0;
	}

	if( p_control->id == V4L2_CID_SPECIAL_CAMERA ) {

		p_control->value = pDevice->m_nCustomSpecialCameraInputProperty;

		return 0;
	}

	if( p_control->id == V4L2_CID_NO_SIGNAL_NO_OUTPUT ) {

		p_control->value = pDevice->m_No_Signal_no_output_property;

		return 0;
	}

	if( p_control->id == V4L2_CID_NO_SIGNAL_YUV_PATTERN ) {

		p_control->value = pDevice->m_No_Signal_yuv_pattern_property;

		return 0;
	}

	if( p_control->id == V4L2_CID_HV_SCALE_FACTOR ) {

		return 0;
	}

	if( p_control->id == V4L2_CID_H264_HV_SCALE_FACTOR ) {

		return 0;
	}

	if( p_control->id == V4L2_CID_INTERLEAVE_DETECTED ) {

		p_control->value = pDevice->m_nCustomAnalogVideoInterleavedProperty;

		#if defined(AME_SC0710) || defined(AME_SC0720)
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

			if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
			{
				ULONG ch_quantity = (pDevice->bcdDevice & 0xF);

				p_control->value = p_sys_cfg->n_input_video_resolution_interleaveds[pVideo->m_nSubDeviceNumber % ch_quantity];
			}
		}
		#endif

		return 0;
	}

	if( p_control->id == V4L2_CID_ENC_DEINTERLACE ) {


		if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) )
		{
			p_control->value = pDevice->m_nCustomAnalogVideoDeinterlaceTypeProperty[ 0 ];
		}

		if( pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001 )
		{
			p_control->value = pDevice->m_nCustomAnalogVideoDeinterlaceTypeProperty[ 0 ];
		}


		return 0;
	}

	if (p_control->id == V4L2_CID_RESET_I2S) {

		p_control->value = pDevice->m_dwDMAErrorFlags;

		return 0;
	}

	if( p_control->id == V4L2_CID_VBI_ENABLE ) {

		p_control->value = pVideo->m_nCustomAnalogVideoVbiLinesEnableProperty;

		return 0;
	}

	if( p_control->id == V4L2_CID_VBI_LINE ) {

		p_control->value = pVideo->m_nCustomAnalogVideoVbiLinesProperty;

		return 0;
	}

	if( p_control->id == V4L2_CID_HANC_TIMECODE_H_M ) {

		p_control->value = pDevice->m_GENNUM_INFO.m_HOURS << 16 | pDevice->m_GENNUM_INFO.m_MINUTES;

		return 0;
	}

	if( p_control->id == V4L2_CID_HANC_TIMECODE_S_F ) {

		p_control->value = pDevice->m_GENNUM_INFO.m_SECONDS << 16 | pDevice->m_GENNUM_INFO.m_FRAMES;

		return 0;
	}

	if( p_control->id == V4L2_CID_ACCESS_I2C_REGISTER ) {



#if defined(AME_SC0710) || defined(AME_SC0720)
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			SC0710_CLOSE_THREAD();
			{
				unsigned char txbuf[2] = { 0, 0 };

				unsigned char rxbuf[2] = { 0, 0 };

				BYTE n_device_address = 0;

				txbuf[0] = pDevice->m_i2c_register;


				n_device_address = pDevice->m_i2c_chipaddr;

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, n_device_address, txbuf, 1, rxbuf, 1);

				p_control->value = rxbuf[0];

				LINUXV4L2_DEBUG(KERN_INFO, "V4L2_CID_ACCESS_I2C_REGISTER get n_device_address(0x%x) register(0x%x) value(0x%x)\n", n_device_address, pDevice->m_i2c_register, p_control->value);
			}
			SC0710_START_THREAD();
		}

#endif


		return 0;
	}

	if( p_control->id == V4L2_CID_CAMERA_TYPE ) {

		p_control->value = pDevice->m_nCustomAnalogVideoPinTopologyProperty;

		return 0;
	}

	if( p_control->id == V4L2_CID_Quad_Link_2_Sample_Interleave ) {

		p_control->value = pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat;

		return 0;
	}

	if( p_control->id == V4L2_CID_MZ0380_FIRMWARE_STATUS ) {

		p_control->value = 0;

		if(	g_pDevice[ 0 ] )
		{
			p_control->value = (g_pDevice[ 0 ]->MZ0380_firmware_statue);
		}
		if(	g_pDevice[ 1 ] )
		{
			p_control->value |= ((g_pDevice[ 1 ]->MZ0380_firmware_statue) << 8);
		}
		if(	g_pDevice[ 2 ] )
		{
			p_control->value |= ((g_pDevice[ 1 ]->MZ0380_firmware_statue) << 16);
		}
		if(	g_pDevice[ 3 ] )
		{
			p_control->value |= ((g_pDevice[ 1 ]->MZ0380_firmware_statue) << 24);
		}

		return 0;
	}

	if( p_control->id == V4L2_CID_MZ0380_OUTPUT_ORIGINAL_DMA_VIDEO ) {

		p_control->value = pDevice->m_nOutputOriginalDMAVideo;

		return 0;
	}

	if( p_control->id == V4L2_CID_COLOR_RANGE ) {

		return 0;
	}

	if( p_control->id == V4L2_CID_VIDEO_INPUT_BANDWIDTH ) {

		p_control->value = pDevice->m_nCustomAnalogVideoInputBandwidthProperty;

		return 0;
	}

	if( p_control->id == V4L2_CID_VIDEO_INPUT_EQ ) {

		p_control->value = pDevice->m_nCustomAnalogVideoInputEqProperty;

		return 0;
	}

	if( p_control->id == V4L2_CID_UPDATE_PID ) {

		ULONG status = 0;

		OnGetCustomDebugEepromPidVidProperty( pVideo, &status );

		p_control->value = status;

		return 0;
	}

	if( p_control->id == V4L2_CID_TIME_ZONE ) {

		p_control->value = pDevice->m_time_zone;

		return 0;
	}

	if( p_control->id == V4L2_CID_720_OUTPUT ) {

		p_control->value = pDevice->m_nCustomAnalogVideo720OutputProperty;

		return 0;
	}

	if( p_control->id == V4L2_CID_TIME_ZONE_HALF_HOUR ) {

		p_control->value = pDevice->m_time_zone_half_hour;

		return 0;
	}

	if( p_control->id == V4L2_CID_DUMP_REGISTER ) {


		return 0;
	}

	if( p_control->id == V4L2_CID_SET_MEDICAL ) {

		p_control->value = pDevice->m_nCustomCompanyMedicalProperty;

		return 0;
	}

	if (p_control->id == V4L2_CID_ACCESS_BAR) {

#if defined(AME_SC0710) || defined(AME_SC0720)

		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			if (pDevice->m_nBAR_block == 0)
			{
				p_control->value = SC0710_GetRegister(pDevice, pDevice->m_nBAR_offset);
			}
			else if (pDevice->m_nBAR_block == 1)
			{
				SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

				p_control->value = READ_REGISTER_ULONG((ULONG*)(p_sys_cfg->p_config_bar_address + pDevice->m_nBAR_offset));
			}
			else
			{
				LINUXV4L2_DEBUG(KERN_INFO, "V4L2_CID_ACCESS_BAR get, wrong bar(0x%x)\n", pDevice->m_nBAR_block);
			}
		}
		#endif


		return 0;
	}

	if( p_control->id == V4L2_CID_HARDWARE_VERSION ) {


		return 0;
	}

	if( p_control->id == V4L2_CID_HDMI_HDRtoSDR ) {

		#ifdef AME_SC0710

		p_control->value = pDevice->m_nCustomAnalogVideoHDR2SDRProperty;

		#endif

		return 0;
	}

	if( p_control->id == V4L2_CID_DEVICE_VIDEO_INPUT_CONFIG ) {




		ULONG nDeviceSerialNumber = 0;

		ULONG mask_input = 0x00000000;

		ULONG j = 0;

		OnGetCustomDeviceSerialNumberProperty( pDevice, &nDeviceSerialNumber );

		for( j = 0 ; j < (sizeof(TABLE_DEVICE_INPUT_TOPOLOGY) / (sizeof(ULONG) * 4)) ; j++ ) {

			if( (TABLE_DEVICE_INPUT_TOPOLOGY[ j ][ 1 ] & nDeviceSerialNumber) ==

				(TABLE_DEVICE_INPUT_TOPOLOGY[ j ][ 0 ]) ) {

				mask_input = TABLE_DEVICE_INPUT_TOPOLOGY[ j ][ 2 ];

				break;
			}
		}
		p_control->value = mask_input;

		LINUXV4L2_DEBUG( KERN_INFO, "V4L2_CID_DEVICE_VIDEO_INPUT_CONFIG( %08X ) nDeviceSerialNumber(0x%x)\n", p_control->value, nDeviceSerialNumber );

		return 0;
	}

	if( p_control->id == V4L2_CID_DEVICE_AUDIO_INPUT_CONFIG ) {




		ULONG nDeviceSerialNumber = 0;

		ULONG mask_input = 0x00000000;

		ULONG j = 0;

		OnGetCustomDeviceSerialNumberProperty( pDevice, &nDeviceSerialNumber );

		for( j = 0 ; j < (sizeof(TABLE_DEVICE_INPUT_TOPOLOGY) / (sizeof(ULONG) * 4)) ; j++ ) {

			if( (TABLE_DEVICE_INPUT_TOPOLOGY[ j ][ 1 ] & nDeviceSerialNumber) ==

				(TABLE_DEVICE_INPUT_TOPOLOGY[ j ][ 0 ]) ) {

				mask_input = TABLE_DEVICE_INPUT_TOPOLOGY[ j ][ 3 ];

				break;
			}
		}
		p_control->value = mask_input;

		LINUXV4L2_DEBUG( KERN_INFO, "V4L2_CID_DEVICE_AUDIO_INPUT_CONFIG( %08X )\n", p_control->value );

		return 0;
	}

	if( p_control->id == V4L2_CID_OUTPUT_VIDEO_AUDIO ) {

		p_control->value = pVideo->m_nCustomAnalogOutputVideoAudioProperty;

		return 0;
	}

	if( p_control->id == V4L2_CID_AUDIO_CHANNEL ) {

		p_control->value = pDevice->m_nCustomAnalogAudioChannelProperty;

		return 0;
	}

	if( p_control->id == V4L2_CID_POWER_SAVING ) {

		p_control->value = pDevice->m_nCustomPowerSavingProperty;

		return 0;
	}

	if (p_control->id == V4L2_CID_MCU_VERSION) {

#if defined(AME_SC0710) || defined(AME_SC0720)
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

			p_control->value = p_sys_cfg->n_mcu_version;

			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] V4L2_CID_MCU_VERSION( %08X )\n", pDevice->m_nKsDeviceNumber, p_control->value);
		}
#endif


		return 0;
	}

	if (p_control->id == V4L2_CID_EXTEND_MCU_VERSION) {

#if defined(AME_SC0710) || defined(AME_SC0720)

		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

			p_control->value = p_sys_cfg->n_extend_mcu_version;

			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] V4L2_CID_EXTEND_MCU_VERSION( %08X )\n", pDevice->m_nKsDeviceNumber, p_control->value);

		}

		#endif

		return 0;
	}

	if( p_control->id == V4L2_CID_C7_VERSION ) {

		#if defined(AME_SC0710) || defined(AME_SC0720)
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

			p_control->value = p_sys_cfg->n_fpga_version;

			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] V4L2_CID_C7_VERSION( %08X )\n", pDevice->m_nKsDeviceNumber, p_control->value);
		}
		#endif

		return 0;
	}

	if( p_control->id == V4L2_CID_FW_VERSION ) {

		#if defined(AME_SC0710) || defined(AME_SC0720)
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);



			p_control->value = (p_sys_cfg->n_firmware_version_year << 24) +

				(p_sys_cfg->n_firmware_version_month << 16) +

				(p_sys_cfg->n_firmware_version_date << 8) +

				(p_sys_cfg->n_firmware_version_sub << 0);

			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] V4L2_CID_FW_VERSION( %08X )\n", pDevice->m_nKsDeviceNumber, p_control->value);
		}
		#endif


		return 0;
	}

	if( p_control->id == V4L2_CID_MAC_ADDRESS_1 ) {

		#ifdef AME_SC0710
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			down(&m_sComponentAccessSemaphore);

			SC0710_CLOSE_THREAD();

			OnGetCustomDebugMacAddressI2cDataProperty(pDevice, 250, &p_control->value);

			SC0710_START_THREAD();

			up(&m_sComponentAccessSemaphore);

			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] V4L2_CID_MAC_ADDRESS_1( 0x%8x )\n", pDevice->m_nKsDeviceNumber, p_control->value);
		}
		#endif

		return 0;
	}

	if( p_control->id == V4L2_CID_MAC_ADDRESS_2 ) {

		#ifdef AME_SC0710
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			down(&m_sComponentAccessSemaphore);

			SC0710_CLOSE_THREAD();

			OnGetCustomDebugMacAddressI2cDataProperty(pDevice, 254, &p_control->value);

			SC0710_START_THREAD();

			up(&m_sComponentAccessSemaphore);

			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] V4L2_CID_MAC_ADDRESS_2( 0x%8x )\n", pDevice->m_nKsDeviceNumber, p_control->value);
		}
		#endif

		return 0;
	}

	if( p_control->id == V4L2_CID_AUTOPHASE ) {

		p_control->value = pDevice->m_nCustomAnalogVideoAutoPhaseTypeProperty[ 0 ] & 0xF7FFFFFF;

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] V4L2_CID_AUTOPHASE( %08X )\n", pDevice->m_nKsDeviceNumber, p_control->value );

		return 0;
	}
	if( p_control->id == V4L2_CID_VBI_MIX_MODE ) {

		p_control->value = (pVideo->m_nCustomAnalogVideoVbiMixModeProperty & 1);

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] V4L2_CID_VBI_MIX_MODE( %08X )\n", pDevice->m_nKsDeviceNumber, p_control->value );

		return 0;
	}

	if (p_control->id == V4L2_PCIE_CONNECTION) {

#if defined(AME_SC0710) || defined(AME_SC0720)
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

			p_control->value = ((p_sys_cfg->n_pcie_generation << 8) | p_sys_cfg->n_pcie_link_width);

			LINUXV4L2_PRINT(KERN_INFO, "generation(0x%x) width(0x%x)\n", p_sys_cfg->n_pcie_generation, p_sys_cfg->n_pcie_link_width);
		}
#endif

		return 0;
	}

	if (p_control->id == V4L2_CID_USE_GPUDIRECT) {

#ifdef AME_SC0710
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720)|| (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
		}
#endif

		return 0;
	}

	if( p_control->id == V4L2_CID_ENABLE_DIRECT ) {

		p_control->value = pDevice->m_nEnableDirect;

		LINUXV4L2_PRINT( KERN_INFO, "[%02d] V4L2_CID_ENABLE_DIRECT(%d)\n", pDevice->m_nKsDeviceNumber, pDevice->m_nEnableDirect );

		return 0;
	}

	if (p_control->id == V4L2_CID_UVC_STATUS) {

		p_control->value = pDevice->m_nUVCRunningStatus;

		return 0;
	}


	return -EINVAL;
}
void SetMCUPolling(CDevice * pDevice, unsigned char stop_polling);

NTSTATUS OnSetCustomDebugEdidAccessEnableProperty( CVideo * pVideo, ULONG PropertyValue, ULONG size );
NTSTATUS OnSetCustomDebugEdidI2cSubProperty( CVideo * pVideo, ULONG PropertyValue );
NTSTATUS OnGetCustomDebugEdidI2cDataProperty( CVideo * pVideo, ULONG* pPropertyValue );
NTSTATUS OnSetCustomDebugEdidI2cDataProperty( CVideo * pVideo, ULONG PropertyValue );

NTSTATUS OnSetCustomDebugMcuAccessEnableProperty( CVideo* pVideo, ULONG pPropertyValue );
NTSTATUS OnSetCustomDebugExtendMcuAccessEnableProperty( CVideo* pVideo, ULONG pPropertyValue );
NTSTATUS OnSetCustomDebugMcuI2cSubProperty( CVideo* pVideo, ULONG pPropertyValue );
NTSTATUS OnGetCustomDebugMcuI2cDataProperty( CVideo* pVideo, ULONG* pPropertyValue );
NTSTATUS OnSetCustomDebugMcuI2cDataProperty( CVideo * pVideo, ULONG pPropertyValue );

NTSTATUS OnSetCustomFirmwareUpdate( CDevice * pDevice, ULONG PropertyValue );

BYTE ptz[ 8 ];

static int common_video_device_vidioc_s_ctrl( struct file * p_file, void * p_priv, struct v4l2_control * p_control )
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	ULONG i = 0;

	ULONG j = 0;

	ULONG ch_quantity = 1;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_ctrl() pVideo == NULL\n" );

		return 0;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_ctrl() pVideo->m_pDevice == NULL\n" );

		return 0;
	}

	pDevice = pVideo->m_pDevice;

	if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
	{
		ch_quantity = (pDevice->bcdDevice & 0xF);
	}

	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6802) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6805) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6814) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6815) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6816) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6817) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x8210) ) { i = pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x5851) ) { i = pDevice->m_nKsDeviceNumber * 8 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pDevice->m_pKsDevice->vendor == 0x1131 && pDevice->m_pKsDevice->device == 0x7160) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1999 && pDevice->m_pKsDevice->device == 0xA04C) ) { i = pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pDevice->m_pKsDevice->vendor == 0x3086 && pDevice->m_pKsDevice->device == 0x0101) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) ) { i = pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1; }
	if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0700) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0701) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) ) { i = pDevice->m_nKsDeviceNumber * 1 + pVideo->m_nSubDeviceNumber * 0; }

	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6801) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6802) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6804) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6805) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6810) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6811) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6812) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6813) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6814) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6815) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6816) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x6817) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x8210) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x14F1 && pDevice->m_pKsDevice->device == 0x5851) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1131 && pDevice->m_pKsDevice->device == 0x7160) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x1999 && pDevice->m_pKsDevice->device == 0xA04C) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x3086 && pDevice->m_pKsDevice->device == 0x0101) ) { j = (pVideo->m_nSubDeviceNumber) % 4; }
	if( (pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) ) { j = pVideo->m_nSubDeviceNumber; }
	if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0700) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0701) ) { j = 0; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) ) { j = pVideo->m_nSubDeviceNumber % ch_quantity; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) ) { j = pVideo->m_nSubDeviceNumber % ch_quantity; }
	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) ) { j = pVideo->m_nSubDeviceNumber % ch_quantity; }


	if( p_control->id == V4L2_CID_BRIGHTNESS ) {

		if(p_control->value < 1 || p_control->value > 255) return 0;

		g_n_analog_decoder_brightness[ i ][ j ] = p_control->value | 0x08000000;

		down( &m_sComponentAccessSemaphore );

		#ifdef AME_SC0710
		if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 ) { SC0710_SetAnalogVideoProcAmpProperties( pDevice, pVideo->m_nSubDeviceNumber, FALSE ); }
		#endif

		up( &m_sComponentAccessSemaphore );

		return 0;
	}
	if( p_control->id == V4L2_CID_CONTRAST ) {

		if(p_control->value < 1 || p_control->value > 255) return 0;

		g_n_analog_decoder_contrast[ i ][ j ] = p_control->value | 0x08000000;

		down( &m_sComponentAccessSemaphore );

		#ifdef AME_SC0710
		if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 ) { SC0710_SetAnalogVideoProcAmpProperties( pDevice, pVideo->m_nSubDeviceNumber, FALSE ); }
		#endif

		up( &m_sComponentAccessSemaphore );

		return 0;
	}
	if( p_control->id == V4L2_CID_SATURATION ) {

		if(p_control->value < 1 || p_control->value > 255) return 0;

		g_n_analog_decoder_saturation[ i ][ j ] = p_control->value | 0x08000000;

		down( &m_sComponentAccessSemaphore );

		#ifdef AME_SC0710
		if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 ) { SC0710_SetAnalogVideoProcAmpProperties( pDevice, pVideo->m_nSubDeviceNumber, FALSE ); }
		#endif

		up( &m_sComponentAccessSemaphore );

		return 0;
	}
	if( p_control->id == V4L2_CID_HUE ) {

		if(p_control->value < 1 || p_control->value > 255) return 0;

		g_n_analog_decoder_hue[ i ][ j ] = p_control->value | 0x08000000;

		down( &m_sComponentAccessSemaphore );

		#ifdef AME_SC0710
		if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 ) { SC0710_SetAnalogVideoProcAmpProperties( pDevice, pVideo->m_nSubDeviceNumber, FALSE ); }
		#endif

		up( &m_sComponentAccessSemaphore );

		return 0;
	}
	if( p_control->id == V4L2_CID_SHARPNESS ) {

		if(p_control->value < 1 || p_control->value > 255) return 0;

		g_n_analog_decoder_sharpness[ i ][ j ] = p_control->value | 0x08000000;

		down( &m_sComponentAccessSemaphore );

		#ifdef AME_SC0710
		if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 ) { SC0710_SetAnalogVideoProcAmpProperties( pDevice, pVideo->m_nSubDeviceNumber, FALSE ); }
		#endif

		up( &m_sComponentAccessSemaphore );

		return 0;
	}

	if( p_control->id == V4L2_CID_AUTOGAIN ) {

		if(p_control->value < 1 || p_control->value > 255) return 0;

		g_n_analog_decoder_gain_auto[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_GAIN ) {

		if(p_control->value < 1 || p_control->value > 255) return 0;

		g_n_analog_decoder_gain[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_SWITCH_SPEED ) {

		pVideo->m_nSwitchSpeed_Spliter = p_control->value;

		pVideo->m_nDropDecumulatorCounter_Spliter = 3;

		return 0;
	}
	if( p_control->id == V4L2_CID_SWITCH_TABLE ) {

		BYTE SWITCH_CHANNEL_TABLE[ 13 ] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

		for( i = 0 ; i < 12 ; i++ ) {

			SWITCH_CHANNEL_TABLE[ i ] = (BYTE)(((p_control->value) >> (i * 2)) & 0x00000003);

			if( SWITCH_CHANNEL_TABLE[ i ] == 0x00 ) { SWITCH_CHANNEL_TABLE[ 12 ] |= 0x01; }

			if( SWITCH_CHANNEL_TABLE[ i ] == 0x01 ) { SWITCH_CHANNEL_TABLE[ 12 ] |= 0x02; }

			if( SWITCH_CHANNEL_TABLE[ i ] == 0x02 ) { SWITCH_CHANNEL_TABLE[ 12 ] |= 0x04; }

			if( SWITCH_CHANNEL_TABLE[ i ] == 0x03 ) { SWITCH_CHANNEL_TABLE[ 12 ] |= 0x08; }
		}
		memcpy( pVideo->m_pSwitchTable_Spliter, SWITCH_CHANNEL_TABLE, 12 );

		pVideo->m_nLockStatusMask_Spliter = SWITCH_CHANNEL_TABLE[ 12 ];

		pVideo->m_nDropDecumulatorCounter_Spliter = 3;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_SENSITIVITY_LOW_THRESHOLD ) {

		g_n_multiplexer_motion_sensitivity_low_threshold[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_SENSITIVITY_HIGH_THRESHOLD ) {

		g_n_multiplexer_motion_sensitivity_high_threshold[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_MASK_CX ) {

		g_n_multiplexer_motion_mask_cx[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_MOTION_MASK_CY ) {

		g_n_multiplexer_motion_mask_cy[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_OSD_POS_X ) {

		g_n_encoder_osd_pos_x[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_OSD_POS_Y ) {

		g_n_encoder_osd_pos_y[ i ] = p_control->value | 0x80000000;

		return 0;
	}
	if( p_control->id == V4L2_CID_OSD_COLOR ) {

		g_n_encoder_osd_color[ i ] = p_control->value | 0x80000000;

		return 0;
	}


	if( p_control->id == V4L2_CID_GPIO_DIRECTION ) {

		g_n_gpio_direction[ i ] = p_control->value;

		return 0;
	}
	if( p_control->id == V4L2_CID_GPIO_DATA ) {

		g_n_gpio_data[ i ] = p_control->value;

		return 0;
	}

	if( p_control->id == V4L2_CID_UPDATE_FW ) {

		OnSetCustomFirmwareUpdate( pDevice, p_control->value & 1 );

		return 0;
	}
	if( p_control->id == V4L2_CID_TW5864_MOTION_TEMPORAL_SENSITIVITY ) {

		if( pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864 )
		{
			pDevice->m_nCustomMotionTemporalSensitivityProperty[pVideo->m_nSubDeviceNumber] = (p_control->value) | 0x08000000;
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_TW5864_MOTION_SPATIAL_SENSITIVITY ) {

		if( pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864 )
		{
			pDevice->m_nCustomMotionSpatialSensitivityProperty[pVideo->m_nSubDeviceNumber] = (p_control->value) | 0x08000000;
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_TW5864_MOTION_LEVEL_SENSITIVITY ) {

		if( pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864 )
		{
			pDevice->m_nCustomMotionLevelSensitivityProperty[pVideo->m_nSubDeviceNumber] = (p_control->value) | 0x08000000;
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_TW5864_MOTION_SPEED ) {

		if( pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864 )
		{
			pDevice->m_nCustomMotionSpeedProperty[pVideo->m_nSubDeviceNumber] = (p_control->value) | 0x08000000;
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_WATCH_DOG_PERIOD ) {

		pDevice->m_nCustomSoftwareWatchdogResetProperty = 0x80000001;

		if( (pDevice->m_nCustomSoftwareWatchdogDurationProperty) != ((p_control->value) & 0xFF) )
		{
			pDevice->m_nCustomSoftwareWatchdogDurationProperty = ((p_control->value) & 0xFF) | 0x80000000;
		}

		return 0;
	}

	if( p_control->id == V4L2_CID_WATCH_DOG ) {

		pDevice->m_nCustomSoftwareWatchdogResetProperty = 0x80000001;

		return 0;
	}

	if( p_control->id == V4L2_CID_WATCH_DOG_ENABLE ) {

		pDevice->m_nCustomSoftwareWatchdogEnableProperty = 0x80000001;

		return 0;
	}

	if( p_control->id == V4L2_CID_SPECIAL_CAMERA ) {

		pDevice->m_nCustomSpecialCameraInputProperty = 0x00000001;

		return 0;
	}

	if( p_control->id == V4L2_CID_NO_SIGNAL_NO_OUTPUT ) {

		pDevice->m_No_Signal_no_output_property = (p_control->value) & 0x1;



		return 0;
	}

	if( p_control->id == V4L2_CID_NO_SIGNAL_YUV_PATTERN ) {

		pDevice->m_No_Signal_yuv_pattern_property = (p_control->value) & 0xFFFFFF;

		LINUXV4L2_PRINT( KERN_INFO, "m_No_Signal_yuv_pattern_property(0x%x)------\n", pDevice->m_No_Signal_yuv_pattern_property );

		return 0;
	}

	if( p_control->id == V4L2_CID_HDCP_ENABLE ) {

		pDevice->m_nAnalogCopyProtMacrovisionEnableProperty = (p_control->value) & 0xFF;


		return 0;
	}

	if( p_control->id == V4L2_CID_HV_SCALE_FACTOR ) {

		return 0;
	}

	if( p_control->id == V4L2_CID_H264_HV_SCALE_FACTOR ) {

		return 0;
	}

	if( p_control->id == V4L2_CID_ENC_DEINTERLACE ) {

		if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

			(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) )
		{
			pDevice->m_nCustomAnalogVideoDeinterlaceTypeProperty[ 0 ] = (p_control->value) | 0x88000000;
		}

		if( pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001 )
		{
			pDevice->m_nCustomAnalogVideoDeinterlaceTypeProperty[ 0 ] = (p_control->value) | 0x88000000;
		}

		return 0;
	}

	if( p_control->id == V4L2_CID_VBI_ENABLE ) {

		pVideo->m_nCustomAnalogVideoVbiLinesEnableProperty = (p_control->value ? TRUE : 0);

		LINUXV4L2_PRINT( KERN_INFO, "[%02d] V4L2_CID_VBI_ENABLE(%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_nCustomAnalogVideoVbiLinesEnableProperty );
		LINUXV4L2_PRINT(KERN_INFO, "[%02d] V4L2_CID_VBI_ENABLE MUST BEFORE VIDIOC_S_FMT\n", pDevice->m_nKsDeviceNumber);

		return 0;
	}

	if( p_control->id == V4L2_CID_VBI_LINE ) {

		pVideo->m_nCustomAnalogVideoVbiLinesProperty = p_control->value;

		LINUXV4L2_PRINT( KERN_INFO, "[%02d] V4L2_CID_VBI_LINE(%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_nCustomAnalogVideoVbiLinesProperty );

		return 0;
	}

	if( p_control->id == V4L2_CID_HDMI_MIXER ) {

		return 0;
	}

	if( p_control->id == V4L2_CID_SET_I2C_REGISTER ) {

		pDevice->m_i2c_register = p_control->value;

		LINUXV4L2_DEBUG( KERN_INFO, "V4L2_CID_SET_I2C_REGISTER set register(0x%x)\n", pDevice->m_i2c_register );

		return 0;
	}

	if (p_control->id == V4L2_CID_ACCESS_I2C_REGISTER) {




#if defined(AME_SC0710) || defined(AME_SC0720)
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			SC0710_CLOSE_THREAD();

			{
				unsigned char txbuf[2] = { 0, 0 };



				BYTE n_device_address = 0;

				txbuf[0] = pDevice->m_i2c_register;

				txbuf[1] = p_control->value;


				n_device_address = pDevice->m_i2c_chipaddr;

				SC0710_AccessSlaveDeviceRegisterS(pDevice, 0, n_device_address, txbuf, 2, NULL, 0);

				LINUXV4L2_DEBUG(KERN_INFO, "V4L2_CID_ACCESS_I2C_REGISTER set n_device_address(0x%x) m_i2c_register(0x%x) value(0x%x)\n", n_device_address, pDevice->m_i2c_register, p_control->value);
			}


			SC0710_START_THREAD();
		}
#endif


		return 0;
	}

	if( p_control->id == V4L2_CID_SIZE_NOT_MATCH ) {

		pDevice->m_size_not_match_no_signal_blue_pattern = p_control->value;

		LINUXV4L2_DEBUG( KERN_INFO, "V4L2_CID_SIZE_NOT_MATCH m_size_not_match_no_signal_blue_pattern(0x%x)\n", pDevice->m_size_not_match_no_signal_blue_pattern );

		return 0;
	}

	if( p_control->id == V4L2_CID_INVERT_UV ) {

		pDevice->m_Invert_UV = p_control->value;

		LINUXV4L2_DEBUG( KERN_INFO, "V4L2_CID_INVERT_UV m_Invert_UV(0x%x)\n", pDevice->m_Invert_UV );

		return 0;
	}

	if( p_control->id == V4L2_CID_SET_I2C_CHIPADDR ) {

		pDevice->m_i2c_chipaddr = p_control->value;

		LINUXV4L2_DEBUG( KERN_INFO, "V4L2_CID_SET_I2C_CHIPADDR set chipaddr(0x%x)\n", pDevice->m_i2c_chipaddr );

		return 0;
	}

	if( p_control->id == V4L2_CID_CAMERA_TYPE ) {

		LINUXV4L2_DEBUG( KERN_INFO, "V4L2_CID_CAMERA_TYPE (0x%x)\n", p_control->value );

		if( ( p_control->value ) != pDevice->m_nCustomAnalogVideoPinTopologyProperty )
		{
			pDevice->m_nCustomAnalogVideoPinTopologyProperty = p_control->value;


		}
		return 0;
	}

	if( p_control->id == V4L2_CID_CommOverCableParams ) {

		LINUXV4L2_DEBUG( KERN_INFO, "V4L2_CID_CommOverCableParams (0x%x)\n", p_control->value );

		return 0;
	}

	if( p_control->id == V4L2_CID_Quad_Link_2_Sample_Interleave ) {

		pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat = (p_control->value & 0x1);

		LINUXV4L2_DEBUG( KERN_INFO, "V4L2_CID_Quad_Link_2_Sample_Interleave (0x%x)\n", pDevice->m_nCustomAnalogVideoMultiSyncPixelFormat );

		return 0;
	}

	if( p_control->id == V4L2_CID_MZ0380_OUTPUT_ORIGINAL_DMA_VIDEO ) {

		pDevice->m_nOutputOriginalDMAVideo = (p_control->value & 0x1);

		LINUXV4L2_DEBUG( KERN_INFO, "V4L2_CID_MZ0380_OUTPUT_ORIGINAL_DMA_VIDEO (0x%x)\n", pDevice->m_nOutputOriginalDMAVideo );
		{
			int i = 1;
			for( i = 1; i < 4; i++ )
			{
				if( g_pDevice[ pDevice->m_nKsDeviceNumber + i ] )
				{
					g_pDevice[ pDevice->m_nKsDeviceNumber + i ]->m_nOutputOriginalDMAVideo = (p_control->value & 0x1);
				}
			}
		}
		return 0;
	}

	if( p_control->id == V4L2_CID_UPDATE_EDID ) {

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] V4L2_CID_UPDATE_EDID \n", pDevice->m_nKsDeviceNumber );

		if(p_control->value & 0x1)
		{
			const struct firmware *fw;
			int ret = 1;

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			u64 ts = 0;
			#else
			struct timeval ts;
			#endif

			ULONGLONG start_times = 0;

			ULONGLONG stop_times = 0;




			#if defined(AME_SC0710) || defined(AME_SC0720)
			if(pDevice->m_pKsVideo[ 0 ])
			{
				ret = request_firmware(&fw, "edid.bin", &(pDevice->m_pKsVideo[ 0 ]->dev));
			}
			#endif

			if (ret) {

				LINUXV4L2_PRINT( KERN_ERR,"[%02d] V4L2_CID_UPDATE_EDID() Upload failed. (file edid.bin not found?)\n", pDevice->m_nKsDeviceNumber );

				return -1;

			} else {

				LINUXV4L2_DEBUG( KERN_INFO,"[%02d] V4L2_CID_UPDATE_EDID() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

				if((fw->size == 256) || (fw->size == 128))
				{

				}
				else
				{
					LINUXV4L2_PRINT( KERN_ERR,"[%02d] V4L2_CID_UPDATE_EDID() fw->size(%d) error \n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

					release_firmware(fw);

					return -1;
				}
			}

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			ts = ktime_get_ns();
			#else
			do_gettimeofday( &ts );
			#endif

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				#ifndef CONFIG_X86_32
				start_times = (ULONGLONG)(ts / 1000);
				#endif
			#else
			start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
			#endif

			OnSetCustomDebugEdidAccessEnableProperty( pVideo, 1, fw->size);

			{
				int i = 0;
				for( i = 0; i < fw->size; i++)
				{
					OnSetCustomDebugEdidI2cSubProperty( pVideo, i);

					OnSetCustomDebugEdidI2cDataProperty( pVideo, *(fw->data + i));
				}
			}

			OnSetCustomDebugEdidAccessEnableProperty( pVideo, 0, fw->size);

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			ts = ktime_get_ns();
			#else
			do_gettimeofday( &ts );
			#endif

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				#ifndef CONFIG_X86_32
				stop_times = (ULONGLONG)(ts / 1000);
				#endif
			#else
			stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
			#endif

			LINUXV4L2_PRINT( KERN_INFO, "[%02d] TOTAL DOWNLOAD TIMES (BUFFER) = %08d (%d BYTES)\n", pDevice->m_nKsDeviceNumber, (ULONG)(stop_times - start_times), (ULONG)fw->size);


			release_firmware(fw);

			return 0;
		}

		return -EINVAL;
	}

	if( p_control->id == V4L2_CID_VIDEO_INPUT_BANDWIDTH ) {

		pDevice->m_nCustomAnalogVideoInputBandwidthProperty = p_control->value;

		LINUXV4L2_DEBUG( KERN_INFO,"[%02d] V4L2_CID_VIDEO_INPUT_BANDWIDTH(%d) \n", pDevice->m_nKsDeviceNumber, pDevice->m_nCustomAnalogVideoInputBandwidthProperty);

		return 0;
	}

	if( p_control->id == V4L2_CID_VIDEO_INPUT_EQ ) {

		pDevice->m_nCustomAnalogVideoInputEqProperty = p_control->value;

		LINUXV4L2_DEBUG( KERN_INFO,"[%02d] V4L2_CID_VIDEO_INPUT_EQ(%d) \n", pDevice->m_nKsDeviceNumber, pDevice->m_nCustomAnalogVideoInputEqProperty);

		return 0;
	}

	if( p_control->id == V4L2_CID_UPDATE_MCU ) {

		LINUXV4L2_DEBUG( KERN_INFO,"[%02d] V4L2_CID_UPDATE_MCU() \n", pDevice->m_nKsDeviceNumber );

		if(p_control->value & 0x1)
		{
			BYTE* pFileBuffer = NULL;

			BYTE* pWorkBuffer = NULL;

			const struct firmware *fw;
			int ret = 1;

			ULONG             nFileSize = 0;

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			u64 ts = 0;
			#else
			struct timeval ts;
			#endif

			ULONGLONG start_times = 0;

			ULONGLONG stop_times = 0;




			#if defined(AME_SC0710) || defined(AME_SC0720)
			if(pDevice->m_pKsVideo[ 0 ])
			{
				ret = request_firmware(&fw, "mcu.bin", &(pDevice->m_pKsVideo[ 0 ]->dev));
			}
			#endif

			if (ret) {

				LINUXV4L2_PRINT( KERN_ERR,"[%02d] V4L2_CID_UPDATE_MCU() Upload failed. (file mcu.bin not found?)\n", pDevice->m_nKsDeviceNumber );

				return -1;

			} else {

				if (fw)
				{
					LINUXV4L2_DEBUG(KERN_INFO, "[%02d] V4L2_CID_UPDATE_MCU() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

					if (fw->size > 0)
					{
						nFileSize = fw->size;

						pFileBuffer = vmalloc(nFileSize + 4);

						pWorkBuffer = vmalloc(nFileSize + 4);

						if (pFileBuffer && pWorkBuffer)
						{
							memset(pFileBuffer, 0x00, nFileSize + 4);

							memcpy(pFileBuffer, fw->data, fw->size);
						}
						else
						{
							LINUXV4L2_PRINT(KERN_ERR, "[%02d] V4L2_CID_UPDATE_MCU() no buffer\n", pDevice->m_nKsDeviceNumber);

							release_firmware(fw);

							FREE_MEMORY(pFileBuffer);

							FREE_MEMORY(pWorkBuffer);

							return FALSE;
						}
					}
					else
					{
						LINUXV4L2_PRINT(KERN_ERR, "[%02d] V4L2_CID_UPDATE_MCU() fw->size == 0 \n", pDevice->m_nKsDeviceNumber);

						release_firmware(fw);

						return -1;
					}
				}
			}

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			ts = ktime_get_ns();
			#else
			do_gettimeofday( &ts );
			#endif

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				#ifndef CONFIG_X86_32
				start_times = (ULONGLONG)(ts / 1000);
				#endif
			#else
			start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
			#endif


			if (pFileBuffer[0] == 0x00 &&

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

				OnSetCustomDebugMcuAccessEnableProperty(pVideo, nFileSize - 16);

				{
					int i = 0;
					for (i = 0; i < (nFileSize - 16); i++)
					{
						OnSetCustomDebugMcuI2cSubProperty(pVideo, i);

						OnSetCustomDebugMcuI2cDataProperty(pVideo, *(pWorkBuffer+i));
					}
				}
				OnSetCustomDebugMcuAccessEnableProperty(pVideo, 0);
			}

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			ts = ktime_get_ns();
			#else
			do_gettimeofday( &ts );
			#endif

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				#ifndef CONFIG_X86_32
				stop_times = (ULONGLONG)(ts / 1000);
				#endif
			#else
			stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
			#endif

			LINUXV4L2_PRINT( KERN_INFO, "[%02d] TOTAL DOWNLOAD TIMES (BUFFER) = %08d (%d BYTES)\n", pDevice->m_nKsDeviceNumber, (ULONG)(stop_times - start_times), (ULONG)fw->size);


			release_firmware(fw);

			return 0;
		}

		return 0;
	}

	if( p_control->id == V4L2_CID_CommOverCableParams_First ) {

		LINUXV4L2_DEBUG( KERN_INFO, "V4L2_CID_CommOverCableParams_First (0x%x)\n", p_control->value );

		return 0;
	}

	if( p_control->id == V4L2_CID_CommOverCableParams_Second ) {

		LINUXV4L2_DEBUG( KERN_INFO, "V4L2_CID_CommOverCableParams_Second (0x%x)\n", p_control->value );

		return 0;
	}

	if( p_control->id == V4L2_CID_UPDATE_PID ) {

		LINUXV4L2_DEBUG( KERN_INFO,"[%02d] V4L2_CID_UPDATE_PID(%d) \n", pDevice->m_nKsDeviceNumber, p_control->value );

		OnSetCustomDebugEepromPidVidProperty( pVideo, p_control->value );

		return 0;
	}

	if( p_control->id == V4L2_CID_SET_I2C_BANK ) {

		pDevice->m_MST3367_bank = p_control->value;

		LINUXV4L2_DEBUG( KERN_INFO, "V4L2_CID_SET_I2C_REGISTER set bank(0x%x)\n", pDevice->m_MST3367_bank );

		return 0;
	}

	if( p_control->id == V4L2_CID_TIME_ZONE ) {

		pDevice->m_time_zone = p_control->value;

		LINUXV4L2_DEBUG( KERN_INFO, "V4L2_CID_TIME_ZONE (0d%d)\n", pDevice->m_time_zone );

		return 0;
	}

	if( p_control->id == V4L2_CID_720_OUTPUT ) {

		pDevice->m_nCustomAnalogVideo720OutputProperty = p_control->value;


		return 0;
	}

	if( p_control->id == V4L2_CID_TIME_ZONE_HALF_HOUR ) {

		pDevice->m_time_zone_half_hour = p_control->value & 1;

		return 0;
	}

	if( p_control->id == V4L2_CID_SET_MEDICAL ) {

		pDevice->m_nCustomCompanyMedicalProperty = p_control->value & 1;


		return 0;
	}

	if( p_control->id == V4L2_CID_SET_BAR_BLOCK ) {

		pDevice->m_nBAR_block = p_control->value & 0x7;

		return 0;
	}

	if( p_control->id == V4L2_CID_SET_BAR_OFFSET ) {

		pDevice->m_nBAR_offset = p_control->value & 0xFFFFFFFF;

		return 0;
	}

	if (p_control->id == V4L2_CID_ACCESS_BAR) {

#if defined(AME_SC0710) || defined(AME_SC0720)

		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			if (pDevice->m_nBAR_block == 0)
			{
				SC0710_SetRegister(pDevice, pDevice->m_nBAR_offset, p_control->value);
			}
			else if (pDevice->m_nBAR_block == 1)
			{
				SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);


				WRITE_REGISTER_ULONG((ULONG*)(p_sys_cfg->p_config_bar_address + pDevice->m_nBAR_offset), p_control->value);
			}
			else
			{
				LINUXV4L2_DEBUG(KERN_INFO, "V4L2_CID_ACCESS_BAR set, wrong bar(0x%x)\n", pDevice->m_nBAR_block);
			}
		}
		#endif


		return 0;
	}

	if (p_control->id == V4L2_CID_HDMI_HDRtoSDR) {

#ifdef AME_SC0710
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			pDevice->m_nCustomAnalogVideoHDR2SDRProperty = p_control->value & 1;

			down(&m_sComponentAccessSemaphore);

			SC0710_CLOSE_THREAD();

			SC0710_EnableHDR2SDR(pDevice, pDevice->m_nCustomAnalogVideoHDR2SDRProperty);

			SC0710_START_THREAD();

			up(&m_sComponentAccessSemaphore);
		}
		#endif

		return 0;
	}

	if( p_control->id == V4L2_CID_OUTPUT_VIDEO_AUDIO ) {

		pVideo->m_nCustomAnalogOutputVideoAudioProperty = p_control->value & 1;

		LINUXV4L2_PRINT( KERN_INFO, "[%02d] V4L2_CID_OUTPUT_VIDEO_AUDIO() m_nCustomAnalogOutputVideoAudioProperty(0x%x)\n", pDevice->m_nKsDeviceNumber, pVideo->m_nCustomAnalogOutputVideoAudioProperty );

		return 0;
	}

	if( p_control->id == V4L2_CID_AUDIO_CHANNEL ) {

		pDevice->m_nCustomAnalogAudioChannelProperty = p_control->value & 0xF;

		LINUXV4L2_PRINT( KERN_INFO, "[%02d] V4L2_CID_AUDIO_CHANNEL() m_nCustomAnalogAudioChannelProperty(0x%x)\n", pDevice->m_nKsDeviceNumber, pDevice->m_nCustomAnalogAudioChannelProperty );

		if( pDevice->m_nCustomAnalogAudioChannelProperty == 0x8 )
		{
			pDevice->m_nCustomAnalogAudio8chI2sEnableProperty = 1;
		}
		else
		{
			pDevice->m_nCustomAnalogAudio8chI2sEnableProperty = 0;
		}

		return 0;
	}

	if (p_control->id == V4L2_CID_POWER_SAVING) {

#ifdef AME_SC0710
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			pDevice->m_nCustomPowerSavingProperty = p_control->value & 1;

			SC0710_PowerSaving(pDevice, FALSE);
		}
		#endif

		return 0;
	}

	if( p_control->id == V4L2_CID_UPDATE_EXTEND_MCU ) {

		LINUXV4L2_DEBUG( KERN_INFO,"[%02d] V4L2_CID_UPDATE_EXTEND_MCU() \n", pDevice->m_nKsDeviceNumber );

		if(p_control->value & 0x1)
		{
			BYTE* pFileBuffer = NULL;

			BYTE* pWorkBuffer = NULL;

			const struct firmware *fw;
			int ret = 1;

			ULONG             nFileSize = 0;

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			u64 ts = 0;
			#else
			struct timeval ts;
			#endif

			ULONGLONG start_times = 0;

			ULONGLONG stop_times = 0;



			#if defined(AME_SC0710)
			if(pDevice->m_pKsVideo[ 0 ])
			{
				ret = request_firmware(&fw, "mcu.bin", &(pDevice->m_pKsVideo[ 0 ]->dev));
			}
			#endif

			if (ret) {

				LINUXV4L2_PRINT( KERN_ERR,"[%02d] V4L2_CID_UPDATE_EXTEND_MCU() Upload failed. (file mcu.bin not found?)\n", pDevice->m_nKsDeviceNumber );

				return -1;

			} else {

				if (fw)
				{
					LINUXV4L2_DEBUG(KERN_INFO, "[%02d] V4L2_CID_UPDATE_EXTEND_MCU() firmware read %d bytes\n", pDevice->m_nKsDeviceNumber, (ULONG)fw->size);

					if (fw->size > 0)
					{
						nFileSize = fw->size;

						pFileBuffer = vmalloc(nFileSize + 4);

						pWorkBuffer = vmalloc(nFileSize + 4);

						if (pFileBuffer && pWorkBuffer)
						{
							memset(pFileBuffer, 0x00, nFileSize + 4);

							memcpy(pFileBuffer, fw->data, fw->size);
						}
						else
						{
							LINUXV4L2_PRINT(KERN_ERR, "[%02d] V4L2_CID_UPDATE_EXTEND_MCU() no buffer\n", pDevice->m_nKsDeviceNumber);

							release_firmware(fw);

							FREE_MEMORY(pFileBuffer);

							FREE_MEMORY(pWorkBuffer);

							return FALSE;
						}
					}
					else
					{
						LINUXV4L2_PRINT(KERN_ERR, "[%02d] V4L2_CID_UPDATE_EXTEND_MCU() fw->size == 0 \n", pDevice->m_nKsDeviceNumber);

						release_firmware(fw);

						return -1;
					}
				}
			}

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			ts = ktime_get_ns();
			#else
			do_gettimeofday( &ts );
			#endif

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				#ifndef CONFIG_X86_32
				start_times = (ULONGLONG)(ts / 1000);
				#endif
			#else
			start_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
			#endif

			if (pFileBuffer[0] == 0x00 &&

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

				OnSetCustomDebugExtendMcuAccessEnableProperty(pVideo, nFileSize - 16);
				{
					int i = 0;
					for (i = 0; i < (nFileSize - 16); i++)
					{
						OnSetCustomDebugMcuI2cSubProperty(pVideo, i);

						OnSetCustomDebugMcuI2cDataProperty(pVideo, *(pWorkBuffer + i));
					}
				}

				OnSetCustomDebugExtendMcuAccessEnableProperty(pVideo, 0);
			}

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			ts = ktime_get_ns();
			#else
			do_gettimeofday( &ts );
			#endif

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
				#ifndef CONFIG_X86_32
				stop_times = (ULONGLONG)(ts / 1000);
				#endif
			#else
			stop_times = (ULONGLONG)(ts.tv_sec * 1000000) + (ULONGLONG)(ts.tv_usec);
			#endif

			LINUXV4L2_PRINT( KERN_INFO, "[%02d] TOTAL DOWNLOAD TIMES (BUFFER) = %08d (%d BYTES)\n", pDevice->m_nKsDeviceNumber, (ULONG)(stop_times - start_times), (ULONG)fw->size);


			release_firmware(fw);

			return 0;
		}

		return 0;
	}

	if( p_control->id == V4L2_CID_MUTE_LOOPTHROUGH ) {

		#ifdef AME_SC0710
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			SC0710_MuteLoopThrough(pDevice, p_control->value & 1);
		}
		#endif

		return 0;
	}

	if (p_control->id == V4L2_CID_MAC_ADDRESS_1) {

#ifdef AME_SC0710
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			OnSetCustomDebugMacAddressI2cSubProperty(250);

			OnSetCustomDebugMacAddressI2cDataProperty((p_control->value >> 8) & 0xFF);

			OnSetCustomDebugMacAddressI2cSubProperty(251);

			OnSetCustomDebugMacAddressI2cDataProperty((p_control->value) & 0xFF);
		}
		#endif

		return 0;
	}

	if( p_control->id == V4L2_CID_MAC_ADDRESS_2 ) {

		#ifdef AME_SC0710
		if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
		{
			OnSetCustomDebugMacAddressI2cSubProperty(252);

			OnSetCustomDebugMacAddressI2cDataProperty((p_control->value >> 24) & 0xFF);

			OnSetCustomDebugMacAddressI2cSubProperty(253);

			OnSetCustomDebugMacAddressI2cDataProperty((p_control->value >> 16) & 0xFF);

			OnSetCustomDebugMacAddressI2cSubProperty(254);

			OnSetCustomDebugMacAddressI2cDataProperty((p_control->value >> 8) & 0xFF);

			OnSetCustomDebugMacAddressI2cSubProperty(255);

			OnSetCustomDebugMacAddressI2cDataProperty((p_control->value >> 0) & 0xFF);

			LINUXV4L2_PRINT(KERN_INFO, "-----------------MAC_ADDRESS 250 %X --------------\n", g_b_mac_address_data[250]);
			LINUXV4L2_PRINT(KERN_INFO, "-----------------MAC_ADDRESS 251 %X --------------\n", g_b_mac_address_data[251]);
			LINUXV4L2_PRINT(KERN_INFO, "-----------------MAC_ADDRESS 252 %X --------------\n", g_b_mac_address_data[252]);
			LINUXV4L2_PRINT(KERN_INFO, "-----------------MAC_ADDRESS 253 %X --------------\n", g_b_mac_address_data[253]);
			LINUXV4L2_PRINT(KERN_INFO, "-----------------MAC_ADDRESS 254 %X --------------\n", g_b_mac_address_data[254]);
			LINUXV4L2_PRINT(KERN_INFO, "-----------------MAC_ADDRESS 255 %X --------------\n", g_b_mac_address_data[255]);


			OnSetCustomDebugMacAddressAccessEnableProperty(pDevice);
		}
		#endif

		return 0;
	}

	if( p_control->id == V4L2_CID_AUTOPHASE ) {

		pDevice->m_nCustomAnalogVideoAutoPhaseTypeProperty[ 0 ] = p_control->value | 0x00000000;

		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] V4L2_CID_AUTOPHASE( %08X )\n", pDevice->m_nKsDeviceNumber, p_control->value );

		return 0;
	}
	if( p_control->id == V4L2_CID_VBI_MIX_MODE ) {

		pVideo->m_nCustomAnalogVideoVbiMixModeProperty = (p_control->value & 1);

		LINUXV4L2_PRINT( KERN_INFO, "[%02d] V4L2_CID_VBI_MIX_MODE(%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_nCustomAnalogVideoVbiMixModeProperty );

		return 0;
	}

	if (p_control->id == V4L2_CID_USE_GPUDIRECT) {

#ifdef AME_SC0710
#endif

		return 0;
	}
	if (p_control->id == V4L2_CID_VIDEO_CONNECTIONS) {

#ifdef AME_SC0710

		if(pDevice->idVendor == 0x12AB && pDevice->idProduct == 0x0710)	{

			if(((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x1D)) ||
				((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D)) ) {

				unsigned char txbuf[16] = { 0 };
				unsigned char rxbuf[16] = { 0 };
				ULONG i = 0;
				BYTE select = ((p_control->value & 0x1) == 0) ? 0 : 1;

				SC0710_CLOSE_THREAD();

				while(1)
				{
					txbuf[0] = 0x12;
					txbuf[1] = 0x34;
					txbuf[2] = 0x5D;
					txbuf[3] = select;
					mcu_i2c_access(pDevice,0x55, 4, txbuf, 3, rxbuf);

					LINUXV4L2_PRINT(KERN_INFO,"SetMCUInput(%d)::AP ROM flag (0x%x, 0x%x, 0x%x) \n", select , rxbuf[0], rxbuf[1], rxbuf[2]);

					if(rxbuf[0] == 0xaa && rxbuf[1] == 0x55 && rxbuf[2] == select)
					{
						break;
					}

					i++;

					if(i >= 30) {

						break;
					}


					MySleep(300);
				}
				SC0710_START_THREAD();
			}
		}
#endif

		return 0;
	}

	if( p_control->id == V4L2_CID_ENABLE_DIRECT ) {

		pDevice->m_nEnableDirect = (p_control->value & 1);

		LINUXV4L2_PRINT( KERN_INFO, "[%02d] V4L2_CID_ENABLE_DIRECT(%d)\n", pDevice->m_nKsDeviceNumber, pDevice->m_nEnableDirect );

		return 0;
	}
	return -EINVAL;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)

static ULONG g_n_edid_i2c_sub = 0x00;

static BYTE  g_b_edid_i2c_data[ 256 ];

static BOOL  g_b_is_write_mode_enabled = FALSE;

ULONG g_n_debug_board_memory_access = 0x00000000;

static int common_video_device_vidioc_g_ext_ctrls( struct file * p_file, void * p_priv, struct v4l2_ext_controls * p_ext_controls )
{
	CDevice* pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	struct v4l2_ext_control * p_ext_control = 0;

	int count = 0;

	int size = 0;

	int i = 0;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_g_ext_ctrls() pVideo == NULL\n" );

		return 0;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_g_ext_ctrls() pVideo->m_pDevice == NULL\n" );

		return 0;
	}

	pDevice = pVideo->m_pDevice;

	p_ext_control = p_ext_controls->controls;

	count = p_ext_controls->count;

	size = sizeof(struct v4l2_ext_control);

	i = pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1;



	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)

	if( p_ext_controls->which == V4L2_CTRL_CLASS_MPEG ) {

	#else

	if( p_ext_controls->ctrl_class == V4L2_CTRL_CLASS_MPEG ) {

	#endif

		if( count > 6 ) return 0;

		while( count ) {

			switch( p_ext_control->id ) {

			case V4L2_CID_MPEG_VIDEO_BITRATE_MODE: p_ext_control->value = pVideo->m_nEncoderMode; break;

			case V4L2_CID_MPEG_VIDEO_BITRATE:      p_ext_control->value = pVideo->m_nEncoderBitRate; break;

			case V4L2_CID_MPEG_VIDEO_QUALITY:      p_ext_control->value = pVideo->m_nEncoderQuality; break;

			case V4L2_CID_MPEG_VIDEO_GOP_SIZE:     p_ext_control->value = pVideo->m_nEncoderGop; break;

			case V4L2_CID_MPEG_VIDEO_FRAMERATE:    p_ext_control->value = pVideo->m_nFrameRate; break;

			case V4L2_CID_MPEG_VIDEO_QPSTEP:       p_ext_control->value = pVideo->m_nEncoderQPStep; break;

			case V4L2_CID_MPEG_VIDEO_MAX_QP:
			case V4L2_CID_MPEG_VIDEO_H264_MAX_QP:
				p_ext_control->value = pDevice->m_nCustomEncoderVideoMaxQPProperty[ 0 ]; break;

			case V4L2_CID_MPEG_VIDEO_MIN_QP:
			case V4L2_CID_MPEG_VIDEO_H264_MIN_QP:

				p_ext_control->value = pDevice->m_nCustomEncoderVideoMinQPProperty[ 0 ]; break;

			case V4L2_CID_MPEG_VIDEO_OSD_ENABLE:	p_ext_control->value = pDevice->m_bCustomOsdTextEnableProperty;	break;

			case V4L2_CID_MPEG_VIDEO_OSD_X_POSITION:	p_ext_control->value = pDevice->m_nCustomOsdTextPositionXProperty[pVideo->m_nSubDeviceNumber][ pDevice->m_nCustomOsdTextLineProperty[pVideo->m_nSubDeviceNumber] ];	break;

			case V4L2_CID_MPEG_VIDEO_OSD_Y_POSITION:	p_ext_control->value = pDevice->m_nCustomOsdTextPositionYProperty[pVideo->m_nSubDeviceNumber][ pDevice->m_nCustomOsdTextLineProperty[pVideo->m_nSubDeviceNumber] ];	break;

			case V4L2_CID_MPEG_VIDEO_OSD_FONT_SIZE:		p_ext_control->value = pDevice->m_nCustomOsdTextFontSizeProperty[pVideo->m_nSubDeviceNumber][ pDevice->m_nCustomOsdTextLineProperty[pVideo->m_nSubDeviceNumber] ];	break;

			case V4L2_CID_MPEG_VIDEO_OSD_TEXT_COLOR:	p_ext_control->value = pDevice->m_nCustomOsdTextColorProperty[pVideo->m_nSubDeviceNumber][ pDevice->m_nCustomOsdTextLineProperty[pVideo->m_nSubDeviceNumber] ];	break;

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)

			case V4L2_CID_MPEG_VIDEO_OSD:

													if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) )

													{
														memcpy( p_ext_control->string, (CHAR *)(pDevice->m_pCustomOsdTextStringProperty[pVideo->m_nSubDeviceNumber][ pDevice->m_nCustomOsdTextLineProperty[pVideo->m_nSubDeviceNumber] ]), 64 );
													}
													else
													{
														memcpy( p_ext_control->string, g_p_encoder_osd_text[ i ], 32 );
													}
													break;

			#else

			case V4L2_CID_MPEG_VIDEO_OSD:

													if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) )

													{
														memcpy( p_ext_control->reserved, pDevice->m_pCustomOsdTextStringProperty[pVideo->m_nSubDeviceNumber][ 0 ], 64 );
													}
													else
													{
														memcpy( p_ext_control->reserved, g_p_encoder_osd_text[ i ], 32 );
													}
													break;


			#endif

			case V4L2_CID_MPEG_VIDEO_H264_PROFILE:	p_ext_control->value = (pDevice->m_nCustomEncoderVideoProfileProperty[pVideo->m_nSubDeviceNumber]) & 0xFF;	break;

			case V4L2_CID_MPEG_VIDEO_H264_LEVEL:	p_ext_control->value = (pDevice->m_nCustom_H264_Level_Property) & 0xFF;	break;

			case V4L2_CID_MPEG_VIDEO_ASPECT:		p_ext_control->value = (pDevice->m_nCustomEncoderVideoAspectRatioProperty[pVideo->m_nSubDeviceNumber]) & 0x77FFFFFF; break;

			case V4L2_CID_MPEG_SET_SLICE:			p_ext_control->value = pVideo->m_nCustomEncoderSlice; break;

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0)
			case V4L2_CID_MPEG_VIDEO_H264_ENTROPY_MODE:			p_ext_control->value = pDevice->m_nCustomEncoderVideoEntropyProperty[ 0 ] ? V4L2_MPEG_VIDEO_H264_ENTROPY_MODE_CAVLC : V4L2_MPEG_VIDEO_H264_ENTROPY_MODE_CABAC; break;
			#endif


			case V4L2_CID_MPEG_XET_DEBUG_EEPROM:
			{

				LINUXV4L2_PRINT( KERN_INFO,"V4L2_CID_MPEG_XET_DEBUG_EEPROM( %08X )\n", p_ext_control->value);

			}
			break;

			case V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_ACCESS:
			{
				p_ext_control->value = g_n_debug_board_memory_access;
			}
			break;


			#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0)
			case V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY:
			{

				#if defined(AME_SC0710) || defined(AME_SC0720)

				if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) )
				{
					down(&m_sComponentAccessSemaphore);

					SC0710_CLOSE_THREAD();
					{
						ULONG ret = 1;

						unsigned char txbuf[8] = { 0 };

						unsigned char rxbuf[8] = { 0 };

						unsigned char rx[64] = { 0 };

						ULONG i = 0;

						SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

						if( p_sys_cfg->n_mcu_version == 0)
						{
							LINUXV4L2_PRINT( KERN_INFO, "V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY, mcu version = 0\n");
							return 0;
						}

						txbuf[0] = 0x12;
						txbuf[1] = 0x34;
						txbuf[2] = 0x66;

						mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);

						LINUXV4L2_PRINT( KERN_INFO, "V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY (0x%x, 0x%x, 0x%x) get\n", rxbuf[0], rxbuf[1], rxbuf[2]);

						for(i = 0;i < 50;i++)
						{
							if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x35)
								break;
							MySleep(100);

							mcu_i2c_access(pDevice,0x55, 0, txbuf, 3, rxbuf);

							LINUXV4L2_PRINT( KERN_INFO, "V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY() %d (0x%x, 0x%x, 0x%x) \n", i, rxbuf[0], rxbuf[1], rxbuf[2]);
						}

						MySleep(100);

						if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x35)
						{
							for(i = 0;i < sizeof(rx); i+=16)
							{
								txbuf[0] = 0x55;
								txbuf[1] = (BYTE)i;

								i2c_write_bytes(pDevice, 0x66, txbuf, 2);

								MySleep(2);

								i2c_read_bytes(pDevice, 0x66, rx + i, 16 );

								MySleep(2);
							}

							i = 0;
							while(1)
							{
								txbuf[0] = 0xcc;
								mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);
								LINUXV4L2_PRINT( KERN_INFO, "Exit ReadEDID wait %d (0x%x, 0x%x, 0x%x) \n", i, rxbuf[0], rxbuf[1], rxbuf[2]);
								if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x31)

									break;
								i++;
								MySleep(100);
								if(i == 3)
								{
									txbuf[0] = 0x00;
									txbuf[1] = 0x00;
									i2c_write_bytes(pDevice, 0x66, txbuf, 2);

									break;
								}
							}

							ret = copy_to_user( p_ext_control->ptr, (void *)rx , sizeof(rx) );

							if( ret != 0 )
							{
								LINUXV4L2_PRINT( KERN_ERR, "CAN NOT COPY DATA TO APPLICATION\n");
							}
						}
					}
					SC0710_START_THREAD();

					up(&m_sComponentAccessSemaphore);
				}
				#endif


				LINUXV4L2_PRINT( KERN_INFO,"[%02d] V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY( 0 )\n", pDevice->m_nKsDeviceNumber );
			}
			break;


			case V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_2:
			{


				LINUXV4L2_PRINT( KERN_INFO,"[%02d] V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_2( 1 )\n", pDevice->m_nKsDeviceNumber );
			}
			break;


			case V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_3:
			{


				LINUXV4L2_PRINT( KERN_INFO,"[%02d] V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_3( 2 )\n", pDevice->m_nKsDeviceNumber );
			}
			break;

			case V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_4:
			{


				LINUXV4L2_PRINT( KERN_INFO,"[%02d] V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_4( 3 )\n", pDevice->m_nKsDeviceNumber );
			}
			break;

			case V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_5:
			{


				LINUXV4L2_PRINT( KERN_INFO,"[%02d] V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_5( 4 )\n", pDevice->m_nKsDeviceNumber );
			}
			break;

			case V4L2_CID_MPEG_VIDEO_OSD_PICTURE_PARM:
			{

			}
			break;

			case V4L2_CID_MPEG_VIDEO_OSD_PICTURE:
			{
			}
			break;

			case V4L2_CID_HDR_PACKETS:
			{

				#if defined(AME_SC0710) || defined(AME_SC0720)
				if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
				{
					ULONG ret = 1;

					SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

					BYTE* HDR = (BYTE*)p_sys_cfg->n_input_video_hdr_packets;

					ret = copy_to_user(p_ext_control->string, p_sys_cfg->n_input_video_hdr_packets, 32);

					if (ret != 0)
					{
						LINUXV4L2_PRINT(KERN_ERR, "CAN NOT COPY DATA TO APPLICATION\n");
					}

					LINUXV4L2_DEBUG(KERN_INFO, "[%02d] [HDR] [%02X.%02X.%02X.%02X | %02X.%02X.%02X.%02X | %02X.%02X.%02X.%02X | %02X.%02X.%02X.%02X | %02X.%02X.%02X.%02X | %02X.%02X.%02X.%02X | %02X.%02X.%02X.%02X | %02X.%02X.%02X.%02X]\n",

						pDevice->m_nKsDeviceNumber, HDR[0], HDR[1], HDR[2], HDR[3], HDR[4], HDR[5], HDR[6], HDR[7],

						HDR[8], HDR[9], HDR[10], HDR[11], HDR[12], HDR[13], HDR[14], HDR[15],

						HDR[16], HDR[17], HDR[18], HDR[19], HDR[20], HDR[21], HDR[22], HDR[23],

						HDR[24], HDR[25], HDR[26], HDR[27], HDR[28], HDR[29], HDR[30], HDR[31]);
				}
				#endif

			}
			break;

			case V4L2_CID_READ_EDID:
			{
				unsigned long ret = 0;

				OnSetCustomDebugEdidAccessEnableProperty(pVideo, 1, 256);

				ret = copy_to_user(p_ext_control->string, g_b_edid_i2c_data, sizeof(g_b_edid_i2c_data));

				LINUXV4L2_PRINT(KERN_INFO, "READ EDID, ret(lld%lld)\n", ret);
			}
			break;


			#endif

			default:  return -EINVAL;
			}
			p_ext_control = p_ext_control + size;

			count--;
		}
	}
	return 0;
}

static int common_video_device_vidioc_s_ext_ctrls( struct file * p_file, void * p_priv, struct v4l2_ext_controls * p_ext_controls )
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	struct v4l2_ext_control * p_ext_control = 0;

	int count = 0;

	int size = 0;

	int i = 0;

	int index = 0;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_ext_ctrls() pVideo == NULL\n" );

		return 0;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_s_ext_ctrls() pVideo->m_pDevice == NULL\n" );

		return 0;
	}

	pDevice = pVideo->m_pDevice;

	p_ext_control = p_ext_controls->controls;

	count = p_ext_controls->count;

	size = sizeof(struct v4l2_ext_control);

	i = pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1;




	if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) )
	{
		ULONG counts = 1;

		if( ((pDevice->iProduct & 0x0F) == 0x02) || ((pDevice->iProduct & 0x0F) == 0x03)  ) {

			if( (pDevice->iManufacturer & 0x0F) == 0x0A )
			{
				counts = 8;
			}
			else
			{
				counts = 4;
			}
		}

		index = pVideo->m_nSubDeviceNumber;
	}

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)

	if( p_ext_controls->which == V4L2_CTRL_CLASS_MPEG ) {

	#else

	if( p_ext_controls->ctrl_class == V4L2_CTRL_CLASS_MPEG ) {

	#endif


		if( count > 7 ) return 0;

		while( count ) {

			switch(p_ext_control->id) {

			case V4L2_CID_MPEG_VIDEO_BITRATE_MODE:	pVideo->m_nEncoderMode    = (p_ext_control->value ) & 3;



													if(pVideo->m_nEncoderMode == V4L2_MPEG_VIDEO_BITRATE_MODE_VBR )
													{
													}

													if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) )
													{

														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pDevice->m_nCustomEncoderVideoBitRateModeProperty[ index ]  =   0x88000000 | pVideo->m_nEncoderMode;

															LINUXV4L2_DEBUG( KERN_INFO,"[%02d] bitrate mode[%d](%u)\n", pDevice->m_nKsDeviceNumber, index, pDevice->m_nCustomEncoderVideoBitRateModeProperty[ index ] );
														}
													}
													if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) ) {

														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pDevice->m_nCustomEncoderVideoBitRateModeProperty[ 0 ]  =   0x88000000 | pVideo->m_nEncoderMode;
														}
													}

													break;

			case V4L2_CID_MPEG_VIDEO_BITRATE:		pVideo->m_nEncoderBitRate = p_ext_control->value;



													if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) )
													{

														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pDevice->m_nCustomEncoderVideoBitRateProperty[ index ] = 0x88000000 | pVideo->m_nEncoderBitRate;

															LINUXV4L2_DEBUG( KERN_INFO,"[%02d] bitrate[%d](%u)\n", pDevice->m_nKsDeviceNumber, index, pDevice->m_nCustomEncoderVideoBitRateProperty[ index ] );
														}
													}
													if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) ) {

														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pDevice->m_nCustomEncoderVideoBitRateProperty[ 0 ] = 0x88000000 | pVideo->m_nEncoderBitRate;
														}
													}
													break;


			case V4L2_CID_MPEG_VIDEO_QUALITY:		pVideo->m_nEncoderQuality = p_ext_control->value;


													if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) )
													{

														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pDevice->m_nAnalogVideoCompressionQualityProperty[ index ]  =   0x88000000 | pVideo->m_nEncoderQuality;
														}
													}
													if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) ) {

														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pDevice->m_nAnalogVideoCompressionQualityProperty[ 0 ]  =   0x88000000 | pVideo->m_nEncoderQuality;
														}
													}

													break;

			case V4L2_CID_MPEG_VIDEO_GOP_SIZE:		pVideo->m_nEncoderGop     = p_ext_control->value;


													if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) )
													{

														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pDevice->m_nAnalogVideoCompressionKeyframeRateProperty[ index ]  =   0x88000000 | pVideo->m_nEncoderGop;
														}
													}

													if( (pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001) ) {

														if( pVideo->m_nSubDeviceType == 0x00000002)
														{
															pDevice->m_nAnalogVideoCompressionKeyframeRateProperty[ 0 ]  =   0x88000000 | pVideo->m_nEncoderGop;
														}
													}
													break;

			case V4L2_CID_MPEG_VIDEO_FRAMERATE:		pVideo->m_nFrameRate      = p_ext_control->value;

													LINUXV4L2_DEBUG( KERN_INFO,"[%02d] m_nFrameRate(0d%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_nFrameRate );


													if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) )
													{

														if(pVideo->m_nSubDeviceType == 0x00000002)
														{
															ULONG avg = (pVideo->m_nFrameRate/1000);

															ULONG fps = pDevice->m_nCustomAnalogVideoFrameRateProperty;


															if( pDevice->m_nCustomAnalogVideoFrameRateProperty )
															{
																if( pDevice->m_nCustomAnalogVideoInterleavedProperty )
																{
																	fps /= 2;
																}

																if( avg > fps )
																{
																	avg = fps;
																}
															}
															pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ index ]  =   0x88000000 | avg;
														}
													}

													if( pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001 )
													{
														if(pVideo->m_nSubDeviceType == 0x00000002)
														{
															ULONG avg = (pVideo->m_nFrameRate/1000);

															ULONG fps = pDevice->m_nCustomAnalogVideoFrameRateProperty;


															if( pDevice->m_nCustomAnalogVideoFrameRateProperty )
															{
																if( pDevice->m_nCustomAnalogVideoInterleavedProperty )
																{
																	fps /= 2;
																}

																if( avg > fps )
																{
																	avg = fps;
																}
															}
															pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[ index ]  =   0x88000000 | avg;
														}
													}
													break;

			case V4L2_CID_MPEG_VIDEO_QPSTEP:		pVideo->m_nEncoderQPStep  = p_ext_control->value;

													break;

			case V4L2_CID_MPEG_VIDEO_MAX_QP:
			#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0)
			case V4L2_CID_MPEG_VIDEO_H264_MAX_QP:
			#endif

				pDevice->m_nCustomEncoderVideoMaxQPProperty[ 0 ] = p_ext_control->value; break;

			case V4L2_CID_MPEG_VIDEO_MIN_QP:
			#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0)
			case V4L2_CID_MPEG_VIDEO_H264_MIN_QP:
			#endif

				pDevice->m_nCustomEncoderVideoMinQPProperty[ 0 ] = p_ext_control->value; break;

			case V4L2_CID_MPEG_SET_KEY_FRAME:
			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,6,0)
			case V4L2_CID_MPEG_VIDEO_FORCE_KEY_FRAME:
			#endif

													if( pDevice->m_pKsDevice->vendor == 0x1AF2 && pDevice->m_pKsDevice->device == 0xA001 )
													{
														pDevice->m_nAnalogVideoCompressionOverrideKeyframeProperty[ 0 ] = 0x1;
													}
													if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

														(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) )
													{
														pDevice->m_nAnalogVideoCompressionOverrideKeyframeProperty[ index ] = 0x1;
													}

													break;

			case V4L2_CID_MPEG_VIDEO_OSD_LINE:

													{
														pDevice->m_nCustomOsdTextLineProperty[pVideo->m_nSubDeviceNumber] = (p_ext_control->value) % 8;
													}

													break;

			case V4L2_CID_MPEG_VIDEO_OSD_ENABLE:
													{
														pDevice->m_bCustomOsdTextEnableProperty = (p_ext_control->value) & 0x1;
													}

													break;
			case V4L2_CID_MPEG_VIDEO_OSD_X_POSITION:
													{
														pDevice->m_nCustomOsdTextPositionXProperty[ index ][ pDevice->m_nCustomOsdTextLineProperty[pVideo->m_nSubDeviceNumber] ] = (p_ext_control->value);
													}

													break;

			case V4L2_CID_MPEG_VIDEO_OSD_Y_POSITION:
													{
														pDevice->m_nCustomOsdTextPositionYProperty[ index ][ pDevice->m_nCustomOsdTextLineProperty[pVideo->m_nSubDeviceNumber] ] = (p_ext_control->value);
													}

													break;

			case V4L2_CID_MPEG_VIDEO_OSD_FONT_SIZE:
													{
														pDevice->m_nCustomOsdTextFontSizeProperty[ index ][ pDevice->m_nCustomOsdTextLineProperty[pVideo->m_nSubDeviceNumber] ] = (p_ext_control->value);
													}

													break;

			case V4L2_CID_MPEG_VIDEO_OSD_TEXT_COLOR:
													{
														pDevice->m_nCustomOsdTextColorProperty[ index ][ pDevice->m_nCustomOsdTextLineProperty[pVideo->m_nSubDeviceNumber] ] = (p_ext_control->value);
													}

													break;

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)

			case V4L2_CID_MPEG_VIDEO_OSD:
													break;

			#else

			case V4L2_CID_MPEG_VIDEO_OSD:
													if( pDevice->m_pKsDevice->vendor == 0x1797 && pDevice->m_pKsDevice->device == 0x5864 )
													{
														memcpy( pDevice->m_pCustomOsdTextStringProperty[pVideo->m_nSubDeviceNumber][ pDevice->m_nCustomOsdTextLineProperty[pVideo->m_nSubDeviceNumber] ], p_ext_control->reserved, 32 );

														pDevice->m_bCustomOsdTextStringRefreshProperty[pVideo->m_nSubDeviceNumber][ pDevice->m_nCustomOsdTextLineProperty[pVideo->m_nSubDeviceNumber] ] = TRUE;
													}
													else if( (pDevice->m_pKsDevice->vendor == 0x0907 && pDevice->m_pKsDevice->device == 0x0100) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0370) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0371) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0380) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0381) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0385) ||

															(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0375) )
													{
														pDevice->m_bCustomOsdTextStringRefreshProperty[pVideo->m_nSubDeviceNumber][ 0 ] = TRUE;

														memcpy( (CHAR *)(pDevice->m_pCustomOsdTextStringProperty[pVideo->m_nSubDeviceNumber][ 0 ]), p_ext_control->reserved, 64 );
													}
													else
													{
														memcpy( g_p_encoder_osd_text[ i ], p_ext_control->reserved, 64 );
													}

													break;


			#endif

			case V4L2_CID_MPEG_VIDEO_H264_PROFILE:
													{
														if(pVideo->m_nSubDeviceType == 0x00000002)
														{
															pDevice->m_nCustomEncoderVideoProfileProperty[pVideo->m_nSubDeviceNumber] = 0x88000000 | ((p_ext_control->value) & 0xFF);

															LINUXV4L2_DEBUG( KERN_INFO, "[%02d] m_nCustomEncoderVideoProfileProperty[%d] = 0x%x\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber, pDevice->m_nCustomEncoderVideoProfileProperty[pVideo->m_nSubDeviceNumber] );
														}
													}

													break;

			case V4L2_CID_MPEG_VIDEO_H264_LEVEL:
													{
														pDevice->m_nCustom_H264_Level_Property = 0x88000000 | ((p_ext_control->value) & 0xFF);
													}

													break;

			case V4L2_CID_MPEG_VIDEO_ASPECT:
													{
														if(pVideo->m_nSubDeviceType == 0x00000002)
														{
															pDevice->m_nCustomEncoderVideoAspectRatioProperty[pVideo->m_nSubDeviceNumber] = 0x88000000 | (p_ext_control->value);
														}
													}

													break;

			case V4L2_CID_MPEG_SET_SLICE:

													{
														if(pVideo->m_nSubDeviceType == 0x00000002)
														{
															pVideo->m_nCustomEncoderSlice = p_ext_control->value;

															LINUXV4L2_DEBUG( KERN_INFO,"[%02d] m_nCustomEncoderSlice(0x%x)\n", pVideo->m_pDevice->m_nKsDeviceNumber, pVideo->m_nCustomEncoderSlice);
														}
													}

													break;


			#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0)
			case V4L2_CID_MPEG_VIDEO_H264_ENTROPY_MODE:


													{
														if(pVideo->m_nSubDeviceType == 0x00000002)
														{
															pDevice->m_nCustomEncoderVideoEntropyProperty[ 0 ] = (p_ext_control->value & 0x1) ? 0 : 1;

															LINUXV4L2_DEBUG( KERN_INFO,"[%02d] m_nCustomEncoderVideoEntropyProperty[ 0 ](0x%x)\n", pDevice->m_nKsDeviceNumber, pDevice->m_nCustomEncoderVideoEntropyProperty[ 0 ]);
														}
													}
													break;
			#endif


			case V4L2_CID_MPEG_XET_DEBUG_EEPROM:
			{
				LINUXV4L2_DEBUG( KERN_INFO, "set V4L2_CID_XET_DEBUG_EEPROM\n" );
			}
			break;

			case V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_ACCESS:
			{
				g_n_debug_board_memory_access = p_ext_control->value;

				LINUXV4L2_DEBUG( KERN_INFO, "set V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_ACCESS() g_n_debug_board_memory_access(0x%x)\n", g_n_debug_board_memory_access );
			}
			break;

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0)
			case V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY:
			{
				LINUXV4L2_DEBUG( KERN_INFO, "set V4L2_CID_XET_DEBUG_BOARD_MEMORY\n" );

				if( g_n_debug_board_memory_access == 0x23921233 )
				{
					#if defined(AME_SC0710) || defined(AME_SC0720)

					if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) )
					{

						BYTE pPropertyValue[ 64 ] = { 0 };

						ULONG ret = 1;

						ret = copy_from_user( (void *)pPropertyValue, p_ext_control->ptr, sizeof(pPropertyValue) );

						if( ret == 0 )
						{
							down(&m_sComponentAccessSemaphore);

							SC0710_CLOSE_THREAD();
							{
								unsigned char txbuf[8] = { 0 };

								unsigned char rxbuf[8] = { 0 };

								int i = 0;

								SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

								if( p_sys_cfg->n_mcu_version == 0)
								{
									LINUXV4L2_PRINT( KERN_INFO, "V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY, mcu version = 0\n");
									return 0;
								}

								txbuf[0] = 0x12;
								txbuf[1] = 0x34;
								txbuf[2] = 0x65;

								mcu_i2c_access(pDevice,0x55, 3, txbuf, 3, rxbuf);

								LINUXV4L2_PRINT( KERN_INFO, "V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY (0x%x, 0x%x, 0x%x) \n", rxbuf[0], rxbuf[1], rxbuf[2]);

								for(i = 0;i < 50;i++)
								{
									if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x30)
										break;
									MySleep(100);

									mcu_i2c_access(pDevice,0x55, 0, txbuf, 3, rxbuf);

									LINUXV4L2_PRINT( KERN_INFO, "V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY() %d (0x%x, 0x%x, 0x%x) \n", i, rxbuf[0], rxbuf[1], rxbuf[2]);
								}

								MySleep(10);

								if(rxbuf[0] == 0x51 && rxbuf[1] == 0x10 && rxbuf[2] == 0x30)
								{


									for(i = 0;i < sizeof(pPropertyValue); i+=16)
									{
										ret = i2c_write_bytes(pDevice, 0x66, pPropertyValue + i, 16);
									}

									MySleep(10);
								}

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
									if(rxbuf[0] >= 17 &&  rxbuf[0] <= 37 && rxbuf[1] < 12 && rxbuf[2] < 31)
										break;
									MySleep(200);
									i++;
									if(i >= 50)

										break;
								}

								LINUXV4L2_PRINT( KERN_INFO, "V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY() end \n");
							}

							SC0710_START_THREAD();

							up(&m_sComponentAccessSemaphore);
						}
						else
						{
							LINUXV4L2_PRINT( KERN_ERR, "CAN NOT COPY DATA TO DRIVER\n");
						}
					}
					#endif


				}
			}
			break;

			case V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_2:
			{

			}
			break;

			case V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_3:
			{

			}
			break;

			case V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_4:
			{

			}
			break;


			case V4L2_CID_MPEG_XET_DEBUG_BOARD_MEMORY_5:
			{

			}
			break;

			case V4L2_CID_MPEG_VIDEO_OSD_PICTURE_PARM:
			{
			}
			break;

			case V4L2_CID_MPEG_VIDEO_OSD_PICTURE:
			{
			}
			break;

			#endif


			default: break;
			}
			p_ext_control = p_ext_control + size;

			count--;
		}
	}

	return 0;
}
static int common_video_device_vidioc_try_ext_ctrls( struct file *p_file, void * p_priv, struct v4l2_ext_controls * p_ext_controls)
{
	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo*)(video_drvdata( p_file ));

	struct v4l2_ext_control * p_ext_control = 0;

	int count = 0;

	int size = 0;

	int i = 0;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_try_ext_ctrls() pVideo == NULL\n" );

		return 0;
	}

	if( pVideo->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_video_device_vidioc_try_ext_ctrls() pVideo->m_pDevice == NULL\n" );

		return 0;
	}

	pDevice = pVideo->m_pDevice;

	p_ext_control = p_ext_controls->controls;

	count = p_ext_controls->count;

	size = sizeof(struct v4l2_ext_control);

	i = pDevice->m_nKsDeviceNumber * 4 + pVideo->m_nSubDeviceNumber * 1;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_video_device_vidioc_try_ext_ctrls()\n", pDevice->m_nKsDeviceNumber );

	return 0;
}

#endif



#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
static int vid_cap_queue_setup(struct vb2_queue *vq,
				unsigned int *nbuffers, unsigned int *nplanes,
				unsigned int sizes[], struct device *alloc_ctxs[])
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
static int vid_cap_queue_setup(struct vb2_queue *vq, const void *parg,
				unsigned int *nbuffers, unsigned int *nplanes,
				unsigned int sizes[], void *alloc_ctxs[])
#else
static int vid_cap_queue_setup(struct vb2_queue *vq, const struct v4l2_format *fmt,
				unsigned int *nbuffers, unsigned int *nplanes,
				unsigned int sizes[], void *alloc_ctxs[])
#endif
{
	CDevice * pDevice = NULL;

	CVideo * pVideo = (CVideo *)(vb2_get_drv_priv(vq));

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "queue_setup() pVideo == NULL\n" );

		return -EINVAL;
	}

	pDevice = pVideo->m_pDevice;

	if( pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "queue_setup() pDevice == NULL\n" );

		return -EINVAL;
	}

	sizes[0] = pVideo->m_nDstFrameSize;

	if( *nbuffers == 0 ) {

		*nbuffers = 32;
	}
    while( (sizes[0]) * (*nbuffers) > (7680 * 4320 * 2 * MAX_VB ) ) {

		*nbuffers = *nbuffers - 1;
	}

	*nplanes = 1;

	LINUXV4L2_PRINT( KERN_INFO, "[%02d] m_nSubDeviceNumber(%d) queue_setup( C:%02d, S:%02d ) memory(%s)\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber, *nbuffers, sizes[0], (vq->memory == VB2_MMAP) ? "MMAP" : "USERPTR");

	return 0;
}

static int vid_cap_buf_prepare(struct vb2_buffer *vb)
{
	unsigned long size;

	CDevice * pDevice = NULL;

	CVideo* pVideo = (CVideo *)vb2_get_drv_priv(vb->vb2_queue);

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "buffer_prepare() pVideo == NULL\n" );

		return -EINVAL;
	}

	pDevice = pVideo->m_pDevice;

	if( pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "buffer_prepare() pDevice == NULL\n" );

		return -EINVAL;
	}

	size = pVideo->m_nDstFrameSize;

	if( size == 0 )
	{
		size = 1920 * 1200 * 2;
	}

	if (vb2_plane_size(vb, 0) < size) {
		LINUXV4L2_DEBUG( KERN_INFO, "%s data will not fit into plane (%lu < %lu)\n",
				__func__, vb2_plane_size(vb, 0), size);
		return -EINVAL;
	}

	vb2_set_plane_payload(vb, 0, size);

	return 0;
}



#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
static void vid_cap_buf_cleanup(struct vb2_buffer *vb)
{
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
static void vid_cap_buf_finish(struct vb2_buffer *vb)
{

}
#endif

static void vid_cap_buf_queue(struct vb2_buffer *vb)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
#endif

	CVideo* pVideo = (CVideo *)vb2_get_drv_priv(vb->vb2_queue);

	CVideoBuffer * pVideoBuffer = 0;


	unsigned long flags = 0x00000000;

	if( pVideo == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "buffer_queue() pVideo == NULL\n" );

		return;
	}


	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
	pVideoBuffer = (CVideoBuffer *)(container_of(vbuf, CVideoBuffer, vb));
	#else
	pVideoBuffer = (CVideoBuffer *)(container_of(vb, CVideoBuffer, vb));
	#endif

	spin_lock_irqsave( &pVideo->m_sKsBufferListSpinLock, flags );

	list_add_tail( &pVideoBuffer->list, &pVideo->m_sKsBufferList );

	spin_unlock_irqrestore( &pVideo->m_sKsBufferListSpinLock, flags );
}

static int vid_cap_start_streaming(struct vb2_queue *vq, unsigned int count)
{
	CVideo * pVideo = (CVideo *)(vb2_get_drv_priv(vq));

	CDevice* pDevice = pVideo->m_pDevice;

	LINUXV4L2_PRINT( KERN_INFO, "[%02d] %s() count(0x%x) m_nSubDeviceNumber(%d)--\n", pDevice->m_nKsDeviceNumber, __func__, count, pVideo->m_nSubDeviceNumber );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
	cpu_latency_qos_add_request(&pVideo->pm_req, 1);
#else

	#ifdef DEF_OS_RHEL

	#pragma
	message("CENTOS 8.5, kernel 4.18.0-408 use cpu_latency_qos_add_request()")

	cpu_latency_qos_add_request(&pVideo->pm_req, 1);

	#else

	pm_qos_add_request(&pVideo->pm_req, PM_QOS_CPU_DMA_LATENCY, 1);

	#endif

#endif

	pDevice->m_nAnalogCopyProtMacrovisionEnableProperty = 0;


	pVideo->m_nFrameNumber = 0;
	pVideo->m_nFrameCopyPolarity = 0;
	pVideo->m_nLastReferenceClockTime = 0;
	pVideo->m_nLastPresentationTime = 0;
	pVideo->m_nNowPresentationTime = 0;
	pVideo->m_nLastPresentationTimeMultipler = 0;
	pVideo->m_nLastPresentationTimeFps = 0;
	pVideo->m_nLastPresentationTimeFpsCounts = 0;
	pVideo->m_nLastPresentationTimeFpsErrors = 0;

	pVideo->m_nFrameLastLine_cnt = 0;


	down( &m_sComponentAccessSemaphore );

	if(pVideo->m_nSubDeviceType == 0x00000001)
	{
#ifdef AME_SC0710

		if (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) { if (FALSE == SC0710_HwVideoComponentsInitialize(pDevice, pVideo)) { ; } }
#endif

	}
	up( &m_sComponentAccessSemaphore );

	if( pVideo->m_nSubDeviceType == 0x00000001 )
	{
		pDevice->m_eVideoStreamState[pVideo->m_nSubDeviceNumber] = 1;

		pVideo->m_nFirstFrameNumber = 0;

		pVideo->m_nFirstPresentationTime = 0;

		pVideo->m_previous_video_ring = 0;

		pVideo->m_Multi_Copy = 0;
	}

	if( pVideo->m_nSubDeviceType == 0x00000002 )
	{
		pDevice->m_eVideoEncoderStreamState[pVideo->m_nSubDeviceNumber] = 1;

		pVideo->m_nCustomEncoderKeyFrameArrived = 0;

		pVideo->m_nFirstFrameNumber = 0;

		pVideo->m_nFirstPresentationTime = 0;

		pVideo->m_previous_h264_ring = 0;

	}

	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)
static void vid_cap_stop_streaming(struct vb2_queue *vq)
#else
static int vid_cap_stop_streaming(struct vb2_queue *vq)
#endif
{
	CVideo * pVideo = (CVideo *)(vb2_get_drv_priv(vq));

	CDevice* pDevice = pVideo->m_pDevice;

	LINUXV4L2_PRINT( KERN_INFO, "[%02d] %s() m_nSubDeviceNumber(%d)\n", pDevice->m_nKsDeviceNumber, __func__, pVideo->m_nSubDeviceNumber );

	if( pVideo->m_nSubDeviceType == 0x00000001 )
	{
		pDevice->m_eVideoStreamState[pVideo->m_nSubDeviceNumber] = 0;

		#ifdef AME_SC0710
		if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 ) { SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty); p_sys_cfg->n_input_video_no_audio_packet = 0; }
		#endif
	}

	if( pVideo->m_nSubDeviceType == 0x00000002 )
	{
		pDevice->m_eVideoEncoderStreamState[pVideo->m_nSubDeviceNumber] = 0;

	}

	down( &m_sComponentAccessSemaphore );

		#ifdef AME_SC0710
		if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 ) { if( FALSE == SC0710_HwVideoComponentsUnInitialize( pDevice ) ) { ; } }
		#endif

	up( &m_sComponentAccessSemaphore );


	#if defined(AME_SC0710) || defined(AME_SC0720)
	if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
	{
		int ret = 0;


		flush_workqueue(pVideo->wq_multich_multicpy);


		ret = wait_event_timeout(pVideo->m_wait_queue_parsing_lines, pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder] == 0, msecs_to_jiffies( 60 ));

		if( ret <= 0 ){

			LINUXV4L2_DEBUG( KERN_ERR, "[%02d] ch(%d) SC710 time out, ret(0d%d)-----\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber, ret );
		}
		else {

			if(pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder] != 0)
			{
				LINUXV4L2_PRINT( KERN_INFO, "[%02d] ch(%d) m_MultiCh_MultiCpy(0x%x), error-----\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber, pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder]);
			}
			else
			{
				LINUXV4L2_DEBUG( KERN_INFO, "[%02d] ch(%d) success, m_MultiCh_MultiCpy(0x%x) ret(%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber, pVideo->m_MultiCh_MultiCpy[pVideo->m_strorder], ret );
			}
		}
	}
	else
	#endif
	{
		#if defined(AME_SC0710) || defined(AME_SC0720)

		flush_workqueue(pVideo->wq_blkcpy);

		#endif

		schedule_timeout_uninterruptible( msecs_to_jiffies( 100 ) );
	}

	{
		unsigned long flags = 0x00000000;


		spin_lock_irqsave( &pVideo->m_sKsBufferListSpinLock, flags );

		while (!list_empty(&pVideo->m_sKsBufferList)) {

			struct CVideoBuffer *buf = NULL;

			if( pVideo->m_sKsBufferList.next )
			{
				buf = list_entry(pVideo->m_sKsBufferList.next, struct CVideoBuffer, list);
			}
			else
			{
				LINUXV4L2_PRINT( KERN_INFO, "[%02d]	in list_empty, ch(%d), m_sKsBufferList.next 0\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber );
			}

			if( buf )
			{

				{
					#ifdef CONFIG_X86_64
					if( ( ((PVOID)(buf->list.next)) == LIST_POISON1) || (((PVOID)(buf->list.prev)) == LIST_POISON2) || ( ((ULONGLONG)(buf->list.next) & 0xFFFF000000000000) == 0xdead000000000000 ) || ( ((ULONGLONG)(buf->list.prev) & 0xFFFF000000000000) == 0xdead000000000000 ) || (((ULONGLONG)(buf->list.next) == 0x8080808080808080)) || (((ULONGLONG)(buf->list.prev)) == 0x8080808080808080) )
					#else
					if( ( ((PVOID)(buf->list.next)) == LIST_POISON1) || (((PVOID)(buf->list.prev)) == LIST_POISON2) || (((ULONG)(buf->list.next)) == 0xdead000000000100) || (((ULONG)(buf->list.prev)) == 0xdead000000000200)|| (((ULONG)(buf->list.next) == 0x8080808080808080)) || (((ULONG)(buf->list.prev)) == 0x8080808080808080) )
					#endif
					{

						LINUXV4L2_PRINT( KERN_INFO, "[%02d] error, next(%p) prev(%p)\n", pDevice->m_nKsDeviceNumber, buf->list.next, buf->list.prev );
						LINUXV4L2_PRINT( KERN_INFO, "[%02d] error, LIST_POISON1(%p) LIST_POISON2(%p)\n", pDevice->m_nKsDeviceNumber, LIST_POISON1, LIST_POISON2 );

						break;
					}
					else
					{
						list_del(&buf->list);
						vb2_buffer_done(&buf->vb2_buffer_cross, VB2_BUF_STATE_ERROR);

						#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
						LINUXV4L2_DEBUG( KERN_INFO,  "[%p/%d] done, set state to VB2_BUF_STATE_ERROR, ch(%d)\n", buf, buf->vb.vb2_buf.index, pVideo->m_nSubDeviceNumber );
						#else
						LINUXV4L2_DEBUG( KERN_INFO,  "[%p/%d] done, set state to VB2_BUF_STATE_ERROR, ch(%d)\n", buf, buf->vb.v4l2_buf.index, pVideo->m_nSubDeviceNumber );
						#endif
					}
				}




			}
			else
			{
				LINUXV4L2_PRINT( KERN_INFO, "[%02d]	list no entry, ch(%d)\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber );
			}
		}
		spin_unlock_irqrestore( &pVideo->m_sKsBufferListSpinLock, flags );
	}

	pVideo->m_nCustomAnalogVideoVbiLinesEnableProperty = 0;
	pVideo->m_nCustomAnalogVideoVbiLinesProperty = 0;
	pVideo->m_nCustomAnalogVideoVbiMixModeProperty = 0;

	pVideo->m_nFrameNumberForQueue = 0;


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
	cpu_latency_qos_remove_request(&pVideo->pm_req);
#else
	#ifdef DEF_OS_RHEL

	cpu_latency_qos_remove_request(&pVideo->pm_req);

	#else

	pm_qos_remove_request(&pVideo->pm_req);

	#endif

#endif

	#if LINUX_VERSION_CODE < KERNEL_VERSION(3,16,0)
	return 0;
	#endif

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] %s() ch(%d) end\n", pDevice->m_nKsDeviceNumber, __func__, pVideo->m_nSubDeviceNumber );

}
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,4,0)
static void vivi_lock(struct vb2_queue *vq)
{
	CVideo * pVideo = (CVideo *)(vb2_get_drv_priv(vq));

	if( pVideo )
	{
		mutex_lock(&pVideo->mutex);
	}
}

static void vivi_unlock(struct vb2_queue *vq)
{
	CVideo * pVideo = (CVideo *)(vb2_get_drv_priv(vq));

	if( pVideo )
	{
		mutex_unlock(&pVideo->mutex);
	}
}
#endif

static int common_snd_pcm_ops_open( struct snd_pcm_substream * p_substream )
{

	CDevice * pDevice = NULL;

	CAudio * pAudio = (CAudio *)(p_substream->pcm->card->private_data);

	int ret = 0;

	if( pAudio == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_open() pAudio == NULL\n" );

		return -EINVAL;
	}

	if( pAudio->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_open() pAudio->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pAudio->m_pDevice;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_open() pAudio(0p%p) p_substream(0p%p)\n", pDevice->m_nKsDeviceNumber, pAudio, p_substream );

	p_substream->runtime->private_data = NULL;

	p_substream->runtime->private_free = NULL;


	if( pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710 )
	{
		p_substream->runtime->hw = LINUXV4L2_SND_PCM_HARDWARE_SC0710;
	}

	ret = snd_pcm_hw_constraint_integer( p_substream->runtime, SNDRV_PCM_HW_PARAM_PERIODS );

	if( ret < 0 ) { return ret; }

	pAudio->m_pKsSubStream = p_substream;

	return 0;
}

static int common_snd_pcm_ops_close( struct snd_pcm_substream * p_substream )
{
	CAudio * pAudio = (CAudio *)(p_substream->pcm->card->private_data);

	if( pAudio == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_close() pAudio == NULL\n" );

		return -EINVAL;
	}

	if( pAudio->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_close() pAudio->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_close() pAudio(0p%p) p_substream(0p%p)\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber), pAudio, p_substream );

	pAudio->m_pKsSubStream = NULL;

	return 0;
}

static int common_snd_pcm_ops_hw_params( struct snd_pcm_substream * p_substream, struct snd_pcm_hw_params * p_hw_params )
{
	CAudio * pAudio = (CAudio *)(p_substream->pcm->card->private_data);

	CDevice* pDevice = NULL;

	if( pAudio == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_hw_params() pAudio == NULL\n" );

		return -EINVAL;
	}

	if( pAudio->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_hw_params() pAudio->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pAudio->m_pDevice;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_hw_params( CHANNELS:%d, PERIODS:%d, PERIOD SIZE:%d, PERIOD BYTES:%d, BUFFER SIZE:%d, BUFFER BYTES:%d ) pAudio(0p%p)\n",

		pDevice->m_nKsDeviceNumber,
		params_channels(p_hw_params),
		params_periods(p_hw_params),
		params_period_size(p_hw_params),
		params_period_bytes(p_hw_params),
		params_buffer_size(p_hw_params),
		params_buffer_bytes( p_hw_params ),
		pAudio );

	return snd_pcm_lib_malloc_pages( p_substream, params_buffer_bytes( p_hw_params ) );
}

static int common_snd_pcm_ops_hw_free( struct snd_pcm_substream * p_substream )
{
	CAudio * pAudio = (CAudio *)(p_substream->pcm->card->private_data);

	if( pAudio == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_hw_free() pAudio == NULL\n" );

		return -EINVAL;
	}

	if( pAudio->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_hw_free() pAudio->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_hw_free() pAudio(0p%p)\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber), pAudio );

	return snd_pcm_lib_free_pages( p_substream );
}

static int common_snd_pcm_ops_prepare( struct snd_pcm_substream * p_substream )
{
	CAudio * pAudio = (CAudio *)(p_substream->pcm->card->private_data);

	CDevice* pDevice = NULL;

	if( pAudio == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_prepare() pAudio == NULL\n" );

		return -EINVAL;
	}

	if( pAudio->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_prepare() pAudio->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	pDevice = pAudio->m_pDevice;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] pcm_ops_prepare() rate(0d%d)\n", pDevice->m_nKsDeviceNumber, p_substream->runtime->rate );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] pcm_ops_prepare() channels(0d%d)\n", pDevice->m_nKsDeviceNumber, p_substream->runtime->channels );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] pcm_ops_prepare() sample_bits(0d%d)\n", pDevice->m_nKsDeviceNumber, p_substream->runtime->sample_bits );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] pcm_ops_prepare() periods(0d%d)\n", pDevice->m_nKsDeviceNumber, p_substream->runtime->periods );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] pcm_ops_prepare() period_size(0d%ld)\n", pDevice->m_nKsDeviceNumber, p_substream->runtime->period_size );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] pcm_ops_prepare() frame_bits(0d%d)\n", pDevice->m_nKsDeviceNumber, p_substream->runtime->frame_bits );
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] pcm_ops_prepare() buffer_size(0d%ld)\n", pDevice->m_nKsDeviceNumber, p_substream->runtime->buffer_size );

	pAudio->m_nRate = p_substream->runtime->rate;

	pAudio->m_nChannels = p_substream->runtime->channels;

	pAudio->m_nSampleBits = p_substream->runtime->sample_bits;

	pAudio->m_nFrameSize = (int)(p_substream->runtime->period_size) * (int)(p_substream->runtime->frame_bits) / (int)(8);

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] pAudio->m_nFrameSize = %d \n", pDevice->m_nKsDeviceNumber, pAudio->m_nFrameSize );

	pAudio->m_nFrames = p_substream->runtime->periods;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] pAudio->m_nFrames = %d \n", pDevice->m_nKsDeviceNumber, pAudio->m_nFrames );

	{
		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_prepare() pAudio(0p%p)\n", pDevice->m_nKsDeviceNumber, pAudio );

		pAudio->m_nFrameNumber = 0;

		#if defined(AME_SC0710) || defined(AME_SC0720)

		#else
		pAudio->m_nFrameDMANumber = 0;
		#endif

		pAudio->m_audio_data_residue = 0;

	}

	return 0;
}


static int common_snd_pcm_ops_trigger( struct snd_pcm_substream * p_substream, int cmd )
{
	CAudio * pAudio = (CAudio *)(p_substream->pcm->card->private_data);

	CDevice* pDevice = NULL;

	if( pAudio == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_trigger() pAudio == NULL\n" );

		return -EINVAL;
	}

	if( pAudio->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_trigger() pAudio->m_pDevice == NULL\n" );

		return -EINVAL;
	}
	pDevice = pAudio->m_pDevice;



	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_trigger( %08X ) pAudio(0p%p) p_substream(0p%p)\n", pDevice->m_nKsDeviceNumber, cmd, pAudio, p_substream );

	if( cmd == SNDRV_PCM_TRIGGER_START ) {

		pAudio->m_nFrameNumber = 0;

		#if defined(AME_SC0710) || defined(AME_SC0720)

		#else
		pAudio->m_nFrameDMANumber = 0;
		#endif

		pAudio->m_audio_data_residue = 0;

		pAudio->n_output_audio_stream_1mins_diff_duration = 0;
		pAudio->m_nAudioResidue = 0;


		#if defined(AME_SC0710) || defined(AME_SC0720)
		if (pDevice->m_pKsDevice->vendor == 0x12AB && pAudio->m_pDevice->m_pKsDevice->device == 0x0710) { if (FALSE == SC0710_HwAudioComponentsInitialize(pDevice)) { ; } }
		if (pDevice->m_pKsDevice->vendor == 0x12AB && pAudio->m_pDevice->m_pKsDevice->device == 0x0720) { if (FALSE == SC0710_HwAudioComponentsInitialize(pDevice)) { ; } }
		if (pDevice->m_pKsDevice->vendor == 0x12AB && pAudio->m_pDevice->m_pKsDevice->device == 0x0750) { if (FALSE == SC0710_HwAudioComponentsInitialize(pDevice)) { ; } }
		#endif


		if( pAudio->m_nSubDeviceType == 0x00000001 ) { pDevice->m_eAudioStreamState[ pAudio->m_nSubDeviceNumber ] |= 1; }

		if( pAudio->m_nSubDeviceType == 0x00000002 ) { pDevice->m_eAudioEncoderStreamState[ pAudio->m_nSubDeviceNumber ] = 0x00000001; }

	}
	if( cmd == SNDRV_PCM_TRIGGER_STOP ) {

		if( pAudio->m_nSubDeviceType == 0x00000001 ) { pDevice->m_eAudioStreamState[ pAudio->m_nSubDeviceNumber ] &= ~1; }

		if( pAudio->m_nSubDeviceType == 0x00000002 ) { pDevice->m_eAudioEncoderStreamState[ pAudio->m_nSubDeviceNumber ] = 0x00000000; }

		if( pDevice->m_eAudioStreamState[ pAudio->m_nSubDeviceNumber ] == 0x00000000 )
		{

			#if defined(AME_SC0710) || defined(AME_SC0720)
			if (pDevice->m_pKsDevice->vendor == 0x12AB && pAudio->m_pDevice->m_pKsDevice->device == 0x0710) { if (FALSE == SC0710_HwAudioComponentsUnInitialize(pDevice)) { ; } }
			if (pDevice->m_pKsDevice->vendor == 0x12AB && pAudio->m_pDevice->m_pKsDevice->device == 0x0720) { if (FALSE == SC0710_HwAudioComponentsUnInitialize(pDevice)) { ; } }
			if (pDevice->m_pKsDevice->vendor == 0x12AB && pAudio->m_pDevice->m_pKsDevice->device == 0x0750) { if (FALSE == SC0710_HwAudioComponentsUnInitialize(pDevice)) { ; } }
			#endif
		}
		pAudio->m_nFrameNumber = 0;

		#if defined(AME_SC0710) || defined(AME_SC0720)

		#else
		pAudio->m_nFrameDMANumber = 0;
		#endif

		pAudio->m_audio_data_residue = 0;

	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_pcm_ops_trigger() end, pAudio(0p%p) m_eAudioStreamState(0x%x)\n", pDevice->m_nKsDeviceNumber, pAudio, pDevice->m_eAudioStreamState[ pAudio->m_nSubDeviceNumber ] );

	return 0;
}

static snd_pcm_uframes_t common_snd_pcm_ops_pointer( struct snd_pcm_substream * p_substream )
{
	CAudio * pAudio = (CAudio *)(p_substream->pcm->card->private_data);

	if( pAudio == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_pointer() pAudio == NULL\n" );

		return -EINVAL;
	}

	if( pAudio->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_pointer() pAudio->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	if( pAudio->m_nFrames )
	{

		return bytes_to_frames( p_substream->runtime, (pAudio->m_nFrameNumber % pAudio->m_nFrames) * (pAudio->m_nFrameSize) );

	}
	else
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_pcm_ops_pointer() pAudio->m_nFrames == NULL\n" );

		return 0;
	}
}

static int common_snd_kcontrol_new_volume_info( struct snd_kcontrol * p_kcontrol, struct snd_ctl_elem_info * p_info )
{
	CAudio * pAudio = (CAudio *)(snd_kcontrol_chip( p_kcontrol ));

	if( pAudio == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_kcontrol_new_volume_info() pAudio == NULL\n" );

		return -EINVAL;
	}

	if( pAudio->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_kcontrol_new_volume_info() pAudio->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_kcontrol_new_volume_info()\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber) );

	p_info->type = SNDRV_CTL_ELEM_TYPE_INTEGER;

	p_info->count = 1;

	p_info->value.integer.min = 0;

	p_info->value.integer.max = 255;

	return 0;
}

static int common_snd_kcontrol_new_volume_get( struct snd_kcontrol * p_kcontrol, struct snd_ctl_elem_value * p_value )
{
	CAudio * pAudio = (CAudio *)(snd_kcontrol_chip( p_kcontrol ));

	if( pAudio == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_kcontrol_new_volume_get() pAudio == NULL\n" );

		return -EINVAL;
	}

	if( pAudio->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_kcontrol_new_volume_get() pAudio->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_kcontrol_new_volume_get()\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber) );


	if( pAudio->m_pDevice->m_pKsDevice->vendor == 0x1AF2 && pAudio->m_pDevice->m_pKsDevice->device == 0xA001 )
	{
		p_value->value.integer.value[ 0 ] = pAudio->m_nVolume;
	}

	if( (pAudio->m_pDevice->m_pKsDevice->vendor == 0x0907 && pAudio->m_pDevice->m_pKsDevice->device == 0x0100) ||

		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x12AB && pAudio->m_pDevice->m_pKsDevice->device == 0x0370) ||

		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x12AB && pAudio->m_pDevice->m_pKsDevice->device == 0x0371) ||

		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x12AB && pAudio->m_pDevice->m_pKsDevice->device == 0x0380) ||

		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x12AB && pAudio->m_pDevice->m_pKsDevice->device == 0x0381) ||

		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x12AB && pAudio->m_pDevice->m_pKsDevice->device == 0x0385) ||

		(pAudio->m_pDevice->m_pKsDevice->vendor == 0x12AB && pAudio->m_pDevice->m_pKsDevice->device == 0x0375) )
	{
		p_value->value.integer.value[ 0 ] = pAudio->m_nVolume;
	}
	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_kcontrol_new_volume_get() channel(%d) volume(0d%ld)\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber), pAudio->m_nSubDeviceNumber, p_value->value.integer.value[ 0 ] );


	return 0;
}

static int common_snd_kcontrol_new_volume_put( struct snd_kcontrol * p_kcontrol, struct snd_ctl_elem_value * p_value )
{
	CAudio * pAudio = (CAudio *)(snd_kcontrol_chip( p_kcontrol ));

	if( pAudio == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_kcontrol_new_volume_put() pAudio == NULL\n" );

		return -EINVAL;
	}

	if( pAudio->m_pDevice == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "common_snd_kcontrol_new_volume_put() pAudio->m_pDevice == NULL\n" );

		return -EINVAL;
	}

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] common_snd_kcontrol_new_volume_put() channel(%d) volume(%d)\n", (int)(pAudio->m_pDevice->m_nKsDeviceNumber), pAudio->m_nSubDeviceNumber, (int)(p_value->value.integer.value[ 0 ]) );

	pAudio->m_nVolume = p_value->value.integer.value[ 0 ];

	if( pAudio->m_nVolume > 255 ) { pAudio->m_nVolume = 255; }

	if( pAudio->m_nVolume <   0 ) { pAudio->m_nVolume =   0; }

	return 0;
}


NTSTATUS OnSetCustomDebugEdidAccessEnableProperty( CVideo * pVideo, ULONG PropertyValue, ULONG size )
{
	CDevice * pDevice = pVideo->m_pDevice;

	LINUXV4L2_PRINT( KERN_INFO, "[%02d] OnSetCustomDebugEdidAccessEnableProperty( %08X )\n", pDevice->m_nKsDeviceNumber, PropertyValue);

	#if defined(AME_SC0710) || defined(AME_SC0720)
	{
		BYTE n_edid_addr = 0x50;

		BYTE n_is_mcu_sc400n1_edid_updated = 0;

		if( ((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0501) && (pDevice->iManufacturer == 0x1F)) ) {

			n_edid_addr = 0x50;
		}
		if( ((pDevice->bcdDevice == 0xA001) && (pDevice->iManufacturer == 0x1F)) ) {

			ULONG ch = pVideo->m_nSubDeviceNumber % 4;

			if( pDevice->m_nAnalogCrossbarVideoInputPropertys[ ch ] == 0 ) { n_edid_addr = 0x50; }

			if( pDevice->m_nAnalogCrossbarVideoInputPropertys[ ch ] == 1 ) { n_edid_addr = 0x51; }

			if( pDevice->m_nAnalogCrossbarVideoInputPropertys[ ch ] == 3 ) { n_edid_addr = 0x52; }
		}
		if( ((pDevice->bcdDevice == 0x1101) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0xA101) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0x0101) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1E)) ||
			((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F)) ||
			((pDevice->bcdDevice == 0x0102) && (pDevice->iManufacturer == 0x1F)) ) {

			ULONG ch = pVideo->m_nSubDeviceNumber % 4;

			if( pDevice->m_nAnalogCrossbarVideoInputPropertys[ ch ] == 0 ) n_edid_addr = 0x50;

			if( pDevice->m_nAnalogCrossbarVideoInputPropertys[ ch ] == 3 ) n_edid_addr = 0x51;
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
			((pDevice->bcdDevice == 0x0601) && (pDevice->iManufacturer == 0x1F)) ) {

			n_is_mcu_sc400n1_edid_updated = 1;
		}
		else{

			n_is_mcu_sc400n1_edid_updated = 0;
		}

		if( PropertyValue == 0 ) {

			SC0710_CLOSE_THREAD();

			if( g_b_edid_i2c_data[ 0 ] == 0x00 &&

				g_b_edid_i2c_data[ 1 ] == 0xFF &&

				g_b_edid_i2c_data[ 2 ] == 0xFF &&

				g_b_edid_i2c_data[ 7 ] == 0x00 &&

				g_b_is_write_mode_enabled ) {

				ULONG n_is_edid_inside_mcu = 0;

				unsigned int checksum = 0;

				ULONG i = 0;

				g_b_is_write_mode_enabled = FALSE;

				if( n_is_mcu_sc400n1_edid_updated == 1 ) {

					n_is_edid_inside_mcu = 1;
				}
				else if (((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2D))) {

					ULONG ch = pVideo->m_nSubDeviceNumber % 4;

					SC0710_EableEDIDAccess(pDevice, ch);

					n_is_edid_inside_mcu = 1;
				}
				else {

					BYTE rxbuf[ 8 ] = { 0x00 };

					BYTE txbuf = 0;

					SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, 0x50, &txbuf, 1, rxbuf, 8 );

					if( (rxbuf[ 0 ] == 0x00) &&
						(rxbuf[ 1 ] == 0xFF) &&
						(rxbuf[ 2 ] == 0xFF) &&
						(rxbuf[ 3 ] == 0xFF) &&
						(rxbuf[ 4 ] == 0xFF) &&
						(rxbuf[ 5 ] == 0xFF) &&
						(rxbuf[ 6 ] == 0xFF) &&
						(rxbuf[ 7 ] == 0x00) ) {

						n_is_edid_inside_mcu = 1;
					}
				}
				if( n_is_edid_inside_mcu ) {

					for( i = 0 ; i < size ; i++ ) {

						checksum += (unsigned char)(g_b_edid_i2c_data[ i ]);
					}
					checksum &= 0xFF;

					if( checksum == 0 ) {

						LINUXV4L2_PRINT( KERN_INFO,"[WRITE.EDID]");

						if( n_is_mcu_sc400n1_edid_updated == 1 ) {

							SetMCUPolling( pDevice, 1 );


							for( i = 0 ; i < 256 ; i += 4 ) {

								BYTE txbuf[ 17 ] = { 0 };

								txbuf[ 0 ] = (BYTE)(i);

								memcpy( txbuf + 1, g_b_edid_i2c_data + i, 4 );

								mcu_i2c_access( pDevice, n_edid_addr, 5, txbuf, 0, NULL );

								DELAY_100NS( 100000 );
							}
							SetMCUPolling( pDevice, 0 );
						}
						else {

							for( i = 0 ; i < 256 ; i += 16 ) {

								BYTE txbuf[ 17 ] = { 0 };

								txbuf[ 0 ] = (BYTE)(i);

								memcpy( txbuf + 1, g_b_edid_i2c_data + i, 16 );





								if( ((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1E)) ||
									((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F)) ||
									((pDevice->bcdDevice == 0x0102) && (pDevice->iManufacturer == 0x1F)) ) {

									if( pDevice->m_nAnalogCrossbarVideoInputPropertys[pVideo->m_nSubDeviceNumber] == 0 ) n_edid_addr = 0x50;

									if( pDevice->m_nAnalogCrossbarVideoInputPropertys[pVideo->m_nSubDeviceNumber] == 3 ) n_edid_addr = 0x51;
								}
								SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, n_edid_addr, txbuf, 17, NULL, 0 );

								DELAY_100NS( 100000 );
							}
						}
						DELAY_100NS( 2000000 );
					}
				}
				else {

					for( i = 0 ; i < size ; i++ ) {

						checksum += (unsigned char)(g_b_edid_i2c_data[ i ]);
					}
					checksum &= 0xFF;

					if( checksum == 0 ) {

						LINUXV4L2_PRINT( KERN_INFO,"[UPDATE.EDID]\n");

						UpdateEDID( pDevice, g_b_edid_i2c_data );
					}
					else
					{
						LINUXV4L2_PRINT( KERN_INFO,"[UPDATE.EDID] checksum(0x%x) ERROR!!!\n", checksum );
					}
				}
			}
			SC0710_START_THREAD();

			LINUXV4L2_PRINT( KERN_INFO,"<----------\n");
		}
		else {

			ULONG i = 0;

			ULONG n_is_edid_inside_mcu = 0;

			LINUXV4L2_PRINT( KERN_INFO,"---------->\n");

			SC0710_CLOSE_THREAD();

			if( n_is_mcu_sc400n1_edid_updated == 1 ) {

				n_is_edid_inside_mcu = 1;
			}
			else {

				BYTE rxbuf[ 8 ] = { 0x00 };

				BYTE txbuf = 0;

				SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, 0x50, &txbuf, 1, rxbuf, 8 );

				if( (rxbuf[ 0 ] == 0x00) &&
					(rxbuf[ 1 ] == 0xFF) &&
					(rxbuf[ 2 ] == 0xFF) &&
					(rxbuf[ 3 ] == 0xFF) &&
					(rxbuf[ 4 ] == 0xFF) &&
					(rxbuf[ 5 ] == 0xFF) &&
					(rxbuf[ 6 ] == 0xFF) &&
					(rxbuf[ 7 ] == 0x00) ) {

					n_is_edid_inside_mcu = 1;
				}
			}
			RtlFillMemory( (BYTE *)(g_b_edid_i2c_data), 256, 0x00 );

			g_b_is_write_mode_enabled = 0;

			if( n_is_edid_inside_mcu ) {

				LINUXV4L2_PRINT( KERN_INFO,"[READ.EDID]");

				if( n_is_mcu_sc400n1_edid_updated == 1 ) {

					SetMCUPolling( pDevice, 1 );

					for( i = 0 ; i < size ; i += 4 ) {

						BYTE txbuf[ 16 ] = { 0 };



						txbuf[ 0 ] = (BYTE)(i);

						mcu_i2c_access( pDevice, n_edid_addr, 1, txbuf, 4, (g_b_edid_i2c_data + i) );

						DELAY_100NS( 100000 );
					}
					SetMCUPolling( pDevice, 0 );
				}
				else {

					for( i = 0 ; i < size ; i += 16 ) {

						BYTE txbuf = (BYTE)i;

						BYTE rxbuf[ 16 ] = { 0 };




						BYTE n_edid_addr = 0x50;

						if( ((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1E)) ||
							((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x1F)) ||
							((pDevice->bcdDevice == 0x0102) && (pDevice->iManufacturer == 0x1F)) ) {

							if( pDevice->m_nAnalogCrossbarVideoInputPropertys[pVideo->m_nSubDeviceNumber] == 0 ) n_edid_addr = 0x50;

							if( pDevice->m_nAnalogCrossbarVideoInputPropertys[pVideo->m_nSubDeviceNumber] == 3 ) n_edid_addr = 0x51;
						}
						SC0710_AccessSlaveDeviceRegisterS( pDevice, 0, n_edid_addr, &txbuf, 1, rxbuf, 16 );

						RtlCopyMemory( g_b_edid_i2c_data + i, rxbuf, 16 );

						DELAY_100NS( 100000 );
					}
				}
			}
			else {

				if( PropertyValue == 3 ) {

					LINUXV4L2_PRINT( KERN_INFO,"[READ.MERGED.EDID]");

					ReadEDID( pDevice, (BYTE *)(g_b_edid_i2c_data), 0x68 );
				}
				else if( PropertyValue == 2 ) {

					LINUXV4L2_PRINT( KERN_INFO,"[READ.PASS.MONITOR.EDID]\n");

					ReadEDID( pDevice, (BYTE *)(g_b_edid_i2c_data), 0x62 );
				}
				else {

					LINUXV4L2_PRINT( KERN_INFO,"[READ.EDID]\n");

					ReadEDID( pDevice, (BYTE *)(g_b_edid_i2c_data), 0x5C );
				}
			}
			SC0710_START_THREAD();
		}
	}
	#endif



	return STATUS_SUCCESS;
}

NTSTATUS OnSetCustomDebugEdidI2cSubProperty( CVideo * pVideo, ULONG PropertyValue )
{
	g_n_edid_i2c_sub = PropertyValue;

	printk( KERN_INFO "EdidI2cSub(0x%x) ", g_n_edid_i2c_sub);

	return STATUS_SUCCESS;
}

NTSTATUS OnGetCustomDebugEdidI2cDataProperty( CVideo * pVideo, ULONG* pPropertyValue )
{
	LINUXV4L2_PRINT( KERN_INFO, "CDevice::OnGetCustomDebugEdidI2cDataProperty()\n");

	*pPropertyValue = 0;

	#if defined(AME_SC0700) || defined(AME_SC0710) || defined(AME_SC0720)
	{
		*pPropertyValue = g_b_edid_i2c_data[ g_n_edid_i2c_sub % 256 ];
	}
	#endif



	return STATUS_SUCCESS;
}

NTSTATUS OnSetCustomDebugEdidI2cDataProperty( CVideo * pVideo, ULONG PropertyValue )
{
	CDevice * pDevice = pVideo->m_pDevice;

	LINUXV4L2_PRINT( KERN_INFO, "[%02d] OnSetCustomDebugEdidI2cDataProperty() PropertyValue(0x%x)\n", pDevice->m_nKsDeviceNumber, PropertyValue);

	#if defined(AME_SC0700) || defined(AME_SC0710) || defined(AME_SC0720)
	{
		g_b_edid_i2c_data[ g_n_edid_i2c_sub % 256 ] = (BYTE)(PropertyValue);

		g_b_is_write_mode_enabled = TRUE;
	}
	#endif



	return STATUS_SUCCESS;
}


static ULONG  g_n_mcu_i2c_sub = 0x00;

#if defined(AME_MZ0380) || defined(AME_SC0710) || defined(AME_SC0720)
static ULONG  g_n_mcu_firmware_size = 0;
static BYTE * g_p_mcu_i2c_data = NULL;
#endif

NTSTATUS OnSetCustomDebugMcuAccessEnableProperty( CVideo* pVideo, ULONG pPropertyValue )
{
	CDevice* pDevice = pVideo->m_pDevice;

	LINUXV4L2_PRINT( KERN_INFO, "OnSetCustomDebugMcuAccessEnableProperty( %08X )\n", pPropertyValue);



	#if defined (AME_SC0710) || defined (AME_SC0720)
	{
		if( pPropertyValue == 0 ) {

			SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

			if( p_sys_cfg->n_mcu_version > 0 ) {

				unsigned char aprom[ 3 ] = { 0, 0, 0 };

				unsigned char mcucode[ 3 ] = { 0, 0, 0 };

				unsigned char ldrom[ 1 ] = { 0 };

				GetMCUROMVersion( pDevice, aprom, mcucode );

				if( aprom[ 0 ] != 0 ) {

					Jump2LDROM( pDevice, ldrom );
				}
				if( UpdateAPROM( pDevice, NULL, g_p_mcu_i2c_data, g_n_mcu_firmware_size ) != 0 ) {

					p_sys_cfg->n_mcu_version = 0;
				}
			}

			FREE_MEMORY( g_p_mcu_i2c_data )

			g_n_mcu_firmware_size = 0;

			SC0710_START_THREAD();

			LINUXV4L2_PRINT( KERN_INFO, "<----------\n");
		}
		else {

			LINUXV4L2_PRINT( KERN_INFO, "---------->\n");

			SC0710_CLOSE_THREAD();

			g_n_mcu_firmware_size = pPropertyValue;

			FREE_MEMORY( g_p_mcu_i2c_data )

			g_p_mcu_i2c_data = vmalloc( g_n_mcu_firmware_size );

			g_n_mcu_i2c_sub = 0;
		}
	}

	#endif

	return STATUS_SUCCESS;
}

NTSTATUS OnSetCustomDebugExtendMcuAccessEnableProperty( CVideo* pVideo, ULONG pPropertyValue )
{
	CDevice* pDevice = pVideo->m_pDevice;

	LINUXV4L2_PRINT( KERN_INFO, "OnSetCustomDebugExtendMcuAccessEnableProperty( %08X )\n", pPropertyValue);

	#ifdef AME_SC0710
	{
		if( pPropertyValue == 0 ) {

			SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

			if( p_sys_cfg->n_mcu_version > 0 ) {

				Update2ndMCU( pDevice, 0x69, g_p_mcu_i2c_data, g_n_mcu_firmware_size);
			}

			FREE_MEMORY( g_p_mcu_i2c_data )

			g_n_mcu_firmware_size = 0;

			SC0710_START_THREAD();

			LINUXV4L2_PRINT( KERN_INFO, "<----------\n");
		}
		else {

			LINUXV4L2_PRINT( KERN_INFO, "---------->\n");

			SC0710_CLOSE_THREAD();

			g_n_mcu_firmware_size = pPropertyValue;

			FREE_MEMORY( g_p_mcu_i2c_data )

			g_p_mcu_i2c_data = vmalloc( g_n_mcu_firmware_size );

			g_n_mcu_i2c_sub = 0;
		}
	}

	#endif

	return STATUS_SUCCESS;
}

NTSTATUS OnSetCustomDebugMcuI2cSubProperty( CVideo* pVideo, ULONG pPropertyValue )
{
	g_n_mcu_i2c_sub = pPropertyValue;

	return STATUS_SUCCESS;
}

NTSTATUS OnGetCustomDebugMcuI2cDataProperty( CVideo* pVideo, ULONG* pPropertyValue )
{


	#if defined(AME_MZ0380) || defined(AME_SC0710) || defined(AME_SC0720)
	{
		if( g_p_mcu_i2c_data != NULL && g_n_mcu_firmware_size > 0 ) {

		   *pPropertyValue = g_p_mcu_i2c_data[ g_n_mcu_i2c_sub % g_n_mcu_firmware_size ];
		}
		else {

		   *pPropertyValue = 0;
		}
	}
	#else
	{
	   *pPropertyValue = 0;
	}
	#endif

	return STATUS_SUCCESS;
}

NTSTATUS OnSetCustomDebugMcuI2cDataProperty( CVideo * pVideo, ULONG pPropertyValue )
{


	#if defined(AME_MZ0380) || defined(AME_SC0710) || defined(AME_SC0720)
	{

		if( g_p_mcu_i2c_data != NULL && g_n_mcu_firmware_size > 0 ) {

			g_p_mcu_i2c_data[ g_n_mcu_i2c_sub % g_n_mcu_firmware_size ] = (BYTE)(pPropertyValue);
		}
		else {

			;
		}
	}
	#endif

	return STATUS_SUCCESS;
}

extern ULONG g_n_mz0380_basefirmware_download_percentage;

BOOLEAN MZ0380_DownloadBaseFirmware( CDevice * pDevice, BYTE* pwszFileName, ULONG nFwType );

NTSTATUS OnSetCustomDebugEepromPidVidProperty(  CVideo * pVideo, ULONG pPropertyValue )
{
	BOOLEAN returns = FALSE;


	return returns;
}

NTSTATUS OnGetCustomDebugEepromPidVidProperty( CVideo * pVideo, ULONG* pPropertyValue )
{

	LINUXV4L2_PRINT( KERN_INFO,"CDevice::OnGetCustomDebugEepromPidVidProperty(%d)\n", *pPropertyValue);

	return STATUS_SUCCESS;
}

int Initialize_pVideo( CVideo* pVideo, CDevice* pDevice, int i)
{
	struct vb2_queue *q = NULL;

	if( pVideo == NULL || pDevice == NULL )
	{
		LINUXV4L2_PRINT( KERN_ERR,"[%02d] Initialize_pVideo() pVideo(0p%p) pDevice(0p%p)\n", g_n_counter_of_video_number, pVideo, pDevice );

		return FALSE;
	}

	pVideo->m_pDevice                         = pDevice;

	pVideo->m_nSubDeviceNumber                = i;

	if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0700) ||

		(pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0701) )
	{
		pVideo->m_nSrcFrameWidth                  = 1920;
		pVideo->m_nSrcFrameHeight                 = 1080;
		pVideo->m_nDstFrameWidth                  = 1920;
		pVideo->m_nDstFrameHeight                 = 1080;
		pVideo->m_nDstFramePitch                  = 1920 * 2;
		pVideo->m_nDstFrameSize                   = 1920 * 1080 * 2;
		pVideo->m_nFramePixelFormat               = V4L2_PIX_FMT_YUYV;
		pVideo->m_nFramePixelField                = V4L2_FIELD_NONE;
		pVideo->m_nFrameRate                      = 60;
	}


	q = &pVideo->vb_vidq;
	q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	q->io_modes = VB2_MMAP | VB2_READ;

	q->drv_priv = pVideo;
	q->buf_struct_size = sizeof(struct CVideoBuffer);
	q->ops = &vivi_video_qops;
	q->mem_ops = &vb2_vmalloc_memops;


	#if defined(AME_SC0710) || defined(AME_SC0720)

	{
		INIT_WORK(&pVideo->w_multich_multicopy[0], MultiCh_MultiCopy_0);
		INIT_WORK(&pVideo->w_multich_multicopy[1], MultiCh_MultiCopy_1);
		INIT_WORK(&pVideo->w_multich_multicopy[2], MultiCh_MultiCopy_2);
		INIT_WORK(&pVideo->w_multich_multicopy[3], MultiCh_MultiCopy_3);

		spin_lock_init(&(pVideo->m_sMultiCh_MultiCopySpinLock));
	}

	{
		INIT_WORK(&pVideo->w_blkcpy_yuyv_bgr[0], BlkCpy_YUYV_BGR_0);
		INIT_WORK(&pVideo->w_blkcpy_yuyv_bgr[1], BlkCpy_YUYV_BGR_1);
		INIT_WORK(&pVideo->w_blkcpy_yuyv_bgr[2], BlkCpy_YUYV_BGR_2);
		INIT_WORK(&pVideo->w_blkcpy_yuyv_bgr[3], BlkCpy_YUYV_BGR_3);
		INIT_WORK(&pVideo->w_blkcpy_yuyv_bgr[4], BlkCpy_YUYV_BGR_4);
		INIT_WORK(&pVideo->w_blkcpy_yuyv_bgr[5], BlkCpy_YUYV_BGR_5);
		INIT_WORK(&pVideo->w_blkcpy_yuyv_bgr[6], BlkCpy_YUYV_BGR_6);

		INIT_WORK(&pVideo->w_blkcpy_y[0], BlkCpy_Y_0);
		INIT_WORK(&pVideo->w_blkcpy_y[1], BlkCpy_Y_1);
		INIT_WORK(&pVideo->w_blkcpy_y[2], BlkCpy_Y_2);
		INIT_WORK(&pVideo->w_blkcpy_y[3], BlkCpy_Y_3);
		INIT_WORK(&pVideo->w_blkcpy_y[4], BlkCpy_Y_4);
		INIT_WORK(&pVideo->w_blkcpy_y[5], BlkCpy_Y_5);
		INIT_WORK(&pVideo->w_blkcpy_y[6], BlkCpy_Y_6);

		INIT_WORK(&pVideo->w_blkcpy_nv12[0], BlkCpy_C_NV12_0);
		INIT_WORK(&pVideo->w_blkcpy_nv12[1], BlkCpy_C_NV12_1);
		INIT_WORK(&pVideo->w_blkcpy_nv12[2], BlkCpy_C_NV12_2);
		INIT_WORK(&pVideo->w_blkcpy_nv12[3], BlkCpy_C_NV12_3);
		INIT_WORK(&pVideo->w_blkcpy_nv12[4], BlkCpy_C_NV12_4);
		INIT_WORK(&pVideo->w_blkcpy_nv12[5], BlkCpy_C_NV12_5);
		INIT_WORK(&pVideo->w_blkcpy_nv12[6], BlkCpy_C_NV12_6);

		INIT_WORK(&pVideo->w_blkcpy_yv12[0], BlkCpy_C_YV12_0);
		INIT_WORK(&pVideo->w_blkcpy_yv12[1], BlkCpy_C_YV12_1);
		INIT_WORK(&pVideo->w_blkcpy_yv12[2], BlkCpy_C_YV12_2);
		INIT_WORK(&pVideo->w_blkcpy_yv12[3], BlkCpy_C_YV12_3);
		INIT_WORK(&pVideo->w_blkcpy_yv12[4], BlkCpy_C_YV12_4);
		INIT_WORK(&pVideo->w_blkcpy_yv12[5], BlkCpy_C_YV12_5);
		INIT_WORK(&pVideo->w_blkcpy_yv12[6], BlkCpy_C_YV12_6);

		INIT_WORK(&pVideo->w_blkcpy_p010[0], BlkCpy_P010_0);
		INIT_WORK(&pVideo->w_blkcpy_p010[1], BlkCpy_P010_1);
		INIT_WORK(&pVideo->w_blkcpy_p010[2], BlkCpy_P010_2);
		INIT_WORK(&pVideo->w_blkcpy_p010[3], BlkCpy_P010_3);
		INIT_WORK(&pVideo->w_blkcpy_p010[4], BlkCpy_P010_4);
		INIT_WORK(&pVideo->w_blkcpy_p010[5], BlkCpy_P010_5);
		INIT_WORK(&pVideo->w_blkcpy_p010[6], BlkCpy_P010_6);


	}

	#endif

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0)
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
	q->timestamp_type = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	#else
	#endif

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0)
	q->dev = pDevice->v4l2_dev.dev;
	#endif

	q->lock = &pVideo->mutex;
	{
		int ret = vb2_queue_init(q);
		if (ret)
		{
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] Initialize_pVideo() vb2_queue_init() fail\n", g_n_counter_of_board_number );

			return FALSE;
		}
		else
		{
			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] Initialize_pVideo() vb2_queue_init() success(0x%x)\n", g_n_counter_of_board_number, i );
		}
	}

	mutex_init(&pVideo->mutex);


	INIT_LIST_HEAD( &(pVideo->m_sKsBufferList) );

	spin_lock_init( &(pVideo->m_sKsBufferListSpinLock) );


	pVideo->m_nCustomAnalogOutputVideoAudioProperty = 1;

	init_waitqueue_head(&pVideo->m_wait_queue_parsing_lines);

#if defined(AME_SC0710) || defined(AME_SC0720)

	pVideo->wq_multich_multicpy = alloc_workqueue("wq_multich_multicpy", WQ_UNBOUND | WQ_HIGHPRI | WQ_CPU_INTENSIVE | __WQ_ORDERED, 0);

	if (pVideo->wq_multich_multicpy == NULL) {

		LINUXV4L2_PRINT(KERN_ERR, "[%02d] alloc_workqueue() wq_multich_multicpy - [FAIL]\n", g_n_counter_of_board_number);

		return FALSE;
	}

	pVideo->wq_blkcpy = alloc_workqueue("wq_blkcpy", WQ_UNBOUND | WQ_HIGHPRI | WQ_CPU_INTENSIVE | __WQ_ORDERED, 0);

	if (pVideo->wq_blkcpy == NULL) {

		LINUXV4L2_PRINT( KERN_ERR, "[%02d] alloc_workqueue() - alloc_workqueue() wq_blkcpy - [FAIL]\n", (int)(g_n_counter_of_board_number) );

		return FALSE;
	}
#endif

	spin_lock_init( &(pVideo->m_sNo_Signal_SpinLock) );



	return TRUE;
}

#if defined(AME_MZ0380) || defined(AME_SC0700) || defined(AME_SC0710) || defined(AME_SC0720)
static void common_pci_driver_on_tasklet_video( unsigned long p_context )
{
	CDevice * pDevice = (CDevice *)(p_context);

	{
		#if defined(AME_SC0710) || defined(AME_SC0720)

		if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) )
		{
			unsigned long flags = 0x00000000;


			spin_lock_irqsave( &pDevice->m_sDPC_Video_SpinLock, flags );

			if( pDevice->m_nTaskletExtraParameterA == 1 )
			{

				SC0710_HwProcessAnalogPCIPacket( pDevice, pDevice->m_nTaskletExtraParameterA );


				pDevice->m_nTaskletExtraParameterA = 0;

				spin_unlock_irqrestore( &pDevice->m_sDPC_Video_SpinLock, flags );


				if( !pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty )
				{

					{
						SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

						XDMA_ENGINE_REGS * p_regs_xdma_engine_video = (XDMA_ENGINE_REGS *)(p_sys_cfg->p_config_bar_address + SC0710_REGBASE_BLOCK_OFFSET);

						XDMA_IRQ_REGS * p_regs_interrupt = (XDMA_IRQ_REGS *)(p_sys_cfg->p_config_bar_address + SC0710_REGBASE_IRQ);

						{
							ULONG counts = READ_REGISTER_ULONG( &(p_regs_xdma_engine_video->completedDescCount) );

							p_sys_cfg->n_completedDescCountVideo = counts;

							if( p_sys_cfg->n_video_running_state )
							{
								if((pDevice->idVendor == 0x12AB) && ((pDevice->idProduct & 0xFFFF) == 0x0750))
								{
									WRITE_REGISTER_ULONG( &(p_regs_interrupt->channelIntEnableW1S), 2 );
								}
								else
								{
									WRITE_REGISTER_ULONG( &(p_regs_interrupt->channelIntEnableW1S), 4 );
								}
							}

							if( pDevice->debug[ 0 ] ) LINUXV4L2_DEBUG( KERN_INFO, "[%02d] V counts(%08X)", pDevice->m_nKsDeviceNumber, counts );
						}
					}
				}
			}
			else
			{
				spin_unlock_irqrestore( &pDevice->m_sDPC_Video_SpinLock, flags );
			}
		}
		#endif
	}
}

void common_pci_driver_on_tasklet_audio( unsigned long p_context )
{
	CDevice * pDevice = (CDevice *)(p_context);
	ULONG i = 0;

	{


		CAudio * pAudio[ MAX_SUB_DEVICE_NUM_X_2 ] = { NULL };

		BOOLEAN  is_complete[ MAX_SUB_DEVICE_NUM_X_2 ] = { FALSE };

		BOOLEAN  is_drop[ MAX_SUB_DEVICE_NUM_X_2 ] = { FALSE };

		memset( pAudio, 0x00, MAX_SUB_DEVICE_NUM_X_2 * sizeof(CAudio*) );

		memset( is_complete, 0x00, MAX_SUB_DEVICE_NUM_X_2 * sizeof(BOOLEAN) );

		memset( is_drop, 0x00, MAX_SUB_DEVICE_NUM_X_2 * sizeof(BOOLEAN) );

		for( i = 0 ; i < MAX_SUB_DEVICE_NUM ; i++ ) {

			if( pDevice->m_eAudioStreamState[ i ] == 0x00000000 ) { continue ; }

			if( pDevice->m_pAudio[ i ] == NULL ) { continue ; }

			pAudio[ i ] = pDevice->m_pAudio[ i ];
		}

		#if defined(AME_SC0710) || defined(AME_SC0720)
		if( (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750) )
		{
			unsigned long flags = 0x00000000;

			spin_lock_irqsave( &pDevice->m_sDPC_Audio_SpinLock, flags );

			{
				ULONG w_cnt = 0;

				while(1) {

					ULONG qp = work_pending(&pDevice->w_audio);

					if(qp)
					{
						udelay(500);

						w_cnt++;



						if(w_cnt > 12)
						{
							LINUXV4L2_PRINT( KERN_ERR, "[%02d] queue_work() wq_audio pending [FAIL]\n", pDevice->m_nKsDeviceNumber );

							break;
						}

						continue;
					}
					else
					{
						ULONG qw = queue_work(pDevice->wq_audio, &pDevice->w_audio );

						if ( qw == 0 ) {

							LINUXV4L2_PRINT( KERN_ERR, "[%02d] queue_work() wq_audio [FAIL]\n", pDevice->m_nKsDeviceNumber );
						}

						break;
					}
				}
			}

			spin_unlock_irqrestore( &pDevice->m_sDPC_Audio_SpinLock, flags );

			{
				SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);

				XDMA_ENGINE_REGS * p_regs_xdma_engine_audio = (XDMA_ENGINE_REGS *)(p_sys_cfg->p_config_bar_address + SC0710_REGBASE_BLOCK_OFFSET + SC0710_REGBASE_ENGINE_OFFSET);

				XDMA_IRQ_REGS * p_regs_interrupt = (XDMA_IRQ_REGS *)(p_sys_cfg->p_config_bar_address + SC0710_REGBASE_IRQ);

				{
					ULONG counts = READ_REGISTER_ULONG( &(p_regs_xdma_engine_audio->completedDescCount) );

					if( counts != (p_sys_cfg->n_completedDescCountAudio + 1) )
					{
						if( pDevice->m_nAnalogVideoDecoderStatusProperty )
						{
							if( (pDevice->m_eAudioStreamState[ 0 ] == 1) && (pDevice->m_nAudioCaptureStreamPowerReference != 0) )
							{
								LINUXV4L2_DEBUG( KERN_INFO, "[%02d] A counts(%d) previous(%d)", pDevice->m_nKsDeviceNumber, counts, p_sys_cfg->n_completedDescCountAudio );
							}
						}
					}

					p_sys_cfg->n_completedDescCountAudio = counts;

					if( p_sys_cfg->n_audio_running_state )
					{
						WRITE_REGISTER_ULONG( &(p_regs_interrupt->channelIntEnableW1S), 0x8 );
					}

					if( pDevice->debug[ 0 ] ) LINUXV4L2_DEBUG( KERN_INFO, "[%02d] A counts(%08X)", pDevice->m_nKsDeviceNumber, counts );

				}
			}
		}
		#endif
	}
}

#endif

NTSTATUS OnSetCustomFirmwareUpdate( CDevice * pDevice, ULONG PropertyValue )
{


	#if defined(AME_SC0710) || defined(AME_SC0720)

    ULONG enable = PropertyValue;

	SC0710_CLOSE_THREAD();

	if( enable == 1 ) {

		if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x23)) ||

			((pDevice->bcdDevice == 0x0204) && (pDevice->iManufacturer == 0x29)) ||

			((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x29)) ||

			((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2B)) ||

			((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2D)) ||

			((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||

			((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D)) ||

			((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||

			((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D)) ||

			((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x57)) ) {

			SC0710_DownloadP2140Firmware( pDevice, "SC0720.BASE.FW.HEX" );
		}
		else {

			if ((pDevice->bcdDevice == 0x0404) && (pDevice->iManufacturer == 0x1D)) {

				SC0710_DownloadP2140Firmware(pDevice, "SC0710.BASE.HD.FW.HEX");
			}
			else if((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x17)) {

				SC0710_DownloadP2140Firmware(pDevice, "SC0710.BASE.4K.FW.HEX");
			}
			else
			{
				SC0710_DownloadP2140Firmware( pDevice, "SC0710.BASE.FW.HEX" );
			}
		}

		{

				if( (pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ) {

					;
				}
				else if( ((pDevice->iManufacturer == 0x1C)) ||
						 ((pDevice->iManufacturer == 0x1D)) ) {

					;
				}
				else if( ((pDevice->iManufacturer == 0x1C)) ||
						 ((pDevice->iManufacturer == 0x1C)) ||
						 ((pDevice->iManufacturer == 0x1D)) ) {

					;
				}
				else if( ((pDevice->iManufacturer == 0x1A)) ||
						 ((pDevice->iManufacturer == 0x1B)) ) {

					if( ((pDevice->bcdDevice == 0x0B04) && (pDevice->iManufacturer == 0x1B)) ) {

						SC0710_DownloadCyclone10Firmware_SPI( pDevice );
					}
					else {

						SC0710_DownloadC5Firmware_SPI( pDevice, "SC0710.CS.FW.HEX" );
					}
				}
				else if( ((pDevice->iManufacturer == 0x1E)) ||
						 ((pDevice->iManufacturer == 0x1F)) ) {

					;
				}
				else if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x16)) ||
						 ((pDevice->bcdDevice == 0x0401) && (pDevice->iManufacturer == 0x16)) ||
						 ((pDevice->bcdDevice == 0xB401) && (pDevice->iManufacturer == 0x16)) ||
						 ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x18)) ||
						 ((pDevice->bcdDevice == 0x0201) && (pDevice->iManufacturer == 0x18)) ||
						 ((pDevice->bcdDevice == 0xB201) && (pDevice->iManufacturer == 0x18)) ||
						 ((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x18)) ||
						 ((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x18)) ||
						 ((pDevice->bcdDevice == 0x0008) && (pDevice->iManufacturer == 0x18)) ) {

					SC0710_DownloadC5Firmware_SPI( pDevice, "SC0710.CS.FW.HEX" );
				}
				else if( ((pDevice->bcdDevice == 0x0204) && (pDevice->iManufacturer == 0x29)) ||
						 ((pDevice->bcdDevice == 0x0304) && (pDevice->iManufacturer == 0x29)) ||
						 ((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2D)) ||
						 ((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2B)) ||
						 ((pDevice->bcdDevice == 0x0A08) && (pDevice->iManufacturer == 0x2D)) ||
						 ((pDevice->bcdDevice == 0xC008) && (pDevice->iManufacturer == 0x2D)) ||
						 ((pDevice->bcdDevice == 0xC010) && (pDevice->iManufacturer == 0x2D)) ) {

					SC0720_DownloadC7Firmware_SPI( pDevice );
				}
				else if( ((pDevice->bcdDevice == 0x0001) && (pDevice->iManufacturer == 0x23)) ||
						 ((pDevice->bcdDevice == 0x0004) && (pDevice->iManufacturer == 0x2B)) ) {

					SC0720_DownloadCyclone10Firmware_SPI( pDevice );
				}
				else if( ((pDevice->bcdDevice == 0x0002) && (pDevice->iManufacturer == 0x12)) ) {


				}
				else {

					SC0710_DownloadC5Firmware( pDevice, "SC0710.CS.FW.HEX" );
				}
		}
	}
	SC0710_START_THREAD();

	#endif

	return STATUS_SUCCESS;
}

NTSTATUS OnGetCustomDeviceSerialNumberProperty( CDevice* pDevice, ULONG* pPropertyValue )
{

	#if defined(AME_SC0710) || defined(AME_SC0720)
	{

		if( (pDevice->iProduct == 0x10 && pDevice->iManufacturer == 0xFC && pDevice->bcdDevice == 0xD07C) ||
			(pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x0011) ||
			(pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x000E) ||
			(pDevice->iProduct == 0x1C && pDevice->iManufacturer == 0xFA && pDevice->bcdDevice == 0x000F) ) {

		   *pPropertyValue = ((pDevice->bcdDevice     << 16) & 0xFFFF0000) |

							 ((pDevice->iProduct      <<  8) & 0x0000FF00) |

							 ((pDevice->iManufacturer <<  0) & 0x000000FF);
		}
		else {

		   *pPropertyValue = ((pDevice->idVendor      << 16) & 0xFFFF0000) |

							 ((pDevice->iProduct      <<  8) & 0x0000FF00) |

							 ((pDevice->iManufacturer <<  0) & 0x000000FF);
		}
	}
	#endif

    LINUXV4L2_DEBUG( KERN_INFO, "OnGetCustomDeviceSerialNumberProperty( %08X )\n", *pPropertyValue);

	return STATUS_SUCCESS;
}

NTSTATUS OnSetCustomDebugMacAddressAccessEnableProperty( CDevice* pDevice )
{
	LINUXV4L2_DEBUG( KERN_INFO, "OnSetCustomDebugMacAddressAccessEnableProperty()\n" );

	#if defined(AME_SC0710) || defined(AME_SC0720)
	if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
	{
		down(&m_sComponentAccessSemaphore);

		SC0710_CLOSE_THREAD();

		UpdateMACaddress(pDevice, g_b_mac_address_data);

		SC0710_START_THREAD();

		up(&m_sComponentAccessSemaphore);

		LINUXV4L2_PRINT(KERN_INFO, "[UPDATE.MAC.ADDRESS]");
	}
	#endif

	return STATUS_SUCCESS;
}

NTSTATUS OnSetCustomDebugMacAddressI2cSubProperty( ULONG PropertyValue )
{
	LINUXV4L2_DEBUG( KERN_INFO, "OnSetCustomDebugMacAddressI2cSubProperty()\n" );

	#if defined(AME_SC0710) || defined(AME_SC0720)

	g_n_mac_address_sub = PropertyValue;

	#endif

	return STATUS_SUCCESS;
}

NTSTATUS OnGetCustomDebugMacAddressI2cDataProperty(CDevice* pDevice, ULONG address_sub, ULONG* pPropertyValue)
{
	LINUXV4L2_DEBUG(KERN_INFO, "OnGetCustomDebugMacAddressI2cDataProperty()\n");

#if defined(AME_SC0710) || defined(AME_SC0720)
	if ((pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0710) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0720) || (pDevice->m_pKsDevice->vendor == 0x12AB && pDevice->m_pKsDevice->device == 0x0750))
	{
		down(&m_sComponentAccessSemaphore);

		SC0710_CLOSE_THREAD();
		{
			BYTE p_mac_address[8];

			if (address_sub == 250) {

				ReadMACaddress(pDevice, p_mac_address);

				*pPropertyValue = p_mac_address[0] << 24 | p_mac_address[1] << 16 | p_mac_address[2] << 8 | p_mac_address[3];
			}
			if (address_sub == 254) {

				ReadMACaddress(pDevice, p_mac_address);

				*pPropertyValue = p_mac_address[4] << 8 | p_mac_address[5];
			}
		}
		SC0710_START_THREAD();

		up(&m_sComponentAccessSemaphore);
	}
	#endif

	return STATUS_SUCCESS;
}

NTSTATUS OnSetCustomDebugMacAddressI2cDataProperty( BYTE PropertyValue )
{
	LINUXV4L2_DEBUG( KERN_INFO, "OnSetCustomDebugMacAddressI2cDataProperty()\n" );

	#if defined(AME_SC0710) || defined(AME_SC0720)

	g_b_mac_address_data[ g_n_mac_address_sub % 256 ] = (BYTE)(PropertyValue);

	#endif

	return STATUS_SUCCESS;
}


int vivid_subscribe_event(struct v4l2_fh* fh,
	const struct v4l2_event_subscription* sub)
{
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)
	switch (sub->type) {
	case V4L2_EVENT_SOURCE_CHANGE:
		if (fh->vdev->vfl_dir == VFL_DIR_RX)
		{
			LINUXV4L2_DEBUG(KERN_INFO, "fh->vdev->vfl_dir == VFL_DIR_RX(0x%x)\n", VFL_DIR_RX);

			return v4l2_src_change_event_subscribe(fh, sub);
		}
		else
		{
			LINUXV4L2_DEBUG(KERN_INFO, "fh->vdev->vfl_dir(0x%x) != VFL_DIR_RX(0x%x)\n", fh->vdev->vfl_dir, VFL_DIR_RX);
		}
		break;
	default:
		return v4l2_ctrl_subscribe_event(fh, sub);
	}
	#endif
	return -EINVAL;
}

MODULE_DEVICE_TABLE( pci, LINUXV4L2_PCI_DEVICE_ID_TABLE );
MODULE_AUTHOR( "Linux V4L2" );

MODULE_DESCRIPTION( "Linux V4L2 Device Driver" );

MODULE_LICENSE( "GPL" );

MODULE_VERSION( LXV4L2D_VER );

module_init( common_module_on_init );

module_exit( common_module_on_exit );

