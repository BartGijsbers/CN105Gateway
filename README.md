# cn105gw
ESP32 based MQTT gateway for the CN105 protocol based Mitsubishi Heat Pumps (FTC5)

## The code is developed to do the following things:
* General
  * Pull all known information for the HP (Heat Pump) in a loop (about one's a minute)
  * Publish all know information using MQTT. Topic: cn105gw/tele
  * The information is JSON encoded  payload: {"Zone 1 Flow Temp Setpoint":"32"})
  * Provide a logging output by telnet
 
* Provide a webinterface for status of the gateway
  * You can set OTA enable/disble
  * Reboot the gateway
  * See the RSSI of the Wifi connection

## Hardware used:
I use a Lolin32 development board, but any ESP32 development board should work. Besides the Lolin32 
I use a level shifting board (5v from HP to 3.3V Lolin32) and a DC-DC convertor. The DC-DC converter 
is use to power the Lolon32 board from the 12V provided by the CN105 interface. I power the Lolin32 
with 5V.

![image](https://raw.githubusercontent.com/BartGijsbers/CN105Gateway/main/images/finishedbox.png)

![image](https://github.com/BartGijsbers/CN105Gateway/blob/main/images/physicalschema.png?raw=true)

## MQTT format
The gateway will send MQTT data in the following format:
* Topic: cn105gw/tele/0x09 (0x09 indicates the command entry received from the Heat Pump)
* Payload: {"Zone1_Room_Temp_Setpoint":"21.0","Zone2_Room_Temp_Setpoint":"23.0","Zone1_Flow_Temp_Setpoint":"32.0","Zone2_Flow_Temp_Setpoint":"31.0","Legionalla_Temp_Setpoint":"65.0","Flow_Temp_Range_Max":"35.0","Flow_Temp_Range_Min":"25.0"}

Last will and testament is implemented:
* Topic: cn105gw/status
* If online: Payload = "Online" else Payload = "Offline"

Setting parameters is done in this format:
* Topic: cn105gw/cmnd
* Payload (json encoded as strings): {"SetFlowTemperature":"33"}

## Implemented settings:
All settings are done in Topic: cn105gw/cmnd
* {"SetSystemModeOnOff":"1"}
  * 0=Standy 
  * 1=On
* {"SetHotWaterMode":"1"}
  * 0=Normal
  * 1=Eco
* {"SetACMode":"1"} 
  * 0=heating room mode
  * 1=heating flow temp
  * 2=heating heat curve
  * 3=cooling room tempmode
  * 4=cooling flow temp
  * 5=floor dryup
* {"SetFlowTemperature":"XX"} 
  * XX=Temperature (whole decimal)
* {"SetBoilerTemperature":"XX"}
  * XX=Temperature (whole decimal)
* {"SetRoomTemperature":"XX"}
  * XX=Temperature (whole decimal)
* {"ForceDHW":"1"}
  * 1=ForceDHW
  * 0= Cancel/back Normal

With help of many on the internet.

Part of code from: https://github.com/SwiCago/HeatPump

Code/decode info: https://github.com/m000c400/Mitsubishi-CN105-Protocol-Decode

Unfortunatelly I am not a professional programmer so my code is hard to follow, not logical and will have many bugs

