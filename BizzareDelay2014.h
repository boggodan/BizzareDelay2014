#ifndef __BIZZAREDELAY2014__
#define __BIZZAREDELAY2014__

#include "IPlug_include_in_plug_hdr.h"

class BizzareDelay2014 : public IPlug
{
public:
  BizzareDelay2014(IPlugInstanceInfo instanceInfo);
  ~BizzareDelay2014();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mGain;
};

#endif
