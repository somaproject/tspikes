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

#include "network/networkinterface.h"
#include "network/data/tspike.h"
#include "network/data/rawdata.h"

#include "clusterview.h"
#include "spikewaveview.h"

class TSpikeWin : public Gtk::Window
{
public:
  explicit TSpikeWin(NetworkInterface * network);
  virtual ~TSpikeWin();

protected:
  virtual bool on_idle();

protected:
  
  
  NetworkInterface * pNetwork_; 

  GLSPVect_t *  spvect_; 

  GLSPVect_tpList spVectpList_; 

  // member widgets:

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

  SpikeWaveView spikeWaveViewX_; 
  SpikeWaveView spikeWaveViewY_; 
  SpikeWaveView spikeWaveViewA_; 
  SpikeWaveView spikeWaveViewB_; 
  
  // append data functions
  bool dataRXCallback(Glib::IOCondition io_condition); 
  void appendTSpikeToSpikewaves(const TSpike_t & tspike); 
  void appendTSpikeToSPL(const TSpike_t & tspike);

};

#endif // TSPIKEWIN_H
