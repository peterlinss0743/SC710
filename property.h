#ifndef PROPERTY_H
#define PROPERTY_H

extern UINT g_n_analog_decoder_brightness                    [ 64 ][ 4 ];

extern UINT g_n_analog_decoder_contrast                      [ 64 ][ 4 ];

extern UINT g_n_analog_decoder_saturation                    [ 64 ][ 4 ];

extern UINT g_n_analog_decoder_hue                           [ 64 ][ 4 ];

extern UINT g_n_analog_decoder_sharpness                     [ 64 ][ 4 ];

extern UINT g_n_analog_decoder_white_balance[64][4];

extern UINT g_n_analog_decoder_gain_auto                     [ 64 ];

extern UINT g_n_analog_decoder_gain                          [ 64 ];

extern UINT g_n_analog_decoder_lock_status                   [ 64 ];


extern UINT g_n_multiplexer_motion_status                    [ 64 ];

extern UINT g_n_multiplexer_motion_mask_cx                   [ 64 ];

extern UINT g_n_multiplexer_motion_mask_cy                   [ 64 ];

extern UINT g_n_multiplexer_motion_sensitivity_low_threshold [ 64 ];

extern UINT g_n_multiplexer_motion_sensitivity_high_threshold[ 64 ];


extern UINT g_n_encoder_osd_pos_x							  [ 64 ];

extern UINT g_n_encoder_osd_pos_y							  [ 64 ];

extern UINT g_n_encoder_osd_color							  [ 64 ];

extern CHAR  g_p_encoder_osd_text 							  [ 64 ][ 64 + 1 ];


extern UINT g_n_gpio_direction                               [ 64 ];

extern UINT g_n_gpio_data                                    [ 64 ];

BOOLEAN update_video_time_stamp( CDevice * pDevice, CVideo * pVideo, CVideoBuffer * pVideoBuffer );

NTSTATUS StartAudioLossThread( CDevice * pDevice );

NTSTATUS StopAudioLossThread( CDevice * pDevice );

int OnAudioLossThread( void * p_context );

NTSTATUS StartVideoLossThread( CDevice * pDevice );

NTSTATUS StopVideoLossThread( CDevice * pDevice );

int OnVideoLossThread( void * p_context );

NTSTATUS StartMultiChVideoLossThread(CDevice* pDevice);

NTSTATUS StopMultiChVideoLossThread(CDevice* pDevice);

int OnMultiChVideoLossThread(void* p_context);

VOID CALCULATE_FPS(CDevice* pDevice, CVideo* pVideo, ULONG real_fps, ULONG output_fps);



#endif

