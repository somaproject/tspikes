#ifndef TTREADER_H
#define TTREADER_H
#include <fstream>
#include <string>
#include <somanetwork/tspike.h>

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
  TSpike_t getTSpike();

  bool eof(); 
  
  
 private:
  std::ifstream file_; 
  int gains_[8]; 
  int thresholds_[8]; 

};
  


#endif // TTREADER_H
