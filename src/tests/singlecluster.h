#ifndef SINGLECLUSTER_H
#define SINGLECLUSTER_H

#include <gtkmm.h>
#include <gtkmm/window.h>
#include "../clusterbox.h"
#include "../spikes.h"
#include <string>
#include <vector>
#include <sigc++/sigc++.h>


using namespace std;

class SingleCluster : public Gtk::Window
{

public:
  SingleCluster();
  virtual ~SingleCluster(); 
protected:
  Gtk::HBox mainbox_; 
  SpikeBuffer  testBuffer_; 
  ClusterBox clusterBox12_, 
    clusterBox13_, 
    clusterBox14_, 
    clusterBox23_, 
    clusterBox24_, 
    clusterBox34_;
  Gtk::Table clustTable_; 
  Gtk::Adjustment adjTime_; 
  Gtk::VScale scaleTime_;
  
  void onTimeChange(); 


};

#endif //SINGLECLUSTER_H
