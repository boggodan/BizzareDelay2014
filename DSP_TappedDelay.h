#pragma once

/**
Readhead for the delay line that does allpass interpolation. Used by TapDelay.
*/
class ReadHead
{
   public:
   int sr;
   float y;
   float y1, y2; //values at point 1 and point 2
   int x1, x2; //point 1 and point 2
   float chorusDepth;
   float width;
   float delta;
   float a;
   float final_read;
   ReadHead(int sampleRate, float newwidth);
   ~ReadHead();

   /**
   Fast float to int casting with bit shifting.
   */
   int FloatToInt(float x);

   /**
   Set chorus width.
   */
   void setWidth(float newwidth);

   /**
   Reset sample rate.
   */
   void reset(int sampleRate, float newwidth);
   
   /**
   Main process function.
   */
   inline float ReadHead::process(int read, float *buffer1)
   {
      //get delayed value from the read head
      final_read = (float)read-(float)width;
      if(final_read>=sr*16)
         final_read = final_read-sr*16;
      if(final_read<0)
         final_read = sr*16 + final_read;
      
      //index before x
      x1 = FloatToInt(final_read);
      //index after x, wrapped around in case it's the last one
      x2 = FloatToInt(final_read+1);
      
      if(x2>=sr*16)
         x2 = 0;
      
      y1 = buffer1[x1];
      y2 = buffer1[x2];
      
      delta = (float)x2-(float)final_read;
      if(x2==0)
         delta = sr-final_read;
      
      a = (1-delta)/(1+delta);
      y = y1*a + y2- y*a;
      
      return y;
   }
};

/**
Delay line.
*/
class TapDelay
{
   public:
   //audio buffer for the delay line
   float *buffer1;
   float d; //delay time in fractional samples
   //read, write heads
   float read;
   float write;
   float width;
   float out;
   float az;
   float sr;
   float bloom;
   //parameter smoothing variables
   float a, b, z;
   //the read head.
   ReadHead *readHead;

   /**
   Fast float to int casting with bit shifting.
   */
   int FloatToInt(float x);
   
   /**
   Main process function.
   */
   inline float TapDelay::Process(float in)
   {
      //increment read write heads
      //these are essentially integers, and increment
      //at one sample per sample, normally
      read++;
      write++;
      
      //wraparound
      if(read==sr*16)
         read = 0;
      
      //wraparound
      if(write==sr*16)
         write = 0;
      
      //write input value to the buffer
      buffer1[FloatToInt(write)] = in;
      
      az =  readHead->process(FloatToInt(read),buffer1);
      out = az;
      return out;
   }

   void setDelay(float value);
   void setBloom(float newbloom);
   void reset(int sampleRate, float delay);
   TapDelay(float sampleRate, float newwidth);
   ~TapDelay();
};