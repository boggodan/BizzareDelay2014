#pragma once
#include "math.h"

/**
 * Sine oscillator.
 */
class Sinosc
{
   public:
   //persistent variables
   float pers_freq;
   float a; 
   float *s;
   float sr;
   //TODO: Should use a lookup table to save CPU cycles on the sin() calls.
   Sinosc(float sampleRate, float freq);
   ~Sinosc();

   /**
   * Reset sample rate.
   */
   void reset(float sampleRate);

   /**
   * Set oscillator frequency.
   */
   void setFrequency(float freq);

   /**
   * Generate a new sample.
   */
   inline float getSample()
   {
      s[0] = s[0] - a*s[1];
      s[1] = s[1] + a*s[0];
      return s[0]*2+1.0f;
   }
};