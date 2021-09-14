// WIRING JOYSTICK NODEMCU ESP8266 - ELEKTRONMART.COM
// ------------------------------------------
// SW > D0 > R10K > 3.3V (di pullup R10K)
// GND > GND
// VRX > A0
// VRY (tidak dipakai karena ESP8266 hanya memiliki 1 pin analog


#define pinLED D4     // pin LED internal ESP12
#define pinTombol D0  // pin Tombol
#define pinVRX A0     // Analog VRx

int koordinatX; // untuk menampung nilai Analog VRx
int nilaiTombol = 0; // untuk nampung nilai tombol tengah joystick

boolean statusLed;    // Menampung nilai status LED On atau Off
boolean statusTombol; // Menampung nilai status Tombol On atau Off



void setup() {
  
  Serial.begin(115200);

  pinMode(pinLED, OUTPUT);
  pinMode(pinTombol, INPUT);
  digitalWrite(pinLED, HIGH); // LOW nyala, HIGH mati
      
}


void loop() {

  // Baca nilai Analog X:
  koordinatX = analogRead(pinVRX);

  // Baca nilai Tombol Tengah:
  nilaiTombol = digitalRead(pinTombol);
  
  // Fungsi mengedipkan LED
  ledKedip();  

  // Fungsi pengaturan respon dari Joystick
  cekJoystick(); 
  
}



// Fungsi mengedipkan LED

void ledKedip() {

  static long pM;  

  // Kedipkan LED dengan kecepatan sesuai koordinat
  if (millis() - pM > koordinatX) {
    statusLed =! statusLed;    
    pM = millis();
  } 
  
}



// Fungsi pengaturan respon dari Joystick

void cekJoystick() {

  // Joystick Click Tombol Tengah
  // Nyalakan LED jika ditekan
  
  if(nilaiTombol == LOW){
    statusTombol =! statusTombol;
    delay(250);
  }

  // Joystick X Analog
  static long pM;
  
  if(millis()-pM > 100) {
    
    if (koordinatX <= 450 or koordinatX >= 550 ) {
      Serial.print("Koordinat X : "); Serial.println(koordinatX);
      digitalWrite(pinLED, statusLed);
    } else {
      digitalWrite(pinLED, statusTombol);
    }
    
    pM = millis();    
  }

}
