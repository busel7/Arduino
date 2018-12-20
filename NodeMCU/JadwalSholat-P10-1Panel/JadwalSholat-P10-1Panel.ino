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
- RTC DS3231 : https://github.com/SodaqMoja/Sodaq_DS3231

email : bonny@grobak.net - www.grobak.net
*/



/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//#include "espneotext.h"
#include <SPI.h>
#include <EEPROM.h>

#include <Wire.h>
#include <Sodaq_DS3231.h>



#ifndef APSSID
#define APSSID "JWSGrobakNet"
#define APPSK  "grobaknet"
#endif

#include <DMD2.h>
#include <fonts/Arial_Black_16.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/angka6x13.h>
#include <fonts/angka_2.h>
#include <fonts/Font3x5.h>

#include "PrayerTimes.h"

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

//SETUP WIFI AP
/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);


String value;

void handleRoot() {
  server.send(200, "text/html", "<h1>Kamu sudah tersambung</h1>");
}


// setup rutinitas yang hanya dilakukan satu kali saat tekan reset:
void setup() {

  delay(1000);
  Serial.begin(115200);
  Serial.println();
  
  
//  Serial.print("Configuring access point...");

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();

  //IPAddress local_IP(192,168,7,77);
  //IPAddress gateway(192,168,7,250);
  //IPAddress subnet(255,255,255,0);
  
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
  
  //RTC D3231
  Wire.begin();
  rtc.begin();
  //rtc.setDateTime(dt); // Setting tanggal dan waktu untuk disimpan di RTC DS3231 sesuai parameter Datetime dt() di atas

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
  
  //server.handleClient();



  AlarmSholat(); // Banyak dipanggil class AlarmSholat() ini agar waktu sholat lebih akurat
  delay(1);
  TampilJam();
  delay(1);
  if(millis()-transisi > 15000) { // Tampilkan Tanggal pada detik ke 15
    AlarmSholat();
    TampilTanggal();
    delay(1);
  }
  
  if(millis()-transisi > 18000) { // Tampilkan Suhu pada detik ke 18

    AlarmSholat();
    TampilSuhu();
    delay(1);
    
    AlarmSholat();
    TampilJadwalSholat();
    delay(1);
    
    transisi = millis();
  }

}


//----------------------------------------------------------------------
//JADWAL SHOLAT

void JadwalSholat() {
  delay(1);

  DateTime now = rtc.now();

  int tahun = now.year();
  int bulan = now.month();
  int tanggal = now.date();

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
  delay(1);
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
  delay(1);

  DateTime now = rtc.now();

  int Hari = now.dayOfWeek();
  int Hor = now.hour();
  int Min = now.minute();
  int Sec = now.second();

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
  delay(1);

  DateTime now = rtc.now();
  //iqomah----------------------------------
  int langkah, i, hours, minutes;
  dmd.clearScreen();
  dmd.selectFont(Font3x5);
  //dmd.clearScreen( true );
  dmd.drawString(3, -2, "IQOMAH"); //koordinat tampilan
  int tampil;
  char iqomah[7];
  //value_iqmh = EEPROM.read(0);

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
        now = rtc.now();
        //String jam = Konversi(now.hour()) + ":" + Konversi(now.minute()) + ":" + Konversi(now.second()) ; //tampilan jam
        char jam[7];
        sprintf(jam, "%02d : %02d : %02d", now.hour(), now.minute(), now.second());
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
  delay(1);
  DateTime now = rtc.now();
  char jam[2];
  char menit[2];
  char detik[2];

  dmd.clearScreen(); 

  //JAM
  sprintf(jam,"%02d", now.hour());
  dmd.selectFont(angka6x13);
  dmd.drawString(3, 0, jam);

  //MENIT
  sprintf(menit,"%02d", now.minute());
  dmd.selectFont(Font3x5);
  dmd.drawString(22, -2, menit);

  //DETIK
  sprintf(detik,"%02d", now.second());
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

  //Serial.println(jam + " : " + menit + " : " + detik);
  delay(1000);
  
}

void TampilJamKecil() {
  delay(1);
  DateTime now = rtc.now();
  char jam[7];
  
  //String jam = Konversi(now.hour()) + ":" + Konversi(now.minute()); //tampilan jam
  sprintf(jam, "%02d : %02d", now.hour(), now.minute());
  
  dmd.clearScreen();
  dmd.selectFont(angka_2);
  dmd.drawString(1,0,jam);
  Serial.println(jam);  
  
}


//----------------------------------------------------------------------
//TAMPILKAN TANGGAL

void TampilTanggal() {
  delay(1);
  DateTime now = rtc.now();
  
  String hari = weekDay[now.dayOfWeek()];
  
  char tanggal[18];
  sprintf(tanggal, "%02d",             //%02d allows to print an integer with leading zero 2 digit to the string, %s print sebagai string
                  now.date()           //get day method
  );

  String bulan = monthYear[now.month()];
  
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
  delay(1);
  //Tampilkan Suhu
  int temp = rtc.getTemperature();
  char suhu[2];
  dmd.clearScreen();
  dmd.selectFont(Font3x5);    
  dmd.drawString(9, -2, "SUHU");
  sprintf(suhu,"%dC",temp);
  dmd.selectFont(SystemFont5x7);
  dmd.drawString(8, 8, suhu);
  //Serial.println("SUHU : " + suhu + "C");
  delay(3000);
}


//----------------------------------------------------------------------
// BUNYIKAN BEEP BUZZER

void BuzzerPanjang() {
  delay(1);
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
  delay(1);
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
  delay(1);
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
