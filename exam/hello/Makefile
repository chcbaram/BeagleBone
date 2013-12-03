#------------------------------------------------------------------------------
#     TITLE   :  Beaglebone kernel driver Makefile
#     WORK    :
#     Made by :  HanCheol Cho
#------------------------------------------------------------------------------

# 

TARGET = hello

obj-m := ${TARGET}.o


KERNELDIR = ~/linux
PWD := $(shell pwd)


default:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERNELDIR) SUBDIRS=$(PWD) modules       

clean : 
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
