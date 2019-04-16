#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb/composite.h>

#ifdef CONFIG_GADGET_UAC1
#define UAC_1
#endif

#define DRIVER_DESC "audio"
#define UAC_DEF_REQ_NUM    2

#ifdef UAC_1
/* from u_uac1.h */
#define UAC1_DEF_CCHMASK    0x3
#define UAC1_DEF_CSRATE     48000
#define UAC1_DEF_CSSIZE     2
#define UAC1_DEF_PCHMASK    0x3
#define UAC1_DEF_PSRATE     48000
#define UAC1_DEF_PSSIZE     2

typedef struct f_uac1_opts {
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
} f_uac_opts_t;

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

static struct usb_function_instance *fi_uac;
static struct usb_function *f_uac;
#else
/* frome u_uac2.h */
#define UAC2_DEF_PCHMASK 0x3
#define UAC2_DEF_PSRATE 48000
#define UAC2_DEF_PSSIZE 2
#define UAC2_DEF_CCHMASK 0x3
#define UAC2_DEF_CSRATE 48000
#define UAC2_DEF_CSSIZE 2

typedef struct f_uac2_opts {
    struct usb_function_instance func_inst;
    int p_chmask;
    int p_srate;
    int p_ssize;
    int c_chmask;
    int c_srate;
    int c_ssize;
    int req_number;
    bool bound;

    struct mutex lock;
    int refcnt;
} f_uac_opts_t;

/* Playback(USB-IN) Default Stereo - Fl/Fr */
static int p_chmask = UAC2_DEF_PCHMASK;
/* Playback Default 48 KHz */
static int p_srate = UAC2_DEF_PSRATE;
/* Playback Default 16bits/sample */
static int p_ssize = UAC2_DEF_PSSIZE;
/* Capture(USB-OUT) Default Stereo - Fl/Fr */
static int c_chmask = UAC2_DEF_CCHMASK;
/* Capture Default 48 KHz */
static int c_srate = UAC2_DEF_CSRATE;
/* Capture Default 16bits/sample */
static int c_ssize = UAC2_DEF_CSSIZE;

static struct usb_function_instance *fi_uac;
static struct usb_function *f_uac;
#endif

int audio_config(struct usb_configuration *c) {
    int status;
    f_uac = usb_get_function(fi_uac);
    if (IS_ERR(f_uac)) {
        status = PTR_ERR(f_uac);
        return status;
    }
    status = usb_add_function(c, f_uac);
    if (status < 0) {
        usb_put_function(f_uac);
        return status;
    }
    return 0;
}

int audio_bind(struct usb_composite_dev *cdev) {
#ifdef UAC_1
    struct f_uac1_opts *uac_opts;
    fi_uac = usb_get_function_instance("uac1");
#else
    struct f_uac2_opts *uac_opts;
    fi_uac = usb_get_function_instance("uac2");
#endif
    if (IS_ERR(fi_uac))
        return PTR_ERR(fi_uac);
    uac_opts = container_of(fi_uac, f_uac_opts_t, func_inst);
    uac_opts->p_chmask = p_chmask;
    uac_opts->p_srate = p_srate;
    uac_opts->p_ssize = p_ssize;
    uac_opts->c_chmask = c_chmask;
    uac_opts->c_srate = c_srate;
    uac_opts->c_ssize = c_ssize;
    uac_opts->req_number = UAC_DEF_REQ_NUM;
    return 0;
}

int audio_unbind(struct usb_composite_dev *cdev) {
    if (!IS_ERR_OR_NULL(f_uac))
        usb_put_function(f_uac);
    if (!IS_ERR_OR_NULL(fi_uac))
        usb_put_function_instance(fi_uac);
    return 0;
}