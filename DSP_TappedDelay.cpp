//fractional delay line with Allpass Interpolation
//by Bogdan Vera under GPL License (redistribute in original form, with attribution)

#include "DSP_TappedDelay.h"

/**
Fast float to int casting with bit shifting.
*/
int ReadHead::FloatToInt(float x)
{
   unsigned e = (0x7F + 31) - ((* (unsigned*) &x & 0x7F800000) >> 23);
   unsigned m = 0x80000000 | (* (unsigned*) &x << 8);
   return int((m >> e) & -(e < 32));
}

ReadHead::ReadHead(int sampleRate, float newwidth)
{
   sr = sampleRate;
   width = newwidth;
   y=0;
   a = 0;
   delta = 0;
   final_read = 0;
   x1 = x2 = y1 = y2 = 0.0;
}
ReadHead::~ReadHead()
{
}

/**
Set chorus width.
*/
void ReadHead::setWidth(float newwidth)
{
   width = newwidth;
}

/**
Reset sample rate.
*/
void ReadHead::reset(int sampleRate, float newwidth)
{
   sr = sampleRate;
   width = newwidth;
}

/**
Fast float to int casting with bit shifting.
*/
int TapDelay::FloatToInt(float x)
{
   unsigned e = (0x7F + 31) - ((* (unsigned*) &x & 0x7F800000) >> 23);
   unsigned m = 0x80000000 | (* (unsigned*) &x << 8);
   return int((m >> e) & -(e < 32));
}

//set current delay in fractional samples
void TapDelay::setDelay(float value)
{	
   readHead->setWidth(value);
}

void TapDelay::setBloom(float newbloom)
{
   bloom = newbloom;
}

void TapDelay::reset(int sampleRate, float delay)
{
   //initialize basic variables
   d = 1;
   sr = sampleRate;
   read = 0;
   write = 1;

   //super secret ninja parameter smoothing
   a = 0.999f;
   b= 1.0f - a;
   z = 0.0f;
   bloom = 0;
   width = delay;
   for(int i =0; i<1552000; i++)
   {
      buffer1[i] = 0.0f;
   }
   readHead->reset(sampleRate,delay);
}

//constructor
TapDelay::TapDelay(float sampleRate, float newwidth)
{
   out = 0;
   az = 0;
   //initialize basic variables
   d = 1;
   sr = sampleRate;
   read = 0;
   write = 1;
   //4 seconds of delay should be enough at 96k (but what if they're not?!?!. Buffer size should be dynamic with tempo. TODO: Fix this.)
   buffer1 = new float[1552000];
   //super secret ninja parameter smoothing
   a = 0.999f;
   b= 1.0f - a;
   z = 0.0f;
   width = newwidth;
   bloom = 0;
   
   for(int i =0; i<1552000; i++)
   {
      buffer1[i] = 0.0f;
   }
   
   readHead = new ReadHead(sr,width);
}
//destructor
//destroy the delay buffer
TapDelay::~TapDelay()
{
	//delete the giant heap allocated buffer.
	delete buffer1;
	buffer1 = 0;
}
	
	