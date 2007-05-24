#ifndef SPIKEPOINT_H
#define SPIKEPOINT_H
#include <data/spike.h>
#include <vector>
#include <iostream>
#include <algorithm>

class SpikePoint
{
 private:
  vector<float> peaks_; 
  soma::Spike spike_;
  int triggerChan_;
  unsigned int time_; 
  float fade_; 
  

 public:
  SpikePoint(soma::Spike &);
  const float getPeak(int); 
  const int getTriggerChan(); 
  const unsigned int getTime(); 
  const float  getFade(); 
  const soma::Spike & getSpike(); 

};
  
#endif // SPIKEPOINTS_H
