#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#if defined( __FreeBSD__)
#include <libusb.h>
#else
#include <libusb-1.0/libusb.h>
#endif

#define USB_HID_SET_REPORT		0x09

#define USB_HID_REPORT_TYPE_INPUT	1
#define USB_HID_REPORT_TYPE_OUTPUT	2
#define USB_HID_REPORT_TYPE_FEATURE	3

#define USB_REQUEST_TYPE_OUT		(LIBUSB_REQUEST_TYPE_CLASS | \
	LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_OUT)

#define USB_INTERFACE			0

#define VENDOR_ID	0x0d8c
#define PRODUCT_ID	0x0102
#define CM_EP_IN	0x81

#define MAX_SIZE 64

void cm_close_device(libusb_context *ctx, libusb_device_handle *devh) {
  libusb_close(devh);
  libusb_exit(ctx);
}

libusb_device_handle* cm_open_device(libusb_context *ctx) {
  struct libusb_device_handle *devh = NULL;
  libusb_device *dev;
  libusb_device **devs;

  int r = 1;
  int i = 0;
  int cnt = 0;

//  libusb_set_debug(ctx, 3);
  
  if ((libusb_get_device_list(ctx, &devs)) < 0) {
//    perror("no usb device found");
//    exit(1);
    return -1;
  }

  /* check every usb devices */
  while ((dev = devs[i++]) != NULL) {
    struct libusb_device_descriptor desc;
    if (libusb_get_device_descriptor(dev, &desc) < 0) {
      perror("failed to get device descriptor\n");
    }
    /* count how many device connected */
    if (desc.idVendor == VENDOR_ID && desc.idProduct == PRODUCT_ID) {
      cnt++;
    }
  }

  /* device not found */
  if (cnt == 0) {
//    fprintf(stderr, "device not connected or lack of permissions\n");
//    exit(1);
    return -1;
  }

  if (cnt > 1) {
//    fprintf(stderr, "multi device is not implemented yet\n");
//    exit(1);
    return -1;
  }


  /* open device */
  if ((devh = libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID)) < 0) {
    perror("can't find device\n");
//    close_device(ctx, devh);
//    exit(1);
    return -1;
  } 

  /* detach kernel driver if attached. */
  r = libusb_kernel_driver_active(devh, 3);
  if (r == 1) {
    /* detaching kernel driver */
    r = libusb_detach_kernel_driver(devh, 3);
    if (r != 0) {
//      perror("detaching kernel driver failed");
//      exit(1);
      return -1;
    }
  }

  r = libusb_claim_interface(devh, 0);
  if (r < 0) {
//    fprintf(stderr, "claim interface failed (%d): %s\n", r, strerror(errno));
//    exit(1);
    return -1;
  }

  return devh;
}

static void write_device(struct libusb_device_handle *devh, unsigned char *cmd, int len) {
  int i, r;
  uint8_t buf[MAX_SIZE];

  int size = 0;

  memset(buf, 0xff, sizeof(buf));
  for (i = 0; i < len; i++) {
    buf[i] = cmd[i];
  }
  r = libusb_control_transfer(devh, USB_REQUEST_TYPE_OUT,
		USB_HID_SET_REPORT, USB_HID_REPORT_TYPE_FEATURE, USB_INTERFACE,
		(unsigned char *)buf, len, 1000);
  if (r < 0) {
    fprintf(stderr, "libusb_control_transfer (%d): %s\n", r, strerror(errno));
    exit(1);
  }
}

static int read_device(struct libusb_device_handle *devh, unsigned char *buf, int bufsize) {
  int size = 0;
  memset(buf, 0x00, bufsize);

  int r = libusb_interrupt_transfer(devh, CM_EP_IN, buf, bufsize, &size, 100);
  if (r < 0) {
    fprintf(stderr, "libusb_interrupt_transfer (%d): %s\n", r, strerror(errno));
    exit(1);
  }

  return size;
}

int cm6206_write(struct libusb_device_handle *devh, int regnum, int value)
{
  int res;
  unsigned char buf[4] = {
            0x20,           		// 0x30 = read, 0x20 = write
            value & 0xff,		// DATAL
            (value >> 8) & 0xff,	// DATAH
            regnum			// Register address
    };

  if (regnum > 5)
	return 0;

  write_device(devh, buf, 4);

  return 1;
}

int cm6206_read(struct libusb_device_handle *devh, int regnum, int *value)
{
  int res;
  unsigned char buf[4] = {
            0x30,           // 0x30 = read, 0x20 = write
            0x00,           // DATAL
            0x00,           // DATAH
            regnum          // Register address
    };

  if (regnum > 5)
	return 0;

  write_device(devh, buf, 4);
  res = read_device(devh, buf, 3);

  if ((buf[0] & 0xe0) != 0x20)    // No register data in the input report
    return -3;

  *value = (((uint16_t)buf[2]) << 8) | buf[1];

  return 1;
}

#ifdef NOTUSE

int main(int argc, char *argv[]) {
  libusb_context *ctx = NULL;
  int r = 1;
  int val;

  /* libusb initialize*/
  if ((r = libusb_init(&ctx)) < 0) {
    perror("libusb_init\n");
    exit(1);
  } 

  /* open device */
  libusb_device_handle *devh = open_device(ctx);

  cm6206_read(devh, 0, &val);

  printf("REG0: %04x\n", val);

  close_device(ctx, devh);

  return 0;
}
#endif
