/*
 * Copyright (c) 2020 Aaron Christophel
 * 211004 changes by Andreas Loew
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once
#include "Arduino.h"
#include "class.h"
#include "images.h"
#include "menu.h"
#include "inputoutput.h"
#include "display.h"
#include "ble.h"
#include "time.h"
#include "battery.h"
#include "accl.h"
#include "push.h"
#include "heartrate.h"
#include "fonts.h"
#include "sleep.h"
#include <lvgl.h>


class HomeScreen : public Screen
{
  public:
    virtual void pre()
    {
      time_data = get_time();
      accl_data = get_accl_data();
      angle = getangle();
    uint8_t wday = (6+time_data.day+((time_data.year-2000-(time_data.month<3))/4)+"beehcfhdgbeg"[time_data.month-1]+(time_data.year-2000))%7; 

      lv_style_copy( &msml, &lv_style_plain );
      msml.text.color = LV_COLOR_WHITE;
      msml.text.font = &sans_regular;



      label_ble = lv_label_create(lv_scr_act(), NULL);
      lv_obj_align(label_ble, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 5, 0);
      lv_label_set_text(label_ble, LV_SYMBOL_BLUETOOTH);

      label_battery = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_battery, &msml );
      lv_obj_align(label_battery, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 25, 0);
      lv_label_set_text_fmt(label_battery, "%i%%", get_battery_percent());

      label_buttime = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_buttime, &msml );
      lv_obj_align(label_buttime, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 75, 0);
      lv_label_set_text_fmt(label_buttime, "%i,%is",secbutton()/10,secbutton()%10);

      label_temp = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_temp, &msml );
      lv_obj_align(label_temp, NULL, LV_ALIGN_IN_TOP_LEFT, 185, 0);
      lv_label_set_text_fmt(label_temp, "%2.1f°C", (float)accl_data.temp);

      label_x = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_x, &msml );
      lv_label_set_text(label_x, "X:");
      lv_obj_align(label_x, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 20);
      label_y = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_y, &msml );
      lv_label_set_text(label_y, "Y:");
      lv_obj_align(label_y, NULL, LV_ALIGN_IN_TOP_LEFT, 90, 20);
      label_z = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_z, &msml );
      lv_label_set_text(label_z, "Z:");
      lv_obj_align(label_z, NULL, LV_ALIGN_IN_TOP_LEFT, 185, 20);

      label_sunr = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_sunr, &msml );
      lv_obj_align(label_sunr, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 38);
      lv_label_set_text_fmt(label_sunr, "Moon %i", get_suntime().moon );

      label_cdsun = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_cdsun, &msml );
      lv_obj_align(label_cdsun, NULL, LV_ALIGN_IN_TOP_LEFT,200, 38);
      lv_label_set_text_fmt(label_cdsun, "%i", 9999);


      label_sunrise = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_sunrise, &msml );
      lv_obj_align(label_sunrise, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 60);
      lv_label_set_text_fmt(label_sunrise, "%i:%002i", get_suntime().sunrise/60,get_suntime().sunrise%60 );

      label_sunset = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_sunset, &msml );
      lv_obj_align(label_sunset, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 97);
      lv_label_set_text_fmt(label_sunset, "%i:%002i", get_suntime().sunset/60,get_suntime().sunset%60 );

      label_sunangle = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_sunangle, &msml );
      lv_obj_align(label_sunangle, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 78);
      lv_label_set_text_fmt(label_sunangle, "%i°", get_suntime().Mangle/100);

      label_doy = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_doy, &msml );
      lv_obj_align(label_doy, NULL, LV_ALIGN_IN_TOP_LEFT, 48, 122);
      lv_label_set_text_fmt(label_doy, "%i.", get_suntime().dayofyear);

      label_kweek = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_kweek, &msml );
      lv_obj_align(label_kweek, NULL, LV_ALIGN_IN_TOP_LEFT, 55, 153);
      lv_label_set_text_fmt(label_kweek, "%i", get_suntime().week);




      lv_style_copy(&style_ble, lv_label_get_style(label_ble, LV_LABEL_STYLE_MAIN));
      style_ble.text.color = LV_COLOR_RED;
      style_ble.text.font = LV_FONT_DEFAULT;
      lv_obj_set_style(label_ble, &style_ble);

      lv_style_copy(&style_battery, lv_label_get_style(label_battery, LV_LABEL_STYLE_MAIN));
      style_battery.text.color = LV_COLOR_WHITE; //lv_color_hsv_to_rgb(10, 5, 95);
      lv_obj_set_style(label_battery, &style_battery);

      lv_style_copy( &stx, &lv_style_plain );
      stx.text.color = LV_COLOR_ORANGE; //lv_color_hsv_to_rgb(10, 5, 95);
      label_sun = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_sun, LV_SYMBOL_SETTINGS);
      lv_obj_set_style( label_sun, &stx );
      lv_obj_align(label_sun, NULL, LV_ALIGN_IN_TOP_LEFT, 5, 78);//LV_ALIGN_CENTER, -40, -30);
      
      
      lv_style_copy( &stt, &lv_style_plain );
      stt.text.color = LV_COLOR_ORANGE; //lv_color_hsv_to_rgb(10, 5, 95);
      stt.text.font = &mksd50;
      label_time = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text_fmt(label_time,  "%02i:%02i:", time_data.hr, time_data.min);
      lv_obj_set_style( label_time, &stt );
      lv_obj_align(label_time, NULL, LV_ALIGN_CENTER, -10, -30);
 
      lv_style_copy( &sts, &lv_style_plain );
      sts.text.color = LV_COLOR_ORANGE; //lv_color_hsv_to_rgb(10, 5, 95);
      sts.text.font = &mksd50;
      label_sec = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text_fmt(label_sec,  "%02i", time_data.sec);
      lv_obj_set_style( label_sec, &sts );
      lv_obj_align(label_sec, NULL, LV_ALIGN_CENTER, 80, -30);

     lv_style_copy( &stwd, &lv_style_plain );
      stwd.text.color = LV_COLOR_RED;
      stwd.text.font = &mksd50;
      label_wday = lv_label_create(lv_scr_act(), NULL);
     lv_label_set_text_fmt(label_wday, "%c%c", "SMDMDFS"[0], "ooiiora"[0]);
     lv_obj_set_style( label_wday, &stwd );
      lv_obj_align(label_wday, NULL, LV_ALIGN_CENTER, -98, 30);
      
      lv_style_copy( &std, &lv_style_plain );
      std.text.color = LV_COLOR_CYAN; //lv_color_hsv_to_rgb(10, 5, 95);
      std.text.font = &mksd50;
      label_date = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text_fmt(label_date, "%02i.%02i.%02i", time_data.day, time_data.month, (time_data.year-2000));
      lv_obj_set_style( label_date, &std );
      lv_obj_align(label_date, NULL, LV_ALIGN_CENTER, 40, 30);





      img_heart = lv_img_create(lv_scr_act(), NULL);
      lv_img_set_src(img_heart, &IsymbolHeartSmall);
      lv_obj_align(img_heart, NULL, LV_ALIGN_IN_BOTTOM_LEFT,  0, -34);

      lv_style_copy( &shr, &lv_style_plain );
      shr.text.color = LV_COLOR_WHITE;
      shr.text.font = &sans_regular;

      label_heart = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_width(label_heart, 240);
      lv_label_set_text_fmt(label_heart, "%i", get_last_heartrate());      
      lv_obj_set_style( label_heart, &shr );
      lv_obj_align(label_heart, img_heart, LV_ALIGN_OUT_RIGHT_MID, 2, 0);


      img_steps = lv_img_create(lv_scr_act(), NULL);
      lv_img_set_src(img_steps, &IsymbolStepsSmall);
      lv_obj_align(img_steps, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 170, -34);

      label_steps = lv_label_create(lv_scr_act(), NULL);
      lv_obj_set_style( label_steps, &msml );
      lv_obj_set_width(label_steps, 240);
      lv_label_set_text_fmt(label_steps, "%i", accl_data.steps);
      lv_obj_align(label_steps, img_steps, LV_ALIGN_OUT_RIGHT_MID, 2, 0);


      img_msg = lv_img_create(lv_scr_act(), NULL);
      lv_img_set_src(img_msg, &IsymbolMsgSmall);
      lv_obj_align(img_msg, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, -8);

      label_msg = lv_label_create(lv_scr_act(), NULL);

      lv_style_copy(&style_msg, lv_label_get_style(label_ble, LV_LABEL_STYLE_MAIN));
      style_msg.text.color = lv_color_hsv_to_rgb(10, 5, 95);
      style_msg.text.font = &sans_regular;
      lv_obj_set_style(label_msg, &style_msg);

      lv_obj_set_width(label_msg, 240);
      lv_label_set_text(label_msg, " ");
      lv_label_set_text(label_msg, string2char(get_push_msg(30)));
      lv_obj_align(label_msg, img_msg, LV_ALIGN_OUT_RIGHT_MID, 2, 0);
    }

    virtual void main()
    {
      time_data = get_time();
      accl_data = get_accl_data();
      angle = getangle();
      uint8_t cfd[48] = {32,64,96,128,160,192,224,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,224,192,160,128,96,64,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
      uint8_t wday = (5+time_data.day+((time_data.year-2000-(time_data.month<3))/4)+"beehcfhdgbeg"[time_data.month-1]+(time_data.year-2000))%7; 
      int16_t cdsunva, cdsunva2;
      cdsunva = get_suntime().sunrise +((time_data.hr>13)*1440) -(time_data.min+(time_data.hr*60));
      cdsunva2 = get_suntime().sunset -(time_data.min+(time_data.hr*60));
      if ((cdsunva2<cdsunva)&&(cdsunva2>0))cdsunva = cdsunva2;
      stx.text.color = LV_COLOR_MAKE(cfd[((time_data.hr*5/3)+32)%48], cfd[(time_data.hr*5/3)], cfd[((time_data.hr*5/3)+16)%48]); //
//      if (time_data.sec%2==0) lv_label_set_text_fmt(label_sun, "@"); else lv_label_set_text_fmt(label_sun, ">");
      if (time_data.sec%2==0) lv_label_set_text(label_sun, LV_SYMBOL_SETTINGS); else lv_label_set_text(label_sun, "");
      stt.text.color = LV_COLOR_WHITE;
      lv_label_set_text_fmt(label_time,  "%02i:%02i:", time_data.hr, time_data.min);
      sts.text.color = LV_COLOR_MAKE(cfd[(((time_data.sec*2)/3)+32)%48], cfd[((time_data.sec*2)/3)], cfd[(((time_data.sec*2)/3)+16)%48]); //
      lv_label_set_text_fmt(label_sec,  "%02i", time_data.sec);
      stwd.text.color = LV_COLOR_MAKE(cfd[(wday*6+32)%48], cfd[wday*6], cfd[(wday*6+16)%48]); //lv_color_hsv_to_rgb(wday*8000, 255, 255);
     lv_label_set_text_fmt(label_wday,  "%c%c","MDMDFSS"[wday], "oiiorao"[wday]);
      std.text.color = LV_COLOR_MAKE(cfd[47-((time_data.month-1)*3+time_data.day/10)], cfd[((time_data.month-1)*3+time_data.day/10)], cfd[(((time_data.month-1)*3+time_data.day/10)+16)%48]); //lv_color_hsv_to_rgb((time_data.month*31+time_data.day)*150, 255, 255);
      lv_label_set_text_fmt(label_date, "%02i.%02i.%02i", time_data.day, time_data.month, (time_data.year-2000));
      //display accl data, add +3/+28/+520 to see 0 0 0 on a flat table.
      lv_label_set_text_fmt(label_x, "X:%+003i°", angle.x);
      lv_label_set_text_fmt(label_y, "Y:%+003i°", angle.y);
      lv_label_set_text_fmt(label_z, "Z:%+003i°", angle.z);
      lv_label_set_text_fmt(label_temp, "%2.1f°C", (float)accl_data.temp);
      if ((secbutton()>125)&&(secbutton()<6000))lv_label_set_text_fmt(label_buttime, "%i,%is ,%iW",secbutton()/10,secbutton()%10,125000/(secbutton()+1));
      else                                      lv_label_set_text_fmt(label_buttime, "    %i,%is",secbutton()/10,secbutton()%10);
lv_label_set_text_fmt(label_sunr, "Moon %i", get_suntime().moon );
lv_label_set_text_fmt(label_sunrise, "%  2i:%002i", get_suntime().sunrise/60,get_suntime().sunrise%60 );
lv_label_set_text_fmt(label_sunset, "%i:%002i", get_suntime().sunset/60,get_suntime().sunset%60 );
lv_label_set_text_fmt(label_sunangle, "%i°", get_suntime().Mangle/100);
      lv_label_set_text_fmt(label_doy, "%   i", get_suntime().dayofyear);
      lv_label_set_text_fmt(label_kweek, "%  i", get_suntime().week);
      lv_label_set_text_fmt(label_cdsun, "%    i", cdsunva);


      lv_label_set_text_fmt(label_battery, "%i%%", get_battery_percent());
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
      lv_label_set_text_fmt(label_heart, "%i(%i-%i-%i)@%i", get_hearthistory(0),hrmi,(hrav+get_hearthistory(0))/(hrcnt),hrma,get_hearthistory(600));
      lv_label_set_text_fmt(label_steps, "%i", accl_data.steps);

      if (get_vars_ble_connected())
        style_ble.text.color = LV_COLOR_MAKE(0x27, 0xA6, 0xFF);
      else
        style_ble.text.color = LV_COLOR_RED;
      lv_obj_set_style(label_ble, &style_ble);

      if (get_charge())
        style_battery.text.color = lv_color_hsv_to_rgb(10, 5, 95);
      else
        style_battery.text.color = LV_COLOR_MAKE(0x05, 0xF9, 0x25);
      lv_obj_set_style(label_battery, &style_battery);

    }

    virtual void up()
    {
      inc_vars_menu();
    }

    virtual void down()
    {
      dec_vars_menu();
    }

    virtual void left()
    {
    }

    virtual void right()
    {
    }

    virtual void button_push(int length)
    {
      sleep_down();
    }

  private:
    time_data_struct time_data;
    accl_data_struct accl_data;
    angle_data angle;
    lv_style_t stt,std,stwd,sts,stx,shr,msml;
    lv_obj_t *label, *label_heart, *label_steps, *label_msg;
    lv_obj_t *label_sun,*label_time,*label_sec, *label_date, *label_wday;
    lv_obj_t *label_temp,*label_x, *label_y, *label_z, *label_sunr, *label_sunrise, *label_sunset, *label_sunangle, *label_doy;
    lv_obj_t *label_ble, *label_battery, *label_buttime, *label_kweek, *label_cdsun;
    lv_style_t style_ble, style_battery, style_msg, style_sun;
    lv_obj_t * img_heart, *img_steps, *img_msg;

    char* string2char(String command) {
      if (command.length() != 0) {
        char *p = const_cast<char*>(command.c_str());
        return p;
      }
    }
};

HomeScreen homeScreen;
