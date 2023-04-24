KERNEL_VERSION := $(shell uname -r)

#released binary /usr/src/kernels/x.x.x
KERNEL_DIR := /lib/modules/$(KERNEL_VERSION)/build

#customed kernel
#KERNEL_DIR := /home/rd/Downloads/linux-5.x.x/

INSTALL_MOD_DIR := misc

PWD := $(shell pwd)

### Set OS type

### We currently need to know if OS is RHEL(Redhat, centos, rocky, almalinux, fedora, etc)

#DEF_OS_TYPE=RHEL

DEF_OS_TYPE=Ubuntu



### Define to switch off all log

### Both LINUXV4L2_PRINT and LINUXV4L2_DEBUG will not print

#DEF_NO_LOG=1



### Define to switch on / off debug mode, 

### if on, LINUXV4L2_PRINT and LINUXV4L2_DEBUG will print.

### if off, only LINUXV4L2_PRINT will print

DEF_DEBUG=1

ifeq ($(DEF_OS_TYPE), RHEL)

CCOPT += -DDEF_OS_RHEL

endif



ifeq ($(DEF_NO_LOG), 1)

CCOPT += -DDEF_NO_LOG

endif



ifeq ($(DEF_DEBUG), 1)

CCOPT += -DDEF_DEBUG

endif

################################

DEVICE=AME_SC0710

ifeq ($(DEVICE), AME_SC0710)
CCOPT	+= -DAME_SC0710 
obj-m := LXV4L2D_SC0710.o
LXV4L2D_SC0710-objs := LINUXV4L2.o property.o lmh1297.o sc0710.o
endif


EXTRA_CFLAGS	+= $(CCOPT)

all: build

version.h: version.h.in
ifndef VERSION
	sed "s/@VERSION@/1/" version.h.in > version.h
else
	sed "s/@VERSION@/$(VERSION)/" version.h.in > version.h
endif


build: version.h

	@echo "     Linux V4L2 Device Driver - Building ..."

#	@(cd $(KERNEL_DIR) && make -C $(KERNEL_DIR) SUBDIRS=$(PWD) CROSS_COMPILE=$(CROSS_COMPILE) modules)

	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

	-rm -f .*.cmd Module.symvers


install:

	@echo "     Linux V4L2 Device Driver - Installing ..."

	@(cd $(KERNEL_DIR) && make -C $(KERNEL_DIR) M=$(PWD) INSTALL_MOD_DIR=$(INSTALL_MOD_DIR) INSTALL_MOD_PATH=$(INSTALL_MOD_PATH) modules_install)

	depmod -a $(KERNEL_VERSION)

ifeq ($(DEVICE), AME_QP0204)
	@echo LXV4L2D_QP0204 >> /etc/modules
endif

start:

	@echo "     Linux V4L2 Device Driver - Starting ..."

#	modprobe LXV4L2D

ifeq ($(DEVICE), AME_SC0710)
	modprobe LXV4L2D_SC0710
endif

stop:

	@echo "     Linux V4L2 Device Driver - Stoping ..."

#	modprobe -r LXV4L2D
ifeq ($(DEVICE), AME_SC0710)
	modprobe -r LXV4L2D_SC0710
endif

clean:

	@echo "     Linux V4L2 Device Driver - Cleaning ..."

	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

	-rm -f *.o *.ko .*.cmd .*.flags *.mod.c Module.symvers version.h

	-rm -rf .tmp_versions

	-rm -f *.h~ *.c~ Makefile~

	-rm -rf Release

	-rm -rf Debug

	-rm -f .*

go: version.h

	@echo "     Linux V4L2 Device Driver - Stoping ..."

#	modprobe -r LXV4L2D
ifeq ($(DEVICE), AME_SC0710)
	modprobe -r LXV4L2D_SC0710
endif

	@echo "     Linux V4L2 Device Driver - Cleaning ..."

	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

	-rm -f *.o *.ko .*.cmd .*.flags *.mod.c Module.symvers
	-rm -rf .tmp_versions

	-rm -f *.h~ *.c~ Makefile~

	-rm -rf Release

	-rm -rf Debug

	@echo "     Linux V4L2 Device Driver - Building ..."

#	@(cd $(KERNEL_DIR) && make -C $(KERNEL_DIR) SUBDIRS=$(PWD) CROSS_COMPILE=$(CROSS_COMPILE) modules)

	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

	@echo "     Linux V4L2 Device Driver - Installing ..."

	@(cd $(KERNEL_DIR) && make -C $(KERNEL_DIR) M=$(PWD) INSTALL_MOD_DIR=$(INSTALL_MOD_DIR) INSTALL_MOD_PATH=$(INSTALL_MOD_PATH) modules_install)

	depmod -a $(KERNEL_VERSION)

	@echo "     Linux V4L2 Device Driver - Starting ..."
	
	echo 3 > /proc/sys/vm/drop_caches
	
#	modprobe LXV4L2D
ifeq ($(DEVICE), AME_SC0710)
	modprobe LXV4L2D_SC0710
endif

