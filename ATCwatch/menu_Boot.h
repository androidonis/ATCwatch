/*
 * Version Info: edit time.cpp for time init value to reflect version date and time!
 * Copyright (c) 2020 Aaron Christophel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once
#include "Arduino.h"
#include "class.h"
#include "images.h"
#include "menu.h"
#include "display.h"
#include "ble.h"
#include "time.h"
#include "battery.h"
#include "accl.h"
#include "push.h"
#include "heartrate.h"
#include "screen_style.h"
#include <lvgl.h>


int where = 0;

class BootScreen : public Screen
{
  public:
    virtual void pre()
    {
      set_gray_screen_style(&lv_font_roboto_28);

      label = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label, "Booting\n""AL-Watch");
      lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
      lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -60);

      lv_obj_t * img1 = lv_img_create(lv_scr_act(), NULL);
      lv_img_set_src(img1, &Iimage2);
      lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 0, 20);
    }

    virtual void main()
    {
      lv_label_set_text_fmt(label, "Booting %d\n AL-Watch",where);
    }

    void setWhere(int w) {
      where = w;
    }

  private:

    lv_obj_t *label;
};

BootScreen bootScreen;
