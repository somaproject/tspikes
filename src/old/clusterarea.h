#ifndef CLUSTERAREA_H
#define CLUSTERAREA_H

#include <gtkmm/drawingarea.h>
#include <gdkmm/colormap.h>
#include <gdkmm/window.h>
#include "spikes.h"
#include "ClusterDisplay.h"

class ClusterArea : public Gtk::DrawingArea
{
public:
  ClusterArea(SpikePointList * spl, SpikeView sv, SpikeTimeline tl);
  virtual ~ClusterArea();
  void rebuild(); 
  void setTimeline(SpikeTimeline tl); 

protected:
  //Overridden default signal handlers:
  virtual void on_realize();
  virtual bool on_expose_event(GdkEventExpose* e);
  Gdk::Color blue_, red_;
  Glib::RefPtr<Gdk::GC> gc_;
  ClusterDisplay cd_; 
  
  //ClusterDisplay cd(&test, sv, tl); 
  //cd.rebuild(); 
  //cd.save();

};

#endif // CLUSTERAREA_H

