// 8 Digit Seven Segment LED RobotDyn 74HC595 - elektronmart.com
//
// 1. Pemetaan Segmen dan Tipe Anoda/Katoda // Mapping Segment and Type Anode/Cathode
// 2. Cari MSB atau LSB (MSB binary kanan ke kiri, LSB binary kiri ke kanan) // Find MSB or LSB (MSB binary right to left, LSB binary left to right)
// 3. Pemetaan Karakter // Character Mapping
// 4. Pemetaan Kolom // Column Mapping

#define RCK     D3    // LAT / CS 
#define SCK     D5    // CLK
#define DIO     D7    // DATA / DIN

// Default Column {1, 2, 4, 8, 16, 32, 64, 128};

//const int kolom[] = {16, 32, 64, 128, 1, 2, 4, 8};

const byte kolom[] = {8, 4, 2, 1, 128, 64, 32, 16};


const byte nomor[] = {
  B00000011,  // 0  B00000011 - 3
  B10011111,  // 1  B10011111 - 159
  B00100101,  // 2  B00100101 - 
  B00001101,  // 3  B00001101
  B10011001,  // 4  B10011001
  B01001001,  // 5  B01001001
  B01000001,  // 6  B01000001
  B00011111,  // 7  B00011111
  B00000001,  // 8  B00000001
  B00001001,  // 9  B00001001
};


void tampilkan() {

  for(int i = 0; i < 10; i++) {
    for(int k = 0; k < 8; k++) {
      digitalWrite(RCK, LOW);    
      shiftOut(DIO, SCK, LSBFIRST, nomor[i]); // Character
      shiftOut(DIO, SCK, LSBFIRST, kolom[k]); // Column
      digitalWrite(RCK, HIGH);
      delay (100);
    }   
  }  
}


void setup() { 

  Serial.begin(115200);
  pinMode(RCK, OUTPUT); 
  pinMode(SCK, OUTPUT);
  pinMode(DIO, OUTPUT);
  
}


void loop() {

  tampilkan();
  
}
