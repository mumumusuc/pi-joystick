#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/usb/g_hid.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/usb/composite.h>
#include <linux/usb/g_hid.h>
#include <linux/hid.h>
#include <linux/usb/composite.h>
#include "joystick.h"

/* please read drivers/usb/gadget/legacy/hid.c */

/* from u_hid.h */
struct f_hid_opts {
    struct usb_function_instance func_inst;
    int minor;
    unsigned char subclass;
    unsigned char protocol;
    unsigned short report_length;
    unsigned short report_desc_length;
    unsigned char *report_desc;
    bool report_desc_alloc;
    struct mutex lock;
    int refcnt;
};

struct hidg_func_dev {
    struct usb_function_instance *func_instance;
    struct usb_function *func;
    struct hidg_func_descriptor *func_descriptor;
} entry = {
        .func_descriptor = &joystick_data,
};

static struct usb_device_descriptor device_desc = {
        .bLength            = sizeof(device_desc),
        .bDescriptorType    = USB_DT_DEVICE,
        .bcdUSB             = cpu_to_le16(0x0200),
        .bDeviceClass       = USB_CLASS_PER_INTERFACE,
        .bDeviceSubClass    = 0,
        .bDeviceProtocol    = 0,
        .bMaxPacketSize0    = 64,
        .idVendor           = cpu_to_le16(HIDG_VENDOR_NUM),
        .idProduct          = cpu_to_le16(HIDG_PRODUCT_NUM),
        .bcdDevice          = cpu_to_le16(BCD_DEVICE),
        .iManufacturer      = 1,
        .iProduct           = 2,
        .iSerialNumber      = 0,
        .bNumConfigurations = 1,
};

static const struct usb_descriptor_header *otg_desc[2];

static struct usb_string strings_dev[] = {
        [USB_GADGET_MANUFACTURER_IDX].s = MANUFACTURER,
        [USB_GADGET_PRODUCT_IDX].s      = PRODUCT,
        [USB_GADGET_SERIAL_IDX].s       = SERIAL,
        {} /* end of list */
};

static struct usb_gadget_strings stringtab_dev = {
        .language    = 0x0409,    /* en-us */
        .strings    = strings_dev,
};

static struct usb_gadget_strings *dev_strings[] = {
        &stringtab_dev,
        NULL,
};

#if DEBUG
void check_confs(struct usb_function *func) {
    int i, size;
    u8 *tmp;
    struct usb_interface_descriptor *hidg_interface_desc = (struct usb_interface_descriptor *) func->hs_descriptors[0];
    struct hid_descriptor *hidg_desc = (struct hid_descriptor *) func->hs_descriptors[1];
    struct usb_endpoint_descriptor *hidg_hs_in_ep_desc = (struct usb_endpoint_descriptor *) func->hs_descriptors[2];
    struct usb_endpoint_descriptor *hidg_hs_out_ep_desc = (struct usb_endpoint_descriptor *) func->hs_descriptors[3];
    debug("hidg_interface_desc[%d]:", size = hidg_interface_desc->bLength);
    tmp = (u8 *) hidg_interface_desc;
    for (i = 0; i < size; i++) {
        printk("0x%02X, ", *(tmp + i));
    }
    printk("\n");
    debug("hidg_desc[%d]:", size = hidg_desc->bLength);
    tmp = (u8 *) hidg_desc;
    for (i = 0; i < size; i++) {
        printk("0x%02X, ", *(tmp + i));
    }
    printk("\n");
    debug("hidg_hs_in_ep_desc[%d]:", size = hidg_hs_in_ep_desc->bLength);
    tmp = (u8 *) hidg_hs_in_ep_desc;
    for (i = 0; i < size; i++) {
        printk("0x%02X, ", *(tmp + i));
    }
    printk("\n");
    debug("hidg_hs_out_ep_desc[%d]:\n", size = hidg_hs_out_ep_desc->bLength);
    tmp = (u8 *) hidg_hs_out_ep_desc;
    for (i = 0; i < size; i++) {
        printk("0x%02X, ", *(tmp + i));
    }
    printk("\n");
}
#endif

static int do_config(struct usb_configuration *c) {
    int status = 0;
    struct hidg_func_dev *e = &entry, *n;
    debug();
    if (gadget_is_otg(c->cdev->gadget)) {
        c->descriptors = otg_desc;
        c->bmAttributes |= USB_CONFIG_ATT_WAKEUP;
    }

    e->func = usb_get_function(e->func_instance);
    if (IS_ERR(e->func))
        goto put;
    status = usb_add_function(c, e->func);
    if (status < 0) {
        printk(KERN_DEBUG"[%s] status = %d\n", __func__, status);
        usb_put_function(e->func);
        goto put;
    }
#if DEBUG
    check_confs(e->func);
#endif
    return 0;
    put:
    usb_remove_function(c, n->func);
    usb_put_function(n->func);
    return status;
}

static struct usb_configuration config_driver = {
        .label                  = "HID Gadget",
        .bConfigurationValue    = 1,
        .bmAttributes           = USB_CONFIG_ATT_SELFPOWER,
};

static int hid_bind(struct usb_composite_dev *cdev) {
    int status;
    struct usb_gadget *gadget = cdev->gadget;
    struct hidg_func_dev *n = &entry, *m;
    struct f_hid_opts *hid_opts;
    printk(KERN_DEBUG"[%s]\n", __func__);

    n->func_instance = usb_get_function_instance("hid");
    if (IS_ERR(n->func_instance)) {
        status = PTR_ERR(n->func_instance);
        goto put;
    }
    hid_opts = container_of(n->func_instance, struct f_hid_opts, func_inst);
    hid_opts->subclass = n->func_descriptor->subclass;
    hid_opts->protocol = n->func_descriptor->protocol;
    hid_opts->report_length = n->func_descriptor->report_length;
    hid_opts->report_desc_length = n->func_descriptor->report_desc_length;
    hid_opts->report_desc = n->func_descriptor->report_desc;

    status = usb_string_ids_tab(cdev, strings_dev);
    if (status < 0)
        goto put;

    device_desc.iManufacturer = strings_dev[USB_GADGET_MANUFACTURER_IDX].id;
    device_desc.iProduct = strings_dev[USB_GADGET_PRODUCT_IDX].id;
    debug("iManufacturer[%u], iProduct[%u]", device_desc.iManufacturer, device_desc.iProduct);

    if (gadget_is_otg(gadget) && !otg_desc[0]) {
        struct usb_descriptor_header *usb_desc;
        usb_desc = usb_otg_descriptor_alloc(gadget);
        if (!usb_desc)
            goto put;
        usb_otg_descriptor_init(gadget, usb_desc);
        otg_desc[0] = usb_desc;
        otg_desc[1] = NULL;
    }
    status = usb_add_config(cdev, &config_driver, do_config);
    if (status < 0)
        goto free_otg_desc;
    return 0;

    free_otg_desc:
    kfree(otg_desc[0]);
    otg_desc[0] = NULL;
    put:
    usb_put_function_instance(m->func_instance);
    return status;
}

static int hid_unbind(struct usb_composite_dev *cdev) {
    struct hidg_func_dev *n = &entry;
    usb_put_function(n->func);
    usb_put_function_instance(n->func_instance);
    kfree(otg_desc[0]);
    otg_desc[0] = NULL;
    return 0;
}

static struct usb_composite_driver hidg_driver = {
        .name       = "g_hid",
        .dev        = &device_desc,
        .strings    = dev_strings,
        .max_speed  = USB_SPEED_HIGH,
        .bind       = hid_bind,
        .unbind     = hid_unbind,
};

static int __init hidg_init(void) {
    int status;
    debug();
    status = usb_composite_probe(&hidg_driver);
    return status;
}

static void __exit hidg_cleanup(void) {
    debug();
    usb_composite_unregister(&hidg_driver);
}

module_init(hidg_init);
module_exit(hidg_cleanup);
MODULE_AUTHOR("mumumusuc@gmail.com");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DRIVER_DESC);