/*
 * Copyright (c) 2020 Aaron Christophel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <Arduino.h>
#include "pinout.h"
#include "i2c.h"

volatile bool i2cReading = false;
bool blockint = false;

void blkint(){
  if (blockint) blockint=false; else blockint = true;
  }

void init_i2c() {

  pinMode(HRS3300_SCL, OUTPUT);
  for(uint16_t lpct=0; lpct<500; lpct++) {digitalWrite(HRS3300_SCL, LOW); delay(1); digitalWrite(HRS3300_SCL, HIGH); delay(1);}  
  Wire_begin(); 
}

void set_i2cReading(bool state) {
  i2cReading = state;
}

bool get_i2cReading() {
  return i2cReading;
}

uint32_t scan_i2c() {
      uint32_t i2c_cnf = 0; uint32_t device = 0x1;
      for( byte addr = 1; addr < 127; addr++ )
  {
    // The i2c_scanner uses the return value of the Write.endTransmisstion to see if a device did acknowledge to the address.
    Wire_beginTransmission(addr);
    byte error = Wire_endTransmission(true); 
    if (error == 0)
    {
      if (device < 0x1000000){ i2c_cnf = i2c_cnf + addr*device; device = device*0x100; }
    }
  
  }
  return i2c_cnf;
}

uint32_t rdid_i2c() {
     uint32_t i2c_id = 0; uint32_t device = 0x1;
      for( byte addr = 1; addr < 127; addr++ )
  {
    // The i2c_scanner uses the return value of the Write.endTransmisstion to see if a device did acknowledge to the address.
    Wire_beginTransmission(addr);
    byte error = Wire_endTransmission(true); 
    if (error == 0)
    {
     Wire_requestFrom(addr, 1,true); 
      if (device < 0x1000000){ i2c_id = i2c_id + Wire_read()*device; device = device*0x100; }
    }
  
  }
  return i2c_id;
  
}


uint8_t user_i2c_read(uint8_t addr, uint8_t reg_addr, uint8_t *reg_data, uint32_t length)
{
  set_i2cReading(true);
if (blockint)  NVIC_DisableIRQ(GPIOTE_IRQn);
  Wire_beginTransmission(addr);
  Wire_write(reg_addr);
  if ( Wire_endTransmission(true)){
if (blockint)    NVIC_EnableIRQ(GPIOTE_IRQn);
    return -1;}
  Wire_requestFrom(addr, length,true);
  for (int i = 0; i < length; i++) {
    *reg_data++ = Wire_read();
  }
if (blockint)  NVIC_EnableIRQ(GPIOTE_IRQn);
  set_i2cReading(false);
  return 0;
}

uint8_t usr_i2c_rreg(uint8_t daddr, uint8_t raddr)
{
  uint8_t data;
  user_i2c_read(daddr, raddr, &data, 1);
  return data;
}


uint8_t user_i2c_write(uint8_t addr, uint8_t reg_addr, const uint8_t *reg_data, uint32_t length)
{
  byte error;
  set_i2cReading(true);
if (blockint)  NVIC_DisableIRQ(GPIOTE_IRQn);
  Wire_beginTransmission(addr);
  Wire_write(reg_addr);
  for (int i = 0; i < length; i++) {
    Wire_write(*reg_data++);
  }
  if ( Wire_endTransmission(true)){
if (blockint)    NVIC_EnableIRQ(GPIOTE_IRQn);
    return -1;
     }
if (blockint)  NVIC_EnableIRQ(GPIOTE_IRQn);   
  set_i2cReading(false);
  return 0;
}

void usr_i2c_wreg(uint8_t daddr, uint8_t waddr, uint8_t data) {
  user_i2c_write(daddr, waddr, &data, 1);
}


uint8_t _uc_pinSDA = g_ADigitalPinMap[TP_SDA];
uint8_t _uc_pinSCL = g_ADigitalPinMap[TP_SCL];
bool receiving;
bool transmissionBegun = false;
bool suspended;
RingBuffer rxBuffer;
RingBuffer txBuffer;
uint8_t txAddress;

void Wire_begin(void) {
  NRF_GPIO->PIN_CNF[_uc_pinSCL] = ((uint32_t)GPIO_PIN_CNF_DIR_Input      << GPIO_PIN_CNF_DIR_Pos)
                                  | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                                  | ((uint32_t)GPIO_PIN_CNF_PULL_Pullup      << GPIO_PIN_CNF_PULL_Pos)
                                  | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0D1       << GPIO_PIN_CNF_DRIVE_Pos)
                                  | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);

  NRF_GPIO->PIN_CNF[_uc_pinSDA] = ((uint32_t)GPIO_PIN_CNF_DIR_Input        << GPIO_PIN_CNF_DIR_Pos)
                                  | ((uint32_t)GPIO_PIN_CNF_INPUT_Connect    << GPIO_PIN_CNF_INPUT_Pos)
                                  | ((uint32_t)GPIO_PIN_CNF_PULL_Pullup      << GPIO_PIN_CNF_PULL_Pos)
                                  | ((uint32_t)GPIO_PIN_CNF_DRIVE_S0D1       << GPIO_PIN_CNF_DRIVE_Pos)
                                  | ((uint32_t)GPIO_PIN_CNF_SENSE_Disabled   << GPIO_PIN_CNF_SENSE_Pos);

  NRF_TWIM1->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K250;
  NRF_TWIM1->ENABLE = (TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos);
  NRF_TWIM1->PSEL.SCL = _uc_pinSCL;
  NRF_TWIM1->PSEL.SDA = _uc_pinSDA;

  NVIC_ClearPendingIRQ(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);
  NVIC_SetPriority(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn, 2);
  NVIC_EnableIRQ(SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn);
}

uint8_t Wire_requestFrom(uint8_t address, size_t quantity, bool stopBit)
{
  if (quantity == 0)
  {
    return 0;
  }

  size_t byteRead = 0;
  rxBuffer.clear();

  NRF_TWIM1->ADDRESS = address;

  NRF_TWIM1->TASKS_RESUME = 0x1UL;
  NRF_TWIM1->RXD.PTR = (uint32_t)rxBuffer._aucBuffer;
  NRF_TWIM1->RXD.MAXCNT = quantity;
  NRF_TWIM1->TASKS_STARTRX = 0x1UL;

  while (!NRF_TWIM1->EVENTS_RXSTARTED && !NRF_TWIM1->EVENTS_ERROR);
  NRF_TWIM1->EVENTS_RXSTARTED = 0x0UL;

  while (!NRF_TWIM1->EVENTS_LASTRX && !NRF_TWIM1->EVENTS_ERROR);
  NRF_TWIM1->EVENTS_LASTRX = 0x0UL;

  if (stopBit || NRF_TWIM1->EVENTS_ERROR)
  {
    NRF_TWIM1->TASKS_STOP = 0x1UL;
    while (!NRF_TWIM1->EVENTS_STOPPED);
    NRF_TWIM1->EVENTS_STOPPED = 0x0UL;
  }
  else
  {
    NRF_TWIM1->TASKS_SUSPEND = 0x1UL;
    while (!NRF_TWIM1->EVENTS_SUSPENDED);
    NRF_TWIM1->EVENTS_SUSPENDED = 0x0UL;
  }

  if (NRF_TWIM1->EVENTS_ERROR)
  {
    NRF_TWIM1->EVENTS_ERROR = 0x0UL;
  }

  byteRead = rxBuffer._iHead = NRF_TWIM1->RXD.AMOUNT;

  return byteRead;
}

void Wire_beginTransmission(uint8_t address) {
  txAddress = address;
  txBuffer.clear();
  transmissionBegun = true;
}

uint8_t Wire_endTransmission(bool stopBit)
{
  transmissionBegun = false ;

  // Start I2C transmission
  NRF_TWIM1->ADDRESS = txAddress;

  NRF_TWIM1->TASKS_RESUME = 0x1UL;

  NRF_TWIM1->TXD.PTR = (uint32_t)txBuffer._aucBuffer;
  NRF_TWIM1->TXD.MAXCNT = txBuffer.available();

  NRF_TWIM1->TASKS_STARTTX = 0x1UL;

  while (!NRF_TWIM1->EVENTS_TXSTARTED && !NRF_TWIM1->EVENTS_ERROR);
  NRF_TWIM1->EVENTS_TXSTARTED = 0x0UL;

  if (txBuffer.available()) {
    while (!NRF_TWIM1->EVENTS_LASTTX && !NRF_TWIM1->EVENTS_ERROR);
  }
  NRF_TWIM1->EVENTS_LASTTX = 0x0UL;

  if (stopBit || NRF_TWIM1->EVENTS_ERROR)
  {
    NRF_TWIM1->TASKS_STOP = 0x1UL;
    while (!NRF_TWIM1->EVENTS_STOPPED);
    NRF_TWIM1->EVENTS_STOPPED = 0x0UL;
  }
  else
  {
    NRF_TWIM1->TASKS_SUSPEND = 0x1UL;
    while (!NRF_TWIM1->EVENTS_SUSPENDED);
    NRF_TWIM1->EVENTS_SUSPENDED = 0x0UL;
  }

  if (NRF_TWIM1->EVENTS_ERROR)
  {
    NRF_TWIM1->EVENTS_ERROR = 0x0UL;

    uint32_t error = NRF_TWIM1->ERRORSRC;

    NRF_TWIM1->ERRORSRC = error;

    if (error == TWIM_ERRORSRC_ANACK_Msk)
    {
      return 2;
    }
    else if (error == TWIM_ERRORSRC_DNACK_Msk)
    {
      return 3;
    }
    else
    {
      return 4;
    }
  }
  return 0;
}

void Wire_write(uint8_t ucData)
{
  if ( !transmissionBegun || txBuffer.isFull() )
  {
    return;
  }
  txBuffer.store_char( ucData ) ;
}

int Wire_read(void)
{
  return rxBuffer.read_char();
}

extern "C"
{
  void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void)
  {
    if (NRF_TWIS1->EVENTS_WRITE)
    {
      NRF_TWIS1->EVENTS_WRITE = 0x0UL;

      receiving = true;

      rxBuffer.clear();

      NRF_TWIS1->RXD.PTR = (uint32_t)rxBuffer._aucBuffer;
      NRF_TWIS1->RXD.MAXCNT = sizeof(rxBuffer._aucBuffer);

      NRF_TWIS1->TASKS_PREPARERX = 0x1UL;
    }

    if (NRF_TWIS1->EVENTS_READ)
    {
      NRF_TWIS1->EVENTS_READ = 0x0UL;

      receiving = false;
      transmissionBegun = true;

      txBuffer.clear();

      transmissionBegun = false;

      NRF_TWIS1->TXD.PTR = (uint32_t)txBuffer._aucBuffer;
      NRF_TWIS1->TXD.MAXCNT = txBuffer.available();

      NRF_TWIS1->TASKS_PREPARETX = 0x1UL;
    }

    if (NRF_TWIS1->EVENTS_STOPPED)
    {
      NRF_TWIS1->EVENTS_STOPPED = 0x0UL;

      if (receiving)
      {
        int rxAmount = NRF_TWIS1->RXD.AMOUNT;

        rxBuffer._iHead = rxAmount;
      }
    }

    if (NRF_TWIS1->EVENTS_ERROR)
    {
      NRF_TWIS1->EVENTS_ERROR = 0x0UL;

      uint32_t error = NRF_TWIS1->ERRORSRC;
      NRF_TWIS1->ERRORSRC = error;

      NRF_TWIS1->TASKS_STOP = 0x1UL;
    }
  }
}
