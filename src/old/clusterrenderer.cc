#include <iostream>
#include <limits>
#include "clusterrenderer.h"
using namespace std; 

ClusterRenderer::ClusterRenderer(SpikePointList *sl, ClusterView & sv, TimeView & tv) :
  sv_ (sv), 
  sl_(sl),
  tv_(tv), 
  frame_(sv.getWidth() * sv.getHeight()*3), 
  slpos_ ( sl_->begin())
{
  rebuild(); 

}

void ClusterRenderer::rebuild(void) {

  int count (0);
  std::fill(frame_.begin(), frame_.end(), 0); 

  if (sl_->empty()) {
    return; 
  }

  SpikePointList::reverse_iterator slrevpos = sl_->rbegin(); 

  
  unsigned int endTime;  
  if (tv_.isEndTimeNow()) {
    endTime = (*slrevpos).getts(); 
  } else {
    endTime = tv_.getEndTime();
  }

  int incamount; 

  while (slrevpos != sl_->rend()) {
    if (slrevpos->getts() < endTime ) {
      int x = sv_.getX(*slrevpos); 
      int y = sv_.getY(*slrevpos); 
      int pos = x*3 + y * sv_.getWidth()*3; 
      if (tv_.getGranularity() > 0) {
	incamount = tv_.getInitialValue() + (endTime - slrevpos->getts()) 
	  / tv_.getGranularity() * tv_.getRate(); 

      }

      if (incamount < 0) { // any later spikes won't be rendered anyway
	break;
      }
      
      if (pos < frame_.size()) {
	int rval = frame_[pos];
	
	int gval = frame_[pos + 1];
	int bval = frame_[pos + 2];
	
	switch ( slrevpos->getTrigChan() ) {
	case 1:
	  rval += incamount; 
	  break; 
	case 2:
	  gval += incamount; 
	  break; 
	case 3:
	  bval += incamount; 
	  break; 
	case 4:
	  rval += incamount; 
	  gval += incamount; 
	  break; 
	}
	
	frame_[pos] = std::min(255, rval); 
	frame_[pos+1] = std::min(255, gval); 
	frame_[pos+2] = std::min(255, bval); 
	
      }
     
    }
    slrevpos++; 
  }
  slpos_ = sl_->end();

} 
