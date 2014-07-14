#pragma once
#include <math.h>

//Exponential Scaling for various params.
float getExpo(float in)
{
   return exp(log(20.0f) + in * (log(20000.0f) - log(20.0f))); 
}