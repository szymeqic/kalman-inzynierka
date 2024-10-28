
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <Servo.h>
#include "Interfejs.h"


//Gdy GPIO 0 jest zwarte do GND, jestesmy w trybie flash - mozna latwiej wgrywac


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
int PIN_ECHO =13; //SDD3  - echo do pomiaru odleglosci
int PIN_TRIG = 2; //SDD2 - trig


int wysokosc_zadana = 0;
int kat_zadany = 0;
int pwm_1_zadany =0;
int pwm_2_zadany =0;
float pwm_freq = 50;

float wysokosc = 0.25;

long czas =0;

bool kalibracja = false;


Servo Silnik1;
Servo Silnik2;

const float u_min = 0; //0% mocy silnika
const float u_max = 100; //100% silnika
const float ts = 0.05; //czas probkowania (mysle ze co 50ms wystarczy)

float kp_wys =1;
float ki_wys =1;
float kd_wys = 0.2;

float kp_kat =1;
float ki_kat = 1;
float kd_kat =1;


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



bool watchdog_ts(){
  static long czas = 0;
  static long czas_stary =0;
  const static long milisek = 2000000;

  czas = micros();
  if(czas - czas_stary >milisek){

    czas_stary = czas;
    return true;

  }
  return false;
}




void SendXML() {

  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");

  sprintf(buf, "<X1>%d,%d</X1>\n", (int) (KalmanAngleRoll), abs((int) (KalmanAngleRoll * 10) - ((int) (KalmanAngleRoll) * 10)));
  strcat(XML, buf);

  sprintf(buf, "<Y1>%d,%d</Y1>\n", (int) (KalmanAnglePitch), abs((int) (KalmanAnglePitch * 10) - ((int) (KalmanAnglePitch) * 10)));
  strcat(XML, buf);

  sprintf(buf, "<W1>%d,%d</W1>\n", (int) (wysokosc), abs((int) (wysokosc * 100) - ((int) (wysokosc) * 100)));
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

void Aktualizuj_PID(){
  String odp = server.arg("ZMIANA_PID");
  
  String war_na_serwer ="";

  if(odp.startsWith("Pasek_wys_kp")){
    odp.replace("Pasek_wys_kp", "");
    kp_wys = odp.toInt()/100.0;
    war_na_serwer = String(kp_wys);
  }

  if(odp.startsWith("Pasek_wys_ki")){
    odp.replace("Pasek_wys_ki", "");
    ki_wys = odp.toInt()/100.0;
    war_na_serwer = String(ki_wys);
  }

  if(odp.startsWith("Pasek_wys_kd")){
    odp.replace("Pasek_wys_kd", "");
    kd_wys = odp.toInt()/100.0;
    war_na_serwer = String(kd_wys);
  }

  if(odp.startsWith("Pasek_kat_kp")){
    odp.replace("Pasek_kat_kp", "");
    kp_kat = odp.toInt()/100.0;
    war_na_serwer =String( kp_kat);
  }

    if(odp.startsWith("Pasek_kat_ki")){
    odp.replace("Pasek_kat_ki", "");
    ki_kat = odp.toInt()/100.0;
    war_na_serwer = String(ki_kat);
  }

    if(odp.startsWith("Pasek_kat_kd")){
    odp.replace("Pasek_kat_kd", "");
    kd_kat = odp.toInt()/100.0;
    war_na_serwer = String(kd_kat);
  }

  server.send(200, "text/plain", war_na_serwer); 

}

void ZmianaPWM_oba(){
  String t_state = server.arg("PWM_oba");

  // conver the string sent from the web page to an int
  pwm_1_zadany =pwm_2_zadany = t_state.toInt();\
  Silnik1.writeMicroseconds(pwm_1_zadany);
  Silnik2.writeMicroseconds(pwm_2_zadany);

  strcpy(buf, "");
  sprintf(buf, "%d", pwm_2_zadany);
  sprintf(buf, buf);

  // now send it back
  server.send(200, "text/plain", buf); //Send web page

}

float zmierzOdleglosc(){
  float czas =0;
  //float temp =micros();
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG,LOW);
  
  //temp = micros() - temp;

  czas = pulseIn(PIN_ECHO, HIGH, 400000);//, 100000);
  //Serial.println(czas);
  //Serial.println(0);
  //return temp;
  return czas/58;

}

void sterowanie(){
  //najpierw wysterowujemy wysokość 
  static float e_wys;
  static float e_wys_stary;
  static float calka_wys;

  float ster_wys =0;

  wysokosc = zmierzOdleglosc();

  e_wys = wysokosc_zadana - wysokosc;
  ster_wys = PID(kp_wys, ki_wys, kd_wys, e_wys, e_wys_stary, &calka_wys);
  e_wys_stary = e_wys;

  static float e_kat;
  static float e_kat_stary;
  static float calka_kat;

  float ster_kat =0;   //dodatni uchyb - chcemy żeby podniósł się LANRC35A 
                      /// czyli silnik1 musi byc podpięty do lanrc2
  e_kat = kat_zadany -KalmanAngleRoll;
  ster_kat = PID(kp_kat, ki_kat, kd_kat, e_kat, e_kat_stary, &calka_kat);
  e_kat_stary = e_kat;

  int delta_ster_kat = map(ster_kat, 0,100, 1000, 2000);
  pwm_1_zadany = pwm_2_zadany = map(ster_wys, 0, 100, 1000, 2000);

  pwm_1_zadany +=delta_ster_kat;
  pwm_2_zadany -= delta_ster_kat;

  if(pwm_1_zadany<1000)
    pwm_1_zadany =1000;
  
  if(pwm_1_zadany >2000)
    pwm_1_zadany =2000;

    if(pwm_2_zadany<1000)
    pwm_2_zadany =1000;
  
  if(pwm_2_zadany >2000)
    pwm_2_zadany =2000;

}

float  PID(float kp, float ki, float kd, float e, float e_stary, float* calka){
  *calka = *calka +e*ts;
  float pochodna = (e - e_stary)/ts;
  float u = kp*e + ki*(*calka) + kd*pochodna; //bez sprawdzenia predkosci

  if(u>u_max){
  u =u_max;
  *calka = (u_max -kp*e-kd*pochodna)/ki;}

  if(u < u_min){
  u = u_min;
  *calka = (u_min -kp*e-kd*pochodna)/ki;
  }

  return u;
}



void loop() {



  if(watchdog_ts())
    wysokosc = zmierzOdleglosc();
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
  server.on("/AKTUALIZUJ_PWM_1", ZmianaPWM_1);
  server.on("/AKTUALIZUJ_PWM_2", ZmianaPWM_2);
  server.on("/AKTUALIZUJ_PWM_OBA", ZmianaPWM_oba);
  server.on("/AKTUALIZUJ_PID", Aktualizuj_PID );
  server.begin();
  Serial.println("Serwer HTTP wystartował");
  Serial.println("aaasd");
  //Serial.begin(57600);
  //pinMode(13, OUTPUT);
  //digitalWrite(13, HIGH);

  Wire.setClock(400000); //czestotliwosc zegara - 400 kHz
  Serial.println("sss");
  Wire.begin();
  Serial.println("ccc");
  delay(1000);

  delay(250);
  Serial.println(micros());
  //wlaczenie zyroskopu
  Wire.beginTransmission(0x68); 
  Serial.println("ccdddc");
  Wire.write(0x6B);
  Serial.println("asdfdfs");
  Wire.write(0x00);
  Serial.println("ccaaaaaaac");
  Wire.endTransmission();
  Serial.println("sss");

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

  
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_TRIG, OUTPUT);
  digitalWrite(PIN_TRIG, LOW);

}

