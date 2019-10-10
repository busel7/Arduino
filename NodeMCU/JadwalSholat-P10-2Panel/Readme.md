# Arduino Project Grobak.Net

## LED JWS 2 Panel - Jam Waktu Sholat

<a href="http://www.youtube.com/watch?feature=player_embedded&v=HxDZwUOh890" target="_blank"><img src="http://img.youtube.com/vi/HxDZwUOh890/0.jpg" 
alt="Jam Waktu Sholat JWS 2 Panel LED Clock Prayer Times NodeMCU ESP8266 DMD P10 Arduino Web Config V3" width="240" height="180" border="10" /></a>

Jam Waktu Sholat menggunakan: 

FullSet PCB ElektronMart JWSNodeMCUP10 Board v2.0
- https://www.tokopedia.com/elektronmartcom/pcb-full-set-jws-nodemcu-board-2-0-elektronmart-led-p10-arduino
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/2cqmkgd-jual-pcb-full-set-jws-nodemcu-board-2-0-elektronmart-led-p10-arduino

PCB ElektronMart JWSNodeMCUP10 Board v2.0
- https://www.tokopedia.com/elektronmartcom/pcb-jws-nodemcu-board-2-0-elektronmart-led-p10-arduino
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/2aiovn2-jual-pcb-jws-nodemcu-board-2-0-elektronmart-led-p10-arduino

NodeMCU ESP8266
- https://www.tokopedia.com/elektronmartcom/nodemcu-esp8266-v3-lua-wifi-board-ch340
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/1lpt86t-jual-nodemcu-esp8266-v3-lua-wifi-board-ch340

LED Panel P10

RTC DS3231
- https://www.tokopedia.com/elektronmartcom/rtc-ds3231-at24c32-iic-precision-clock-module-ds3231sn-without-battery
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/1flsmxn-jual-rtc-ds3231sn-at24c32-iic-precision-clock-module-without-battery

Buzzer
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/1fo3u1t-jual-piezo-active-buzzer-5v
- https://www.tokopedia.com/elektronmartcom/piezo-active-buzzer-5v

### FITUR

- Jam Besar
- Jadwal Sholat 5 Waktu dan Tanbih Imsak
- Alarm Adzan 5 Waktu Solat dan Tanbih Imsak
- Tanggal
- Suhu
- Hitung Mundur Iqomah
- Setting Waktu lewat Wifi dengan browser
- Setting Display tingkat kecerahan

### Pin on DMD LED P10 Panel

| DMD P10 | NODEMCU | 
| ------- | ------- |
| A       | D0      |                                                 
| B       | D6      |
| CLK     | D5      |
| SCK     | D3      |                            
| R       | D7      |
| NOE     | D8      |
| GND     | GND     |

### Pin on RTC DS3231

| DS3231 | NODEMCU |
| ------ | ------- |
| SCL    | D1      |
| SDA    | D2      |
| VCC    | 3V      |
| GND    | GND     |

### Pin on Buzzer

| Buzzer | NODEMCU |
| ------ | ------- |
| +      | RX      |
| -      | GND     |

### Eksternal Library
- HJS589(DMD3 porting for ESP8266 by Ahmad Herman) < DMD3 by Wardi Utari @ MFH Robotic (info https://www.facebook.com/MFH.Robotic/)) < DMD : https://github.com/freetronics/DMD
- PrayerTime : https://github.com/asmaklad/Arduino-Prayer-Times
- RTC DS3231 : https://github.com/Makuna/Rtc
- ArduinoJson V6 : https://github.com/bblanchon/ArduinoJson

### Setting Jam dan Waktu Sholat
1. Sambungkan Wifi Handphone atau Laptop ke SSID : JWSP10
2. Dengan Password : elektronmart
3. Buka browser misalnya Chrome
4. Masukkan alamat http://192.168.4.1


### Catatan : 
- Perlu Power Eksternal 5V ke LED P10.
- Saat Flashing (upload program) cabut sementara pin untuk buzzer.

> email : bonny@grobak.net - www.grobak.net - www.elektronmart.com
