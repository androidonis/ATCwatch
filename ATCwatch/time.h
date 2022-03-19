/*
 * Copyright (c) 2020 Aaron Christophel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "Arduino.h"

struct time_data_struct {
  int year;
  int month;
  int day;
  int hr;
  int min;
  int sec;
};

struct SunTime {
  int16_t sunrise; // in minutes 0...1440 of day
  int16_t sunset;  // in minutes 0...1440 of day
  int16_t Mangle;  // Max Angle
  int32_t moon;    // MoonPhase +00..+99/-99..0
  uint16_t dayofyear = dayofyear;
  uint8_t week;
  uint8_t wday;
  
};


void init_time();
time_data_struct get_time();
void SetDateTimeString(String datetime);
void SetDate(int year, int month, int day);
void SetTime(int hr, int min);
String GetDateTimeString();
void setbuttontime(uint16_t timebetwpress);
uint16_t secbutton();
SunTime get_suntime();
int32_t sumsin( uint16_t angle);
void Setcoordinates(int longitu, int latitu, int timezn);
