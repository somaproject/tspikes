#ifndef TSPIKES_FAKEDATA_H
#define TSPIKES_FAKEDATA_H
#include <somanetwork/tspike.h>
#include "somatime.h"
#include "util.h" 

/*
  Code to generate test data. 

 */ 

namespace fakedata { 
  
  class FakeDataSource  {
  public: 
    virtual somanetwork::TSpike_t next() = 0; 
  }; 

  class Grid : public FakeDataSource {
    /* 
       An isotropic set of points in space
     */ 
  public:
     Grid(somatime_t starttime,  // start time 
	  float rate, // rate in spikes-per-second
	  int32_t amplitude_delta);  // amplitude delta 

    somanetwork::TSpike_t next(); 
  private:
    int coord_;  // which coordinate are we incrementing next
    int ampval_; 
    float rate_; 
    somanetwork::TSpike_t tspike_; 
    FillSpace fs_; 
  }; 


}

#endif // TSPIKES_FAKEDATA_H
