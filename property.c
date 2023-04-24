#include "LINUXV4L2.h"
#include "property.h"




#include "sc0710.h"


#ifdef FH8735_DEBUG_TIMESTAMP
static LONGLONG xxxxx[ 4 ] = { 0, 0, 0, 0 };

static LONGLONG ooooo[ 4 ] = { 0, 0, 0, 0 };
#endif

BOOLEAN update_video_time_stamp( CDevice * pDevice, CVideo * pVideo, CVideoBuffer * pVideoBuffer )
{

#ifndef CONFIG_X86_32

	#if defined(AME_SC0710) || defined(AME_SC0720)

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG *)(pVideo->m_pDevice->m_pCustomSystemConfigProperty);

	#endif

	LONGLONG o_ts = pVideo->m_nLastReferenceClockTime;

	LONGLONG e_ts_offset = 0;

	LONGLONG nFrameNumber = pVideo->m_nFrameNumber;

	LONGLONG base_fps = 0;



	BOOL     is_1st = FALSE;

	#if defined(AME_SC0710) || defined(AME_SC0720) || defined(FH8735_DEBUG_TIMESTAMP) || defined(OUTPUT_IDEAL_VIDEO_TIME_STAMP)
	ULONG    m = p_sys_cfg->n_input_video_resolution_fps_m;
	#endif

	LONGLONG base = 0;

	LONGLONG d_ts = 0;

	LONGLONG e_ts = 0;

	ULONG fps = 0;

	ULONG interleaved = p_sys_cfg->n_output_video_resolution_interleaved;

	#ifdef OUTPUT_REAL_VIDEO_TIME_STAMP
		return TRUE;
	#endif

	fps = p_sys_cfg->n_output_video_resolution_fps;

	#if defined(AME_SC0710) || defined(AME_SC0720)
	if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0 )
	{
		m = p_sys_cfg->n_input_video_resolution_fps_ms[pVideo->m_nSubDeviceNumber % 16];

		fps = p_sys_cfg->n_output_video_resolution_fpss[pVideo->m_nSubDeviceNumber % 16];
	}
	#endif



	if( fps == 0 ) {

		if( pDevice->m_nCustomVideoStandardProperty & V4L2_STD_525_60 ) {

			fps = 60;
		}
		else {

			fps = 50;
		}
	}



	if( pDevice->m_nCustomCompanyJojoroProperty	> 0 )
	{
		if(	(p_sys_cfg->n_input_video_resolution_cx == 720) &&

			(interleaved == TRUE) ) {

			e_ts_offset = 1 * 10000000 / ( 2 * fps );
		}
		else if( (p_sys_cfg->n_input_video_resolution_cx == 1920) &&

				 (interleaved == TRUE) ) {

			e_ts_offset = 1 * 10000000 / ( 2 * 2 * fps );
		}
		else {

			e_ts_offset = 0;
		}
	}


	if(pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty == 0)
	{
		if(pVideo->m_nSubDeviceType == 0x00000001)
		{

			ULONG output_fps = 0;

			if((pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF) == 0x77FFFFFF)
			{
				output_fps = p_sys_cfg->n_output_video_resolution_fps;
			}
			else
			{
				output_fps = pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF;
			}

			if(output_fps)
			{
				nFrameNumber *= (fps / output_fps);
			}
		}
		else
		{

			ULONG output_fps = 0;

			if((pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF) == 0x77FFFFFF)
			{
				LINUXV4L2_PRINT(KERN_INFO, "[%02d] ch(%d) UNKNOWN EncoderVideoAvgFrameRate\n", pDevice->m_nKsDeviceNumber, pVideo->m_nSubDeviceNumber);
			}
			else
			{
				output_fps = pDevice->m_nCustomEncoderVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF;
			}

			if(output_fps)
			{
				nFrameNumber *= (fps / output_fps);
			}
		}
	}
	#if defined(AME_SC0710) || defined(AME_SC0720)
	else
	{

		if(pVideo->m_nSubDeviceType == 0x00000001)
		{

			ULONG output_fps = 0;

			if((pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF) == 0x77FFFFFF)
			{
				output_fps = p_sys_cfg->n_output_video_resolution_fpss[pVideo->m_nSubDeviceNumber % 16];
			}
			else
			{
				output_fps = pDevice->m_nCustomAnalogVideoAvgFrameRateProperty[pVideo->m_nSubDeviceNumber] & 0x77FFFFFF;
			}

			if(output_fps)
			{
				nFrameNumber *= (fps / output_fps);
			}
		}
	}
	#endif


	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	e_ts = pVideoBuffer->vb.vb2_buf.timestamp / 100 + (e_ts_offset);
	#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
	e_ts = pVideoBuffer->vb.timestamp.tv_sec * 10000000 + pVideoBuffer->vb.timestamp.tv_usec * 10 + (e_ts_offset);
	#else
	e_ts = pVideoBuffer->vb.v4l2_buf.timestamp.tv_sec * 10000000 + pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec * 10 + (e_ts_offset);
	#endif


	d_ts  = e_ts - o_ts;

	#if defined(AME_SC0710) || defined(AME_SC0720)
	if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0 )
	{
		m = p_sys_cfg->n_input_video_resolution_fps_ms[pVideo->m_nSubDeviceNumber % 16];
	}
	#endif


	base_fps = p_sys_cfg->n_output_video_resolution_fps_base_fps;

	#if defined(AME_SC0710) || defined(AME_SC0720)

	if( pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0 )
	{
		base_fps = p_sys_cfg->n_output_video_resolution_fps_base_fpss[pVideo->m_nSubDeviceNumber % 16];
	}
	#endif


	if( base_fps == 0 ) {

		base_fps = 60000;
	}






	{
		LONGLONG a_temp = 10000000000;
		LONGLONG b_temp = 1000000000000000000;
		LONGLONG remains = 0;

		do_div(a_temp, base_fps);

		base = a_temp;

		do_div( b_temp, base_fps);

		remains = b_temp - (base * 100000000);

		d_ts = base;

		if( o_ts == 0 ) {

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] o_ts == 0\n", (int)(pDevice->m_nKsDeviceNumber));

			o_ts = e_ts;

			d_ts = 0;

			remains = 0;

			is_1st = TRUE;

			pVideo->m_nLastPresentationTime = e_ts;

			pVideo->m_nLastPresentationTimeMultipler = 0;
		}
		else {

			pVideo->m_nLastPresentationTimeMultipler += (LONG)(remains);

			if( pVideo->m_nLastPresentationTimeMultipler >= 100000000 ) {

				pVideo->m_nLastPresentationTimeMultipler -= 100000000;

				d_ts += 1;
			}
		}
	}


	pVideo->m_nNowPresentationTime = pVideo->m_nLastPresentationTime + d_ts;




	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	pVideoBuffer->vb.vb2_buf.timestamp = (pVideo->m_nNowPresentationTime) * 100;
	#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
	{
		LONGLONG m_nNowPresentationTime_temp = pVideo->m_nNowPresentationTime;
		pVideoBuffer->vb.timestamp.tv_usec = do_div(m_nNowPresentationTime_temp, 10000000);
		pVideoBuffer->vb.timestamp.tv_sec = m_nNowPresentationTime_temp;
	}
	pVideoBuffer->vb.timestamp.tv_usec /= 10;
	#else
	{
		LONGLONG m_nNowPresentationTime_temp = pVideo->m_nNowPresentationTime;
		pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec = do_div(m_nNowPresentationTime_temp, 10000000);
		pVideoBuffer->vb.v4l2_buf.timestamp.tv_sec = m_nNowPresentationTime_temp;
	}
	pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec /= 10;
	#endif







	{











		{

			LONGLONG delta = 0;

			LONGLONG _fps = p_sys_cfg->n_output_video_resolution_fps;

#if defined(AME_SC0710) || defined(AME_SC0720)
			if (pDevice->m_nCustomAnalogVideoMultiChannelSupportProperty > 0)
			{
				_fps = p_sys_cfg->n_output_video_resolution_fpss[pVideo->m_nSubDeviceNumber % 16];
			}
#endif



			if( pVideo->m_nFirstPresentationTime == 0 ) {

				pVideo->m_nFirstPresentationTime = e_ts;

				pVideo->m_nFirstFrameNumber = nFrameNumber;
			}

			if( p_sys_cfg->n_output_video_resolution_fps_1_2 )
			{
				delta = ((nFrameNumber - pVideo->m_nFirstFrameNumber) * 20000000);
			}
			else
			{
				delta = ((nFrameNumber - pVideo->m_nFirstFrameNumber) * 10000000);
			}


			if( m == 0 ) {






				if (_fps) delta /= _fps;


			}
			else {



				delta *= 1001;



				if (_fps) delta /= (_fps * 1000);


			}



			pVideo->m_nNowPresentationTime = pVideo->m_nFirstPresentationTime + delta;

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			pVideoBuffer->vb.vb2_buf.timestamp = (pVideo->m_nNowPresentationTime) * 100;
			#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
			{
				LONGLONG m_nNowPresentationTime_temp = pVideo->m_nNowPresentationTime;
				pVideoBuffer->vb.timestamp.tv_usec = do_div(m_nNowPresentationTime_temp, 10000000);
				pVideoBuffer->vb.timestamp.tv_sec = m_nNowPresentationTime_temp;
			}
			pVideoBuffer->vb.timestamp.tv_usec /= 10;
			#else
			{
				LONGLONG m_nNowPresentationTime_temp = pVideo->m_nNowPresentationTime;
				pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec = do_div(m_nNowPresentationTime_temp, 10000000);
				pVideoBuffer->vb.v4l2_buf.timestamp.tv_sec = m_nNowPresentationTime_temp;
			}
			pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec /= 10;
			#endif

			pVideo->m_nLastPresentationTimeFpsErrors = 0;
		}
	}

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	if( ( pVideoBuffer->vb.vb2_buf.timestamp / 100 ) <= pVideo->m_nLastPresentationTime &&
	#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
	if( ( (pVideoBuffer->vb.timestamp.tv_sec * 10000000) + (pVideoBuffer->vb.timestamp.tv_usec * 10) ) <= pVideo->m_nLastPresentationTime &&
	#else
	if( ( (pVideoBuffer->vb.v4l2_buf.timestamp.tv_sec * 10000000) + (pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec * 10) ) <= pVideo->m_nLastPresentationTime &&
	#endif

	is_1st == FALSE ) {

#ifdef FH8735_DEBUG_TIMESTAMP


		if( pVideo->m_nSubDeviceType == 0x00000001 )
		{
			LINUXV4L2_PRINT( KERN_INFO, "[%02d] TS.DEBUG.PREVIEW( RESET#2, last.ts = %lld, calc.ts = %lld )", pDevice->m_nKsDeviceNumber, pVideo->m_nLastPresentationTime, e_ts);
		}
		else
		{
			LINUXV4L2_PRINT( KERN_INFO, "[%02d] TS.DEBUG.H264( RESET#2, last.ts = %lld, calc.ts = %lld )", pDevice->m_nKsDeviceNumber, pVideo->m_nLastPresentationTime, e_ts);
		}
#endif
		if( e_ts > pVideo->m_nLastPresentationTime ) {



			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			pVideoBuffer->vb.vb2_buf.timestamp = e_ts * 100;
			#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
			{
				LONGLONG e_ts_temp = e_ts;
				pVideoBuffer->vb.timestamp.tv_usec = do_div(e_ts_temp, 10000000);
				pVideoBuffer->vb.timestamp.tv_sec = (e_ts_temp);
				pVideoBuffer->vb.timestamp.tv_usec /= 10;
			}
			#else
			{
				LONGLONG e_ts_temp = e_ts;
				pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec = do_div(e_ts_temp, 10000000);
				pVideoBuffer->vb.v4l2_buf.timestamp.tv_sec = (e_ts_temp);
				pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec /= 10;
			}
			#endif

			pVideo->m_nFirstPresentationTime = e_ts;
		}
		else {



			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			pVideoBuffer->vb.vb2_buf.timestamp = (pVideo->m_nLastPresentationTime + 10000) * 100;
			#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
			{
				LONGLONG m_nLastPresentationTime_temp = pVideo->m_nLastPresentationTime + 10000;
				pVideoBuffer->vb.timestamp.tv_usec = do_div(m_nLastPresentationTime_temp, 10000000);
				pVideoBuffer->vb.timestamp.tv_sec = (m_nLastPresentationTime_temp);

				pVideoBuffer->vb.timestamp.tv_usec /= 10;
			}
			#else
			{
				LONGLONG m_nLastPresentationTime_temp = pVideo->m_nLastPresentationTime + 10000;
				pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec = do_div(m_nLastPresentationTime_temp, 10000000);
				pVideoBuffer->vb.v4l2_buf.timestamp.tv_sec = (m_nLastPresentationTime_temp);

				pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec /= 10;
			}
			#endif

			pVideo->m_nFirstPresentationTime = pVideo->m_nLastPresentationTime + 10000;
		}


		pVideo->m_nFirstFrameNumber = nFrameNumber;

		pVideo->m_nLastPresentationTimeMultipler = 0;

		pVideo->m_nLastPresentationTimeFpsErrors = 0;
	}

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	pVideo->m_nLastPresentationTime = pVideoBuffer->vb.vb2_buf.timestamp / 100;
	#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
	pVideo->m_nLastPresentationTime = pVideoBuffer->vb.timestamp.tv_sec * 10000000 + pVideoBuffer->vb.timestamp.tv_usec * 10;
	#else
	pVideo->m_nLastPresentationTime = pVideoBuffer->vb.v4l2_buf.timestamp.tv_sec * 10000000 + pVideoBuffer->vb.v4l2_buf.timestamp.tv_usec * 10;
	#endif

	pVideo->m_nLastReferenceClockTime = e_ts;



#ifdef FH8735_DEBUG_TIMESTAMP

	if( is_1st ) {

		ooooo[ 0 ] = e_ts;

		xxxxx[ 0 ] = 0;
	}
	if( (xxxxx[ 0 ]++ % 500) == 0 ) {

		LONGLONG ttttt = e_ts - ooooo[ 0 ];

		ttttt /= xxxxx[ 0 ];

		LINUXV4L2_DEBUG( KERN_INFO, "video.%d %lld | (%lld) (%lld) (%lld) (%lld) | ----> (%lld) | (m = %d)\n", 0,

			(pVideo->m_nNowPresentationTime),

			(xxxxx[ 0 ] * 10000000000) / base_fps,

			(e_ts),

			(pVideo->m_nNowPresentationTime - ((xxxxx[ 0 ] * 10000000000) / base_fps)),

			(pVideo->m_nNowPresentationTime - e_ts),



			ttttt,

			m  );
	}
#endif

#endif

	return TRUE;
}

NTSTATUS StartAudioLossThread( CDevice * pDevice )
{
#if 1
	if( pDevice->m_pAudioLossThread == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] StartAudioLossThread( ) \n", (int)(pDevice->m_nKsDeviceNumber) );

		pDevice->m_pAudioLossThread = kthread_run( OnAudioLossThread, pDevice, "OnAudioLossThread" );

		if( IS_ERR( pDevice->m_pAudioLossThread ) ) {

			int ret = PTR_ERR( pDevice->m_pAudioLossThread );

			pDevice->m_pAudioLossThread = NULL;

			return ret;
		}
	}
#endif
	return STATUS_SUCCESS;
}

NTSTATUS StopAudioLossThread( CDevice * pDevice )
{
#if 1
	int thread_stop_counter = 0;

	if( pDevice->m_pAudioLossThread ) {

		if( pDevice->m_ReadyToStopAudioLossThread == 0 )
		{
			pDevice->m_ReadyToStopAudioLossThread = 1;

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] StopAudioLossThread( ) \n", (int)(pDevice->m_nKsDeviceNumber) );

			kthread_stop( pDevice->m_pAudioLossThread );

			while( pDevice->m_pAudioLosslThread_already_stop == FALSE )
			{
				schedule_timeout_interruptible( msecs_to_jiffies(200) );

				LINUXV4L2_DEBUG( KERN_INFO, "[%02d] StopAudioLossThread( ) ---\n", (int)(pDevice->m_nKsDeviceNumber) );

				thread_stop_counter++;

				if(thread_stop_counter > 5)
				{
					break;
				}
			}

			pDevice->m_pAudioLossThread = NULL;

			pDevice->m_ReadyToStopAudioLossThread = 0;

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] StopVideoLossThread( ) end\n", (int)(pDevice->m_nKsDeviceNumber) );
		}
	}
#endif
	return STATUS_SUCCESS;
}

int OnAudioLossThread( void * p_context )
{
	CDevice * pDevice = (CDevice *)p_context;

	CVideo* pVideo = pDevice->m_pVideo[0];





#if defined(AME_SC0710) || defined(AME_SC0720)
	SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
#endif

	ULONGLONG n_delta_sum = 0;

	ULONGLONG n_delta_sum_remains = 0;

	ULONG counts = 0;

	ULONG timeouts = 0;

	ULONGLONG n_start_time = 0;


	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	u64 ts = 0;
	#else
	struct timeval ts;
	#endif

	LINUXV4L2_DEBUG( KERN_INFO,"OnAudioLossThread()\n");

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	ts = ktime_get_ns();
	#else
	do_gettimeofday( &ts );
	#endif

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
		#ifndef CONFIG_X86_32
		n_start_time = (ULONGLONG)(ts / 100);
		#endif
	#else
		n_start_time = (ULONGLONG)(ts.tv_sec * 10000000) + (ULONGLONG)(ts.tv_usec * 10);
	#endif

	pDevice->m_pAudioLosslThread_already_stop = FALSE;

	while( kthread_should_stop() == FALSE ) {

		BOOL n_signal_loss = FALSE;

		BOOL n_stream_loss = FALSE;

		#ifndef CONFIG_X86_32

		if( pDevice->m_nAudioCaptureStreamPowerReference > 0 &&



			pDevice->m_nAnalogVideoDecoderStatusProperty == 0 &&

			pDevice->m_nAnalogCrossbarAudioInputProperty == 0 ) {

			n_signal_loss = TRUE;
		}
		if( pDevice->m_nAudioCaptureStreamPowerReference > 0 &&



			pDevice->m_nAnalogVideoDecoderStatusProperty == 1 &&

			pDevice->m_nAnalogCrossbarAudioInputProperty == 0 ) {

			if( pDevice->m_nDmaBaseCommonBufferNumber[ 1 ] > 0 ) {

				pDevice->m_nDmaBaseCommonBufferNumber[ 1 ] = 0;

				timeouts = 0;
			}
			else {

				timeouts++;
			}
			if( timeouts >= 10 ) {

				n_stream_loss = TRUE;
			}
		}

		if( n_signal_loss ) {

			if( pDevice->m_nDmaBaseCommonBufferNumber[ 1 ] > 0 ) {

				pDevice->m_nDmaBaseCommonBufferNumber[ 1 ] = 0;

				n_signal_loss = FALSE;

				counts = 0;
			}
			else {

				counts++;
			}
			if( counts < 100 ) {

				n_signal_loss = FALSE;
			}
		}

		if( n_signal_loss ||

			n_stream_loss ) {

			ULONGLONG n_current_time = 0;

			BOOL is_on = FALSE;

			ULONG duration = 213333;

			ULONG frq = 48000;

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			ts = ktime_get_ns();
			#else
			do_gettimeofday( &ts );
			#endif

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			n_current_time = (ULONGLONG)(ts / 100);
			#else
			n_current_time = (ULONGLONG)(ts.tv_sec * 10000000) + (ULONGLONG)(ts.tv_usec * 10);
			#endif

			if( n_start_time == 0 ) {

				n_start_time = n_current_time;

				n_delta_sum = 0;

				n_delta_sum_remains = 0;
			}
			if( p_sys_cfg->n_output_audio_sampling_frequency == 48000 ) { duration = 213333; frq = 48000; }

			if( p_sys_cfg->n_output_audio_sampling_frequency == 44100 ) { duration = 232199; frq = 44100; }

			if( p_sys_cfg->n_output_audio_sampling_frequency == 32000 ) { duration = 320000; frq = 32000; }

			{

				LONGLONG a_temp = 1024000000000000000;

				do_div(a_temp, frq);
				{
					LONGLONG remains = a_temp - (duration * 100000000);

					if( (n_current_time - n_start_time) - (n_delta_sum) >= duration ) {

						n_delta_sum += duration;

						n_delta_sum_remains += remains;

						if( n_delta_sum_remains >= 100000000 ) {

							n_delta_sum_remains -= 100000000;

							n_delta_sum++;
						}
						is_on = TRUE;
					}
				}
			}
			if( is_on == TRUE ) {

				if( pDevice->m_nAudioCaptureStreamPowerReference >= 0x00000001 )
				{




					#if defined(AME_SC0710) || defined(AME_SC0720)

					unsigned long flags = 0x00000000;

					pDevice->m_nTaskletExtraParameter_a = 0xEEEEEEEE;

					spin_lock_irqsave( &pVideo->m_sNo_Signal_SpinLock, flags );

					if(	pDevice->m_nDmaBaseCommonBufferNumber[1] == 0 )
					{
						pDevice->m_InNoAudioDPC = 1;
					}

					spin_unlock_irqrestore( &pVideo->m_sNo_Signal_SpinLock, flags );

					if( pDevice->m_InNoAudioDPC == 1 )
					{

						common_pci_driver_on_tasklet_audio( (unsigned long)pDevice );


						spin_lock_irqsave( &pVideo->m_sNo_Signal_SpinLock, flags );

						pDevice->m_InNoAudioDPC = 0;

						spin_unlock_irqrestore( &pVideo->m_sNo_Signal_SpinLock, flags );
					}

					#endif
				}
			}
		}
		#endif

		if( n_signal_loss ||

			n_stream_loss ) {

			DELAY_100NS(  10000 );
		}
		else {

			DELAY_100NS( 100000 );

			n_start_time = 0;

			n_delta_sum = 0;
		}

		timeouts++;
	}

	pDevice->m_pAudioLosslThread_already_stop = TRUE;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] OnAudioLossThread( exit ) - %p\n", (int)(pDevice->m_nKsDeviceNumber), pDevice->m_pAudioLossThread );

	return 0;
}

NTSTATUS StartVideoLossThread( CDevice * pDevice )
{
#if 1
	if( pDevice->m_pVideoLossThread == NULL )
	{
		LINUXV4L2_DEBUG( KERN_INFO, "[%02d] StartVideoLossThread( ) \n", (int)(pDevice->m_nKsDeviceNumber) );

		pDevice->m_pVideoLossThread = kthread_run( OnVideoLossThread, pDevice, "OnVideoLossThread" );

		if( IS_ERR( pDevice->m_pVideoLossThread ) ) {

			int ret = PTR_ERR( pDevice->m_pVideoLossThread );

			pDevice->m_pVideoLossThread = NULL;

			return ret;
		}
	}
#endif
	return STATUS_SUCCESS;
}

NTSTATUS StopVideoLossThread( CDevice * pDevice )
{
#if 1
	int thread_stop_counter = 0;

	if( pDevice->m_pVideoLossThread ) {

		if( pDevice->m_ReadyToStopVideoLossThread == 0 )
		{
			pDevice->m_ReadyToStopVideoLossThread = 1;

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] StopVideoLossThread( ) \n", (int)(pDevice->m_nKsDeviceNumber) );

			kthread_stop( pDevice->m_pVideoLossThread );

			while( pDevice->m_pVideoLosslThread_already_stop == FALSE )
			{
				schedule_timeout_interruptible( msecs_to_jiffies(200) );

				LINUXV4L2_DEBUG( KERN_INFO, "[%02d] StopVideoLossThread( ) ---\n", (int)(pDevice->m_nKsDeviceNumber) );

				thread_stop_counter++;

				if(thread_stop_counter > 5)
				{
					break;
				}
			}

			pDevice->m_pVideoLossThread = NULL;

			pDevice->m_ReadyToStopVideoLossThread = 0;

			LINUXV4L2_DEBUG( KERN_INFO, "[%02d] StopVideoLossThread( ) end\n", (int)(pDevice->m_nKsDeviceNumber) );
		}
	}
#endif
	return STATUS_SUCCESS;
}

int OnVideoLossThread( void * p_context )
{
	CDevice * pDevice = (CDevice *)p_context;

	CVideo* pVideo = pDevice->m_pVideo[0];





#if defined(AME_SC0710) || defined(AME_SC0720)
	SC0710_SYS_CFG * p_sys_cfg = (SC0710_SYS_CFG *)(pDevice->m_pCustomSystemConfigProperty);
#endif

	ULONGLONG n_start_time = 0;

	ULONGLONG n_delta_sum = 0;

	ULONGLONG n_delta_sum_remains = 0;

	ULONGLONG pictures_preview = 0;

	ULONG     timeouts_preview = 0;

	LINUXV4L2_DEBUG( KERN_INFO,"OnVideoLossThread()\n");

	pDevice->m_pVideoLosslThread_already_stop = FALSE;

	while( kthread_should_stop() == FALSE ) {

		BOOL n_signal_loss = FALSE;

		BOOL n_stream_loss_preview = FALSE;

		#ifndef CONFIG_X86_32

		if( pDevice->m_nAnalogCaptureStreamPowerReference > 0 &&

			pDevice->m_nAnalogVideoDecoderStatusProperty == 0 ) {

			if( pDevice->m_nAnalogCrossbarVideoInputProperty == 5 ||

				pDevice->m_nAnalogCrossbarVideoInputProperty == 6 ) {

				n_signal_loss = FALSE;
			}
			else {

				n_signal_loss = TRUE;
			}
		}

		if( pDevice->m_nAnalogCaptureStreamPowerReference > 0 &&

			pDevice->m_nAnalogVideoDecoderStatusProperty == 1 ) {

			if( pDevice->m_nDmaBaseCommonBufferNumber[ 0 ] > 0 ) {

				pDevice->m_nDmaBaseCommonBufferNumber[ 0 ] = 0;

				timeouts_preview = 0;
			}
			else {

				timeouts_preview++;
			}
			if( timeouts_preview >= 250 ) {

				n_stream_loss_preview = TRUE;
			}
		}
		else
		{
			timeouts_preview = 0;
		}

		if( n_signal_loss ||

			n_stream_loss_preview ) {

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			u64 ts = 0;
			#else
			struct timeval ts;
			#endif

			ULONGLONG n_current_time = 0;

			BOOL is_on = FALSE;

			ULONGLONG duration = 333333;

			ULONG fps = p_sys_cfg->n_output_video_resolution_fps * 1000;

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			ts = ktime_get_ns();
			#else
			do_gettimeofday( &ts );
			#endif

			#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
			n_current_time = (ULONGLONG)(ts / 100);
			#else
			n_current_time = (ULONGLONG)(ts.tv_sec * 10000000) + (ULONGLONG)(ts.tv_usec * 10);
			#endif

			if( n_start_time == 0 ) {

				n_start_time = n_current_time;

				n_delta_sum = 0;

				n_delta_sum_remains = 0;

				pictures_preview = 0;
			}

			if( p_sys_cfg->n_output_video_resolution_fps_m >= 1 ) {

				fps *= 1000;

				fps /= 1001;
			}

			if( fps == 0 ) {

				fps = 30000;
			}



			{
				LONGLONG a_temp = 10000000000;
				LONGLONG b_temp = 1000000000000000000;

				do_div(a_temp, fps);

				duration = a_temp;

				do_div( b_temp, fps);
				{
					LONGLONG remains = b_temp - (duration * 100000000);

					if( (n_current_time - n_start_time) - (n_delta_sum) >= duration ) {

						n_delta_sum += duration;

						n_delta_sum_remains += remains;

						if( n_delta_sum_remains >= 100000000 ) {

							n_delta_sum_remains -= 100000000;

							n_delta_sum++;
						}
						is_on = TRUE;
					}
				}
			}
			if( is_on == TRUE ) {

				BOOL is_yv12_on = FALSE;

				pictures_preview += fps;

				if( pictures_preview >= fps ) { pictures_preview -= fps; is_yv12_on = TRUE; }

				if( n_stream_loss_preview ) { pDevice->m_nDmaBaseCommonBufferCustomMiscProperty[ 3 ]++; }

				if( is_yv12_on && (n_signal_loss || n_stream_loss_preview )) {


					if( pDevice->m_eVideoStreamState[ 0 ] >=  0x00000001 ) {

						#if defined(AME_SC0710) || defined(AME_SC0720)

						unsigned long flags = 0x00000000;

						spin_lock_irqsave( &pVideo->m_sNo_Signal_SpinLock, flags );

						if(	pDevice->m_nDmaBaseCommonBufferNumber[ 0 ] == 0 )
						{
							pVideo->m_InNoVideoDPC = 1;
						}

						spin_unlock_irqrestore( &pVideo->m_sNo_Signal_SpinLock, flags );

						if(pVideo->m_InNoVideoDPC == 1 )
						{
							pDevice->m_nTaskletExtraParameterC = 0x00000080;

							SC0710_HwProcessAnalogRawVideoFrame(pDevice, pDevice->m_pVideo[0]);

							spin_lock_irqsave( &pVideo->m_sNo_Signal_SpinLock, flags );

							pVideo->m_InNoVideoDPC = 0;

							spin_unlock_irqrestore( &pVideo->m_sNo_Signal_SpinLock, flags );

						}
						#endif
					}
				}
			}

		}
		#endif

		if( n_signal_loss ||

			n_stream_loss_preview ) {

			DELAY_100NS(  10000 );
		}
		else {

			DELAY_100NS( 100000 );

			n_start_time = 0;

			n_delta_sum = 0;

			n_delta_sum_remains = 0;

			pictures_preview = 0;
		}

		timeouts_preview++;
	}

	pDevice->m_pVideoLosslThread_already_stop = TRUE;

	LINUXV4L2_DEBUG( KERN_INFO, "[%02d] OnVideoLossThread( exit ) - %p\n", (int)(pDevice->m_nKsDeviceNumber), pDevice->m_pVideoLossThread );

	return 0;
}




NTSTATUS StartMultiChVideoLossThread(CDevice* pDevice)
{
#if 1
	if (pDevice->m_pMultiChVideoLossThread == NULL)
	{
		LINUXV4L2_DEBUG(KERN_INFO, "[%02d] StartMultiChVideoLossThread() \n", pDevice->m_nKsDeviceNumber);

		pDevice->m_pMultiChVideoLossThread = kthread_run(OnMultiChVideoLossThread, pDevice, "OnMultiChVideoLossThread");

		if (IS_ERR(pDevice->m_pMultiChVideoLossThread)) {

			int ret = PTR_ERR(pDevice->m_pMultiChVideoLossThread);

			pDevice->m_pMultiChVideoLossThread = NULL;

			return ret;
		}
	}
#endif
	return STATUS_SUCCESS;
}

NTSTATUS StopMultiChVideoLossThread(CDevice* pDevice)
{
#if 1
	int thread_stop_counter = 0;

	if (pDevice->m_pMultiChVideoLossThread) {

		if (pDevice->m_ReadyToStopMultiChVideoLossThread == 0)
		{
			pDevice->m_ReadyToStopMultiChVideoLossThread = 1;

			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] StopMultiChVideoLossThread( ) \n", pDevice->m_nKsDeviceNumber);

			kthread_stop(pDevice->m_pMultiChVideoLossThread);

			while (pDevice->m_pMultiChVL_already_stop == FALSE)
			{
				schedule_timeout_interruptible(msecs_to_jiffies(200));

				LINUXV4L2_DEBUG(KERN_INFO, "[%02d] StopMultiChVideoLossThread( ) ---\n", pDevice->m_nKsDeviceNumber);

				thread_stop_counter++;

				if (thread_stop_counter > 5)
				{
					break;
				}
			}

			pDevice->m_pMultiChVideoLossThread = NULL;

			pDevice->m_ReadyToStopMultiChVideoLossThread = 0;

			LINUXV4L2_DEBUG(KERN_INFO, "[%02d] StopMultiChVideoLossThread( ) end\n", pDevice->m_nKsDeviceNumber);
		}
	}
#endif
	return STATUS_SUCCESS;
}
#define MAX_MULTI_CH 16

int OnMultiChVideoLossThread(void* p_context)
{
	CDevice* pDevice = (CDevice*)p_context;

	CVideo* pVideo = NULL;

#if defined(AME_SC0710) || defined(AME_SC0720)

	SC0710_SYS_CFG* p_sys_cfg = (SC0710_SYS_CFG*)(pDevice->m_pCustomSystemConfigProperty);

	ULONGLONG n_start_time[MAX_MULTI_CH] = { 0 };

	ULONGLONG n_delta_sum[MAX_MULTI_CH] = { 0 };

	ULONGLONG n_delta_sum_remains[MAX_MULTI_CH] = { 0 };

	ULONGLONG pictures_preview[MAX_MULTI_CH] = { 0 };

	ULONG ch = 0;

	LINUXV4L2_DEBUG(KERN_INFO, "[%02d] OnMultiChVideoLossThread()\n", pDevice->m_nKsDeviceNumber);

	pDevice->m_pMultiChVL_already_stop = FALSE;

	while(kthread_should_stop() == FALSE) {

		for(ch = 0; ch < (pDevice->bcdDevice & 0x1F); ch++)
		{
			BOOL n_signal_loss = FALSE;

			pVideo = pDevice->m_pVideo[ch];

			if(pVideo)
			{
				#ifndef CONFIG_X86_32

				if (pDevice->m_eVideoStreamState[pVideo->m_nSubDeviceNumber] > 0 &&

					pDevice->m_nAnalogVideoDecoderStatusPropertys[pVideo->m_nSubDeviceNumber] == 0) {

					if (pDevice->m_nAnalogCrossbarVideoInputProperty == 5 ||

						pDevice->m_nAnalogCrossbarVideoInputProperty == 6) {

						n_signal_loss = FALSE;
					}
					else {

						n_signal_loss = TRUE;
					}
				}

				if (n_signal_loss) {

					#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
					u64 ts = 0;
					#else
					struct timeval ts;
					#endif

					ULONGLONG n_current_time = 0;

					BOOL is_on = FALSE;

					ULONGLONG duration = 333333;

					ULONG fps = p_sys_cfg->n_output_video_resolution_fpss[pVideo->m_nSubDeviceNumber % 16] * 1000;

					#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
					ts = ktime_get_ns();

					n_current_time = (ULONGLONG)(ts / 100);
					#else
					do_gettimeofday(&ts);

					n_current_time = (ULONGLONG)(ts.tv_sec * 10000000) + (ULONGLONG)(ts.tv_usec * 10);
					#endif


					if (n_start_time[ch] == 0) {

						n_start_time[ch] = n_current_time;

						n_delta_sum[ch] = 0;

						n_delta_sum_remains[ch] = 0;

						pictures_preview[ch] = 0;
					}

					if (p_sys_cfg->n_output_video_resolution_fps_ms[pVideo->m_nSubDeviceNumber % 16] >= 1) {

						fps *= 1000;

						fps /= 1001;
					}

					if (fps == 0) {

						fps = 30000;
					}



					{
						LONGLONG a_temp = 10000000000;
						LONGLONG b_temp = 1000000000000000000;

						do_div(a_temp, fps);

						duration = a_temp;

						do_div(b_temp, fps);
						{
							LONGLONG remains = b_temp - (duration * 100000000);

							if ((n_current_time - n_start_time[ch]) - (n_delta_sum[ch]) >= duration) {

								n_delta_sum[ch] += duration;

								n_delta_sum_remains[ch] += remains;

								if (n_delta_sum_remains[ch] >= 100000000) {

									n_delta_sum_remains[ch] -= 100000000;

									n_delta_sum[ch]++;
								}
								is_on = TRUE;
							}
						}
					}
					if (is_on == TRUE) {

						BOOL is_yv12_on = FALSE;

						pictures_preview[ch] += fps;

						if (pictures_preview[ch] >= fps) { pictures_preview[ch] -= fps; is_yv12_on = TRUE; }

						if (is_yv12_on && n_signal_loss) {

							if (pDevice->m_eVideoStreamState[pVideo->m_nSubDeviceNumber] > 0) {

								unsigned long flags = 0x00000000;

								spin_lock_irqsave(&pVideo->m_sNo_Signal_SpinLock, flags);


								pVideo->m_InNoVideoDPC = 1;

								spin_unlock_irqrestore(&pVideo->m_sNo_Signal_SpinLock, flags);

								if (pVideo->m_InNoVideoDPC == 1)
								{
									MultiChLastLine(pVideo, ch, 0);

									spin_lock_irqsave(&pVideo->m_sNo_Signal_SpinLock, flags);

									pVideo->m_InNoVideoDPC = 0;

									spin_unlock_irqrestore(&pVideo->m_sNo_Signal_SpinLock, flags);

								}
							}
						}
					}

				}
				#endif

				if (n_signal_loss) {


				}
				else {



					n_start_time[ch] = 0;

					n_delta_sum[ch] = 0;

					n_delta_sum_remains[ch] = 0;

					pictures_preview[ch] = 0;
				}

				DELAY_100NS(10000);
			}

		}

		DELAY_100NS(100000);
	}

	pDevice->m_pMultiChVL_already_stop = TRUE;

	LINUXV4L2_DEBUG(KERN_INFO, "[%02d] OnVideoLossThread( exit ) - %p\n", (int)(pDevice->m_nKsDeviceNumber), pDevice->m_pVideoLossThread);

#endif

	return 0;
}


VOID CALCULATE_FPS(CDevice* pDevice, CVideo* pVideo, ULONG real_fps, ULONG output_fps)
{
	ULONG counts = 0;

	ULONGLONG i = 0;

	if(output_fps == 0)
	{
		LINUXV4L2_PRINT(KERN_ERR, "[%02d] output_fps 0\n", pDevice->m_nKsDeviceNumber);

		return;
	}

	if(real_fps > 240)
	{
		LINUXV4L2_PRINT(KERN_ERR, "[%02d] real_fps > 240\n", pDevice->m_nKsDeviceNumber);

		return;
	}

	for (i = 0; i < real_fps; i++) {

		counts += output_fps;

		if (counts >= real_fps) {

			counts -= real_fps;


			pDevice->m_pVideo[pVideo->m_nSubDeviceNumber]->m_valid[i] = 1;
		}
		else
		{
			pDevice->m_pVideo[pVideo->m_nSubDeviceNumber]->m_valid[i] = 0;
		}
	}
}


