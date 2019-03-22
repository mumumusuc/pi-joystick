obj-m += js-audio.o
js-audio-objs = driver.o audio.o joystick.o

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
	-rmmod js-audio
	-insmod js-audio.ko
uninstall:
	-rmmod js-audio

