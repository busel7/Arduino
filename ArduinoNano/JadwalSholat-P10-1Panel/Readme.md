# Arduino Project Grobak.Net

## JWS - Jadwal Waktu Sholat

Jadwal Waktu Sholat menggunakan Arduino Nano, LED Panel P10, RTC DS3231 dan BUZZER

### FITUR

- Jam Besar
- Jadwal Sholat 5 Waktu dan Tanbih Imsak
- Alarm Adzan Waktu Solat dan Tanbih Imsak
- Tanggal
- Suhu
- Hitung Mundur Iqomah dan dapat diset perwaktu sholat
- Jam Kecil pada baris atas dan Update scroll teks

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

| Buzzer | Nano |
| ------ | ------- |
| +      | D2 |
| -      | GND         |

### Eksternal Library
- DMD : https://github.com/freetronics/DMD
- PrayerTime : https://github.com/asmaklad/Arduino-Prayer-Times
- RTC DS3231 : https://github.com/SodaqMoja/Sodaq_DS3231
        
### Catatan : 
- Power bisa langsung dari USB Arduino Nano

> email : bonny@grobak.net - www.grobak.net

