/*
 * JADWAL WAKTU SHOLAT MENGGUNAKAN NODEMCU ESP8266, LED P10, RTC DS3241, BUZZER
 * FITUR :  JADWAL SHOLAT 5 WAKTU DAN TANBIH IMSAK, JAM BESAR, TANGGAL, SUHU, ALARAM ADZAN DAN TANBIH IMSAK,
 *          DAN HITUNG MUNDUR IQOMAH DAN UBAH WAKTU LEWAT WIFI DENGAN BROWSER.
 * 

Pin on  DMD P10     GPIO      NODEMCU               Pin on  DS3231      NODEMCU                   Pin on  Buzzer       NODEMCU

        2  A        GPIO16    D0                            SCL         D1 (GPIO 5)                       +            RX (GPIO 3)
        4  B        GPIO12    D6                            SDA         D2 (GPIO 4)                       -            GND
        8  CLK      GPIO14    D5                            VCC         3V
        10 SCK      GPIO0     D3                            GND         GND
        12 R        GPIO13    D7
        1  NOE      GPIO15    D8
        3  GND      GND       GND

Catatan : 

o Perlu Power Eksternal 5V ke LED P10.
o Saat Flashing (upload program) cabut sementara pin untuk buzzer.

Eksternal Library

- HJS589 : DMD3
- PrayerTime : https://github.com/asmaklad/Arduino-Prayer-Times
- RTC DS3231 : https://github.com/Makuna/Rtc

email : bonny@grobak.net - www.grobak.net
*/


#include <SPI.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);

#include <HJS589.h>
#include <fonts/Arial_Black_16.h>
#include <fonts/angka6x13.h>
#include <fonts/Font3x5.h>
#include <fonts/angkasm47.h>
#include <fonts/Britannic.h>
#include <fonts/arab6x13.h>

#include "PrayerTimes.h"
#include "WebPage.h"


// JADWAL SHOLAT
double times[sizeof(TimeName)/sizeof(char*)];

// Durasi waktu iqomah
struct Config {
  int iqmhs;
  int iqmhd;
  int iqmha;
  int iqmhm;
  int iqmhi;
  int ihti; // Koreksi Waktu Menit Jadwal Sholat
  float latitude;
  float longitude;
  char merek[64];
  char info1[512];
  char info2[512];
};

int iqmh;

struct ConfigWifi {
  char wifissid[64];
  char wifipassword[64];
};

uint32_t durasiadzan = 3000; // Durasi Adzan 1 detik = 1000 ms, 180000 berarti 180 detik atau 3 menit


// BUZZER
const int buzzer = 3; // Pin GPIO Buzzer - RX


// LED Internal
uint8_t pin_led = 2;


//SETUP RTC
//year, month, date, hour, min, sec and week-day(Senin 0 sampai Ahad 6)
//DateTime dt(2018, 12, 20, 16, 30, 0, 3);
RtcDateTime now;
char weekDay[][7] = {"AHAD", "SENIN", "SELASA", "RABU", "KAMIS", "JUM'AT", "SABTU", "AHAD"}; // array hari, dihitung mulai dari senin, hari senin angka nya =0,
char monthYear[][4] = { "DES", "JAN", "FEB", "MAR", "APR", "MEI", "JUN", "JUL", "AGU", "SEP", "OKT", "NOV", "DES" };


//SETUP DMD
#define DISPLAYS_WIDE 1
#define DISPLAYS_HIGH 1
HJS589 Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);  // Jumlah Panel P10 yang digunakan (KOLOM,BARIS)


//WEB Server
ESP8266WebServer server(80);

const char* password = "grobaknet";
const char* mySsid = "JWSP10"; //kalau gagal konek

IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress netmask(255, 255, 255, 0);

const char *fileconfigjws = "/configjws.json";
Config config;

const char *fileconfigwifi = "/configwifi.json";
ConfigWifi configwifi;



//----------------------------------------------------------------------
// XML UNTUK JEMBATAN DATA MESIN DENGAN WEB

void buildXML(){

  RtcDateTime now = Rtc.GetDateTime();
  RtcTemperature temp = Rtc.GetTemperature();
  XML="<?xml version='1.0'?>";
  XML+="<t>";
    XML+="<rWifissid>";
    XML+= configwifi.wifissid;
    XML+="</rWifissid>";
    XML+="<rYear>";
    XML+=now.Year();
    XML+="</rYear>";
    XML+="<rMonth>";
    XML+=now.Month();
    XML+="</rMonth>";
    XML+="<rDay>";
    XML+=now.Day();
    XML+="</rDay>";
    XML+="<rHour>";
      if(now.Hour()<10){
        XML+="0";
        XML+=now.Hour();
      }else{    XML+=now.Hour();}
    XML+="</rHour>";
    XML+="<rMinute>";
      if(now.Minute()<10){
        XML+="0";
        XML+=now.Minute();
      }else{    XML+=now.Minute();}
    XML+="</rMinute>";
    XML+="<rSecond>";
      if(now.Second()<10){
        XML+="0";
        XML+=now.Second();
      }else{    XML+=now.Second();}
    XML+="</rSecond>";
    XML+="<rTemp>";
    XML+= temp.AsFloatDegC();
    XML+="</rTemp>";
    XML+="<rIqmhs>";
    XML+= config.iqmhs;
    XML+="</rIqmhs>";
    XML+="<rIqmhd>";
    XML+= config.iqmhd;
    XML+="</rIqmhd>";
    XML+="<rIqmha>";
    XML+= config.iqmha;
    XML+="</rIqmha>";
    XML+="<rIqmhm>";
    XML+= config.iqmhm;
    XML+="</rIqmhm>";
    XML+="<rIqmhi>";
    XML+= config.iqmhi;
    XML+="</rIqmhi>";
    XML+="<rIhti>";
    XML+= config.ihti;
    XML+="</rIhti>";
    XML+="<rLatitude>";
    XML+= config.latitude;
    XML+="</rLatitude>";
    XML+="<rLongitude>";
    XML+= config.longitude;
    XML+="</rLongitude>";
    XML+="<rMerek>";
    XML+= config.merek;
    XML+="</rMerek>";
    XML+="<rInfo1>";
    XML+= config.info1;
    XML+="</rInfo1>";
    XML+="<rInfo2>";
    XML+= config.info2;
    XML+="</rInfo2>";
  XML+="</t>"; 
}

void handleXML(){
  buildXML();
  server.send(200,"text/xml",XML);
}



//----------------------------------------------------------------------
// PENGATURAN WIFI AUTO MODE STATION ATAU ACCESS POINT

void wifiConnect() {

  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  delay(1000);

  Serial.println("Wifi Sation Mode");
  WiFi.mode(WIFI_STA);
  WiFi.begin(configwifi.wifissid,configwifi.wifipassword);
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(pin_led, !digitalRead(pin_led));
    if (millis() - startTime > 3000) break;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(pin_led, HIGH);
  } else {
    Serial.println("Wifi AP Mode");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, netmask);
    WiFi.softAP(mySsid, password);
    digitalWrite(pin_led, LOW);
  }

  //Serial.println("");
  WiFi.printDiag(Serial);
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

}



//----------------------------------------------------------------------
// SETUP

void setup() {

  Serial.begin(9600);

  pinMode(pin_led, OUTPUT);

  SPIFFS.begin();
  
  loadWifiConfig(fileconfigwifi, configwifi);
  loadJwsConfig(fileconfigjws, config);
  
  WiFi.hostname("elektronmart");
  WiFi.begin(configwifi.wifissid, configwifi.wifipassword);

  wifiConnect();
  
  server.on("/", []() {
    server.send_P(200, "text/html", setwaktu);

    // Kalau ada perubahan tanggal
    if (server.hasArg("date")) {
    
      uint16_t jam;
      uint8_t menit;
      uint8_t detik;      
      String sd=server.arg("date");
      String lastSd;
      
      jam = ((sd[0]-'0')*1000)+((sd[1]-'0')*100)+((sd[2]-'0')*10)+(sd[3]-'0');
      menit = ((sd[5]-'0')*10)+(sd[6]-'0');
      detik = ((sd[8]-'0')*10)+(sd[9]-'0');
      
      if (sd != lastSd){
        RtcDateTime now = Rtc.GetDateTime();
        uint8_t hour = now.Hour();
        uint8_t minute = now.Minute();
        Rtc.SetDateTime(RtcDateTime(jam, menit, detik, hour, minute, 0));
        lastSd=sd;
      }
  
     server.send ( 404 ,"text", message );
    
    }

    // Kalau ada perubahaan jam
    if (server.hasArg("time")) {
      
      String st=server.arg("time");
      String lastSt;
      uint8_t jam = ((st[0]-'0')*10)+(st[1]-'0');
      uint8_t menit = ((st[3]-'0')*10)+(st[4]-'0');
      
      if (st != lastSt){
         RtcDateTime now = Rtc.GetDateTime();
         uint16_t year = now.Year();
         uint8_t month = now.Month();
         uint8_t day = now.Day();
         Rtc.SetDateTime(RtcDateTime(year, month, day, jam, menit, 0));
         lastSt=st;}
      server.send ( 404 ,"text", message );
  
    }
  });

  server.on("/setwifi", []() {
    server.send_P(200, "text/html", setwifi);
  });
  
  server.on("/toggle", toggleLED);
  
  server.on("/settingwifi", HTTP_POST, handleSettingWifiUpdate); 
  
  server.on("/setjws", []() {
    server.send_P(200, "text/html", setjws);
  });
  
  server.on("/settingjws", HTTP_POST, handleSettingJwsUpdate);

  server.on ( "/xml", handleXML) ;
  
  
  
  server.begin();
  Serial.println("HTTP server started");
  
  //RTC D3231
  Rtc.Begin();
  
  Wire.begin(4, 5);

  if (!Rtc.GetIsRunning()) {
    
    Rtc.SetIsRunning(true);
    
  }
  
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
  

  //Buzzer
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  delay(50);

  BuzzerPendek();

  //DMD
  Disp_init();
  Disp.setBrightness(64);

  // BRANDING
  branding();
  
}



//----------------------------------------------------------------------
// LOOP

uint8_t tampilanutama;

void loop() {

  server.handleClient();

  switch(tampilanutama) {
    case 0 :
      tampilan();
      break;
    case 1 :
      Iqomah();
      break;
  }  


}



//----------------------------------------------------------------------
//MODE TAMPILAN JAM

uint8_t tampilanjam;

void tampilan() {
  
  if (tampilanjam > 6) {
    tampilanjam = 0;
  }

  switch(tampilanjam) {

    case 0 :
      animLogoX();
      AlarmSholat();
      break;  
      
    case 1 :
      JamJatuhPulse();
      AlarmSholat();
      break;

    case 2 :
      JamArabJatuhPulse();
      AlarmSholat();
      break;

    case 3 :
      TampilTanggal();
      AlarmSholat();
      break;

    case 4 :
      TampilSuhu();
      AlarmSholat();
      break;
      
    case 5 :
      TampilJadwalSholat();
      AlarmSholat();
      break;

    case 6 :
      TeksJalan();
      AlarmSholat();
      break;
  }

}



void JamJatuhPulse() {
    
  static uint8_t y;
  static uint8_t d;           
  static uint32_t pM;
  uint32_t cM = millis();

  static uint32_t pMPulse;
  static uint8_t pulse;
  
  
  if (cM - pMPulse >= 100) {
    pMPulse = cM;
    pulse++;
  }
  
  if (pulse > 8) {
    pulse=0;
  }

  if(cM - pM > 50) { 
    if(d == 0 and y < 32) {
      pM=cM;
      y++;
    }
    if(d  == 1 and y > 0) {
      pM=cM;
      y--;
    }    
  }
  
  if(cM - pM > 15000 and y == 32) {
    d=1;
  }
  
  if(y == 32) {
    Disp.drawRect(17,3+pulse,20,11-pulse,0,1);       
  }
  
  if(y < 32) {
    Disp.drawRect(17,3,20,11,0,0);
  }
   
  if(y == 0 and d == 1) {
    d=0;
    Disp.clear();
    tampilanjam = 2;
  }
  
  TampilJamDinamis(y - 32);
  
    
}



void JamArabJatuhPulse() {
    
  static uint8_t y;
  static uint8_t d;           
  static uint32_t pM;
  uint32_t cM = millis();

  static uint32_t pMPulse;
  static uint8_t pulse;
  
  
  if (cM - pMPulse >= 100) {
    pMPulse = cM;
    pulse++;
  }
  
  if (pulse > 8) {
    pulse=0;
  }

  if(cM - pM > 50) { 
    if(d == 0 and y < 32) {
      pM=cM;
      y++;
    }
    if(d  == 1 and y > 0) {
      pM=cM;
      y--;
    }    
  }
  
  if(cM - pM > 15000 and y == 32) {
    d=1;
  }
  
  if(y == 32) {
    Disp.drawRect(14,3+pulse,17,11-pulse,0,1);       
  }
  
  if(y < 32) {
    Disp.drawRect(14,3,17,11,0,0);
  }
   
  if(y == 0 and d == 1) {
    d=0;
    Disp.clear();
    tampilanjam = 3;
  }
  
  TampilJamArabDinamis(y - 32);
  
    
}


void JamJatuh() {
    
  static uint8_t y;
  static uint8_t d;              
  static uint32_t pM;
  uint32_t cM = millis();
  
  static uint32_t pMKedip;
  static boolean kedip;  

  if (cM - pMKedip >= 500) {
    pMKedip = cM;
    kedip = !kedip;    
  }

  if(cM - pM > 50) { 
    if(d == 0 and y < 32){
      pM = cM;
      y++;
    }
    
    if(d == 1 and y > 0){
      pM=cM;
      y--;
    }    
  }
  
  if(cM - pM > 15000 and y == 32) {
    d=1;    
  }
  
  if (y==32) {
    
    if (kedip) {
        // TITIK DUA ON
        Disp.drawRect(18,3,19,5,1,1);
        Disp.drawRect(18,9,19,11,1,1);
    } else {
        // TITIK DUA OFF
        Disp.drawRect(18,3,19,5,0,0);
        Disp.drawRect(18,9,19,11,0,0);          
    }
      
  }
  
  if (y < 32) {
    Disp.drawRect(18,3,19,5,0,0);
    Disp.drawRect(18,9,19,11,0,0); 
  }
   
  if (y == 3 and d==1) {
    d=0;
    Disp.clear();
    tampilanjam = 2;
  }  
  
  TampilJamDinamis(y-32); 
    
}


void TampilJamDinamis(uint32_t y) {

  RtcDateTime now = Rtc.GetDateTime();
  char jam[3];
  char menit[3];
  char detik[3];
  sprintf(jam,"%02d", now.Hour());
  sprintf(menit,"%02d", now.Minute());
  sprintf(detik,"%02d", now.Second());

  //JAM
  Disp.setFont(angka6x13);
  Disp.drawText(3, y, jam);

  //MENIT          
  Disp.setFont(Font3x5);
  Disp.drawText(22, y, menit);

  //DETIK          
  Disp.setFont(Font3x5);
  Disp.drawText(22, y+8, detik);

 
}


void TampilJamArabDinamis(uint32_t y) {

  RtcDateTime now = Rtc.GetDateTime();
  char jam[3];
  char menit[3];
  sprintf(jam,"%02d", now.Hour());
  sprintf(menit,"%02d", now.Minute());

  //JAM
  Disp.setFont(arab6x13);
  Disp.drawText(0, y, jam);

  //MENIT          
  Disp.setFont(arab6x13);
  Disp.drawText(19, y, menit);
 
}



//----------------------------------------------------------------------
//TAMPILKAN JAM BESAR

void TampilJam() {

  static uint8_t d;
  static uint32_t pM;
  static uint32_t pMJam;
  static uint32_t pMKedip;
  uint32_t cM = millis();
  static boolean kedip;
  
  RtcDateTime now = Rtc.GetDateTime();
  char jam[3];
  char menit[3];
  char detik[3];
  
  if (cM - pMJam >= 1000) {
   
    pMJam = cM;
    d++;
    
    //JAM
    sprintf(jam,"%02d", now.Hour());
    Disp.setFont(angka6x13);
    Disp.drawText(3, 0, jam);
  
    //MENIT
    sprintf(menit,"%02d", now.Minute());
    Disp.setFont(Font3x5);
    Disp.drawText(22, 0, menit);
  
    //DETIK
    sprintf(detik,"%02d", now.Second());
    Disp.setFont(Font3x5);
    Disp.drawText(22, 8, detik);

    if (d >= 10) {
      d = 0;
      Disp.clear();
      tampilanjam = 2;
    }
        
  }

  //KEDIP DETIK
  if (millis() - pMKedip >= 500) {
    pMKedip = millis();
    kedip = !kedip;
  }

  if (kedip) {    
      Disp.drawRect(18,3,19,5,1,1);
      Disp.drawRect(18,9,19,11,1,1);
  } else {
      Disp.drawRect(18,3,19,5,0,0);
      Disp.drawRect(18,9,19,11,0,0);
  }
  
}



//----------------------------------------------------------------------
//TAMPILKAN TANGGAL

void TampilTanggal() {

  RtcDateTime now = Rtc.GetDateTime();
  static uint8_t d;
  static char hari[8];
  static char tanggal[18];

  static uint32_t pM;
  uint32_t cM = millis();

  if (cM - pM > 2000) {
    pM = cM;
    d++;
  
    sprintf(hari, "%s", weekDay[now.DayOfWeek()]);
    sprintf(tanggal, "%02d %s", now.Day(), monthYear[now.Month()]);  
    
    Disp.setFont(Font3x5);
    textCenter(0,hari);;
    textCenter(9,tanggal);

    if (d >= 2) {
      d = 0;
      Disp.clear();
      tampilanjam = 4;
    }
  } 
  
}



//----------------------------------------------------------------------
// TAMPILKAN SUHU

void TampilSuhu(){
  //Tampilkan Suhu
  RtcTemperature temp = Rtc.GetTemperature();
  int celsius = temp.AsFloatDegC();
  char suhu[2];
  int koreksisuhu = 2; // Perkiraan selisih suhu ruangan dan luar ruangan

  static uint8_t d;
  static uint32_t pM;
  uint32_t cM = millis();

  if (cM - pM > 2000) {
    pM = cM;
    d++;
  
    Disp.setFont(Font3x5);    
    textCenter(0, "SUHU");
    sprintf(suhu,"%dC",celsius - koreksisuhu);
    Disp.setFont(angkasm47);
    textCenter(8, suhu);

    if (d >= 2) {
      d = 0;
      Disp.clear();
      tampilanjam = 5;
    }  

  } 
}



//----------------------------------------------------------------------
// PARAMETER PENGHITUNGAN JADWAL SHOLAT

void JadwalSholat() {

  RtcDateTime now = Rtc.GetDateTime();

  int tahun = now.Year();
  int bulan = now.Month();
  int tanggal = now.Day();

  int dst=7; // TimeZone
  
  set_calc_method(Karachi);
  set_asr_method(Shafii);
  set_high_lats_adjust_method(AngleBased);
  set_fajr_angle(20);
  set_isha_angle(18);
  
  //SETTING LOKASI DAN WAKTU Masjid Miftahul Jannah
  float latitude=-6.165010;
  float longitude=106.608892;

  get_prayer_times(tahun, bulan, tanggal, latitude, longitude, dst, times);

}



//----------------------------------------------------------------------
// TAMPILAN JADWAL SHOLAT

void TampilJadwalSholat() {
  
  JadwalSholat();

  static uint8_t i;
  static uint32_t pM;
  uint32_t cM = millis();
  
  char sholat[7];
  char jam[5];
  char TimeName[][8] = {"SUBUH","TERBIT","DZUHUR","ASHAR","TRBNM","MAGHRIB"," ISYA'"};
  int hours, minutes;    
  
  if (cM - pM >= 2000) {
    
    pM = cM;    
    Disp.clear();

    if (i == 1) {i = 2;} // Abaikan Terbit
    if (i == 4) {i = 5;} // Abaikan Terbenam

    get_float_time_parts(times[i], hours, minutes);
  
    minutes = minutes + config.ihti;
    
    if (minutes >= 60) {
      minutes = minutes - 60;
      hours ++;
    }
    
    String sholat = TimeName[i];
    sprintf(jam,"%02d:%02d", hours, minutes);     
    
    Disp.setFont(Font3x5);
    textCenter(0,sholat);
    Disp.setFont(angkasm47);
    textCenter(8,jam);
    
    i++;    
    
    if (i > 7) {
      get_float_time_parts(times[0], hours, minutes);
      minutes = minutes + config.ihti;
      if (minutes < 11) {
        minutes = 60 - minutes;
        hours --;
      } else {
        minutes = minutes - 10 ;
      }
      sprintf(jam,"%02d:%02d", hours, minutes);
      Disp.setFont(Font3x5);
      textCenter(0,"TANBIH");
      Disp.setFont(angkasm47);
      textCenter(8,jam);
      
      if (i > 8) {
        i = 0;
        Disp.clear();
        tampilanjam = 6;
      }
      
    }  
     
  }
    
}



//----------------------------------------------------------------------
// ALARM SHOLAT BERJALAN SAAT MASUK WAKTU SHOLAT

void AlarmSholat() {

  RtcDateTime now = Rtc.GetDateTime();

  int Hari = now.DayOfWeek();
  int Hor = now.Hour();
  int Min = now.Minute();
  int Sec = now.Second();

  JadwalSholat();
  int hours, minutes, seconds;

  // Tanbih Imsak
  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes + config.ihti;

  if (minutes < 11) {
    minutes = 60 - minutes;
    hours --;
  } else {
    minutes = minutes - 10 ;
  }

  if (Hor == hours && Min == minutes) {
    BuzzerPendek();
    Disp.clear();
    Disp.setFont(Font3x5);
    textCenter(5, "TANBIH");
    delay(durasiadzan);
    Disp.clear();

  }

  // Subuh
  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes + config.ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }

  if (Hor == hours && Min == minutes) {
    BuzzerPendek();
    Disp.clear();
    Disp.setFont(Font3x5);
    textCenter(5, "SUBUH");
    delay(durasiadzan);
    Disp.clear();

    iqmh = config.iqmhs;
    tampilanutama = 1;
  }

  // Dzuhur
  get_float_time_parts(times[2], hours, minutes);
  minutes = minutes + config.ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }

  if (Hor == hours && Min == minutes && Hari != 5) {
    BuzzerPendek();
    Disp.clear();
    Disp.setFont(Font3x5);
    textCenter(5, "DZUHUR");
    delay(durasiadzan);
    Disp.clear();

    iqmh = config.iqmhd;
    tampilanutama = 1;

  } else if (Hor == hours && Min == minutes && Hari == 5) {
    BuzzerPendek();
    Disp.clear();
    Disp.setFont(Font3x5);
    textCenter(5, "JUM'AT");
    delay(durasiadzan);

  }

  // Ashar
  get_float_time_parts(times[3], hours, minutes);
  minutes = minutes + config.ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }

  if (Hor == hours && Min == minutes) {
    BuzzerPendek();
    Disp.clear();
    Disp.setFont(Font3x5);
    textCenter(5, "ASHAR");
    delay(durasiadzan);
    Disp.clear();

    iqmh = config.iqmha;
    tampilanutama = 1;
  }

  // Maghrib
  get_float_time_parts(times[5], hours, minutes);
  minutes = minutes + config.ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }

  if (Hor == hours && Min == minutes) {
    BuzzerPendek();
    Disp.clear();
    Disp.setFont(Font3x5);
    textCenter(5, "MAGHRIB");
    delay(durasiadzan);
    Disp.clear();

    iqmh = config.iqmhm;
    tampilanutama = 1;
  }

  // Isya'
  get_float_time_parts(times[6], hours, minutes);
  minutes = minutes + config.ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }

  if (Hor == hours && Min == minutes) {
    BuzzerPendek();
    Disp.clear();
    Disp.setFont(Font3x5);
    textCenter(5, "ISYA'");
    delay(durasiadzan);
    Disp.clear();

    iqmh = config.iqmhi;
    tampilanutama = 1;
  }

}


//----------------------------------------------------------------------
// HITUNG MUNDUR WAKTU SETELAH ADZAN SAMPAI MULAI IQOMAH

void Iqomah() {

  static uint8_t menit = iqmh - 1;
  static uint8_t detik = 59;
  static uint32_t pMIqmh;
  uint32_t cM = millis();
  static char hitungmundur[6];
  
  Disp.setFont(Font3x5);
  textCenter(0, "IQOMAH");

  if(cM - pMIqmh > 1000) {
    pMIqmh = cM;
    detik--;
    
    if (menit == 0 && detik == 0){
        Disp.clear();
        textCenter(0, "LURUS 8");
        textCenter(9, "RAPAT");
        BuzzerPanjang();
        detik = 59;
        Disp.clear();
        tampilanutama = 0;
        return;
    }
    
    if (detik == 0) {
      menit--;
      detik = 59;      
    }
    
  }
  
  sprintf(hitungmundur, "%02d:%02d", menit, detik);
  Disp.setFont(angkasm47);
  textCenter(9, hitungmundur);
  
}



//----------------------------------------------------------------------
// BUNYIKAN BEEP BUZZER

void BuzzerPanjang() {
  
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
  delay(1000);
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
  delay(50);
  
}

void BuzzerPendek() {
  
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(100);
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(50);
  
}



//----------------------------------------------------------------------
// TAMPILKAN SCROLLING TEKS YANG DIINPUT MELALUI WEBSITE

static char *merek[] = {config.merek};

void TeksJalan() {

  static uint16_t durasi;
  static uint32_t pM;
  static uint32_t x;
  static uint32_t Speed = 50;
  int width = Disp.width();
  Disp.setFont(Britannic);
  int fullScroll = Disp.textWidth(merek[0]) + width;
  if((millis() - pM) > Speed) { 
    pM = millis();
    if (x < fullScroll) {
      ++x;
    } else {
      x = 0;
      tampilanjam = 0;
      return;
    }
    Disp.drawText(width - x, 3, merek[0]);
  }

  durasi = (fullScroll * Speed) + (fullScroll * 5);
  

}



//----------------------------------------------------------------------
// FORMAT TEKS

void textCenter(int y,String Msg) {
  
  int center = int((Disp.width()-Disp.textWidth(Msg)) / 2);
  Disp.drawText(center,y,Msg);
  
}



//----------------------------------------------------------------------
// ANIMASI LOGO

void animLogoX() {

  static uint8_t x;
  static uint8_t s; // 0=in, 1=out
  static uint32_t pM;
  uint32_t cM = millis();

  if ((cM - pM) > 35) {
    if (s == 0 and x < 16) {
      pM = cM;
      x++;
    }
    if (s == 1 and x > 0) {
      pM = cM;
      x--;
    }
  }
  
  if ((cM - pM) > 5000 and x == 16) {
    s = 1;
  }

  if (x == 0 and s == 1) {    
    s = 0;
    tampilanjam = 1;
  }

  logoax(x - 16);
  logobx(32 - x);

}


void logoax(uint32_t x) {
  static const uint8_t logoax[] PROGMEM = {
    16, 16,
    B00000000,B00000000,
    B01100110,B01111110,
    B01100110,B01111110,
    B01100110,B01100110,
    B01100110,B01100110,
    B01111110,B01111110,
    B01111110,B01111110,
    B01100000,B01100000,
    B01100000,B01100000,
    B01111110,B01111110,
    B01111110,B01111110,
    B01100110,B00000110,
    B01100110,B00000110,
    B01111111,B11111110,
    B01111111,B11111110,
    B00000000,B00000000
  };
  Disp.drawBitmap(x, 0, logoax);
}

void logobx(uint32_t x) {
  static const uint8_t logobx[] PROGMEM = {
    16, 16,
    B00000000,B00000000,
    B01111111,B11111110,
    B01111111,B11111110,
    B00000000,B00000000,
    B00000000,B00000000,
    B01111110,B01100110,
    B01111110,B01100110,
    B00000110,B01100110,
    B00000110,B01100110,
    B01111110,B01100110,
    B01111110,B01100110,
    B01100110,B01100110,
    B01100110,B01100110,
    B01111111,B11111110,
    B01111111,B11111110,
    B00000000,B00000000
  };
  Disp.drawBitmap(x, 0, logobx);
}



//----------------------------------------------------------------------
// TOGGLE LED INTERNAL UNTUK STATUS MODE WIFI

void toggleLED() {

  digitalWrite(pin_led, !digitalRead(pin_led));
  server.send_P(200, "text/html", setwaktu);

}



//----------------------------------------------------------------------
// HJS589 P10 FUNGSI TAMBAHAN UNTUK NODEMCU ESP8266

void ICACHE_RAM_ATTR refresh() { 
  
  Disp.refresh();
  timer0_write(ESP.getCycleCount() + 32000);
  

}


void Disp_init() {
  
  Disp.start();
  timer0_attachInterrupt(refresh);
  //timer0_write(ESP.getCycleCount() + 40000);
  Disp.setBrightness(100);  
  Disp.clear();
  
}

void setBrightness(int bright) {
  
  Disp.setBrightness(bright);

}



//----------------------------------------------------------------------
// HJS589 P10 utility Function

void branding() {
  
  Disp.clear();
  Disp.setFont(Font3x5); 
  Disp.drawText(4,0, "GROBAK");
  Disp.drawText(2,9, ".NET");
  delay(1000);

  Disp.clear();
  Disp.setFont(Arial_Black_16); 
  Disp.drawText(-2,1, "JWS");
  delay(1000);

  Disp.clear();
  Disp.setFont(Font3x5); 
  textCenter(3, "VER.2");
  delay(1000);
  Disp.clear();
  
}



void handleSettingJwsUpdate() {

  timer0_detachInterrupt();

  String datajws = server.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(datajws);

  File configFileJws = SPIFFS.open(fileconfigjws, "w");
  if (!configFileJws) {
    Serial.println("Error opening JWS configFile for writing");
    return;
  }
  jObject.printTo(configFileJws);

  if (jObject.success()) {
    
    configFileJws.flush();
    configFileJws.close();
    Serial.println("Berhasil mengubah configFileJws");
    
    server.send(200, "application/json", "{\"status\":\"ok\"}");    

    delay(3000);
    ESP.restart();
  
  }  

}



void handleSettingWifiUpdate() {

  timer0_detachInterrupt();
  
  String data = server.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);

  File configFile = SPIFFS.open("/configwifi.json", "w");
  if (!configFile) {
    Serial.println("Error opening Wifi configFile for writing");
    return;
  }
  jObject.printTo(configFile);

  if (jObject.success()) {

    configFile.flush();
    configFile.close();
    Serial.println("Berhasil mengubah configFileWifi");
    
    server.send(200, "application/json", "{\"status\":\"ok\"}");

    delay(3000);
    ESP.restart();

  } 

}



void loadWifiConfig(const char *fileconfigwifi, ConfigWifi &configwifi) {

  timer0_detachInterrupt();
  
  File configFileWifi = SPIFFS.open(fileconfigwifi, "r");
  
  if (!configFileWifi) {
    Serial.println("Gagal membuka fileconfigwifi untuk dibaca");
    return;
  }

  size_t size = configFileWifi.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFileWifi.readBytes(buf.get(), size);

  DynamicJsonBuffer jsonBuffer;
  JsonObject& jObject = jsonBuffer.parseObject(buf.get());

  if (!jObject.success()) {
    Serial.println("Gagal untuk parse config file");
    return;
  }

  strlcpy(configwifi.wifissid, jObject["wifissid"] | "grobak.net", sizeof(configwifi.wifissid));
  strlcpy(configwifi.wifipassword, jObject["wifipassword"] | "", sizeof(configwifi.wifipassword));

  configFileWifi.flush();
  configFileWifi.close();

}



void loadJwsConfig(const char *fileconfigjws, Config &config) {

  timer0_detachInterrupt();
  
  File configFileJws = SPIFFS.open(fileconfigjws, "r");
  
  if (!configFileJws) {
    Serial.println("Gagal membuka fileconfigjws untuk dibaca");
    return;
  }

  size_t size = configFileJws.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFileJws.readBytes(buf.get(), size);

  DynamicJsonBuffer jsonBuffer;
  JsonObject& jObject = jsonBuffer.parseObject(buf.get());

  if (!jObject.success()) {
    Serial.println("Gagal parse fileconfigjws");
    return;
  }

  config.iqmhs = jObject["iqmhs"];
  config.iqmhd = jObject["iqmhd"];
  config.iqmha = jObject["iqmha"];
  config.iqmhm = jObject["iqmhm"];
  config.iqmhi = jObject["iqmhi"];
  config.ihti = jObject["ihti"];
  config.latitude = jObject["latitude"];
  config.longitude = jObject["longitude"];
  strlcpy(config.merek, jObject["merek"] | "GROBAK", sizeof(config.merek));
  strlcpy(config.info1, jObject["info1"] | "ELEKTRON", sizeof(config.info1));
  strlcpy(config.info2, jObject["info2"] | " MART ", sizeof(config.info2));

  configFileJws.flush();
  configFileJws.close();

}
