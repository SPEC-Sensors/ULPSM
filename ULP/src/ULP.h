/*
  ULP.h - Library for reading SPEC Sensors ULP.
  Created by David E. Peaslee, OCT 27, 2017.
  Released into the public domain.
*/
#ifndef ULP_h
#define ULP_h

#include "Arduino.h"

class ULP {
    //  static float _Vsup;
    //static float _Vcc;
  private:
    const int _CPin, _TPin;
    float _Sf;  //initializers for sensor
    float _Htemp, _Ltemp;   //temps for cal of temp sensor
    float _Hvolt, _Lvolt;   //volts for cal of temp sensor
    float _Tb, _Ts;         //span and offset for temp equation

  public:

    static float _Vcc;  //analog read reference voltage, usually 5 V for Uno
    static float _Vsup; //voltage supplied to ULP, !!!! max 3.3 V !!!!
    float _Voff;        //offset voltage due to baseline at room temp, with no gas
    float _Vref;        //vref is voltage divider, ideally voltage with no current through circuit (electronic zero)
    float _Tc;          //temperature span correction coefficient
	float _Ah;			//temperature baseline correction coefficient
	float _Al;			//temperature baseline correction coefficient

	float _Tz =20;  	//zero temp for temp correction
    long int _Gain;     //gain of trans impedance amplifier (TIA)

    ULP (int a, int b, float c);

    int getTemp(int n, char* U = "C");

	float getConc(int n, float t = 20);
    
	float setVref (long R1, long R2, long R3, int bias);

    float zero();

    float getVgas(int n);

    float setXSpan();

    void setTSpan(float t, String R);
};

class EtOH: public ULP {
  public:
    EtOH(int a, int b, float c);
};

class H2S: public ULP {
  public:
    H2S (int a, int b, float c);
};

class CO: public ULP {
  public:
    CO(int a, int b, float c) ;
};

class IAQ: public ULP {
  public:
    IAQ(int a, int b, float c);
};

class SO2: public ULP {
  public:
    SO2(int a, int b, float c);
};

class NO2: public ULP {
  public:
    NO2(int a, int b, float c);
};

class RESP: public ULP {
  public:
    RESP(int a, int b, float c);
};

class O3: public ULP {
  public:
    O3(int a, int b, float c);
};

class SPEC: public ULP {
  public:
    SPEC(int a, int b, float c);
};

#endif
