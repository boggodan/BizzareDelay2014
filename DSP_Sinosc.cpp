#include "DSP_Sinosc.h"
//TODO: Should use a lookup table to save CPU cycles on the sin() calls... although this is only called once.
Sinosc::Sinosc(float sampleRate, float freq)
{
   a = 2.f*(float)sin(3.14*freq/sampleRate);
   pers_freq = freq;
   s = new float[2];
   s[0] = 0.5f;
   s[1] = 0.f;
   sr = sampleRate;
}

Sinosc::~Sinosc()
{
}

/**
* Reset sample rate.
*/
void Sinosc::reset(float sampleRate)
{
   a = 2.f*(float)sin(3.14*pers_freq/sampleRate);
   s = new float[2];
   s[0] = 0.5f;
   s[1] = 0.f;
   sr = sampleRate;
}

/**
* Set oscillator frequency.
*/
void Sinosc::setFrequency(float freq)
{
   pers_freq = freq;
   a = 2.f*(float)sin(3.14*pers_freq/sr);
}






