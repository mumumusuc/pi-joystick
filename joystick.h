#ifndef HID_JOYSTICK_H
#define HID_JOYSTICK_H

#include <linux/usb/g_hid.h>
#include "hori-s.h"

#define DEBUG   0
#if DEBUG
#define debug(fmt, ...) printk(KERN_DEBUG"[%s] "fmt"\n", __func__,##__VA_ARGS__)
#else
#define debug(fmt, ...)  {}
#endif

#endif
