/* Jadwal Waktu Sholat NodeMCU + RTC DS3231 + Led Matrix MAX7219 + Buzzer + Power Bank
 * 
 * bonny@grobak.net

Pengkabelan:

NodeMCU             Matrix
---------------------------
D7-GPIO13           DIN
D5-GPIO14           CLK
D3-GPIO0            CS(LOAD)


NodeMCU             DS3231
--------------------------
D1-GPIO5            SCL
D2-GPIO4            SDA

Vin & GND NodeMCU to 5V Power Supply 
VCC & GND MAX7219 Panel to 5V Power Supply

NodeMCU             Buzzer
--------------------------
D6-GPIO12           +
G                   -

VCC and GND DS3231 Module to 5V Power Supply

*/

#include <Wire.h>
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);
#include "PrayerTimes.h"
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <Fonts/Font3x78pt7b.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

// Ubah nama hari dan nama bulan
char weekDay[][3] = {"AH","SN","SL","RB","KM","JM","SB","AH"};
char monthYear[][4] = { " ", "JAN", "FEB", "MAR", "APR", "MEI", "JUN", "JUL", "AGU", "SEP", "OKT", "NOV", "DES" };

// Inisiasi Jadwal Sholat
double times[sizeof(TimeName)/sizeof(char*)];
int ihti = 2;   // Koreksi Waktu Menit Jadwal Sholat
int value_iqmh;  // Waktu Iqomah 10 menit

// Set Wifi SSID dan Password
#ifndef APSSID
#define APSSID "JWSGrobakNet1"
#define APPSK  "grobaknet"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

char datestring[20];
String message,javaScript,XML;
    
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

RtcDateTime now;


// Matrix Display
long period;
int offset=1,refresh=0;
int pinCS = 0; // Connecting the CS pin
int numberOfHorizontalDisplays = 4; // Number of LED matrices by Horizontal
int numberOfVerticalDisplays = 1; // Number of LED matrices per Vertical
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
int wait = 20; // Running speed
int spacer = 2;
int width = 5 + spacer; // Adjust the distance between the characters

// BUZZER
uint8_t buzzer = D6;

int updCnt = 0;
long clkTime = 0;



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


void setup() {
  delay(1000);
  Serial.begin(115200);
  
  Serial.println();
  Serial.print("Configuring access point...");
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

  // RTC --------------------------------------------------------------------------------------
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


  //Matrix Display
  matrix.setIntensity(1); // Matrix brightness from 0 to 15
    
  // The initial coordinates of the matrices 8 * 8
  matrix.setRotation(0, 1);        // 1 matrix
  matrix.setRotation(1, 1);        // 2 matrix
  matrix.setRotation(2, 1);        // 3 matrix
  matrix.setRotation(3, 1);        // 4 matrix

  //Buzzer
  pinMode(buzzer, OUTPUT);      // inisiasi pin untuk buzzer

  BuzzerPendek();

  Branding();
  
}


void loop() {
  
  server.handleClient();
  mulai();

}

void mulai() {
  
  TampilJam();

  AlarmSholat();

  if(millis()-clkTime > 15000) {  //Every 15 seconds, tampilkan tanggal bergerak
    TampilTanggal();
    AlarmSholat();
  }

  if(millis()-clkTime > 18000) {  //Every 15 seconds, tampilkan tanggal bergerak
    TampilSuhu();
    delay(3000);
    AlarmSholat();
    TampilJadwalSholat();
    AlarmSholat();
    TeksBerjalan("GROBAK.NET");
    AlarmSholat();
    
    clkTime = millis();
  }
  
}


//-----------------------------------------------------
//Menampilkan Mode Setting

void SetMode(){

   TeksStatis("SETUP...");

}

//-----------------------------------------------------
//Menampilkan Jam

void TampilJam(){

  RtcDateTime now = Rtc.GetDateTime();

  char jam[8];

  sprintf(jam, "%02d:%02d:%02d",    //%02d print jam dengan format 2 digit
                  now.Hour(),       //get hour method
                  now.Minute(),     //get minute method
                  now.Second()      //get second method
  );
  
  Serial.println(jam);    //print the string to the serial port

  TeksStatis(jam);

  delay(1000);            //second delay

}

//-----------------------------------------------------
//Menampilkan Tanggal

void TampilTanggal(){

  RtcDateTime now = Rtc.GetDateTime();

  char tanggal[18];

  sprintf(tanggal, "%s,%02d%s",             //%02d allows to print an integer with leading zero 2 digit to the string, %s print sebagai string
                  weekDay[now.DayOfWeek()],   //ambil method hari dalam format lengkap
                  now.Day(),                 //get day method
                  monthYear[now.Month()]     //get month method
  );
  
  Serial.println(tanggal);    //print the string to the serial port

  TeksStatis(tanggal);
  delay(3000);

}

//-----------------------------------------------------
//Menampilkan Suhu

void TampilSuhu() {

  RtcTemperature temp = Rtc.GetTemperature();
  int celsius = temp.AsFloatDegC();
  char suhu[2];
  int koreksisuhu = 2; // Perkiraan selisih suhu ruangan dan luar ruangan
  
  sprintf(suhu, "SUHU %dC", celsius - koreksisuhu);
  
  TeksStatis(suhu);
    
}


void JadwalSholat() {

  RtcDateTime now = Rtc.GetDateTime();

  int tahun = now.Year();
  int bulan = now.Month();
  int tanggal = now.Day();

  int dst=7; // TimeZone
  
  set_calc_method(ISNA);
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
  char TimeName[][8] = {"SU","TE","DZ","AS","TE","MG","IS","WA"};
  int hours, minutes;

  for (int i=0;i<8;i++){

    get_float_time_parts(times[i], hours, minutes);

    ihti = 2;
    minutes = minutes + ihti;
    
    if (minutes >= 60) {
      minutes = minutes - 60;
      hours ++;
    }
    else {
      ;
    }
    
    if (i==0 || i==2 || i==3 || i==5 || i==6) { //Tampilkan hanya Subuh, Dzuhur, Ashar, Maghrib, Isya
      //sprintf(sholat,"%s",TimeName[i]); 
      sprintf(jam,"%s %02d:%02d",TimeName[i], hours, minutes);
      TeksStatis(jam);
      Serial.println(jam);
      delay(2000);
      
    }
  }

  //Tambahan Waktu Tanbih (Peringatan 10 menit sebelum mulai puasa) yang biasa disebut Imsak
  
  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes + ihti;
  if (minutes < 11) {
    minutes = 60 - minutes;
    hours --;
  } else {
    minutes = minutes - 10 ;
  }
  sprintf(jam,"TI %02d:%02d",hours,minutes);     
  TeksStatis(jam);
  Serial.println(jam);
  delay(2000);

}

void AlarmSholat() {

  RtcDateTime now = Rtc.GetDateTime();

  int Hari = now.DayOfWeek();
  int Hor = now.Hour();
  int Min = now.Minute();
  int Sec = now.Second();

  JadwalSholat();
  int hours, minutes;

  // Tanbih Imsak
  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes+ihti;

  if (minutes < 11) {
    minutes = 60 - minutes;
    hours --;
  } else {
    minutes = minutes - 10 ;
  }

  if (Hor == hours && Min == minutes) {
    TeksStatis("TANBIH");
    BuzzerPendek();
    Serial.println("TANBIH");
    delay(60000);
  }

  // Subuh
  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes + ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }

  if (Hor == hours && Min == minutes) {
    TeksStatis("SUBUH");
    BuzzerPanjang();
    Serial.println("SUBUH");
    delay(180000);//180 detik atau 3 menit untuk adzan
    
    BuzzerPendek();
    value_iqmh = 10;
    Iqomah();
  }

  // Dzuhur
  get_float_time_parts(times[2], hours, minutes);
  minutes = minutes + ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }
  
  if (Hor == hours && Min == minutes && Hari != 5) {
    
    TeksStatis("DZUHUR");
    BuzzerPanjang();
    Serial.println("DZUHUR");
    delay(180000);//180 detik atau 3 menit untuk adzan
    
    BuzzerPendek();
    value_iqmh = 3;
    Iqomah();
     
  } else if (Hor == hours && Min == minutes && Hari == 5) { 
    
    TeksStatis("JUM'AT");
    BuzzerPanjang();
    Serial.println("JUM'AT");
    delay(180000);//180 detik atau 3 menit untuk adzan
    
  }

  // Ashar
  get_float_time_parts(times[3], hours, minutes);
  minutes = minutes + ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }
  
  if (Hor == hours && Min == minutes) {
    TeksStatis("ASHAR");
    BuzzerPanjang();
    Serial.println("ASHAR");
    delay(180000);//180 detik atau 3 menit untuk adzan
    
    BuzzerPendek();
    value_iqmh = 3;
    Iqomah();
  }

  // Maghrib
  get_float_time_parts(times[5], hours, minutes);
  minutes = minutes + ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }
  
  if (Hor == hours && Min == minutes) {
    TeksStatis("MAGHRIB");
    BuzzerPanjang();
    Serial.println("MAGHRIB");
    delay(180000);//180 detik atau 3 menit untuk adzan
    
    BuzzerPendek();
    value_iqmh = 2;
    Iqomah();
  }

  // Isya'
  get_float_time_parts(times[6], hours, minutes);
  minutes = minutes + ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }
  
  if (Hor == hours && Min == minutes) {
    TeksStatis("ISYA");
    BuzzerPanjang();
    Serial.println("ISYA");
    delay(180000);//180 detik atau 3 menit untuk adzan

    BuzzerPendek();
    value_iqmh = 2;
    Iqomah();
  }
  
}


//--------------------------------------------------
//IQOMAH

void Iqomah() {

  RtcDateTime now = Rtc.GetDateTime();
  //iqomah----------------------------------
  char iqomah[8];
  int tampil;
  int detik = 0, menit = value_iqmh;
  for (detik = 0; detik >= 0; detik--) {
    delay(1000);
    sprintf(iqomah,"IQ %02d:%02d",menit,detik);     
    TeksStatis(iqomah);
    if (detik <= 0) {
      detik = 60;
      menit = menit - 1;
    } if (menit <= 0 && detik <= 1) {      
      BuzzerPendek();
      TeksStatis("SHOLAT");
      delay(10000);
      for (tampil = 0; tampil < 300 ; tampil++) { //< tampil selama 300 detik waktu saat sholat
        menit = 0;
        detik = 0;
        ///////////////////////
        //now = rtc.now();
        sprintf(iqomah,"SH %02d:%02d",now.Hour(),now.Minute());  
        TeksStatis(iqomah);
        /////////////////////
        delay (1000);
      }
    }
  }
}

//-----------------------------------------------------
//Membuat Format Teks Statis

void TeksStatis(String text){

  matrix.fillScreen(LOW);

  // Setting font untuk jam
  spacer = 2;
  width = 3 + spacer;
  matrix.setFont(&Font3x78pt7b);

  for (int i=0; i<text.length(); i++){
  
  int letter =(matrix.width())- i * (width-1);
  int x = (matrix.width() +0) -letter;
  int y = 11; // Set posisi vertikal font kadang suka terlalu atas atau terlalu bawah
  matrix.drawChar(x, y, text[i], HIGH, LOW, 1);
    
  matrix.write(); // Tampilkan

  }

}


// =======================================================================
void TeksBerjalan (String text){

  // Setting font untuk jam

  spacer = 2;
  width = 5 + spacer;
  matrix.setFont(); // Reset font to default
  
  for ( int i = 0 ; i < width * text.length() + matrix.width() - 1 - spacer; i++ ) {
    if (refresh==1) i=0;
    refresh=0;
    matrix.fillScreen(LOW);
    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // Center text on Vertical

    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < text.length() ) {
        matrix.drawChar(x, y, text[letter], HIGH, LOW, 1);
      }
      letter--;
      x -= width;
    }
  matrix.write(); // Display
  delay(wait);
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

void Branding() {
  TeksStatis("JWS MEJA");
  delay(2000);
  TeksStatis("VER.1");
  delay(2000);
}
