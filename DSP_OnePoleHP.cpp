#pragma once
#include "DSP_OnePoleHP.h"

/**
* Kill denormal numbers;
*/
void OnePoleHP::killDenormal (float &val)
{
   if(val <1.0E-30 && val> -1.0E-30)
   val = 0;
}


OnePoleHP::OnePoleHP()
{
   sr = prevout = previn = out = 0.0f; 
   a0 = a1 = b1 = 0.0f;
	
}

/**
* Reset sample rate.
*/
void OnePoleHP::reset(float sampleRate)
{
   sr = sampleRate;
}

/**
* Set filter cutoff.
*/
void OnePoleHP::setCutoff(float fCut)
{
   float w = 2.0 * sr;
   float Norm;
   
   fCut *= 2.0F * 3.14;
   Norm = 1.0 / (fCut + w);
   a0 = w * Norm;
   a1 = -a0;
   b1 = (w - fCut) * Norm;
}


