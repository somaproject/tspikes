#ifndef CLUSTERRENDERER_H
#define CLUSTERRENDERER_H

#include "spikes.h"
#include "clusterview.h"
#include "timeview.h"

class ClusterRenderer
{
 public:
  std::vector<unsigned char> frame_;
  ClusterView sv_; 
  SpikePointList * sl_; 
  SpikePointList::iterator slpos_;
  TimeView tv_; 
  ClusterRenderer(SpikePointList * sl, ClusterView & sv, 
		 TimeView & tl); 
 private:
  void rebuild(void); 
};


#endif //CLUSTERRENDERER_H
