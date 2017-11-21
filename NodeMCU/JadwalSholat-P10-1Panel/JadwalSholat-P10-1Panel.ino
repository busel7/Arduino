/*
 * JADWAL WAKTU SHOLAT MENGGUNAKAN NODEMCU ESP8266, LED P10, RTC DS3241, BUZZER
 * FITUR :      JADWAL SHOLAT 5 WAKTU, JAM BESAR, TANGGAL, SUHU, ALARAM ADZAN,
 *              DAN HITUNG MUNDUR IQOMAH DAN UPDATE SCROLL TEKS MALALUI WIFI.
 * 

Pin on  DMD P10     GPIO      NODEMCU               Pin on  DS3231      NODEMCU                   Pin on  Buzzer       NODEMCU

        A           GPIO16    D0                            SCL         D1 (GPIO 5)                       +            RX (GPIO 3)
        B           GPIO12    D6                            SDA         D2 (GPIO 4)                       -            GND
        CLK         GPIO14    D5                            VCC         3V
        SCK         GPIO0     D3                            GND         GND
        R           GPIO13    D7
        NOE         GPIO15    D8
        GND         GND       GND

Catatan : 

o Perlu Power Eksternal 5V ke LED P10.
o Saat Flashing (upload program) cabut sementara pin untuk buzzer.

email : bonny@grobak.net - www.grobak.net
*/

#include <ESP8266WiFi.h>
#include "espneotext.h"
#include <SPI.h>
#include <EEPROM.h>

#include <Wire.h>
#include <Sodaq_DS3231.h>

#include <DMD2.h>
//#include <fonts/Arial14.h>
#include <fonts/Arial_Black_16.h>
//#include <fonts/Droid_Sans_12.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/angka6x13.h>
#include <fonts/angka_2.h>
#include <fonts/Font3x5.h>

#include "PrayerTimes.h"

// Inisiasi Jadwal Sholat
double times[sizeof(TimeName)/sizeof(char*)];
int ihti = 2;   // Koreksi Waktu Menit Jadwal Sholat

byte value_iqmh=1, value_ihti=2, value_hari;
//byte S_IQMH = 0, S_IHTI = 0, S_HARI = 0;

// BUZZER
const int buzzer = 3; // Pin GPIO Buzzer - RX

//SETUP RTC
//year, month, date, hour, min, sec and week-day(starts from 0 and goes to 6)
//writing any non-existent time-data may interfere with normal operation of the RTC.
//Take care of week-day also.
//DateTime dt(2017, 11, 21, 15, 52, 0, 1);
char weekDay[][7] = {"SENIN ", "SELASA", " RABU ", "KAMIS ", "JUM'AT", "SABTU ", " AHAD ", "SENIN "}; // array hari, dihitung mulai dari senin, hari senin angka nya =0,
char monthYear[][4] = { " ", "JAN", "FEB", "MAR", "APR", "MEI", "JUN", "JUL", "AGU", "SEP", "OKT", "NOV", "DES" };

//SETUP DMD
SPIDMD dmd(1,1);  // DMD controls the entire display
DMD_TextBox box(dmd);  // "box" provides a text box to automatically write to/scroll the display

//SETUP WIFI AP
IPAddress local_IP(192,168,7,77);
IPAddress gateway(192,168,7,250);
IPAddress subnet(255,255,255,0);

const char *ssid = "NodeMCU";
const char *pass = "1234efgh";
String value;
WiFiServer server(80);


// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);

  //RTC D3231
  Wire.begin();
  rtc.begin();
  //rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above (ini untuk setting jam)

  //DMD
  dmd.begin();

  //Buzzer
  pinMode(buzzer, OUTPUT);      // inisiasi pin untuk buzzer
  digitalWrite(buzzer, LOW);
  delay(50);

  //attachInterrupt(0, Setting, FALLING);
  Buzzer();

  //WIFI AP
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid,pass) ? "Ready" : "Failed!");

  // Start the server
  server.begin();
  Serial.println("Server started");

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

  // INTRO BRANDING
  
  dmd.clearScreen();
  dmd.selectFont(Font3x5); 
  dmd.drawString(4,-1, "GROBAK");
  dmd.drawString(2,7, ".NET");
  delay(3000);

  dmd.clearScreen();
  dmd.selectFont(Arial_Black_16); 
  dmd.drawString(-2,1, "JWS");
  delay(2000);

  dmd.clearScreen();
  dmd.selectFont(Font3x5); 
  dmd.drawString(7,3, "VER.1");
  delay(2000);

  dmd.clearScreen();
  dmd.selectFont(Font3x5); 
  dmd.drawString(1,-2, "ALMT IP");
  DMD_TextBox box(dmd, 0, 7);
  String ip = WiFi.softAPIP().toString();
  const char *alamatip = ip.c_str();
  const char *lanjut = alamatip;
  while(*lanjut) {
    Serial.print(*lanjut);
    box.print(*lanjut);
    delay(250);
    lanjut++;
  }
  delay(2000);

}





long transisi = 0;


// the loop routine runs over and over again forever:
void loop() {

  AlarmSholat(); // Banyak dipanggil class AlarmSholat() ini agar waktu sholat lebih akurat

  TampilJam();
  

  if(millis()-transisi > 15000) { // Tampilkan Tanggal pada detik ke 10
    AlarmSholat();
    TampilTanggal();
  }
  
  if(millis()-transisi > 18000) { // Tampilkan Suhu pada detik ke 13
    AlarmSholat();
    WebTeks();
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

  DateTime now = rtc.now();

  int tahun = now.year();
  int bulan = now.month();
  int tanggal = now.date();

  int dst=7; // TimeZone

  byte value_iqmh = 1;
  
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

  String Teks;  

  for (int i=0;i<7;i++){
    char sholat[7];
    char jam[5];
    char TimeName[][6] = {"SUBUH","TRBIT","DZUHR","ASHAR","TRBNM","MGHRB","ISYA'"};
    int hours, minutes;
    get_float_time_parts(times[i], hours, minutes);
    if (i==0 || i==2 || i==3 || i==5 || i==6) { //Tampilkan hanya Subuh, Dzuhur, Ashar, Maghrib, Isya
      sprintf(sholat,"%s",TimeName[i]); 
      sprintf(jam,"%02d:%02d",hours,minutes+ihti);     
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
  
}

void AlarmSholat() {

  DateTime now = rtc.now();

  int Hor = now.hour();
  int Min = now.minute();
  int Sec = now.second();

  JadwalSholat();
  int hours, minutes;

  get_float_time_parts(times[0], hours, minutes);
  int jamsubuh = hours;
  int menitsubuh = minutes+ihti;

  if (Hor == jamsubuh && Min == menitsubuh) {
    dmd.clearScreen();
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN"); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(6, 7, "SUBUH"); //koordinat tampilan
    Buzzer();
    Serial.println("SUBUH");
    delay(120000);//150000delay untuk adzan
    
    Buzzer();
    value_iqmh = value_iqmh + 2;    //Saat Subuh tambah 2 menit waktu Iqomah 
    Iqomah();
  }

  get_float_time_parts(times[2], hours, minutes);
  int jamdzuhur = hours;
  int menitdzuhur = minutes+ihti;

  if (Hor == jamdzuhur && Min == menitdzuhur) {
    dmd.clearScreen();
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN"); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(4, 7, "DZUHUR"); //koordinat tampilan
    Buzzer();
    Serial.println("DZUHUR");
    delay(120000);
    
    Buzzer();    
    Iqomah();
  }

  get_float_time_parts(times[3], hours, minutes);
  int jamashar = hours;
  int menitashar = minutes+ihti;

  if (Hor == jamashar && Min == menitashar) {
    dmd.clearScreen();
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN "); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(6, 7, "ASHAR"); //koordinat tampilan
    Buzzer();
    Serial.println("ASHAR");
    delay(120000);
    
    Buzzer();
    Iqomah();
  }

  get_float_time_parts(times[5], hours, minutes);
  int jammaghrib = hours;
  int menitmaghrib = minutes+ihti;

  if (Hor == jammaghrib && Min == menitmaghrib) {
    dmd.clearScreen();
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN "); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(1, 7, "MAGHRIB"); //koordinat tampilan
    Buzzer();
    Serial.println("MAGHRIB");
    delay(120000);
    
    Buzzer();
    Iqomah();
  }

  get_float_time_parts(times[6], hours, minutes);
  int jamisya = hours;
  int menitisya = minutes+ihti;

  if (Hor == jamisya && Min == menitisya) {
    dmd.clearScreen();
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN"); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(8, 7, "ISYA'"); //koordinat tampilan
    Buzzer();
    Serial.println("ISYA");
    delay(120000);
    
    Buzzer();
    Iqomah();
  }
  
}


//----------------------------------------------------------------------
//IQOMAH

void Iqomah() {

  JadwalSholat();

  DateTime now = rtc.now();
  //iqomah----------------------------------
  int langkah, i, hours, minutes;
  dmd.clearScreen();
  dmd.selectFont(Font3x5);
  //dmd.clearScreen( true );
  dmd.drawString(3, -2, "IQOMAH"); //koordinat tampilan
  int tampil;
  //value_iqmh = EEPROM.read(0);

  int detik = 0, menit = value_iqmh;
  for (detik = 0; detik >= 0; detik--) {
    delay(1000);
    String iqomah = Konversi(menit) + ":" + Konversi(detik);
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
      for (tampil = 0; tampil < 300 ; tampil++) { //<300s nilai tunda sholt
        menit = 0;
        detik = 0;
        dmd.clearScreen();
        dmd.selectFont(Font3x5);
        dmd.drawString(4, -2, "SHOLAT"); //koordinat tampilan
        ///////////////////////
        now = rtc.now();
        String jam = Konversi(now.hour()) + ":" + Konversi(now.minute()) + ":" + Konversi(now.second()) ; //tampilan jam
        dmd.selectFont(Font3x5);
        dmd.drawString(2, 7, jam);
        /////////////////////
        delay (1000);
      }
    }
  }
}


//----------------------------------------------------------------------
//TAMPILKAN JAM BESAR

void TampilJam() {
  DateTime now = rtc.now();

  dmd.clearScreen(); 

  //JAM
  String jam = Konversi(now.hour()) ; //tampilan jam  
  dmd.selectFont(angka6x13);
  dmd.drawString(3, 0, jam);

  //MENIT
  String menit = Konversi(now.minute()) ; //tampilan menit
  dmd.selectFont(Font3x5);
  dmd.drawString(22, -2, menit);

  //DETIK
  String detik = Konversi(now.second()) ; //tampilan menit
  dmd.selectFont(Font3x5);
  dmd.drawString(22, 6, detik);

  
  //DETIK TITIK DUA
  int n;
  n = now.second()+1;

  if(n % 2 == 0) {
    dmd.drawFilledBox(18,4,19,6, GRAPHICS_OFF);
    dmd.drawFilledBox(18,8,19,10, GRAPHICS_OFF);
  } else {
    dmd.drawFilledBox(18,4,19,6);
    dmd.drawFilledBox(18,8,19,10);
  }

  Serial.println(jam + " : " + menit + " : " + detik);
  delay(1000);
  
}

void TampilJamKecil() {
  DateTime now = rtc.now();
  
  String jam = Konversi(now.hour()) + ":" + Konversi(now.minute()); //tampilan jam
  
  dmd.clearScreen();
  dmd.selectFont(angka_2);
  dmd.drawString(1,0,jam);
  Serial.println(jam);  
  
}


//----------------------------------------------------------------------
//TAMPILKAN TANGGAL

void TampilTanggal() {
  DateTime now = rtc.now();
  
  String hari = weekDay[now.dayOfWeek()];
  String tanggal = Konversi(now.date()) + " " + monthYear[now.month()];
  
  dmd.clearScreen();
  dmd.selectFont(Font3x5);
  dmd.drawString(4,-2,hari);
  dmd.drawString(4,7,tanggal);
  Serial.println(hari + ", " + tanggal);
  delay(3000);
  
}


//----------------------------------------------------------------------
// TAMPILKAN SUHU

void TampilSuhu(){
  //Tampilkan Suhu
  dmd.clearScreen();
  dmd.selectFont(Font3x5);    
  dmd.drawString(9, -2, "SUHU");
  String suhu = Konversi(rtc.getTemperature()) + "C";
  dmd.selectFont(SystemFont5x7);
  dmd.drawString(8, 8, suhu);
  Serial.println("SUHU : " + suhu + "C");
  delay(3000);
}


//----------------------------------------------------------------------
// BUNYIKAN BEEP BUZZER

void Buzzer() {
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
  delay(50);
}

//----------------------------------------------------------------------
// TAMPILKAN SCROLLING TEKS YANG DIINPUT MELALUI WEBSITE

void WebTeks() {
  // Check if a client has connected
  WiFiClient client = server.available();
  
   // Read the first line of the request
  String command1 = client.readStringUntil('/');
  String command = client.readStringUntil('/');
  Serial.println(command);
  
  if (command == "text") {
    value = client.readStringUntil('/');
    value.replace("%20", " ");
    Serial.println(value);
  }
  
  else {  // Prepare the response
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    s += file1;  
    client.flush();
  
    // Send the response to the client
    while(s.length()>2000) {
      String dummy = s.substring(0,2000);
      client.print(dummy);
      s.replace(dummy," ");
    }
  
    client.print(s);
    delay(1);
    Serial.println("Client disconnected");
 
  }

  dmd.clearScreen();
  TampilJamKecil();
  dmd.selectFont(Font3x5); 
  DMD_TextBox box(dmd, 0, 8);
  String spasi = "          ";
  String logo = "GROBAK.NET - WEB DESIGN - WEB DEVELOPER" + spasi;
  String tampil = spasi + logo + value + spasi; 
  const char *MESSAGE = tampil.c_str();
  const char *next = MESSAGE;
  while(*next) {
    Serial.print(*next);
    box.print(*next);
    delay(200);
    next++;
  }
}



//----------------------------------------------------------------------
///Konversi angka agar ada nol didepannya jika diawah 10

String Konversi(int sInput) {
  if (sInput < 10)
  {
    return"0" + String(sInput);
  }
  else
  {
    return String(sInput);
  }
}
