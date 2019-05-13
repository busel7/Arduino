
## JWS - Jadwal Waktu Sholat

Jadwal Waktu Sholat menggunakan hardware:

NodeMCU ESP8266
- https://www.tokopedia.com/elektronmartcom/nodemcu-esp8266-v3-lua-wifi-board-cp2102
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/1fl2z9z-jual-nodemcu-esp8266-v3-lua-wifi-board-cp2102

RTC DS3231
- https://www.tokopedia.com/elektronmartcom/rtc-ds3231-at24c32-iic-precision-clock-module-ds3231sn-without-battery
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/1flsmxn-jual-rtc-ds3231sn-at24c32-iic-precision-clock-module-without-battery

Led Matrix MAX7219
- https://www.tokopedia.com/elektronmartcom/led-dot-matrix-max7219-4-in-1-display-green-hijau
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/1flxxkk-jual-led-dot-matrix-max7219-4-in-1-display-red-merah

Buzzer
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/1fo3u1t-jual-piezo-active-buzzer-5v
- https://www.tokopedia.com/elektronmartcom/piezo-active-buzzer-5v

### FITUR

- Waktu 24 Jam
- Jadwal Sholat 5 Waktu dan Tanbih Imsak
- Alarm Adzan Waktu Solat dan Tanbih Imsak
- Tanggal
- Suhu
- Hitung Mundur Iqomah
- Setting Jam dan Tanggal via Wifi

### Pin on MAX7219 1x4 Panel

| NodeMCU | MAX7219  | 
| ------- | -------- |
| D7      | DIN      |                                                 
| D5      | CLK      |                                                  
| D3      | DS(LOAD) |

Vin & GND NodeMCU to 5V Power Supply
VCC & GND MAX7219 Panel to 5V Power Supply

### Pin on RTC DS3231

| NODEMCU | DS3231 |
| ------- | ------ |
| D1      | SCL    |
| D2      | SDA    |

VCC and GND DS3231 Module to 5V Power Supply

### Pin on Buzzer

| NODEMCU | BUZZER |
| ------- | ------ |
| D6      | +      |
| G       | GND    |

### Eksternal Library
- <RtcDS3231.h>
- "PrayerTimes.h"
- <Adafruit_GFX.h>
- <Max72xxPanel.h>
- <Fonts/Font3x78pt7b.h>
        

> email : bonny@grobak.net - www.grobak.net
