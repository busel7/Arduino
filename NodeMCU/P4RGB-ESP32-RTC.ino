/*

    Demo P4 RGB 64x32 HUB75E menggunakan NodeMCU ESP32S (Yellow Pin) dan RTC DS3231

    Wiring
    ---------
    
    HUB75E    NodeMCU 32      RTC DS3231  NodeMCU 32
    --------------------      ----------------------
    R1        2               SCL         33
    G1        15              SDA         32
    B1        4               VCC         3.3V
    R2        16              GND         GND
    G2        27
    B2        17
    A         5
    B         18
    C         19
    D         21
    E         12
    LAT       26
    OE        25
    CLK       22


    Bonny Useful (ElektronMart.Com)

    Library : 
    o SmartMatrix - Louis Beaudoin (Pixelmatix) - https://github.com/pixelmatix/SmartMatrix/tree/teensylc
    o RTC - Makuna - https://github.com/Makuna/Rtc
    o F1kM_Hisab - Wardi Utari - https://github.com/wardi1971/F1kM_Hisab
    
    PCB :
    o https://www.tokopedia.com/elektronmartcom/pcb-controller-led-rgb-nodemcu-esp-32-esp32-smartmatrix-arduino
    o https://www.bukalapak.com/p/elektronik/elektronik-lainnya/35vq5s0-jual-pcb-controller-led-rgb-nodemcu-esp-32-esp32-smartmatrix-arduino-elektronmart
    o https://shopee.co.id/PCB-Controller-LED-RGB-NodeMCU-ESP-32-ESP32-SmartMatrix-Arduino-ElektronMart-i.139057740.4635178639

    Updated : 14 Juli 2020
    
*/

#include <Wire.h>
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);

#include <SmartMatrix3.h>

#include <F1kM_Hisab.h>


#define COLOR_DEPTH 24                  // known working: 24, 48 - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24
const uint8_t kMatrixWidth = 64;        // known working: 32, 64, 96, 128
const uint8_t kMatrixHeight = 32;       // known working: 16, 32, 48, 64
const uint8_t kRefreshDepth = 36;       // known working: 24, 36, 48
const uint8_t kDmaBufferRows = 2;       // known working: 2-4, use 2 to save memory, more to keep from dropping frames and automatically lowering refresh rate
const uint8_t kPanelType = SMARTMATRIX_HUB75_32ROW_MOD16SCAN;   // use SMARTMATRIX_HUB75_16ROW_MOD8SCAN for common 16x32 panels
const uint8_t kMatrixOptions = (SMARTMATRIX_OPTIONS_NONE);      // see http://docs.pixelmatix.com/SmartMatrix for options
const uint8_t kScrollingLayerOptions = (SM_SCROLLING_OPTIONS_NONE);
const uint8_t kIndexedLayerOptions = (SM_INDEXED_OPTIONS_NONE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_SCROLLING_LAYER(scrollingLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kScrollingLayerOptions);
SMARTMATRIX_ALLOCATE_SCROLLING_LAYER(scrollingLayer2, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kScrollingLayerOptions);
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(indexedLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);

const int defaultBrightness = (20*255)/100;        // full (100%) brightness
//const int defaultBrightness = (15*255)/100;       // dim: 15% brightness
const int defaultScrollOffset = 6;
const rgb24 defaultBackgroundColor = {0, 100, 0};


// RTC

RtcDateTime now;
#define sdaPin 32
#define sclPin 33

char bulanMasehi[][10] = { "", "JANUARI", "FEBRUARI", "MARET", "APRIL", "MEI", "JUNI", "JULI", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DESEMBER" };
char bulanHijriah[][14] = { "", "MUHARAM", "SAFAR", "RABIUL AWAL", "RABIUL AKHIR", "JUMADIL AWAL", "JUMADIL AKHIR", "RAJAB", "SYA'BAN", "RAMADAN", "SYAWAL", "ZULKAIDAH", "ZULHIJAH" };

uint8_t rJam;
uint8_t rMen;
uint8_t rDet;
uint8_t rTgl;
uint8_t rHar;
uint8_t rBul;
uint16_t rTah;
int celsius;


// HISAB TANGGAL HIJRIAH

F1kM_Hisab HisabTanggal;

int Hjr_Date, Hjr_Month, Hjr_Year;

// Koordinat Tangerang
float latitud = -6.16;
float longitud = 106.61;
int hilal = 0;


void setup() {
  
  Serial.begin(115200);

  mulaiRTC();

  // setup matrix
  matrix.addLayer(&scrollingLayer);
  matrix.addLayer(&scrollingLayer2);
  matrix.addLayer(&indexedLayer);
  
  matrix.begin();  

  matrix.setBrightness((30*255)/100);

  UpdateWaktu();
  TeksBerjalan();

  
  
}


void loop() {

  JamTanggal();
  UpdateWaktu();
  
  
}



void JamTanggal() {

  char jam[9];
  char tanggal[9];
  char suhu[5];  

  /* Draw Clock to SmartMatrix */
  sprintf(jam, "%02d:%02d:%02d", rJam, rMen, rDet);
  sprintf(tanggal, "%02d/%02d/%02d", rTgl, rBul, rTah-2000);
  sprintf(suhu, "%d*C", celsius-3);


  // clear screen before writing new text
  indexedLayer.fillScreen(0);
  
  indexedLayer.setFont(font3x5);
  indexedLayer.drawString(0, 1, 1, "JAM");
  indexedLayer.drawString(0, 8, 1, "TANGGAL");
  indexedLayer.drawString(0, 15, 1, "SUHU");
  indexedLayer.drawString(31, 1,  1, jam);
  indexedLayer.drawString(31, 8, 1, tanggal);
  indexedLayer.drawString(31, 15, 1, suhu);
  indexedLayer.swapBuffers();
  
}


void TeksBerjalan() {  
  
  char S_Tanggal_H[35];

  char tanggalmasehi[16];
  char tanggalhijriah[20];
  char suhu[3];
  
  sprintf(tanggalmasehi, "%d %s %d", rTgl, bulanMasehi[rBul], rTah);
  sprintf(tanggalhijriah, "%d %s %d", Hjr_Date, bulanHijriah[Hjr_Month], Hjr_Year);


  scrollingLayer.setMode(bounceForward);
  scrollingLayer.setOffsetFromTop(21);
  scrollingLayer.setSpeed(20);
  scrollingLayer.setFont(font3x5);
  scrollingLayer.setColor(rgb24(255,50,50));  
  scrollingLayer.start(tanggalmasehi, -1);

  scrollingLayer2.setMode(bounceForward);
  scrollingLayer2.setOffsetFromTop(27);
  scrollingLayer2.setSpeed(20);
  scrollingLayer2.setFont(font3x5);
  scrollingLayer2.setColor(rgb24(200,250,0));  
  scrollingLayer2.start(tanggalhijriah, -1);   
}







// ------------------
// MULAI RTC DS3231

void mulaiRTC() {
  
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
    Wire.begin(sdaPin,sclPin);
  }

  Rtc.Begin();

  if (!Rtc.GetIsRunning()) {
    
    Rtc.SetIsRunning(true);
    
  }
  
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
  
  Serial.println("Setup RTC selesai");
  
}


//--------------
// UPDATE WAKTU

void BacaRTC() {
  
  RtcDateTime now = Rtc.GetDateTime();
  RtcTemperature temp = Rtc.GetTemperature();
  celsius = temp.AsFloatDegC();
  rJam = now.Hour();
  rMen = now.Minute();
  rDet = now.Second();
  rTgl = now.Day();
  rHar = now.DayOfWeek();
  rBul = now.Month();
  rTah = now.Year();
  
}


// ----------------------
// UPDATE HISAB TANGGAL

void TanggalHijriah() {

  HisabTanggal.setLocationOnEarth(latitud, longitud); //Latitude Longitude TANGERANG
  HisabTanggal.setHilalVisibilityFactor(hilal);
  HisabTanggal.Greg2HijrDate(rTgl,rBul,rTah,Hjr_Date,Hjr_Month,Hjr_Year);

}


//--------------
// UPDATE WAKTU

void UpdateWaktu() {

  static long pM;
  static uint16_t d;
  

  if (millis() - pM > 1000) {
    d++;
    if (d < 2) {
      BacaRTC();
      TanggalHijriah();
    } else if (d < 60) { // Update setiap 60 detik
      rDet++;
      if (rDet > 59) {
        rMen++;
        rDet=0;
        if (rMen > 59) {
          rJam++;
          rMen = 0;
        }
      }
    } else {
      d = 0; 
    }
    pM = millis();    
  }
  
}


//----------------------------------------------------------------------
// I2C_ClearBus menghindari gagal baca RTC (nilai 00 atau 165)

int I2C_ClearBus() {
  
#if defined(TWCR) && defined(TWEN)
  TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly
#endif

  pinMode(sdaPin, INPUT_PULLUP); // Make SDA (data) and SCL (clock) pins Inputs with pullup.
  pinMode(sclPin, INPUT_PULLUP);

  delay(2500);  // Wait 2.5 secs. This is strictly only necessary on the first power
  // up of the DS3231 module to allow it to initialize properly,
  // but is also assists in reliable programming of FioV3 boards as it gives the
  // IDE a chance to start uploaded the program
  // before existing sketch confuses the IDE by sending Serial data.

  boolean SCL_LOW = (digitalRead(sclPin) == LOW); // Check is SCL is Low.
  if (SCL_LOW) { //If it is held low Arduno cannot become the I2C master. 
    return 1; //I2C bus error. Could not clear SCL clock line held low
  }

  boolean SDA_LOW = (digitalRead(sdaPin) == LOW);  // vi. Check SDA input.
  int clockCount = 20; // > 2x9 clock

  while (SDA_LOW && (clockCount > 0)) { //  vii. If SDA is Low,
    clockCount--;
  // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
    pinMode(sclPin, INPUT); // release SCL pullup so that when made output it will be LOW
    pinMode(sclPin, OUTPUT); // then clock SCL Low
    delayMicroseconds(10); //  for >5uS
    pinMode(sclPin, INPUT); // release SCL LOW
    pinMode(sclPin, INPUT_PULLUP); // turn on pullup resistors again
    // do not force high as slave may be holding it low for clock stretching.
    delayMicroseconds(10); //  for >5uS
    // The >5uS is so that even the slowest I2C devices are handled.
    SCL_LOW = (digitalRead(sclPin) == LOW); // Check if SCL is Low.
    int counter = 20;
    while (SCL_LOW && (counter > 0)) {  //  loop waiting for SCL to become High only wait 2sec.
      counter--;
      delay(100);
      SCL_LOW = (digitalRead(sclPin) == LOW);
    }
    if (SCL_LOW) { // still low after 2 sec error
      return 2; // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
    }
    SDA_LOW = (digitalRead(sdaPin) == LOW); //   and check SDA input again and loop
  }
  if (SDA_LOW) { // still low
    return 3; // I2C bus error. Could not clear. SDA data line held low
  }

  // else pull SDA line low for Start or Repeated Start
  pinMode(sdaPin, INPUT); // remove pullup.
  pinMode(sdaPin, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
  // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
  /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
  delayMicroseconds(10); // wait >5uS
  pinMode(sdaPin, INPUT); // remove output low
  pinMode(sdaPin, INPUT_PULLUP); // and make SDA high i.e. send I2C STOP control.
  delayMicroseconds(10); // x. wait >5uS
  pinMode(sdaPin, INPUT); // and reset pins as tri-state inputs which is the default state on reset
  pinMode(sclPin, INPUT);
  return 0; // all ok
}
