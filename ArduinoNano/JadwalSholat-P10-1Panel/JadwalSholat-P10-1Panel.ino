/*
 * JADWAL WAKTU SHOLAT MENGGUNAKAN ARDUINO NANO, LED P10, RTC DS3241, BUZZER, PUSH BUTTON
 * FITUR :  JADWAL SHOLAT 5 WAKTU DAN TANBIH IMSAK, JAM BESAR, TANGGAL, SUHU, ALARAM ADZAN DAN TANBIH IMSAK,
 *          DAN HITUNG MUNDUR IQOMAH.
 * 

Pin on  DMD P10     GPIO      Arduino Nano          Pin on  DS3231      Arduino Nano      Pin on  Buzzer       Arduino Nano 

        2  A        GPIO16    D6                            SCL         A5                        +            D2 (GPIO 2)
        4  B        GPIO12    D7                            SDA         A4                        -            GND
        8  CLK      GPIO14    D13                           VCC         3V
        10 SCK      GPIO0     D8                            GND         GND
        12 R-Data   GPIO13    D11
        1  NOE      GPIO15    D9
        3  GND      GND       GND


Pin Button
    Button 1 +      A1
    Button 1 -      GND
    Button 2 +      A2
    Button 2 -      GND
    Button 3 +      A3
    Button 3 -      GND

email : bonny@grobak.net - www.grobak.net
*/


#include <DMD.h>
#include <TimerOne.h>
#include <Wire.h>
#include <Sodaq_DS3231.h>
#include "PrayerTimes.h"
#include <SystemFont5x7.h>
#include <angka6x13.h>
#include <angka_2.h>
#include <huruf_kecilku.h>
#include <Font3x5.h>
#include <SPI.h>
#include <EEPROM.h>

char weekDay[][7] = {" AHAD ","SENIN ","SELASA"," RABU ","KAMIS ","JUM'AT","SABTU"," AHAD "}; // array hari, dihitung mulai dari Ahad, hari Ahad angka nya =0.
char namaBulan[][13] = {" ","JAN","FEB","MAR","APR","MEI","JUN","JUL","AGU","SEP","OKT","NOV","DES"}; // Urutan nama bulan dihitung dari 0 sampai 12
int langkah;
int lama_tampilnya;
boolean Waduh_Setting = false;
//
int address = 0;
byte value_ihti=2, value_iqmh_subuh=10, value_iqmh_dzuhur=8, value_iqmh_ashar=7, value_iqmh_maghrib=7, value_iqmh_isya=7, value_hari, m_iqmh;
int addr = 0;
byte S_IQMH = 0, S_IHTI = 0, S_HARI = 0;

//BUZZER PIN
const int buzzer = 2; // Pin GPIO Buzzer - D2

///////////////////////////////// Mendifinisikan Pin Tombol ////////////////////////////////////////
#define Jam_bUP A1                  // Tombol setting sesuaikan dengan PIN di arduino anda mana  klo ini terhubung dengan PIN 3 Digital
#define tombol_bOK A2             // Tombol OK sesuaikan dengan PIN di arduino anda mana  klo ini terhubung dengan PIN 5 Digital
#define Jam_bDN A3           //tombol setting   sesuaikan dengan PIN di arduino anda mana  klo ini terhubung dengan PIN 4 Digital
///////////////////koding ini untuk setting rtc sesuai dengan laptop//// /////
//year, month, date, hour, min, sec and week-day(starts from 0 and goes to 6)
//writing any non-existent time-data may interfere with normal operation of the RTC.
//Take care of week-day also.
//DateTime dt(2018, 4, 8, 12, 52, 0, 6);

DMD dmd(1, 1); //untuk mengatur jumlah panel yang kamu pakai
double times[sizeof(TimeName) / sizeof(char*)];
void ScanDMD()
{
  dmd.scanDisplayBySPI();
}
//
void setup()
{
  pinMode(buzzer, OUTPUT);//----pin 2 untuk alarm adzan maghrib ---hubungkan dengan buzzer
  langkah = 0;
  rtc.begin();
  //rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above (ini untuk setting jam)
  Wire.begin();
  Timer1.initialize( 500 );
  Timer1.attachInterrupt( ScanDMD );
  dmd.clearScreen( true );

  pinMode(tombol_bOK, INPUT_PULLUP);                                 // Mode Pin Sebagai Input dengan Pull Up Internal
  pinMode(Jam_bUP, INPUT_PULLUP);                                    // Mode Pin Sebagai Input dengan Pull Up Internal
  pinMode(Jam_bDN, INPUT_PULLUP);                                    // Mode Pin Sebagai Input dengan Pull Up Internal
  //
  attachInterrupt(0, Setting, FALLING);
  BuzzerPendek();
}
////////////////////////////////// Fungsi Looping/Pengulangan ///////////////////////////////////////
void loop() {
  if (Waduh_Setting) {
    MenuSetting(); // Jika Tombol OK/Interrupt ditekan maka masuk menu setting Manual
  }
  mulai();
}
/////////////////////////////////// Interrupt Button Setting ////////////////////////////////////////
void Setting() {
  Waduh_Setting = true;                                                // Aktifkan Menu Setting
}
////////////////////////////////////// Fungsi Menu Setting //////////
void MenuSetting() {
  DateTime now = rtc.now();
  String Teks;
  int tgl = now.date();         //Tanggal
  int bln = now.month();        //Bulan
  int thn = now.year();         //Tahun
  int hours, minutes;        //jam dan menit jawdwal sholat
  int jame = now.hour();     //jam
  int menit_e = now.minute(); //menit
  int harin_e;  //hari
  detachInterrupt(0);// Matikan interrupt "0"
  delay(500);
  /////////////////////////////////////// Setting Jam & Menit ///////
  int kasus = 0;
setTimes:                                                           // Setting Times (Jam & Menit)
  if (digitalRead(Jam_bUP) == LOW && kasus == 1) {                               // Tombol Setting Menit
    delay(10); menit_e++;                                        // Delay Tombol & Tambah Nilai Menit
    if (menit_e >= 60) {
      menit_e = 0; // Batas Nilai Menit Lebih dari 59 Kembali ke 0
    }
    while (digitalRead(Jam_bUP) == LOW && kasus == 1) {}                         // Kunci Tombol Setting Menit Sampai Lepas Tombol
  }
  if (digitalRead(Jam_bDN) == LOW && kasus == 1) {                               // Tombol Setting Menit
    delay(10); jame++;                                          // Delay Tombol & Tambah Nilai Jam
    if (jame >= 24) {
      jame = 0; // Batas Nilai Jam Lebih dari 23 Kembali ke 0
    }
    while (digitalRead(Jam_bDN) == LOW && kasus == 1) {}                         // Kunci Tombol Setting Jam Sampai Lepas Tombol
  }
  //=========================================
setTgl:                                                           // Setting tanggal
  if (digitalRead(Jam_bDN) == LOW && kasus == 2) {                               // Tombol Setting tanggal
    delay(10); tgl++;
    if (tgl >= 32) {
      tgl = 1; // Batas Nilai tgl Lebih dari 31 Kembali ke 1
    }
    while (digitalRead(Jam_bDN) == LOW && kasus == 2) {}                         // Kunci Tombol Setting tgl Sampai Lepas Tombol
  }
  if (digitalRead(Jam_bUP) == LOW && kasus == 2) {                               // Tombol Setting bln
    delay(10); bln++;                                          // Delay Tombol & Tambah Nilai bln
    if (bln >= 13) {
      bln = 1; // Batas Nilai bln Lebih dari 12 Kembali ke 1
    }
    while (digitalRead(Jam_bUP) == LOW && kasus == 2) {}                         // Kunci Tombol Setting blnSampai Lepas Tombol
  }
  ///////=======setting thn==
setThn:                                                           // Setting thn
  if (digitalRead(Jam_bUP) == LOW && kasus == 3) {                               // Tombol Setting thn
    delay(10); thn++;                                        // Delay Tombol & Tambah Nilai thn
    if (thn >= 9999) {
      thn = 0; // Batas Nilai Menit Lebih dari 9999 Kembali ke 0
    }
    while (digitalRead(Jam_bUP) == LOW && kasus == 3) {}                         // Kunci Tombol Setting thn Sampai Lepas Tombol
  }
  if (digitalRead(Jam_bDN) == LOW && kasus == 3) {                               // Tombol Setting thn
    delay(10); thn--;                                          // Delay Tombol & Tambah Nilai thn
    if (thn <= 0) {
      thn = 0; // Batas Nilai kurang dari 0 Kembali ke 0
    }
    while (digitalRead(Jam_bDN) == LOW && kasus == 3) {}                         // Kunci Tombol Setting thn Sampai Lepas Tombol
  }
  // =======setting Hari======
  //hitungan hari mulai dari 0,1,2,3,4,5,6 = ahad, senin, selasa, rabu, kamis, jumat, sabtu
setHari:                                                           // Setting Times hari
  value_hari = EEPROM.read(6);//
  if (digitalRead(Jam_bUP) == LOW && kasus == 4) {                               // Tombol Setting hari
    //harin_e = harin_e;
    harin_e = value_hari;
    //harin_e = S_HARI;
    delay(50);
    harin_e++;                                        // Delay Tombol & Tambah Nilai hari,
    if (harin_e >= 7) {
      harin_e = 0; // Batas Nilai Menit Lebih dari 6 Kembali ke 0
    }
    while (digitalRead(Jam_bUP) == LOW && kasus == 4) {
      EEPROM.write(6, harin_e); // Kunci Tombol Setting hariSampai Lepas Tombol
    }
  }
  if (digitalRead(Jam_bDN) == LOW && kasus == 4) {                               // Tombol Setting hari
    //harin_e = harin_e;
    harin_e = value_hari;
    //harin_e = S_HARI;
    delay(50);
    harin_e--;                                          // Delay Tombol & Tambah Nilai hari
    if (harin_e < 0) {
      harin_e = 6; // Batas Nilai kurang dari 0 Kembali ke 0
    }
    while (digitalRead(Jam_bDN) == LOW && kasus == 4) {
      EEPROM.write(6, harin_e); // Kunci Tombol Setting hariSampai Lepas Tombol
    }
  }
  //====================================================>Set Iqomah<=================================================
setIQMHSubuh:
  value_iqmh_subuh = EEPROM.read(1);//value_sbh
  if (digitalRead(Jam_bUP) == LOW && kasus == 5) {                               // Tombol Setting hari
    S_IQMH = value_iqmh_subuh;//value_sbh;
    delay(50);
    S_IQMH++;                                        // Delay Tombol & Tambah Nilai hari,
    if (S_IQMH >= 60) {
      S_IQMH = 0; // Batas Nilai Menit Lebih dari 6 Kembali ke 0
    }
    EEPROM.write(1, S_IQMH);
    while (digitalRead(Jam_bUP) == LOW && kasus == 5) {}                         // Kunci Tombol Setting hariSampai Lepas Tombol
  }
  if (digitalRead(Jam_bDN) == LOW && kasus == 5) {                               // Tombol Setting hari
    S_IQMH = value_iqmh_subuh;//value_sbh;
    delay(50);
    S_IQMH--;                                        // Delay Tombol & Tambah Nilai hari,
    if (S_IQMH <= 0) {
      S_IQMH = 59; // Batas Nilai Menit Lebih dari 6 Kembali ke 0
    }
    EEPROM.write(1, S_IQMH);
    while (digitalRead(Jam_bDN) == LOW && kasus == 5) {}                         // Kunci Tombol Setting hari Sampai Lepas Tombol
  }

setIQMHDzuhur:
  value_iqmh_dzuhur = EEPROM.read(2);//value_sbh
  if (digitalRead(Jam_bUP) == LOW && kasus == 6) {                               // Tombol Setting hari
    S_IQMH = value_iqmh_dzuhur;//value_sbh;
    delay(50);
    S_IQMH++;                                        // Delay Tombol & Tambah Nilai hari,
    if (S_IQMH >= 60) {
      S_IQMH = 0; // Batas Nilai Menit Lebih dari 6 Kembali ke 0
    }
    EEPROM.write(2, S_IQMH);
    while (digitalRead(Jam_bUP) == LOW && kasus == 6) {}                         // Kunci Tombol Setting hariSampai Lepas Tombol
  }
  if (digitalRead(Jam_bDN) == LOW && kasus == 6) {                               // Tombol Setting hari
    S_IQMH = value_iqmh_dzuhur;//value_sbh;
    delay(50);
    S_IQMH--;                                        // Delay Tombol & Tambah Nilai hari,
    if (S_IQMH <= 0) {
      S_IQMH = 59; // Batas Nilai Menit Lebih dari 6 Kembali ke 0
    }
    EEPROM.write(2, S_IQMH);
    while (digitalRead(Jam_bDN) == LOW && kasus == 6) {}                         // Kunci Tombol Setting hari Sampai Lepas Tombol
  }

setIQMHAshar:
  value_iqmh_ashar = EEPROM.read(3);//value_sbh
  if (digitalRead(Jam_bUP) == LOW && kasus == 7) {                               // Tombol Setting hari
    S_IQMH = value_iqmh_ashar;//value_sbh;
    delay(50);
    S_IQMH++;                                        // Delay Tombol & Tambah Nilai hari,
    if (S_IQMH >= 60) {
      S_IQMH = 0; // Batas Nilai Menit Lebih dari 6 Kembali ke 0
    }
    EEPROM.write(3, S_IQMH);
    while (digitalRead(Jam_bUP) == LOW && kasus == 7) {}                         // Kunci Tombol Setting hariSampai Lepas Tombol
  }
  if (digitalRead(Jam_bDN) == LOW && kasus == 7) {                               // Tombol Setting hari
    S_IQMH = value_iqmh_ashar;//value_sbh;
    delay(50);
    S_IQMH--;                                        // Delay Tombol & Tambah Nilai hari,
    if (S_IQMH <= 0) {
      S_IQMH = 59; // Batas Nilai Menit Lebih dari 6 Kembali ke 0
    }
    EEPROM.write(3, S_IQMH);
    while (digitalRead(Jam_bDN) == LOW && kasus == 7) {}                         // Kunci Tombol Setting hari Sampai Lepas Tombol
  }

setIQMHMaghrib:
  value_iqmh_maghrib = EEPROM.read(4);//value_sbh
  if (digitalRead(Jam_bUP) == LOW && kasus == 8) {                               // Tombol Setting hari
    S_IQMH = value_iqmh_maghrib;//value_sbh;
    delay(50);
    S_IQMH++;                                        // Delay Tombol & Tambah Nilai hari,
    if (S_IQMH >= 60) {
      S_IQMH = 0; // Batas Nilai Menit Lebih dari 6 Kembali ke 0
    }
    EEPROM.write(4, S_IQMH);
    while (digitalRead(Jam_bUP) == LOW && kasus == 8) {}                         // Kunci Tombol Setting hariSampai Lepas Tombol
  }
  if (digitalRead(Jam_bDN) == LOW && kasus == 8) {                               // Tombol Setting hari
    S_IQMH = value_iqmh_maghrib;//value_sbh;
    delay(50);
    S_IQMH--;                                        // Delay Tombol & Tambah Nilai hari,
    if (S_IQMH <= 0) {
      S_IQMH = 59; // Batas Nilai Menit Lebih dari 6 Kembali ke 0
    }
    EEPROM.write(4, S_IQMH);
    while (digitalRead(Jam_bDN) == LOW && kasus == 8) {}                         // Kunci Tombol Setting hari Sampai Lepas Tombol
  }

setIQMHIsya:
  value_iqmh_isya = EEPROM.read(5);//value_sbh
  if (digitalRead(Jam_bUP) == LOW && kasus == 9) {                               // Tombol Setting hari
    S_IQMH = value_iqmh_isya;//value_sbh;
    delay(50);
    S_IQMH++;                                        // Delay Tombol & Tambah Nilai hari,
    if (S_IQMH >= 60) {
      S_IQMH = 0; // Batas Nilai Menit Lebih dari 6 Kembali ke 0
    }
    EEPROM.write(5, S_IQMH);
    while (digitalRead(Jam_bUP) == LOW && kasus == 9) {}                         // Kunci Tombol Setting hariSampai Lepas Tombol
  }
  if (digitalRead(Jam_bDN) == LOW && kasus == 9) {                               // Tombol Setting hari
    S_IQMH = value_iqmh_isya;//value_sbh;
    delay(50);
    S_IQMH--;                                        // Delay Tombol & Tambah Nilai hari,
    if (S_IQMH <= 0) {
      S_IQMH = 59; // Batas Nilai Menit Lebih dari 6 Kembali ke 0
    }
    EEPROM.write(5, S_IQMH);
    while (digitalRead(Jam_bDN) == LOW && kasus == 9) {}                         // Kunci Tombol Setting hari Sampai Lepas Tombol
  }
  //
setIHTI:
  value_ihti = EEPROM.read(0);//value_sbh
  if (digitalRead(Jam_bUP) == LOW && kasus == 10) {                               // Tombol Setting hari
    S_IHTI = value_ihti;//value_sbh;
    delay(50);
    S_IHTI++;                                        // Delay Tombol & Tambah Nilai hari,
    if (S_IHTI >= 60) {
      S_IHTI = 0; // Batas Nilai Menit Lebih dari 6 Kembali ke 0
    }
    EEPROM.write(0, S_IHTI);
    while (digitalRead(Jam_bUP) == LOW && kasus == 10) {}                         // Kunci Tombol Setting hariSampai Lepas Tombol
  }
  if (digitalRead(Jam_bDN) == LOW && kasus == 10) {                               // Tombol Setting hari
    S_IHTI = value_ihti;//value_sbh;
    delay(50);
    S_IHTI--;                                        // Delay Tombol & Tambah Nilai hari,
    if (S_IHTI <= 0) {
      S_IHTI = 59; // Batas Nilai Menit Lebih dari 6 Kembali ke 0
    }
    EEPROM.write(0, S_IHTI);
    while (digitalRead(Jam_bDN) == LOW && kasus == 10) {}                         // Kunci Tombol Setting hari Sampai Lepas Tombol
  }
  //
  ///============Tombol OKE =======================
  if (digitalRead(tombol_bOK) == LOW) {                               // Tombol Selesai Setting

    kasus = kasus + 1;
    dmd.clearScreen(true);                                                                 // Hapus Tampilan P10
    DateTime dt(thn, bln, tgl, jame, menit_e, 0, harin_e);
    // DateTime dt(2017, 5, 4, 15, 11, 0, 4);
    rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above
    while (digitalRead(tombol_bOK) == LOW) {}
    delay(500);
    //  ============================================kasus-kasus===================
  } if (kasus == 1) {     //kasus seting jam
    dmd.clearScreen(true);
    dmd.selectFont(Font3x5);
    dmd.drawString(1, -2, "SET JAM", 7, 0);
    String xjam = Konversi(jame) + ":" + Konversi(menit_e) ;
    int pjg = xjam.length() + 1;
    char sjam[pjg];
    xjam.toCharArray(sjam, pjg);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(0, 9, sjam, pjg, 0);
    delay (100);
    goto setTimes; //lari ke fungsi seting jam
  }
  if (kasus == 2) {  //kasus seting tanggal
    dmd.clearScreen(true);
    dmd.selectFont(Font3x5);
    dmd.drawString(1, -2, "TANGGAL", 7, 0);
    String xjam = Konversi(tgl) + "/" + Konversi(bln) ;
    int pjg = xjam.length() + 1;
    char sjam[pjg];
    xjam.toCharArray(sjam, pjg);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(0, 9, sjam, pjg, 0);
    delay (100);
    goto setTgl;//lari ke fungsi setting tanggal
  } if (kasus == 3) {  ///kasus seting tahun
    dmd.clearScreen(true);
    dmd.selectFont(Font3x5);
    dmd.drawString(1, -2, "TAHUN", 5, 0);
    String xjam = Konversi(thn)  ;
    int pjg = xjam.length() + 1;
    char sjam[pjg];
    xjam.toCharArray(sjam, pjg);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(0, 9, sjam, pjg, 0);
    delay (100);
    goto setThn; //lari ke fungsi setting tahun

  }

  if (kasus == 4) { //kasus seting hari
    value_hari = EEPROM.read(6);//
    harin_e = value_hari;
    dmd.clearScreen(true);
    dmd.selectFont(Font3x5);
    dmd.drawString(1, -2, "HARI", 4, 0);
    String xjam = Konversi(harin_e)  ;
    if (harin_e == 0) {
      xjam = "AHD";
      int pjg = xjam.length() + 1;
      char sjam[pjg];
      xjam.toCharArray(sjam, pjg);
      dmd.selectFont(SystemFont5x7);
      dmd.drawString(0, 9, sjam, pjg, 0);
    }  if (harin_e == 1) {
      xjam = "SEN";

      int pjg = xjam.length() + 1;
      char sjam[pjg];
      xjam.toCharArray(sjam, pjg);
      dmd.selectFont(SystemFont5x7);
      dmd.drawString(0, 9, sjam, pjg, 0);
    }
    if (harin_e == 2) {
      xjam = "SEL";

      int pjg = xjam.length() + 1;
      char sjam[pjg];
      xjam.toCharArray(sjam, pjg);
      dmd.selectFont(SystemFont5x7);
      dmd.drawString(0, 9, sjam, pjg, 0);
    } if (harin_e == 3) {
      xjam = "RAB";

      int pjg = xjam.length() + 1;
      char sjam[pjg];
      xjam.toCharArray(sjam, pjg);
      dmd.selectFont(SystemFont5x7);
      dmd.drawString(0, 9, sjam, pjg, 0);
    } if (harin_e == 4) {
      xjam = "KAM";

      int pjg = xjam.length() + 1;
      char sjam[pjg];
      xjam.toCharArray(sjam, pjg);
      dmd.selectFont(SystemFont5x7);
      dmd.drawString(0, 9, sjam, pjg, 0);
    } if (harin_e == 5) {
      xjam = "JUM";

      int pjg = xjam.length() + 1;
      char sjam[pjg];
      xjam.toCharArray(sjam, pjg);
      dmd.selectFont(SystemFont5x7);
      dmd.drawString(0, 9, sjam, pjg, 0);
    } if (harin_e == 6) {
      xjam = "SAB";

      int pjg = xjam.length() + 1;
      char sjam[pjg];
      xjam.toCharArray(sjam, pjg);
      dmd.selectFont(SystemFont5x7);
      dmd.drawString(0, 9, sjam, pjg, 0);
    }
    delay (100);
    goto setHari; //lari ke seting hari

  }
  //=======================================tambah mennu======================
  if (kasus == 5) {  ///kasus seting tahun

    dmd.clearScreen(true);
    dmd.selectFont(Font3x5);
    dmd.drawString(1, -2, "IQM-SBH", 7, 0); //(kolom,baris,"text",jmltext,batas ahir)    
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(0, 9, "  ", 2, 0);//S:
    dmd.drawString(24, 9, "M", 1, 0);
    String xjam = Konversi(value_iqmh_subuh);//(value_sbh)  ;
    int pjg = xjam.length() + 1;
    char sjam[pjg];
    xjam.toCharArray(sjam, pjg);
    dmd.drawString(11, 9, sjam, pjg, 0);
    delay (100);
    goto setIQMHSubuh; //lari ke fungsi setting iqomah subuh
  }

  if (kasus == 6) {  ///kasus seting tahun

    dmd.clearScreen(true);
    dmd.selectFont(Font3x5);
    dmd.drawString(1, -2, "IQM-DZH", 7, 0); //(kolom,baris,"text",jmltext,batas ahir)
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(0, 9, "  ", 2, 0);//S:
    dmd.drawString(24, 9, "M", 1, 0);
    String xjam = Konversi(value_iqmh_dzuhur);//(value_sbh)  ;
    int pjg = xjam.length() + 1;
    char sjam[pjg];
    xjam.toCharArray(sjam, pjg);
    dmd.drawString(11, 9, sjam, pjg, 0);
    delay (100);
    goto setIQMHDzuhur; //lari ke fungsi setting tahun
  }
  if (kasus == 7) {  ///kasus seting tahun

    dmd.clearScreen(true);
    dmd.selectFont(Font3x5);
    dmd.drawString(1, -2, "IQM-ASH", 7, 0); //(kolom,baris,"text",jmltext,batas ahir)
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(0, 9, "  ", 2, 0);//S:
    dmd.drawString(24, 9, "M", 1, 0);
    String xjam = Konversi(value_iqmh_ashar);//(value_sbh)  ;
    int pjg = xjam.length() + 1;
    char sjam[pjg];
    xjam.toCharArray(sjam, pjg);
    dmd.drawString(11, 9, sjam, pjg, 0);
    delay (100);
    goto setIQMHAshar; //lari ke fungsi setting tahun
  }
  if (kasus == 8) {  ///kasus seting tahun

    dmd.clearScreen(true);
    dmd.selectFont(Font3x5);
    dmd.drawString(1, -2, "IQM-MAG", 7, 0); //(kolom,baris,"text",jmltext,batas ahir)
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(0, 9, "  ", 2, 0);//S:
    dmd.drawString(24, 9, "M", 1, 0);
    String xjam = Konversi(value_iqmh_maghrib);//(value_sbh)  ;
    int pjg = xjam.length() + 1;
    char sjam[pjg];
    xjam.toCharArray(sjam, pjg);
    dmd.drawString(11, 9, sjam, pjg, 0);
    delay (100);
    goto setIQMHMaghrib; //lari ke fungsi setting tahun
  }
  if (kasus == 9) {  ///kasus seting tahun

    dmd.clearScreen(true);
    dmd.selectFont(Font3x5);
    dmd.drawString(1, -2, "IQM-ISY", 7, 0); //(kolom,baris,"text",jmltext,batas ahir)
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(0, 9, "  ", 2, 0);//S:
    dmd.drawString(24, 9, "M", 1, 0);
    String xjam = Konversi(value_iqmh_isya);//(value_sbh)  ;
    int pjg = xjam.length() + 1;
    char sjam[pjg];
    xjam.toCharArray(sjam, pjg);
    dmd.drawString(11, 9, sjam, pjg, 0);
    delay (100);
    goto setIQMHIsya; //lari ke fungsi setting tahun
  }
  
  if (kasus == 10) {  ///kasus seting tahun

    dmd.clearScreen(true);
    dmd.selectFont(Font3x5);
    dmd.drawString(1, -2, "IHTIYATI", 8, 0); //(kolom,baris,"text",jmltext,batas ahir)
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(0, 9, "  ", 3, 0);//S:
    dmd.drawString(24, 9, "M", 1, 0);//24
    String xjam = Konversi(value_ihti);//(value_sbh)  ;
    int pjg = xjam.length() + 1;
    char sjam[pjg];
    xjam.toCharArray(sjam, pjg);
    dmd.drawString(11, 9, sjam, pjg, 0);//11
    delay (100);
    goto setIHTI; //lari ke fungsi setting tahun
  }
  if (kasus == 11) {       //kasus wis Rampung
    dmd.clearScreen(true);
    goto endSetting;
    delay(500);
  }

endSetting:                                                         // Label End Setting
  attachInterrupt(0, Setting, FALLING);                               // Interrupt Tombol OK
  Waduh_Setting = false;

  kasus = 0;


}



//////////////////////==========================Program Jam==============

void mulai()
{
  TampilJam();
  AlarmSholat();

  TampilTanggal();
  AlarmSholat();
  TampilSuhu();
  AlarmSholat();

  TampilJadwalSholat();
  AlarmSholat();
  
  RunningText();
  AlarmSholat();


}


//----------------------------------------------------------------------
//TAMPILKAN JAM BESAR

void TampilJam() {
  
  DateTime now = rtc.now();
  
  dmd.clearScreen( true );
  dmd.selectFont(angka6x13);
  int menit_e = now.minute();
  int jame = now.hour();
  
  String xjam = Konversi(jame) ; //tampilan jam
  int pjg = xjam.length() + 1;
  char sjam[pjg];
  xjam.toCharArray(sjam, pjg);
  dmd.drawString(1, 0, sjam, pjg, 0);
  
  /////menit///
  String xxjam = Konversi(now.minute()) ; //tampilan menit
  int xpjg = xxjam.length() + 1;
  char xsjam[xpjg];
  xxjam.toCharArray(xsjam, xpjg);
  dmd.drawString(18, 0, xsjam, xpjg, 0);
  
  ///titik dua :
  
  dmd.selectFont(SystemFont5x7);
  dmd.drawChar( 14,  4, ':', GRAPHICS_NOR   );   // clock colon overlay on
  delay( 1000 );
  dmd.drawChar( 14, 4, ':', GRAPHICS_OR   );   // clock colon overlay off
  delay( 1000 );
  ///////////////interupsi tombol menu-----------
  if (digitalRead(tombol_bOK) == LOW) {
    MenuSetting(); // Jika Tombol OK/Interrupt ditekan maka masuk menu setting Manual
  }
  /////////////////////
  dmd.selectFont(SystemFont5x7);
  dmd.drawChar( 14,  4, ':', GRAPHICS_NOR   );   // clock colon overlay on
  delay( 1000 );
  dmd.drawChar( 14, 4, ':', GRAPHICS_OR   );   // clock colon overlay off
  delay( 1000 );
  dmd.drawChar( 14,  4, ':', GRAPHICS_NOR   );   // clock colon overlay on
  delay( 1000 );
  dmd.drawChar( 14, 4, ':', GRAPHICS_OR   );   // clock colon overlay off
  delay( 1000 );
  ///////////////interupsi tombol menu-----------
  if (digitalRead(tombol_bOK) == LOW) {
    MenuSetting(); // Jika Tombol OK/Interrupt ditekan maka masuk menu setting Manual
  }
  /////////////////////
  dmd.selectFont(SystemFont5x7);
  dmd.drawChar( 14,  4, ':', GRAPHICS_NOR   );   // clock colon overlay on
  delay( 1000 );
  dmd.drawChar( 14, 4, ':', GRAPHICS_OR   );   // clock colon overlay off
  delay( 1000 );
  dmd.drawChar( 14,  4, ':', GRAPHICS_NOR   );   // clock colon overlay on
  delay( 1000 );
  dmd.drawChar( 14, 4, ':', GRAPHICS_OR   );   // clock colon overlay off
  delay( 1000 );
  ///////////////interupsi tombol menu-----------
  if (digitalRead(tombol_bOK) == LOW) {
    MenuSetting(); // Jika Tombol OK/Interrupt ditekan maka masuk menu setting Manual
  }
  /////////////////////
  dmd.selectFont(SystemFont5x7);
  dmd.drawChar( 14,  4, ':', GRAPHICS_NOR   );   // clock colon overlay on
  delay( 1000 );
  dmd.drawChar( 14, 4, ':', GRAPHICS_OR   );   // clock colon overlay off
  delay( 1000 );
  dmd.drawChar( 14,  4, ':', GRAPHICS_NOR   );   // clock colon overlay on
  delay( 1000 );
  dmd.drawChar( 14, 4, ':', GRAPHICS_OR   );   // clock colon overlay off
  delay( 1000 );
  dmd.selectFont(SystemFont5x7);
  dmd.drawChar( 14,  4, ':', GRAPHICS_NOR   );   // clock colon overlay on
  delay( 1000 );

}


//------------------------------------------------------
//RUNNING TEXT

void RunningText() {
  dmd.clearScreen( true );
  DateTime now = rtc.now();
  String Teks;
  int tgl = now.date();         //Tanggal
  int bln = now.month();        //Bulan
  int thn = now.year();         //Tahun
  // int hours, minutes;        //jam dan menit jawdwal sholat
  int jame = now.hour();     //jam
  int menit_e = now.minute(); //menit
  int detik_e = now.second(); //detik
  int harin_e = harin_e;  //hari
  delay(100);
  detachInterrupt(0);// Matikan interrupt "0"
  ////   // Tampilkan hari
  dmd.selectFont(angka_2);
  String hariku = Konversi(now.hour()) + ":" + Konversi(now.minute()) ;
  int dowo = hariku.length() + 1;
  char detikanku[dowo];
  hariku.toCharArray(detikanku, dowo);
  dmd.drawString(2, 0, detikanku, dowo, 0);
  Teks = "GROBAK.NET" ; ///tampilkan teks
  int kecepatan;
  kecepatan = 35;     //kecepatan runing teks
  int pj = Teks.length() + 1;
  char tampil[pj];
  Teks.toCharArray(tampil, pj);
  dmd.selectFont(SystemFont5x7);
  dmd.drawMarquee(tampil, pj, 64, 9);
  //
  long timer = millis();
  boolean ret = false;
  while (!ret) {
    if ((timer + kecepatan) < millis()) {
      ret = dmd.stepSplitMarquee(8, 15, 0); ///dianimasikan baris 8 sampai 15, sampai kolom 0
      timer = millis();

      ///TOMBOL SET JAM//
      if (digitalRead(tombol_bOK) == LOW) {
        MenuSetting(); // Jika Tombol OK/Interrupt ditekan maka masuk menu setting Manual


      }
    }
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
  char TimeName[][8] = {" SUBUH","TERBIT"," DZUHUR"," ASHAR","TERBENAM","MAGHRIB","  ISYA", "WAKTU"};
  int hours, minutes;

  for (int i=0;i<8;i++){

    get_float_time_parts(times[i], hours, minutes);

    value_ihti = EEPROM.read(0);
    minutes = minutes + value_ihti;
    
    if (minutes >= 60) {
      minutes = minutes - 60;
      hours ++;
    }
    else {
      ;
    }
    
    if (i==0 || i==2 || i==3 || i==5 || i==6) { //Tampilkan hanya Subuh, Dzuhur, Ashar, Maghrib, Isya
      sprintf(sholat,"%s",TimeName[i]); 
      sprintf(jam,"%02d:%02d", hours, minutes);
      dmd.clearScreen(true);
      dmd.selectFont(Font3x5);
      dmd.drawString(1,-2, sholat, 7, 0);
      dmd.selectFont(angka_2);
      dmd.drawString(1,8,jam, 5, 0);
      Serial.println(sholat);
      Serial.println(" : ");
      Serial.println(jam);
      delay(2000);

      ///////////////interupsi tombol menu-----------
      if (digitalRead(tombol_bOK) == LOW) {
        MenuSetting(); // Jika Tombol OK/Interrupt ditekan maka masuk menu setting Manual
    
      }
      
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
  dmd.clearScreen(true);
  dmd.selectFont(Font3x5);
  dmd.drawString(4,-2,"TANBIH", 6, 0);
  dmd.selectFont(angka_2);
  dmd.drawString(1,8,jam, 5, 0);
  Serial.print("TANBIH");
  Serial.println(" : ");
  Serial.println(jam);
  delay(2000);

  ///////////////interupsi tombol menu-----------
  if (digitalRead(tombol_bOK) == LOW) {
    MenuSetting(); // Jika Tombol OK/Interrupt ditekan maka masuk menu setting Manual

  }
  
}


//--------------------------------------------------
//ALARM ADZAN WAKTU SHOLAT

void AlarmSholat() {

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
    dmd.clearScreen(true);
    dmd.selectFont(Font3x5);
    dmd.drawString(4, -2, "TANBIH", 6, 0); //koordinat tampilan
    dmd.drawString(6, 7, "IMSAK", 5, 0); //koordinat tampilan
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
    dmd.clearScreen(true);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN", 5, 0); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(6, 7, "SUBUH", 5, 0); //koordinat tampilan
    BuzzerPanjang();
    Serial.println("SUBUH");
    delay(180000);//180 detik atau 3 menit untuk adzan
    
    BuzzerPendek();
    m_iqmh = EEPROM.read(1);    //Saat Subuh tambah 2 menit waktu Iqomah 
    Iqomah();
  }

  // Dzuhur
  get_float_time_parts(times[2], hours, minutes);
  minutes = minutes + value_ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }
  
  if (Hor == hours && Min == minutes && Hari != 5) {
    
    dmd.clearScreen(true);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN", 5, 0); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(4, 7, "DZUHUR", 6, 0); //koordinat tampilan
    BuzzerPanjang();
    Serial.println("DZUHUR");
    delay(180000);//180 detik atau 3 menit untuk adzan
    
    BuzzerPendek();
    m_iqmh = EEPROM.read(2);
    Iqomah();
    
  } else if (Hor == hours && Min == minutes && Hari == 5) { 
    
    dmd.clearScreen(true);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN", 5, 0); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(4, 7, "JUM'AT", 6, 0); //koordinat tampilan
    BuzzerPanjang();
    Serial.println("Adzan Jum'at");
    delay(180000);//180 detik atau 3 menit untuk adzan
  }

  // Ashar
  get_float_time_parts(times[3], hours, minutes);
  minutes = minutes + value_ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }
  
  if (Hor == hours && Min == minutes) {
    dmd.clearScreen(true);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN ", 6, 0); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(6, 7, "ASHAR", 5, 0); //koordinat tampilan
    BuzzerPanjang();
    Serial.println("ASHAR");
    delay(180000);//180 detik atau 3 menit untuk adzan
    
    BuzzerPendek();
    m_iqmh = EEPROM.read(3);
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
    dmd.clearScreen(true);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN ", 6, 0); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(1, 7, "MAGHRIB", 7, 0); //koordinat tampilan
    BuzzerPanjang();
    Serial.println("MAGHRIB");
    delay(180000);//180 detik atau 3 menit untuk adzan
    
    BuzzerPendek();
    m_iqmh = EEPROM.read(4);
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
    dmd.clearScreen(true);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 0, "ADZAN", 5, 0); //koordinat tampilan
    dmd.selectFont(Font3x5);
    dmd.drawString(8, 7, "ISYA'", 5, 0); //koordinat tampilan
    BuzzerPanjang();
    Serial.println("ISYA");
    delay(180000);//180 detik atau 3 menit untuk adzan
    
    BuzzerPendek();
    m_iqmh = EEPROM.read(5);  
    Iqomah();
  }
  
}


//--------------------------------------------------
//IQOMAH

void Iqomah() {

  DateTime now = rtc.now();
  //iqomah----------------------------------
  int langkah;
  int waktu_iqomah;
  int panjang_iqomah;
  int iqomah;
  dmd.clearScreen( true );
  dmd.selectFont(Font3x5);
  //dmd.clearScreen( true );
  dmd.drawString(3, -2, "IQOMAH", 6, 0); //koordinat tampilan
  int tampil;
  int detik = 0, menit = m_iqmh;
  for (detik = 0; detik >= 0; detik--) {
    delay(1000);
    String iqomah = Konversi(menit) + ":" + Konversi(detik);
    int panjang_iqomah = iqomah.length() + 1;
    char waktu_iqomah[panjang_iqomah];
    iqomah.toCharArray(waktu_iqomah, panjang_iqomah);
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(1, 9, waktu_iqomah, panjang_iqomah, 0);
    if (detik <= 0) {
      detik = 60;
      menit = menit - 1;
    } if (menit <= 0 && detik <= 1) {
      dmd.clearScreen( true );
      digitalWrite(2, HIGH);//alarm Iqomah
      delay(1000);
      digitalWrite(2, LOW);//alarm Iqomah
      delay(50);
      dmd.selectFont(Font3x5);
      dmd.drawString(2, -2, "LURUS 8", 7, 0); //koordinat tampilan
      dmd.drawString(0, 7, "RAPATKAN", 8, 0); //koordinat tampilan
      delay(10000);
      for (tampil = 0; tampil < 300 ; tampil++) { //< tampil selama 300 detik waktu saat sholat
        menit = 0;
        detik = 0;
        dmd.clearScreen( true );
        dmd.selectFont(Font3x5);
        dmd.drawString(4, -2, "SHOLAT", 6, 0); //koordinat tampilan
        ///////////////////////
        now = rtc.now();
        String xjam = Konversi(now.hour()) + ":" + Konversi(now.minute()) + ":" + Konversi(now.second()) ; //tampilan jam
        int pjg = xjam.length() + 1;
        char sjam[pjg];
        xjam.toCharArray(sjam, pjg);
        dmd.selectFont(Font3x5);
        dmd.drawString(2, 7, sjam, pjg, 0);
        /////////////////////
        delay (1000);
      }
    }
  }
}



// Tampil Tanggal Baris Atas, Tampil Hari Baris Bawah

void TampilTanggal() {
  
    DateTime now = rtc.now();    
    
    char tanggalan[5];
    
    sprintf(tanggalan,"%02d %s", now.date(), namaBulan[now.month()]);

    dmd.clearScreen( true );
    dmd.selectFont(Font3x5);
    dmd.drawString(4, -2, (weekDay[now.dayOfWeek()]), 6, 0);   //tampilan hari
    dmd.drawString(4, 7, tanggalan, 6, 0);                     //tampilan tanggal dan bulan contoh 08 APR
    
    delay(3000);
}

void TampilSuhu(){
  //Tampilkan Suhu
  dmd.clearScreen( true );
  dmd.selectFont(Font3x5);    
  dmd.drawString(9, -2, "SUHU", 5, 0);
  String suhuku = Konversi(rtc.getTemperature() - 1) + "C";
  int suhul = suhuku.length() + 1;
  char suhu[suhul];
  suhuku.toCharArray(suhu, suhul);
  dmd.selectFont(SystemFont5x7);
  dmd.drawString(8, 9, suhu, suhul, 0);
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
///konversi angka agar ada nol didepannya jika diawah 10

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
