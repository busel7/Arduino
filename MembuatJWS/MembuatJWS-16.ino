/*
 * JADWAL WAKTU SHOLAT MENGGUNAKAN NODEMCU ESP8266, LED P10, RTC DS3231, BUZZER
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

Project Git
- https://github.com/busel7/Arduino

Eksternal Library
- HJS589(DMD porting for ESP8266 by dmk007)
  < DMD (https://rweather.github.io/arduino-projects/classDMD.html)
- PrayerTime : https://github.com/asmaklad/Arduino-Prayer-Times
- RTC DS3231 : https://github.com/Makuna/Rtc
- ArduinoJson V6 : https://github.com/bblanchon/ArduinoJson

Diskusi Grup Untuk1000Masjid : https://www.facebook.com/groups/761058907424496/

Tools : http://dotmatrixtool.com

www.grobak.net - www.elektronmart.com

Updated : 6 Oktober 2019
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <Wire.h>
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);

#include <HJS589.h>

#include <fonts/ElektronMart5x6.h>
#include <fonts/ElektronMart6x8.h>
#include <fonts/ElektronMart6x12.h>
#include <fonts/ElektronMart6x16.h>
#include <fonts/ElektronMartArabic5x6.h>
#include <fonts/ElektronMartArabic6x16.h>


#include <PrayerTimes.h>
#include "WebPage.h"


//----------------------------
// SETUP DMD HJS589

#define DISPLAYS_WIDE 2 
#define DISPLAYS_HIGH 1
HJS589 Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);  // Jumlah Panel P10 yang digunakan (KOLOM,BARIS)



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
  interrupts();
  Disp.clear();
  
}


//SETUP RTC
//year, month, date, hour, min, sec and week-day(Senin 0 sampai Ahad 6)
//DateTime dt(2018, 12, 20, 16, 30, 0, 3);
RtcDateTime now;
char weekDay[][7] = {"AHAD", "SENIN", "SELASA", "RABU", "KAMIS", "JUM'AT", "SABTU", "AHAD"}; // array hari, dihitung mulai dari senin, hari senin angka nya =0,
char monthYear[][4] = { "DES", "JAN", "FEB", "MAR", "APR", "MEI", "JUN", "JUL", "AGU", "SEP", "OKT", "NOV", "DES" };


// PrayerTimes
double times[sizeof(TimeName)/sizeof(char*)];
int ihti = 2;

int detikiqmh;
int menitiqmh;

// BUZZER
const int buzzer = 3; // Pin GPIO Buzzer - RX


// WIFI

// LED Internal
uint8_t pin_led = 2;

//WEB Server
ESP8266WebServer server(80);

// Sebagai Station
const char* wifissid = "grobak.net"; //kalau gagal konek
const char* wifipassword = "";

// Sebagai AccessPoint
const char* ssid = "JWSP10"; //kalau gagal konek
const char* password = "elektronmart";

IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress netmask(255, 255, 255, 0);



void wifiConnect() {

  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  delay(1000);

  Serial.println("Wifi Sation Mode");
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifissid, wifipassword);
  
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
    WiFi.softAP(ssid, password);
    digitalWrite(pin_led, LOW);
    
  }

  //Serial.println("");
  WiFi.printDiag(Serial);
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

}



//----------------------------------------------------------------------
// XML UNTUK JEMBATAN DATA MESIN DENGAN WEB

void buildXML(){

  RtcDateTime now = Rtc.GetDateTime();
  RtcTemperature temp = Rtc.GetTemperature();
  XML="<?xml version='1.0'?>";
  XML+="<t>";
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
  XML+="</t>"; 
}

void handleXML(){
  buildXML();
  server.send(200,"text/xml",XML);
}





//----------------------------------------------------------------------
// SETUP

void setup() {

  Serial.begin(115200);

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

  //Rtc.SetDateTime(RtcDateTime(2019, 10, 11, 22, 20, 50));
  
  Serial.println("Setup RTC selesai");


  //WIFI

  pinMode(pin_led, OUTPUT);

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


  server.on ( "/xml", handleXML) ; // http://192.168.4.1/xml

  server.begin();
  Serial.println("HTTP server started");  

  //DMD
  Disp_init();
  
  Disp.setBrightness(20);

  
}




//----------------------------------------------------------------------
// LOOP

uint8_t tmputama;

void loop() {

  server.handleClient();

  switch(tmputama) {

    case 0 :      
      tampiljws();
      break;

    case 1 :
      iqomah();
      break;

    
  }
  
}


uint8_t tmpjws;

void tampiljws() {

  switch(tmpjws) {

    case 0 :
      animLogoX();
      AlarmSholat();
      break;

    case 1 :
      JamBesar(1);
      tampilinfo();
      AlarmSholat();
      break;

    case 2 :
      TeksBerjalanKananKiri();
      AlarmSholat();
      break;
    
  }
  
}



uint8_t tmpinfo;

void tampilinfo() {

  switch(tmpinfo) {

    case 0 :
      TampilTanggal();
      break;

    case 1 :
      TampilSuhu();
      break;

    case 2 :
      TampilJadwalSholat();
      break;
    
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
  int durasiadzan = 3 * 60000;

  JadwalSholat();

  int hours, minutes, seconds;

  // Tanbih Imsak
  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes + ihti;

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
    delay(durasiadzan);
    Disp.clear();

  }

  // Subuh
  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes + ihti;

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
    delay(durasiadzan);
    menitiqmh = 12;
    tmputama = 1;
    Disp.clear();
    
  }


  // Dzuhur
  get_float_time_parts(times[2], hours, minutes);
  minutes = minutes + ihti;

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
    delay(durasiadzan);    
    Disp.clear();
    menitiqmh = 7;
    tmputama = 1;

  } else if (Hor == hours && Min == minutes && Hari == 5) {
    
    BuzzerPendek();
    Disp.clear();
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "ADZAN");
    textCenter(8, "JUM'AT");
    delay(durasiadzan);

  }


  // Ashar
  get_float_time_parts(times[3], hours, minutes);
  minutes = minutes + ihti;

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
    delay(durasiadzan);
    Disp.clear();
    menitiqmh = 5;
    tmputama = 1;
    
  }

  // Maghrib
  get_float_time_parts(times[5], hours, minutes);
  minutes = minutes + ihti;

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
    delay(durasiadzan);
    Disp.clear();
    menitiqmh = 5;
    tmputama = 1;
    
  }

  // Isya'
  get_float_time_parts(times[6], hours, minutes);
  minutes = minutes + ihti;

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
    delay(durasiadzan);
    Disp.clear();
    menitiqmh = 5;
    tmputama = 1;
    
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
//TAMPILKAN RUNNING TEKS


static String nama1 = (" UNTUK 1000 MASJID - ElektronMart.Com - 2019 ");
static String nama2 = (" Membuat JWS Running Teks ");

void TeksBerjalanKananKiri() {

  static uint32_t pM;
  uint32_t cM = millis();
  static uint8_t Kecepatan = 50;
  static uint32_t x;

  int width = Disp.width();
  int fullScroll = Disp.textWidth(nama1) + width;

  Disp.setFont(ElektronMart6x8);
  textCenter(0,"#");
  Disp.drawRect(0,6,Disp.width(),6,1,1);

  if(cM - pM > Kecepatan) {

    pM = cM;

    if (x < fullScroll) {

      ++x;
      
    } else {

      x = 0;
      Disp.clear();
      tmpjws = 0;
      
    }

    Disp.drawText(width - x, 8, nama1);
    
  }
  
}


void TeksBerjalanKiriKanan() {

  static uint32_t pM;
  uint32_t cM = millis();
  static uint8_t Kecepatan = 50;
  static uint32_t x;

  int width = Disp.width();
  int fullScroll = Disp.textWidth(nama2) + width;

  Disp.setFont(ElektronMart6x8);

  if(cM - pM > Kecepatan) {

    pM = cM;

    if (x < fullScroll) {

      ++x;
      
    } else {

      x = 0;
      
    }

    Disp.drawText(x - fullScroll + width, 8, nama2);
    
  }
  
}



//----------------------------------------------------------------------
//TAMPILKAN LOGO


//----------------------------------------------------------------------
// ANIMASI LOGO

void animLogoX() {

  static uint8_t x;
  static uint8_t s; // 0=in, 1=out
  static uint32_t pM;
  uint32_t cM = millis();

  JamBesar2(16);

  if (cM - pM > 35) {
    
    if (s == 0 and x < 16) {
      
      pM = cM;
      x++;
      
    }
    
    if (s == 1 and x > 0) {
      
      pM = cM;
      x--;
      
    }
    
  }
  
  if (cM - pM > 10000 and x == 16) {
    
    s = 1; 
       
  }

  if (x == 0 and s == 1) {
    
    Disp.clear();
    s = 0;
    tmpjws = 1;
     
  }


  LogoA(Disp.width() - x);
  LogoM(x - 16);  

}

void LogoA(uint32_t x) {

  static const uint8_t LogoA[] PROGMEM = {

    16, 16,

    0x00, 0x00, 0x7f, 0xfe, 0x7f, 0xfe, 0x00, 0x00, 0x7e, 0x66, 0x7e, 0x66, 0x06, 0x66, 0x06, 0x66, 0x7e, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7f, 0xfe, 0x7f, 0xfe, 0x00, 0x00

  };
  
  Disp.drawBitmap(x, 0, LogoA);
}


void LogoM(uint32_t x) {

  static const uint8_t LogoM[] PROGMEM = {

    16, 16,

    0x00, 0x00, 0x66, 0x7e, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x7e, 0x7e, 0x7e, 0x7e, 0x60, 0x60, 0x60, 0x60, 0x7e, 0x7e, 0x7e, 0x7e, 0x66, 0x06, 0x66, 0x06, 0x7f, 0xfe, 0x7f, 0xfe, 0x00, 0x00

  };
  
  Disp.drawBitmap(x, 0, LogoM);
}




//----------------------------------------------------------------------
//TAMPILKAN JADWAL SHOLAT

void JadwalSholat() {

  /*
    CALCULATION METHOD
    ------------------
    Jafari,   // Ithna Ashari
    Karachi,  // University of Islamic Sciences, Karachi
    ISNA,     // Islamic Society of North America (ISNA)
    MWL,      // Muslim World League (MWL)
    Makkah,   // Umm al-Qura, Makkah
    Egypt,    // Egyptian General Authority of Survey
    Custom,   // Custom Setting

    JURISTIC
    --------
    Shafii,    // Shafii (standard)
    Hanafi,    // Hanafi

    ADJUSTING METHOD
    ----------------
    None,        // No adjustment
    MidNight,   // middle of night
    OneSeventh, // 1/7th of night
    AngleBased, // angle/60th of night

    TIME IDS
    --------
    Fajr,
    Sunrise,
    Dhuhr,
    Asr,
    Sunset,
    Maghrib,
    Isha
  
  */

  RtcDateTime now = Rtc.GetDateTime();
  
  int tahun = now.Year();
  int bulan = now.Month();
  int tanggal = now.Day();
  float latitude = -6.16;
  float longitude = 106.61;
  int zonawaktu = 7;
  
  set_calc_method(Karachi);
  set_asr_method(Shafii);
  set_high_lats_adjust_method(AngleBased);
  set_fajr_angle(20);
  set_isha_angle(18);

  get_prayer_times(tahun, bulan, tanggal, latitude, longitude, zonawaktu, times);

}

void TampilJadwalSholat() {

  JadwalSholat();

  static uint8_t i;
  static uint32_t pM;
  uint32_t cM = millis();
  
  char sholat[7];
  char jam[5];
  char TimeName[][8] = {"SUBUH","TERBIT","DZUHUR","ASHAR","TRBNM","MAGHRIB","ISYA"};
  int hours, minutes;  

  if (cM - pM >= 3000) {
    
    pM = cM;
    Disp.drawRect(31,0,64,15,0,0);

    if (i == 1) {i = 2;} // Abaikan Terbit
    if (i == 4) {i = 5;} // Abaikan Terbenam

    get_float_time_parts(times[i], hours, minutes);
  
    minutes = minutes + ihti;

    if (minutes >= 60) {
      minutes = minutes - 60;
      hours ++;
    }

    String sholat = TimeName[i];
    sprintf(jam,"%02d:%02d", hours, minutes);     
    
    Disp.setFont(ElektronMart5x6);
    textCenter1PKanan(0, sholat);
    textCenter1PKanan(8, jam);

    i++;

    if (i > 7) {
      get_float_time_parts(times[0], hours, minutes);
      minutes = minutes + ihti;
      if (minutes < 11) {
        minutes = 60 - minutes;
        hours --;
      } else {
        minutes = minutes - 10 ;
      }
      sprintf(jam,"%02d:%02d", hours, minutes);
      Disp.drawRect(31,0,64,15,0,0);
      Disp.setFont(ElektronMart5x6);
      textCenter1PKanan(0, "TANBIH");
      textCenter1PKanan(8, jam);
      
      if (i > 8) {
        i = 0;
        Disp.drawRect(0,0,64,15,0,0);
        tmpinfo = 0;
        tmpjws = 2;
      }
      
    }      
    
  }
  
}


//----------------------------------------------------------------------
// HITUNG MUNDUR WAKTU SETELAH ADZAN SAMPAI MULAI IQOMAH

void iqomah() {

  static uint32_t pM;
  uint32_t cM = millis();
  static char hitungmundur[6];  

  Disp.setFont(ElektronMart5x6);
  textCenter(0, "IQOMAH");

  if (detikiqmh == 60) {
    detikiqmh = 0;
  }

  if (cM - pM >= 100) {
    pM = cM;
    detikiqmh--;

    if (menitiqmh == 0 && detikiqmh == 0) {
      Disp.clear();
      Disp.setFont(ElektronMart5x6);
      textCenter(0, "LURUSKAN DAN");
      textCenter(8, "RAPATKAN SHAF");
      BuzzerPanjang();
      detikiqmh = 59;
      Disp.clear();
      tmputama = 1;
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
//TAMPILKAN TANGGAL

void TampilTanggal() {

  RtcDateTime now = Rtc.GetDateTime();

  char hari[8];
  char tanggal[7]; // 21 SEP

  static uint32_t pM;
  uint32_t cM = millis();

  static uint8_t flag;

  if(cM - pM > 3000) {
    
    pM = cM;
    flag++;

    sprintf(hari, "%s", weekDay[now.DayOfWeek()]);
    sprintf(tanggal, "%02d %s", now.Day(), monthYear[now.Month()]);
  
    Disp.setFont(ElektronMart5x6);
    textCenter1PKanan(0, hari);
    textCenter1PKanan(8, tanggal);

    if (flag >= 2) {
      flag = 0;
      Disp.drawRect(32,0,64,15,0,0);
      tmpinfo = 1;
      
    }
    
  } 
  
}



//----------------------------------------------------------------------
// TAMPILKAN SUHU

void TampilSuhu() {

  RtcTemperature temp = Rtc.GetTemperature();
  int celsius = temp.AsFloatDegC();

  char suhu[3];
  int koreksisuhu = 2; // Perkiraan selisih suhu mesin dengan suhu ruangan

  static uint32_t pM;
  uint32_t cM = millis();

  static uint8_t flag;

  if(cM - pM > 3000) {
    
    pM = cM;
    flag++;

    sprintf(suhu, "%02d*C", celsius - koreksisuhu);
    
    Disp.setFont(ElektronMart5x6);
    textCenter1PKanan(0, "SUHU");
    textCenter1PKanan(8, suhu);

    if (flag >= 2) {
      flag = 0;
      Disp.drawRect(32,0,64,15,0,0);
      tmpinfo = 2;
      
    }
  
  }
  
}



//----------------------------------------------------------------------
// TAMPILKAN JAM

void JamBesar(uint16_t x) {

  RtcDateTime now = Rtc.GetDateTime();

  char jam[3];
  char menit[3];
  char detik[3];

  sprintf(jam, "%02d", now.Hour());
  sprintf(menit, "%02d", now.Minute());
  sprintf(detik, "%02d", now.Second());

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


void JamBesar2(uint16_t x) {

  RtcDateTime now = Rtc.GetDateTime();

  char jam[3];
  char menit[3];

  sprintf(jam, "%02d", now.Hour());
  sprintf(menit, "%02d", now.Minute());

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
      pM = cM;
      y++;
    }
    
    if(d  == 1 and y > 0) {
      pM = cM;      
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

  static uint32_t pMJam;
  uint32_t cM = millis();
  
  RtcDateTime now = Rtc.GetDateTime();
  char jam[9];
  
  if (cM - pMJam >= 1000) {
   
    pMJam = cM;
    
    //JAM
    sprintf(jam,"%02d:%02d:%02d", now.Hour(), now.Minute(), now.Second());
    Disp.setFont(ElektronMart5x6);
    Disp.drawText(20,7, jam);
        
  }
 
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
  delay(3000);
  
}

void BuzzerPendek() {
  
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(100);
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(3000);
  
}

void BunyiBeep() {

  static uint32_t pMBuz;
  uint32_t cM = millis();
  static boolean beep;  
  
  if (cM - pMBuz >= 200) {
   
    pMBuz = cM;

    beep = !beep;    
        
  }

  if (beep) {

    digitalWrite(buzzer, HIGH);
    
  } else {
    
    digitalWrite(buzzer, LOW);
    
  }
 
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
