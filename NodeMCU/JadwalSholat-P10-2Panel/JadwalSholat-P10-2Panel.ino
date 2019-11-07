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
- HJS589(DMD porting for ESP8266 by dmk007)
  < DMD (https://rweather.github.io/arduino-projects/classDMD.html)
- PrayerTime : https://github.com/asmaklad/Arduino-Prayer-Times
- RTC DS3231 : https://github.com/Makuna/Rtc
- ArduinoJson V6 : https://github.com/bblanchon/ArduinoJson

email : bonny@grobak.net - www.grobak.net - www.elektronmart.com

Updated : 16 September 2019
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

#include <fonts/ElektronMart6x8.h>
#include <fonts/ElektronMart6x16.h>
#include <fonts/ElektronMart5x6.h>
#include <fonts/ElektronMartArabic6x16.h>
#include <fonts/ElektronMartArabic5x6.h>

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
  int durasiadzan;
  int ihti; // Koreksi Waktu Menit Jadwal Sholat
  float latitude;
  float longitude;
  int zonawaktu;  
};

struct ConfigInfo {
  char nama[64];
  char info1[512];
  char info2[512];
};

int iqmh;
int menitiqmh;
int detikiqmh = 60;

struct ConfigWifi {
  char wifissid[64];
  char wifipassword[64];
};

struct ConfigDisp {
  int cerah;
};



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
#define DISPLAYS_WIDE 2
#define DISPLAYS_HIGH 1
HJS589 Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);  // Jumlah Panel P10 yang digunakan (KOLOM,BARIS)


//WEB Server
ESP8266WebServer server(80);

const char* password = "elektronmart";
const char* mySsid = "JWSP10"; //kalau gagal konek

IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress netmask(255, 255, 255, 0);


const char *fileconfigdisp = "/configdisp.json";
ConfigDisp configdisp;

const char *fileconfigjws = "/configjws.json";
Config config;

const char *fileconfiginfo = "/configinfo.json";
ConfigInfo configinfo;

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
    XML+="<rDurasiAdzan>";
    XML+= config.durasiadzan;
    XML+="</rDurasiAdzan>";
    XML+="<rIhti>";
    XML+= config.ihti;
    XML+="</rIhti>";
    XML+="<rLatitude>";
    XML+= config.latitude;
    XML+="</rLatitude>";
    XML+="<rLongitude>";
    XML+= config.longitude;
    XML+="</rLongitude>";
    XML+="<rZonaWaktu>";
    XML+= config.zonawaktu;
    XML+="</rZonaWaktu>";
    XML+="<rNama>";
    XML+= configinfo.nama;
    XML+="</rNama>";
    XML+="<rInfo1>";
    XML+= configinfo.info1;
    XML+="</rInfo1>";
    XML+="<rInfo2>";
    XML+= configinfo.info2;
    XML+="</rInfo2>";
    XML+="<rCerah>";
    XML+= configdisp.cerah;
    XML+="</rCerah>";
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
// HJS589 P10 FUNGSI TAMBAHAN UNTUK NODEMCU ESP8266

void ICACHE_RAM_ATTR refresh() { 
  
  Disp.refresh();
  timer0_write(ESP.getCycleCount() + 80000);  

}

void Disp_init() {
  
  Disp.start();
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(refresh);
  timer0_write(ESP.getCycleCount() + 80000);
  interrupts();
  Disp.clear();
  
}



//----------------------------------------------------------------------
// HJS589 P10 FUNGSI TAMBAHAN UNTUK NODEMCU ESP8266

void LoadDataAwal() {


  if (config.iqmhs == 0) {
    config.iqmhs = 12;    
  }

  if (config.iqmhd == 0) {
    config.iqmhd = 8;    
  }

  if (config.iqmha == 0) {
    config.iqmha = 6;    
  }

  if (config.iqmhm == 0) {
    config.iqmhm = 5;    
  }

  if (config.iqmhi == 0) {
    config.iqmhi = 5;    
  }

  if (config.durasiadzan == 0) {
    config.durasiadzan = 1;    
  }

  if (config.ihti == 0) {
    config.ihti = 2;    
  }

  if (config.latitude == 0) {
    config.latitude = -6.16;    
  }

  if (config.longitude == 0) {
    config.longitude = 106.61;    
  }

  if (config.zonawaktu == 0) {
    config.zonawaktu = 7;    
  }

  if (strlen(configinfo.nama) == 0) {
    strlcpy(configinfo.nama, "MASJID AL KAUTSAR", sizeof(configinfo.nama));
  }

  if (strlen(configinfo.info1) == 0) {
    strlcpy(configinfo.info1, "www.grobak.net", sizeof(configinfo.info1));
  }

  if (strlen(configinfo.info2) == 0) {
    strlcpy(configinfo.info2, "www.elektronmart.com", sizeof(configinfo.info2));
  }

  if (strlen(configwifi.wifissid) == 0) {
    strlcpy(configwifi.wifissid, "grobak.net", sizeof(configwifi.wifissid));
  }

  if (strlen(configwifi.wifipassword) == 0) {
    strlcpy(configwifi.wifipassword, "password", sizeof(configwifi.wifipassword));
  }

  if (configdisp.cerah == 0) {
    configdisp.cerah = 100;    
  }
  
}



//----------------------------------------------------------------------
// SETUP

void setup() {

  Serial.begin(9600);

  //Buzzer
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  delay(50);

  //RTC D3231

  int rtn = I2C_ClearBus(); // clear the I2C bus first before calling Wire.begin()
  if (rtn != 0) {
    Serial.println(F("I2C bus error. Could not clear"));
    if (rtn == 1) {
      Serial.println(F("SCL clock line held low"));
    } else if (rtn == 2) {
      Serial.println(F("SCL clock line held low by slave clock stretch"));
    } else if (rtn == 3) {
      Serial.println(F("SDA data line held low"));
    }
  } else { // bus clear, re-enable Wire, now can start Wire Arduino master
    Wire.begin();
  }
  
  Rtc.Begin();

  if (!Rtc.GetIsRunning()) {
    
    Rtc.SetIsRunning(true);
    
  }
  
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
  
  Serial.println("Setup RTC selesai");


  //WIFI

  pinMode(pin_led, OUTPUT);

  SPIFFS.begin();
  
  loadWifiConfig(fileconfigwifi, configwifi);
  loadJwsConfig(fileconfigjws, config);
  loadInfoConfig(fileconfiginfo, configinfo);
  loadDispConfig(fileconfigdisp, configdisp);

  LoadDataAwal();
   
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

  server.on("/toggle", toggleLED);

  server.on("/setwifi", []() {
    server.send_P(200, "text/html", setwifi);
  });  
  
  server.on("/settingwifi", HTTP_POST, handleSettingWifiUpdate); 
  
  server.on("/setjws", []() {
    server.send_P(200, "text/html", setjws);
  });
  
  server.on("/settingjws", HTTP_POST, handleSettingJwsUpdate);

  server.on("/setinfo", []() {
    server.send_P(200, "text/html", setinfo);
  });
  
  server.on("/settinginfo", HTTP_POST, handleSettingInfoUpdate);

  server.on("/setdisplay", []() {
    server.send_P(200, "text/html", setdisplay);
  });  

  server.on("/settingdisp", HTTP_POST, handleSettingDispUpdate);

  server.on ( "/xml", handleXML) ;  
  
  server.begin();
  Serial.println("HTTP server started");  
  

  //Buzzer

  BuzzerPendek();

  //DMD
  Disp_init();
  
  Disp.setBrightness(configdisp.cerah);

  // BRANDING
  branding();
  
}



void loadDispConfig(const char *fileconfigdisp, ConfigDisp &configdisp) {

  File configFileDisp = SPIFFS.open(fileconfigdisp, "r");

  if (!configFileDisp) {
    Serial.println("Gagal membuka fileconfigdisp untuk dibaca");
    return;
  }

  size_t size = configFileDisp.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFileDisp.readBytes(buf.get(), size);

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    Serial.println("Gagal parse fileconfigdisp");
    return;
  }
  
  configdisp.cerah = doc["cerah"];

  configFileDisp.close();

}



void handleSettingDispUpdate() {

  timer0_detachInterrupt();
  
  String datadisp = server.arg("plain");
  
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, datadisp);

  File configFileDisp = SPIFFS.open(fileconfigdisp, "w");
  
  if (!configFileDisp) {
    Serial.println("Gagal membuka Display configFile untuk ditulis");
    return;
  }
  
  serializeJson(doc, configFileDisp);

  if (error) {
    
    Serial.print(F("deserializeJson() gagal kode sebagai berikut: "));
    Serial.println(error.c_str());
    return;
    
  } else {
    
    configFileDisp.close();
    Serial.println("Berhasil mengubah configFileDisp");

    server.send(200, "application/json", "{\"status\":\"ok\"}");

    loadDispConfig(fileconfigdisp, configdisp);
    
    delay(500);
    Disp_init();

    Disp.setBrightness(configdisp.cerah);
  
  }  

}






void loadJwsConfig(const char *fileconfigjws, Config &config) {

  File configFileJws = SPIFFS.open(fileconfigjws, "r");
  
  if (!configFileJws) {
    Serial.println("Gagal membuka fileconfigjws untuk dibaca");
    return;
  }

  size_t size = configFileJws.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFileJws.readBytes(buf.get(), size);

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    Serial.println("Gagal parse fileconfigjws");
    return;
  }

  config.iqmhs = doc["iqmhs"];
  config.iqmhd = doc["iqmhd"];
  config.iqmha = doc["iqmha"];
  config.iqmhm = doc["iqmhm"];
  config.iqmhi = doc["iqmhi"];
  config.durasiadzan = doc["durasiadzan"];
  config.ihti = doc["ihti"];
  config.latitude = doc["latitude"];
  config.longitude = doc["longitude"];
  config.zonawaktu = doc["zonawaktu"];

  configFileJws.close();

}



void handleSettingJwsUpdate() {

  timer0_detachInterrupt();

  String datajws = server.arg("plain");
  
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, datajws);

  File configFileJws = SPIFFS.open(fileconfigjws, "w");
  
  if (!configFileJws) {
    Serial.println("Gagal membuka JWS configFile untuk ditulis");
    return;
  }
  
  serializeJson(doc, configFileJws);

  if (error) {
    
    Serial.print(F("deserializeJson() gagal kode sebagai berikut: "));
    Serial.println(error.c_str());
    return;
    
  } else {
    
    configFileJws.close();
    Serial.println("Berhasil mengubah configFileJws");
    
    server.send(200, "application/json", "{\"status\":\"ok\"}");    
    
    loadJwsConfig(fileconfigjws, config);
    
    delay(500);
    Disp_init();
  
  }  

}



void loadInfoConfig(const char *fileconfiginfo, ConfigInfo &configinfo) {

  File configFileInfo = SPIFFS.open(fileconfiginfo, "r");
  
  if (!configFileInfo) {
    Serial.println("Gagal membuka fileconfiginfo untuk dibaca");
    return;
  }

  size_t size = configFileInfo.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFileInfo.readBytes(buf.get(), size);

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    Serial.println("Gagal parse fileconfiginfo");
    return;
  }

  strlcpy(configinfo.nama, doc["nama"] | "MASJID AL KAUTSAR", sizeof(configinfo.nama));  // Set awal Nama
  strlcpy(configinfo.info1, doc["info1"] | "www.grobak.net", sizeof(configinfo.info1));  // Set awal Info1 
  strlcpy(configinfo.info2, doc["info2"] | "www.elektronmart.com", sizeof(configinfo.info2));  // Set awal Info2

  configFileInfo.close();

}



void handleSettingInfoUpdate() {

  timer0_detachInterrupt();

  String datainfo = server.arg("plain");
  
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, datainfo);

  File configFileInfo = SPIFFS.open(fileconfiginfo, "w");
  
  if (!configFileInfo) {
    Serial.println("Gagal membuka Info configFile untuk ditulis");
    return;
  }
  
  serializeJson(doc, configFileInfo);

  if (error) {
    
    Serial.print(F("deserializeJson() gagal kode sebagai berikut: "));
    Serial.println(error.c_str());
    return;
    
  } else {
    
    configFileInfo.close();
    Serial.println("Berhasil mengubah configFileInfo");
    
    server.send(200, "application/json", "{\"status\":\"ok\"}");    
    
    loadInfoConfig(fileconfiginfo, configinfo);
    
    delay(500);
    Disp_init();
  
  }  

}



void loadWifiConfig(const char *fileconfigwifi, ConfigWifi &configwifi) {

  File configFileWifi = SPIFFS.open(fileconfigwifi, "r");
  
  if (!configFileWifi) {
    Serial.println("Gagal membuka fileconfigwifi untuk dibaca");
    return;
  }

  size_t size = configFileWifi.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFileWifi.readBytes(buf.get(), size);

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    Serial.println("Gagal untuk parse config file");
    return;
  }

  strlcpy(configwifi.wifissid, doc["wifissid"] | "grobak.net", sizeof(configwifi.wifissid));
  strlcpy(configwifi.wifipassword, doc["wifipassword"] | "12345", sizeof(configwifi.wifipassword));

  configFileWifi.close();

}



void handleSettingWifiUpdate() {

  timer0_detachInterrupt();
  
  String data = server.arg("plain");

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, data);

  File configFile = SPIFFS.open("/configwifi.json", "w");
  if (!configFile) {
    Serial.println("Error opening Wifi configFile for writing");
    return;
    
  }
  
  serializeJson(doc, configFile);

  if (error) {
    
    Serial.print(F("deserializeJson() gagal kode sebagai berikut: "));
    Serial.println(error.c_str());
    return;
    
  } else {

    configFile.close();
    Serial.println("Berhasil mengubah configFileWifi");
    
    server.send(200, "application/json", "{\"status\":\"ok\"}");
    loadWifiConfig(fileconfigwifi, configwifi);

    delay(500);
    Disp_init();

  } 

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
  
  if (tampilanjam > 4) {
    tampilanjam = 0;
  }

  switch(tampilanjam) {

    case 0 :
      animLogoX();      
      AlarmSholat();
      break;  
      
    case 1 :
      JamJatuhPulse(0);
      TampilJadwal();
      AlarmSholat();
      break;

    case 2 :
      TeksJalanNama();
      AlarmSholat();
      break;
      
    case 3 :
      TeksJalanInfo1();
      AlarmSholat();
      break;
      
    case 4 :
      TeksJalanInfo2();
      AlarmSholat();
      break;
  }

}


//----------------------------------------------------------------------
//MODE TAMPILAN JAM


void JamJatuhPulse(int x) {

  RtcDateTime now = Rtc.GetDateTime();
  char jam[3];
  char menit[3];
  char detik[3];
  sprintf(jam,"%02d", now.Hour());
  sprintf(menit,"%02d", now.Minute());
  sprintf(detik,"%02d", now.Second());

  static boolean huruf;
  
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

  if(cM - pM > 35) { 
    if(d == 0 and y < 20) {
      pM=cM;
      y++;
    }
    if(d  == 1 and y > 0) {
      pM=cM;
      y--;
    }    
  }
  
  if(cM - pM > 10000 and y == 20) {
    d=1;
  }
  
  if(y == 20) {
    Disp.drawRect(x+15,3+pulse,x+18,11-pulse,0,1);       
  }
  
  if(y < 20) {
    Disp.drawRect(x+15,3,x+18,11,0,0);
  }
   
  if(y == 0 and d == 1) {
    d=0;
    huruf = !huruf;
  }
  
  //JAM
  if (huruf) {
    Disp.setFont(ElektronMart6x16);
  } else {
    Disp.setFont(ElektronMartArabic6x16);
  }  
  Disp.drawText(x+1, y - 20, jam);

  //MENIT
  if (huruf) {
    Disp.setFont(ElektronMart5x6);
  } else {
    Disp.setFont(ElektronMartArabic5x6);
  }  
  Disp.drawText(x+20, y - 20, menit);

  //DETIK          
  if (huruf) {
    Disp.setFont(ElektronMart5x6);
  } else {
    Disp.setFont(ElektronMartArabic5x6);
  }  
  Disp.drawText(x+20, y - 20 + 8, detik);
  
    
}

void JamJatuhPulse2(int x) {

  RtcDateTime now = Rtc.GetDateTime();
  char jam[3];
  char menit[3];
  //char detik[3];
  sprintf(jam,"%02d", now.Hour());
  sprintf(menit,"%02d", now.Minute());
  //sprintf(detik,"%02d", now.Second());

  static boolean huruf;
  
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

  if(cM - pM > 35) { 
    if(d == 0 and y < 20) {
      pM=cM;
      y++;
    }
    if(d  == 1 and y > 0) {
      pM=cM;
      y--;
    }    
  }
  
  if(cM - pM > 10000 and y == 20) {
    d=1;
  }
  
  if(y == 20) {
    Disp.drawRect(x+14,3+pulse,x+17,11-pulse,0,1);       
  }
  
  if(y < 20) {
    Disp.drawRect(x+14,3,x+18,17,0,0);
  }
   
  if(y == 0 and d == 1) {
    d=0;
    huruf = !huruf;
  }
  
  //JAM
  if (huruf) {
    Disp.setFont(ElektronMart6x16);
  } else {
    Disp.setFont(ElektronMartArabic6x16);
  }  
  Disp.drawText(x+1, y - 20, jam);

  //MENIT
  if (huruf) {
    Disp.setFont(ElektronMart6x16);
  } else {
    Disp.setFont(ElektronMartArabic6x16);
  }  
  Disp.drawText(x+18, y - 20, menit);
 
    
}


void TampilJamKecil() {

  static uint32_t pM;
  static uint32_t pMJam;
  uint32_t cM = millis();
  static boolean kedip;
  
  RtcDateTime now = Rtc.GetDateTime();
  char jam[3];
  char menit[3];
  char detik[3];
  
  if (cM - pMJam >= 1000) {
   
    pMJam = cM;
    
    //JAM
    sprintf(jam,"! %02d:%02d:%02d", now.Hour(), now.Minute(), now.Second());
    //sprintf(jam,"%02d:%02d", now.Hour(), now.Minute());
    Disp.setFont(ElektronMart5x6);
    textCenter(0,jam);
        
  }
 
}



uint8_t tampilanjadwal;

void TampilJadwal() {

  if (tampilanjadwal > 2) {
    tampilanjadwal = 0;
  }
  
  switch(tampilanjadwal) {

    case 0 :
      TampilTanggal();
      AlarmSholat();
      break;
      
    case 1 :
      TampilSuhu();
      AlarmSholat();
      break;

    case 2 :
      TampilJadwalSholat();
      AlarmSholat();
      break;

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

  if (cM - pM > 3000) {
    pM = cM;
    d++;
  
    sprintf(hari, "%s", weekDay[now.DayOfWeek()]);
    sprintf(tanggal, "%02d %s", now.Day(), monthYear[now.Month()]);  
    
    Disp.setFont(ElektronMart5x6);
    textCenter1PKanan(0,hari);
    textCenter1PKanan(8,tanggal);

    if (d >= 2) {
      d = 0;
      Disp.drawRect(31,0,64,15,0,0);
      tampilanjadwal = 1;
    }
  } 
  
}



//----------------------------------------------------------------------
// TAMPILKAN SUHU

void TampilSuhu(){

  RtcTemperature temp = Rtc.GetTemperature();
  int celsius = temp.AsFloatDegC();
  char suhu[2];
  int koreksisuhu = 2; // Perkiraan selisih suhu ruangan dan luar ruangan

  static uint8_t d;
  static uint32_t pM;
  uint32_t cM = millis();

  if (cM - pM > 3000) {
    pM = cM;
    d++;
  
    Disp.setFont(ElektronMart5x6);    
    textCenter1PKanan(0, "SUHU");
    sprintf(suhu,"%dC*",celsius - koreksisuhu);
    textCenter1PKanan(8, suhu);

    if (d >= 2) {
      d = 0;
      Disp.drawRect(31,0,64,15,0,0);
      tampilanjadwal = 2;
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
  
  set_calc_method(Karachi);
  set_asr_method(Shafii);
  set_high_lats_adjust_method(AngleBased);
  set_fajr_angle(20);
  set_isha_angle(18);  

  get_prayer_times(tahun, bulan, tanggal, config.latitude, config.longitude, config.zonawaktu, times);

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
  
  if (cM - pM >= 3000) {
    
    pM = cM;    
    Disp.drawRect(31,0,64,15,0,0);

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
    
    Disp.setFont(ElektronMart5x6);
    textCenter1PKanan(0,sholat);
    textCenter1PKanan(8,jam);
    
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
      Disp.drawRect(31,0,64,15,0,0);
      Disp.setFont(ElektronMart5x6);
      textCenter1PKanan(0,"TANBIH");
      textCenter1PKanan(8,jam);
      
      if (i > 8) {
        i = 0;
        Disp.drawRect(31,0,64,15,0,0);
        tampilanjadwal = 0;
        Disp.clear();
        tampilanjam = 2;
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
  int adzan = config.durasiadzan * 60000;

  JadwalSholat();
  int hours, minutes, seconds;

  // Tanbih Imsak
  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes + config.ihti;

  if (minutes < 10) {
    minutes = 60 - minutes;
    hours --;
  } else {
    minutes = minutes - 10 ;
  }

  if (Hor == hours && Min == minutes) {
    BuzzerPendek();
    Disp.clear();
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "TANBIH");
    textCenter(8, "IMSAK");
    delay(adzan);
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
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "ADZAN");
    textCenter(8, "SUBUH");
    delay(adzan);
    Disp.clear();
    iqmh = config.iqmhs;
    menitiqmh = iqmh;
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
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "ADZAN");
    textCenter(8, "DZUHUR");
    delay(adzan);
    Disp.clear();
    iqmh = config.iqmhd;
    menitiqmh = iqmh;
    tampilanutama = 1;

  } else if (Hor == hours && Min == minutes && Hari == 5) {
    BuzzerPendek();
    Disp.clear();
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "ADZAN");
    textCenter(8, "JUM'AT");
    delay(adzan);

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
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "ADZAN");
    textCenter(8, "ASHAR");
    delay(adzan);
    Disp.clear();
    iqmh = config.iqmha;
    menitiqmh = iqmh;
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
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "ADZAN");
    textCenter(8, "MAGHRIB");
    delay(adzan);
    Disp.clear();
    iqmh = config.iqmhm;
    menitiqmh = iqmh;
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
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "ADZAN");
    textCenter(8, "ISYA'");
    delay(adzan);
    Disp.clear();
    iqmh = config.iqmhi;
    menitiqmh = iqmh;
    tampilanutama = 1;
  }

}


//----------------------------------------------------------------------
// HITUNG MUNDUR WAKTU SETELAH ADZAN SAMPAI MULAI IQOMAH

void Iqomah() {  

  static uint32_t pMIqmh;
  uint32_t cM = millis();
  static char hitungmundur[6];

  Disp.setFont(ElektronMart5x6);
  textCenter(0, "IQOMAH");

  if (detikiqmh == 60) {
    detikiqmh = 0;
  }  

  if(cM - pMIqmh >= 1000) {
    
    pMIqmh = cM;    
    detikiqmh--;
    
    if (menitiqmh == 0 && detikiqmh == 0){
        Disp.clear();
        Disp.setFont(ElektronMart5x6);
        textCenter(0, "LURUSKAN DAN");
        textCenter(8, "RAPATKAN SHAF");
        BuzzerPanjang();
        detikiqmh = 59;
        Disp.clear();
        tampilanutama = 0;
    }

    if (detikiqmh < 0) {
      detikiqmh = 59;
      menitiqmh--;            
    }
    
  }

  sprintf(hitungmundur, "%02d:%02d", menitiqmh, detikiqmh);
  Disp.setFont(ElektronMart6x8);
  textCenter(8, hitungmundur);  
  
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



//-------------------------------------------------------
// TAMPILKAN SCROLLING TEKS NAMA

static char *nama[] = {configinfo.nama};

void TeksJalanNama() {

  static uint32_t pM;
  static uint32_t x;
  static uint32_t Speed = 50;
  int width = Disp.width();
  Disp.setFont(ElektronMart6x8);
  textCenter(0,"#");
  Disp.drawRect(0,6,Disp.width(),6,1,1);
  int fullScroll = Disp.textWidth(nama[0]) + width;
  if((millis() - pM) > Speed) { 
    pM = millis();
    if (x < fullScroll) {
      ++x;
    } else {
      x = 0;
      Disp.clear();
      tampilanjam = 3;
    }
    Disp.drawText(width - x, 8, nama[0]);
  }  

}



//-------------------------------------------------------
// TAMPILKAN SCROLLING TEKS INFO1

static char *info1[] = {configinfo.info1};

void TeksJalanInfo1() {

  TampilJamKecil();
  
  static uint32_t pM;
  static uint32_t x;
  static uint32_t Speed = 50;
  int width = Disp.width();
  Disp.setFont(ElektronMart6x8);
  int fullScroll = Disp.textWidth(info1[0]) + width;
  if((millis() - pM) > Speed) { 
    pM = millis();
    if (x < fullScroll) {
      ++x;
    } else {
      x = 0;
      tampilanjam = 4;
      return;
    }
    Disp.drawText(width - x, 8, info1[0]);
  }  

}



//-------------------------------------------------------
// TAMPILKAN SCROLLING TEKS INFO2

static char *info2[] = {configinfo.info2};

void TeksJalanInfo2() {

  TampilJamKecil();

  static uint32_t pM;
  static uint32_t x;
  static uint32_t Speed = 50;
  int width = Disp.width();
  Disp.setFont(ElektronMart6x8);
  int fullScroll = Disp.textWidth(info2[0]) + width;
  if((millis() - pM) > Speed) { 
    pM = millis();
    if (x < fullScroll) {
      ++x;
    } else {
      x = 0;
      Disp.clear();
      tampilanjam = 0;
      return;
    }
    Disp.drawText(width - x, 8, info2[0]);
  }

}



//----------------------------------------------------------------------
// FORMAT TEKS

void textCenter(int y,String Msg) {
  
  int center = int((Disp.width()-Disp.textWidth(Msg)) / 2);
  Disp.drawText(center,y,Msg);
  
}

void textCenter1PKanan(int y,String Msg) {
  
  int center = ((Disp.width()-Disp.textWidth(Msg)) / 2) + (Disp.width() / 4);
  Disp.drawText(center,y,Msg);
  
}



//----------------------------------------------------------------------
// ANIMASI LOGO

void animLogoX() {

  static uint8_t x;
  static uint8_t s; // 0=in, 1=out
  static uint32_t pM;
  uint32_t cM = millis();

  JamJatuhPulse2(16);

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
  
  if ((cM - pM) > 10000 and x == 16) {    
    s = 1;    
  }

  if (x == 0 and s == 1) {
    Disp.clear();
    s = 0;    
    tampilanjam = 1;    
  }

  
  logoax(x - 16);
  logobx(Disp.width() - x);

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
// HJS589 P10 utility Function

void branding() {
  
  Disp.clear();
  Disp.setFont(ElektronMart5x6); 
  Disp.drawText(20,0, "GROBAK");
  Disp.drawText(18,7, ".NET");
  delay(1000);

  Disp.clear();
  Disp.setFont(ElektronMart6x8); 
  textCenter(0, "#");
  textCenter(8, "JWS");
  delay(1000);

  Disp.clear();
  Disp.setFont(ElektronMart5x6); 
  textCenter(3, "VER.3");
  delay(1000);
  Disp.clear();
  
}



//----------------------------------------------------------------------
// I2C_ClearBus menghindari gagal baca RTC (nilai 00 atau 165)

int I2C_ClearBus() {
  
#if defined(TWCR) && defined(TWEN)
  TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly
#endif

  pinMode(SDA, INPUT_PULLUP); // Make SDA (data) and SCL (clock) pins Inputs with pullup.
  pinMode(SCL, INPUT_PULLUP);

  delay(2500);  // Wait 2.5 secs. This is strictly only necessary on the first power
  // up of the DS3231 module to allow it to initialize properly,
  // but is also assists in reliable programming of FioV3 boards as it gives the
  // IDE a chance to start uploaded the program
  // before existing sketch confuses the IDE by sending Serial data.

  boolean SCL_LOW = (digitalRead(SCL) == LOW); // Check is SCL is Low.
  if (SCL_LOW) { //If it is held low Arduno cannot become the I2C master. 
    return 1; //I2C bus error. Could not clear SCL clock line held low
  }

  boolean SDA_LOW = (digitalRead(SDA) == LOW);  // vi. Check SDA input.
  int clockCount = 20; // > 2x9 clock

  while (SDA_LOW && (clockCount > 0)) { //  vii. If SDA is Low,
    clockCount--;
  // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
    pinMode(SCL, INPUT); // release SCL pullup so that when made output it will be LOW
    pinMode(SCL, OUTPUT); // then clock SCL Low
    delayMicroseconds(10); //  for >5uS
    pinMode(SCL, INPUT); // release SCL LOW
    pinMode(SCL, INPUT_PULLUP); // turn on pullup resistors again
    // do not force high as slave may be holding it low for clock stretching.
    delayMicroseconds(10); //  for >5uS
    // The >5uS is so that even the slowest I2C devices are handled.
    SCL_LOW = (digitalRead(SCL) == LOW); // Check if SCL is Low.
    int counter = 20;
    while (SCL_LOW && (counter > 0)) {  //  loop waiting for SCL to become High only wait 2sec.
      counter--;
      delay(100);
      SCL_LOW = (digitalRead(SCL) == LOW);
    }
    if (SCL_LOW) { // still low after 2 sec error
      return 2; // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
    }
    SDA_LOW = (digitalRead(SDA) == LOW); //   and check SDA input again and loop
  }
  if (SDA_LOW) { // still low
    return 3; // I2C bus error. Could not clear. SDA data line held low
  }

  // else pull SDA line low for Start or Repeated Start
  pinMode(SDA, INPUT); // remove pullup.
  pinMode(SDA, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
  // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
  /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
  delayMicroseconds(10); // wait >5uS
  pinMode(SDA, INPUT); // remove output low
  pinMode(SDA, INPUT_PULLUP); // and make SDA high i.e. send I2C STOP control.
  delayMicroseconds(10); // x. wait >5uS
  pinMode(SDA, INPUT); // and reset pins as tri-state inputs which is the default state on reset
  pinMode(SCL, INPUT);
  return 0; // all ok
}
