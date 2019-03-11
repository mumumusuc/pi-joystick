#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/usb/composite.h>
#include <linux/usb/g_hid.h>
#include "joystick.h"

/* device */
struct hidg_func_descriptor joystick_data = {
        .subclass           = 0,
        .protocol           = 1,
        .report_length      = 64,
        .report_desc_length = 80,
        .report_desc = {
                0x05, 0x01,             // Generic desktop controls
                0x09, 0x05,             // Joystick
                0xA1, 0x01,             // Application
                // Buttons (2 bytes)
                0x15, 0x00,             // button off state
                0x25, 0x01,             // button on state
                0x35, 0x00,             // button off state
                0x45, 0x01,             // button on state
                0x75, 0x01,             // 1 bit per report field
                0x95, 0x0E,             // 14 report fields (14 buttons)
                0x05, 0x09,             // Buttons (section 12)
                0x19, 0x01,
                0x29, 0x0E,
                0x81, 0x02,             // Variable input
                0x95, 0x02,             // 2 report fields (empty 2 bits)
                0x81, 0x01,             // Array input
                // HAT switch
                0x05, 0x01,             // Generic desktop controls
                0x25, 0x07,             // 8 valid HAT states, sending 0x08 = nothing pressed
                0x46, 0x3B, 0x01,       // HAT "rotation"
                0x75, 0x04,             // 4 bits per report field
                0x95, 0x01,             // 1 report field (a nibble containing entire HAT state)
                0x65, 0x14,             // unit degrees
                0x09, 0x39,             // Hat switch (section 4.3)
                0x81, 0x42,             // Variable input, null state
                0x65, 0x00,             // No units
                0x95, 0x01,             // 1 report field (empty upper nibble)
                0x81, 0x01,             // Array input
                // Joystick (4 bytes)
                0x26, 0xFF, 0x00,       // 0-255 for analog sticks
                0x46, 0xFF, 0x00,
                0x09, 0x30,             // X (left X)
                0x09, 0x31,             // Y (left Y)
                0x09, 0x32,             // Z (right X)
                0x09, 0x35,             // Rz (right Y)
                0x75, 0x08,             // 1 byte per report field
                0x95, 0x04,             // 4 report fields (left X, left Y, right X, right Y)
                0x81, 0x02,             // Variable input
                0x75, 0x08,             // 1 byte per report field
                0x95, 0x01,             // 1 report field
                0x81, 0x01,             // Array input
                0xC0,
        },
};


static struct platform_device joystick_dev = {
        .name               = "hidg",
        .id                 = 0,
        .num_resources      = 0,
        .resource           = NULL,
        .dev.platform_data  = &joystick_data,
};

static int __init joystick_init(void) {
    debug();
    return platform_device_register(&joystick_dev);;
}

static void __exit joystick_exit(void) {
    debug();
    platform_device_unregister(&joystick_dev);
}

module_init(joystick_init);
module_exit(joystick_exit);
MODULE_AUTHOR("mumumusuc@gmail.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DRIVER_DESC);