#!/bin/bash

# Check if current user is root, if not, add sudo 
if [ "$EUID" -ne 0 ]; then
    export SUDO=sudo 
else
    export SUDO
fi

# Set firmware path 
export FW_PATH=/lib/firmware

# Set module path
export MODULE_PATH=/lib/modules/`uname -r`

# Set module name
export MODULE_NAME=LXV4L2D_SC0710

# Set module file
export MODULE_FILE=$MODULE_NAME.ko

# Copy firmware files to firmware path
$SUDO mkdir -p $FW_PATH
$SUDO cp SC0710*.HEX $FW_PATH/


# Copy module file to module path
$SUDO mkdir -p $MODULE_PATH
$SUDO cp $MODULE_FILE $MODULE_PATH/$MODULE_FILE

# Check module dep
$SUDO depmod -a

# Start module
$SUDO modprobe $MODULE_NAME
