#include "BizzareDelay2014.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

/*
BizzareDelay2014 - VST, AU

Developed for Bizzare Contact by 1st Creative Solutions
© 2011 1st Creative Solutions LTD

Code: Bogdan Vera.
Graphical Design: Matt O'Neill
*/

const int kNumPrograms = 3;

//enumeration of the param numbers
enum EParams
{
   k2hicut = 0,
   k2lowcut = 1,
   k2msDelay1 = 2,
   k2feedback = 3,
   k2pong = 4,
   k2note = 5,
   k2fine = 6,
   k2LFOintensity = 7,
   k2LFOrate = 8,
   k2FltInt = 9,
   k2FltRate = 10,
   k2Smooth = 11,
   k2mix = 12,
   kswitch = 13,
   kunits =14, 
   kwetdry1 = 15,
   kLP1 = 16,
   kHP1 = 17,
   kPan1 = 18,
   kDelay1 = 19,
   kFeedback1 = 20,
   kLevel1= 21,
   kLP2 = 22,
   kHP2 = 23,
   kPan2 = 24,
   kDelay2 = 25,
   kFeedback2 = 26,
   kLevel2 = 27,
   kNumParams = 28
 
};

enum ELayout
{
   kWidth = GUI_WIDTH,
   kHeight = GUI_HEIGHT,
};

BizzareDelay2014::BizzareDelay2014(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
   TRACE;

   

   //init the main delay objects
   tapeDelay1 = new TapeDelay(44100);
   stereoDelay = new StereoDelay(44100);

    

   //Initial mixing levels (will be reset by preset loading anyway).
   dry1 = dry2 =  0.0f;
   wet1 = wet2 = 0.0f;

   //Used for tempo syncing.
   tempoUnit = 1.0/5.0;

   bottomDelay_smooth = 0.9995;
   bottomDelay_lfoRate = bottomDelay_lfoIntensity = 0.0f;
   bottomDelay_fltIntensity = 0.0;
   bottomDelay_fltRate = 0.0f;
   bottomDelay_fine = 0.2;
   bottomDelay_fudgedFine = 0.0;
   currentTempoUnit = 0.0;
   bottomDelay_tempoNote = 0.5;
   bottomDelay_lpCut = 0.0f;
   bottomDelay_hpCut = 1.0f;
   bottomDelay_mix = 0.5f;
   bottomDelay_feedback = 0.5;
   rackSwitch = 1.0f;
   beatsPerMinute = 0.0f;

   topDelay_tempoNote= 0.0f;
   topDelay_wetDry= 0.0f;
   topDelay_lp1Cutoff = topDelay_lp2Cutoff= 0.0f;
   topDelay_hp1Cutoff = topDelay_hp2Cutoff= 0.0f;
   topDelay_pan1 = topDelay_pan2= 0.0f;
   topDelay_delay1 = topDelay_delay2= 0.0f;
   topDelay_fb1 = topDelay_fb2= 0.0f;
   topDelay_level1 = topDelay_level2= 0.0f;
   onOffPong = 0.0;

   //arguments are: name, defaultVal, minVal, maxVal, step, label
   GetParam(k2hicut)->InitDouble("2hicut", 50.0, 0., 100.0, 0.01, "%");
   GetParam(k2lowcut)->InitDouble("2lowcut", 50., 0., 100.0, 0.01, "%");
   GetParam(k2msDelay1)->InitDouble("2msDelay1", 50., 0., 100.0, 0.01, "%");
   GetParam(k2feedback)->InitDouble("2feedback", 50., 0., 100.0, 0.01, "%");
   GetParam(k2pong)->InitDouble("2pong", 50., 0., 100.0, 0.01, "%");
   GetParam(k2note)->InitDouble("2note", 50., 0., 100.0, 0.01, "%");
   GetParam(k2fine)->InitDouble("2fine", 50., 0., 100.0, 0.01, "%");
   GetParam(k2LFOintensity)->InitDouble("2LFO intensity", 50., 0., 100.0, 0.01, "%");
   GetParam(k2LFOrate)->InitDouble("2LFO rate", 50., 0., 100.0, 0.01, "%");
   GetParam(k2FltInt )->InitDouble("2Flt Int", 50., 0., 100.0, 0.01, "%");
   GetParam(k2FltRate)->InitDouble("2Flt Rate", 50., 0., 100.0, 0.01, "%");
   GetParam(k2Smooth)->InitDouble("2Smooth", 50., 0., 100.0, 0.01, "%");
   GetParam(k2mix)->InitDouble("2mix", 50., 0., 100.0, 0.01, "%");
   GetParam(kswitch)->InitDouble("switch", 50., 0., 100.0, 0.01, "%");
   GetParam(kunits)->InitDouble("topDelay_tempoNote", 50., 0., 100.0, 0.01, "%");
   GetParam(kwetdry1)->InitDouble("topDelay_wetDry", 50., 0., 100.0, 0.01, "%");
   GetParam(kLP1)->InitDouble("LP1", 50., 0., 100.0, 0.01, "%");
   GetParam(kHP1)->InitDouble("HP1", 50., 0., 100.0, 0.01, "%");
   GetParam(kPan1)->InitDouble("Pan1", 50., 0., 100.0, 0.01, "%");
   GetParam(kDelay1)->InitDouble("Delay1", 50., 0., 100.0, 0.01, "%");
   GetParam(kFeedback1)->InitDouble("Feedback1", 50., 0., 100.0, 0.01, "%");
   GetParam(kLevel1)->InitDouble("Level1", 50., 0., 100.0, 0.01, "%");
   GetParam(kLP2)->InitDouble("LP2", 50., 0., 100.0, 0.01, "%");
   GetParam(kHP2)->InitDouble("HP2", 50., 0., 100.0, 0.01, "%");
   GetParam(kPan2)->InitDouble("Pan2", 50., 0., 100.0, 0.01, "%");
   GetParam(kDelay2)->InitDouble("Delay2", 50., 0., 100.0, 0.01, "%");
   GetParam(kFeedback2)->InitDouble("Feedback2", 50., 0., 100.0, 0.01, "%");
   GetParam(kLevel2)->InitDouble("Level2", 50., 0., 100.0, 0.01, "%");
   
    

   IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
   
   

   pGraphics->AttachBackground(BG_ID,BG_FN);
   
   

   IBitmap knob_top_big = pGraphics->LoadIBitmap(KNOB_TOP1, KNOB_TOP1_FN, 60);
   IBitmap knob_top_small = pGraphics->LoadIBitmap(KNOB_TOP2, KNOB_TOP2_FN, 60);
   IBitmap knob_bottom_big = pGraphics->LoadIBitmap(KNOB_BTM1, KNOB_BTM1_FN, 60);
   IBitmap knob_bottom_small = pGraphics->LoadIBitmap(KNOB_BTM2, KNOB_BTM2_FN, 60);
   IBitmap slider_body = pGraphics->LoadIBitmap(SLIDER_BODY,SLIDER_BODY_FN, 1);
   IBitmap slider_knob = pGraphics->LoadIBitmap(SLIDER_KNOB, SLIDER_KNOB_FN, 1);
   IBitmap switcher_button = pGraphics->LoadIBitmap(SWITCHER_BUTTON, SWITCHER_BUTTON_FN, 2);
   IBitmap pong_button = pGraphics->LoadIBitmap(PONG_BTN, PONG_BTN_FN, 2);
   
 

   IText topText = IText(11);
   IText bottomText = IText(11);
   
   IColor topColor =  IColor(255,153,153,255);
   
   IColor bottomColor  = IColor(255,255,255,0);
   
   topText.mColor= topColor;
   bottomText.mColor= bottomColor;
   //bottom
   pGraphics->AttachControl(new IKnobMultiControl(this, 14, 289,   k2fine, &knob_bottom_big, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(k2fineDisplay = new ITextControl(this, IRECT(65, 304, 100,317), &bottomText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 135, 289, k2feedback, &knob_bottom_big, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(k2feedbackDisplay = new ITextControl(this, IRECT(184, 303, 219,316), &bottomText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 14, 353,   k2note, &knob_bottom_big, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(k2noteDisplay = new ITextControl(this, IRECT(65, 366, 100,379), &bottomText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 136, 353, k2mix, &knob_bottom_big, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(k2mixDisplay = new ITextControl(this, IRECT(184, 366, 219,379), &bottomText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 379, 287, k2FltRate, &knob_bottom_big, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(k2FltRateDisplay = new ITextControl(this, IRECT(423, 301, 460,314), &bottomText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 379, 353, k2FltInt, &knob_bottom_big, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(k2FltIntDisplay = new ITextControl(this, IRECT(425, 366, 460,379), &bottomText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 489, 287, k2LFOrate, &knob_bottom_big, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(k2LFOrateDisplay = new ITextControl(this, IRECT(535, 300, 570,313), &bottomText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 489, 353, k2LFOintensity, &knob_bottom_big, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(k2LFOintensityDisplay = new ITextControl(this, IRECT(535, 366, 570,379), &bottomText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 283, 260, k2hicut, &knob_bottom_big, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(k2hicutDisplay = new ITextControl(this, IRECT(284, 305, 319,318), &bottomText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 283, 350, k2lowcut, &knob_bottom_big, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(k2lowcutDisplay = new ITextControl(this, IRECT(283, 395, 318,408), &bottomText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 113, 401, k2Smooth, &knob_bottom_small, kVertical, DEFAULT_GEARING));
   
   //top
   pGraphics->AttachControl(new IKnobMultiControl(this, 43, 70, kunits, &knob_top_big, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(kunitsDisplay = new ITextControl(this, IRECT(44, 117, 79,130), &topText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 123, 70, kwetdry1, &knob_top_big, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(kwetdry1Display = new ITextControl(this, IRECT(124, 117, 159,130), &topText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 218, 30, kLP1, &knob_top_small, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(kLP1Display = new ITextControl(this, IRECT(209, 57, 244,70), &topText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 218, 91, kHP1, &knob_top_small, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(kHP1Display = new ITextControl(this, IRECT(208, 120, 244,133), &topText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 218, 154, kPan1, &knob_top_small, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(kPan1Display = new ITextControl(this, IRECT(208, 183, 245,196), &topText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 392, 30, kLP2, &knob_top_small, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(kLP2Display = new ITextControl(this, IRECT(383, 57, 418,70), &topText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 392, 91, kHP2, &knob_top_small, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(kHP2Display = new ITextControl(this, IRECT(383, 120, 418,133), &topText, " aaaa "));
   
   pGraphics->AttachControl(new IKnobMultiControl(this, 392, 154, kPan2, &knob_top_small, kVertical, DEFAULT_GEARING));
   pGraphics->AttachControl(kPan2Display = new ITextControl(this, IRECT(383, 183, 418,196), &topText, " aaaa "));
   
   //sliders
   pGraphics->AttachControl(new IFaderControl(this, 267, 52, 123, kDelay1, &slider_knob, kVertical)); 
   pGraphics->AttachControl(kDelay1Display = new ITextControl(this, IRECT(265, 183, 286,196), &topText, " aaaa "));
   
   pGraphics->AttachControl(new IFaderControl(this, 300,52, 123, kFeedback1, &slider_knob, kVertical)); 
   pGraphics->AttachControl(kFeedback1Display = new ITextControl(this, IRECT(299, 183, 320,196), &topText, " aaaa "));
   
   pGraphics->AttachControl(new IFaderControl(this, 333,52, 123, kLevel1, &slider_knob, kVertical)); 
   pGraphics->AttachControl(kLevel1Display = new ITextControl(this, IRECT(333, 183, 354,196), &topText, " aaaa "));
   
   pGraphics->AttachControl(new IFaderControl(this, 440, 52, 123, kDelay2, &slider_knob, kVertical)); 
   pGraphics->AttachControl(kDelay2Display = new ITextControl(this, IRECT(439, 183, 460,196), &topText, " aaaa "));
   
   pGraphics->AttachControl(new IFaderControl(this, 474,52, 123, kFeedback2, &slider_knob, kVertical)); 
   pGraphics->AttachControl(kFeedback2Display = new ITextControl(this, IRECT(473, 183, 494,196), &topText, " aaaa "));
   
   pGraphics->AttachControl(new IFaderControl(this, 508,52, 123, kLevel2, &slider_knob, kVertical)); 
   pGraphics->AttachControl(kLevel2Display = new ITextControl(this, IRECT(507, 183, 528,196), &topText, " aaaa "));
   
   pGraphics->AttachControl(new ISwitchControl(this, 390, 402, k2pong, &pong_button, IChannelBlend::kBlendNone));
   pGraphics->AttachControl(new ISwitchControl(this, 543,83, kswitch, &switcher_button, IChannelBlend::kBlendNone));
  
   AttachGraphics(pGraphics);
 
   //Default presets

   MakePresetFromNamedParams(  (char*) "Bizzare Delay", 27, 
		kunits, 24.29,
		kwetdry1, 47.0,
		kLP1, 100.0,
		kHP1, 38.61, 
		kPan1, 0.0,
		kDelay1, 16.36,
		kFeedback1, 75.0,
		kLevel1, 47.0,
		kLP2, 100.0,
		kHP2, 38.61,
		kPan2, 100.0,
		kDelay2, 27.27,
		kFeedback2,75.0,
		kLevel2, 47.0,
		k2fine, 40.13,
		k2feedback, 15.0,
		k2note, 65.66,
		k2mix, 50.0,
		k2hicut, 100.0,
		k2lowcut, 21.68,
		k2FltRate, 0.0,
		k2FltInt, 0.0,
		k2LFOintensity, 0.0,
		k2LFOrate, 0.0,
		kswitch, 100.0,
		k2Smooth, 39.47,
		k2pong, 100.0);

   MakePresetFromNamedParams(  (char*) "Madcap", 27, 
		kunits, 24.29,
		kwetdry1, 47.0,
		kLP1, 100.0,
		kHP1, 38.61, 
		kPan1, 0.0,
		kDelay1, 16.36,
		kFeedback1, 100.0,
		kLevel1, 47.0,
		kLP2, 100.0,
		kHP2, 38.61,
		kPan2, 100.0,
		kDelay2, 27.27,
		kFeedback2,100.0,
		kLevel2, 47.0,
		k2fine, 40.13,
		k2feedback, 69.0,
		k2note, 65.66,
		k2mix, 50.0,
		k2hicut, 100.0,
		k2lowcut, 21.68,
		k2FltRate, 0.0,
		k2FltInt, 0.0,
		k2LFOintensity, 0.0,
		k2LFOrate, 0.0,
		kswitch, 0.0,
		k2Smooth, 39.47,
		k2pong, 100.0);

   MakePresetFromNamedParams(  (char*) "Dissipate", 27, 
		kunits, 100.0,
		kwetdry1, 47.0,
		kLP1, 81.0,
		kHP1, 38.61, 
		kPan1, 0.0,
		kDelay1, 20.0,
		kFeedback1, 100.0,
		kLevel1, 75.0,
		kLP2, 81.0,
		kHP2, 38.61,
		kPan2, 100.0,
		kDelay2, 20.0,
		kFeedback2,100.0,
		kLevel2, 75.0,
		k2fine, 40.13,
		k2feedback, 67.0,
		k2note, 100.0,
		k2mix, 50.0,
		k2hicut, 100.0,
		k2lowcut, 30.0,
		k2FltRate, 0.0,
		k2FltInt, 0.0,
		k2LFOintensity, 100.0,
		k2LFOrate, 16.5,
		kswitch, 0.0,
		k2Smooth, 39.47,
		k2pong, 100.0);

   //Load the first preset on startup.
   RestorePreset(0);
   tempo = 1.0;
}

BizzareDelay2014::~BizzareDelay2014() 
{
	delete stereoDelay;
	stereoDelay = 0;
	delete tapeDelay1;
	tapeDelay1 = 0;
}

void BizzareDelay2014::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

   double* in1 = inputs[0];
   double* in2 = inputs[1];
   double* out1 = outputs[0];
   double* out2 = outputs[1];
   
   double tempo = GetTempo();



   /* Why are we doing this next part here? I couldn't find a function that informs the plug-in
   directly about host tempo changing, so here we need to check if the host tempo is changing
   to set the right tempo values.  Maybe there is a better way. */

   /*The strange hack requested by Bizzare that snaps to 1.0 fine delay smoothly*/

   bottomDelay_fudgedFine = 0.66666666667 + bottomDelay_fine * 0.83333333334;
   float intUnits = (int)(topDelay_tempoNote*15) + 1;
   float intDelay1 = (int)(topDelay_delay1*15) + 1;
   float intDelay2 = (int)(topDelay_delay2*15) + 1;
   
   if((currentTempoUnit > 2 || tempo > 60))
   {
      float snapFine = bottomDelay_fudgedFine;
      if(snapFine>=0.99 && snapFine<=1.01)
      snapFine = 1.0;
      tapeDelay1->setDelayMS(((240000.0f * (1.0/currentTempoUnit)) * snapFine / tempo) );
   }

   //If the tempo is smaller than 60BPM don't even bother to set the delay time.
   //It would take up too much memory and is an unlikely usecase.
   if(tempo>60)
   {
      stereoDelay->setDelayMS1( ( 240000.0f * (1.0f/intUnits/4.0f)) * intDelay1 / tempo);
      stereoDelay->setDelayMS2( ( 240000.0f * (1.0f/intUnits/4.0f)) * intDelay2 / tempo);
   }

   //go through the samples and process them.
   for (int i = 0; i < nFrames; ++i, ++in1, ++in2, ++out1, ++out2)
   {
      inl = (*in1);
      inr = (*in2);
       
      //if the stereo delay is used, process with that object.
      if(rackSwitch>0.5)
      {
         outl = stereoDelay->process((inl + inr)/2);
         (*out1) = (outl[0]*wet1 + inl*dry1);
         (*out2) = (outl[1]*wet1 + inr*dry1);
      }

	  else //if the distorting delay is used, process with that object.
	  {
	     outl = tapeDelay1->process((inl + inr)/2);
         (*out1) = (outl[0]*wet2 + inl*dry2);
         (*out2) = (outl[1]*wet2 + inr*dry2);
	  }
   }
}

//When the plug-in resets, inform all the various audio objects about the sample rate.
void BizzareDelay2014::Reset()
{
   TRACE;
   IMutexLock lock(this);

   double sr = GetSampleRate();
   tapeDelay1->reset(sr);
   stereoDelay->reset(sr);

}

//Called when the user changes a parameter value.
void BizzareDelay2014::OnParamChange(int paramIdx)
{
   IMutexLock lock(this);
   char* writeToDisplay = new char[30];

   //Lowpass cutoff.
   if(paramIdx == EParams::k2hicut)
   {
      bottomDelay_lpCut = GetParam(paramIdx) -> Value() / 100.0;
      tapeDelay1->setLPCutoff(bottomDelay_lpCut);
      sprintf(writeToDisplay, "%4.2f", bottomDelay_lpCut *20.0);
      k2hicutDisplay->SetTextFromPlug(writeToDisplay);
   }
   else
   if(paramIdx == EParams::k2lowcut) //Highpass Cutoff
   {
      bottomDelay_hpCut = GetParam(paramIdx) -> Value() / 100.0;
      tapeDelay1->setHPCutoff(bottomDelay_hpCut);
      sprintf(writeToDisplay, "%4.2f", bottomDelay_hpCut *20.0);
      k2lowcutDisplay->SetTextFromPlug(writeToDisplay); 
   }
   else //OBSOLETE PARAMETER
   if(paramIdx == 2)
   {
      //OBSOLETE
   }
   else
   if(paramIdx == EParams::k2feedback) //Delay bottomDelay_feedback (for distorting delay)
   {
      bottomDelay_feedback = GetParam(paramIdx) -> Value() / 100.0;
      tapeDelay1->setFeedback(bottomDelay_feedback*1.6);
      sprintf(writeToDisplay, "%4.2f", bottomDelay_feedback *100.0);
      k2feedbackDisplay->SetTextFromPlug(writeToDisplay);
   }
   else
   if(paramIdx == EParams::k2pong) //Turn on or off the ping-pong function for the distorting delay.
   {
      onOffPong = GetParam(paramIdx) -> Value() / 100.0;
      if(onOffPong<0.5)
      tapeDelay1->p_pong=false;
      else
      tapeDelay1->p_pong=true;
   }
   else
   if(paramIdx == EParams::k2note) //Set the tempo-synced delay time tempoUnit.
   {
      bottomDelay_tempoNote = GetParam(paramIdx) -> Value() / 100.0;
   
      float tempoUnit = 1.0/5.0;
      float fin = 0;
   	
      if(bottomDelay_tempoNote<tempoUnit)
      {
         fin = 1;
		 currentTempoUnit = 1;
      }

      if(bottomDelay_tempoNote>=tempoUnit && bottomDelay_tempoNote<tempoUnit*2)
      {
         fin = 2;
		 currentTempoUnit = 2;
      }

      if(bottomDelay_tempoNote>=tempoUnit * 2 && bottomDelay_tempoNote<tempoUnit*3)
      {
         fin = 4;
		 currentTempoUnit = 4;
      }

      if(bottomDelay_tempoNote>=tempoUnit*3 && bottomDelay_tempoNote<=4)
      {
         fin = 8;
		 currentTempoUnit = 8;
      }

      if(bottomDelay_tempoNote>=tempoUnit*4 && bottomDelay_tempoNote<=1)
      {
         fin = 16;
		 currentTempoUnit = 16;
      }
   	
      sprintf(writeToDisplay, "%4.1f", fin);
      k2noteDisplay->SetTextFromPlug(writeToDisplay);
   }
   else
   if(paramIdx == EParams::k2fine) //Set the fine delay time value.
   {
   	
      bottomDelay_fine = GetParam(paramIdx) -> Value() / 100.0;
      sprintf(writeToDisplay, "%4.2f", (0.66666666667 + bottomDelay_fine * 0.83333333334)/2.0f);
      k2fineDisplay->SetTextFromPlug(writeToDisplay);
   	
   }
   else
   if(paramIdx == EParams::k2LFOintensity) //Set LFO intensity
   {
   	
      bottomDelay_lfoIntensity = GetParam(paramIdx) -> Value() / 100.0;;;
      tapeDelay1->setLfoIntensity(bottomDelay_lfoIntensity);
      sprintf(writeToDisplay, "%4.2f", bottomDelay_lfoIntensity);
      k2LFOintensityDisplay->SetTextFromPlug(writeToDisplay);	
   }
   else
   if(paramIdx == EParams::k2LFOrate) //Set the LFO rate
   {
      bottomDelay_lfoRate = GetParam(paramIdx) -> Value() / 100.0;;
      tapeDelay1->setLfoRate((bottomDelay_lfoRate*0.8f+0.2f)*20.0f);
      sprintf(writeToDisplay, "%4.2f", bottomDelay_lfoRate);
      k2LFOrateDisplay->SetTextFromPlug(writeToDisplay);
   }
   else
   if(paramIdx == EParams::k2FltInt) //Set the Flutter intensity.
   {
   	
      bottomDelay_fltIntensity = GetParam(paramIdx) -> Value() / 100.0;;
      tapeDelay1->setFltIntensity(bottomDelay_fltIntensity);
      sprintf(writeToDisplay, "%4.2f", bottomDelay_fltIntensity);
      k2FltIntDisplay->SetTextFromPlug(writeToDisplay);
   }
   else
   if(paramIdx == EParams::k2FltRate) //Set the flutter rate.
   {
      bottomDelay_fltRate= GetParam(paramIdx) -> Value() / 100.0;;
      tapeDelay1->setFltRate((bottomDelay_fltRate*0.2+0.8)*50);
      sprintf(writeToDisplay, "%4.2f", bottomDelay_fltRate);
      k2FltRateDisplay->SetTextFromPlug(writeToDisplay);
   }
   else
   if(paramIdx == EParams::k2Smooth) //Set the smoothing amount for the fine delay time knob.
   {
   	
      bottomDelay_smooth= GetParam(paramIdx) -> Value() / 100.0;;
      tapeDelay1->setSmooth(bottomDelay_smooth*0.0095 + 0.99);
   
   }else
   if(paramIdx == EParams::k2mix) //Set wet/dry value.
   {
      char *ptr;
      bottomDelay_mix = GetParam(paramIdx) -> Value() / 100.0;;
      if(bottomDelay_mix<0.5)
      {
         wet2 = bottomDelay_mix*2.0f;
         dry2 = 1.0f;
      }

      if(bottomDelay_mix>=0.5)
      {
         dry2 = 2.0f - ((bottomDelay_mix - 0.5f)*2.0f);
         dry2 = 1.0f - ((bottomDelay_mix -0.5f)*2.0f);
         wet2 = 1.0f;
      }
      sprintf(writeToDisplay, "%4.2f", bottomDelay_mix);
      k2mixDisplay->SetTextFromPlug(writeToDisplay);
   }else
   if(paramIdx == EParams::kswitch) //Switches between the two delay topDelay_tempoNote.
   {
      rackSwitch = GetParam(paramIdx) -> Value() / 100.0;
   }else
   
   //STEREO STARTS HERE
   //UNITS
   if(paramIdx == EParams::kunits) //Set the tempo-synced delay topDelay_tempoNote for the stereo delay.
   {
      topDelay_tempoNote = GetParam(paramIdx) -> Value() / 100.0;
      sprintf(writeToDisplay, "%4.1f", (int)(topDelay_tempoNote*15.0f)+1.0f);
      kunitsDisplay->SetTextFromPlug(writeToDisplay);
   }else
   if(paramIdx == EParams::kwetdry1) //Set wet/dry for the stereo delay.
   {
      char *ptr;
      
      topDelay_wetDry = GetParam(paramIdx) -> Value() / 100.0;
      if(topDelay_wetDry<0.5)
      {
         wet1 = topDelay_wetDry*2.0f;
         dry1 = 1.0f;
      }
      if(topDelay_wetDry>=0.5)
      {
         dry1 = 2.0f - ((topDelay_wetDry - 0.5f)*2.0f);
         dry1 = 1.0f - ((topDelay_wetDry -0.5f)*2.0f);
         wet1 = 1.0f;
      }
      
      sprintf(writeToDisplay, "%4.2f", topDelay_wetDry);
      kwetdry1Display->SetTextFromPlug(writeToDisplay);
   	
   }else
   /////////////////////////////
   if(paramIdx == EParams::kLP1) //Set LP1 cutoff
   {  
   	
      topDelay_lp1Cutoff = GetParam(paramIdx) -> Value() / 100.0;;
      stereoDelay->setLP1Cutoff(topDelay_lp1Cutoff);
      sprintf(writeToDisplay, "%4.0f", topDelay_lp1Cutoff*20000.0f);
      kLP1Display->SetTextFromPlug(writeToDisplay);
   
   }else
   if(paramIdx == EParams::kHP1) //Set HP1 cutoff
   {
      topDelay_hp1Cutoff = GetParam(paramIdx) -> Value() / 100.0;
      stereoDelay->setHP1Cutoff(topDelay_hp1Cutoff);
      sprintf(writeToDisplay, "%4.0f", getExpo(topDelay_hp1Cutoff));
      kHP1Display->SetTextFromPlug(writeToDisplay);
   }else
   if(paramIdx == EParams::kPan1) //Set first panner.
   {
      topDelay_pan1 = GetParam(paramIdx) -> Value() / 100.0;
      stereoDelay->setPan1(topDelay_pan1);
      sprintf(writeToDisplay, "%4.1f", topDelay_pan1*2.0f - 1.0);
      kPan1Display->SetTextFromPlug(writeToDisplay);
   }else
   if(paramIdx == EParams::kDelay1) //Set the first delay time.
   {
      topDelay_delay1  = GetParam(paramIdx) -> Value() / 100.0;
      sprintf(writeToDisplay, "%4.1f", (int)(topDelay_delay1*15.0f)+1.0f);
      kDelay1Display->SetTextFromPlug(writeToDisplay);
   }else
   if(paramIdx == EParams::kFeedback1) //Set the first bottomDelay_feedback time.
   {
      topDelay_fb1 = GetParam(paramIdx) -> Value() / 100.0;
      stereoDelay->setFeedback1(topDelay_fb1);
      sprintf(writeToDisplay, "%4.2f", topDelay_fb1);
      kFeedback1Display->SetTextFromPlug(writeToDisplay);
   
   }else
   if(paramIdx == EParams::kLevel1) //Set the first gain value.
   {
      topDelay_level1 = GetParam(paramIdx) -> Value() / 100.0;
      stereoDelay->setLevel1(topDelay_level1);
      sprintf(writeToDisplay, "%4.2f", topDelay_level1);
      kLevel1Display->SetTextFromPlug(writeToDisplay);
   }else
   //////////////////////////////
   if(paramIdx == EParams::kLP2) //Set the second LP Cutoff.
   {
      topDelay_lp2Cutoff = GetParam(paramIdx) -> Value() / 100.0;
      stereoDelay->setLP2Cutoff(topDelay_lp2Cutoff);
      sprintf(writeToDisplay, "%4.0f", topDelay_lp2Cutoff*20000.0f);
      kLP2Display->SetTextFromPlug(writeToDisplay);
   }else
   if(paramIdx == EParams::kHP2) //Set the second HP cutoff.
   {
      topDelay_hp2Cutoff = GetParam(paramIdx) -> Value() / 100.0;
      stereoDelay->setHP2Cutoff(topDelay_hp2Cutoff);
      sprintf(writeToDisplay, "%4.0f", getExpo(topDelay_hp2Cutoff));
      kHP2Display->SetTextFromPlug(writeToDisplay);
   }else
   if(paramIdx == EParams::kPan2) //Set the second panner.
   {
      topDelay_pan2 = GetParam(paramIdx) -> Value() / 100.0;
      stereoDelay->setPan2(topDelay_pan2);
      sprintf(writeToDisplay, "%4.1f", topDelay_pan2*2.0f - 1.0);
      kPan2Display->SetTextFromPlug(writeToDisplay);
   }else
   if(paramIdx == EParams::kDelay2) //Set the second delay time.
   {
      topDelay_delay2  = GetParam(paramIdx) -> Value() / 100.0;
      sprintf(writeToDisplay, "%4.1f", (int)(topDelay_delay2*15.0f)+1.0f);
      kDelay2Display->SetTextFromPlug(writeToDisplay);
   }else
   if(paramIdx == EParams::kFeedback2) //Set the second bottomDelay_feedback.
   {
   	
      topDelay_fb2 = GetParam(paramIdx) -> Value() / 100.0;
      stereoDelay->setFeedback2(topDelay_fb2);
      sprintf(writeToDisplay, "%4.2f", topDelay_fb2);
      kFeedback2Display->SetTextFromPlug(writeToDisplay);
   }else
   if(paramIdx == EParams::kLevel2) //Set the second gain.
   {
      topDelay_level2 = GetParam(paramIdx) -> Value() / 100.0;
      stereoDelay->setLevel2(topDelay_level2);
      sprintf(writeToDisplay, "%4.2f", topDelay_level2);
      kLevel2Display->SetTextFromPlug(writeToDisplay);
   }
}

