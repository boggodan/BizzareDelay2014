#pragma once
#include "DSP_OnePoleLP.h"

/**
* Kill denormal numbers.
*/
void OnePoleLP::killDenormal (float &val)
{
   if(val <1.0E-30 && val> -1.0E-30)
   val = 0;
}

OnePoleLP::OnePoleLP()
{
   sr = prevout = previn = out = 0.0f; 
   a0 = a1 = b1 = 0.0f;
}

/**
* Reset sample rate.
*/
void OnePoleLP::reset(float sampleRate)
{
   sr = sampleRate;
}

/**
* Set cutoff.
*/
void OnePoleLP::setCutoff(float fCut)
{	
   float w = 2.0 * sr;
   float Norm;
   
   fCut *= 2.0F * 3.14f;
   Norm = 1.0 / (fCut + w);
   b1 = (w - fCut) * Norm;
   a0 = a1 = fCut * Norm;
}

