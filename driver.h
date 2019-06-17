#ifndef HID_DRIVER_H
#define HID_DRIVER_H

#include "joystick.h"

int audio_bind(struct usb_composite_dev *);

int audio_unbind(struct usb_composite_dev *);

int audio_config(struct usb_configuration *);

int joystick_bind(struct usb_composite_dev *);

int joystick_unbind(struct usb_composite_dev *);

int joystick_config(struct usb_configuration *);

#endif //HID_DRIVER_H
