/*
 * Copyright (c) 2020 Aaron Christophel
 * modifications (c) 2021 Andreas Loew
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "heartrate.h"
#include "pinout.h"
#include "i2c.h"
#include "inputoutput.h"
#include "sleep.h"
#include "HRS3300lib.h"

bool heartrate_enable = false;
bool is_heartrate_enable = false;
byte last_heartrate_ms;
byte last_heartrate;

//timed heartrate stuff
bool timed_heart_rates = true;
bool has_good_heartrate = false;
int hr_answers;
bool disabled_hr_allready = false;
uint8_t heartrates[601] = {0}; //history of rates
int nextminute = 99;
int hrevery = 5; 
byte hr;

void init_hrs3300() {
  pinMode(HRS3300_TEST, INPUT);
  HRS3300_begin(user_i2c_read, user_i2c_write);//set the i2c read and write function so it can be a user defined i2c hardware see i2c.h
  heartrate_enable = true;
  end_hrs3300();
}

void start_hrs3300() {
  if (!heartrate_enable) {
    HRS3300_enable();
    heartrate_enable = true;
  }
}

void end_hrs3300() {
  if (heartrate_enable) {
    heartrate_enable = false;
    HRS3300_disable();
  }
}

byte get_heartrate() {
  byte hr = last_heartrate_ms;
  switch (hr) {
    case 0:
      break;
    case 255:
      break;
    case 254://No Touch
      break;
    case 253://Please wait
      break;
    default:
      last_heartrate = hr;
      break;
  }
  return hr;
}

byte get_last_heartrate() {
  return last_heartrate;
}

void get_heartrate_ms() {
  if (heartrate_enable) {
    last_heartrate_ms = HRS3300_getHR();
  }
}
byte get_hearthistory(uint16_t history) {
  return heartrates[history];
}

void sethrevery(int interv){
  hrevery = interv;
  }

int get_hrint(){
  return hrevery;
  }

void check_timed_heartrate(int minutes) {
  if (timed_heart_rates) {
    if (minutes == nextminute || nextminute == 99 ) {
      if (!has_good_heartrate) {
        disabled_hr_allready = false;
        start_hrs3300();
        hr = get_heartrate();
        if (hr > 0 && hr < 253) {
          hr_answers++;
          if (hr_answers >= 5) {
            has_good_heartrate = true;
          }
        } else if (hr == 254) {
          hr_answers++;
          if (hr_answers >= 10) {
            has_good_heartrate = true;
          }
        }
      } else {
        end_hrs3300();
        nextminute = minutes + hrevery; if (nextminute > 59) nextminute = nextminute - 60;
        for (int16_t lp=599; lp>0; lp--) heartrates[lp] = heartrates[lp-1]; //shift up history
        heartrates[0]=hr; // just take last measurement, maybe median filter the 5 measurements.
        heartrates[600]=nextminute;
      }
    } else {
      if (!disabled_hr_allready) {
        disabled_hr_allready = true;
        end_hrs3300();
        hr_answers = 0;
        has_good_heartrate = false;
      }
    }
  }
}
