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

#include "clusterview.h"
#include "spikewaveview.h"

class TSpikeWin : public Gtk::Window
{
public:
  explicit TSpikeApp(Network * network);
  virtual ~TSpikeApp();

protected:
  virtual bool on_idle();

protected:
  // member widgets:
  
  Gtk::Table clusterTable_; 
  Gtk::Table spikeWaveTable_; 

  ClusterView clusterView12_; 
  ClusterView clusterView13_; 
  ClusterView clusterView14_; 
  ClusterView clusterView23_; 
  ClusterView clusterView24_; 
  ClusterView clusterView34_; 

  SpikeWaveView SpikeWaveView1_; 
  SpikeWaveView SpikeWaveView2_; 
  SpikeWaveView SpikeWaveView3_; 
  SpikeWaveView SpikeWaveView4_; 

  Network * pNetwork_; 
  GLSPVectpList spVectpList_; 
  

};

#endif // TSPIKEAPP_H
