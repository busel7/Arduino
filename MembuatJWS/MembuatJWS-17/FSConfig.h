
// PrayerTimes
double times[sizeof(TimeName)/sizeof(char*)];
int ihti = 2;

int detikiqmh;
int menitiqmh;

uint8_t tmputama;
uint8_t tmpjws;
uint8_t tmpinfo;


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
