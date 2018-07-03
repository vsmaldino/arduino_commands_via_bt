/*
Arduino Turn LED On/Off using Serial Commands
Created April 22, 2015
Hammad Tariq, Incubator (Pakistan)
Vito A. Smaldino (Italy)

Possible string values:
a (to turn the LED on)
b (to turn the LED off)
v (to read battery voltage )
t (to read temperature from ds18b20 )
*/

// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 4   
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/ 

//Includo libreria SoftwareSerial
#include <SoftwareSerial.h>
 
//definisco pin RX e TX da Arduino verso modulo BT
#define BT_TX_PIN 3
#define BT_RX_PIN 2
 
//istanzio oggetto SoftwareSerial (il nostro futuro bluetooth)
SoftwareSerial bt =  SoftwareSerial(BT_RX_PIN, BT_TX_PIN);


char junk;
String inputString="";
int sensor0Value;
float sensor0Voltage;
float batteryVoltage;
float temper;
int command;

void writeResponseOnOff(String message) {
   bt.print("<response> ");
   bt.print("<action> ");
   bt.print(message);
   bt.print("</action> ");
   bt.print("</response> ");
   bt.print("\n");
} // writeResponseOnOff

void writeResponseError(String message) {
   bt.print("<response> ");
   bt.print("<error> ");
   bt.print(message);
   bt.print("</error> ");
   bt.print("/<response> ");
   bt.print("\n");
} // writeResponseError

void writeResponsePower(float pinvoltage, float battery) {
   bt.print("<response> ");
   bt.print("<power> ");
   bt.print("<pinvoltage> ");
   bt.print(pinvoltage);
   bt.print("</pinvoltage> ");
   bt.print("<battery> ");
   bt.print(battery);
   bt.print("</battery> ");
   bt.print("</power> ");
   bt.print("</response> ");
   bt.print("\n");
} // writeResponsePower

void writeResponseTemp(float temperature) {
   bt.print("<response> ");
   bt.print("<temperature> ");
   bt.print(temperature);
   bt.print("</temperature> ");
   bt.print("</response> ");
   bt.print("\n");
} // writeResponseTemp

void setup()                    // run once, when the sketch starts
{
   //definisco modalità pin
   pinMode(BT_RX_PIN, INPUT);
   pinMode(BT_TX_PIN, OUTPUT);
   // sensore
   sensors.begin();
  
   Serial.begin(9600);            // set the baud rate to 9600, same should be of your Serial Monitor
   bt.begin(9600);            // set the baud rate to 9600, same should be of your Serial Monitor
   pinMode(13, OUTPUT);
} // setup

void loop()
{
   if(bt.available()) {
      while(bt.available()) {
         char inChar = (char)bt.read(); //read the input
         inputString += inChar;        //make a string of the characters coming on serial
      }
      
      delay(100);
      while (bt.available()) {
         junk = (char)bt.read() ;       // clear the serial buffer
         delay(100);
      }

      command = inputString[0];
      switch (command) {
         case 'a':
            digitalWrite(13, HIGH);
            Serial.println("Accendo");
            writeResponseOnOff("On");
            break;
         case 'b':
            digitalWrite(13, LOW);
            Serial.println("Spengo");
            writeResponseOnOff("Off");
            break;
         case 'v':
            sensor0Value=analogRead(A0);
            sensor0Voltage=sensor0Value*(5.0/1023.0);
            batteryVoltage=sensor0Voltage*3.165;
            Serial.print("Tensione: ");
            Serial.print(sensor0Voltage);
            Serial.print(" Batteria: ");
            Serial.println(batteryVoltage);
            writeResponsePower(sensor0Voltage, batteryVoltage);
            break;
         case 't':
            sensors.requestTemperatures(); // Send the command to get temperatures
            temper=sensors.getTempCByIndex(0);
            Serial.print("Temperatura: ");
            Serial.println(temper);
            writeResponseTemp(temper);
            break;
         default:
            Serial.println("Unknown command!");
            // delay(1000);
            writeResponseError("Unknown command!");           
      }
      inputString = "";     
   }
} // loop



