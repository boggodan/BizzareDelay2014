#ifndef __BIZZAREDELAY__
#define __BIZZAREDELAY__

/*
BizzareDelay - VST, AU

Developed for Bizzare Contact by 1st Creative Solutions

Code: Bogdan Vera.
Graphical Design: Matt O'Neill
*/

//Include the files for the two delay classes.
#include "DSP_TapeDelay.h"
#include "DSP_StereoDelay.h"
#include "IPlug_include_in_plug_hdr.h"

//the exponential scaling function and denormalizer
#include "utilities.h"

class IKnobMultiControlText : public IKnobControl  
{

private:
  IRECT mTextRECT, mImgRECT;
  IBitmap mBitmap;
  
public:
  IKnobMultiControlText(IPlugBase* pPlug, IRECT pR, int paramIdx, IBitmap* pBitmap, IText* pText)
	:	IKnobControl(pPlug, pR, paramIdx), mBitmap(*pBitmap)
  {
    mText = *pText;
    mTextRECT = IRECT(mRECT.L, mRECT.B-20, mRECT.R, mRECT.B);
    mImgRECT = IRECT(mRECT.L, mRECT.T, &mBitmap);
    mDisablePrompt = false;
	}
	
	~IKnobMultiControlText() {}
	
  bool Draw(IGraphics* pGraphics)
  {
    int i = 1 + int(0.5 + mValue * (double) (mBitmap.N - 1));
    i = BOUNDED(i, 1, mBitmap.N);
    pGraphics->DrawBitmap(&mBitmap, &mImgRECT, i, &mBlend);
    //pGraphics->FillIRect(&COLOR_WHITE, &mTextRECT);
    
    char disp[20];
    mPlug->GetParam(mParamIdx)->GetDisplayForHost(disp);
    
    if (CSTR_NOT_EMPTY(disp)) {
      return pGraphics->DrawIText(&mText, disp, &mTextRECT);
    }
    return true;
  }
  
	void OnMouseDown(int x, int y, IMouseMod* pMod)
	{
    if (mTextRECT.Contains(x, y)) PromptUserInput(&mTextRECT);
    #ifdef RTAS_API
    else if (pMod->A) {
      if (mDefaultValue >= 0.0) {
        mValue = mDefaultValue;
        SetDirty();
      }
    }
    #endif
    else {
      OnMouseDrag(x, y, 0, 0, pMod);
    }
	}
  
  void OnMouseDblClick(int x, int y, IMouseMod* pMod)
  {
#ifdef RTAS_API
    PromptUserInput(&mTextRECT);
#else
    if (mDefaultValue >= 0.0) {
      mValue = mDefaultValue;
      SetDirty();
    }
#endif
  }
  
};

class BizzareDelay : public IPlug
{
public:
  
  BizzareDelay(IPlugInstanceInfo instanceInfo);
  ~BizzareDelay();
  
  void Reset();
  void OnParamChange(int paramIdx);
  
  
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  
  private:

  //the main delay objects
  TapeDelay *tapeDelay1;
  StereoDelay *stereoDelay;

  //the plug-in parameter values
  float tempoUnit;
  float *outl;
  float inl,inr;
  //count samples until nag cutout (No longer used because the plug-in is free).
  int nagCut; 
  //cutout multipler
  //can be 0 or 1 and we multiply the output by it
  float cutout;
  /**************************************************
  *********THE ANALOG DELAY**************************/
  
  
  float tempo;
  
  float bottomDelay_lpCut;
  float bottomDelay_hpCut;
  
  float bottomDelay_feedback;
  float bottomDelay_mix;
  float onOffPong;
  
  float bottomDelay_tempoNote;
  float currentTempoUnit;
  
  float bottomDelay_fine;
  float bottomDelay_fudgedFine;
  
  float bottomDelay_lfoRate;
  float bottomDelay_lfoIntensity;
  
  float bottomDelay_fltRate;
  float bottomDelay_fltIntensity;
  
  float bottomDelay_smooth;
  
  float rackSwitch;
  
  /**************************************************
  *********THE STEREO DELAY**************************/
  
  
  
  float beatsPerMinute;
  float topDelay_tempoNote;
  float topDelay_wetDry;
  float topDelay_lp1Cutoff, topDelay_lp2Cutoff;
  float topDelay_hp1Cutoff,topDelay_hp2Cutoff;
  float topDelay_pan1,topDelay_pan2;
  float topDelay_delay1,topDelay_delay2;
  float topDelay_fb1,topDelay_fb2;
  float topDelay_level1,topDelay_level2;
  
  float wet1,dry1;
  float wet2,dry2;

  ITextControl 
  *k2hicutDisplay,
  *k2lowcutDisplay,
  *k2msDelay1Display,
  *k2feedbackDisplay,
  *k2noteDisplay,
  *k2fineDisplay,
  *k2LFOintensityDisplay,
  *k2LFOrateDisplay,
  *k2FltIntDisplay,
  *k2FltRateDisplay,
  *k2SmoothDisplay,
  *k2mixDisplay,
  *kunitsDisplay,
  *kwetdry1Display,
  *kLP1Display,
  *kHP1Display,
  *kPan1Display,
  *kDelay1Display,
  *kFeedback1Display,
  *kLevel1Display,
  *kLP2Display,
  *kHP2Display,
  *kPan2Display,
  *kDelay2Display,
  *kFeedback2Display,
  *kLevel2Display;
};

#endif
