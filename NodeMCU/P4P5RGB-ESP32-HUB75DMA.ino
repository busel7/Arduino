// Contoh penggunakaan PCB LED RGB elektronmart.com menggunakan library ESP32 HUB75 LED Matrix Panel DMA Faptastic
// 
// PRODUK :
// - Tokopedia.com  : https://tk.tokopedia.com/ZSkBFhwgS/
// - Shopee.co.id   : https://shopee.co.id/elektronmart/4635178639
//
// ENVIRONTMENT :
// - Sistem Operasi Ubuntu 24.04.2 LTS
// - Arduino IDE 2.3.6
// - ESP32 Core 3.2.0 - espresif
// - ESP32 HUB75 LED Matrix Panel DMA Display 3.0.11 - Faptastic
//

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// DEFINISI PIN PCB LED RGB NodeMCU32 / EPS32 elektronmart.com Board V1.1, V2.1

#define EM_R1_PIN 2
#define EM_R2_PIN 16
#define EM_G1_PIN 15
#define EM_G2_PIN 27
#define EM_B1_PIN 4
#define EM_B2_PIN 17

#define EM_A_PIN 5
#define EM_B_PIN 18
#define EM_C_PIN 19
#define EM_D_PIN 21
#define EM_E_PIN 12

#define EM_OE_PIN 25
#define EM_CLK_PIN 22
#define EM_LAT_PIN 26

HUB75_I2S_CFG::i2s_pins _pins_x1 = {EM_R1_PIN, EM_G1_PIN, EM_B1_PIN, EM_R2_PIN, EM_G2_PIN, EM_B2_PIN, EM_A_PIN, EM_B_PIN, EM_C_PIN, EM_D_PIN, EM_E_PIN, EM_LAT_PIN, EM_OE_PIN, EM_CLK_PIN};

#define PANEL_RES_X 64  // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 32  // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1   // Total number of panels chained one to another

MatrixPanel_I2S_DMA *Disp = nullptr;



void setup() {
  
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN,    // Chain length
    _pins_x1       // pin mapping for port X1
  );
  
  Disp = new MatrixPanel_I2S_DMA(mxconfig);

  Disp->begin();

  Disp->setTextColor(Disp->color565(255, 255, 255)); 

  Disp->setBrightness8(20);    // range is 0-255, 0 - 0%, 255 - 100%
  Disp->setTextWrap(false);

}



void loop() {

  // Putih
  Disp->setTextColor(Disp->color565(255, 255, 255));
  Disp->setCursor(1, 0);
  Disp->print("LEDRGB PTH");
  // Merah
  Disp->setTextColor(Disp->color565(255, 0, 0));
  Disp->setCursor(1, 8);
  Disp->print("LEDRGB MRH");
  // Hijau
  Disp->setTextColor(Disp->color565(0, 255, 0));
  Disp->setCursor(1, 16);
  Disp->print("LEDRGB HJU");
  // Biru
  Disp->setTextColor(Disp->color565(0, 0, 255));
  Disp->setCursor(1, 24);
  Disp->print("LEDRGB BRU");

}
