
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <Servo.h>
#include "Interfejs.h"

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

int PIN_SILNIK1 = 14;
int PIN_SILNIK2 = 12; //GPIO12


int wysokosc_zadana = 0;
int kat_zadany = 0;
int pwm_1_zadany =0;
int pwm_2_zadany =0;
float pwm_freq = 50;

long czas =0;

bool kalibracja = false;

bool silnik1_skalibrowany = false;
bool silnik2_skalibrowany = false;

Servo Silnik1;
Servo Silnik2;

float e =0;
float e_stary =0;
float calka =0;
float u =0;
const float u_min = 0; //0% mocy silnika
const float u_max = 1; //100% silnika
const float ts = 0.05; //czas probkowania (mysle ze co 50ms wystarczy)

const float kp =1;
const float ki =1;
const float kd = 0.2;


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
  Serial.begin(9600);
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
  server.on("/BUTTON_PWM_MIN",PodaniePWM_MIN );
  server.on("/BUTTON_PWM_MAX", PodaniePWM_MAX);
  server.on("/BUTTON_PWM_ZERO", PodaniePWM_ZERO);
  server.on("/AKTUALIZUJ_PWM_1", ZmianaPWM_1);
  server.on("/AKTUALIZUJ_PWM_2", ZmianaPWM_2);
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

}
void loop() {

  czas = millis();
  Serial.println(czas);

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
  //Serial.print("Roll Angle [°] ");
  //Serial.print(KalmanAngleRoll);
  //Serial.print(RateRoll);
  //Serial.print(" Pitch Angle [°] ");
  //Serial.println(RatePitch);
  //Serial.println(KalmanAnglePitch);
  
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
  wysokosc_zadana = t_state.toInt();
  Serial.print("UpdateWysokosc"); Serial.println(wysokosc_zadana);
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
  //wysokosc_zadana = map(wysokosc_zadana_bity, 0, 255, 5, 40);
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
  kat_zadany = t_state.toInt();
  Serial.print("UpdateKat"); Serial.println(kat_zadany);

  //kat_zadany = map(kat_zadany_bity, 0, 255, -65, 65);
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


void PodaniePWM_MIN(){
  Silnik1.writeMicroseconds(1000);
  Silnik2.writeMicroseconds(1000);
  server.send(200, "text/plain", "");
}

void PodaniePWM_MAX(){
  Silnik1.writeMicroseconds(2000);
  Silnik2.writeMicroseconds(2000);
  server.send(200, "text/plain", "");
}

void PodaniePWM_ZERO(){

  Silnik1.detach();
  Silnik2.detach();
  Silnik1.attach(PIN_SILNIK1);
  Silnik2.attach(PIN_SILNIK2);
  pwm_1_zadany =0;
  pwm_2_zadany =0;
  server.send(200, "text/plain", "");
}

void ZmianaPWM_1(){
  String t_state = server.arg("PWM1");
  Serial.println(t_state);

  // conver the string sent from the web page to an int
  pwm_1_zadany = t_state.toInt();

  //Zmiana PWM silnika
  Silnik1.writeMicroseconds(pwm_1_zadany);

  strcpy(buf, "");
  sprintf(buf, "%d", pwm_1_zadany);
  sprintf(buf, buf);

  // now send it back
  server.send(200, "text/plain", buf); //Send web page
}
void ZmianaPWM_2(){
  String t_state = server.arg("PWM2");

  // conver the string sent from the web page to an int
  pwm_2_zadany = t_state.toInt();
  Silnik2.writeMicroseconds(pwm_2_zadany);

  strcpy(buf, "");
  sprintf(buf, "%d", pwm_2_zadany);
  sprintf(buf, buf);

  // now send it back
  server.send(200, "text/plain", buf); //Send web page
}


void  PID(){
calka = calka +e*ts;
float pochodna = (e - e_stary)/ts;
u = kp*e + ki*calka + kd*pochodna; //bez sprawdzenia predkosci

if(u>u_max){
  u =u_max;
  calka = (u_max -kp*e-kd*pochodna)/ki;}

if(u < u_min){
  u = u_min;
  calka = (u_min -kp*e-kd*pochodna)/ki;
}
}

