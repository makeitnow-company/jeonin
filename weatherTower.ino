#define BLYNK_PRINT Serial

//*******************************
//이곳에 BLYNK FirmwareConfiglation 값을 입력하세요.



//***********************************

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "jeonin";
char pass[] = "jeonin2023*";

#define DHTPIN 14        
#define DHTTYPE DHT11   

#define DUST_LED_PIN 13
#define DUST_MEASURE_PIN 35 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 
#define SCREEN_ADDRESS 0x3C

BlynkTimer timer;

DHT dht(DHTPIN, DHTTYPE);                
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int voMeasured;
float calcVoltage, dustDensity, temp, humidity;


#define CDS 39
#define WATER 36

boolean rain;

void weatherEvent()
{  
  int cds_value = analogRead(CDS);
  int water_value = analogRead(WATER);
  if(water_value > 2000){
    rain = true;
  }else{
    rain = false;
  }
  Blynk.virtualWrite(V5, cds_value/1000);
  Blynk.virtualWrite(V6, rain);
}

void dht11Event()
{  
  temp = dht.readTemperature();
  humidity = dht.readHumidity();      
  Blynk.virtualWrite(V7, temp);
  Blynk.virtualWrite(V8, humidity);
}

void dustEvent(){
  digitalWrite(DUST_LED_PIN, LOW);
  delayMicroseconds(280);
  voMeasured = analogRead(DUST_MEASURE_PIN);
  delayMicroseconds(40);
  digitalWrite(DUST_LED_PIN, HIGH);
  delayMicroseconds(9680);
  calcVoltage = voMeasured * (3.3 / 4095);
  dustDensity = 170 * calcVoltage - 0.1;
  Blynk.virtualWrite(V9, dustDensity);
}

void displayOLED(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("HUMI:");
  display.print(humidity);
  display.setCursor(0,15);
  display.print("TEMP:");
  display.print(temp);
  display.setCursor(0,30);
  display.print("DUST:");
  display.print(dustDensity);
  display.display();
}


void setup()
{
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  dht.begin();        
  pinMode(DUST_LED_PIN, OUTPUT);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, dht11Event);
  timer.setInterval(1000L, dustEvent);
  timer.setInterval(500L, displayOLED);

  display.setTextSize(2);
  display.setTextColor(WHITE,0);
  timer.setInterval(200L, weatherEvent);

}

void loop()
{
  Blynk.run();
  timer.run();
}