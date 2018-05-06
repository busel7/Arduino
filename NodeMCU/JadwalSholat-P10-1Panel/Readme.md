# Arduino Project Grobak.Net

## JWS - Jadwal Waktu Sholat

Jadwal Waktu Sholat menggunakan NodeMCU atau ESP8266, LED Panel P10, RTC DS3231 dan BUZZER

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
- RTC DS3231 : https://github.com/SodaqMoja/Sodaq_DS3231
        
### Catatan : 
- Perlu Power Eksternal 5V ke LED P10.
- Saat Flashing (upload program) cabut sementara pin untuk buzzer.

> email : bonny@grobak.net - www.grobak.net
