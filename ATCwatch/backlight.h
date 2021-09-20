/*
 * Copyright (c) 2020 Aaron Christophel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include "Arduino.h"

void init_backlight();
void set_backlight();
void set_bright(uint8_t lowbright);
void disp_pwm(bool state);
int get_backlight();
void set_backlight(int brightness);
void inc_backlight();
void dec_backlight();
