//----------------------------------------------------------------------
// FORMAT TEKS

void textCenter(int y,String Msg) {
  
  int center = int((Disp.width()-Disp.textWidth(Msg)) / 2);
  Disp.drawText(center,y,Msg);
  
}

void textCenter1PKanan(int y,String Msg) {
  
  int center = ((Disp.width()-Disp.textWidth(Msg)) / 2) + (Disp.width() / 4);
  Disp.drawText(center,y,Msg);
  
}



//----------------------------------------------------------------------
// ALARM SHOLAT BERJALAN SAAT MASUK WAKTU SHOLAT

void AlarmSholat() {  

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

  if (rJam == hours && rMen == minutes && rDet == 0) {

    tipealarm = 0;
    detikbeep = 0;
    Disp.clear();
    tmputama = 1;

  }

  // Subuh
  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes + ihti;

  if (minutes >= 60) {
    
    minutes = minutes - 60;
    hours ++;
    
  }

  if (rJam == hours && rMen == minutes && rDet == 0) {
    
    tipealarm = 1;
    detikbeep = 0;
    Disp.clear();
    tmputama = 1;
    
  }


  // Dzuhur
  get_float_time_parts(times[2], hours, minutes);
  minutes = minutes + ihti;

  if (minutes >= 60) {
    
    minutes = minutes - 60;
    hours ++;
    
  }

  if (rJam == hours && rMen == minutes && rDet == 0 && rHar != 5) {
    
    tipealarm = 2;
    detikbeep = 0;
    Disp.clear();
    tmputama = 1;

  } else if (rJam == hours && rMen == minutes && rDet == 0 && rHar == 5) {
    
    tipealarm = 3;
    detikbeep = 0;
    Disp.clear();
    tmputama = 1;

  }


  // Ashar
  get_float_time_parts(times[3], hours, minutes);
  minutes = minutes + ihti;

  if (minutes >= 60) {
    
    minutes = minutes - 60;
    hours ++;
    
  }

  if (rJam == hours && rMen == minutes && rDet == 0) {
    
    tipealarm = 4;
    detikbeep = 0;
    Disp.clear();
    tmputama = 1;
    
  }

  // Maghrib
  get_float_time_parts(times[5], hours, minutes);
  minutes = minutes + ihti;

  if (minutes >= 60) {
    
    minutes = minutes - 60;
    hours ++;
    
  }

  if (rJam == hours && rMen == minutes && rDet == 0) {
    
    tipealarm = 5;
    detikbeep = 0;
    Disp.clear();
    tmputama = 1;
    
  }

  // Isya'
  get_float_time_parts(times[6], hours, minutes);
  minutes = minutes + ihti;

  if (minutes >= 60) {
    
    minutes = minutes - 60;
    hours ++;
    
  }

  if (rJam == hours && rMen == minutes && rDet == 0) {
    
    tipealarm = 6;
    detikbeep = 0;
    Disp.clear();
    tmputama = 1;
    
  }  
  
}



//----------------------------------------------------------------------
// TAMPILAN ADZAN

void TampilAdzan() {

  BunyiBeep(2,6);
  
  static uint32_t pM;
  uint32_t cM = millis();
  static uint16_t det;

  if (tipealarm == 0) {
      
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "TANBIH");
    textCenter(8, "IMSAK");
    
  }
  
  if (tipealarm == 1) {
  
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "ADZAN");
    textCenter(8, "SUBUH");

    menitiqmh = iqmhs;
    
  }
  
  if (tipealarm == 2) {
    
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "ADZAN");
    textCenter(8, "DZUHUR");

    menitiqmh = iqmhd;
    
  }
  
  if (tipealarm == 3) {
    
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "ADZAN");
    textCenter(8, "JUM'AT");
    
  }
  
  if (tipealarm == 4) {
  
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "ADZAN");
    textCenter(8, "ASHAR");
    
    menitiqmh = iqmha;
    
  }
  
  if (tipealarm == 5) {
  
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "ADZAN");
    textCenter(8, "MAGHRIB");

    menitiqmh = iqmhm;
    
  } 
  
  if (tipealarm == 6) {
  
    Disp.setFont(ElektronMart6x8);
    textCenter(0, "ADZAN");
    textCenter(8, "ISYA");

    menitiqmh = iqmhi;
    
  }  

  if (cM-pM >= 1000) {
    
    det++;   

    if (tipealarm == 0) {      
      
      if (det == 60) {
        Disp.clear();
        det = 0;
        detikbeep = 0;
        tmputama = 0;
      }
      
    }

    
    if (tipealarm == 3) {
    
      if (det == durasiadzan * 60) {
        Disp.clear();
        det = 0;
        detikbeep = 0;
        tmputama = 0;
      }
      
    }
    
    
    if (tipealarm == 1 or tipealarm == 2 or tipealarm == 4 or tipealarm == 5 or tipealarm == 6) {
          
      if (det == durasiadzan * 60) {
        Disp.clear();
        det = 0;
        detikbeep = 0;
        tmputama = 2;
      }
      
    }
    
    pM = cM;
    
  }  
  
}



//----------------------------------------------------------------------
// HITUNG MUNDUR WAKTU SETELAH ADZAN SAMPAI MULAI IQOMAH

void Iqomah() {

  static uint32_t pM;
  uint32_t cM = millis();
  static char hitungmundur[6];  

  Disp.setFont(ElektronMart5x6);
  textCenter(0, "IQOMAH");

  if (detikiqmh == 60) {
    detikiqmh = 0;
  }

  if (cM - pM >= 1000) {
    pM = cM;
    detikiqmh--;

    if (menitiqmh == 0 && detikiqmh == 0) {
      Disp.clear();
      detikbeep = 0;
      detikiqmh = 59;
      tmputama = 3;
    }

    if (detikiqmh < 0) {
      detikiqmh = 59;
      menitiqmh--;
    }
    
  }

  sprintf(hitungmundur, "%02d:%02d", menitiqmh, detikiqmh);
  Disp.setFont(ElektronMart6x8);
  textCenter(8, hitungmundur);
  
}



//----------------------------------------------------------------------
// TAMPILAN SAAT SHOLAT

void TampilSaatSholat() {

  BunyiBeep(1,4);

  static uint32_t pM;
  uint32_t cM = millis();
  static uint32_t durasi = 10; // Detik
  static uint32_t det;

  Disp.setFont(ElektronMart5x6);
  textCenter(0, "LURUSKAN DAN");
  textCenter(8, "RAPATKAN SHAF");

  if (cM - pM >= 1000) {
    pM = cM;
    det++;

    if (durasi == det) {
      Disp.clear();
      detikbeep = 0;
      det = 0;
      tmputama = 0;
    }
  }  
  
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
  textCenter(0,"#");
  Disp.drawRect(0,6,Disp.width(),6,1,1);

  if(cM - pM > Kecepatan) {

    pM = cM;

    if (x < fullScroll) {

      ++x;
      
    } else {

      x = 0;
      Disp.clear();
      tmpjws = 0;
      
    }

    Disp.drawText(width - x, 8, nama1);
    
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





void LogoA(uint32_t x) {

  static const uint8_t LogoA[] PROGMEM = {

    16, 16,

    0x00, 0x00, 0x7f, 0xfe, 0x7f, 0xfe, 0x00, 0x00, 0x7e, 0x66, 0x7e, 0x66, 0x06, 0x66, 0x06, 0x66, 0x7e, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7f, 0xfe, 0x7f, 0xfe, 0x00, 0x00

  };
  
  Disp.drawBitmap(x, 0, LogoA);
}


void LogoM(uint32_t x) {

  static const uint8_t LogoM[] PROGMEM = {

    16, 16,

    0x00, 0x00, 0x66, 0x7e, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x7e, 0x7e, 0x7e, 0x7e, 0x60, 0x60, 0x60, 0x60, 0x7e, 0x7e, 0x7e, 0x7e, 0x66, 0x06, 0x66, 0x06, 0x7f, 0xfe, 0x7f, 0xfe, 0x00, 0x00

  };
  
  Disp.drawBitmap(x, 0, LogoM);
}






void TampilJadwalSholat() {

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
    textCenter1PKanan(0, sholat);
    textCenter1PKanan(8, jam);

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
      textCenter1PKanan(0, "TANBIH");
      textCenter1PKanan(8, jam);
      
      if (i > 8) {
        i = 0;
        Disp.drawRect(0,0,64,15,0,0);
        tmpinfo = 0;
        tmpjws = 2;
      }
      
    }      
    
  }
  
}




//----------------------------------------------------------------------
//TAMPILKAN TANGGAL

void TampilTanggal() {

  char hari[8];
  char tanggal[7]; // 21 SEP

  static uint32_t pM;
  uint32_t cM = millis();

  static uint8_t flag;

  if(cM - pM > 3000) {
    
    pM = cM;
    flag++;

    sprintf(hari, "%s", namaHari[rHar]);
    sprintf(tanggal, "%02d %s", rTgl, namaBulan[rBul]);
  
    Disp.setFont(ElektronMart5x6);
    textCenter1PKanan(0, hari);
    textCenter1PKanan(8, tanggal);

    if (flag >= 2) {
      flag = 0;
      Disp.drawRect(32,0,64,15,0,0);
      tmpinfo = 1;
      
    }
    
  } 
  
}



//----------------------------------------------------------------------
// TAMPILKAN SUHU

void TampilSuhu() {

  char suhu[3];
  int koreksisuhu = 2; // Perkiraan selisih suhu mesin dengan suhu ruangan

  static uint32_t pM;
  uint32_t cM = millis();

  static uint8_t flag;

  if(cM - pM > 3000) {
    
    pM = cM;
    flag++;

    sprintf(suhu, "%02d*C", celsius - koreksisuhu);
    
    Disp.setFont(ElektronMart5x6);
    textCenter1PKanan(0, "SUHU");
    textCenter1PKanan(8, suhu);

    if (flag >= 2) {
      flag = 0;
      Disp.drawRect(32,0,64,15,0,0);
      tmpinfo = 2;
      
    }
  
  }
  
}



//----------------------------------------------------------------------
// TAMPILKAN JAM

void JamBesar(uint16_t x) {

  char jam[3];
  char menit[3];
  char detik[3];

  sprintf(jam, "%02d", rJam);
  sprintf(menit, "%02d", rMen);
  sprintf(detik, "%02d", rDet);

  static boolean huruf;

  static uint8_t y;
  static uint8_t d;           
  static uint32_t pM;
  uint32_t cM = millis();

  static uint32_t pMPulse;
  static uint8_t pulse;

  if (cM - pMPulse >= 100) {
    pMPulse = cM;
    pulse++;
  }
  
  if (pulse > 8) {
    pulse=0;
  }

  if(cM - pM > 35) { 
    if(d == 0 and y < 20) {
      pM=cM;
      y++;
    }
    if(d  == 1 and y > 0) {
      pM=cM;
      y--;
    }    
  }
  
  if(cM - pM > 10000 and y == 20) {
    d=1;
  }
  
  if(y == 20) {
    Disp.drawRect(x+15,3+pulse,x+18,11-pulse,0,1);       
  }
  
  if(y < 20) {
    Disp.drawRect(x+15,3,x+18,11,0,0);
  }
   
  if(y == 0 and d == 1) {
    d=0;
    huruf = !huruf;
  }
  
  //JAM
  if (huruf) {
    Disp.setFont(ElektronMart6x16);
  } else {
    Disp.setFont(ElektronMartArabic6x16);
  }  
  Disp.drawText(x+1, y - 20, jam);

  //MENIT
  if (huruf) {
    Disp.setFont(ElektronMart5x6);
  } else {
    Disp.setFont(ElektronMartArabic5x6);
  }  
  Disp.drawText(x+20, y - 20, menit);

  //DETIK          
  if (huruf) {
    Disp.setFont(ElektronMart5x6);
  } else {
    Disp.setFont(ElektronMartArabic5x6);
  }  
  Disp.drawText(x+20, y - 20 + 8, detik);
  
}


void JamBesar2(uint16_t x) {

  char jam[3];
  char menit[3];

  sprintf(jam, "%02d", rJam);
  sprintf(menit, "%02d", rMen);

  static boolean huruf;

  static uint8_t y;
  static uint8_t d;           
  static uint32_t pM;
  uint32_t cM = millis();

  static uint32_t pMPulse;
  static uint8_t pulse;

  if (cM - pMPulse >= 100) {
    
    pMPulse = cM;
    pulse++;
    
  }

  if (pulse > 8) {
    pulse=0;
  }

  if(cM - pM > 35) { 
    
    if(d == 0 and y < 20) {
      pM = cM;
      y++;
    }
    
    if(d  == 1 and y > 0) {
      pM = cM;      
      y--;
    }

  }

  if(cM - pM > 10000 and y == 20) {
    d=1;
  }

  if(y == 20) {
    Disp.drawRect(x+14,3+pulse,x+17,11-pulse,0,1);       
  }

  if(y < 20) {
    Disp.drawRect(x+14,3,x+18,17,0,0);
  }

  if(y == 0 and d == 1) {
    d=0;
    huruf = !huruf;
  }

  //JAM
  if (huruf) {
    Disp.setFont(ElektronMart6x16);
  } else {
    Disp.setFont(ElektronMartArabic6x16);
  }  
  Disp.drawText(x+1, y - 20, jam);


  //MENIT
  if (huruf) {
    Disp.setFont(ElektronMart6x16);
  } else {
    Disp.setFont(ElektronMartArabic6x16);
  }  
  Disp.drawText(x+18, y - 20, menit);

  
  
  
}


void TampilJamKecil() {

  static uint32_t pMJam;
  uint32_t cM = millis();
  
  char jam[9];
  
  if (cM - pMJam >= 1000) {
   
    pMJam = cM;
    
    //JAM
    sprintf(jam,"%02d:%02d:%02d", rJam, rMen, rDet);
    Disp.setFont(ElektronMart5x6);
    Disp.drawText(20,7, jam);
        
  }
 
}



//----------------------------------------------------------------------
//TAMPILKAN LOGO


//----------------------------------------------------------------------
// ANIMASI LOGO

void animLogoX() {

  static uint8_t x;
  static uint8_t s; // 0=in, 1=out
  static uint32_t pM;
  uint32_t cM = millis();

  JamBesar2(16);

  if (cM - pM > 35) {
    
    if (s == 0 and x < 16) {
      
      pM = cM;
      x++;
      
    }
    
    if (s == 1 and x > 0) {
      
      pM = cM;
      x--;
      
    }
    
  }
  
  if (cM - pM > 10000 and x == 16) {
    
    s = 1; 
       
  }

  if (x == 0 and s == 1) {
    
    Disp.clear();
    s = 0;
    tmpjws = 1;
     
  }


  LogoA(Disp.width() - x);
  LogoM(x - 16);  

}
