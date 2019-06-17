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
#include <linux/cdev.h>
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

struct f_hidg {
    /* configuration */
    unsigned char bInterfaceSubClass;
    unsigned char bInterfaceProtocol;
    unsigned char protocol;
    unsigned short report_desc_length;
    char *report_desc;
    unsigned short report_length;

    /* recv report */
    struct list_head completed_out_req;
    spinlock_t read_spinlock;
    wait_queue_head_t read_queue;
    unsigned int qlen;

    /* send report */
    spinlock_t write_spinlock;
    bool write_pending;
    wait_queue_head_t write_queue;
    struct usb_request *req;

    int minor;
    struct cdev cdev;
    struct usb_function func;

    struct usb_ep *in_ep;
    struct usb_ep *out_ep;
};


struct hidg_func_dev {
    struct usb_function_instance *func_instance;
    struct usb_function *func;
    struct hidg_func_descriptor *func_descriptor;
} entry = {
        .func_descriptor = &joystick_data,
};

#if DEBUG

void check_confs(struct usb_configuration *c, struct usb_function *func) {
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
    debug("bmAttributes = %02X,(INT=%02X,BULK=%02X)", hidg_hs_in_ep_desc->bmAttributes, USB_ENDPOINT_XFER_INT,
          USB_ENDPOINT_XFER_BULK);
    printk("\n");
    debug("hidg_hs_out_ep_desc[%d]:\n", size = hidg_hs_out_ep_desc->bLength);
    tmp = (u8 *) hidg_hs_out_ep_desc;
    for (i = 0; i < size; i++) {
        printk("0x%02X, ", *(tmp + i));
    }
    printk("\n");

    hidg_hs_in_ep_desc->bmAttributes |= USB_ENDPOINT_XFER_BULK;
    hidg_hs_out_ep_desc->bmAttributes |= USB_ENDPOINT_XFER_BULK;

    struct usb_ep *ep;
    struct f_hidg *hidg = container_of(func, struct f_hidg, func);
    ep = usb_ep_autoconfig(c->cdev->gadget, hidg_hs_in_ep_desc);
    if (!ep)
        goto fail;
    hidg->in_ep = ep;

    ep = usb_ep_autoconfig(c->cdev->gadget, hidg_hs_out_ep_desc);
    if (!ep)
        goto fail;
    hidg->out_ep = ep;

    return;

    fail:
    ERROR(func->config->cdev, "hidg_bind FAILED\n");
    if (hidg->req != NULL)
        debug("free_ep_req");
    //free_ep_req(hidg->in_ep, hidg->req);
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
    check_confs(c, entry.func);
#endif
    return 0;

    put:
    usb_remove_function(c, entry.func);
    usb_put_function(entry.func);
    return status;
}

int joystick_bind(struct usb_composite_dev *cdev) {
    struct f_hid_opts *hid_opts;
    debug();
    entry.func_instance = usb_get_function_instance("hid");
    if (IS_ERR(entry.func_instance))
        return PTR_ERR(entry.func_instance);
    hid_opts = container_of(entry.func_instance, struct f_hid_opts, func_inst);
    hid_opts->subclass = entry.func_descriptor->subclass;
    hid_opts->protocol = entry.func_descriptor->protocol;
    hid_opts->report_length = entry.func_descriptor->report_length;
    hid_opts->report_desc_length = entry.func_descriptor->report_desc_length;
    hid_opts->report_desc = entry.func_descriptor->report_desc;
    return 0;
}

int joystick_unbind(struct usb_composite_dev *cdev) {
    usb_put_function(entry.func);
    usb_put_function_instance(entry.func_instance);
    return 0;
}