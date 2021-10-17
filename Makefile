obj-m += dht11_drv.o
dht11_drv-y := source/dht11_drv.o\
               source/bcm2711_reg.o\
		       source/dht11.o

KERNEL_PATH:="/home/ubuntu/Desktop/linux-raspi-5.4.0"
COMPIL := "/home/ubuntu/Desktop/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-"

ARCH:="arm64"
CURRENT_PATH:=${PWD}
SOURCE_PATH:="${CURRENT_PATH})/source"
INCLUDE_PATH:="${CURRENT_PATH}/include"

ccflags-y += "-I${INCLUDE_PATH}"

all:
	make -C ${KERNEL_PATH} ARCH=${ARCH} CROSS_COMPILE=${COMPIL} M=${CURRENT_PATH} V=0 modules
clean:
	make -C ${KERNEL_PATH} ARCH=${ARCH} CROSS_COMPILE=${COMPIL} M=${CURRENT_PATH} clean