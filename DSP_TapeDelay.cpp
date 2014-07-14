#pragma once
#include "DSP_TapeDelay.h"

/* This is the class for the lower delay rack, the distorting tape delay with fine delay time control. The bottomDelay_feedback goes
way over 1.0 (that corresponds to roughly half the knob range). At higher values the sound will get loud enough to
saturate in the sinusoidal waveshaper, so it will distort but will never 'explode', i.e. the values cannot get infinitely
large.

by Bogdan Vera
*/

/**
Sinusoidal waveshaping.
*/
float TapeDelay::Saturate(float in)
{
   return sinf(in);
}

/**
Kill denormal numbers.
*/
void TapeDelay::killDenormal (double &val)
{
   if(val <1.0E-30 && val> -1.0E-30)
   val = 0;
}

/**
Exponential scaling.
*/
float TapeDelay::getExpo(float in)
{
   return (exp(log(20.0f) + in * (log(20000.0f) - log(20.0f)))) /20000.0f; 
}

TapeDelay::TapeDelay(float sampleRate)
{
   o1 = o2 = 0.0f;
   SampDelay1  = 0.0f;
   
   srCompensate = compensated_a = 0.0;
   onePoleLP = new OnePoleLP();
   onePoleHP = new OnePoleHP();
   FLT = new Sinosc(sampleRate,5000.0f);
   LFO = new Sinosc(44100,0.0);
   LFOout = 0.0f;
   p_flti=0.0f;
   p_fltr=5000.0f;
   p_lfoi=0.0f;
   p_lfor=2000.0f;
   	
   outputs = new float[2];
   outputs[0] =0.0;
   outputs[1] = 0.0f;
   p_pong = false;
   
   a1 = aLFO = 0.9995f;
   b1 = bLFO =1.f-a1;
   z1 = zLFO = 0;
   p_Lp1Cf = p_Hp1Cf = p_Feedback1 = p_ms1 = 0.0;
   ms1 = ms2 = 0;
   out1 = 0.0f;
   out2 = 0.0f;
   sr = sampleRate;

   tDelay1 = new TapDelay(sampleRate,10);
   tDelay2 = new TapDelay(sampleRate,10);
}

/**
Reset samplerate.
*/
void TapeDelay::reset(float sampleRate)
{
	
   onePoleLP->reset(sampleRate);
   onePoleHP->reset(sampleRate);
   onePoleLP->setCutoff( p_Lp1Cf*20000.0f);
   onePoleHP->setCutoff(getExpo(p_Hp1Cf)*20000.0f);
   
   sr = sampleRate;
   FLT->reset(sampleRate);
   LFO->reset(sampleRate);

   out1 = 0.0f;
   out2 = 0.0f;
   
   float SampDelay = sr/1000.0f * ms1;
   
   tDelay1->reset(sr, SampDelay);
   tDelay2->reset(sr, SampDelay);
   //used to make the delay time consistent between sample rates
   srCompensate = sr/44100.0f;
   srCompensate = 1.0f/srCompensate;
   //used to calculate delay time 
   sr_over_1000 = sampleRate/1000.0f;

}

//destroy the objects
TapeDelay::~TapeDelay()
{
	delete onePoleHP;
	delete onePoleLP;
	delete FLT;
	delete LFO;
	delete outputs;
	delete tDelay1;
	delete tDelay2;
	onePoleHP = 0;
	onePoleLP = 0;
	FLT = 0;
	LFO = 0;
	outputs = 0;
	tDelay1 = 0;
	tDelay2 = 0;
}
	
void TapeDelay::setLPCutoff(float cutoff)
{
   p_Lp1Cf = cutoff;

   onePoleLP->setCutoff((p_Lp1Cf)*20000.0f);
}

void TapeDelay::setHPCutoff(float cutoff)
{
   p_Hp1Cf = cutoff;
  
   onePoleHP->setCutoff(getExpo(p_Hp1Cf)*20000.0f);
}

void TapeDelay::setDelayMS(float ms)
{
   p_ms1 = ms;
   p_ms2 = ms;
   this->ms1 = ms;		
   this->ms2 = ms;
}

void TapeDelay::setFltIntensity(float newi)
{
   p_flti = newi;
}
void TapeDelay::setFltRate(float newr)
{
   p_fltr = newr;
   FLT->setFrequency(p_fltr);
}

void TapeDelay::setLfoIntensity(float newi)
{
   p_lfoi = newi;
}
void TapeDelay::setLfoRate(float newr)
{
   p_lfor = newr;
   LFO->setFrequency(p_lfor);
}

void TapeDelay::setFeedback(float fb)
{
   p_Feedback1 = fb;
}

void TapeDelay::setSmooth(float smth)
{
   a1 = smth;
   b1 = 1.f-a1;
}

	