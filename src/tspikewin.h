#ifndef TSPIKEWIN_H
#define TSPIKEWIN_H

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

#include "network/network.h"

#include "clusterview.h"
#include "spikewaveview.h"

class TSpikeWin : public Gtk::Window
{
public:
  explicit TSpikeWin(Network * network);
  virtual ~TSpikeWin();

protected:
  virtual bool on_idle();

protected:
  // member widgets:
  
  
  Network * pNetwork_; 


  GLSPVect *  spvect_; 

  GLSPVectpList spVectpList_; 


  Gtk::Table clusterTable_; 
  Gtk::Table spikeWaveTable_; 
  Gtk::VBox spikeWaveVBox_, clusterViewVBox_; 
  Gtk::HBox mainHBox_; 


  ClusterView clusterView12_; 
  ClusterView clusterView13_; 
  ClusterView clusterView14_; 
  ClusterView clusterView23_; 
  ClusterView clusterView24_; 
  ClusterView clusterView34_; 

  SpikeWaveView spikeWaveView1_; 
  SpikeWaveView spikeWaveView2_; 
  SpikeWaveView spikeWaveView3_; 
  SpikeWaveView spikeWaveView4_; 
  

  

};

#endif // TSPIKEWIN_H
