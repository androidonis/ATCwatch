/*
 * Copyright (c) 2020 Aaron Christophel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <Arduino.h>
#include "nrf.h"
#include "Stream.h"
#include "RingBuffer.h"

void init_i2c();
bool blkint();

//void set_i2cReading(bool state);
bool get_i2cReading();
uint32_t scan_i2c();
uint32_t rdid_i2c();
void usr_i2c_wreg(uint8_t daddr, uint8_t waddr, uint8_t data);
uint8_t usr_i2c_rreg(uint8_t daddr, uint8_t raddr);

uint8_t user_i2c_read(uint8_t addr, uint8_t reg_addr, uint8_t *reg_data, uint32_t length);
uint8_t user_i2c_write(uint8_t addr, uint8_t reg_addr, const uint8_t *reg_data, uint32_t length);

void Wire_begin();
void Wire_beginTransmission(uint8_t);
uint8_t Wire_endTransmission(bool stopBit);
uint8_t Wire_requestFrom(uint8_t address, size_t quantity, bool stopBit);
uint8_t Wire_requestFrom(uint8_t address, size_t quantity);
void Wire_write(uint8_t data);
int Wire_read(void);
