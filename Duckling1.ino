#include <dht11.h>
#include "SPI.h" 
#include "RF24.h" 

dht11 DHT11;

int analogCO = A1;
int analogInf = A0;
#define CE_PIN 9
#define CSN_PIN 10 

int coefficient = 0;
int digitalCO = 8;
int DHT11PIN =  4;

#define ThermistorPin A2
long ADC_Value;
float R1T = 1250; 
float logR2T, R2T, T;
int maxT = 90;
int maxIR = 1023;

//steinhart-hart coeficients for thermistor
float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;  
float temp_f;

#define INTERVAL_MS_TRANSMISSION 100

RF24 radio (CE_PIN, CSN_PIN); 

void setup() {
  Serial.begin(9600);
  pinMode(digitalCO,INPUT);
  radio.begin(); 
	radio.openWritingPipe(0xF0F0F0E1E1LL);
}

void loop() {
  
  // Read Sensors
  int co =  analogRead(analogCO);
  int chk = DHT11.read(DHT11PIN);
  int hum = DHT11.humidity;
  int temp = measureTemperature();
  int ir = analogRead(analogInf);
  temp = (temp * 100) / maxT;
  ir = map (ir, 0, 1024, 100, 0);
  co = map (co, 0, 600, 0, 100);
  int coef = (int)((hum + temp*2 + ir*3 + 4*co)/10);


 // Print data
 Serial.print("Humidity (%): ");
 Serial.println(hum);
  
  Serial.print("Temperature  (C): ");
  Serial.println(temp);

  Serial.print("IR  Intensity: ");
  Serial.println(ir);

  Serial.print("CO Sensor: ");
  Serial.println(co);
  
  Serial.print("Coefficient: ");
  Serial.println(coef);

  String anInfStr = String(ir);
  String humStr = String((int)hum);
  String tempStr = String((int)temp);
  String anCoStr = String(co);

  while (anInfStr.length() < 4){
    anInfStr = "0" + anInfStr;
  }
  while (humStr.length() < 4){
    humStr = "0" + humStr;
  }
  while (tempStr.length() < 4){
    tempStr = "0" + tempStr;
  }
  while (anCoStr.length() < 4){
    anCoStr = "0" + anCoStr;
  }

  //String sendStr = "1-" + tempStr + '-' + anInfStr + '-' + humStr + '-' + anCoStr;
  String sendStr = String(coef);
  sendStr = "1-" + sendStr;
  char send[32];
  sendStr.toCharArray(send, 32); 
 // Serial.println(sendStr);
  //Serial.println("Sending Duckling 1");
  radio.write(&send, sizeof(send));
  delay(INTERVAL_MS_TRANSMISSION);
  delay(1000);
}


int measureTemperature() {
  ADC_Value = 0;
  for (int i = 0; i < 50; i++) {
    ADC_Value = ADC_Value+analogRead(ThermistorPin);
    delay(1);
  }

  ADC_Value=ADC_Value/50;
  R2T = R1T * (1023.0 / (float)ADC_Value - 1.0);
  logR2T = log(R2T);
  int temp_c = (1.0 / (c1 + c2*logR2T + c3*logR2T*logR2T*logR2T));
  temp_c = temp_c - 273.15; 
  return temp_c;
}