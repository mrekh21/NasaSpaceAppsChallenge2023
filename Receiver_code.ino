#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
LiquidCrystal_I2C lcd(0x27,20,4); 


RF24 radio(7, 8); // CE, CSN

void setup() {
    Wire.begin();
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  radio.begin(); 
  radio.startListening(); 

  radio.openReadingPipe(1, 0xF0F0F0E1E1LL);  
  radio.openReadingPipe(2, 0xF0F0F0E1E2LL);
}

void loop() {
  
    lcd.setCursor(0, 0);

    char receivedData[32] = "";
    if (radio.available()) {
    radio.read(&receivedData, sizeof(receivedData)); 
    Serial.println("Received Data From Duckling: " + String(receivedData));
    String data = String(receivedData);
    int dNum = (data.substring(0, 1)).toInt();
    String coeff = (data.substring(2, 4));
    int k = coeff.toInt();
    if( k < 1) k = random(1, 10);
    Serial.println(coeff);

    if(dNum == 1){
      lcd.setCursor(0, 0);
      lcd.print("Sensor 1: ");
      lcd.print(String(k));    
    }
    if(dNum == 2){
      lcd.setCursor(0, 1);
      lcd.print("Sensor 2: ");
      lcd.print(String(k));       
    }   
  }
  
}