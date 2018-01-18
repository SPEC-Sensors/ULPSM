/*
  ULP - Library for reading SPEC Sensors ULP.
  Created by David E. Peaslee, OCT 27, 2017.
  Released into the SPEC Sensors domain.
*/

/* The library is built to use several SPEC ULP sensors. An Arduino Uno can handle 3 sensors, since 2 analog pins are required for each sensor.
  Functions include getTemp(seconds, C/F): returns temperature, setTSpan(seconds, "HIGH"/"LOW"): for calibrating temperature, setVref(R1,R2,R3) for custom sensors,
  getVgas(seconds): for getting voltage of sensor, getConc(seconds, temperature in degC): for getting temperature corrected concentration,
  setXSpan(): for setting the calibration factor with known concentration of gas. Variable include _Vcc: the voltage ref of the ADC, _Vsup: the voltage to the ULP, 
  and _Gain the value of resistor R6. For more details see ULP.cpp in the libraries folder and the data sheet at http://www.spec-sensors.com/wp-content/uploads/2016/06/Analog-SDK-Users-Manual-v18.pdf
*/  

/* As an example, the ULP is connected this way: Vgas (pin1) to A0, Vtemp (Pin3) to A3, Gnd (Pin6) to Gnd, V+ (Pin7 or Pin8) to 3.3V of Arduino (must not be above 3.3V!!!).*/

#include "Arduino.h"
#include "ULP.h"

// These constants won't change.  They're used to give names to the pins used and to the sensitivity factors of the sensors:

const int C1 = A0; //change these to the pins you are using, only need Vgas and Vtemp connected
const int T1 = A3;
const int C2 = A1;
const int T2 = A4;

const float Sf1 = 4.05;   //change this to your sensitivity factor in nA/ppm
const float Sf2 = -56.45; //change this to your sensitivity factor in nA/ppm

float temp1, temp2;
float Vzero1, Vzero2;
float zero1, zero2;

CO sensor1(C1, T1, Sf1);	//Sensor Types are EtOH, H2S, CO, IAQ, SO2, NO2, RESP, O3, and SPEC (custom)
O3 sensor2(C2, T2, Sf2);	//Example O3
//H2S sensor3(C3, T3, Sf3);	//Example H2S

//  Include these if using different boards with different voltages
//float ULP::_Vcc = 5.0;  //analogRead Reference Voltage
//float ULP::_Vsup =3.3;  //voltage supplied to V+ of ULP, ideally 3.3 Volts 


void setup() {
  Serial.flush();
  Serial.begin(9600);    // initialize serial communications at 9600 bps:

  Serial.println();
  Serial.println("Setting Up");

  Serial.print("Vsup for all sensors = ");
  Serial.println(ULP::_Vsup);
  Serial.print("Vcc for all sensors = ");
  Serial.println(ULP::_Vcc);
  Serial.print("Vref for sensor 1 = ");
  Serial.println(sensor1._Vref);
  Serial.print("Vref for sensor 2 = ");
  Serial.println(sensor2._Vref);
  
  //  Using resistor values from board R1, R2, R3 are for setting _Vref and Bias, while R6 sets the gain
  //  If using modified or custom boards set Vref and Gain like this
  //long int R1 = 61900, R2 = 1000, R3 = 1000000;
  //int bias = 1; //alternatively bias=-1; for negative bias.
  //sensor1.setVref(R1, R2, R3, bias); //will set the new Vref for custom sensor voltage ladder. bias is necessary to set the correct arrangement
  //sensor1._Gain = 49900; //resistor R6

  //  Vref is not necessary if zero() is called for each sensor. If you already know the sensor zero you can comment this out, and set the zero with zero1 = measured mV.
  Serial.print("Vzero1 = ");
  Serial.println(Vzero1 = sensor1.zero());   //.zero() sets and returns the baseline voltage at current temperature with only clean air present
  Serial.print("Tzero1 = ");
  Serial.println(sensor1._Tz);

  Serial.print("Vzero2 = ");
  Serial.println(Vzero2 = sensor2.zero());   //.zero() sets and returns the baseline voltage at current temperature with only clean air present
  Serial.print("Tzero2 = ");
  Serial.println(sensor2._Tz);
  
  Serial.println("Finished Setting Up");
  Serial.println("Temp (F), C1 (ppm), C2 (ppm)");

}

void loop() {
  // Use .getTemp(n, "F") to get temp in Fahrenheit, with n as int number of seconds for averaging and "F" or "C" for temp units
  temp1 = sensor1.getTemp(1,"F"); 
  temp2 = sensor2.getTemp(1,"F");  
  Serial.print((temp1+temp2)/2);
  Serial.print(", ");

  //Use .getConc(1, temp1) where temp1 is in deg C for temperature corrected span
  Serial.print(sensor1.getConc(1,sensor1.getTemp(1)));
  Serial.print(", ");
  Serial.println(sensor2.getConc(1,sensor2.getTemp(1)));

  delay(100);
}
