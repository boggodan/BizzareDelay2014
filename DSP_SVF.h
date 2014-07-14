#include "math.h"


#pragma once

/**
State Variable Filter that outputs LP, HP, BP and Notch at once.
NO LONGER USED.
*/
class StateVariableFilter
{
   public:
   StateVariableFilter(float sampleRate, bool type);
   ~StateVariableFilter();
   void killDenormal (float &val);
   void SetCutoff(float cutoff);
   void SetResonance(float res);
   void reset(float sampleRate);
   inline float Process(float input);

   private:
   bool type;
   float cutoff;
   float f;
   float q;
   float LP,HP,BP,notch;
   float scale;
   float sr;
};