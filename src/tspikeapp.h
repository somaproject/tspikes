#ifndef TSPIKEAPP_H
#define TSPIKEAPP_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <assert.h>

#include <stdlib.h>
#include <gtkmm.h>
#define GL_GLEXT_PROTOTYPES
#include <gtkglmm.h>

#ifdef G_OS_WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

class TSpikeApp : public Gtk::Window
{
public:
  explicit TSpikeApp(bool is_sync = true);
  virtual ~TSpikeApp();

protected:
  virtual bool on_idle();

protected:
  // member widgets:
  Gtk::VBox m_VBox;
  Gtk::Table table_; 

  ClusterView clusterView12_; 
  ClusterView clusterView13_; 
  ClusterView clusterView14_; 
  ClusterView clusterView23_; 
  ClusterView clusterView24_; 
  ClusterView clusterView34_; 

  Glib::Timer timer_; 
  Glib::Timer dtimer_; 
  Gtk::Button m_ButtonQuit;
  Gtk::Adjustment spikePosAdjustment_; 
  Gtk::HScale spikePosScale_; 
  void updateSpikePosFromAdj(); 
  sigc::connection m_ConnectionIdle;

  virtual bool on_key_press_event(GdkEventKey* event); 


};

#endif // TSPIKEAPP_H
