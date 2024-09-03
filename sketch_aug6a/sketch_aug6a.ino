
int syg;

void setup() {
  // put your setup code here, to run once:
  syg = 7;
  pinMode(syg,OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  // digitalWrite(syg,HIGH);
  // delayMicroseconds(2000);
  // digitalWrite(syg,LOW);
  // delayMicroseconds(18000);
  // delay(10000);

  // digitalWrite(syg,HIGH);
  // delayMicroseconds(1000);
  // digitalWrite(syg,LOW);
  // delayMicroseconds(19000);
  // delay(10000);

  digitalWrite(syg,HIGH);
  delayMicroseconds(1500);
  digitalWrite(syg,LOW);
  delayMicroseconds(18500);

}
