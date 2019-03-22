#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb/composite.h>
#include "driver.h"

#define DRIVER_DESC           "Joystick/Audio Gadget"
#define DRIVER_VERSION        "March 22, 2019"
#define USB_VENDOR_NUM        0x0f0d
#define USB_PRODUCT_NUM       0x00c1
#define BCD_DEVICE            0x0572

static struct usb_string strings_dev[] = {
        [USB_GADGET_MANUFACTURER_IDX].s = "",
        [USB_GADGET_PRODUCT_IDX].s      = DRIVER_DESC,
        [USB_GADGET_SERIAL_IDX].s       = "",
        {}
};

static struct usb_gadget_strings stringtab_dev = {
        .language   = 0x0409,
        .strings    = strings_dev,
};

static struct usb_gadget_strings *usb_strings[] = {
        &stringtab_dev,
        NULL,
};

static struct usb_configuration configuration = {
        .label                  = "Joystick/Audio",
        .bConfigurationValue    = 1,
        .bmAttributes           = USB_CONFIG_ATT_SELFPOWER,
};

static int do_config(struct usb_configuration *c) {
    int ret;
    ret = joystick_config(c);
    if (ret < 0)
        pr_err("[%s] error[%d]\n", __func__, ret);
    ret = audio_config(c);
    if (ret < 0)
        pr_err("[%s] error[%d]\n", __func__, ret);
    return 0;
}

static struct usb_device_descriptor device_desc = {
        .bLength            = sizeof device_desc,
        .bDescriptorType    = USB_DT_DEVICE,
        .bDeviceClass       = USB_CLASS_MISC,
        .bDeviceSubClass    = 0x02,
        .bDeviceProtocol    = 0x01,
        .bcdUSB             = cpu_to_le16(0x0200),
        .idVendor           = cpu_to_le16(USB_VENDOR_NUM),
        .idProduct          = cpu_to_le16(USB_PRODUCT_NUM),
        .bcdDevice          = cpu_to_le16(BCD_DEVICE),
        .bNumConfigurations = 1,
};

static int do_bind(struct usb_composite_dev *cdev) {
    int status;
    status = usb_string_ids_tab(cdev, strings_dev);
    if (status < 0)
        return status;
    device_desc.iManufacturer = strings_dev[USB_GADGET_MANUFACTURER_IDX].id;
    device_desc.iProduct = strings_dev[USB_GADGET_PRODUCT_IDX].id;
    device_desc.iSerialNumber = strings_dev[USB_GADGET_SERIAL_IDX].id;
    INFO(cdev, "%s, version: %s\n", DRIVER_DESC, DRIVER_VERSION);
    status = joystick_bind(cdev);
    if (status < 0)
        return status;
    status = audio_bind(cdev);
    if (status < 0)
        goto unbind_js;
    status = usb_add_config(cdev, &configuration, do_config);
    if (status < 0)
        goto unbind_all;
    return 0;

    unbind_all:
    audio_unbind(cdev);
    unbind_js:
    joystick_unbind(cdev);
    return status;
}

static int do_unbind(struct usb_composite_dev *cdev) {
    audio_unbind(cdev);
    joystick_unbind(cdev);
    return 0;
}

static struct usb_composite_driver usb_driver = {
        .name       = "joystick&audio",
        .dev        = &device_desc,
        .strings    = usb_strings,
        .max_speed  = USB_SPEED_HIGH,
        .bind       = do_bind,
        .unbind     = do_unbind,
};

module_usb_composite_driver(usb_driver);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR("mumumusuc@gmail.com");
MODULE_LICENSE("GPL");