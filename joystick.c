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

int joystick_config(struct usb_configuration *c) {
    int status = 0;
    debug();
    entry.func = usb_get_function(entry.func_instance);
    if (IS_ERR(entry.func))
        goto put;
    status = usb_add_function(c, entry.func);
    if (status < 0)
        goto put;
#if DEBUG
    check_confs(e->func);
#endif
    return 0;

    put:
    usb_remove_function(c, entry.func);
    usb_put_function(entry.func);
    return status;
}

int joystick_bind(struct usb_composite_dev *cdev) {
    struct hidg_func_dev *n = &entry, *m;
    struct f_hid_opts *hid_opts;
    debug();
    n->func_instance = usb_get_function_instance("hid");
    if (IS_ERR(n->func_instance))
        return PTR_ERR(n->func_instance);
    hid_opts = container_of(n->func_instance, struct f_hid_opts, func_inst);
    hid_opts->subclass = n->func_descriptor->subclass;
    hid_opts->protocol = n->func_descriptor->protocol;
    hid_opts->report_length = n->func_descriptor->report_length;
    hid_opts->report_desc_length = n->func_descriptor->report_desc_length;
    hid_opts->report_desc = n->func_descriptor->report_desc;
    return 0;
}

int joystick_unbind(struct usb_composite_dev *cdev) {
    usb_put_function(entry.func);
    usb_put_function_instance(entry.func_instance);
    return 0;
}