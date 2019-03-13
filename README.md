# pi zero joystick
**测试环境 raspberry-zero-w with Linux/arm 4.14.95 kernel**

### 配置pi-zero
**若已配置了dwc2,g_ether则需要配置pi0其他的连接方式**

1. 开启dwc2
    ```
    # echo "dtoverlay=dwc2" >> /boot/config.txt
    ```
    或直接编辑/boot/config.txt文件，在文末添加
    `dtoverlay=dwc2`

2. 开机加载libcomposite
    ```
    # echo "libcomposite" >> /etc/modules
    ```
    或直接编辑/etc/modules文件，在文末添加
    `libcomposite`
    
### 加载g_hid驱动 
pi0的linux内核中(4.14.95)默认编译了g_hid模块，可以直接加载。
```
# modprobe g_hid bcdDevice=0x0572 idVendor=0x0f0d idProduct=0x00c1 iProduct="HORIPAD S" iManufacturer="HORI CO.,LTD."
```
成功后`dmesg`可以看到类似以下的内核信息
```
[10369.856626] g_hid gadget: HID Gadget, version: 2010/03/16
[10369.856644] g_hid gadget: g_hid ready
[10369.856657] dwc2 20980000.usb: bound driver g_hid
[10370.056493] dwc2 20980000.usb: new device is high-speed
[10370.184605] dwc2 20980000.usb: new device is high-speed
[10370.261554] dwc2 20980000.usb: new address 29
[10370.282458] g_hid gadget: high-speed config #1: HID Gadget
```

### 加载本模块(joystick.ko)
1. 编译

    **CMakeLists.txt仅用于IDE浏览，不能进行编译**
    + 本地编译
    
        下载完整的linux-headers
        ```
        apt install raspberrypi-kernel-headers
        ```
        修改Makefile
        ```
        obj-m += joystick.o
        HEAD := $(shell uname -r)
        KERNEL := /usr/src/linux-headers-$(HEAD)/
        PWD := $(shell pwd) 
        all:
        	make -C $(KERNEL) M=$(PWD) modules
        clean:
        	make -C $(KERNEL) M=$(PWD) clean
        ```
        编译
        ```
        make
        ```
    + 交叉编译
    
        请自行搜索搭建树莓派的交叉编译环境，并保证源码版本与树莓派内核版本一致。
    + 内核中没有g_hid模块时
        如果pi-zero的内核中并没有编译g_hid模块，可以修改Makefile中
        ```
        obj-m += joystick.o
        ```
        为
        ```
        obj-m += joystick_driver.o
        ```
        连同hid驱动一起编译并加载
        ```
        # make
        # insmod joystick_driver.ko
        ```
        关于hid驱动详情请参考linux源码
        > drivers/usb/gadget/legacy/hid.c
2. 加载

	```
    # insmod joystick.ko
	```
	或
	
    ```
    # make install
    ```
    
    成功后可在/dev看到新增的hid设备
    
    ```
    # ls -al /dev/hid*
    
    crw------- 1 root root 243, 0 3月  11 17:38 /dev/hidg0
    ```
    
    利用udev或直接修改设备权限
    
    ```
    # chmod 666 /dev/hidg0
    ```
3. 卸载

	```
    # rmmod joystick.ko
	```
	
	或
	
    ```
    # make uninstall
    ```

### 主机端的测试(Ubuntu18.04)

以上步骤成功的话于host端会看到类似内核信息
    
    
```
    # dmesg
    
    [497647.192333] usb 3-1: new high-speed USB device number 12 using xhci_hcd
    [497647.340787] usb 3-1: New USB device found, idVendor=0f0d, idProduct=00c1
    [497647.340789] usb 3-1: New USB device strings: Mfr=1, Product=2, SerialNumber=0
    [497647.340790] usb 3-1: Product: HORIPAD S
    [497647.340791] usb 3-1: Manufacturer: HORI CO.,LTD.
    [497647.342165] input: HORI CO.,LTD. HORIPAD S as /devices/pci0000:00/0000:00:14.0/usb3/3-1/3-1:1.0/0003:0F0D:00C1.00FA/input/input297
    [497647.342284] hid-generic 0003:0F0D:00C1.00FA: input,hidraw2: USB HID v1.01 Gamepad [HORI CO.,LTD. HORIPAD S] on usb-0000:00:14.0-1/input0
```
    
   
我们的树莓派zero已被成功识别为
>USB HID v1.01 Gamepad [HORI CO.,LTD. HORIPAD S]“，“idVendor=0f0d, idProduct=00c1”
    
在ubuntu下可安装Joystick进行查看测试

```
# apt install joystick
```
	
![joystick](https://github.com/mumumusuc/pi-joystick/blob/master/image/joystick.png)
	
### 输入指令与其他

共8个字节，前两字节为14个按键的高低8位，第三字节为方向键，后四字节为左右模拟摇杆的X与Y轴。
    
参照Switch-Fightstick给出的按键值，输入按键A和B
    
    
```
// button A
# echo -ne "\x04\x00\x00\x00\x00\x00\x00\x00" > /dev/hidg0

// button B
# echo -ne "\x02\x00\x00\x00\x00\x00\x00\x00" > /dev/hidg0
```

![简单的GUI测试](https://github.com/mumumusuc/pi-joystick/blob/master/image/switch.gif)

用python+vue可以简单快速的搭建一个图形测试环境，项目中使用的是Sanic后端与vue前端的组合，延迟会相对较大，可用的GUI程序运行在本地会更为合适。


	
### 参考项目

[Switch-Fightstick](https://github.com/progmem/Switch-Fightstick )

[SwitchInputEmulator](https://github.com/wchill/SwitchInputEmulator )

