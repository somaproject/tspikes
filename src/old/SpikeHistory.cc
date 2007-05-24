

#include "Spike.h"

SpikeHistory::SpikeHistory(int gran)  :
  granularity_ (gran), 
  spikebins_ ()

{
  
}

void SpikeHistory::addSpike(const Spike & s) 
{
  SpikePoint sp; 
  sp.ts = s.ts; 
  sp.p1 = s.peak(1); 
  sp.p2 = s.peak(2); 
  sp.p3 = s.peak(3); 
  sp.p4 = s.peak(4); 

  int binnum = sp.ts / granularity_ ; 
  
  while (binnum + 1 > spikeBins.size()) {
    SpikeBin b; 
    spikebins_.push_back(b);
  }
  
  assert(binnum +1 == spikeBins.size());
  
  spikebins_.back().push_back(sp);

  
} 


