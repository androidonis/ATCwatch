# ATCwatch  -WatcH
Custom Arduino C++ firmware for the P8 and PineTime plus many more DaFit Smartwatches

forked from atc1441/ATCwatch to add functions to my P8 (P8a). 
I started with a P8B (MOY-TON5), which has different accelerator chip SC7A20 similar to STM LIS3DH and unfortunately unidentified HR Sensor.
I got a used P8A MOY-TFK5 which uses accelerator chip BMA421 and HRS3300 heart rate sensor.

BLE Functions I added to ATCwatch:
- BLE command AT+I2CwDDRRVV / AT+I2CrDDRR to write and read internal I2C bus DD=Device,RR=Register,VV=Value(allHex). 
- In the info menu I added I2C addresses which are active (44 18 15 for P8a HRS, ACL, Touch)
- BLE command AT+DateMMddhhmm!text to set alarm notification. MM=Month,dd=Day,hh=hour,mm=minute,! indicates repetitive, text can be 19char
- BLE command AT+Alarm as it is send by D6-Notification App
- BLE command AT+CDhhmm! to set count down for hh=hours,mm=minutes, ! indicates repetitive 
- BLE commant AT+heart### to show stored heart rates, 000 is most recent, 500 are kept, default every 5 minutes there is a new measurement
see ble.cpp for more details.

Boot-Process:
I added a number to the bootscreen to see the boot process and identify a problem if there is a hang in the process
see ATCwatch.ino for details

Home screen:
- I added temperature vlaue and accelerator values on the top of screen. On a flat table xzy values should be close to zero
- The @ in front of the time changes color from blue to cyan to green to yellow to orange to red to magenta during one day
- Seconds change color during one minute
- Weekday is displayed with a color for every weekday (blue=Mo, magenta = So). (My week starts at Monday)
- Date changes color during the year (blue 1.1.-magenta 31.12.)
- 4 heart rates are shown and the minute when a new measurement is scheduled
- steps are unfortunately 0 (I still need to get this working, maybe I missed update on BMA421)
see menu_Home.h for details

Heart Rate:
- still in Work, should show min avg max and actual.
see menu_Heart.h for details

I2C:
I experienced a hang with blocked I2C communication. Solution was to toggle SCL pin to force HRS to release I2C bus.
see i2c.cpp for details.

Backlight:
I work to get PWM running on the Backlight pins. I will test an option to have all time on night display with minimal LED current (0.5mA?)
This should be configurable with BLE command AT+BLllmmhh, ll=PWM value for low-pin, mm= PWM value for mid-pin, hh= PWM value for high-pin
sleep.cpp needs to be modified to not disable PWM, if BLE command AT+CBL1 is send, AT+CBL0 for default.
see backlight.cpp for details.



About 150–200uA standby current consumtion. Currently 92 Hours Runtime with heavy notification and usage!
Basic menu system and notification working.

If you want to use it on PineTime comment the #define P8WATCH to enable the PineTime Pinout. 
For other watches please try to check the pinout of them and make changes as needed.

**You can support my work via paypal: https://paypal.me/hoverboard1 This keeps projects like this coming.**

Use this portable Arduino version:
- https://atcnetz.de/downloads/D6Arduino.rar

Here is a manual on how to use it:
- https://youtu.be/3gjmEdEDJ5A

Unzip, open Arduino.exe, open the ATCwatch.ino file and compile. Done.
Simply select the DaFit as the board and click on compile. You will get an Update file you can flash in the Arduino folder.

Please make sure you have always an option to go back to Bootloader somehow. I can only suggest to have one watch opened and connected an SWD St-Link V2 to it to develop on that.


**This can be flashed without opening the Watch with this Repo https://github.com/atc1441/DaFlasherFiles and the DaFlasher app from the PlayStore (Android):**
- https://play.google.com/store/apps/details?id=com.atcnetz.paatc.patc&hl=en_US
- https://www.youtube.com/watch?v=gUVEz-pxhgg

This firmware is meant to be used with the D6Notification App from the PlayStore (Android):
- https://play.google.com/store/apps/details?id=com.atcnetz.de.notification&hl=gsw

## Credits
Many Thanks to Daniel Thompson for his help (https://github.com/daniel-thompson/wasp-os).

Also many Thanks to https://github.com/endian-albin for helping so much with the Licensing.

## Copyright and licenses

This program is created by Aaron Christophel and made available under the GNU General Public License version 3 or, at your option, any later version.

The icons are under the Apache License 2.0 and taken from here:
- https://github.com/Remix-Design/RemixIcon
- https://github.com/google/material-design-icons/ (the boot logo)

The Montserrat font is under the SIL Open Font License, Version 1.1 and taken from here:
- https://fonts.google.com/specimen/Montserrat

## FAQ / Errors
#### Code does not compile / Arduino puts out errors.
---> This code is meant to be used with the D6Arduino.rar on Windows and is tested that way. Sometimes I make changes to it so you need to update that also when getting the newest firmware. I will try to add information when you need to update it but can not guarantee it, so just try an update if it does not work.

The latest update that needs the newest D6Arduino.rar file is this: https://github.com/atc1441/ATCwatch/commit/0dd3138d10d5c8f1a776ad1b7f1d4819d686e46f

SPDX-License-Identifier: GPL-3.0-or-later
