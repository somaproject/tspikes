

#include "spvectdb.h"

SpikePointVectDatabase::SpikePointVectDatabase(reltime_t minBufferDuration, 
					       size_t maxSpikeCount) : 
  BUFDUR_(minBufferDuration), 
  MAXSPIKECOUNT_(maxSpikeCount)
{
  
  reset(); 
  
}

void SpikePointVectDatabase::reset()
{
  spVectpMap_.clear(); 
  spikeCount_ = 0; 
  reltime_t val(0.0); 
  spVectpMap_.insert(val, new GLSPVect_t); 
      
}



size_t SpikePointVectDatabase::size() {
  return spikeCount_; 
}

void SpikePointVectDatabase::append(const GLSpikePoint_t & sp) {
  GLSPVectMap_t::iterator i = spVectpMap_.end(); 
  i--; 
  i->second->push_back(sp); 

  spikeCount_++; 
  if (spikeCount_ > MAXSPIKECOUNT_) {
    spikeCount_ -= spVectpMap_.rbegin()->second->size(); 
    //spVectpMap_.erase(spVectpMap_.rbegin()->first); // FIXME: this does an unnecessary look-up
    spVectpMap_.erase(--(spVectpMap_.end())); 

  }
}



void SpikePointVectDatabase::setTime(reltime_t reltime) {
  /* Fixme : should create the correct number of spvects? maybe? 
   */ 
  
  if ((reltime - spVectpMap_.rbegin()->first ) >= BUFDUR_ )
    {
      spVectpMap_.insert(reltime, new GLSPVect_t); 
    }
  
}
