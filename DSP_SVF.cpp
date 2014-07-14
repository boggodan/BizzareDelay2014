#include "DSP_SVF.h"

StateVariableFilter::StateVariableFilter(float sampleRate, bool type)
{
   this->type = type;
   f = 0;
   q = 0;
   LP=HP=BP=notch= 0;
   scale = 0;
   sr = sampleRate;
}

StateVariableFilter::~StateVariableFilter()
{

}

void StateVariableFilter::killDenormal (float &val)
{
   if(val <1.0E-30 && val> -1.0E-30)
      val = 0;
}

void StateVariableFilter::SetCutoff(float cutoff)
{
   //sample rate fraction factor
   float sr_factor;
   sr_factor = sr/44100.0f;
   //set f and account for sample rate variation
   f = 2 * sin(3.14 * (cutoff / (6.0f * sr_factor)));
}

void StateVariableFilter::SetResonance(float res)
{
   q = 1 - res;
   scale = sqrtf(q);
}

void StateVariableFilter::reset(float sampleRate)
{
   f = 0;
   q = 0;
   LP=HP=BP=notch= 0;
   scale = 0;
   sr = sampleRate;
}

inline float StateVariableFilter::Process(float input)
{
   LP = LP + f * BP + 1.0E-25;
   HP = scale * input - LP - q*BP + 1.0E-25;
   BP = f * HP + BP + 1.0E-25;
   
   if (type==0)
   return LP;
   if (type==1)
   return HP;
}
	
