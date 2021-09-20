/*
 * Copyright (c) 2020 Aaron Christophel
 * modifications (c) 2021 Andreas Loew
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "backlight.h"
#include "Arduino.h"
#include "pinout.h"

int backlight_brightness = 0;
int min_backlight_brightness = 1;
int max_backlight_brightness = 7;
uint8_t low_brightness = 224;
bool use_pwm = true;

void init_backlight() {
  pinMode(LCD_BACKLIGHT_LOW, OUTPUT);
  pinMode(LCD_BACKLIGHT_MID, OUTPUT);
  pinMode(LCD_BACKLIGHT_HIGH, OUTPUT);

  set_backlight(0);
}

void set_bright(uint8_t lowbright)
{
low_brightness = lowbright;   
  }

void disp_pwm(bool state)
{
use_pwm = state;   
  }

void set_backlight() {
  set_backlight(backlight_brightness);
}

int get_backlight() {
  return backlight_brightness;
}

void set_backlight(int brightness) {
  if ((brightness>0)&&(brightness<7)) backlight_brightness = brightness;
  switch (brightness) {
    case 0:
      if (use_pwm) analogWrite(LCD_BACKLIGHT_LOW, 255); else digitalWrite(LCD_BACKLIGHT_LOW, HIGH);
      digitalWrite(LCD_BACKLIGHT_MID, HIGH);
      digitalWrite(LCD_BACKLIGHT_HIGH, HIGH);
      break;
    case 1:
      if (use_pwm) analogWrite(LCD_BACKLIGHT_LOW, low_brightness); else digitalWrite(LCD_BACKLIGHT_LOW, LOW);
      digitalWrite(LCD_BACKLIGHT_MID, HIGH);
      digitalWrite(LCD_BACKLIGHT_HIGH, HIGH);
      break;
    case 2:
      if (use_pwm) analogWrite(LCD_BACKLIGHT_LOW, 255); else digitalWrite(LCD_BACKLIGHT_LOW, HIGH);
      digitalWrite(LCD_BACKLIGHT_MID, LOW);
      digitalWrite(LCD_BACKLIGHT_HIGH, HIGH);
      break;
    case 3:
      if (use_pwm) analogWrite(LCD_BACKLIGHT_LOW, 0); else digitalWrite(LCD_BACKLIGHT_LOW, LOW);
      digitalWrite(LCD_BACKLIGHT_MID, LOW);
      digitalWrite(LCD_BACKLIGHT_HIGH, HIGH);
      break;
    case 4:
      if (use_pwm) analogWrite(LCD_BACKLIGHT_LOW, 255); else digitalWrite(LCD_BACKLIGHT_LOW, HIGH);
      digitalWrite(LCD_BACKLIGHT_MID, HIGH);
      digitalWrite(LCD_BACKLIGHT_HIGH, LOW);
      break;
    case 5:
      if (use_pwm) analogWrite(LCD_BACKLIGHT_LOW, 0); else digitalWrite(LCD_BACKLIGHT_LOW, LOW);
      digitalWrite(LCD_BACKLIGHT_MID, HIGH);
      digitalWrite(LCD_BACKLIGHT_HIGH, LOW);
      break;
    case 6:
      if (use_pwm) analogWrite(LCD_BACKLIGHT_LOW, 255); else digitalWrite(LCD_BACKLIGHT_LOW, HIGH);
      digitalWrite(LCD_BACKLIGHT_MID, LOW);
      digitalWrite(LCD_BACKLIGHT_HIGH, LOW);
      break;
    case 7:
      if (use_pwm) analogWrite(LCD_BACKLIGHT_LOW, 0); else digitalWrite(LCD_BACKLIGHT_LOW, LOW);
      digitalWrite(LCD_BACKLIGHT_MID, LOW);
      digitalWrite(LCD_BACKLIGHT_HIGH, LOW);
      break;
  }
}

void inc_backlight() {
  backlight_brightness++;
  if (backlight_brightness > max_backlight_brightness)backlight_brightness = min_backlight_brightness;
}

void dec_backlight() {
  backlight_brightness--;
  if (backlight_brightness < min_backlight_brightness)backlight_brightness = max_backlight_brightness;
}
