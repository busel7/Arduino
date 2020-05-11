
// WIFI

// LED Internal
uint8_t pin_led = 2;

//WEB Server
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

// Sebagai Station
const char* wifissid = "grobak.net"; //kalau gagal konek
const char* wifipassword = "";

// Sebagai AccessPoint
const char* ssid = "JWSP10"; //kalau gagal konek
const char* password = "elektronmart";

IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress netmask(255, 255, 255, 0);



void wifiConnect() {

  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  delay(1000);

  Serial.println("Mencoba sambungan ke Hotspot atau Router");
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifissid, wifipassword);
  
  unsigned long startTime = millis();
  
  while (WiFi.status() != WL_CONNECTED) {
    
    delay(500);
    Serial.print(".");
    
    digitalWrite(pin_led, !digitalRead(pin_led));
    
    if (millis() - startTime > 3000) {
      Serial.println(" ");
      break;
    }
    
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    
    digitalWrite(pin_led, HIGH);
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
  } else {
    
    Serial.println("Gagal tersambung ke Hotspot, mode Hotspot aktif.");
    
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, netmask);
    WiFi.softAP(ssid, password);
    
    digitalWrite(pin_led, LOW);

    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.print("IP: ");
    Serial.println(local_ip);
    
  }

}
