#pragma once
#include "DSP_TappedDelay.h"
#include "DSP_OnepoleLP.h"
#include "DSP_Sinosc.h"
#include "DSP_OnePoleHP.h"

/**
 * This is the class for the topmost rack, the stereo delay. It's fairly straightforward and uses two delay lines in parallel with
independent controls for delay time (in tempo topDelay_tempoNote), filters, gains, etc. The bottomDelay_feedback goes up to 1.0 so there is no chance for 
the echoes to grow in loudness, they will at most remiain the same level
 */
class StereoDelay
{

   public:
   //is pong on?
   bool p_pong;
   //sample rate
   float sr;

   /*LEFT DELAY*/
   
   //Delay Lines
   TapDelay* tDelay1;    
   TapDelay* tDelay2;  
   
   //Filters
   OnePoleLP *onePoleLP1;
   OnePoleLP *onePoleLP2;
   OnePoleHP *onePoleHP1;
   OnePoleHP *onePoleHP2;
   
   //Output values
   float out1;
   float out2;
   
   //persistent parameter values
   float p_Lp1Cf, p_Hp1Cf;
   float p_ms1;
   float p_Feedback1;

   //current/smoothed
   float ms1;

   //delay line smoothing
   float a1,b1,z1;
   
   /*RIGHT DELAY*/
   //smoothing for the LFO
   	
   //persistent parameter values
   float p_Lp2Cf, p_Hp2Cf;
   float p_ms2;
   float p_Feedback2;
   //current/smoothed
   float ms2;
   //delay line smoothing
   float a2,b2,z2;

   //array of output samples (process returns two samples)
   float* outputs;
   
   float topDelay_pan1,topDelay_pan2;
   float topDelay_level1,topDelay_level2;
   float srCompensate;
   float compensated_a;
   
   //incidental variables
   float sr_over_1000;
   float SampDelay1,SampDelay2;
   float o1,o2;
   float getExpo(float in);
   
   /**
   Kill denormal numbers.
   */
   void killDenormal (float &val);
   StereoDelay(float sampleRate);

   /**
   Reset sample rate.
   */
   void reset(float sampleRate);
   ~StereoDelay();
   
   /**
   Main process function.
   */
   inline float* process(float in)
   {
      //must compensate the smoothing filter coeffs for sample rate change
      //TODO: this is really slow, could use a better formula with no powf
      compensated_a = powf(a1, srCompensate);
      
      z1 = (ms1 * (1.0f-compensated_a)) + (z1*compensated_a);
      z2 = (ms2 * (1.0f-compensated_a)) + (z2*compensated_a);
      
      
      tDelay1->setDelay(sr_over_1000 * z1);
      tDelay2->setDelay(sr_over_1000 * z2);
      
      out1 = tDelay1->Process((in + out1 )* p_Feedback1);
      out2 = tDelay2->Process((in + out2 )* p_Feedback2);
      
      if(p_Hp1Cf>0.0)
      	out1 = onePoleHP1->process(out1);
      if(p_Hp2Cf>0.0)
      	out2 = onePoleHP2->process(out2);
      if(p_Lp1Cf<1.0)
      	out1 = onePoleLP1->process(out1);
      if(p_Lp2Cf<1.0)
      	out2 = onePoleLP2->process(out2);
      
      
      killDenormal(out1);
      killDenormal(out2);
      
      o1 = out1 * (1.0f-topDelay_pan1) *topDelay_level1 + out2 * (1.0f-topDelay_pan2) *topDelay_level2;
      o2 = out1 * (topDelay_pan1)* topDelay_level1 + out2 * (topDelay_pan2) * topDelay_level2;

      outputs[0] = o1;
      outputs[1] = o2;
      
      return outputs;
   }
   
   void setLP1Cutoff(float cutoff);
   void setHP1Cutoff(float cutoff);
   void setLP2Cutoff(float cutoff);
   void setHP2Cutoff(float cutoff);
   void setDelayMS1(float ms);
   void setDelayMS2(float ms);
   void setFeedback1(float fb);
   void setFeedback2(float fb);
   void setPan1(float newpan);
   void setPan2(float newpan);
   void setLevel1(float lvl);
   void setLevel2(float lvl);

};