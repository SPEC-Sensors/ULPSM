/*
  ULP.cpp - Library for reading SPEC Sensors ULP.
  Created by David E. Peaslee, OCT 27, 2017.
  Released into the public domain.
*/

#include "Arduino.h"
#include "ULP.h"

//#define DEBUG
#ifdef DEBUG
	#define DEBUG_PRINT(x)     Serial.print (x)
#else 
	#define DEBUG_PRINT(x)
#endif



ULP::ULP (int a, int b, float c) : _CPin(a), _TPin(b), _Sf(c) {
  _Htemp = 40.0;
  _Ltemp = 20.0; //temps for cal of temp sensor
  _Tb = 18.0;
  _Ts = 87.0;
  _Tz = 20.0;
  _Hvolt = (_Htemp + _Tb) * _Vsup / _Ts;
  _Lvolt = (_Ltemp + _Tb) * _Vsup / _Ts; //volts for cal of temp sensor
}

float ULP::_Vcc = 5.0; 	//voltage for ADC conversion of analogRead()	
float ULP::_Vsup = 3.3;	//voltage of ULP supply for thermister, etc.

int  ULP::getTemp(int n, char* U) { //get temperature using n as time in seconds for averaging, and U for units "F"/"C"
  unsigned long etime, i = 0;
  unsigned long AC = 0;
  float temp;
  etime = millis() + n * 1000;

  do {
    AC = AC + analogRead(_TPin);
    delay(1);
    i++;
  } while (millis() < etime);
  
  DEBUG_PRINT(AC);
  DEBUG_PRINT(", ");
  
  float Cnts = float (AC) / float(i);
  float Volts = Cnts * _Vcc / 1024.0;

  temp = (_Ts / _Vsup) * Volts - _Tb;

  if (U == "F") {
    return temp * 9 / 5 + 32;
  }
  else if ( U == "C") {
    return temp;
  }
  else return 0;
}

void ULP::setTSpan(float t, String R) { //advanced function for setting high or low calibration factor for thermistor
  Serial.print(F("Old temp. span and offset: "));
  Serial.print(_Ts);
  Serial.print(F(", "));
  Serial.println(_Tb);
  unsigned long etime, i = 0, n = 10;
  unsigned long AC = 0;
  etime = millis() + n * 1000;
  do {
    AC = AC + analogRead(_TPin);
    delay(1);
    i++;
  } while (millis() < etime);
  float Cnts = float (AC) / float(i);
  float Volts = Cnts * _Vcc / 1024;

  if (R == "HIGH") {
    _Htemp = t;
    _Hvolt = Volts;
  }
  else if (R == "LOW") {
    _Ltemp = t;
    _Lvolt = Volts;
  }
  _Ts = _Vsup * (_Htemp - _Ltemp) / (_Hvolt - _Lvolt);
  _Tb = _Lvolt * (_Htemp - _Ltemp) / (_Hvolt - _Lvolt) - _Ltemp;
  Serial.print(F("New temp. span and offset: "));
  Serial.print(_Ts);
  Serial.print(F(", "));
  Serial.println(_Tb);
}

float  ULP::setVref (long R1, long R2, long R3, int bias) {	//function for setting vref for custom sensors. Not critical value as call to concentration cancels out _Vref with _Voff
	if (bias>0){
		_Vref = _Vsup * float(R1+R2) / float(R1 + R2 + R3) * 1000.0;
	}
	else if(bias<0) {
		_Vref = _Vsup * float(R1) / float(R1 + R2 + R3) * 1000.0;
	}
	else return 0;
  
  return _Vref;
}

float  ULP::zero() { //function to zero sensor and get temperature at that zero _Tz
  unsigned long etime, i = 0, n = 10;
  unsigned long AC = 0;
  etime = millis() + n * 1000;
  do {
    AC = AC + analogRead(_CPin);
    delay(1);
    i++;
  } while (millis() < etime);
  float Cnts = float (AC) / float(i);

  float Vgas = Cnts * _Vcc * 1000.0 / 1024.0; //in mV
  _Voff = Vgas - _Vref;
  _Tz = getTemp(10);
  return _Voff;
} //returns voltage at zero concentration

float ULP::getVgas(int n)	//gets the gas voltage from analog read. using n seconds as averaging time
{
  unsigned long etime, i = 0;
  unsigned long AC = 0;
  etime = millis() + n * 1000;
  do {
    AC = AC + analogRead(_CPin);
    delay(1);
    i++;
  } while (millis() < etime);
  float Cnts = float (AC) / float(i);

  float Vgas = Cnts * _Vcc * 1000.0 / 1024.0; //in mV

  return Vgas;
} //returns voltage at zero concentration



float  ULP::getConc(int n, float t) { //gets the concentration using n seconds as averaging time, with temperature correction if including t, otherwise, not temperature corrected.

  float nA = (getVgas(n) - _Vref - _Voff)/1000.0/ float(_Gain) * 1000000000.0;
    DEBUG_PRINT(nA);
	DEBUG_PRINT(", ");
  if (t>23){
	nA = nA - _Ah*(t-_Tz); 
  }
  else{
	nA = nA - _Al*(t-_Tz);
  }
  
  float Conc = nA /_Sf*(1-_Tc*(t-_Tz));
 
  return Conc;
}

float ULP::setXSpan() { //advanced function for setting the new sensitivity factor when at 20C. Must be withing 10% of previous sensitivity.
  Serial.setTimeout(10000);
  float X;
  float nA, Sf;
  Serial.print(F("When gas concentration steady, enter Concentration in ppm followed by 'cr' = "));
  while (Serial.available() <= 0) {}
  X = Serial.parseFloat();
  Serial.println(X);
  nA = (getVgas(30) - _Vref - _Voff) / 1000.0 / _Gain * 1000000000;
  Sf = nA / X;
  if(abs(Sf-_Sf)*2/(Sf+_Sf)<.1){
    _Sf = Sf;
  }
  else {
    Serial.println(F("Error Setting Span"));
  }
  return _Sf;

}

EtOH::EtOH(int a, int b, float c) : ULP(a, b, c) {
  setVref(1000000, 69800, 1000000, +1);
  _Gain = 249000;
  _Tc = 0.6;
  _Ah = 22.5;
  _Al = 1.7;
}

H2S::H2S (int a, int b, float c): ULP(a, b, c) {
  setVref(1000000, 2000, 1000000, +1);
  _Gain = 49900;
  _Tc = 0.15;
  _Ah = 1.63;
  _Al = 0.55;
}

CO::CO(int a, int b, float c) : ULP(a, b, c) {
  setVref(1000000, 2000, 1000000, +1);
  _Gain = 100000;
  _Tc = 0.5;
  _Ah = 3.91;
  _Al = 1.11;
}

IAQ::IAQ(int a, int b, float c) : ULP(a, b, c) {
  setVref(1000000, 105000, 1000000, +1);
  _Gain = 100000;
  _Tc = 0.5;
  _Ah = 16.20;
  _Al = 4.61;
}

SO2::SO2(int a, int b, float c) : ULP(a, b, c) {
  setVref(1000000, 143000, 1000000, +1);
  _Gain = 100000;
  _Tc = 0.85;
  _Ah = 0.41;
  _Al = 0.14;
}

NO2::NO2(int a, int b, float c) : ULP(a, b, c) {
  setVref(1000000, 143000, 1000000, -1);
  _Gain = 499000;
  _Tc = 0.02;
  _Ah = 0.02;
  _Al = 0.01;
}

RESP::RESP(int a, int b, float c) : ULP(a, b, c) {
  setVref(1000000, 143000, 1000000, -1);
  _Gain = 499000;
  _Tc = 0.02;
  _Ah = 0.99;
  _Al = 0.26;
}

O3::O3(int a, int b, float c) : ULP(a, b, c) {
  setVref(1000000, 16200, 1000000, -1);
  _Gain = 499000;
  _Tc = 0.25;
  _Ah = 0.04;
  _Al = 0.02;
}

SPEC::SPEC(int a, int b, float c): ULP(a, b, c) {
	//must manually set gain, Tc, Ah, Al, and setVref
}






