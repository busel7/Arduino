# Arduino Project Grobak.Net

## JWS - Jadwal Waktu Sholat

Jadwal Waktu Sholat menggunakan:

Arduino Nano
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/1fmlrqv-jual-nano-v3-mini-usb-ch340-black-compatible-arduino
- https://tokopedia.link/GqZKub3EjX

LED Panel P10

RTC DS3231
- https://www.tokopedia.com/elektronmartcom/rtc-ds3231-at24c32-iic-precision-clock-module-ds3231sn-without-battery
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/1flsmxn-jual-rtc-ds3231sn-at24c32-iic-precision-clock-module-without-battery

Buzzer
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/1fo3u1t-jual-piezo-active-buzzer-5v
- https://www.tokopedia.com/elektronmartcom/piezo-active-buzzer-5v

LED Panel P10

Push Button
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/1s1wa9a-jual-switch-tactile-push-button-6x6x10mm
- https://tokopedia.link/InIOtraFjX


### FITUR

- Jam Besar
- Jadwal Sholat 5 Waktu dan Tanbih Imsak
- Alarm Adzan Waktu Solat dan Tanbih Imsak
- Tanggal
- Suhu
- Hitung Mundur Iqomah dan dapat diset perwaktu sholat
- Jam Kecil pada baris atas dan Teks Berjalan

### Pin on DMD LED P10 Panel

| DMD P10 | Nano  | 
| ------- | ----- |
| A       | D6    |                                                 
| B       | D7    |                                                  
| CLK     | D13   |                           
| SCK     | D8    |                           
| R       | D11   |
| NOE     | D9    |
| GND     | GND   |

### Pin on RTC DS3231

| DS3231 | Nano |
| ------ | ---- |
| SCL    | A5   |
| SDA    | A4   |
| VCC    | 3V   |
| GND    | GND  |

### Pin on Buzzer

| Buzzer | Nano  |
| ------ | ----- |
| +      | D2    |
| -      | GND   |

### Pin on Push Button

| Button | Nano  |
| ------ | ----- |
| B1+    | A1    |
| B1-    | GND   |
| B2+    | A2    |
| B2-    | GND   |
| B3+    | A3    |
| B3-    | GND   |

### Eksternal Library
- DMD : https://github.com/freetronics/DMD
- PrayerTime : https://github.com/asmaklad/Arduino-Prayer-Times
- RTC DS3231 : https://github.com/SodaqMoja/Sodaq_DS3231
        
### Catatan : 
- Power bisa langsung dari USB Arduino Nano

> email : bonny@grobak.net - www.grobak.net

