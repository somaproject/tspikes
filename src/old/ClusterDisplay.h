#ifndef CLUSTERDISPLAY_H
#define CLUSTERDISPLAY_H

#include "spikes.h"
#include <gtkmm.h>

class ClusterDisplay
{
 public:
  std::vector<unsigned char> frame_;
  SpikeView sv_; 
  SpikePointList * sl_; 
  SpikePointList::iterator slpos_;
  SpikeTimeline tl_; 
  ClusterDisplay(SpikePointList * sl, SpikeView sv, 
		 SpikeTimeline tl); 
  void renderToWindow(const Glib::RefPtr<Gdk::Drawable>& drawable, const Glib::RefPtr<Gdk::GC>& gc);
  void rebuild(); 
  void setTimeline(SpikeTimeline tl); 
  void save(); 

};


#endif //CLUSTERDISPLAY_H
