/*
** mrb_cm6206.c - CM6206 class
**
** Copyright (c) Hiroki Mori 2022
**
** See Copyright Notice in LICENSE
*/

#include "mruby.h"
#include "mruby/data.h"
#include "mrb_cm6206.h"

#if defined( __FreeBSD__ )
#include <libusb.h>
#else
#include <libusb-1.0/libusb.h>
#endif

libusb_device_handle* cm_open_device(libusb_context *ctx);
void cm_close_device(libusb_context *ctx, libusb_device_handle *devh);
int cm6206_read(struct libusb_device_handle *devh, int reg, int *val);
int cm6206_write(struct libusb_device_handle *devh, int reg, int val);

#define DONE mrb_gc_arena_restore(mrb, 0);

typedef struct {
  char *str;
  mrb_int len;
  libusb_context *ctx;
  libusb_device_handle *devh;
} mrb_cm6206_data;

static const struct mrb_data_type mrb_cm6206_data_type = {
  "mrb_cm6206_data", mrb_free,
};

static mrb_value mrb_cm6206_init(mrb_state *mrb, mrb_value self)
{
  mrb_cm6206_data *data;

  data = (mrb_cm6206_data *)DATA_PTR(self);
  if (data) {
    mrb_free(mrb, data);
  }
  DATA_TYPE(self) = &mrb_cm6206_data_type;
  DATA_PTR(self) = NULL;

  data = (mrb_cm6206_data *)mrb_malloc(mrb, sizeof(mrb_cm6206_data));

  DATA_PTR(self) = data;

  return self;
}

static mrb_value mrb_cm6206_open(mrb_state *mrb, mrb_value self)
{
  mrb_cm6206_data *data = DATA_PTR(self);

  data->ctx = NULL;
  libusb_init(&data->ctx);
  data->devh = cm_open_device(data->ctx);
  if (data->devh == -1) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "Can't open USB module.");
    return mrb_nil_value();
  }

  return mrb_fixnum_value(1);
}

static mrb_value mrb_cm6206_write(mrb_state *mrb, mrb_value self)
{
  mrb_cm6206_data *data = DATA_PTR(self);
  mrb_int reg, val;

  mrb_get_args(mrb, "ii", &reg, &val);

  cm6206_write(data->devh, (int)reg, (int)val);

  return mrb_fixnum_value(1);
}

static mrb_value mrb_cm6206_read(mrb_state *mrb, mrb_value self)
{
  mrb_cm6206_data *data = DATA_PTR(self);
  mrb_int reg;
  int val;

  mrb_get_args(mrb, "i", &reg);

  cm6206_read(data->devh, (int)reg, &val);

  return mrb_fixnum_value(val);
}

void mrb_mruby_cm6206_gem_init(mrb_state *mrb)
{
  struct RClass *cm6206;
  cm6206 = mrb_define_class(mrb, "CM6206", mrb->object_class);
  mrb_define_method(mrb, cm6206, "initialize", mrb_cm6206_init, MRB_ARGS_NONE());
  mrb_define_method(mrb, cm6206, "open", mrb_cm6206_open, MRB_ARGS_NONE());
  mrb_define_method(mrb, cm6206, "read", mrb_cm6206_read, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, cm6206, "write", mrb_cm6206_write, MRB_ARGS_REQ(2));
  DONE;
}

void mrb_mruby_cm6206_gem_final(mrb_state *mrb)
{
}

