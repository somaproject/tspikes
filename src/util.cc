
#include "util.h"

FillSpace::FillSpace()
{
  /* 4d space-filling*/ 
  shell_ = 0; 
}

std::vector<int> FillSpace::next()
{
  if (points_.empty()) {
    /*
      This is -really- a pathetic way of computing this
     */ 
    for (int i = 0; i <= shell_; i++) {
      for (int j = 0; j <= shell_; j++) {
	for (int k = 0; k <= shell_; k++) {
	  for (int l = 0; l <= shell_; l++) {
	    if ((i == shell_) or
		(j == shell_) or
		(k == shell_) or
		(l == shell_)) {
	      std::vector<int> newv(4);
	      newv[0] = i; 
	      newv[1] = j;
	      newv[2] = k;
	      newv[3] = l; 
	      points_.push_back(newv); 
	    }
	  }
	}
      }
    }
    shell_++; 
    
  }
  std::vector<int> v = points_.front(); 
  points_.pop_front(); 
  return v; 
}
