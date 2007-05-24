#include <gtkmm.h>
#include <gdkmm.h>
#include "clusterarea.h"
#include "spikes.h"

class SpikeWindow : public Gtk::Window
{
public:
  SpikeWindow(SpikePointList* spl);
  virtual ~SpikeWindow();

protected:
  //Signal handlers:

  SpikeView sv1_; 
  SpikeView sv2_; 
  SpikeView sv3_; 
  SpikeView sv4_; 
  SpikeView sv5_; 
  SpikeView sv6_; 

  SpikeTimeline tl_; 

  ClusterArea area1_; 
  ClusterArea area2_;
  ClusterArea area3_; 
  ClusterArea area4_; 
  ClusterArea area5_; 
  ClusterArea area6_; 

  Gtk::Adjustment rateAdj_; 
  Gtk::Adjustment endTimeAdj_; 

  Gtk::HScrollbar rateScroll_; 
  Gtk::HScrollbar endTimeScroll_; 
  
  Gtk::VBox box_;
  Gtk::VBox controlbox_; 

  Gtk::Table cltable_; 
  Gtk::Button button_;
  
  void buttonclick(); 
};
