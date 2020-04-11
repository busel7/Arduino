/*
    Demo P10 Single Color HUB12 menggunakan NodeMCU 32S ESP32S (Yellow Pin) dan RTC DS3231


    WIRING
    ------

    P10         ESP32
    A           5
    B           18
    C           19
    CLK         22
    LAT/SCLK    26
    R           2
    OE          25
    GND         GND

    RTC
    ---
    SDA         32
    SCL         33


    Bonny Useful (ElektronMart.Com)

    Library : 
    o SmartMatrix - Louis Beaudoin (Pixelmatix) - https://github.com/pixelmatix/SmartMatrix/tree/teensylc
    o RTC - Makuna - https://github.com/Makuna/Rtc 

    
*/

#include <Wire.h>
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);
#include <SmartMatrix3.h>

#define COLOR_DEPTH 24                  // known working: 24, 48 - If the sketch uses type `rgb24` directly, COLOR_DEPTH must be 24
const uint8_t kMatrixWidth = 32;        // known working: 32, 64, 96, 128
const uint8_t kMatrixHeight = 32*2;       // known working: 32 (untuk 1 baris), 64 (untuk 2 baris)
const uint8_t kRefreshDepth = 24;       // known working: 24, 36, 48
const uint8_t kDmaBufferRows = 2;       // known working: 2-4, use 2 to save memory, more to keep from dropping frames and automatically lowering refresh rate
const uint8_t kPanelType = SMARTMATRIX_HUB12_16ROW_32COL_MOD4SCAN;   // use SMARTMATRIX_HUB75_16ROW_MOD8SCAN for common 16x32 panels
const uint8_t kMatrixOptions = (SMARTMATRIX_OPTIONS_HUB12_MODE);      // see http://docs.pixelmatix.com/SmartMatrix for options
const uint8_t kIndexedLayerOptions = (SMARTMATRIX_OPTIONS_HUB12_MODE);

SMARTMATRIX_ALLOCATE_BUFFERS(matrix, kMatrixWidth, kMatrixHeight, kRefreshDepth, kDmaBufferRows, kPanelType, kMatrixOptions);
SMARTMATRIX_ALLOCATE_INDEXED_LAYER(indexedLayer, kMatrixWidth, kMatrixHeight, COLOR_DEPTH, kIndexedLayerOptions);

const int defaultBrightness = (25*255)/100;        // full (100%) brightness
//const int defaultBrightness = (15*255)/100;       // dim: 15% brightness
const int defaultScrollOffset = 6;
const rgb24 defaultBackgroundColor = {0, 0, 0};


RtcDateTime now;

void setup() {
  
  Serial.begin(115200);

  // setup matrix
  matrix.addLayer(&indexedLayer); 
  matrix.begin();

  Wire.begin(32,33);
  Rtc.Begin();

  if (!Rtc.GetIsRunning()) {
    
    Rtc.SetIsRunning(true);
    
  }
  
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 

  matrix.setBrightness(defaultBrightness);
}

void loop() {

  RtcDateTime now = Rtc.GetDateTime();
  
  char jam[9];
  char tanggal[9];

  // clear screen before writing new text
  indexedLayer.fillScreen(0);

  /* Draw Clock to SmartMatrix */
  uint8_t hour = now.Hour();
  sprintf(jam, "%02d:%02d:%02d", hour, now.Minute(), now.Second());
  sprintf(tanggal, "%02d-%02d-%02d", now.Day(), now.Month(), now.Year()-2000);
  indexedLayer.setFont(font3x5);
  
  //Panel 1 Kiri atau Atas
  indexedLayer.drawString(0, 1,  1, jam);
  indexedLayer.drawString(0, 10, 1, tanggal);

  //Panel 2 Kanan
  indexedLayer.drawString(32, 1, 1, "LED PKA"); // Tes Horizontal
  indexedLayer.drawString(32, 10, 1, "LED PKB"); // Tes Horizontal

  //Panel 2 Bawah
  indexedLayer.drawString(0, 33, 1, "LED PBA"); // Tes Vertikal 
  indexedLayer.drawString(0, 42, 1, "LED PBB"); // Tes Vertikal

  indexedLayer.swapBuffers();
  delay(1000);
  
}
