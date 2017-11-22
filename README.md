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

| DMD P10 | GPIO | NODEMCU | 
| ------- | ---- | ------- |
| A       | GPIO16 | D0    |                                                 
| B       | GPIO12 | D6    |                                                  
| CLK     | GPIO14 | D5    |                           
| SCK     | GPIO0  | D3    |                           
| R       | GPIO13 | D7    |
| NOE     | GPIO15 | D8    |
| GND     | GND    | GND   |

### Pin on RTC DS3231

| DS3231 | NODEMCU |
| ------ | ------- |
| SCL    | D1 (GPIO 5) |
| SDA    | D2 (GPIO 4) |
| VCC    | 3V          |
| GND    | GND         |

### Pin on Buzzer

| Buzzer | NODEMCU |
| ------ | ------- |
| +      | RX (GPIO 3) |
| -      | GND         |

### Library
DMD2 : https://github.com/freetronics/DMD2
PrayerTime : https://github.com/asmaklad/Arduino-Prayer-Times
RTC DS3231 : https://github.com/SodaqMoja/Sodaq_DS3231
        
### Catatan : 
- Perlu Power Eksternal 5V ke LED P10.
- Saat Flashing (upload program) cabut sementara pin untuk buzzer.

> email : bonny@grobak.net - www.grobak.net
