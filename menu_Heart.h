/*
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
#include "inputoutput.h"
#include "heartrate.h"


class HeartScreen : public Screen
{
  public:
    virtual void pre()
    {
      start_hrs3300();

      label = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label, "Heartrate");
      lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 60, 0);

      set_gray_screen_style(&lv_font_roboto_28);
      
      label_hr = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hr, "now");
      lv_obj_align(label_hr, NULL, LV_ALIGN_IN_TOP_LEFT, 88, 40);

      label_hrmi = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hrmi, "min");
      lv_obj_align(label_hrmi, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 40);

      label_hrma = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hrma, "max");
      lv_obj_align(label_hrma, NULL, LV_ALIGN_IN_TOP_LEFT, 175, 40);

      label_hr_last = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hr_last, "Last:");
      lv_obj_align(label_hr_last, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 80);

      label_hrav = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hrav, "Avg:");
      lv_obj_align(label_hrav, NULL, LV_ALIGN_IN_TOP_LEFT, 120, 80);

      lv_obj_t * img1 = lv_img_create(lv_scr_act(), NULL);
      lv_img_set_src(img1, &IsymbolHeart);
      lv_obj_align(img1, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    }

    virtual void main()
    {
      byte hr = get_heartrate();
      switch (hr) {
        case 0:
        case 255:
          break;
        case 254:
          lv_label_set_text_fmt(label_hr, "NoT");
          break;
        case 253:
          lv_label_set_text_fmt(label_hr, "Wait");
          break;
        default:
          lv_label_set_text_fmt(label_hr, "%i", hr);
          break;
      }
      byte hrma = 1;
      byte hrmi = 250;
      byte hrav = 0;
      int16_t hrcnt=0;
      for (int lpct = 0; lpct < 500; lpct++) {
        if  (get_hearthistory(lpct)>0) {
          hrav+=get_hearthistory(lpct); hrcnt++; 
          if (get_hearthistory(lpct)>hrma) hrma=get_hearthistory(lpct); 
          if (get_hearthistory(lpct)<hrmi) hrmi=get_hearthistory(lpct);   
          }
      }
      lv_label_set_text_fmt(label_hrmi, "%i", hrmi);
      lv_label_set_text_fmt(label_hrma, "%i", hrma);
      lv_label_set_text_fmt(label_hrav, "Avg:%i", (hrav+(hrcnt/2))/hrcnt);
      lv_label_set_text_fmt(label_hr_last, "Last:%i", get_last_heartrate());
    }

    virtual void post()
    {
      end_hrs3300();
    }

    virtual uint32_t sleepTime()
    {
      return 50000;
    }

    virtual void right()
    {
      set_last_menu();
    }

  private:
    lv_obj_t *label, *label_hr, *label_hrmi, *label_hrma, *label_hrav, *label_hr_last;

};

HeartScreen heartScreen;
