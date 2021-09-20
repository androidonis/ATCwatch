/*
 * Copyright (c) 2020 Aaron Christophel
 * modifications (c) 2021 Andreas Loew
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
/*
 * Added features: 
 * Send BLE: AT+I2Cr[Dadr][Radr] to read I2C of Device Dadr Register Radr, Radr/Dadr 2Byte Hex, Response on Watch and BLE.
 * Send BLE: AT+I2Cw[Dadr][Radr][Rval] to write I2C of Device Dadr Register Radr with Rval, Radr/Dadr/Rval 2Byte Hex, Response of read-back on Watch and BLE.
 * Send BLE: AT+Date[MM][dd][hh][mm][!]text to set alarm notification. 
 *                  MM can be ** = void, md = multiple day hex value follows, wd/am = Weekday follows, 01-12 = Jan-Dez
 *                  dd can be 01-31 (day), Mo-So = Weekday, Hex value for multiple weekday 0/Mo/Di/Mi Do/Fr/Sa/So (03=Sa+So,7C=Mo-Fr), ** = void (every day in a month)
 *                  hhmm can be info to show matching entries, hour+minute of the alarm
 *                  ! can be ! to keep the alarm for repetition or used for one more character for one time only alarm, X for deleting matching alarm.
 * Send BLE: AT+Alarm[##][hh][mm][md] to set alarm, format is like D6-notification app, ##=number, hhmm = hour+minute, md= hex coded weekday(s) (like above).
 * Send BLE: AT+CD[hh][mm][!] to set count down for hh hours and mm minutes. [!] will give you repeating intervall
 * Send BLE: AT+heart[###] to respond with 10 heartrates, 000 most recent, 490 oldest 10 measurements.
 * Send BLE  AT+BLxxyyzz to set PWM for low,mid,high backlight output.
 * Send BLE  AT+PWM* to set PWM always on or off during sleep
 */
#include "ble.h"
#include "pinout.h"
#include <BLEPeripheral.h>
#include "sleep.h"
#include "time.h"
#include "battery.h"
#include "inputoutput.h"
#include "backlight.h"
#include "bootloader.h"
#include "push.h"
#include "accl.h"
#include "i2c.h"
#include "heartrate.h"

BLEPeripheral                   blePeripheral           = BLEPeripheral();
BLEService                      main_service     = BLEService("190A");
BLECharacteristic   TXchar        = BLECharacteristic("0002", BLENotify, 20);
BLECharacteristic   RXchar        = BLECharacteristic("0001", BLEWriteWithoutResponse, 20);

bool vars_ble_connected = false;

const uint8_t mxdate = 25;
char datetxt[mxdate][20] = {' '};
uint32_t dateinf[mxdate+5] = {0};
bool kuckkuck = true;
bool countdown =false, cdrep  = false;
unsigned long cd_time, cd_value;

void checktermin(time_data_struct t_data){
// 
  uint8_t wday = (5+t_data.day+((t_data.year-2000-(t_data.month<3))/4)+"beehcfhdgbeg"[t_data.month-1]+(t_data.year-2000))%7;
                                  if(kuckkuck){ //signal every 15min, full hour 400ms, 15min=35ms, 30min=2x70ms, 45min=3x50ms
                                  if((t_data.hr>=8)&&(t_data.min==0 )&&(t_data.sec ==0)) {set_motor_ms(400); kuckkuck = false;if (t_data.hr==8) set_backlight(2);}
                                  if((t_data.hr>=8)&&(t_data.min==15)&&(t_data.sec ==0)) {set_motor_ms(35); kuckkuck = false; if (t_data.hr==10) set_backlight(3);}
                                  if((t_data.hr>=8)&&(t_data.min==30)&&(t_data.sec ==0)) {set_motor_ms(70); kuckkuck = false;}
                                  if((t_data.hr>=8)&&(t_data.min==30)&&(t_data.sec ==2)) {set_motor_ms(70); kuckkuck = false; if (t_data.hr==19) set_backlight(2);}
                                  if((t_data.hr>=8)&&(t_data.min==45)&&(t_data.sec ==0)) {set_motor_ms(50); kuckkuck = false;}
                                  if((t_data.hr>=8)&&(t_data.min==45)&&(t_data.sec ==2)) {set_motor_ms(50); kuckkuck = false;}
                                  if((t_data.hr>=8)&&(t_data.min==45)&&(t_data.sec ==4)) {set_motor_ms(50); kuckkuck = false; if (t_data.hr==23) set_backlight(1);}
                                               }
                                  if(t_data.sec%2) kuckkuck = true; //re-set kuckkuck in odd seconds   
                                  for (uint8_t lct=0; lct<mxdate+5; lct++) { String tmp = "";
                                  if ((dateinf[lct]>>28)!=0) //top 4bit should not be zero
                                  if ((dateinf[lct]&0x00000FFF) == ((t_data.hr*100+t_data.min))) //time matches 
                                    {
                                   if(((dateinf[lct]>>28)&0x4)==4) {                             //date in weekday format
                                    //show_push("match:"+String(lct)+String(wday));
                                    if ((wday==0)&&((dateinf[lct]&0x00040000)>0)||
                                        (wday==1)&&((dateinf[lct]&0x00020000)>0)||
                                        (wday==2)&&((dateinf[lct]&0x00010000)>0)||
                                        (wday==3)&&((dateinf[lct]&0x00008000)>0)||
                                        (wday==4)&&((dateinf[lct]&0x00004000)>0)||
                                        (wday==5)&&((dateinf[lct]&0x00002000)>0)||
                                        (wday==6)&&((dateinf[lct]&0x00001000)>0)) {              // weekday matches
                                          if (lct>=mxdate) tmp="Alarm "+String(lct-mxdate+1);    // Alarm has no text
                                          else for (uint8_t sct = 0; sct <20; sct++) tmp = tmp + datetxt[lct][sct]; //load text
                                          if ((t_data.sec ==0)||(t_data.sec ==15)||(t_data.sec ==30)||(t_data.sec ==45))show_push(tmp);
                                          if (t_data.sec ==59) if(((dateinf[lct]>>28)&0x1)==0) dateinf[lct]=0; //get rid of date if it was of 'once' type at end of minute. 
                                          }
                                       } //end weekday date check
                                    else if (((dateinf[lct]&0x00FFF000) == ((t_data.day+t_data.month*32)<<12)) //day,month matches exactly
                                          || ((dateinf[lct]&0x00FFF000) == (t_data.day<<12))                   //day matches exactly, month=0(void)
                                          || ((dateinf[lct]&0x00FFF000) == 0)                                ) //day,month =0 (void) daily 
                                     { 
                                      for (uint8_t sct = 0; sct <20; sct++) tmp = tmp + datetxt[lct][sct];  //load text                              
                                      if ((t_data.sec ==0))show_push(tmp); //vibrate, push for wakeup
                                      if ((t_data.sec%30) ==20)set_motor_ms(150); //vibrate 2 times more for wakeup
                                      if (t_data.sec ==59) if(((dateinf[lct]>>28)&0x1)==0) dateinf[lct]=0; //get rid of date if it was of 'once' type at end of minute.                               
                                      }
                                    } //end match check any hit stored in tmp String
                                       
                                  } //end lct loop
                                  if (countdown) {
                                  if ((millis()-cd_time)>0) if ((((millis()-cd_time)/1000)%6)==0) {set_motor_ms(200); show_push("Countdown");}
                                  if ((millis()-cd_time)>60000) {cd_time=cd_time+cd_value; if (cdrep==false) countdown = false;}
                                  }
}                         

void clrtermin(){ 
  //obsolete  
}

void init_ble() {
  blePeripheral.setLocalName("AL_Watch_P8a"); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<inividual label for a certain device ALtable_P8a
  blePeripheral.setAdvertisingInterval(500);
  blePeripheral.setDeviceName("AL_Watch_P8a"); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<inividual label for a certain device ALtable_P8a
  blePeripheral.setAdvertisedServiceUuid(main_service.uuid());
  blePeripheral.addAttribute(main_service);
  blePeripheral.addAttribute(TXchar);
  blePeripheral.addAttribute(RXchar);
  RXchar.setEventHandler(BLEWritten, ble_written);
  blePeripheral.setEventHandler(BLEConnected, ble_ConnectHandler);
  blePeripheral.setEventHandler(BLEDisconnected, ble_DisconnectHandler);
  blePeripheral.begin();
  ble_feed();
}

void ble_feed() {
  blePeripheral.poll();
}

void ble_ConnectHandler(BLECentral& central) {
  sleep_up(WAKEUP_BLECONNECTED);
  set_vars_ble_connected(true);
}

void ble_DisconnectHandler(BLECentral& central) {
  sleep_up(WAKEUP_BLEDISCONNECTED);
  set_vars_ble_connected(false);
}

String answer = "";
String tempCmd = "";
int tempLen = 0, tempLen1;
boolean syn;

void ble_written(BLECentral& central, BLECharacteristic& characteristic) {
  char remoteCharArray[22];
  tempLen1 = characteristic.valueLength();
  tempLen = tempLen + tempLen1;
  memset(remoteCharArray, 0, sizeof(remoteCharArray));
  memcpy(remoteCharArray, characteristic.value(), tempLen1);
  tempCmd = tempCmd + remoteCharArray;
  if (tempCmd[tempLen - 2] == '\r' && tempCmd[tempLen - 1] == '\n') {
    answer = tempCmd.substring(0, tempLen - 2);
    tempCmd = "";
    tempLen = 0;
    filterCmd(answer);
  }
}

void ble_write(String Command) {
  Command = Command + "\r\n";
  while (Command.length() > 0) {
    const char* TempSendCmd;
    String TempCommand = Command.substring(0, 20);
    TempSendCmd = &TempCommand[0];
    TXchar.setValue(TempSendCmd);
    Command = Command.substring(20);
  }
}

bool get_vars_ble_connected() {
  return vars_ble_connected;
}

void set_vars_ble_connected(bool state) {
  vars_ble_connected = state;
}

void filterCmd(String Command) {
  if (Command == "AT+BOND") {
    ble_write("AT+BOND:OK");
  } else if (Command == "AT+ACT") {
    ble_write("AT+ACT:0");
  } else if (Command.substring(0, 7) == "BT+UPGB") {
    start_bootloader();
  } else if (Command.substring(0, 8) == "BT+RESET") {
    set_reboot();
  } else if (Command.substring(0, 7) == "AT+RUN=") {
    ble_write("AT+RUN:" + Command.substring(7));
  } else if (Command.substring(0, 8) == "AT+USER=") {
    ble_write("AT+USER:" + Command.substring(8));
  } else if (Command == "AT+PACE") {
    accl_data_struct accl_data = get_accl_data();
    ble_write("AT+PACE:" + String(accl_data.steps));
  } else if (Command == "AT+BATT") {
    ble_write("AT+BATT:" + String(get_battery_percent()));
  } else if (Command.substring(0, 8) == "AT+PUSH=") {
    ble_write("AT+PUSH:OK");
    show_push(Command.substring(8));
  } else if (Command == "BT+VER") {
    ble_write("BT+VER:P8");
  } else if (Command == "AT+VER") {
    ble_write("AT+VER:P8");
  } else if (Command == "AT+SN") {
    ble_write("AT+SN:P8");
  } else if (Command.substring(0, 12) == "AT+CONTRAST=") {
    //String contrastTemp = Command.substring(12);
    uint8_t perc1 = (uint8_t)(strtoul(Command.substring(12,13).c_str(), NULL, 16));
    set_backlight(perc1);
    /*
    if (contrastTemp == "100")
      set_backlight(1);
    else if (contrastTemp == "175")
      set_backlight(3);
    else set_backlight(7);
    */
    ble_write("AT+CONTRAST:" + Command.substring(12));
  } else if (Command.substring(0, 10) == "AT+MOTOR=1") {
    String motor_power = Command.substring(10);
    if (motor_power == "1")
      set_motor_power(50);
    else if (motor_power == "2")
      set_motor_power(200);
    else set_motor_power(350);
    ble_write("AT+MOTOR:1" + Command.substring(10));
    set_motor_ms();
  } else if (Command.substring(0, 6) == "AT+DT=") {
    SetDateTimeString(Command.substring(6));
    ble_write("AT+DT:" + GetDateTimeString());
  } else if (Command.substring(0, 5) == "AT+DT") {
    ble_write("AT+DT:" + GetDateTimeString());
  } else if (Command.substring(0, 8) == "AT+HTTP=") {
    show_http(Command.substring(8));
  } else if (Command.substring(0, 6) == "AT+I2C") {
    if (Command.substring(6, 7)=="w") { 
      uint8_t daddr = (uint8_t)(strtoul(Command.substring(7,9).c_str(), NULL, 16));
      uint8_t waddr = (uint8_t)(strtoul(Command.substring(9,11).c_str(), NULL, 16));
      uint8_t wdat = (uint8_t)(strtoul(Command.substring(11,13).c_str(), NULL, 16));
      usr_i2c_wreg(daddr,waddr,wdat);
      ble_write("AT+I2Cw:D:" + String(daddr,HEX)+" R:"+ String(waddr,HEX)+ "="+ String(wdat,HEX));
      wdat =  usr_i2c_rreg(daddr,waddr); // read back the data to see if it was accepted
      show_push("I2Cw:D:"+ String(daddr,HEX)+" R:"+ String(waddr,HEX)+ "="+ String(wdat,HEX));
    }
    if (Command.substring(6, 7)=="r") {
      uint8_t daddr = (uint8_t)(strtoul(Command.substring(7,9).c_str(), NULL, 16));
      uint8_t raddr = (uint8_t)(strtoul(Command.substring(9,11).c_str(), NULL, 16));
      uint8_t rval = usr_i2c_rreg(daddr,raddr); 
      ble_write("AT+I2Cr:D:" + String(daddr,HEX)+" R:"+ String(raddr,HEX) + "="+ String(rval,HEX));
      show_push("I2Cr:D:"+ String(daddr,HEX)+" R:"+ String(raddr,HEX)+ "="+ String(rval,HEX));
    }
  }
else if (Command.substring(0, 7) == "AT+Date") {
           uint8_t intype=0; uint16_t inmonth, inmin = 0;  
           if (Command.substring(7, 9) == "**") { intype=2; if (Command.substring(9, 11) == "**") inmonth= 0; else inmonth = (uint8_t)(strtoul(Command.substring(9,11).c_str(), NULL, 10));}
           else if (Command.substring(7, 9) == "md") { intype=4; inmonth = (uint8_t)(strtoul(Command.substring(9,11).c_str(), NULL, 16)); }
           else if ((Command.substring(7, 9) == "wd")||(Command.substring(7, 9) == "am")) { intype=4;
                                   if      (Command.substring(9, 11) == "Mo") inmonth=0x40;
                                   else if (Command.substring(9, 11) == "Di") inmonth=0x20; 
                                   else if (Command.substring(9, 11) == "Mi") inmonth=0x10;
                                   else if (Command.substring(9, 11) == "Do") inmonth=0x08;
                                   else if (Command.substring(9, 11) == "Fr") inmonth=0x04;
                                   else if (Command.substring(9, 11) == "Sa") inmonth=0x02;
                                   else if (Command.substring(9, 11) == "So") inmonth=0x01;
                                   else                                       inmonth=0xFF;   }
           else { intype=2; inmonth = (uint8_t)(strtoul(Command.substring(7,9).c_str(), NULL, 10));
                  if (Command.substring(9, 11) == "**") inmonth = inmonth*32; else inmonth = inmonth*32 + (uint8_t)(strtoul(Command.substring(9,11).c_str(), NULL, 10));}
           if (Command.substring(11, 15) == "info") {
                                  intype=0; String tmp = Command.substring(7, 11)+":";
                                  for (uint8_t lct=0; lct<mxdate; lct++) 
                                    {if ((dateinf[lct]&0x00FFF000) == (inmonth<<12)) 
                                     { tmp=tmp+"@"+String((dateinf[lct]&0x00000FFF) + 10000).substring(1,5);
                                       for (uint8_t sct = 0; sct <20; sct++) if (datetxt[lct][sct]>= 0x20) tmp = tmp + datetxt[lct][sct]; 
                                     }
                                    }                                
                                  show_push(tmp);                              
                                } //generate info on entries equal to inmonth, no further action
           else inmin = (uint8_t)(strtoul(Command.substring(11,13).c_str(), NULL, 10))*100 + (uint8_t)(strtoul(Command.substring(13,15).c_str(), NULL, 10)); 
           if (intype) {
                  if (Command.substring(15, 16) == "X") { // Delete entries with matching inmonth, inmin, inmin=2400 deletes all matching inmonth
                                                intype = 0; 
                                                for (uint8_t lct = 0; lct < mxdate; lct++) if ((inmin==2400)&&((dateinf[lct]&0x0FFFF000) == (inmonth<<12))) dateinf[lct]=0;
                                                                              else if ((dateinf[lct]&0x0FFFFFFF) == (inmin + (inmonth<<12)) ) dateinf[lct]=0;
                                                show_push("deleted");
                                                         } 
                
                  else { //find avail storage and store input
                    if (Command.substring(15, 16) == "!") intype = intype+1; // Key char for permanent date
                    int idx=-1; uint8_t avail = 24;
                    for (uint8_t lct = 0; lct < mxdate; lct++) if ((dateinf[lct]&0xF0000000) == 0x00000000 ) idx=lct; else avail--;
                    if (idx<0) show_push("No Space");
                    else {
                      dateinf[idx]= inmin + (inmonth<<12) + (intype << 28); 
                      (Command.substring(15)+"                    ").toCharArray(datetxt[idx], 20); 
                      show_push("OK"+String(dateinf[idx]) + "@"+String(idx)+" Rest:"+String(avail));                     
                    }
                  }
           }
        } 

               
else if (Command.substring(0, 9) == "AT+ALARM=") {
           uint8_t intype=5; uint16_t inmonth, inmin = 0;  
           inmonth = (uint8_t)(strtoul(Command.substring(15,17).c_str(), NULL, 16));
           inmin = (uint8_t)(strtoul(Command.substring(11,13).c_str(), NULL, 10))*100 + (uint8_t)(strtoul(Command.substring(13,15).c_str(), NULL, 10));
           //store this type of date at fix position, no custom text.
                   int idx=mxdate+(uint8_t)(strtoul(Command.substring(9,11).c_str(), NULL, 10));
                   if (idx<mxdate+5) dateinf[idx]= inmin + (inmonth<<12) + (intype << 28); 
                   show_push("OK"+String(dateinf[idx]) + "@"+String(idx));                     
                       
        } 
else if (Command.substring(0, 8) == "AT+heart") {
   uint16_t start = (uint16_t)(strtoul(Command.substring(8).c_str(), NULL, 10));
   String heartinfo = "";
   for (int8_t lpct = 0; lpct < 10; lpct++){ heartinfo += String(get_hearthistory((start+lpct)%500))+ " "; }
       ble_write(heartinfo);
      show_push(heartinfo); 
  
  }
else if (Command.substring(0, 5) == "AT+BL") {
   uint8_t perc1 = (uint8_t)(strtoul(Command.substring(5,7).c_str(), NULL, 16));
   set_bright(perc1);
   show_push("Backlight"+String(perc1));  
  }
else if (Command.substring(0, 6) == "AT+PWM") {
   uint8_t val = (uint8_t)(strtoul(Command.substring(6,8).c_str(), NULL, 16));
   if (val==0) { disp_pwm(false); show_push("NoPWM"); shdn_pwm(true); } 
   if (val==1)  {disp_pwm(true); show_push("DisplayPWM"); } 
   if (val==2)  {shdn_pwm(false); show_push("PWMalwaysOn"); }  
  }
  
else if (Command.substring(0, 5) == "AT+cd") {
           countdown = true;
           cd_value = (uint8_t)(strtoul(Command.substring(5,7).c_str(), NULL, 10))*3600000 + (uint8_t)(strtoul(Command.substring(7,9).c_str(), NULL, 10))*60000;
           if (Command.substring(9, 10) == "!") cdrep=true;
           cd_time = millis() + cd_value;
                   show_push("Countdown"+Command.substring(5, 9));                     
                       
        } 
               
   
   
}
