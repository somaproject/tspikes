#ifndef SPIKE_H
#define SPIKE_H

class Spike
{
 public:
  float thold_;
  std::vector<float> w1_; 
  std::vector<float> w2_;
  std::vector<float> w3_;
  std::vector<float> w4_; 

  Spike(int g1, int g2, int g3, int g4, 
	short [] w1s, short[] w2s, short[] w3s, short[] w4s) :
    w1_(32), 
    w2_(32),
    w3_(32),
    w4_(32){ 
    
    for (int i = 0; i < 32; i++) {
      w1_[i] = w1s[i] / g1; 
      w2_[i] = w2s[i] / g2; 
      w3_[i] = w3s[i] / g3; 
      w4_[i] = w4s[i] / g4; 
    }

  }

  Spike()
    w1_(32), 
    w2_(32),
    w3_(32),
    w4_(32){ 
    
  }
  
  float Spike::peak(int chan) {
    switch( chan)
      {
      case 1:
	return * (std::max_element( w1_.begin(), w1_.end())); 
	break;
      case 2:
	return * (std::max_element( w2_.begin(), w2_.end())); 
	break;
      case 3:
	return * (std::max_element( w3_.begin(), w3_.end())); 
	break;
      case 4:
	return * (std::max_element( w4_.begin(), w4_.end())); 
	break;
      }
  }


}

#endif // SPIKE_H
