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
#include "heartrate.h"


class ChargingScreen : public Screen
{
  public:
    virtual void pre()
    {
      time_data = get_time();

      lv_style_copy( &std, &lv_style_plain );
      std.text.color = LV_COLOR_ORANGE; //lv_color_hsv_to_rgb(10, 5, 95);
      std.text.font = &mksd50;
      label_date = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text_fmt(label_date, "%02i.%02i.%02i", time_data.day, time_data.month, (time_data.year-2000));
      lv_obj_set_style( label_date, &std );
      lv_obj_align(label_date, NULL, LV_ALIGN_IN_TOP_LEFT, 40, 5);

      
      label = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label, "Charging");
      lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -30);

      lv_style_copy( &st, &lv_style_plain );
      st.text.color = lv_color_hsv_to_rgb(10, 5, 95);
      st.text.font = &mksd50;

      label_battery_big = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_battery_big, &st );
      lv_label_set_text(label_battery_big, "---%");
      lv_obj_align(label_battery_big, lv_scr_act(), LV_ALIGN_CENTER, -20, 20);


      lv_style_copy( &stt, &lv_style_plain );
      stt.text.color = LV_COLOR_ORANGE; //lv_color_hsv_to_rgb(10, 5, 95);
      stt.text.font = &mksd50;
      label_time = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text_fmt(label_time,  "%02i:%02i:%02i", time_data.hr, time_data.min, time_data.sec);
      lv_obj_set_style( label_time, &stt );
      lv_obj_align(label_time, NULL, LV_ALIGN_CENTER, 0, 80);
 

    }

    virtual void main()
    {
      time_data = get_time();
      if (get_charge()) st.text.color = LV_COLOR_ORANGE; else st.text.color = LV_COLOR_CYAN;
      lv_label_set_text_fmt(label_battery_big, "%i%%", get_battery_percent());
      stt.text.color = LV_COLOR_WHITE;
      lv_label_set_text_fmt(label_time,  "%02i:%02i:%02i", time_data.hr, time_data.min, time_data.sec);
      lv_label_set_text_fmt(label_date, "%02i.%02i.%02i", time_data.day, time_data.month, (time_data.year-2000));

    }

    virtual void long_click()
    {
      display_home();
    }

    virtual void left()
    {
      display_home();
    }

    virtual void right()
    {
      display_home();
    }

    virtual void up()
    {
      display_home();
    }
    virtual void down()
    {
      display_home();
    }

    virtual uint32_t sleepTime()
    {
      return 600000; // see charging progress for 10 minutes
    }


    virtual void click(touch_data_struct touch_data)
    {
      display_home();
    }
    
  private:
    time_data_struct time_data;
    lv_obj_t *label;
    lv_obj_t *label_battery, *label_battery_big, *label_backlight_big, *label_time, *label_date;
    lv_style_t style_battery, st, stt, std;
};

ChargingScreen chargingScreen;
