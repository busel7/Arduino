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
- HJS589(DMD3 porting for ESP8266 by Ahmad Herman) 
  < DMD3 by Azis Kurosaki 
  < DMD2(https://github.com/freetronics/DMD2) 
  < DMD (https://github.com/freetronics/DMD)
- PrayerTime : https://github.com/asmaklad/Arduino-Prayer-Times
- RTC DS3231 : https://github.com/Makuna/Rtc
- ArduinoJson V6 : https://github.com/bblanchon/ArduinoJson

Diskusi Grup Untuk1000Masjid : https://www.facebook.com/groups/761058907424496/

Tools : http://dotmatrixtool.com

www.grobak.net - www.elektronmart.com

Updated : 6 Oktober 2019
*/

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

// BUZZER
const int buzzer = 3; // Pin GPIO Buzzer - RX



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

  Rtc.SetDateTime(RtcDateTime(2019, 10, 11, 22, 20, 50));
  
  Serial.println("Setup RTC selesai");
  

  //DMD
  Disp_init();
  
  Disp.setBrightness(20);

  
}




//----------------------------------------------------------------------
// LOOP


void loop() {
  
  JamBesar();
  //TampilTanggal();
  //TampilSuhu();
  TampilJadwalSholat(); 
  //LogoA(47);
  //LogoM(0);

  //TeksBerjalanKananKiri();
  //TeksBerjalanKiriKanan();

  AlarmSholat();
  
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
    
  }
  
  
}


//----------------------------------------------------------------------
// FORMAT TEKS

void textCenter(int y,String Msg) {
  
  int center = int((Disp.width()-Disp.textWidth(Msg)) / 2);
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

  if(cM - pM > Kecepatan) {

    pM = cM;

    if (x < fullScroll) {

      ++x;
      
    } else {

      x = 0;
      
    }

    Disp.drawText(width - x, 0, nama1);
    
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


void LogoA(uint8_t x) {

  static const uint8_t LogoA[] PROGMEM = {

    16, 16,

    0x00, 0x00, 0x7f, 0xfe, 0x7f, 0xfe, 0x00, 0x00, 0x7e, 0x66, 0x7e, 0x66, 0x06, 0x66, 0x06, 0x66, 0x7e, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7f, 0xfe, 0x7f, 0xfe, 0x00, 0x00

  };
  
  Disp.drawBitmap(x, 0, LogoA);
}


void LogoM(uint8_t x) {

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
    Disp.drawText(33, 0, sholat);
    Disp.drawText(33, 8, jam);

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
      Disp.drawText(33, 0, "TANBIH");
      Disp.drawText(33, 8, jam);
      
      if (i > 8) {
        i = 0;
        Disp.drawRect(31,0,64,15,0,0);
      }
      
    }  

    
    
  }
  
}



//----------------------------------------------------------------------
//TAMPILKAN TANGGAL

void TampilTanggal() {

  RtcDateTime now = Rtc.GetDateTime();

  char hari[8];
  char tanggal[7]; // 21 SEP

  sprintf(hari, "%s", weekDay[now.DayOfWeek()]);
  sprintf(tanggal, "%02d %s", now.Day(), monthYear[now.Month()]);

  Disp.setFont(ElektronMart5x6);
  Disp.drawText(33, 0, hari);
  Disp.drawText(33, 8, tanggal);
  
}



//----------------------------------------------------------------------
// TAMPILKAN SUHU

void TampilSuhu() {

  RtcTemperature temp = Rtc.GetTemperature();
  int celsius = temp.AsFloatDegC();

  char suhu[3];
  int koreksisuhu = 2; // Perkiraan selisih suhu mesin dengan suhu ruangan

  sprintf(suhu, "%02d*C", celsius - koreksisuhu);
  
  Disp.setFont(ElektronMart5x6);
  Disp.drawText(33, 0, "SUHU");
  Disp.drawText(33, 8, suhu);
  
}



//----------------------------------------------------------------------
// TAMPILKAN JAM

void JamBesar() {

  RtcDateTime now = Rtc.GetDateTime();

  char jam[3];
  char menit[3];
  char detik[3];

  sprintf(jam, "%02d", now.Hour());
  sprintf(menit, "%02d", now.Minute());
  sprintf(detik, "%02d", now.Second());


  Disp.setFont(ElektronMart6x16);
  Disp.drawText(1,0,jam);

  Disp.drawRect(16,4,17,6,1,1);
  Disp.drawRect(16,9,17,11,1,1);
  
  Disp.setFont(ElektronMart5x6);
  Disp.drawText(20,0,menit);
  Disp.drawText(20,8,detik);
  
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
