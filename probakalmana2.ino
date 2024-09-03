
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
float RateRoll, RatePitch, RateYaw;
float RateCalibrationRoll, RateCalibrationPitch, RateCalibrationYaw;
int RateCalibrationNumber;
float AccX, AccY, AccZ;
float AngleRoll, AnglePitch;
uint32_t LoopTimer;
float KalmanAngleRoll=0, KalmanUncertaintyAngleRoll=2*2;
float KalmanAnglePitch=0, KalmanUncertaintyAnglePitch=2*2;
float Kalman1DOutput[]={0,0};

const char* ssid = "moj_access_point"; //nazwa sieci utworzonej przez esp8266
ESP8266WebServer server(80);

String kodzik = "";

void handleRoot(){
  // funkcja pozwalajaca stworzyc 'strone internetowa' z danymi
  kodzik += "<!DOCTYPE html> <html> <body> <head> <meta http-equiv=\"refresh\" content=\"2\"> </head>"; //w tagu meta jest automatyczne odswiezanie strony co 2 sekundy
  kodzik += "<h1>Dane z MPU5060</h1>";
  kodzik += "<p>Kat obrotu wokol osi X: ";
  kodzik += KalmanAngleRoll;
  kodzik += "</p> <p>Kat obrotu wokol osi Y: ";
  kodzik += KalmanAnglePitch;
  kodzik += "</p> </body> </html>";

  server.send(200, "text/html", kodzik);
}

void kalman_1d(float KalmanState, float KalmanUncertainty, float KalmanInput, float KalmanMeasurement) {
  //funkcja realizujaca filtr Kalmana
  KalmanState=KalmanState+0.004*KalmanInput;  //nowa predykcja kata
  KalmanUncertainty=KalmanUncertainty + 0.004 * 0.004 * 4 * 4;  //niepewnosc predykcji (4 * 4 - wariancja pomiarow zyroskopu)
  float KalmanGain=KalmanUncertainty * 1/(1*KalmanUncertainty + 3 * 3); //wzmocnienie Kalmana - od niego zalezy, jak wazne sa pomiary, a jak wazna predykcja (3 * 3 - wariancja pomiarow akcelerometru)
  KalmanState=KalmanState+KalmanGain * (KalmanMeasurement-KalmanState);  //kolejna predykcja kata (na podstawie wzmocnienia Kalmana)
  KalmanUncertainty=(1-KalmanGain) * KalmanUncertainty; //niepewnosc kolejnej predykcji
  Kalman1DOutput[0]=KalmanState; 
  Kalman1DOutput[1]=KalmanUncertainty;
}

// 0.004 - okres próbkowania
// 4 - wariancja 
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
  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP(); 
  Serial.print("Adres IP AP: ");
  Serial.println(myIP);
  server.on("/", handleRoot); //'wrzucenie' na serwer strony z danymi
  server.begin();
  Serial.println("Serwer HTTP wystartował");
  //Serial.begin(57600);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
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
  LoopTimer=micros();
}
void loop() {
  server.handleClient();
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
  Serial.print(" Pitch Angle [°] ");
  Serial.println(KalmanAnglePitch);
  while (micros() - LoopTimer < 4000);
  LoopTimer=micros();
}