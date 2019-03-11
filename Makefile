obj-m += joystick.o

HEAD := $(shell uname -r)
#KERNEL := /usr/src/linux-headers-$(HEAD)/
KERNEL := /home/mumumusuc/raspberry/linux/
PWD := $(shell pwd)

CC = arm-linux-gnueabihf-gcc

all:
	make -C $(KERNEL) M=$(PWD) modules
clean:
	make -C $(KERNEL) M=$(PWD) clean
install:
	-rmmod joystick
	-insmod joystick.ko
uninstall:
	-rmmod joystick

