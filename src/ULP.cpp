/*
  ULP.cpp - Library for reading SPEC Sensors ULP.
  Revised by David E. Peaslee, May 22, 2020.
  Created by David E. Peaslee, OCT 27, 2016.
  Released into the public domain.
*/

#include "Arduino.h"
#include "ULP.h"

ULP::ULP (int a, int b, float c) : pCPin(a), pTPin(b), pSf(c) {

  pTzero = 20.0;
  pIzero = 0.0;
  
  //Temperature Sensor Settings
  pHtemp = 40.0;
  pLtemp = 20.0; //temps for cal of temp sensor
  pTb = 18.0;     //temperature sensor coef
  pTs = 87.0;     //temperature sensor coef
  pHvolt = (pHtemp + pTb) * pVsup / pTs;  //volts for cal of temp sensor
  pLvolt = (pLtemp + pTb) * pVsup / pTs; //volts for cal of temp sensor
}

//float ULP::pVcc = 5.0;
//float ULP::pVsup = 3.3;

void ULP::getTemp(int n) {
  unsigned long etime, i = 0;
  unsigned long anaCounts = 0;
  etime = millis() + n * 1000;
  do {
    anaCounts = anaCounts + analogRead(pTPin);
    delay(1);
    i++;
  } while (millis() < etime);
  float Cnts = float (anaCounts) / float(i);
  float Volts = Cnts * pVcc / 1024.0;

  pT = (pTs / pVsup) * Volts - pTb;
  
 
}

float ULP::convertT(char U){
   if (U == 'F') {
    float TempF = pT * 9 / 5 + 32;
    return TempF;
  }
  else if ( U == 'C') {
    return pT;
  }
  else return 0;
}

float ULP::convertX(char U){
  if (U == 'B')  return pX;
  else if (U == 'M') return (pX/1000.0);
  else return 0;
}

void ULP::setTSpan(float t, String R) {
  Serial.print("Old temp. span and offset: ");
  Serial.print(pTs);
  Serial.print(", ");
  Serial.println(pTb);
  unsigned long etime, i = 0, n = 10;
  unsigned long anaCounts = 0;
  etime = millis() + n * 1000;
  do {
    anaCounts = anaCounts + analogRead(pTPin);
    delay(1);
    i++;
  } while (millis() < etime);
  float Cnts = float (anaCounts) / float(i);
  float Volts = Cnts * pVcc / 1024;

  if (R == "HIGH") {
    pHtemp = t;
    pHvolt = Volts;
  }
  else if (R == "LOW") {
    pLtemp = t;
    pLvolt = Volts;
  }
  pTs = pVsup * (pHtemp - pLtemp) / (pHvolt - pLvolt);
  pTb = pLvolt * (pHtemp - pLtemp) / (pHvolt - pLvolt) - pLtemp;
  Serial.print("New temp. span and offset: ");
  Serial.print(pTs);
  Serial.print(", ");
  Serial.println(pTb);
}

void  ULP::setVref (int b, long R2) {
  //Caluclate Expected Vref
  if (b >= 0)  pVref = pVsup * float(R2 + 1000000) / float(R2 + 2000000) * 1000.0;
  else  pVref = pVsup * float(1000000) / float(R2 + 2000000) * 1000.0;
  pVref_set = pVref;
}

bool  ULP::OCzero (int n) {
  //Measure real Vref
  unsigned long etime, i = 0;
  unsigned long anaCounts = 0;
  Serial.println("Send any character when sensor is removed.");
  while (Serial.available() <= 0) {}
  Serial.println("Zeroing");
  Serial.flush();
  etime = millis() + n * 1000;
  do {
    anaCounts = anaCounts + analogRead(pCPin);
    delay(1);
    i++;
  } while (millis() < etime);
  float Cnts = float (anaCounts) / float(i);
  pVref_set = Cnts * pVcc * 1000.0 / 1024.0; //in mV
      Serial.println(abs(pVref - pVref_set));

  if (abs(pVref - pVref_set) > 50)return false;
  else return true;
}

void  ULP::zero() {
  pIzero = pInA;
  pTzero = pT;
} 

void ULP::getIgas(int n){
  unsigned long etime, i = 0;
  unsigned long anaCounts = 0;
  etime = millis() + n * 1000;
  do {
    anaCounts = anaCounts + analogRead(pCPin);
    delay(1);
    i++;
  } while (millis() < etime);
  float Cnts = float (anaCounts) / float(i);

  pVgas = Cnts * pVcc * 1000.0 / 1024.0; //in mV
  pInA = (pVgas-pVref_set)/pGain * 1000.0; //in nA 
} 

void  ULP::getConc(float t) {
  float nA = pInA - pIzero * expI(t-pTzero);
  float Sens = pSf * (1.0 + pTc * (t - 20.0));
  pX = nA / Sens * 1000.0; //output in ppb
}

float ULP::expI(float T){
  return exp(T/pn);
}

void ULP::setXSpan() {
  Serial.setTimeout(10000);
  float X;
  float nA, Sf;
  Serial.print("When gas concentration steady, enter Concentration in ppm followed by 'cr' = ");
  while (Serial.available() <= 0) {}
  X = Serial.parseFloat();
  Serial.println(X);
  getIgas(10);
  
  Sf = pInA / X;
  if (abs(Sf - pSf) * 2 / (Sf + pSf) < .1) {
    pSf = Sf;
  }
  else {
    Serial.println("Error Setting Span");
  }

}

EtOH::EtOH(int a, int b, float c) : ULP(a, b, c = 14.0) {
  setVref(+100, 69800);
  pGain = 249.0;
  pn = 1;
  pTc = 0.01;
}

H2S::H2S (int a, int b, float c): ULP(a, b, c = 194.0) { //works
  setVref(+3, 2000);
  pGain = 49.9;
  pn = -300.0;
  pTc = 0.007;
}

CO::CO(int a, int b, float c) : ULP(a, b, c = 2.44) {
  setVref(+3, 2000);
  pGain = 100.0;
  pn = 13.6;
  pTc = 0.007;
}

IAQ::IAQ(int a, int b, float c) : ULP(a, b, c = 150.0) {
  setVref(+150, 105000);
  pGain = 100.0;
  pn = 1;
  pTc = 0.01;
}

SO2::SO2(int a, int b, float c) : ULP(a, b, c = 14.6) {
  setVref(+200, 143000);
  pGain = 100.0;
  pn = 10.26;
  pTc = -0.026;
}

NO2::NO2(int a, int b, float c) : ULP(a, b, c = -25.0) {
  setVref(-25, 16200);
  pGain = 499.0;
  pn = 109.6;
  pTc = 0.005;
}

RESP::RESP(int a, int b, float c) : ULP(a, b, c = -21.5) {
  setVref(-200, 143000);
  pGain = 499.0;
  pn = 1;
  pTc = 0.00;
}

O3::O3(int a, int b, float c) : ULP(a, b, c = -20.0) {
  setVref(-25, 16200);
  pGain = 499.0;
  pn = 109.6;
  pTc = -0.005;
}

SPEC::SPEC(int a, int b, float c): ULP(a, b, c = 1.0) {
}
