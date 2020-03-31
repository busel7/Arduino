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


RtcDateTime now;
char bulanMasehi[][10] = { "", "JANUARI", "FEBRUARI", "MARET", "APRIL", "MEI", "JUNI", "JULI", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DESEMBER" };
char bulanHijriah[][14] = { "", "MUHARAM", "SAFAR", "RABIUL AWAL", "RABIUL AKHIR", "JUMADIL AWAL", "JUMADIL AKHIR", "RAJAB", "SYA'BAN", "RAMADAN", "SYAWAL", "ZULKAIDAH", "ZULHIJAH" };


//=============================
// Construct Object
//=============================
F1kM_Hisab HisabTanggal;


void setup() {
  
  Serial.begin(115200);

  /* I2C Changes Needed for SmartMatrix Shield */
  // switch pins to use 16/17 for I2C instead of 18/19, after calling matrix.begin()//
  Wire.begin(32,33);
  Rtc.Begin();

  if (!Rtc.GetIsRunning()) {
    
    Rtc.SetIsRunning(true);
    
  }
  
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

  // setup matrix
  matrix.addLayer(&scrollingLayer);
  matrix.addLayer(&scrollingLayer2);
  matrix.addLayer(&indexedLayer);
  
  matrix.begin();

  matrix.setBrightness(defaultBrightness);

  TeksBerjalan();
  
}


void loop() {

  JamTanggal();  
  
}

void JamTanggal() {

  RtcDateTime now = Rtc.GetDateTime();
  RtcTemperature temp = Rtc.GetTemperature();
  int celsius = temp.AsFloatDegC();  

  char jam[9];
  char tanggal[9];
  char suhu[3];  

  /* Draw Clock to SmartMatrix */
  sprintf(jam, "%02d:%02d:%02d", now.Hour(), now.Minute(), now.Second());
  sprintf(tanggal, "%02d %02d %02d", now.Day(), now.Month(), now.Year()-2000);
  sprintf(suhu, "%d*C", celsius);


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

  RtcDateTime now = Rtc.GetDateTime();  
  
  int Gre_Date  = now.Day();
  int Gre_Month = now.Month();
  int Gre_Year  = now.Year();

  char S_Tanggal_H[35];
  int Hjr_Date, Hjr_Month, Hjr_Year;

  char tanggalmasehi[16];
  char tanggalhijriah[20];
  char suhu[3];

  //=============================
  // Set Locations
  //=============================
  HisabTanggal.setLocationOnEarth(-6.171497, 106.640406); //Latitude Longitude TANGERANG
  
  //Set Imkan Rukyah ... Visibility Factor
  // Muhammadiah      ==> 0 derajat
  // Nahdatul Ulama   ==> 2 derajat
  HisabTanggal.setHilalVisibilityFactor(0);
  
  HisabTanggal.Greg2HijrDate(Gre_Date,Gre_Month,Gre_Year,Hjr_Date,Hjr_Month,Hjr_Year);

  
  sprintf(tanggalmasehi, "%d %s %d", Gre_Date, bulanMasehi[Gre_Month], Gre_Year);
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
