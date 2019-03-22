#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb/composite.h>

#define DRIVER_DESC "audio"
/* from u_uac1.h */
#define UAC1_OUT_EP_MAX_PACKET_SIZE    200
#define UAC1_DEF_CCHMASK    0x3
#define UAC1_DEF_CSRATE     48000
#define UAC1_DEF_CSSIZE     2
#define UAC1_DEF_PCHMASK    0x3
#define UAC1_DEF_PSRATE     48000
#define UAC1_DEF_PSSIZE     2
#define UAC1_DEF_REQ_NUM    2

struct f_uac1_opts {
    struct usb_function_instance func_inst;
    int c_chmask;
    int c_srate;
    int c_ssize;
    int p_chmask;
    int p_srate;
    int p_ssize;
    int req_number;
    unsigned bound:1;
    struct mutex lock;
    int refcnt;
};

/* Playback(USB-IN) Default Stereo - Fl/Fr */
static int p_chmask = UAC1_DEF_PCHMASK;
/* Playback Default 48 KHz */
static int p_srate = UAC1_DEF_PSRATE;
/* Playback Default 16bits/sample */
static int p_ssize = UAC1_DEF_PSSIZE;
/* Capture(USB-OUT) Default Stereo - Fl/Fr */
static int c_chmask = UAC1_DEF_CCHMASK;
/* Capture Default 48 KHz */
static int c_srate = UAC1_DEF_CSRATE;
/* Capture Default 16bits/sample */
static int c_ssize = UAC1_DEF_CSSIZE;

static struct usb_function_instance *fi_uac1;
static struct usb_function *f_uac1;

int audio_config(struct usb_configuration *c) {
    int status;
    f_uac1 = usb_get_function(fi_uac1);
    if (IS_ERR(f_uac1)) {
        status = PTR_ERR(f_uac1);
        return status;
    }
    status = usb_add_function(c, f_uac1);
    if (status < 0) {
        usb_put_function(f_uac1);
        return status;
    }
    return 0;
}

int audio_bind(struct usb_composite_dev *cdev) {
    struct f_uac1_opts *uac1_opts;
    fi_uac1 = usb_get_function_instance("uac1");
    if (IS_ERR(fi_uac1))
        return PTR_ERR(fi_uac1);
    uac1_opts = container_of(fi_uac1, struct f_uac1_opts, func_inst);
    uac1_opts->p_chmask = p_chmask;
    uac1_opts->p_srate = p_srate;
    uac1_opts->p_ssize = p_ssize;
    uac1_opts->c_chmask = c_chmask;
    uac1_opts->c_srate = c_srate;
    uac1_opts->c_ssize = c_ssize;
    uac1_opts->req_number = UAC1_DEF_REQ_NUM;
    return 0;
}

int audio_unbind(struct usb_composite_dev *cdev) {
    if (!IS_ERR_OR_NULL(f_uac1))
        usb_put_function(f_uac1);
    if (!IS_ERR_OR_NULL(fi_uac1))
        usb_put_function_instance(fi_uac1);
    return 0;
}