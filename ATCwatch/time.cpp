/*
 * Copyright (c) 2020 Aaron Christophel
 * changes 2021 by Andreas Loew
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "Arduino.h"
#include "sleep.h"
#include "time.h"
#include "heartrate.h"
#include "inputoutput.h"
#include <TimeLib.h>
#include "accl.h" // to use isin, arcsin
// #include "nrf_rtc.h" in progress to store time in a rtc that survives a reboot

time_data_struct time_data;
SunTime sun;
int lastday = 99;
uint16_t secsbetweenbuttonpress = 0;
  int timezone = 1;
  int longitude = 885;
  int latitude = 4988;
  bool summer = false;

void Setcoordinates(int longitu, int latitu, int timezn){
  timezone = timezn;
  longitude = longitu;
  latitude = latitu;
  lastday = 99;
  }

void setbuttontime(uint16_t timebetwpress){
  secsbetweenbuttonpress = timebetwpress;  
  }
uint16_t secbutton(){
  return secsbetweenbuttonpress;  
  }


void init_time() {
  int year = 2022;
  int month = 03;
  int day = 19;
  int hr = 23;
  int min = 44;
  int sec = 0;
  if (get_time().year < 2021) setTime( hr, min, sec, day, month, year);
}

time_data_struct get_time() {
  time_data.year = year();
  time_data.month = month();
  time_data.day = day();
  time_data.hr = hour();
  time_data.min = minute();
  time_data.sec = second();
  return time_data;
}


int32_t sumsin( uint16_t angle) {
  uint32_t accu, mult,  summe;
  int32_t n1, add;
  uint8_t itera;
  bool negate = false;
  if (angle>18000){angle = 36000 - angle; negate = true;}
  if (angle>9000){angle = 18000 - angle;}
  summe = (angle* 31416 )/ 180;
  n1 = 1; 
  accu = summe/10;
  mult = accu/10;
  for (itera = 3; itera < 11; itera+=2){
    n1 = n1 * (1-itera)*itera;
    if(accu<1048576) accu = ((((accu/10) *mult)/10000)*mult)/1000;
    else accu = (((accu/10 *mult)/100000)*mult)/100;
    add = (int32_t)(accu *10) / n1;
    summe = summe + add;
  }
  //return accu;
  if (negate) return ((0-(int32_t)summe-5)/10); else return ((summe+5)/10); // input value 0000-36000 angle, output -100.000-100.000
  }


SunTime get_suntime() {

  uint32_t days, meansolar, coshour_bot, halfsunday;
  int32_t center, declination, coshour_top;
  uint16_t dayofyear,anomalie,eclilongi,transit;
  int16_t decangle;
  int8_t signlat = 1;
  uint8_t firstwday;
  if (day()!=lastday){  //only do calculation once a day
  // day of the year:
  dayofyear = day()+ (month()-1)*28 + ("ADDGILNQTVY["[month()-1]-65); if (((year()%4)==0)&&(month()>2)) dayofyear++;
  // days since 1.1.2000
  days = (year()-2000)*365 + ((year()-2001)/4);// works this century, use -(year()>2100) later....
  firstwday = (days+1+5)%7; // First weekday of the actual year
  days = days + dayofyear; 
  meansolar = days*100000-longitude*1000/360 + 80;
  anomalie = ((meansolar%36525964)/100 *9856 - 24709000)/100000; //angle 0-36000 Better formular with closer reference day possible!
  center = ((sumsin(anomalie)*19148)+ sumsin((2*anomalie)%36000) * 200 + sumsin((3*anomalie)%36000)*3)/10000;
  eclilongi = (anomalie + center/1000 + 18000 + 10294)%36000; //ideal 10293,72
  transit = ((((sumsin(anomalie)*53 - sumsin((2*eclilongi)%36000)*69)/10000)- (longitude*1000/360) + 80)*144)/1000 + 7200 + timezone*600;
  declination = 39779*sumsin(eclilongi)/100000; // value -39779....39779
  if (declination <0)decangle = 0-arcsin(abs(declination)); else decangle = arcsin(abs(declination));
  if (latitude<0) signlat = -1; else signlat = 1; //south of the equator
  coshour_top = (-144900000/2) - ((signlat*sumsin(abs(latitude))/2)*declination); //signed sin(-0,83°)=-1449
  coshour_bot = ((sumsin(latitude+9000)/2)*(sumsin(decangle+9000)/2)); //positive
  if (coshour_top>0) halfsunday = ((9000-arcsin((coshour_top*2)/(coshour_bot/25000)))*144)/360;
  else halfsunday = ((9000+arcsin((abs(coshour_top)*2)/(coshour_bot/25000)))*144)/360;
  if (halfsunday>transit) sun.sunrise=0; else sun.sunrise = (transit-halfsunday)/10;//transit - halfsunday;
  if ((halfsunday+transit)>14390) sun.sunset=1439; else sun.sunset = (transit + halfsunday)/10;
  sun.Mangle = 9000 - latitude + decangle;
  meansolar = ((((days*100000-805300000)%2953059)*360)/29530); 
  sun.moon = sumsin((meansolar+9000)%36000)/2+50000; if (meansolar < 18000) sun.moon = (0-sun.moon)/1000; else sun.moon = sun.moon/1000; //sumsin((((((days*100000-805300000)%2953059)*360)/29530)+9000)%36000)/2 + 50000; //0...100000
  //https://www.rustimation.eu/index.php/mondphase-und-beleuchtung-ausrechnen/
  // Samstag, 17. Juni 2000, 00:27:00 Uhr
  // Dienstag, 18. Januar 2022, 00:48:30 Uhr = day8053
  // phase = (tag - tagVollmond)%29.530589
  // s = cos(phase * 2 * π) 0<phase<0.5 (abnehmend)  s = – cos(phase * 2 * π) 0.5<phase<1 (zunehmend) 0.5=Neumond
  // FlächeEllipse A = s * 1 * π, Proz = ((0.5*A)+1.5708)/3.1416
  sun.dayofyear = dayofyear;
  sun.wday = (days+5)%7;
  if (firstwday>3) sun.week = (dayofyear-(7-firstwday)-1)/7 + 1; else sun.week = (dayofyear+firstwday-1)/7 + 1; // adjust to first Thursday in Week 1, week changes on adjusted day 8,15,.... 
  if ((month()==3)&&(day()>24)&&(sun.wday==6)&&(hour()==2)&&(!summer)) {summer = true;  setTime( time_data.hr+1, time_data.min, time_data.sec, time_data.day, time_data.month, time_data.year);}
  if ((month()==10)&&(day()>24)&&(sun.wday==6)&&(hour()==3)&&(summer)) {summer = false; setTime( time_data.hr-1, time_data.min, time_data.sec, time_data.day, time_data.month, time_data.year);}
  
  // auto-dst Monat Oktober 25...31 und So dann 1->0, März 25...31 und So dann 0->1
  // sun.sunrise = halfsunday; sun.sunset = transit;
  lastday = day();
  }
  return sun; 
  }

void SetDateTimeString(String datetime) {
  int year = datetime.substring(0, 4).toInt();
  int month = datetime.substring(4, 6).toInt();
  int day = datetime.substring(6, 8).toInt();
  int hr = datetime.substring(8, 10).toInt();
  int min = datetime.substring(10, 12).toInt();
  int sec = datetime.substring(12, 14).toInt();
  setTime( hr, min, sec, day, month, year);
}

void SetDate(int year, int month, int day) {
  time_data = get_time();
  setTime( time_data.hr, time_data.min, time_data.sec, day, month, year);
}

void SetTime(int hr, int min) {
  time_data = get_time();
  setTime( hr, min, 0, time_data.day, time_data.month, time_data.year);
}

String GetDateTimeString() {
  String datetime = String(year());
  if (month() < 10) datetime += "0";
  datetime += String(month());
  if (day() < 10) datetime += "0";
  datetime += String(day());
  if (hour() < 10) datetime += "0";
  datetime += String(hour());
  if (minute() < 10) datetime += "0";
  datetime += String(minute());
  return datetime;
}
