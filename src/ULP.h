/*
  ULP.h - Library for reading SPEC Sensors ULP.
  Revised by David E. Peaslee, May 22, 2020.
  Created by David E. Peaslee, OCT 27, 2016.
  Released into the public domain.
*/
#ifndef ULP_h
#define ULP_h

#include "Arduino.h"

class ULP {

  private:
    const int pCPin, pTPin; //the pin numbers for ADC read

    //temperature sensor settings
    float pHtemp;
    float pLtemp; //temps for cal of temp sensor
    float pHvolt;  //volts for cal of temp sensor
    float pLvolt; //volts for cal of temp sensor

  public:
    //gas sensor settings
    float pSf;              //initializers for sensor
    float pVcc = 5.0;  //analog read reference voltage, usually 5 V for Uno
    float pVsup =3.3; //voltage supplied to ULP, !!!! max 3.3 V !!!!
    float pVref_set;    //initially set to pVref, then reset to include V of open circuit voltage during OCzero()
    float pVref;        //vref is voltage divider, this is set ideally voltage with no current through circuit (electronic zero)
    float pInA;         //the last calculated value of current for the sensor
    float pVgas;         //the last measured value of voltage for the sensor
    float pT;         //the last calculated value of temperature in degrees C
    float pX;         //the last calculated value of concentration in ppb
    
    //temperature sensor settings
    float pTb;     //temperature sensor coef
    float  pTs;     //temperature sensor coef

    //temperature correction coefficient
    float pTc;          //sensitivity coeficient
    float pn;           //exponential correction to baseline
    float pIzero;       //exponential coeficeient to correction to baseline reset during zero()
    float pTzero;       //exponential zero temperature factor

    float pGain;     //gain of trans impedance amplifier (TIA)

    ULP (int a, int b, float c);

    float convertT(char U = 'C');               //output T in F or C

    float convertX(char U = 'B');               //output X in ppm or ppb

    float expI(float T);         //custom exponential function

    void getTemp(int n);  //ADC read to calculate temp

    void getConc(float t = 20.0);   //ADC of sensor voltage, converted to current then corrected for temperature and gas

    void setVref (int b, long R2);       //basic setting to calculate initial offset, good for expected high levels of gas

    void zero();                         //Measure current values and store Izero and Tzero for temperature correction of baseline

    bool OCzero(int n = 10);                        //Measure differential of open circuit voltage, used for precise ADC measurments

    void getIgas(int n);                 //ADC of sensor voltage, converted to current

    void setXSpan();                     //not recomended for use, simple calibration of gas concentration, input is expected gas concentration

    void setTSpan(float t, String R);     //simple calibration of temperature sensor
};

class SPEC: public ULP {
  public:
    SPEC(int a, int b, float c = 1.0);
};

class EtOH: public ULP {
  public:
    EtOH(int a, int b, float c = 14.0);
};

class H2S: public ULP {
  public:
    H2S (int a, int b, float c = 194.0);
};

class CO: public ULP {
  public:
    CO(int a, int b, float c = 2.44) ;
};

class IAQ: public ULP {
  public:
    IAQ(int a, int b, float c = 150.0);
};

class SO2: public ULP {
  public:
    SO2(int a, int b, float c = 14.6);
};

class NO2: public ULP {
  public:
    NO2(int a, int b, float c = -25.0);
};

class RESP: public ULP {
  public:
    RESP(int a, int b, float c = -21.5);
};

class O3: public ULP {
  public:
    O3(int a, int b, float c = -20.0);
};



#endif
