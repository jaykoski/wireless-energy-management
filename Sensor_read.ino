#include <Sensirion.h>


const byte dataPin =  2;                 // SHTxx serial data
const byte sclkPin =  3;                 // SHTxx serial clock
const byte ledPin  = 13;                 // Arduino built-in LED
const unsigned long TRHSTEP   = 50000UL;  // Sensor query period
const unsigned long BLINKSTEP =  250UL;  // LED blink period

Sensirion sht = Sensirion(dataPin, sclkPin);

unsigned int rawData;
float temperature;
float humidity;
float dewpoint;

byte ledState = 0;
byte measActive = false;
byte measType = TEMP;

unsigned long trhMillis = 0;             // Time interval tracking
unsigned long blinkMillis = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  delay(15);                             // Wait >= 11 ms before first cmd
  
  // Demonstrate blocking calls
  sht.measTemp(&rawData);    // sht.meas(TEMP, &rawData, BLOCK)
  temperature = sht.calcTemp(rawData);
  sht.measHumi(&rawData);    // sht.meas(HUMI, &rawData, BLOCK)
  humidity = sht.calcHumi(rawData, temperature);
  dewpoint = sht.calcDewpoint(humidity, temperature);
  logData();
}

void loop() {
  unsigned long curMillis = millis();          // Get current time


  // Rapidly blink LED.  Blocking calls take too long to allow this.
  if (curMillis - blinkMillis >= BLINKSTEP) {  // Time to toggle the LED state?
    ledState ^= 1;
    digitalWrite(ledPin, ledState);
    blinkMillis = curMillis;
  }

  // Demonstrate non-blocking calls
  if (curMillis - trhMillis >= TRHSTEP) {      // Time for new measurements?
    measActive = true;
    measType = TEMP;
    sht.meas(TEMP, &rawData, NONBLOCK); // Start temp measurement
    trhMillis = curMillis;
  }
  if (measActive && sht.measRdy()) { // Check measurement status
    if (measType == TEMP) {                    // Process temp or humi?
      measType = HUMI;
      temperature = sht.calcTemp(rawData);     // Convert raw sensor data
      sht.meas(HUMI, &rawData, NONBLOCK); // Start humi measurement
    } else {
      measActive = false;
      humidity = sht.calcHumi(rawData, temperature); // Convert raw sensor data
      dewpoint = sht.calcDewpoint(humidity, temperature);
      logData();
    }
    }
 }


void logData() {
  Serial.print(temperature);
  Serial.print(" \t");
  Serial.print(humidity);       //Printing values
  Serial.print(" \t");
  Serial.print(dewpoint); 
}

