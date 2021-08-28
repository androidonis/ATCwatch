/*
 * Copyright (c) 2020 Aaron Christophel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "backlight.h"
#include "Arduino.h"
#include "pinout.h"

int backlight_brightness = 0;
int min_backlight_brightness = 1;
int max_backlight_brightness = 7;
uint8_t low_brightness = 128;
uint8_t mid_brightness = 0;
uint8_t high_brightness = 0;

void init_backlight() {
  pinMode(LCD_BACKLIGHT_LOW, OUTPUT);
  pinMode(LCD_BACKLIGHT_MID, OUTPUT);
  pinMode(LCD_BACKLIGHT_HIGH, OUTPUT);

  set_backlight(0);
}

void set_bright(uint8_t lowbright, uint8_t midbright, uint8_t highbright)
{
low_brightness = lowbright;
mid_brightness = midbright;
high_brightness = highbright;   
  }

void set_backlight() {
  set_backlight(backlight_brightness);
}

int get_backlight() {
  return backlight_brightness;
}

void set_backlight(int brightness) {
  if (brightness)backlight_brightness = brightness;
  switch (brightness) {
    case 0:
      analogWrite(LCD_BACKLIGHT_LOW, 255);
      analogWrite(LCD_BACKLIGHT_MID, 255);
      analogWrite(LCD_BACKLIGHT_HIGH, 255);
      break;
    case 1:
      analogWrite(LCD_BACKLIGHT_LOW, low_brightness);
      analogWrite(LCD_BACKLIGHT_MID, 255);
      analogWrite(LCD_BACKLIGHT_HIGH, 255);
      break;
    case 2:
      analogWrite(LCD_BACKLIGHT_LOW, 255);
      analogWrite(LCD_BACKLIGHT_MID, mid_brightness);
      analogWrite(LCD_BACKLIGHT_HIGH, 255);
      break;
    case 3:
      analogWrite(LCD_BACKLIGHT_LOW, low_brightness);
      analogWrite(LCD_BACKLIGHT_MID, mid_brightness);
      analogWrite(LCD_BACKLIGHT_HIGH, 255);
      break;
    case 4:
      analogWrite(LCD_BACKLIGHT_LOW, 255);
      analogWrite(LCD_BACKLIGHT_MID, 255);
      analogWrite(LCD_BACKLIGHT_HIGH, high_brightness);
      break;
    case 5:
      analogWrite(LCD_BACKLIGHT_LOW, low_brightness);
      analogWrite(LCD_BACKLIGHT_MID, 255);
      analogWrite(LCD_BACKLIGHT_HIGH, high_brightness);
      break;
    case 6:
      analogWrite(LCD_BACKLIGHT_LOW, 255);
      analogWrite(LCD_BACKLIGHT_MID, mid_brightness);
      analogWrite(LCD_BACKLIGHT_HIGH,high_brightness);
      break;
    case 7:
      analogWrite(LCD_BACKLIGHT_LOW, low_brightness);
      analogWrite(LCD_BACKLIGHT_MID, mid_brightness);
      analogWrite(LCD_BACKLIGHT_HIGH, high_brightness);
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
