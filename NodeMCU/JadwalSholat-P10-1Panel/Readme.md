# Arduino Project Grobak.Net

## LED JWS - Jam Waktu Sholat

Jam Waktu Sholat menggunakan: 

NodeMCU ESP8266
- https://www.tokopedia.com/elektronmartcom/nodemcu-esp8266-v3-lua-wifi-board-cp2102
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/1fl2z9z-jual-nodemcu-esp8266-v3-lua-wifi-board-cp2102

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
- Alarm Adzan Waktu Solat dan Tanbih Imsak
- Tanggal
- Suhu
- Hitung Mundur Iqomah
- Jam Kecil pada baris atas dan Update scroll teks malului Wifi pada baris bawah

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
- DMD2 : https://github.com/freetronics/DMD2
- PrayerTime : https://github.com/asmaklad/Arduino-Prayer-Times
- RTC DS3231 : https://github.com/Makuna/Rtc
        
### Catatan : 
- Perlu Power Eksternal 5V ke LED P10.
- Saat Flashing (upload program) cabut sementara pin untuk buzzer.

> email : bonny@grobak.net - www.grobak.net
