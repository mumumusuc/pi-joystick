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