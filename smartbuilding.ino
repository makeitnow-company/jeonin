#define BLYNK_PRINT Serial

//*******************************
//이곳에 BLYNK FirmwareConfiglation 값을 입력하세요.



//***********************************
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "jeonin";
char pass[] = "jeonin2023*";

#define BTN_SECURITY_MODE 23 //보안 모드 전환 버튼
#define BTN_ADMIN_CHECK 27 //관리자 확인 버튼
#define HALLSENSOR 15
#define PROXSENSOR 34   

BlynkTimer timer;

boolean securityMode;
boolean doorOpen;
boolean windowOpen;
boolean adminCheck;

BLYNK_WRITE(V20){
  int v20_value = param.asInt();
  digitalWrite(4,v20_value);
}

BLYNK_WRITE(V21){
  int v21_value = param.asInt();
  digitalWrite(5,v21_value);
}

BLYNK_WRITE(V22){
  int v22_value = param.asInt();
  digitalWrite(12,v22_value);
}

void securityEvent()
{  
  if(!digitalRead(HALLSENSOR)){
     windowOpen = true;
  }

  if(analogRead(PROXSENSOR) > 4000){
    doorOpen = true;
  }

  if(digitalRead(BTN_SECURITY_MODE)){
    securityMode = !securityMode;
    windowOpen = false;
    doorOpen = false;
  }

  if(securityMode){
    if(windowOpen){
      Blynk.virtualWrite(V2, "창문 침입 감지");    
    }else if(doorOpen){
      Blynk.virtualWrite(V1, "출입문 침입 감지");    
    }else{
      Blynk.virtualWrite(V2, "창문 보안 작동중");    
      Blynk.virtualWrite(V1, "출입문 보안 작동중");    
    }
  }else{
      Blynk.virtualWrite(V1, "출입문 보안 해제 해제중"); 
      Blynk.virtualWrite(V2, "창문 보안 해제중"); 
  }
  
  if(digitalRead(BTN_ADMIN_CHECK)){
    windowOpen = false;
    doorOpen = false;
  }
}


void setup()
{
  Serial.begin(9600);
  pinMode(HALLSENSOR, INPUT);
  pinMode(BTN_SECURITY_MODE, INPUT);
  pinMode(BTN_ADMIN_CHECK, INPUT);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(200L, securityEvent);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(12, OUTPUT);
}

void loop()
{
  Blynk.run();
  timer.run();
}