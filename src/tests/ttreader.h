#ifndef TTREADER_H
#define TTREADER_H

#include <fstream>
#include <string>
#include "spikes.h"

struct ttspike {
  unsigned int ts; 
  short w1[32];
  short w2[32];
  short w3[32];
  short w4[32];
};
  
class ttreader {
  
 public:
  ttreader(std::string filename); 
  
  int getSpike(ttspike * sp); 
  
  //void getPeaks(ttspike *spike, SpikePoint *sp); 

  bool eof(); 
 private:
  std::ifstream file_; 
  int gain_; 
};
  


#endif // TTREADER_H
