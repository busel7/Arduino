# Arduino Project Grobak.Net

## PANDUAN MEMBUAT JWS - Jadwal Waktu Sholat

Jadwal Waktu Sholat menggunakan NodeMCU atau ESP8266, LED Panel P10, RTC DS3231 dan BUZZER

Youtube Channel Playlist : https://www.youtube.com/watch?v=lvVriKy26gw&list=PLKuQ-UWqAt3O8WmFeoTbUa2keQKbdOXVv

### FITUR

- Jam Besar
- Jadwal Sholat 5 Waktu dan Tanbih Imsak
- Alarm Adzan Waktu Solat dan Tanbih Imsak
- Tanggal Masehi dan Tanggal Hijriah
- Suhu
- Alarm Adzan
- Hitung Mundur Iqomah
- Tampilan Waktu Sholat
- Running Text Nama Masjid

### Hardware

JWS KIT
- https://tokopedia.com/elektronmartcom/jws-kit-controller-led-p10-hub12-mp3-murottal-jadwal-sholat-digital
- https://www.bukalapak.com/p/elektronik/media-player-set-top-box/36buedq-jual-jws-kit-controller-led-p10-hub12-mp3-murottal-jadwal-sholat-digital

Controller JWS FullSet PCB ElektronMart JWSNodeMCUP10 Board v2.0
- https://tokopedia.com/elektronmartcom/controller-jws-murottal-quad-multi-panel-wifi-mp3-jam-jadwal-sholat
- https://www.bukalapak.com/p/elektronik/elektronik-lainnya/315l4pa-jual-controller-jws-murottal-quad-multi-panel-wifi-mp3-jam-jadwal-sholat-led-masjid

LED Panel P10
- https://www.tokopedia.com/elektronmartcom/led-display-panel-p10-smd-outdoor-green-hijau
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/2qkxq35-jual-led-display-panel-p10-smd-outdoor-green-hijau

PCB Only
- https://tokopedia.com/elektronmartcom/pcb-jws-nodemcu-board-2-0-elektronmart-led-p10-arduino
- https://www.bukalapak.com/p/elektronik/komponen-elektronik/2aiovn2-jual-pcb-jws-nodemcu-board-2-0-elektronmart-led-p10-arduino



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

### Eksternal Library
- DMDESP : https://github.com/busel7/DMDESP
- PrayerTime : https://github.com/asmaklad/Arduino-Prayer-Times
- RTC DS3231 : https://github.com/Makuna/Rtc
- ArduinoJson V6 : https://github.com/bblanchon/ArduinoJson
- F1kM_Hisab : https://github.com/wardi1971/F1kM_Hisab

Tools : 
- Desain bitmap dan font : http://dotmatrixtool.com
- LittleFS Uploader : https://github.com/earlephilhower/arduino-esp8266littlefs-plugin/releases

        
### Catatan : 
- Perlu Power Eksternal 5V ke LED P10.
- Saat Flashing (upload program) cabut sementara pin untuk buzzer.

> email : bonny@grobak.net - www.grobak.net
