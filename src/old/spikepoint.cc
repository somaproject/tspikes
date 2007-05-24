#include "spikepoint.h"
#include <iostream> 

SpikePoint::SpikePoint(soma::Spike & newSpike) 
  : peaks_(4),
    spike_(newSpike), // invoke copy constructor
    triggerChan_(0), 
    time_(0), 
    fade_(100)
{
  // spikepoint performs the necessary computation the first
  // time it is called. 

  time_ = spike_.timestamp; 
  
  // get peaks
  peaks_[0] = 0.0;
  peaks_[1] = 0.0;
  peaks_[2] = 0.0;
  peaks_[3] = 0.0; 

  
  for (int wf = 0; wf < soma::WAVECNT; wf++) 
    {
      
      short wfp(0); 
      for (int w = 0; w < soma::WAVELEN; w++)
	{
	  if (wfp < spike_.waveforms[wf].waveform[w])
	    wfp = spike_.waveforms[wf].waveform[w]; 
	  
	  peaks_[spike_.waveforms[wf].channel] = float(wfp) / 32768.0 / 
	    spike_.waveforms[wf].gain * spike_.waveforms[wf].ADCrange/2;    
	  if (spike_.waveforms[wf].waveform[spike_.tholdpos] 
	      > spike_.waveforms[wf].thold)
	    triggerChan_ = spike_.waveforms[wf].channel; 
	  
	}
    }

}

const float SpikePoint::getPeak(int chan)
{
  return peaks_[chan]; 
}

const int SpikePoint::getTriggerChan()
{
  return triggerChan_;
}

const unsigned int SpikePoint::getTime()
{
  return time_; 
}

const float SpikePoint::getFade()
{
  return fade_; 
}

