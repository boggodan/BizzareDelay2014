#pragma once

/**
 * One pole lowpass filter.
 */
class OnePoleLP
{
   public:
   float prevout, previn,out;
   float sr;
   float a0,a1,b1;

   /**
   * Kill denormal numbers.
   */
   void killDenormal (float &val);
   OnePoleLP();
   /**
   * Reset sample rate.
   */
   void reset(float sampleRate);

   /**
   * Set cutoff.
   */
   void setCutoff(float fCut);

   /**
   * Main process function.
   */
   inline float process(float in)
   {
      out = in*a0 + previn*a1 + out*b1;
      previn = in;
      killDenormal(out);
      killDenormal(previn);
      return out;
   }
};