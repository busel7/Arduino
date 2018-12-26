/* Jadwal Waktu Sholat NodeMCU + RTC DS3231 + Led Matrix MAX7219 + Buzzer + Power Bank
 * 
 * bonny@grobak.net

Pengkabelan:

NodeMCU             Matrix
---------------------------
3V                  VCC
G                   GND
D7-GPIO13           DIN
D5-GPIO14           CLK
D3-GPIO0            CS(LOAD)


NodeMCU             DS3231
--------------------------
D1-GPIO5            SCL
D2-GPIO4            SDA
3V                  VCC
G                   GND


NodeMCU             Buzzer
--------------------------
D6-GPIO12           +
G                   -

*/


#include <stdio.h>
#include <Wire.h>
#include "Sodaq_DS3231.h"
#include "PrayerTimes.h"
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <Fonts/Picopixel.h>

// Ubah nama hari dan nama bulan
char weekDay[][3] = {"AH","SN","SL","RB","KM","JM","SB","AH"};
char monthYear[][4] = { " ", "JAN", "FEB", "MAR", "APR", "MEI", "JUN", "JUL", "AGU", "SEP", "OKT", "NOV", "DES" };

// Inisiasi Jadwal Sholat
double times[sizeof(TimeName)/sizeof(char*)];
int ihti = 2;   // Koreksi Waktu Menit Jadwal Sholat
int value_iqmh;  // Waktu Iqomah 10 menit

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
const int buzzer = 12;


void setup() {

  //Matrix Display
  matrix.setIntensity(1); // Matrix brightness from 0 to 15
    
  // The initial coordinates of the matrices 8 * 8
  matrix.setRotation(0, 1);        // 1 matrix
  matrix.setRotation(1, 1);        // 2 matrix
  matrix.setRotation(2, 1);        // 3 matrix
  matrix.setRotation(3, 1);        // 4 matrix

  //Buzzer
  pinMode(buzzer, OUTPUT);      // inisiasi pin untuk buzzer
  
  Serial.begin(115200);   //Starts serial connection
  Wire.begin();
  rtc.begin();

  //SETTING WAKTU
  //year, month, date, hour, min, sec and week-day(starts from 0 and goes to 6)
  //writing any non-existent time-data may interfere with normal operation of the RTC.
  //Take care of week-day also.
  
  //DateTime dt(2018, 12, 26, 17, 33, 0, 2);
  //rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above 

  BuzzerPendek();
  
}

int updCnt = 0;
long clkTime = 0;

void loop() {

  TampilJam();            //Tampilkan Jam
  
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

void TampilTanggal(){

  DateTime now = rtc.now();

  char tanggal[18];

  sprintf(tanggal, "%s,%02d%s",             //%02d allows to print an integer with leading zero 2 digit to the string, %s print sebagai string
                  weekDay[now.dayOfWeek()],   //ambil method hari dalam format lengkap
                  now.date(),                 //get day method
                  monthYear[now.month()]     //get month method
  );
  
  Serial.println(tanggal);    //print the string to the serial port

  TeksStatis(tanggal);
  delay(3000);

}

void TampilJam(){

  DateTime now = rtc.now();

  char jam[8];

  sprintf(jam, "%02d:%02d:%02d",    //%02d print jam dengan format 2 digit
                  now.hour(),       //get hour method
                  now.minute(),     //get minute method
                  now.second()      //get second method
  );
  
  Serial.println(jam);    //print the string to the serial port

  TeksStatis(jam);

  delay(1000);            //second delay

}

void TampilSuhu() {

  int temp = rtc.getTemperature();
  char suhu[2];
  int koreksisuhu = 3; // Perkiraan selisih suhu ruangan dan luar ruangan
  
  sprintf(suhu, "SUHU %dC", temp - koreksisuhu);
  
  TeksStatis(suhu);
    
}

void p(char *fmt, ... ){
        char tmp[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(tmp, 128, fmt, args);
        va_end (args);
        Serial.print(tmp);
}


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
  char TimeName[][8] = {"SU","TE","DZ","AS","TE","M.","IS","WA"};
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

  DateTime now = rtc.now();

  int Hari = now.dayOfWeek();
  int Hor = now.hour();
  int Min = now.minute();
  int Sec = now.second();

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

  DateTime now = rtc.now();
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
        now = rtc.now();
        sprintf(iqomah,"SH %02d:%02d",now.hour(),now.minute());  
        TeksStatis(iqomah);
        /////////////////////
        delay (1000);
      }
    }
  }
}


// =======================================================================
void TeksStatis(String text){

  matrix.fillScreen(LOW);

  // Setting font untuk jam
  spacer = 2;
  width = 3 + spacer;
  matrix.setFont(&Picopixel);

  for (int i=0; i<text.length(); i++){
  
  int letter =(matrix.width())- i * (width-1);
  int x = (matrix.width() +0) -letter;
  int y = 5; // Center text on Vertical
  matrix.drawChar(x, y, text[i], HIGH, LOW, 1);
  
  //delay(75);
  
  matrix.write(); // Display

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
