#include <i2cdetect.h>

#include <MPU6050.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <Servo.h>
#include <Adafruit_VL53L0X.h>
#include "Interfejs.h"


//Gdy GPIO 0 jest zwarte do GND, jestesmy w trybie flash - mozna latwiej wgrywac


float RateRoll, RatePitch, RateYaw;
float RateCalibrationRoll, RateCalibrationPitch, RateCalibrationYaw;
int RateCalibrationNumber;
float AccX, AccY, AccZ;
float AngleRoll, AnglePitch;
uint32_t LoopTimer;
float KalmanAngleRoll = 0, KalmanUncertaintyAngleRoll = 1000;
float KalmanAnglePitch = 0, KalmanUncertaintyAnglePitch = 1000;
float Kalman1DOutput[] = { 0, 0 };


const char* ssid = "Filtr Kalmana";  //nazwa sieci utworzonej przez esp8266
const char* password = "ilovekalmanfilter";
ESP8266WebServer server(80);

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

char XML[2560];
char buf[64];

int PIN_SILNIK1 = 12;
int PIN_SILNIK2 = 14;  //GPIO14
int PIN_ECHO = 13;     //SDD3  - echo do pomiaru odleglosci
int PIN_TRIG = 2;      //SDD2 - trig


int wysokosc_zadana = 0;
int kat_zadany = 0;
int pwm_1_zadany = 0;
int pwm_2_zadany = 0;
float pwm_freq = 50;
int wsp_moc = 30;  //30 %mocy
bool stop =false; //zeruj calki w sterowaniu

float wysokosc = 0.25;
float wysokosc_stara = 0.25;
float predkosc_wysokosci = 0;

float Kalman_wysokosc = 0;
float KalmanUncertaintyWysokosc = 1000;

long czas = 0;

bool kalibracja = false;
bool ster_auto = false;

bool test_wysylania_danych = false;
long czas_wysylania_danych1 = 0;
long czas_wysylania_danych2 = 0;
long czas_wysylania_danych3 = 0;

bool test_odbioru_danych = false;

Servo Silnik1;
Servo Silnik2;

float u_min = 2;  //2% mocy silnika
float u_max = 1;  //100% silnika  - potem mnożone przez współczynnik mocy, nie bać się!!!!
//const float ts = 0.2;      //czas probkowania (mysle ze co 50ms wystarczy)

float kp_wys = 0.01;
float ki_wys = 0;
float kd_wys = 0;

float kp_kat = 0.01;
float ki_kat = 0;
float kd_kat = 0;

char sterowanie_tryb = 'o';  ///o - oba, k- kątem, w - wysokością

float uchyb = 0;
float kat_stary = 0;
float kat_zadany1 = 0;
float F1 = 0;
float F2 = 0;
float x1 = 0;
float x2 = 0;
bool odwrocenie = false;


MPU6050 mpu;
#define OUTPUT_READABLE_ACCELGYRO
int16_t ax, ay, az;
int16_t gx, gy, gz;


void kalman_1d(float KalmanState, float KalmanUncertainty, float KalmanInput, float KalmanMeasurement, bool roll, bool wysokosc) {
  //funkcja realizujaca filtr Kalmana
  //uwaga
  
  static float war_zyro = 3, war_akc = 1;  // większe zyro - mniej ufamy predkosci, wieksze akc - mniej ufamy odczyttom (generalnie odczyty są dobre)
  if (!wysokosc){
    float ts_kalman = 0.004;
  }
  else {
    float ts_kalman = 0.02;
  }
  /*
  static float ts_roll, ts_pitch = 0;
  static long t_minal_roll = micros(), t_minal_pitch = micros();

  if (roll) {
    ts_roll = (micros() - t_minal_roll) / 1000000.0;  //podzielic
    t_minal_roll = micros();
    ts_kalman = ts_roll;
  } else {
    ts_pitch = (micros() - t_minal_pitch) / 1000000.0;  //podzielic
    t_minal_pitch = micros();
    ts_kalman = ts_pitch;
  }
  */

  KalmanState = KalmanState + ts_kalman * KalmanInput;                                       //nowa predykcja kata
  KalmanUncertainty = KalmanUncertainty + ts_kalman * ts_kalman * war_zyro * war_zyro;       //niepewnosc predykcji (4 * 4 - wariancja pomiarow zyroskopu)
  float KalmanGain = KalmanUncertainty * 1 / (1 * KalmanUncertainty + war_akc * war_akc);  //wzmocnienie Kalmana - od niego zalezy, jak wazne sa pomiary, a jak wazna predykcja (3 * 3 - wariancja pomiarow akcelerometru)
  KalmanState = KalmanState + KalmanGain * (KalmanMeasurement - KalmanState);  //kolejna predykcja kata (na podstawie wzmocnienia Kalmana)
  KalmanUncertainty = (1 - KalmanGain) * KalmanUncertainty;                    //niepewnosc kolejnej predykcji
  Kalman1DOutput[0] = KalmanState;
  Kalman1DOutput[1] = KalmanUncertainty;
}
void gyro_signals() {

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  RateRoll = (float)-gx / 65.5;  //prawdziwa predkosc po podzieleniu przez ustalona rozdzielczosc bitowa
  RatePitch = (float)gy / 65.5;
  RateYaw = (float)-gz / 65.5;

  AccX = (float)-ax / 4096 - 0.04;  // wartosci sluza do kalibracji akcelerometru
  AccY = (float)ay / 4096;
  AccZ = (float)-az / 4096 - 0.11;

  AngleRoll = atan(AccY / sqrt(AccX * AccX + AccZ * AccZ)) * 1 / (3.142 / 180);
  AnglePitch = -atan(AccX / sqrt(AccY * AccY + AccZ * AccZ)) * 1 / (3.142 / 180);


  //równanie do 2 miejsc po przecinku
  //AngleRoll = int(AngleRoll);// + (int(AngleRoll*10) - int(AngleRoll)*10)/10;
  //AnglePitch= int(AnglePitch);// + (int(AnglePitch*10) - int(AnglePitch)*10)/10;
}



void watchdog_ts() {
  static long czas_odl, czas_kat = 0;
  static long czas_stary_odl = micros(), czas_stary_kat = micros();

  czas_odl = czas_kat = micros();
  if (czas_odl - czas_stary_odl > 20000) {
    zmierzOdleglosc();
    kalman_1d(Kalman_wysokosc, KalmanUncertaintyWysokosc, predkosc_wysokosci, wysokosc, false, true);
    czas_stary_odl = czas_odl;
    if (ster_auto) { 
      sterowanie();        //sterowanie z częstotliwością 50 Hz
    }
  }
  if (czas_kat - czas_stary_kat > 4000) {
    gyro_signals();
    RateRoll -= RateCalibrationRoll;  //poprawka o wartosci kalibracyjne
    RatePitch -= RateCalibrationPitch;
    RateYaw -= RateCalibrationYaw;

    //roll
    kalman_1d(KalmanAngleRoll, KalmanUncertaintyAngleRoll, RateRoll, AngleRoll, true, false);
    KalmanAngleRoll = Kalman1DOutput[0];
    KalmanUncertaintyAngleRoll = Kalman1DOutput[1];
    //KalmanAngleRoll = AngleRoll;

    //pitch
    kalman_1d(KalmanAnglePitch, KalmanUncertaintyAnglePitch, RatePitch, AnglePitch, false, false);
    KalmanAnglePitch = Kalman1DOutput[0];
    KalmanUncertaintyAnglePitch = Kalman1DOutput[1];
    //KalmanAnglePitch = AnglePitch;
    czas_stary_kat = czas_kat;
  }
}




void SendXML() {

  strcpy(XML, "<?xml version = '1.0'?>\n<Data>\n");


  sprintf(buf, "<X1>%f</X1>\n", ((int)(KalmanAngleRoll*1000))/1000.0);
  strcat(XML, buf);

  sprintf(buf, "<Y1>%f</Y1>\n", ((int)(KalmanAnglePitch*1000))/1000.0);
  strcat(XML, buf);

  sprintf(buf, "<W1>%d,%d</W1>\n", (int)(wysokosc), abs((int)(wysokosc * 10) - ((int)(wysokosc)*10)));
  strcat(XML, buf);

  sprintf(buf, "<PWM1>%d</PWM1>\n", pwm_1_zadany);
  strcat(XML, buf);

  sprintf(buf, "<PWM2>%d</PWM2>\n", pwm_2_zadany);
  strcat(XML, buf);

  //Dane Kalmana dla kąta roll
  /*

  sprintf(buf, "<XKALMR>%f</XKALMR>\n", ((int)(RateRoll*1000))/1000.0);
  strcat(XML, buf);

  sprintf(buf, "<XKALMU>%f</XKALMU>\n", ((int)(KalmanUncertaintyAngleRoll*1000))/1000.0);
  strcat(XML, buf);

  //Dane Kalmana dla kąta pitch

  sprintf(buf, "<YKALMR>%f</YKALMR>\n",  ((int)(RatePitch*1000))/1000.0);
  strcat(XML, buf);

  sprintf(buf, "<YKALMU>%f</YKALMU>\n", ((int)(KalmanUncertaintyAnglePitch*1000))/1000.0);
  strcat(XML, buf);

*/

  sprintf(buf, "<YKALMR>%f</YKALMR>\n",  kd_kat);
  strcat(XML, buf);

  sprintf(buf, "<YKALMU>%f</YKALMU>\n", kp_kat);
  strcat(XML, buf);

  sprintf(buf, "<XCZYST>%f</XCZYST>\n", ((int)(AngleRoll*1000))/1000.0);
  strcat(XML, buf);

  sprintf(buf, "<YCZYST>%f</YCZYST>\n", ((int)(AnglePitch*1000))/1000.0);
  strcat(XML, buf);

  strcat(XML, "</Data>\n");

  //Serial.println(XML);
  if (test_wysylania_danych) {
    czas_wysylania_danych1 = millis();
  }
  server.send(200, "text/xml", XML);
}

void Test_wysylania_danych() {
  if (test_wysylania_danych) {
    czas_wysylania_danych2 = millis();
    czas_wysylania_danych3 = czas_wysylania_danych2 - czas_wysylania_danych1;
    //Serial.print(" Czas_wysylania_danych_na_serwer [ms]: ");
    Serial.println(czas_wysylania_danych3);
  }
}

void SendWebsite() {
  Serial.println("Wysylanie strony");
  server.send(200, "text/html", PAGE_MAIN);
}

void UpdateWysokosc() {

  // many I hate strings, but wifi lib uses them...
  String t_state = server.arg("WYSOKOSC");

  // conver the string sent from the web page to an int
  wysokosc_zadana = t_state.toInt();
  Serial.print("UpdateWysokosc");
  Serial.println(wysokosc_zadana);
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
  server.send(200, "text/plain", buf);  //Send web page
}
void UpdateKat() {

  // many I hate strings, but wifi lib uses them...
  String t_state = server.arg("KAT");

  // conver the string sent from the web page to an int
  kat_zadany = t_state.toInt();
  Serial.print("UpdateKat");
  Serial.println(kat_zadany);

  //kat_zadany = map(kat_zadany_bity, 0, 255, -65, 65);
  strcpy(buf, "");
  sprintf(buf, "%d", kat_zadany);
  sprintf(buf, buf);

  // now send it back
  server.send(200, "text/plain", buf);  //Send web page
}

void KalibracjaUkladu() {
  kalibracja = true;
  Serial.println("Kalibracja wlaczona");
  server.send(200, "text/plain", "");
}


void zmien_pwm(int pwm, int silnik){

  if(silnik==1){
    if(!Silnik1.attached())
      Silnik1.attach(PIN_SILNIK1);  

    Silnik1.writeMicroseconds(pwm);
  }

  if(silnik==2){
    if(!Silnik2.attached())
      Silnik2.attach(PIN_SILNIK2);
  Silnik2.writeMicroseconds(pwm);
  }

}


void ZmianaPWM_1() {
  String t_state = server.arg("PWM1");
  Serial.println(t_state);

  // conver the string sent from the web page to an int
  pwm_1_zadany = t_state.toInt();

  //Zmiana PWM silnika
  zmien_pwm(pwm_1_zadany, 1);
  //Silnik1.writeMicroseconds(pwm_1_zadany);

  strcpy(buf, "");
  sprintf(buf, "%d", pwm_1_zadany);
  sprintf(buf, buf);

  // now send it back
  server.send(200, "text/plain", buf);  //Send web page
}
void ZmianaPWM_2() {
  String t_state = server.arg("PWM2");

  // conver the string sent from the web page to an int
  pwm_2_zadany = t_state.toInt();
  zmien_pwm(pwm_2_zadany, 2);
  //Silnik2.writeMicroseconds(pwm_2_zadany);

  strcpy(buf, "");
  sprintf(buf, "%d", pwm_2_zadany);
  sprintf(buf, buf);

  // now send it back
  server.send(200, "text/plain", buf);  //Send web page
}

void Aktualizuj_PID() {
  String odp = server.arg("ZMIANA_PID");

  String war_na_serwer = "";

  if (odp.startsWith("Pasek_wys_kp")) {
    odp.replace("Pasek_wys_kp", "");
    kp_wys = odp.toInt() / 500.0;
    war_na_serwer = String(kp_wys);
  }

  if (odp.startsWith("Pasek_wys_ki")) {
    odp.replace("Pasek_wys_ki", "");
    ki_wys = odp.toInt() / 500.0;
    war_na_serwer = String(ki_wys);
  }

  if (odp.startsWith("Pasek_wys_kd")) {
    odp.replace("Pasek_wys_kd", "");
    kd_wys = odp.toInt() / 500.0;
    war_na_serwer = String(kd_wys);
  }

  if (odp.startsWith("Pasek_kat_kp")) {
    odp.replace("Pasek_kat_kp", "");
    kp_kat = odp.toInt() / 500.0;
    war_na_serwer = String(kp_kat);
  }

  if (odp.startsWith("Pasek_kat_ki")) {
    odp.replace("Pasek_kat_ki", "");
    ki_kat = odp.toInt() / 500.0;
    war_na_serwer = String(ki_kat);
  }

  if (odp.startsWith("Pasek_kat_kd")) {
    odp.replace("Pasek_kat_kd", "");
    kd_kat = odp.toInt() / 500.0;
    war_na_serwer = String(kd_kat);
  }

  server.send(200, "text/plain", war_na_serwer);
}

void ZmianaPWM_oba() {
  
  String t_state = server.arg("PWM_oba");

  // conver the string sent from the web page to an int
  pwm_1_zadany = pwm_2_zadany = t_state.toInt();

  zmien_pwm(pwm_1_zadany, 1);
  zmien_pwm(pwm_2_zadany, 2);
  //Silnik1.writeMicroseconds(pwm_1_zadany);
  //Silnik2.writeMicroseconds(pwm_2_zadany);
  stop = true;

  strcpy(buf, "");
  sprintf(buf, "%d", pwm_2_zadany);
  sprintf(buf, buf);

  // now send it back
  server.send(200, "text/plain", buf);  //Send web page
}

void Aktualizuj_ster() {



  String t_state = server.arg("STER");

  if (t_state[1] == 'r')
    ster_auto = false;
  else if (t_state[1] == 'a')
    ster_auto = true;

  if (t_state[0] == 'k')
    sterowanie_tryb = 'k';
  else if (t_state[0] == 'w')
    sterowanie_tryb = 'w';
  else if (t_state[0] == 'o')
    sterowanie_tryb = 'o';

  server.send(200, "text/plain", String(sterowanie_tryb));
  return;
}

void Pasek_moc() {

  wsp_moc = server.arg("MOC").toInt();

  if (wsp_moc < 0)
    wsp_moc = 0;
  else if (wsp_moc > 100)
    wsp_moc = 100;

  //u_min = -wsp_moc;
  u_max = wsp_moc;

  if (pwm_1_zadany > 1000 + wsp_moc * 10) {
    pwm_1_zadany = 1000 + wsp_moc * 10;
    zmien_pwm(pwm_1_zadany, 1);
    //Silnik1.writeMicroseconds(pwm_1_zadany);
  }

  if (pwm_2_zadany > 1000 + wsp_moc * 10) {
    pwm_2_zadany = 1000 + wsp_moc * 10;
    zmien_pwm(pwm_2_zadany, 2);
    //Silnik2.writeMicroseconds(pwm_2_zadany);
  }
  server.send(200, "text/plain", String(wsp_moc));
  return;
}

void zmierzOdleglosc() {
  stara_wysokosc = wysokosc;
  if (lox.isRangeComplete())
    wysokosc = ((float)lox.readRange()) / 10;

  predkosc_wysokosci = (wysokosc - stara_wysokosc) / 0.02;
  return;
}

void sterowanie() {
  //najpierw wysterowujemy wysokość

  /*
  static float ts = 0;
  static long t_minal = micros();
  ts = (micros() - t_minal) / 1000000.0;  //podzielic
  t_minal = micros();
  */

  static float ts = 0.02;
  //dane do sterowania kątem

    static float e_kat;
    static float e_kat_stary;
    static float calka_kat_plus, calka_kat_minus;
    
  // dane do sterowania wysokością
    static float e_wys;
    static float e_wys_stary;
    static float calka_wys;

  

    if(stop){
      calka_wys = calka_kat_plus = calka_kat_minus =0;
      stop = false;
      return;
    }

  if (sterowanie_tryb == 'k' || sterowanie_tryb == 'o') {

    float ster_kat_plus = 0;   //dodatni uchyb - chcemy żeby podniósł się LANRC35A
    float ster_kat_minus = 0;  /// czyli silnik1 musi byc podpięty do lanrc2


    e_kat = kat_zadany - KalmanAnglePitch;
    //e_kat = 0;
    ster_kat_plus = PID(kp_kat, ki_kat, kd_kat, e_kat, e_kat_stary, &calka_kat_plus, ts);
    ster_kat_minus = PID(kp_kat, ki_kat, kd_kat, -e_kat, -e_kat_stary, &calka_kat_minus, ts);
    e_kat_stary = e_kat;
    //e_kat_stary = 0;

    ster_kat_plus *= 10;  //zamiast na dole to tutaj
    ster_kat_minus *= 10;

    pwm_1_zadany = (int)ster_kat_plus;
    pwm_1_zadany += 1000;

    pwm_2_zadany = (int)ster_kat_minus;
    pwm_2_zadany += 1000;
  }

  if (sterowanie_tryb == 'w' || sterowanie_tryb == 'o') {

    float ster_wys = 0;
    e_wys = wysokosc_zadana - wysokosc;
    ster_wys = PID(kp_wys, ki_wys, kd_wys, e_wys, e_wys_stary, &calka_wys, ts);
    e_wys_stary = e_wys;
    pwm_1_zadany += ((int)(ster_wys * 10));
    pwm_2_zadany += ((int)(ster_wys * 10));
  }

  //eliminujemy potencjalne wykroczenie poza skalę

  nasycenie();

  zmien_pwm(pwm_1_zadany, 1);
  zmien_pwm(pwm_2_zadany, 2);
  //Silnik1.writeMicroseconds(pwm_1_zadany);
  //Silnik2.writeMicroseconds(pwm_2_zadany);
  return;
}

void nasycenie() {
  return;
  if (pwm_1_zadany > 1000 + 10 * wsp_moc || pwm_1_zadany > 2000)
    pwm_1_zadany = 1000 + 10 * wsp_moc;

  if (pwm_2_zadany > 1000 + 10 * wsp_moc || pwm_2_zadany > 2000)
    pwm_2_zadany = 1000 + 10 * wsp_moc;

  if (pwm_1_zadany < 1020)
    pwm_1_zadany = 1020;

  if (pwm_2_zadany < 1020)
    pwm_2_zadany = 1020;
}


float PID(float kp, float ki, float kd, float e, float e_stary, float* calka, float ts_PID) {

  *calka = *calka + e * ts_PID;
  float pochodna = (e - e_stary) / ts_PID;
  float u =kp * e + ki * (*calka)  + kd * pochodna;

  if (u > u_max) {
    u = u_max;
    *calka = (u_max - kp * e - kd * pochodna) / ki;
  }

  if (u < u_min) {
    u = u_min;
    *calka = (u_min - kp * e - kd * pochodna) / ki;
  }

  return u;
}

void Sterowanie1() {



  static float ts = 0;
  static long t_minal = micros();
  ts = (micros() - t_minal) / 1000000.0;  //podzielic
  t_minal = micros();

  kat_zadany1 = (float)kat_zadany;

  uchyb = kat_zadany1 - kat_stary;
  //if (abs(uchyb) > 1) {
  F1 = 0.02 * uchyb + 1.082 * x2;
  if (F1 > 14) {
    F1 = 14;
  } else if (F1 < -14) {
    F1 = -14;
  }
  F2 = -0.02 * uchyb + 1.171 * x2;
  if (F2 > 14) {
    F2 = 14;
  } else if (F2 < -14) {
    F2 = -14;
  }
  pwm_1_zadany = -1.8623 * abs(F1) * abs(F1) + 96.6731 * abs(F1) + 1071.87;
  pwm_2_zadany = -1.8623 * abs(F2) * abs(F2) + 96.6731 * abs(F2) + 1071.87;

  pwm_1_zadany = map(pwm_1_zadany, 1000, 2000, 0, 100);
  pwm_2_zadany = map(pwm_2_zadany, 1000, 2000, 0, 100);

  /*
    if (pwm_1_zadany < 1000) {
      pwm_1_zadany = 1000;
    } else if (pwm_1_zadany > 1200) {
      pwm_1_zadany = 1200;
    }
    if (pwm_2_zadany < 1000) {
      pwm_2_zadany = 1000;
    } else if (pwm_2_zadany > 1200) {
      pwm_2_zadany = 1200;
    }
    */

  x1 = (KalmanAnglePitch);  // + (int(KalmanAngleRoll*10) - int(KalmanAngleRoll)*10)/10.0;
  x2 = (x1 - kat_stary) / ts;
  kat_stary = x1;

  if (F1 > 0 || F2 > 0) {
    odwrocenie = false;
  } else if (F1 < 0 && F2 < 0) {
    odwrocenie = true;
  }
  if (!odwrocenie) {
    Silnik1.writeMicroseconds(pwm_1_zadany);
    Silnik2.writeMicroseconds(pwm_2_zadany);
  } else if (odwrocenie) {
    Silnik1.writeMicroseconds(pwm_2_zadany);
    Silnik2.writeMicroseconds(pwm_1_zadany);
  }
  //}
}

void WireReset() {
  Wire.setClock(400000);  //czestotliwosc zegara - 400 kHz
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
  for (RateCalibrationNumber = 0; RateCalibrationNumber < 2000; RateCalibrationNumber++) {
    gyro_signals();
    RateCalibrationRoll += RateRoll;
    RateCalibrationPitch += RatePitch;
    RateCalibrationYaw += RateYaw;
    delay(1);
  }

  //wartości kalibracyjne ustalone poprzez uśrednienie 2000 pomiarów
  RateCalibrationRoll /= 2000;
  RateCalibrationPitch /= 2000;
  RateCalibrationYaw /= 2000;
}

void loop() {

  watchdog_ts();

  if (kalibracja) {
    RateCalibrationRoll = 0;
    RateCalibrationPitch = 0;
    RateCalibrationYaw = 0;
    for (RateCalibrationNumber = 0; RateCalibrationNumber < 2000; RateCalibrationNumber++) {
      gyro_signals();
      RateCalibrationRoll += RateRoll;
      RateCalibrationPitch += RatePitch;
      RateCalibrationYaw += RateYaw;
      delay(1);
    }
    RateCalibrationRoll /= 2000;
    RateCalibrationPitch /= 2000;
    RateCalibrationYaw /= 2000;
    kalibracja = false;
  }

  //Serial.print("Roll Angle [°] ");
  //Serial.print(KalmanAngleRoll);
  //Serial.print(RateRoll);
  //Serial.print(" Pitch Angle [°] ");
  //Serial.println(RatePitch);
  //Serial.println(KalmanAnglePitch);

  if (test_odbioru_danych) {
    Serial.println("Regulator wysokosci: ");
    Serial.print("Wzmocnienie członu proporcjonalnego: ");
    Serial.println(kp_wys);
    Serial.print("Wzmocnienie członu całkującego: ");
    Serial.println(ki_wys);
    Serial.print("Wzmocnienie członu różniczkującego: ");
    Serial.println(kd_wys);
    Serial.println(" ");
    Serial.println("Regulator kata: ");
    Serial.print("Wzmocnienie członu proporcjonalnego: ");
    Serial.println(kp_kat);
    Serial.print("Wzmocnienie członu całkującego: ");
    Serial.println(ki_kat);
    Serial.print("Wzmocnienie członu różniczkującego: ");
    Serial.println(kd_kat);
  }

  server.handleClient();
}



void setup() {
  delay(1000);
  Serial.begin(9600);
  Wire.begin();
  Wire.setTimeout(1000);
  Serial.println();
  i2cdetect();

  pinMode(15, OUTPUT);   //AD0 do MPU
  digitalWrite(15, LOW);

  //pinMode(9, OUTPUT);   //ewentualnie AD0 do Vlx53lox
  //digitalWrite(9, LOW);

  Serial.println("Initializing MPU...");
  mpu.initialize();
  Serial.println("Testing MPU6050 connection...");
  delay(500);
  while (mpu.testConnection() == false) {
    Serial.println("MPU6050 connection failed");
    delay(1000);
  }

  Serial.println("MPU6050 connection successful");
  mpu.setDLPFMode(0x04); //filtr dolnoprzepustowy 10Hz  0x05, 5Hz 0x06  
  delay(1000);


  while (!lox.begin()) {
    Serial.println("nie udalo sie zbootowac czujnika wysokosci");
    delay(1500);
  }

  Serial.println("VLX53L0X connection successful");

  //tworzenie sieci bezprzewodowej z mikrokontrolera
  Serial.println("Konfugiracja access pointa...");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Adres IP AP: ");
  Serial.println(myIP);
  delay(200);
  server.on("/", SendWebsite);  //'wrzucenie' na serwer strony z danymi
  server.on("/xml", SendXML);
  server.on("/UPDATE_WYSOKOSC", UpdateWysokosc);
  server.on("/UPDATE_KAT", UpdateKat);
  server.on("/BUTTON_KALIBRACJA", KalibracjaUkladu);
  server.on("/AKTUALIZUJ_PWM_1", ZmianaPWM_1);
  server.on("/AKTUALIZUJ_PWM_2", ZmianaPWM_2);
  server.on("/AKTUALIZUJ_PWM_OBA", ZmianaPWM_oba);
  server.on("/AKTUALIZUJ_PID", Aktualizuj_PID);
  server.on("/UPDATE_STER", Aktualizuj_ster);
  server.on("/TEST_WYS", Test_wysylania_danych);
  server.on("/WIRE_RESET", WireReset);
  server.on("/PASEK_MOC", Pasek_moc);
  server.begin();
  delay(800);
  Serial.println("Serwer HTTP wystartował");

  //u_min = -wsp_moc;
  u_max = wsp_moc;

  //ustalenie wartości kalibracyjnych
  //2000 pomiarów, w których mpu5060 powinno leżeć na płaskim podłożu


  for (RateCalibrationNumber = 0; RateCalibrationNumber < 2000; RateCalibrationNumber++) {
    gyro_signals();
    RateCalibrationRoll += RateRoll;
    RateCalibrationPitch += RatePitch;
    RateCalibrationYaw += RateYaw;
    delay(1);
  }

  //wartości kalibracyjne ustalone poprzez uśrednienie 2000 pomiarów
  RateCalibrationRoll /= 2000;
  RateCalibrationPitch /= 2000;
  RateCalibrationYaw /= 2000;



  lox.startRangeContinuous();
}
