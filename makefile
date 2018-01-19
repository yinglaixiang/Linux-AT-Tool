#--------------------------------------------------------------
#               Quectel OpenLinux
#
#--------------------------------------------------------------
COMPILER_ROOT=/opt/ql-oe/sysroots
CROSS_COMPILER=$(COMPILER_ROOT)/x86_64-linux/usr/bin/arm-oe-linux-gnueabi/arm-oe-linux-gnueabi-
GCC_VERSION=4.9.2

#---------------------
# Include definition
#---------------------
INC_DIR=-I ./ \
		-I ./include \
        -I $(COMPILER_ROOT)/mdm9607/usr/include \
        -I example/at/inc \

#---------------------
# Source code files
#---------------------
LOCAL_SRC_FILES = example/at/example_at.c

#---------------------
# Library definition
#---------------------
STD_LIB_PATH=$(COMPILER_ROOT)/x86_64-linux/usr/lib
OEM_LIB_PATH=$(COMPILER_ROOT)/mdm9607/usr/lib
USR_LIB_PATH=./libs

USR_LIB=
STD_LIB=\
		$(OEM_LIB_PATH)/librt.so \


#---------------------
# Compilation options
#---------------------
LOCAL_C_FLAGS = -march=armv7-a -mfloat-abi=soft -mfpu=vfp --sysroot=$(COMPILER_ROOT)/mdm9607 \
				-g -Wall -Wno-unused-function -pthread


build/$(AppImgBin): clean
	$(CROSS_COMPILER)gcc $(LOCAL_C_FLAGS) $(INC_DIR) $(LOCAL_SRC_FILES) -s -o $@ $(STD_LIB) $(USR_LIB)


clean:
	rm -rf  *~ *.o
