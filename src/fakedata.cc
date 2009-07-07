#include "fakedata.h"
#include <stdlib.h>

namespace fakedata { 





  Grid::Grid(somatime_t starttime,  // start time 
	     float rate, // rate in spikes-per-second
	     int32_t amplitude_delta) :  // amplitude delta 
    coord_(0), 
    rate_(rate), 
    ampval_(amplitude_delta) 
  {
    tspike_.time = starttime; 
    for (int i = 0; i < somanetwork::TSPIKEWAVE_LEN; i++) {
      tspike_.x.wave[i] = 0; 
      tspike_.y.wave[i] = 0; 
      tspike_.a.wave[i] = 0; 
      tspike_.b.wave[i] = 0; 
    }
    
  }
  
  somanetwork::TSpike_t Grid::next() {
    
    somatime_t timedelta = somatime_t(50000 * (1.0/rate_));
    tspike_.time += timedelta; 
    int pos = 8;

    std::vector<int> v = fs_.next(); 
    float jitter =  0.1 * ampval_ * float(rand()) / RAND_MAX; 

    tspike_.x.wave[pos] = v[0] * ampval_ + jitter; 
    tspike_.y.wave[pos] = v[1] * ampval_  + jitter; 
    tspike_.a.wave[pos] = v[2] * ampval_  + jitter; 
    tspike_.b.wave[pos] = v[3] * ampval_  + jitter; 

    return tspike_; 
  
  }

}
