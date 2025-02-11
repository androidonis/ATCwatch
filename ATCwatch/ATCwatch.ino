/*
 * Copyright (c) 2020 Aaron Christophel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "pinout.h"
#include "watchdog.h"
#include "tasks.h"
#include "fast_spi.h"
#include "i2c.h"
#include "bootloader.h"
#include "inputoutput.h"
#include "backlight.h"
#include "battery.h"
#include "heartrate.h"
#include "time.h"
#include "touch.h"
#include "sleep.h"
#include "ble.h"
#include "interrupt.h"
#include "menu.h"
#include "display.h"
#include "accl.h"
#include "push.h"
#include "flash.h"

bool stepsWhereReseted = false;
uint32_t timebuttonpress = 0;
bool butTrig = false;


void setup() {
  delay(500);
  if (get_button()) {//if button is pressed on startup goto Bootloader, function of inputoutput.cpp
    NRF_POWER->GPREGRET = 0x01;
    NVIC_SystemReset();
  }
  init_watchdog();// Init watchdog with time: watchdog_enable(8000); in watchdog.cpp
  initRTC2(); //see sleep.cpp
  init_tasks();
  init_bootloader();
  init_fast_spi();//needs to be before init_display and external flash
  init_i2c();//needs to be before init_hrs3300, init_touch and init_accl
  init_inputoutput();
  init_backlight(); // just declare the 3 pins that control the LED and set intensity to (0), all pins are high. LED resistor 30Ohm,100Ohm,2.2KOhm (PineT)
  set_backlight(7);
  init_display();
  display_booting(10); 
  init_battery();
  display_booting(9);
  init_hrs3300();
  display_booting(8);
  init_time();
  display_booting(7);
  init_touch();
  display_booting(6);
  init_sleep();
  display_booting(5);
  init_menu();
  display_booting(4);
  init_push();
  display_booting(3);
  init_flash();
  display_booting(2);
  init_accl();
  display_booting(1);
  init_ble();//must be before interrupts!!!
  display_booting(0);
  init_interrupt();//must be after ble!!!
  delay(100);
  set_backlight(1);
  clrtermin();
  display_home(); //menu.cpp
}

void loop() {
  ble_feed();//manage ble connection
  if (!get_button()){watchdog_feed(); butTrig=true;} //reset the watchdog if the push button is not pressed, if it is pressed for more then WATCHDOG timeout the watch will reset 
  else if (butTrig) {setbuttontime((millis()- timebuttonpress)/100); timebuttonpress=millis(); butTrig=false; set_motor_ms(20);}
  if (get_sleep()) {//see if we are sleeping
    sleep_wait();//just sleeping
  } else {//if  we are awake do display stuff etc
    check_sleep_times();//check if we should go sleeping again
    display_screen();//manage menu and display stuff
    check_battery_status();// check battery status. if lower than XX show message
  }
  if (get_timed_int()) {//Theorecticly every 40ms via RTC2 but since the display takes longer its not accurate at all when display on
    if (get_sleep()) {
      if (acc_input()){
         sleep_up(WAKEUP_ACCL);//check if the hand was lifted and turn on the display if so
      }

    }
    time_data_struct time_data = get_time();
    if (time_data.hr == 0) {// check for new day
      if (!stepsWhereReseted) {//reset steps on a new day
        stepsWhereReseted = true;
        reset_step_counter();
      }
    } else stepsWhereReseted = false;
// check for stored dates:
    checktermin(time_data); //define in ble.h
   check_timed_heartrate(time_data.min);//Meassure HR every 1minutes
  }
  gets_interrupt_flag();//check interrupt flags and do something with it
}
