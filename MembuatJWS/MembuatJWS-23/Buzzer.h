// BUZZER
const int buzzer = 3; // Pin GPIO Buzzer - RX



//----------------------------------------------------------------------
// BUNYI BEEP BUZZER

int detikbeep;

void BunyiBeep(uint8_t tipebeep, uint8_t durasibeep) {

  static uint32_t pMBuzT;
  static uint32_t pMBuzB;
  static boolean beep;
  static uint8_t dt;

  if (millis() - pMBuzB >= 1000) {   
    detikbeep++;
    pMBuzB = millis();
  }

  if (detikbeep < durasibeep) {
    tipebeep = tipebeep;
  } else {
    tipebeep = 0;
    durasibeep = 0;
    //db = 0;
  }

  if (tipebeep == 0) {
      digitalWrite(buzzer, LOW);
  }

  if (tipebeep == 1) {    
    if (millis() - pMBuzT >= 500) {   
      dt++;
      beep = !beep;
      pMBuzT = millis();        
    }
  
    if (dt == 6) {
      dt = 0;
      tipebeep = 0;       
    }
  
    if (beep) {
      digitalWrite(buzzer, HIGH);
    } else {
      digitalWrite(buzzer, LOW);
    }    
  }

  if (tipebeep == 2) {
    if (millis() - pMBuzT >= 1000) {   
      dt++;
      beep = !beep;
      pMBuzT = millis();     
    }
  
    if (dt == 6) {
      dt = 0;
      tipebeep = 0;       
    }
  
    if (beep) {
      digitalWrite(buzzer, HIGH);
    } else {
      digitalWrite(buzzer, LOW);
    }
  }
    
}
