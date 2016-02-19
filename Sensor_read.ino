#include <Sensirion.h>

#define ENA_ERRCHK  // Enable error checking code

const byte dataPin =  2;                 // SHTxx serial data
const byte sclkPin =  3;                 // SHTxx serial clock
const byte ledPin  = 13;                 // Arduino built-in LED
const unsigned long TRHSTEP   = 60000UL;  // Sensor query period, Getting readins every 1min.
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


// This version of the code checks return codes for errors
byte error = 0;

void setup() {
  byte stat;
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  delay(15);                             // Wait >= 11 ms before first cmd
  // Demonstrate status register read/write
  if (error = sht.readSR(&stat))         // Read sensor status register
    logError(error);
  Serial.print("Status reg = 0x");
  Serial.println(stat, HEX);
  if (error = sht.writeSR(LOW_RES))      // Set sensor to low resolution
    logError(error);
  if (error = sht.readSR(&stat))         // Read sensor status register again
    logError(error);
  Serial.print("Status reg = 0x");
  Serial.println(stat, HEX);
  // Demonstrate blocking calls
  if (error = sht.measTemp(&rawData))    // sht.meas(TEMP, &rawData, BLOCK)
    logError(error);
  temperature = sht.calcTemp(rawData);
  if (error = sht.measHumi(&rawData))    // sht.meas(HUMI, &rawData, BLOCK)
    logError(error);
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
    if (error = sht.meas(TEMP, &rawData, NONBLOCK)) // Start temp measurement
      logError(error);
    trhMillis = curMillis;
  }
  if (measActive && (error = sht.measRdy())) { // Check measurement status
    if (error != S_Meas_Rdy)
      logError(error);
    if (measType == TEMP) {                    // Process temp or humi?
      measType = HUMI;
      temperature = sht.calcTemp(rawData);     // Convert raw sensor data
      if (error = sht.meas(HUMI, &rawData, NONBLOCK)) // Start humi measurement
        logError(error);
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
  /*
  Serial.print("Temppi = ");   Serial.print(temperature);
  Serial.print(" C, Humidity = ");  Serial.print(humidity);
  Serial.print(" %, Dewpoint = ");  Serial.print(dewpoint);
  Serial.println(" C");
  */

  
}

// The following code is only used with error checking enabled
void logError(byte error) {
  switch (error) {
    case S_Err_NoACK:
      Serial.println("Error: No response (ACK) received from sensor!");
      break;
    case S_Err_CRC:
      Serial.println("Error: CRC mismatch!");
      break;
    case S_Err_TO:
      Serial.println("Error: Measurement timeout!");
      break;
    default:
      Serial.println("Unknown error received!");
      break;
  }
}
