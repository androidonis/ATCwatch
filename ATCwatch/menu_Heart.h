/*
 * Copyright (c) 2020 Aaron Christophel
 * modifications (c) 2021 Andreas Loew
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
      lv_label_set_text(label, "Heartrate:");
      lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 0);

      set_gray_screen_style(&lv_font_roboto_28);
      lv_obj_t * img1 = lv_img_create(lv_scr_act(), NULL);
      lv_img_set_src(img1, &IsymbolHeart);
      lv_obj_align(img1, NULL, LV_ALIGN_IN_TOP_LEFT, 169, 0);
            
      label_hr = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hr, "now");
      lv_obj_align(label_hr, NULL, LV_ALIGN_IN_TOP_LEFT, 180, 18);

      label_hrmi1 = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hrmi1, "min");
      lv_obj_align(label_hrmi1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 80);

      label_hrma1 = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hrma1, "max");
      lv_obj_align(label_hrma1, NULL, LV_ALIGN_IN_TOP_LEFT, 175, 80);

      label_hrav1 = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hrav1, "avg:");
      lv_obj_align(label_hrav1, NULL, LV_ALIGN_IN_TOP_LEFT, 88, 80);

      label_hrmi2 = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hrmi2, "min");
      lv_obj_align(label_hrmi2, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 120);

      label_hrma2 = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hrma2, "max");
      lv_obj_align(label_hrma2, NULL, LV_ALIGN_IN_TOP_LEFT, 175, 120);

      label_hrav2 = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hrav2, "avg:");
      lv_obj_align(label_hrav2, NULL, LV_ALIGN_IN_TOP_LEFT, 88, 120);

      label_hrmi3 = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hrmi3, "min");
      lv_obj_align(label_hrmi3, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 160);

      label_hrma3 = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hrma3, "max");
      lv_obj_align(label_hrma3, NULL, LV_ALIGN_IN_TOP_LEFT, 175, 160);

      label_hrav3 = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_hrav3, "avg:");
      lv_obj_align(label_hrav3, NULL, LV_ALIGN_IN_TOP_LEFT, 88, 160);



      slider = lv_slider_create(lv_scr_act(), NULL);
      lv_slider_set_range(slider, 1, 20);
      lv_obj_set_event_cb(slider, lv_event_handler);
      lv_obj_align(slider, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 200);
      lv_slider_set_value(slider, get_hrint(), LV_ANIM_OFF);

    
      slider_label = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text_fmt(slider_label, "%u", lv_slider_get_value(slider));
      lv_obj_set_auto_realign(slider_label, true);
      lv_obj_align(slider_label, NULL, LV_ALIGN_IN_TOP_LEFT, 200, 200);

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
      uint32_t hrav = 0;
      int16_t hrcnt=1;
      for (int lpct = 1; lpct < 60; lpct++) {
        if  (get_hearthistory(lpct)>0) {
          hrav+=get_hearthistory(lpct); hrcnt++; 
          if (get_hearthistory(lpct)>hrma) hrma=get_hearthistory(lpct); 
          if (get_hearthistory(lpct)<hrmi) hrmi=get_hearthistory(lpct);   
          }
      }
      lv_label_set_text_fmt(label_hrmi1, "%i", hrmi);
      lv_label_set_text_fmt(label_hrma1, "%i", hrma);
      lv_label_set_text_fmt(label_hrav1, "%i", (hrav+get_hearthistory(0))/hrcnt);
      hrma = 1;
      hrmi = 250;
      hrav = 0;
      hrcnt=1;
      for (int lpct = 61; lpct < 120; lpct++) {
        if  (get_hearthistory(lpct)>0) {
          hrav+=get_hearthistory(lpct); hrcnt++; 
          if (get_hearthistory(lpct)>hrma) hrma=get_hearthistory(lpct); 
          if (get_hearthistory(lpct)<hrmi) hrmi=get_hearthistory(lpct);   
          }
      }
      lv_label_set_text_fmt(label_hrmi2, "%i", hrmi);
      lv_label_set_text_fmt(label_hrma2, "%i", hrma);
      lv_label_set_text_fmt(label_hrav2, "%i", (hrav+get_hearthistory(60))/hrcnt);
      hrma = 1;
      hrmi = 250;
      hrav = 0;
      hrcnt=1;
      for (int lpct = 1; lpct < 500; lpct++) {
        if  (get_hearthistory(lpct)>0) {
          hrav+=get_hearthistory(lpct); hrcnt++; 
          if (get_hearthistory(lpct)>hrma) hrma=get_hearthistory(lpct); 
          if (get_hearthistory(lpct)<hrmi) hrmi=get_hearthistory(lpct);   
          }
      }
      lv_label_set_text_fmt(label_hrmi3, "%i", hrmi);
      lv_label_set_text_fmt(label_hrma3, "%i", hrma);
      lv_label_set_text_fmt(label_hrav3, "%i", (hrav+get_hearthistory(0))/hrcnt);
    }

   virtual void lv_event_class(lv_obj_t * object, lv_event_t event)
    {
      if (object == slider && event == LV_EVENT_VALUE_CHANGED) {
        int slider_hrint = lv_slider_get_value(slider);
        lv_label_set_text_fmt(slider_label, "%u", slider_hrint);
        sethrevery(slider_hrint);
      }
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
    lv_obj_t *label, *label_hr, *label_hrmi1, *label_hrma1, *label_hrav1, *label_hrmi2, *label_hrma2, *label_hrav2, *label_hrmi3, *label_hrma3, *label_hrav3, *label_hr_last, *slider, *slider_label;

};

HeartScreen heartScreen;
