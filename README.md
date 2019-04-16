# pi zero joystick
**测试环境 raspberry-zero-w with Linux/arm 4.14.95+ kernel**

### 配置pi-zero
**若已配置了dwc2,g_ether则需要配置pi0其他的连接方式**

1. 开启dwc2
    在/boot/config.txt文末添加
    `dtoverlay=dwc2`

2. 开机加载libcomposite
    在/etc/modules文末添加
    `libcomposite`
    
### 加载模块
1. 编译

    **CMakeLists.txt仅用于IDE浏览，不能进行编译**
    + 本地编译
    
        下载完整的linux-headers
        
        ```
        apt install raspberrypi-kernel-headers
        ```
        
        修改Makefile
        
        ```
        obj-m += js-audio.o
        js-audio-objs = driver.o audio.o joystick.o
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
  
2. 配置udev

    复制规则文件到pi-zero中
    
    ```
    # cp ./udev/50-joystick.rules /etc/udev/rules.d/
    ```
    更新规则
    
    ```
    # sudo udevadm control --reload-rules
    # sudo udevadm trigger
    ```
    
3. 加载

	```
    # insmod js-audio.ko
	```
	
	或
	
    ```
    # make install
    ```
    
    成功后可在/dev看到新增的hid设备
    
    ```
    # ls -al /dev/hid*
    
    crw-rw-rw- 1 root root 243, 0 3月  11 17:38 /dev/hidg0
    ```
    
3. 卸载

	```
    # rmmod js-audio.ko
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

### 模拟声卡(New)

事实上Nintendo Switch是支持Gadget复合设备的，可以在此基础上实现Joystick/Audio复合设备。
从[USB DAC with new 4.0 update](https://www.reddit.com/r/NintendoSwitch/comments/77whjd/usb_dac_with_new_40_update/?utm_source=BD&utm_medium=Search&utm_name=Bing&utm_content=PSR1)可以了解到switch支持了UAC1.0规范，而pi-zero的默认内核实现的UAC2.0，所以直接加载g_audio模块连接到switch上是不会被识别的。

1. 重新编译树莓派的linux内核(<del>是否必要？</del>)

    在路径 

    > Device Drivers > USB support > USB Gadget Support
    
    下选中
    
    > UAC 1.0

    ![编译支持UAC 1.0](https://github.com/mumumusuc/pi-joystick/blob/master/image/pi_1.png)
	
	保存配置后重新编译内核安装即可
	
2. 测试USB声卡

    在主机端查看内核信息或者进入设置中的声音选项可以看到pi-zero模拟的声卡设备信息
   
    ![模拟USB声卡](https://github.com/mumumusuc/pi-joystick/blob/master/image/pi_2.png)
	
3. 连接到Nintendo Switch！

    ![连接](https://github.com/mumumusuc/pi-joystick/blob/master/image/pi_3.png)
    
    看来switch已经把pi-zero模拟的复合设备分别识别为手柄和USB声卡了。	
	
### 模拟Joy-Con与Pro手柄(New)

关于Joy-Con可以参照这个著名的逆向工程[Nintendo_Switch_Reverse_Engineering](https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering)，
也可以参照我dump的Pro设备描述符[pro-hid-descriptor.txt](https://github.com/mumumusuc/pi-joystick/blob/master/pro-hid-descriptor.txt)。

相对于Hori的手柄，NS的Pro提供了更加复杂和丰富的设备指令，包括HD震动、红外摄像头、陀螺仪与加速度传感器等，并且可以自行配置手柄在系统中的颜色显示。当然，协议也是相当复杂。
相关驱动我追加在了[pi-ds4-dongle](https://github.com/mumumusuc/pi-ds4-dongle)。

![模拟的Pro手柄](https://github.com/mumumusuc/pi-joystick/blob/master/image/pro.jpg)
	
### 转接PS4蓝牙手柄

我的另一项目[pi-ds4-dongle](https://github.com/mumumusuc/pi-ds4-dongle)。
	
### 参考项目

[Switch-Fightstick](https://github.com/progmem/Switch-Fightstick )

[SwitchInputEmulator](https://github.com/wchill/SwitchInputEmulator )

