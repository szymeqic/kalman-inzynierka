
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include "Interfejs.h"
#include "ESP8266_PWM.h"
#include <Servo.h>
float RateRoll, RatePitch, RateYaw;
float RateCalibrationRoll, RateCalibrationPitch, RateCalibrationYaw;
int RateCalibrationNumber;
float AccX, AccY, AccZ;
float AngleRoll, AnglePitch;
uint32_t LoopTimer;
float KalmanAngleRoll=0, KalmanUncertaintyAngleRoll=1*1;
float KalmanAnglePitch=0, KalmanUncertaintyAnglePitch=1*1;
float Kalman1DOutput[]={0,0};

const char* ssid = "Filtr Kalmana"; //nazwa sieci utworzonej przez esp8266
const char* password = "ilovekalmanfilter";
ESP8266WebServer server(80);

char XML[2048];
char buf[32];

int PIN_SILNIK1 = 9;
int PIN_SILNIK2 = 10;

String kodzik = "";
String html = "";
String os_x = "";

int wysokosc_zadana = 0;
int wysokosc_zadana_bity = 0;
int kat_zadany = 0;
int kat_zadany_bity = 0;
float pwm_freq = 50;

float silnik1 = 0;
float silnik2 = 0;

bool kalibracja = false;
bool kalibracja_silnikow = false;
bool pwm1 = false;
bool pwm2 = false;
bool silnik1_skalibrowany = false;
bool silnik2_skalibrowany = false;

Servo Silnik1;
Servo Silnik2;
void handleRoot(){
  // funkcja pozwalajaca stworzyc 'strone internetowa' z danymi
  kodzik = "";
  kodzik += "<!DOCTYPE html> <html>";

  kodzik += "<style>";
  kodzik += ".bodytext { font-family: \"Verdana\", \"Arial\", sans-serif; font-size: 24px;";
  kodzik += "text-align: left; font-weight: light; border-radius: 5px; display:inline; }";
  kodzik += ".category { font-family: \"Verdana\", \"Arial\", sans-serif; font-weight: bold;";
  kodzik += "font-size: 32px; line-height: 50px; padding: 20px 10px 0px 10px; color:#000000; }";
  kodzik += "</style>";
//<meta http-equiv=\"refresh\" content=\"2\"> 
  kodzik += "<body> <head> </head>"; //w tagu meta jest automatyczne odswiezanie strony co 2 sekundy
  kodzik += "<div class=\"category\">Dane z MPU5060</div>";
  kodzik += "<div class=\"bodytext\" id=\"kat_osi_x\">Kat obrotu wokol osi X: ";
  kodzik += KalmanAngleRoll;
  kodzik += "</div> <br> <div class=\"bodytext\" id=\"kat_osi_y\">Kat obrotu wokol osi Y: ";
  kodzik += KalmanAnglePitch;
  kodzik += "</div> </body>";

  kodzik += "<div class=\"category\">Sensor Controls</div>";
  kodzik += "<br>";
  //kodzik += "<div class=\"bodytext\">LED </div>";
  //kodzik += "<button type=\"button\" id = \"btn0\">Toggle</button></div>";
  //kodzik += "<br>";
  //kodzik += "<div class=\"bodytext\">LED </div>";
  //kodzik += "<button type=\"button\" id = \"btn1\">Toggle</button></div>";
  //kodzik += "<br>";
  kodzik += "<div class=\"bodytext\">Wpisz zadane wychylenie drona:   </div>";
  kodzik += "<input type=\"text\" id=\"kat\">"; 
  kodzik += "<br>";
  kodzik += "<div class=\"bodytext\">Wpisz zadana wysokosc drona:   </div>";
  kodzik += "<input type=\"text\" id=\"wysokosc\">";

  //kodzik += "<script type=\"text/javascript\" src=\"http://code.jquery.com/jquery-1.8.2.js\"> $(document).ready(function() {";
  //kodzik += "var timeout = setTimeout(reloadChat, 2000); $.ajax({ type: \"GET\", url: \"url\",";
  //kodzik += "success: function(data) { $(\'#kat_osi_x\').html(data); timeout = setTimeout(reloadChat, 2000);";
  //kodzik += "} }); } }); </script> </html>";

  os_x = String(KalmanAngleRoll);

  kodzik += "<script>";
  kodzik += "function myTimer() { ";
  kodzik += "document.getElementById(\"kat_osi_x\").innerHTML = \"Kat obrotu wokol osi X: \" + ";
  kodzik += os_x;
  //kodzik += KalmanAngleRoll;
  kodzik += " } ";
  kodzik += "setInterval(myTimer, 1000);";
  kodzik += "</script> </html>";

  server.send(200, "text/html", kodzik);
}

void kalman_1d(float KalmanState, float KalmanUncertainty, float KalmanInput, float KalmanMeasurement) {
  //funkcja realizujaca filtr Kalmana
  KalmanState=KalmanState+0.004*KalmanInput;  //nowa predykcja kata
  KalmanUncertainty=KalmanUncertainty + 0.004 * 0.004 * 1 * 1;  //niepewnosc predykcji (4 * 4 - wariancja pomiarow zyroskopu)
  float KalmanGain=KalmanUncertainty * 1/(1*KalmanUncertainty + 1 * 1); //wzmocnienie Kalmana - od niego zalezy, jak wazne sa pomiary, a jak wazna predykcja (3 * 3 - wariancja pomiarow akcelerometru)
  KalmanState=KalmanState+KalmanGain * (KalmanMeasurement-KalmanState);  //kolejna predykcja kata (na podstawie wzmocnienia Kalmana)
  KalmanUncertainty=(1-KalmanGain) * KalmanUncertainty; //niepewnosc kolejnej predykcji
  Kalman1DOutput[0]=KalmanState; 
  Kalman1DOutput[1]=KalmanUncertainty;
}
void gyro_signals(void) {
  //wpisanie do rejestru informacji o czestotliwosci odciecia filtru dolnopasmowego (5 Hz)
  Wire.beginTransmission(0x68);
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();
  //wpisanie do rejestru informacji o zakresie pomiarowym akcelerometru (+/- 8g)
  Wire.beginTransmission(0x68);
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();
  //ustalenie pierwszego z rejestrow, z ktorych dane beda pobierane
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(); 
  Wire.requestFrom(0x68,6); //żądanie 6 bajtow z mpu5060 (kazda z osi to 2 bajty danych)
  int16_t AccXLSB = Wire.read() << 8 | Wire.read(); //odczytanie dwoch rejestrow z danymi
  int16_t AccYLSB = Wire.read() << 8 | Wire.read();
  int16_t AccZLSB = Wire.read() << 8 | Wire.read();
  //wpisanie do rejestru informacji o zakresie pomiarowym i rozdzielczosci bitowej zyroskopu
  //(+/- 500 °/s, 65,5 LSB/°/s)
  Wire.beginTransmission(0x68);
  Wire.write(0x1B); 
  Wire.write(0x8);
  Wire.endTransmission();
  //ustalenie pierwszego z rejestrow, z ktorych dane beda pobierane     
  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(0x68,6); //żądanie 6 bajtow z mpu5060 (kazda z osi to 2 bajty danych)
  int16_t GyroX=Wire.read()<<8 | Wire.read(); //odczytanie dwoch rejestrow z danymi
  int16_t GyroY=Wire.read()<<8 | Wire.read();
  int16_t GyroZ=Wire.read()<<8 | Wire.read();
  RateRoll=(float)GyroX/65.5; //prawdziwa predkosc po podzieleniu przez ustalona rozdzielczosc bitowa
  RatePitch=(float)GyroY/65.5;
  RateYaw=(float)GyroZ/65.5;
  AccX=(float)AccXLSB/4096 - 0.04; // wartosci sluza do kalibracji akcelerometru
  AccY=(float)AccYLSB/4096;
  AccZ=(float)AccZLSB/4096 - 0.11;
  AngleRoll=atan(AccY/sqrt(AccX*AccX+AccZ*AccZ))*1/(3.142/180);
  AnglePitch=-atan(AccX/sqrt(AccY*AccY+AccZ*AccZ))*1/(3.142/180);
}

void setup() {
  delay(1000);
  Serial.begin(57600);
  Serial.println();
  //tworzenie sieci bezprzewodowej z mikrokontrolera
  Serial.println("Konfugiracja access pointa...");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP(); 
  Serial.print("Adres IP AP: ");
  Serial.println(myIP);
  server.on("/", SendWebsite); //'wrzucenie' na serwer strony z danymi
  server.on("/xml", SendXML);
  server.on("/UPDATE_WYSOKOSC", UpdateWysokosc);
  server.on("/UPDATE_KAT", UpdateKat);
  server.on("/BUTTON_KALIBRACJA", KalibracjaUkladu);
  server.on("/BUTTON_SILNIKI", KalibracjaSilnikow);
  server.on("/BUTTON_PWM1", PodaniePWM1);
  server.on("/BUTTON_PWM2", PodaniePWM2);
  server.begin();
  Serial.println("Serwer HTTP wystartował");
  //Serial.begin(57600);
  //pinMode(13, OUTPUT);
  //digitalWrite(13, HIGH);
  Wire.setClock(400000); //czestotliwosc zegara - 400 kHz
  Wire.begin();
  delay(250);
  //wlaczenie zyroskopu
  Wire.beginTransmission(0x68); 
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  //ustalenie wartości kalibracyjnych
  //2000 pomiarów, w których mpu5060 powinno leżeć na płaskim podłożu
  for (RateCalibrationNumber=0; RateCalibrationNumber<2000; RateCalibrationNumber ++) {
    gyro_signals();
    RateCalibrationRoll+=RateRoll;
    RateCalibrationPitch+=RatePitch;
    RateCalibrationYaw+=RateYaw;
    delay(1);
  }
  //wartości kalibracyjne ustalone poprzez uśrednienie 2000 pomiarów
  RateCalibrationRoll/=2000;
  RateCalibrationPitch/=2000;
  RateCalibrationYaw/=2000;

  Silnik1.attach(PIN_SILNIK1);
  Silnik2.attach(PIN_SILNIK2);

  //LoopTimer=micros();
}
void loop() {
  
  if (kalibracja){
    RateCalibrationRoll = 0;
    RateCalibrationPitch = 0;
    RateCalibrationYaw = 0;
    for (RateCalibrationNumber=0; RateCalibrationNumber<2000; RateCalibrationNumber ++) {
    gyro_signals();
    RateCalibrationRoll+=RateRoll;
    RateCalibrationPitch+=RatePitch;
    RateCalibrationYaw+=RateYaw;
    delay(1);
    }
    RateCalibrationRoll/=2000;
    RateCalibrationPitch/=2000;
    RateCalibrationYaw/=2000;
    kalibracja = false;
  }
  if (pwm1) {
    // do zrobienia podawanie pwma o określonym wypełnieniu
    silnik1_skalibrowany = false;
    silnik2_skalibrowany = false;
    //analogWriteFreq(50);
    //PWM_Silnik1(PIN_SILNIK1, 0);
    //PWM_Silnik2(PIN_SILNIK2, 0);
    Silnik1.write(0);
    Silnik2.write(0);
    //PWM_Silnik1(PIN_SILNIK1, 10);
    //PWM_Silnik2(PIN_SILNIK2, 10);
    pwm1 = false;
  }
  if (pwm2) {
    // do zrobienia podawanie pwma o określonym wypełnieniu
    //analogWriteFreq(50);
    //analogWrite(PIN_SILNIK1, 0);
    //analogWrite(PIN_SILNIK2, 0);
    Silnik1.write(180);
    Silnik2.write(180);
    //PWM_Silnik1(PIN_SILNIK1, 20);
    //PWM_Silnik2(PIN_SILNIK2, 20);
    silnik1_skalibrowany = true;
    silnik2_skalibrowany = true;
    pwm2 = false;
  }
  gyro_signals();
  RateRoll-=RateCalibrationRoll; //poprawka o wartosci kalibracyjne
  RatePitch-=RateCalibrationPitch;
  RateYaw-=RateCalibrationYaw;
  kalman_1d(KalmanAngleRoll, KalmanUncertaintyAngleRoll, RateRoll, AngleRoll);
  KalmanAngleRoll=Kalman1DOutput[0]; 
  KalmanUncertaintyAngleRoll=Kalman1DOutput[1];
  kalman_1d(KalmanAnglePitch, KalmanUncertaintyAnglePitch, RatePitch, AnglePitch);
  KalmanAnglePitch=Kalman1DOutput[0]; 
  KalmanUncertaintyAnglePitch=Kalman1DOutput[1];
  Serial.print("Roll Angle [°] ");
  Serial.print(KalmanAngleRoll);
  //Serial.print(RateRoll);
  Serial.print(" Pitch Angle [°] ");
  //Serial.println(RatePitch);
  Serial.println(KalmanAnglePitch);
  //while (micros() - LoopTimer < 4000);
  //LoopTimer=micros();
  if (silnik1_skalibrowany){
    Silnik1.write(18);
  }
  if (silnik2_skalibrowany){
    Silnik2.write(18);
  }
  
  server.handleClient();
}

void SendXML() {

  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");

  sprintf(buf, "<X1>%d,%d</X1>\n", (int) (KalmanAngleRoll), abs((int) (KalmanAngleRoll * 10) - ((int) (KalmanAngleRoll) * 10)));
  strcat(XML, buf);

  sprintf(buf, "<Y1>%d,%d</Y1>\n", (int) (KalmanAnglePitch), abs((int) (KalmanAnglePitch * 10) - ((int) (KalmanAnglePitch) * 10)));
  strcat(XML, buf);

  strcat(XML, "</Data>\n");

  Serial.println(XML);
  server.send(200, "text/xml", XML);


}

void SendWebsite(){
  Serial.println("Wysylanie strony");
  server.send(200, "text/html", PAGE_MAIN);
}

void UpdateWysokosc() {

  // many I hate strings, but wifi lib uses them...
  String t_state = server.arg("WYSOKOSC");

  // conver the string sent from the web page to an int
  wysokosc_zadana_bity = t_state.toInt();
  Serial.print("UpdateWysokosc"); Serial.println(wysokosc_zadana_bity);
  // now set the PWM duty cycle
  //ledcWrite(0, FanSpeed);


  // YOU MUST SEND SOMETHING BACK TO THE WEB PAGE--BASICALLY TO KEEP IT LIVE

  // option 1: send no information back, but at least keep the page live
  // just send nothing back
  // server.send(200, "text/plain", ""); //Send web page

  // option 2: send something back immediately, maybe a pass/fail indication, maybe a measured value
  // here is how you send data back immediately and NOT through the general XML page update code
  // my simple example guesses at fan speed--ideally measure it and send back real data
  // i avoid strings at all caost, hence all the code to start with "" in the buffer and build a
  // simple piece of data
  wysokosc_zadana = map(wysokosc_zadana_bity, 0, 255, 5, 40);
  strcpy(buf, "");
  sprintf(buf, "%d", wysokosc_zadana);
  sprintf(buf, buf);

  // now send it back
  server.send(200, "text/plain", buf); //Send web page

}
void UpdateKat() {

  // many I hate strings, but wifi lib uses them...
  String t_state = server.arg("KAT");

  // conver the string sent from the web page to an int
  kat_zadany_bity = t_state.toInt();
  Serial.print("UpdateKat"); Serial.println(kat_zadany_bity);
  // now set the PWM duty cycle
  //ledcWrite(0, FanSpeed);


  // YOU MUST SEND SOMETHING BACK TO THE WEB PAGE--BASICALLY TO KEEP IT LIVE

  // option 1: send no information back, but at least keep the page live
  // just send nothing back
  // server.send(200, "text/plain", ""); //Send web page

  // option 2: send something back immediately, maybe a pass/fail indication, maybe a measured value
  // here is how you send data back immediately and NOT through the general XML page update code
  // my simple example guesses at fan speed--ideally measure it and send back real data
  // i avoid strings at all caost, hence all the code to start with "" in the buffer and build a
  // simple piece of data
  kat_zadany = map(kat_zadany_bity, 0, 255, -65, 65);
  strcpy(buf, "");
  sprintf(buf, "%d", kat_zadany);
  sprintf(buf, buf);

  // now send it back
  server.send(200, "text/plain", buf); //Send web page

}

void KalibracjaUkladu(){
  kalibracja = true;
  Serial.println("Kalibracja wlaczona");
  server.send(200, "text/plain", "");
}

void KalibracjaSilnikow(){
  kalibracja_silnikow = true;
  Serial.println("Kalibracja silnikow wlaczona");
  server.send(200, "text/plain", "");
}

void PodaniePWM1(){
  pwm1 = true;
  Serial.println("Podanie pierwszego PWM");
  server.send(200, "text/plain", "");
}

void PodaniePWM2(){
  pwm2 = true;
  Serial.println("Podanie drugiego PWM");
  server.send(200, "text/plain", "");
}

/*void PWM_Silnik1(int pin, float procent_wypelnienia){
  unsigned long czas1 = millis();
    if ((czas1 - millis() == (procent_wypelnienia / 100) * 20) && silnik1 == 1){
      digitalWrite(pin, LOW);
      silnik1 = 0;
      Serial.print("Silnik 1: ");
      Serial.println(silnik1);
      czas1 = millis();
    }
    else if ((czas1 - millis() == ((100 - procent_wypelnienia) / 100) * 20) && silnik1 == 0){
      digitalWrite(pin, HIGH);
      silnik1 = 1;
      Serial.print("Silnik 1: ");
      Serial.println(silnik1);
      czas1 = millis();
    }
}*/

/*void PWM_Silnik2(int pin, float procent_wypelnienia){
  unsigned long czas2 = millis();
    if ((czas2 - millis() == (procent_wypelnienia / 100) * 20) && silnik2 == 1){
      digitalWrite(pin, LOW);
      silnik2 = 0;
      Serial.print("Silnik 2: ");
      Serial.println(silnik2);
      czas2 = millis();
    }
    else if ((czas2 - millis() == ((100 - procent_wypelnienia) / 100) * 20) && silnik2 == 0){
      digitalWrite(pin, HIGH);
      silnik2 = 1;
      Serial.print("Silnik 2: ");
      Serial.println(silnik2);
      czas2 = millis();
    }
}*/