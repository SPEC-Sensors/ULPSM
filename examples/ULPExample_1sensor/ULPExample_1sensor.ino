/*
  ULP.cpp - Library for reading SPEC Sensors ULP.
  Revised by David E. Peaslee, May 22, 2020.
  Created by David E. Peaslee, OCT 27, 2016.
  Released into the public domain.
*/


#include "Arduino.h"
#include "ULP.h"

// These constants won't change.  They're used to give names to the pins used and to the sensitivity factors of the sensors:

const int C1 = A0;
const int T1 = A3;
//averaging times, keep these low, so that the ADC read does not overflow 32 bits. For example n = 5 reads ADC 4465 times which could add to 22bit number.
const int n = 5; //number of seconds to read gas sensor
const int m = 1; //number of seconds to read temperature sensor
const int s = 10; //number of seconds to read all sensors should be greater than n+m+1

const float Sf1 = 11.61; //nA/ppm change this to match your barcode!!!!

unsigned long etime;

SO2 sensor1(C1, T1, Sf1);  //Sensor Types are EtoH, H2S, CO, IAQ, SO2, NO2, RESP, O3, and SPEC (custom)
//O3 sensor2(C2, T2, Sf2);  //Example O3
//CO sensor3(C3, T3, Sf3);  //Example CO

//  Include these if using different boards with different voltages



void setup() {
  sensor1.pVcc = 5.04;  //analogRead Reference Voltage, maybe measure Aref??
  sensor1.pVsup = 3.31;  //voltage supplied to V+ of ULP, default is 3.3 Volts, probably should measure this as well.

  Serial.flush();
  Serial.begin(9600);    // initialize serial communications at 9600 bps:

  Serial.println();
  Serial.println("Setting Up.");

  Serial.print("Vsup for all sensors = ");
  Serial.println(sensor1.pVsup);
  Serial.print("Vcc for all sensors = ");
  Serial.println(sensor1.pVcc);
  Serial.print("Vref for sensor 1 = ");
  Serial.println(sensor1.pVref);
  

  //  Using resistor values from board R1, R2, R3 are for setting pVref and Bias, while R6 sets the gain
  //  If using modified or custom boards set Vref and Gain like this
  //long int R2 = 1000;  //Assumes R1 and R3 are 1 MOhms in resistor ladder
  //float bias = -25.0
  //sensor2.setVref(bias, R2);
  //sensor2.pGain = 100000; //resistor R6

  //if you know the V_ref replace the following code...
  Serial.println("Remove Sensor.");
  if (sensor1.OCzero(n)) {
    Serial.print("Vref new = ");
    Serial.println(sensor1.pVref_set);
  } else {
    Serial.println("Recheck Settings, Zero out of range");
    while (1) {
      Serial.println(analogRead(A0));
      delay(1000);
      }
  }
  //...with this code and your measured value of new Vref
  //sensor1.pVref_set = ????
  


  //sensor1.setXSpan();                                //Must have previously zeroed in clean air, returns new span factor.

  //When calibrating the temperature use "LOW"/"HIGH" for the temperature range ie .setTSpan(40.2, "HIGH") where T is the current high temperature
  //sensor1.setTSpan((71 - 32.0) * 5.0 / 9.0, "LOW");


  Serial.println("Finished Setting Up, Replace Sensor Now.");
  Serial.println("T1, mV, nA, C1");
  etime = millis();
}

void loop() {
  
  while (Serial.available()) {
    if (Serial.read() == 'Z') {
      Serial.println("Zeroing");
      sensor1.zero(); //Uses last values read of Izero and Tzero
      Serial.print("Izero, Tzero: ");
      Serial.print(sensor1.pIzero);
      Serial.print(", ");
      Serial.println(sensor1.pTzero);
    }
  }

  if (millis() - etime > s * 1000) {
    etime = millis();

    sensor1.getIgas(n);
    sensor1.getTemp(m);
    sensor1.getConc(sensor1.pT);

    Serial.print(sensor1.convertT('C')); //use 'C' or 'F' for units
    Serial.print(", ");
    Serial.print(sensor1.pVgas);
    Serial.print(", ");
    Serial.print(sensor1.pInA);
    Serial.print(", ");
    Serial.println(sensor1.convertX('M')); //use 'M' or 'B' for units or Serial.println(sensor1.pX); to just print ppb

  } else if (millis() - etime < 0)    etime = millis();

}
