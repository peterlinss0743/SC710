Step 0) SC710
   please set "DEVICE=AME_SC0710" in MAKEFILE then make

Step1. Copy /firmware/SC* to /lib/firmware/

cp /firmware/SC* /lib/firmware/

Step2. Copy LXV4L2D_SC0710.ko to your module directory.

cp LXV4L2D_SC0710.ko /lib/modules/`uname -r`/

Step3. Add LXV4L2D_SC0710.ko to modules.dep.

depmod -a

Step4. Load LXV4L2D_SC0710.ko module.

modprobe LXV4L2D_SC0710



If ¡§invalid module format¡¨ error occurs, try to use ¡Vf parameter.

modprobe -f LXV4L2D_SC0710



[AUDIO CAPTURE]
1) 2 CHANNEL (STEREO)
arecord -D hw:x,0 -c 2 -r 48000 -f S16_LE --period-size=1024 ch2.wav

2) 8 CHANNEL
arecord -D hw:x,0 -c 8 -r 48000 -f S16_LE --period-size=1024 ch8.wav

3) DOLBY
arecord -D hw:x,0 -c 4 -r 48000 -f S16_LE --period-size=1024 ch4.wav


[INTEL IPP PMU ERROR]
modify function PMU_LIST_Build_MMIO_List() at /opt/intel/oneapi/vtune/2022.3.0/sepdk/src/socperf/src/pmu_list.c
array index pmu_info_index maybe -1

