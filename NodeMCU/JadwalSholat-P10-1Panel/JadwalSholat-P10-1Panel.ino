/*
 * JADWAL WAKTU SHOLAT MENGGUNAKAN NODEMCU ESP8266, LED P10, RTC DS3241, BUZZER
 * FITUR :  JADWAL SHOLAT 5 WAKTU DAN TANBIH IMSAK, JAM BESAR, TANGGAL, SUHU, ALARAM ADZAN DAN TANBIH IMSAK,
 *          DAN HITUNG MUNDUR IQOMAH DAN UPDATE SCROLL TEKS MALALUI WIFI.
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

- DMD2 : https://github.com/freetronics/DMD2
- PrayerTime : https://github.com/asmaklad/Arduino-Prayer-Times
- RTC DS3231 : https://github.com/Makuna/Rtc

email : bonny@grobak.net - www.grobak.net
*/



/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <SPI.h>
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);

#include <DMD2.h>
#include <fonts/Arial_Black_16.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/angka6x13.h>
#include <fonts/angka_2.h>
#include <fonts/Font3x5.h>

#include "PrayerTimes.h"


#ifndef APSSID
#define APSSID "JWSGrobakNet7"
#define APPSK  "grobaknet"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

// Inisiasi Jadwal Sholat
double times[sizeof(TimeName)/sizeof(char*)];

// Durasi waktu iqomah
byte value_iqmh; 
byte iqomahsubuh = 10;     // Durasi Iqomah dalam detik
byte iqomahdzuhur = 10;   // Durasi Iqomah dalam detik
byte iqomahashar = 7;     // Durasi Iqomah dalam detik
byte iqomahmaghrib = 5;   // Durasi Iqomah dalam detik
byte iqomahisya = 7;      // Durasi Iqomah dalam detik

int durasijamsholat = 300;  // Durasi Jam Sholat setelah Iqomah dalam detik

int durasiadzan = 180000; // Durasi Adzan 1 detik = 1000 ms, 180000 berarti 180 detik atau 3 menit

byte value_ihti=2; // Koreksi Waktu Menit Jadwal Sholat
byte value_hari;
//byte S_IQMH = 0, S_IHTI = 0, S_HARI = 0;



// BUZZER
const int buzzer = 3; // Pin GPIO Buzzer - RX

//SETUP RTC
//year, month, date, hour, min, sec and week-day(Senin 0 sampai Ahad 6)
//DateTime dt(2018, 12, 20, 16, 30, 0, 3);
char weekDay[][7] = {"SENIN ", "SELASA", " RABU ", "KAMIS ", "JUM'AT", "SABTU ", " AHAD ", "SENIN "}; // array hari, dihitung mulai dari senin, hari senin angka nya =0,
char monthYear[][4] = { " ", "JAN", "FEB", "MAR", "APR", "MEI", "JUN", "JUL", "AGU", "SEP", "OKT", "NOV", "DES" };

//SETUP DMD
SPIDMD dmd(1,1);  // Jumlah Panel P10 yang digunakan (KOLOM,BARIS)
DMD_TextBox box(dmd);  // "box" provides a text box to automatically write to/scroll the display


//WEB Server

char datestring[20];
String message,javaScript,XML;

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

RtcDateTime now;


/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/



//=============================================================================================
void buildJavascript(){
//=============================================================================================
  javaScript="<SCRIPT>\n";
  javaScript+="var xmlHttp=createXmlHttpObject();\n";

  javaScript+="function createXmlHttpObject(){\n";
  javaScript+=" if(window.XMLHttpRequest){\n";
  javaScript+="    xmlHttp=new XMLHttpRequest();\n";
  javaScript+=" }else{\n";
  javaScript+="    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";// code for IE6, IE5
  javaScript+=" }\n";
  javaScript+=" return xmlHttp;\n";
  javaScript+="}\n";

  javaScript+="function process(){\n";
  javaScript+=" if(xmlHttp.readyState==0 || xmlHttp.readyState==4){\n";
  javaScript+="   xmlHttp.open('PUT','xml',true);\n";
  javaScript+="   xmlHttp.onreadystatechange=handleServerResponse;\n";
  javaScript+="   xmlHttp.send(null);\n";
  javaScript+=" }\n";
  javaScript+=" setTimeout('process()',1000);\n";
  javaScript+="}\n";
 
  javaScript+="function handleServerResponse(){\n";
  javaScript+=" if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
  javaScript+="   xmlResponse=xmlHttp.responseXML;\n";
  
  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('rYear');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('year').innerHTML=message;\n";
  
  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('rMonth');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('month').innerHTML=message;\n";
  
  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('rDay');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('day').innerHTML=message;\n";
  
  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('rHour');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('hour').innerHTML=message;\n";
  
  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('rMinute');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('minute').innerHTML=message;\n";
  
  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('rSecond');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('second').innerHTML=message;\n";
  
  javaScript+="   xmldoc = xmlResponse.getElementsByTagName('rTemp');\n";
  javaScript+="   message = xmldoc[0].firstChild.nodeValue;\n";
  javaScript+="   document.getElementById('temp').innerHTML=message;\n";
 
  javaScript+=" }\n";
  javaScript+="}\n";
  javaScript+="</SCRIPT>\n";
}
//=============================================================================================
void buildXML(){
//=============================================================================================
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
    XML+="</rTemp>";
    XML+= temp.AsFloatDegC();
    XML+="</rTemp>";
  XML+="</t>"; 
}

//=============================================================================================
void handleRoot() {
//=============================================================================================
  buildJavascript();
  IPAddress ip = WiFi.localIP();
  String ipStr = (String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]));

  message = "<!DOCTYPE HTML>";
  message += "<html>";
  message += "<head>";
  message += javaScript;
  message += "<title>JWS Grobak.Net</title>";
  message += "<style> body { font-family: Arial, Helvetica, Sans-Serif; color: green; }";
  message += "h1 {text-align:center;}";
  message += "h5 {text-align:center;}";
  message += "a {text-decoration:none; color:#fff; background:green; padding:10px; border-radius:15px; }";
  message += "p {text-align:center;}";
  message += "table.center { width:80%; margin-left:10%; margin-right:10%;}";
  message += "</style>";
  message += "  </head>";
  message += "  <body onload='process()'>";
  message += "<table class='center'>";
  message += "  <tr>";
  message += "    <th>";
  message += "<h1>JWS Grobak.Net</h1>";
  message += "    </th> ";
  message += "  </tr>";
  message += "  <tr>";
  message += "    <td align='center'>";
  message += "    </td>";
  message += "  </tr>";

  
  message += "  <tr>";
  message += "    <td align='center'>";
  message += "<span id='year'></span>/<span id='month'></span>/<span id='day'></span>   <span id='hour'></span>:<span id='minute'></span>:<span id='second'></span><BR>";
  message += "    </td>";
  message += "  </tr>";
  
  message += "  <tr>";
  message += "    <td align='center'>";
  message += "Temp =<span id='temp'></span>C<BR>";
  message += "    </td>";
  message += "  </tr>";

  message += "  <tr>";
  message += "    <td>";
  message += "<h5><a href='/setTime'>Ubah Tanggal dan Jam</a></h5>";
  message += "    </td>";
  message += "  </tr>";

  message += "  <tr>";
  message += "    <td align='center'>";
  message += "<BR>IP  ";
  message += ipStr;
  message += "    </td>";
  message += "  </tr>";
  message += "</table>";
 
  message += "<BR>";

  message += "";
 
  message += "</body></html>";

  server.send ( 404 ,"text/html", message );
}

//=============================================================================================
void setTime() {
//=============================================================================================
  buildJavascript();
  message = "<!DOCTYPE HTML>";
  message += "<html>";
  message += "<head>";
  message += javaScript;
  message += "<title>JWS Grobak.Net</title>";
  message += "<style> body { font-family: Arial, Helvetica, Sans-Serif; color: green; }";
  message += "h1 {text-align:center;}";
  message += "h5 {text-align:center;}";
  message += "a {text-decoration:none; color:#fff; background:green; padding:10px; border-radius:15px; }";
  message += "p {text-align:center;}";
  message += "table.center { width:80%; margin-left:10%; margin-right:10%;}";
  message += "</style>";
  message += "  </head>";
  message += "  <body onload='process()'>";/////////////////////////////////////////

  message += "";
  
  message += "<table class='center'>";
  message += "  <tr>";
  message += "    <th>";
  message += "<h1>Ubah Tanggal dan Jam</h1>";
  message += "    </th> ";
  message += "  </tr>";
  message += "  <tr>";
  message += "    <td align='center'>";
  message += "Tanggal Sekarang  ";
  message += " <BR>   </td>";
  message += "  </tr>";
  message += "  <tr>";
  message += "    <td align='center'>";
  message += "<span id='year'></span>/<span id='month'></span>/<span id='day'></span><BR>";
  message += "    </td>";
  message += "  </tr>";
  message += "  <tr>";
  message += "    <td align='center'>";

  message += "<form >";
  message += "Format tanggal 2017-03-20<br><br>";
  message += "<input type='date' name='date' min='2017-03-20' style='height:75px; width:200px'><br><br>";
  message += "<input type='submit' value='Ubah Tanggal' style='height:75px; width:200px'> ";
  message += "</form>";
  

  message += "    </td>";
  message += "  </tr>";
  message += "  <tr>";
  message += "    <td align='center'>";
  message += "Jam Tersimpan<BR><span id='hour'></span>:<span id='minute'></span>:<span id='second'></span><BR><BR>";
  message += "    </td>";
  message += "  </tr>";
  message += "  <tr>";
  message += "    <td align='center'>";
  message += "<form >";  
  message += "Tentukan Jam<br>";
  message += "<input type='TIME' name='time' style='height:75px; width:200px'><br><br>";
  message += "<input type='submit' value='Ubah Jam' style='height:75px; width:200px'> ";
  message += "</form>";
  message += "    </td>";
  message += "  </tr>";
  message += "  <tr>";
  message += "    <td>";
  message += "<h5><a href='/'>Kembali</a></h5>";
  message += "    </td>";
  message += "  </tr>";
  message += "</table>";


  message += "";
  
  message += "</body></html>";

  server.send ( 404 ,"text/html", message );
  
// Tanggal--------------------------------------------------------------------  
  if (server.hasArg("date")) {
    
    uint16_t ano;
    uint8_t mes;
    uint8_t dia;
    Serial.print("ARGdate");
    Serial.println(server.arg("date"));
    String sd=server.arg("date");
    String lastSd;
     ano = ((sd[0]-'0')*1000)+((sd[1]-'0')*100)+((sd[2]-'0')*10)+(sd[3]-'0');
     mes = ((sd[5]-'0')*10)+(sd[6]-'0');
     dia = ((sd[8]-'0')*10)+(sd[9]-'0');
    if (sd != lastSd){
      RtcDateTime now = Rtc.GetDateTime();
      uint8_t hour = now.Hour();
      uint8_t minute = now.Minute();
      Rtc.SetDateTime(RtcDateTime(ano, mes, dia, hour, minute, 0));
      lastSd=sd;}
  // Serial.println(fa);

   server.send ( 404 ,"text", message );
}//if has date
// Jam ------------------------------------------------
  if (server.hasArg("time")) {
    Serial.println(server.arg("time"));
    String st=server.arg("time");
    String lastSt;
    uint8_t hora = ((st[0]-'0')*10)+(st[1]-'0');
    uint8_t minuto = ((st[3]-'0')*10)+(st[4]-'0');
    if (st != lastSt){
       RtcDateTime now = Rtc.GetDateTime();
       uint16_t year = now.Year();
       uint8_t month = now.Month();
       uint8_t day = now.Day();
       Rtc.SetDateTime(RtcDateTime(year, month, day, hora, minuto, 0));
       lastSt=st;}
    server.send ( 404 ,"text", message );

  }//if has time
}
//=============================================================================================
void handleXML(){
//=============================================================================================
  buildXML();
  server.send(200,"text/xml",XML);
}
//=============================================================================================
void handleNotFound() {
//=============================================================================================
  String message = "<html><head>";
  message += "<title>JWS Grobak.Net - Halaman tidak ditemukan</title>";
  message += "<style> body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }";
  message += "h1 {text-align:center;}";
  message += "h5 {text-align:center;}";
  message += "a {text-decoration:none; color:#fff; background:green; padding:10px; border-radius:15px; }";
  message += "</style>";
  message += "  </head>";
  message += "  <body>";
  message += "<table style='width:80%'>";
  message += "<tr>";//baris 2
  message += "<th>";//kolom judul
  message += "<h1>Tidak ditemukan</h1>";
  message += "</th>";
  message += "<tr>";//baris 2
  message += "<td>";//kolom isi
  message += "<h5><a href='/'>Kembali</a></h5>";
  message += "<td>";
  message += "</tr>";
  message += "</table>";
  message += "</body></html>";
  message += "";
  
  server.send ( 404 ,"text", message );
}


// setup rutinitas yang hanya dilakukan satu kali saat tekan reset:

void setup() {

  delay(1000);
  Serial.begin(115200);
  Serial.println();

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.on ( "/", handleRoot );
  server.on ( "/setTime", setTime );
  server.on ( "/xml", handleXML) ;
  server.onNotFound ( handleNotFound );
  
  server.begin();
  Serial.println("HTTP server started");
  
  //RTC D3231
  Rtc.Begin();
  // if you are using ESP-01 then uncomment the line below to reset the pins to
  // the available pins for SDA, SCL
  //Wire.begin(0, 2); // due to limited pins, use pin 0 and 2 for SDA, SCL

  //   RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (!Rtc.GetIsRunning()){
  //     Serial.println("RTC was not actively running, starting now");
       Rtc.SetIsRunning(true);}
  //RtcDateTime now = Rtc.GetDateTime();
  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
  
  //DMD
  dmd.begin();

  //Buzzer
  pinMode(buzzer, OUTPUT);      // inisiasi pin untuk buzzer
  digitalWrite(buzzer, LOW);
  delay(50);

  BuzzerPendek();


  // INTRO BRANDING
  
  dmd.clearScreen();
  dmd.selectFont(Font3x5); 
  dmd.drawString(4,-1, "GROBAK");
  dmd.drawString(2,7, ".NET");
  delay(1000);

  dmd.clearScreen();
  dmd.selectFont(Arial_Black_16); 
  dmd.drawString(-2,1, "JWS");
  delay(1000);

  dmd.clearScreen();
  dmd.selectFont(Font3x5); 
  dmd.drawString(7,3, "VER.1");
  delay(1000);
  
}

long transisi = 0;

// loop rutinitas yang dijalankan berulang selamanya:
void loop() {
  
  server.handleClient();
  mulai();

}

void mulai() {
  
  AlarmSholat(); // Banyak dipanggil class AlarmSholat() ini agar waktu sholat lebih akurat

  TampilJam();

  if(millis()-transisi > 15000) { // Tampilkan Tanggal pada detik ke 15
    AlarmSholat();
    TampilTanggal();

  }
  
  if(millis()-transisi > 18000) { // Tampilkan Suhu pada detik ke 18

    AlarmSholat();
    TampilSuhu();
    
    AlarmSholat();
    TampilJadwalSholat();
    
    transisi = millis();
  }
  
}


//----------------------------------------------------------------------
//JADWAL SHOLAT

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

void TampilJadwalSholat() {
  JadwalSholat();
  
  char sholat[7];
  char jam[5];
  char TimeName[][6] = {"SUBUH","TRBIT","DZUHR","ASHAR","TRBNM","MGHRB","ISYA'"};
  int hours, minutes;

  for (int i=0;i<7;i++){

    get_float_time_parts(times[i], hours, minutes);
    
    minutes = minutes + value_ihti;
    
    if (minutes >= 60) {
      minutes = minutes - 60;
      hours ++;
    }
    
    if (i==0 || i==2 || i==3 || i==5 || i==6) { //Tampilkan hanya Subuh, Dzuhur, Ashar, Maghrib, Isya
      //sprintf(sholat,"%s",TimeName[i]);
      String sholat = TimeName[i];
      sprintf(jam,"%02d:%02d", hours, minutes);     
      dmd.clearScreen();
      dmd.selectFont(Font3x5);
      dmd.drawString(6,-2,sholat);
      dmd.selectFont(angka_2);
      dmd.drawString(1,8,jam);
      Serial.println(sholat);
      Serial.println(" : ");
      Serial.println(jam);
      delay(2000);
    }
  }

  //Tambahan Waktu Tanbih (Peringatan 10 menit sebelum mulai puasa) yang biasa disebut Imsak
  
  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes + value_ihti;
  if (minutes < 11) {
    minutes = 60 - minutes;
    hours --;
  } else {
    minutes = minutes - 10 ;
  }
  sprintf(jam,"%02d:%02d",hours,minutes);     
  dmd.clearScreen();
  dmd.selectFont(Font3x5);
  dmd.drawString(4,-2,"TANBIH");
  dmd.selectFont(angka_2);
  dmd.drawString(1,8,jam);
  Serial.print("TANBIH");
  Serial.println(" : ");
  Serial.println(jam);
  delay(1000);
  
}

void AlarmSholat() {

  RtcDateTime now = Rtc.GetDateTime();

  int Hari = now.DayOfWeek();
  int Hor = now.Hour();
  int Min = now.Minute();
  int Sec = now.Second();

  JadwalSholat();
  
  int hours, minutes;

  // Tanbih atau Imsak
  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes + value_ihti;

  if (minutes < 11) {
    minutes = 60 - minutes;
    hours --;
  } else {
    minutes = minutes - 10 ;
  }
  
  if (Hor == hours && Min == minutes) {
    dmd.clearScreen();
    dmd.selectFont(Font3x5);
    dmd.drawString(4, -2, "TANBIH"); //koordinat tampilan
    dmd.drawString(6, 7, "IMSAK"); //koordinat tampilan
    BuzzerPendek();
    Serial.println("TANBIH");
    delay(60000);
  }

  // Subuh
  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes + value_ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }
  
  if (Hor == hours && Min == minutes) {
    dmd.clearScreen();
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN"); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(6, 7, "SUBUH"); //koordinat tampilan
    BuzzerPanjang();
    Serial.println("SUBUH");
    delay(durasiadzan);//180 detik atau 3 menit untuk adzan
    
    BuzzerPendek();
    value_iqmh = iqomahsubuh;    //Saat Subuh tambah 2 menit waktu Iqomah 
    Iqomah();
  }

  // Dzuhur
  get_float_time_parts(times[2], hours, minutes);
  minutes = minutes + value_ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }
  
  if (Hor == hours && Min == minutes && Hari != 4) {
    
    dmd.clearScreen();
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN"); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(4, 7, "DZUHUR"); //koordinat tampilan
    BuzzerPanjang();
    Serial.println("DZUHUR");
    delay(durasiadzan);//180 detik atau 3 menit untuk adzan
    
    BuzzerPendek();
    value_iqmh = iqomahdzuhur;
    Iqomah();
    
  } else if (Hor == hours && Min == minutes && Hari == 4) { 
    
    dmd.clearScreen();
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN"); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(4, 7, "JUM'AT"); //koordinat tampilan
    BuzzerPanjang();
    Serial.println("Adzan Jum'at");
    delay(durasiadzan);//180 detik atau 3 menit untuk adzan
    
    BuzzerPanjang();
    
    PesanTeks();
  }

  // Ashar
  get_float_time_parts(times[3], hours, minutes);
  minutes = minutes + value_ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }
  
  if (Hor == hours && Min == minutes) {
    dmd.clearScreen();
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN "); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(6, 7, "ASHAR"); //koordinat tampilan
    BuzzerPanjang();
    Serial.println("ASHAR");
    delay(durasiadzan);//180 detik atau 3 menit untuk adzan
    
    BuzzerPendek();
    value_iqmh = iqomahashar;
    Iqomah();
  }

  // Maghrib
  get_float_time_parts(times[5], hours, minutes);
  minutes = minutes + value_ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }
  
  if (Hor == hours && Min == minutes) {
    dmd.clearScreen();
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN "); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(1, 7, "MAGHRIB"); //koordinat tampilan
    BuzzerPanjang();
    Serial.println("MAGHRIB");
    delay(durasiadzan);//180 detik atau 3 menit untuk adzan
    
    BuzzerPendek();
    value_iqmh = iqomahmaghrib;
    Iqomah();
  }

  // Isya'
  get_float_time_parts(times[6], hours, minutes);
  minutes = minutes + value_ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }
  
  if (Hor == hours && Min == minutes) {
    dmd.clearScreen();
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN"); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(8, 7, "ISYA'"); //koordinat tampilan
    BuzzerPanjang();
    Serial.println("ISYA");
    delay(durasiadzan);//180 detik atau 3 menit untuk adzan
    
    BuzzerPendek();
    value_iqmh = iqomahisya;  
    Iqomah();
  }
  
}


//----------------------------------------------------------------------
//IQOMAH

void Iqomah() {

  JadwalSholat();

  RtcDateTime now = Rtc.GetDateTime();

  int langkah, i, hours, minutes;
  dmd.clearScreen();
  dmd.selectFont(Font3x5);
  dmd.drawString(3, -2, "IQOMAH"); //koordinat tampilan
  int tampil;
  char iqomah[7];

  int detik = 0, menit = value_iqmh;
  for (detik = 0; detik >= 0; detik--) {
    delay(1000);
    sprintf(iqomah, "%02d : %02d", menit, detik);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 9, iqomah);
    if (detik <= 0) {
      detik = 60;
      menit = menit - 1;
    } if (menit <= 0 && detik <= 1) {
      dmd.clearScreen();
      digitalWrite(2, HIGH);//alarm Iqomah
      delay(1000);
      digitalWrite(2, LOW);//alarm Iqomah
      delay(50);
      dmd.selectFont(Font3x5);
      dmd.drawString(2, -2, "LURUS 8"); //koordinat tampilan
      dmd.drawString(0, 7, "RAPATKAN"); //koordinat tampilan
      delay(10000);
      for (tampil = 0; tampil < durasijamsholat ; tampil++) { // 300 detik atau 5 menit nilai tunda sholt
        menit = 0;
        detik = 0;
        dmd.clearScreen();
        dmd.selectFont(Font3x5);
        dmd.drawString(4, -2, "SHOLAT"); //koordinat tampilan
        ///////////////////////
        RtcDateTime now = Rtc.GetDateTime();        
        char jam[7];
        sprintf(jam, "%02d : %02d : %02d", now.Hour(), now.Minute(), now.Second());
        dmd.selectFont(Font3x5);
        dmd.drawString(2, 7, jam);
        /////////////////////
        delay (1000);
      }
    }
  }

  value_iqmh = 0;   // Kembalikan waktu iqomah ke 0
}


//----------------------------------------------------------------------
//TAMPILKAN JAM BESAR

void TampilJam() {
  RtcDateTime now = Rtc.GetDateTime();
  char jam[2];
  char menit[2];
  char detik[2];

  dmd.clearScreen(); 

  //JAM
  sprintf(jam,"%02d", now.Hour());
  dmd.selectFont(angka6x13);
  dmd.drawString(3, 0, jam);

  //MENIT
  sprintf(menit,"%02d", now.Minute());
  dmd.selectFont(Font3x5);
  dmd.drawString(22, -2, menit);

  //DETIK
  sprintf(detik,"%02d", now.Second());
  dmd.selectFont(Font3x5);
  dmd.drawString(22, 6, detik);

  
  //DETIK TITIK DUA
  int n;
  n = now.Second()+1;

  if(n % 2 == 0) {
    dmd.drawFilledBox(18,4,19,6, GRAPHICS_OFF);
    dmd.drawFilledBox(18,8,19,10, GRAPHICS_OFF);
  } else {
    dmd.drawFilledBox(18,4,19,6);
    dmd.drawFilledBox(18,8,19,10);
  }

  delay(1000);
  
}

void TampilJamKecil() {
  RtcDateTime now = Rtc.GetDateTime();
  char jam[7];  
  
  sprintf(jam, "%02d : %02d", now.Hour(), now.Minute());
  
  dmd.clearScreen();
  dmd.selectFont(angka_2);
  dmd.drawString(1,0,jam);
  Serial.println(jam);  
  
}


//----------------------------------------------------------------------
//TAMPILKAN TANGGAL

void TampilTanggal() {
  RtcDateTime now = Rtc.GetDateTime();
  
  String hari = weekDay[now.DayOfWeek()];
  
  char tanggal[18];
  sprintf(tanggal, "%02d",             //%02d allows to print an integer with leading zero 2 digit to the string, %s print sebagai string
                  now.Day()           //get day method
  );

  String bulan = monthYear[now.Month()];
  
  dmd.clearScreen();
  dmd.selectFont(Font3x5);
  dmd.drawString(4,-2,hari);
  dmd.drawString(4,7,tanggal);
  dmd.drawString(14,7,bulan);
  //Serial.println(hari + ", " + tanggal);
  delay(3000);
  
}


//----------------------------------------------------------------------
// TAMPILKAN SUHU

void TampilSuhu(){
  //Tampilkan Suhu
  RtcTemperature temp = Rtc.GetTemperature();
  int celsius = temp.AsFloatDegC();
  char suhu[2];
  int koreksisuhu = 2; // Perkiraan selisih suhu ruangan dan luar ruangan
  
  dmd.clearScreen();
  dmd.selectFont(Font3x5);    
  dmd.drawString(9, -2, "SUHU");
  sprintf(suhu,"%dC",celsius - koreksisuhu);
  dmd.selectFont(SystemFont5x7);
  dmd.drawString(8, 8, suhu);
  
  delay(3000);
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

void PesanTeks() {
  for (int i=0;i<5;i++){
  dmd.clearScreen();
  dmd.selectFont(Font3x5);
  dmd.drawString(4, -2, "JUM'AT"); //koordinat tampilan
  DMD_TextBox box(dmd, 0, 8);
  String spasi = "          ";
  String pesan = "PERIKSA KEMBALI HP ANDA DAN PASTIKAN SUDAH DI MATIKAN DEMI KEKHUSUAN DALAM BERIBADAH" + spasi;
  String tampil = spasi + pesan + spasi; 
  const char *MESSAGE = tampil.c_str();
  const char *next = MESSAGE;
  while(*next) {
    Serial.print(*next);
    box.print(*next);
    delay(200);
    next++;
  }
      
  }
}
