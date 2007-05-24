
#include "FrameCache.h"
#include <strings.h>
#include <iostream>

Frame::Frame(int width, int height, unsigned int tbase)  :
  width_ (width),
  height_ (height),
  tbase_ (tbase),
  r_(width * height),
  g_(width * height),
  b_(width * height), 
  pix_(width * height * 3)
{
  
  zero(); 
}

void Frame::zero(void)
{
  std::fill(r_.begin(), r_.end(), 0); 
  std::fill(g_.begin(), g_.end(), 0); 
  std::fill(b_.begin(), b_.end(), 0); 

  std::fill(pix_.begin(), pix_.end(), 0); 

}

void Frame::hitPoint(int x, int y, int chan) 
{
  // this is where we define what having a point there means: 
  int newr (0), newg(0), newb(0); 

  switch (chan) 
    {
    case 1:
      newr = 0; 
      newg = 0; 
      newb = 200; 
      break; 
    case 2:
      newr = 0; 
      newg = 200; 
      newb = 0; 
      break; 
    case 3:
      newr = 200; 
      newg = 0; 
      newb = 0; 
      break; 
    case 4:
      newr = 200; 
      newg = 0; 
      newb = 200; 
      break; 
    }
  
  if ((x >= 0 and x < width_) and (y >=0 and y < height_)) {
    //r_[x*y] = std::min(255, r_[x*y] + newr); 
    //g_[x*y] = std::min(255, g_[x*y] + newr); 
    //b_[x*y] = std::min(255, b_[x*y] + newr); 
    pix_[x*y] = std::min(255, pix_[x*y] + newr); 
    pix_[x*y+1] = std::min(255, pix_[x*y+1] + newg); 
    pix_[x*y+2] = std::min(255, pix_[x*y+2] + newb); 


  }
}

FrameCache::FrameCache(SpikePointList * sl, const SpikeView & sv, 
		       const SpikeTimeline & tl)  :
  sl_ (sl),
  sv_ (sv),
  tl_ (tl),
  spikePos_(0)
  
{
  fc_ = new std::list<Frame>; 
  Frame fnew(sv_.getWidth(), sv_.getHeight(), 0);
  for (int i = 0; i < 300; i++) {
    fc_->push_back(fnew); 
  }
}

int FrameCache::size() {
  return fc_->size(); 
}

void FrameCache::rebuildCache() 
{
  // delete the old cache
  
  int fcsize = 0; 

  //fc_ = new std::list<Frame>(); 
  //Frame f(sv_.getWidth(), sv_.getHeight(), 0);
  //fc_->push_back(f); 
  
  std::list<Frame>::iterator fcpoint = fc_->begin();
  fcpoint->zero();
  
  fcsize++; 
  fcpoint++; 



  for (int i = 0; i < sl_->size(); i++)
    {  
      // add in extra frames
      //while ( sp->ts / tl_.granularity > fcsize) {
	//Frame fnew(sv_.getWidth(), sv_.getHeight(), 0);
	//fc_->push_back(f); 
      //fcpoint->zero();
      //fcpoint++; 
      //fcsize++; 
      //}
      
      fcpoint->hitPoint(sv_.getX(sl_->at(i)), sv_.getY(sl_->at(i)), 1); 
    }
}

