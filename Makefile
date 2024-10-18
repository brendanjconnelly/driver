KERNEL_VERSION=$(shell uname -r)
MODULE_DIR += ${PWD}

obj-m += drv.o
drv-y := src/char_driver.o src/fops.o

ccflags-y += -Wall -DTEST_DRV_VER=\"${shell cat ${MODULE_DIR}/buildno}\" -Isrc/

all: compile

compile:
	echo $$(($$(cat ${MODULE_DIR}/buildno) + 1)) > ${MODULE_DIR}/buildno
	make -C /lib/modules/${KERNEL_VERSION}/build M=${PWD} modules

clean:
	mv drv.ko drv.ko_
	make -C /lib/modules/${KERNEL_VERSION}/build M=${PWD} clean
	mv drv.ko_ drv.ko
