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
#include "i2c.h"

#define versionString "2.0.1"

class InfosScreen : public Screen
{
  public:
    virtual void pre()
    {
      char version_char[10];
      sprintf(version_char, versionString);
      char mac_char[16];
      sprintf(mac_char, "%04X%08X", (NRF_FICR->DEVICEADDR[1] & 0xffff), NRF_FICR->DEVICEADDR[0]);

      label = lv_label_create(lv_scr_act(), NULL);
      
      lv_label_set_text(label,
                        "Infos:\n"
                        "FW by: ATCnetz.de\n"
                        "    (Aaron Christophel)\n"
                        "  Compiled:\n"
                        "  "__DATE__"\n"
                        "  "__TIME__
                       );
                       
      lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

      label_version = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_version,"  Version:");
      lv_label_set_text_fmt(label_version, "  Version: %s",version_char);
      lv_obj_align(label_version, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

      label_mac = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_mac,"  MAC:");
      lv_label_set_text_fmt(label_mac, "  MAC: %s",mac_char);
      lv_obj_align(label_mac, label_version, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
      
      uint32_t i2c_cnf = scan_i2c();
      uint32_t i2c_id = rdid_i2c();
      
      label_i2c = lv_label_create(lv_scr_act(), NULL);
      lv_label_set_text(label_i2c,"  I2C:");
      lv_label_set_text_fmt(label_i2c, "  I2C A:%x Id:%x",i2c_cnf,i2c_id);
      lv_obj_align(label_i2c, label_mac, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    }

    virtual void main()
    {
//    uint32_t i2c_cnf = scan_i2c();  
//    lv_label_set_text_fmt(label_i2c, "  I2C: %x",i2c_cnf);
    }

    virtual void right()
    {
      set_last_menu();
    }

  private:
    lv_obj_t *label, *label_version, *label_mac, *label_i2c;
};

InfosScreen infosScreen;
