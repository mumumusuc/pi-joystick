#ifndef HID_JOYSTICK_H
#define HID_JOYSTICK_H

#include <linux/usb/g_hid.h>

#define HIDG_VENDOR_NUM     0x0f0d
#define HIDG_PRODUCT_NUM    0x00c1
#define BCD_DEVICE          0x0572
#define MANUFACTURER        "HORI CO.,LTD."
#define PRODUCT             "HORIPAD S"
#define SERIAL              ""
#define DRIVER_DESC         "HID Joystick"

#define DEBUG   0
#if DEBUG
#define debug(fmt, ...) printk(KERN_DEBUG"[%s] "fmt"\n", __func__,##__VA_ARGS__)
#else
#define debug(fmt, ...)  {}
#endif

extern struct hidg_func_descriptor joystick_data;

#endif
