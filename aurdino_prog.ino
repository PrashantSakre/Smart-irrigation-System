/*  Author: Prashant Sakre 
 *  Date:   March - May 2020
 * 
 */
#include <dht.h>
#include <SoftwareSerial.h>

//10-reciverpin 11-transmitter
SoftwareSerial mySerial(10,11);

dht DHT;  
#define DHTxxPIN 4
int ack;

//Motor pin initialization
int motorPin=8;

//Moisture sensor initialization
int sensor_pin = A0;
int Moisture_value;

void setup()  {
    //Motorpin setup
    digitalWrite(motorPin,HIGH);
    pinMode(motorPin,OUTPUT);
    
  Serial.begin(115200);
  mySerial.begin(115200);
  Serial.println("Reading From the Sensor ....");

}

void loop() {
  
  //Reading from Moisture sensor
  Moisture_value = analogRead(sensor_pin);
  Moisture_value = map(Moisture_value,550,0,0,100);

  //Motor value initialization
  int value;
  
  //program for sending data to Esp  
  ack = 0;
  int chk = DHT.read11(DHTxxPIN);
  switch (chk)  {
    case DHTLIB_ERROR_CONNECT:
      ack = 1;
      break;
  }
  
  if (ack == 0) {
    //Turing ON motor if moisture is less then 10
    if (Moisture_value < 20 ){
      value = 11;
      Serial.println("Motor ON");
      digitalWrite(motorPin,LOW); //LOW means motor is ON
      delay(5000);
      digitalWrite(motorPin,HIGH); //HIGH means motor is OFF
    }
    else if (Moisture_value > 20) {
      value = 12;
      digitalWrite(motorPin,HIGH);
      Serial.println("Motor Off");
      //delay(500);
    }

    //printing on the serial Monitor
    Serial.print("Temperature(*C) = ");
    Serial.println(DHT.temperature, 0);
    Serial.print("Humidity(%) = ");
    Serial.println(DHT.humidity, 0);
    Serial.print("Moisture(%) =");
      if (Moisture_value < 10 && Moisture_value >= 0) {
        Serial.print('0'); 
        Serial.print('0');
      }
      else if (Moisture_value > 9 && Moisture_value < 100) Serial.print('0');
      else if (Moisture_value <= -1 && Moisture_value >= -9) Serial.print('0');
    Serial.println(Moisture_value);
    Serial.print("Motorstate =");
    Serial.println(value);
    //Serial.print("%");
    Serial.println("-------------------------");
    
    //------Sending Data to ESP8266--------//
    mySerial.print('*');                            // Starting char
    mySerial.print(DHT.temperature, 0);             //2 digit data
    mySerial.print(DHT.humidity, 0);                //2 digit data
      if (Moisture_value < 10 && Moisture_value >= 0) {     //3 digit data
        mySerial.print('0'); 
        mySerial.print('0');
      }
      else if (Moisture_value > 9 && Moisture_value < 100) mySerial.print('0');
      else if (Moisture_value < -1 && Moisture_value > -9) mySerial.print('0');
    mySerial.print(Moisture_value);
    mySerial.print(value);
    mySerial.print('#');                            // Ending char
    //------------------------------------//
    
    delay(25000);
  }
  
  
  if (ack == 1) {
    Serial.print("NO DATA");
    Serial.print("\n\n\n");
    delay(4000);
  }
}
//-------------Electronics-project-hub>com-------------//
