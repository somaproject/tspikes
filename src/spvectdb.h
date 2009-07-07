#ifndef SPVECTDB_H
#define SPVECTDB_H

#include <boost/utility.hpp>
#include "glspikes.h"

/*
  An attempt to centralize the mechanisms behind the spike point
  vector buffer. Not really a database, but because there is some
  policy in here it seemed inappropriate to just call it a container.

  
 */

class SpikePointVectDatabase : boost::noncopyable
{
public:
  SpikePointVectDatabase(reltime_t minBufferDuration, 
			 size_t maxSpikeCount); 

  void append(const GLSpikePoint_t & sp); 

  void setTime(reltime_t); 
  
  void reset(); 
  size_t size(); // number of spikes
  
  
  inline GLSPVectMap_t::const_iterator begin() const {

    return spVectpMap_.begin(); 

  }

  inline GLSPVectMap_t::const_iterator end() const {

    return spVectpMap_.end(); 

  }

  inline GLSPVectMap_t::const_reverse_iterator rbegin() const {

    return spVectpMap_.rbegin(); 

  }

  inline GLSPVectMap_t::const_reverse_iterator rend() const {

    return spVectpMap_.rend(); 

  }

  inline GLSPVectMap_t::const_iterator lower_bound(reltime_t r) const {

    return spVectpMap_.lower_bound(r); 

  }

  
  inline GLSPVectMap_t::const_iterator upper_bound(reltime_t r) const {

    return spVectpMap_.lower_bound(r); 

  }

  inline GLSPVectMap_t::const_iterator getLastIter() const {
    GLSPVectMap_t::const_iterator i; 
    i = spVectpMap_.end(); 
    assert(!spVectpMap_.empty()); 

    --i; 
    return i; 
}

private:
  GLSPVectMap_t spVectpMap_; 
  const reltime_t BUFDUR_; 
  const size_t MAXSPIKECOUNT_; // upper limit on how much memory we can consume
  size_t spikeCount_; 
  
}; 




#endif // SPVECTDB_H
