#include "DSP_TappedDelay.h"
#include "DSP_SVF.h"
#include "DSP_Sinosc.h"
#include "DSP_OnePoleHP.h"
#include "DSP_OnePoleLP.h"



/**
This is the class for the lower delay rack, the distorting tape delay with fine delay time control. The bottomDelay_feedback goes
way over 1.0 (that corresponds to roughly half the knob range). At higher values the sound will get loud enough to
saturate in the sinusoidal waveshaper, so it will distort but will never 'explode', i.e. the values cannot get infinitely
large.
*/
class TapeDelay
{
   public:
   bool p_pong;
   float sr;
   
   /*LEFT DELAY*/
   TapDelay* tDelay1;    
   OnePoleLP *onePoleLP;
   OnePoleHP *onePoleHP;
   
   double out1;
   //persistent
   float p_Lp1Cf, p_Hp1Cf;
   float p_ms1;
   float p_Feedback1;
   //current/smoothed
   float ms1;
   //delay line smoothing
   float a1,b1,z1;
   /*RIGHT DELAY*/
   //smoothing for the LFO
   float aLFO,bLFO,zLFO;
   //flo intensity and rate
   float p_lfoi,p_lfor;
   StateVariableFilter* Lp2;
   StateVariableFilter* Hp2;
   TapDelay* tDelay2;    
   
   double out2;
   //persistent
   float p_Lp2Cf, p_Hp2Cf;
   float p_ms2;
   float p_Feedback2;
   //current/smoothed
   float ms2;
   //delay line smoothing
   float a2,b2,z2;
   float p_flti,p_fltr;
   float* outputs;
   
   float LFOout;
   Sinosc *FLT;
   Sinosc *LFO;
   
   //incidental stuff
   float sr_over_1000;
   float SampDelay1,SampDelay2;
   float o1,o2;
   float srCompensate;
   float compensated_a;
   
   /**
   Sinusoidal waveshaping.
   */
   float Saturate(float in);
   /**
   Kill denormal numbers.
   */
   void killDenormal (double &val);
   /**
   Exponential scaling.
   */
   float getExpo(float in);
   TapeDelay(float sampleRate);
   /**
   Reset samplerate.
   */
   void reset(float sampleRate);
   ~TapeDelay();
   
   /**
   Main process function.
   */
   inline float* TapeDelay::process(float in)
   {
      //bottomDelay_smooth delay time
      //must compensate this factor for sample rate change
      
      compensated_a = powf(a1, srCompensate);
      
      z1 = (ms1 * (1.0f - compensated_a)) + (z1*compensated_a);
      LFOout = LFO->getSample();
      zLFO = (p_lfoi * bLFO)+(zLFO*aLFO);
      
      SampDelay1 = sr_over_1000 * z1;
      tDelay1->setDelay(SampDelay1 + FLT->getSample()*10.0*p_flti + LFOout*20.0*zLFO);
      tDelay2->setDelay(SampDelay1 + FLT->getSample()*10.0*p_flti + LFOout*20.0*zLFO);
      
      double dummy = 0.0;
      
      if(p_pong==false)
      {
         out1 = tDelay1->Process((in + out1 )* p_Feedback1);
         out1 = onePoleHP->process(onePoleLP->process(out1));
         out1 = Saturate(out1);
         out2 = out1;
      }
      else
      {
         out1 = tDelay1->Process((in + out2 )* p_Feedback1);
         out2 = tDelay2->Process(out1);
         out2 = onePoleHP->process(onePoleLP->process(out2));
         out1 = Saturate(out1*0.95);
         out2 = Saturate(out2*0.95);
      }
      
      killDenormal(out1);
      killDenormal(out2);
      
      o1 = out1;
      o2 = out2;
      
      outputs[0] = o1;
      outputs[1] = o2;
      
      return outputs;
   }
   
   void setLPCutoff(float cutoff);
   void setHPCutoff(float cutoff);
   void setDelayMS(float ms);
   void setFltIntensity(float newi);
   void setFltRate(float newr);
   void setLfoIntensity(float newi);
   void setLfoRate(float newr);
   void setFeedback(float fb);
   void setSmooth(float smth);
   
};