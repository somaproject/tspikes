#ifndef FRAMECACHE_H
#define FRAMECACHE_H

#include "spikes.h"
#include <vector>
#include <list>

class Frame {
  
 public: 
  Frame(int width, int height, unsigned int tbase); 
  
  void hitPoint(int x, int y, int chan); 
  void zero(void);
 private:
  int width_; 
  int height_; 
  unsigned int tbase_; 
  std::vector<unsigned char > r_; 
  std::vector<unsigned char > g_;
  std::vector<unsigned char > b_;
  std::vector<unsigned char > pix_; 

}; 
 
class FrameCache {
  
 public:
  FrameCache(SpikePointList * sl, const SpikeView & sv, 
	     const SpikeTimeline & tl); 
  void setView(const SpikeView & sv); 
  void setTimeline(const SpikeTimeline & tl); 
  void setPoints(void); 
  
  void rebuildCache(); 
  int size(); 
 private:
  SpikePointList * sl_;
  SpikeView sv_; 
  SpikeTimeline tl_; 
  int spikePos_;
  std::list<Frame> * fc_; 


};


#endif //FRAMECACHE_H
