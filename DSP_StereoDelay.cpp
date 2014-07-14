#include "DSP_StereoDelay.h"

float StereoDelay::getExpo(float in)
{
   return (exp(log(1.0f) + in * (log(20000.0f) - log(1.0f)))) /20000.0f; 
}

/**
Kill denormal numbers.
*/
void StereoDelay::killDenormal (float &val)
{
   if(val <1.0E-30 && val> -1.0E-30)
      val = 0;
}

StereoDelay::StereoDelay(float sampleRate)
{
   o1 = o2 = 0.0f;
   SampDelay1 = SampDelay2 = 0.0f;
   
   //used to make the delay time consistent between sample rates
   srCompensate = compensated_a = 0.0;
   onePoleLP1 = new OnePoleLP();
   onePoleLP2 = new OnePoleLP();
   onePoleHP1 = new OnePoleHP();
   onePoleHP2 = new OnePoleHP();
   topDelay_pan1 = 0.0f;
   topDelay_pan2 = 1.0f;
   topDelay_level1=topDelay_level2=0.5f;
	
   outputs = new float[2];
   outputs[0] = 0.0;
   outputs[1] = 0.0f;
	
   a1  =  a2 = 0.9995f;
   b1  =  b2 = 1.f-a1;
   z1  =  z2 = 0;
   p_Lp1Cf = p_Hp1Cf = p_Lp2Cf = p_Hp2Cf = p_Feedback1 =  p_Feedback2 = p_ms1 = p_ms2 = 0.0;
   ms1 = ms2 = 0;
   out1 = 0.0f;
   out2 = 0.0f;
   sr = sampleRate;

   tDelay1 = new TapDelay(sampleRate,10);
   tDelay2 = new TapDelay(sampleRate,10);
 
}

/**
Reset sample rate.
*/
void StereoDelay::reset(float sampleRate)
{
   sr = sampleRate;
   onePoleLP1->reset(sr);
   onePoleLP2->reset(sr);
   onePoleHP1->reset(sr);
   onePoleHP2->reset(sr);
   
   onePoleLP1->setCutoff(getExpo(p_Lp1Cf) * 22050.0f );
   onePoleLP2->setCutoff(getExpo(p_Lp2Cf) * 22050.0f);
   onePoleHP1->setCutoff(getExpo(p_Hp1Cf) * 22050.0f);
   onePoleHP2->setCutoff(getExpo(p_Hp2Cf) * 22050.0f);

   out1 = 0.0f;
   out2 = 0.0f;
   
   
   float SampDelay1 = sr/1000 * ms1;
   float SampDelay2 = sr/1000 * ms2;
   
   tDelay1->reset(sr, SampDelay1);
   tDelay2->reset(sr, SampDelay2);
   
   //used to make the delay time consistent between sample rates
   srCompensate = sr/44100.0f;
   srCompensate = 1.0f/srCompensate;
   //used to calculate delay time 
   sr_over_1000 = sampleRate/1000.0f;
	
}

//destroy the objects
StereoDelay::~StereoDelay()
{
	delete onePoleLP1;
	delete onePoleLP2;
	delete onePoleHP1;
	delete onePoleHP2;
	delete tDelay1;
	delete tDelay2;
	delete outputs;

	onePoleLP1 = 0;
	onePoleLP2 = 0;
	onePoleHP1 = 0;
	onePoleHP2 = 0;
	tDelay1 = 0;
	tDelay2 = 0;
	outputs = 0;
}


void StereoDelay::setLP1Cutoff(float cutoff)
{
   p_Lp1Cf = cutoff;
   
   onePoleLP1->setCutoff(getExpo(p_Lp1Cf)* 22050.0f );
}
void StereoDelay::setHP1Cutoff(float cutoff)
{
   p_Hp1Cf = cutoff;
   
   onePoleHP1->setCutoff(getExpo(cutoff) * 22050.0f);
}

void StereoDelay::setLP2Cutoff(float cutoff)
{
   p_Lp2Cf = cutoff;
   
   onePoleLP2->setCutoff(getExpo(p_Lp2Cf) * 22050.0f);
}
void StereoDelay::setHP2Cutoff(float cutoff)
{
   p_Hp2Cf = cutoff;
   
   onePoleHP2->setCutoff(getExpo(cutoff) * 22050.0f);
}

void StereoDelay::setDelayMS1(float ms)
{
   p_ms1 = ms;
   this->ms1 = ms;		
}
void StereoDelay::setDelayMS2(float ms)
{
   p_ms2 = ms;
   this->ms2 = ms;		
}


void StereoDelay::setFeedback1(float fb)
{
   p_Feedback1 = fb;
}

void StereoDelay::setFeedback2(float fb)
{
   p_Feedback2 = fb;
}

void StereoDelay::setPan1(float newpan)
{
   topDelay_pan1 = newpan;
}

void StereoDelay::setPan2(float newpan)
{
   topDelay_pan2 = newpan;
}

void StereoDelay::setLevel1(float lvl)
{
   topDelay_level1 = lvl;
}

void StereoDelay::setLevel2(float lvl)
{
   topDelay_level2 = lvl;
}

	

