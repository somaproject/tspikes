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

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <somanetwork/networkinterface.h>
#include <somanetwork/tspike.h>
#include <somanetwork/datapacket.h>

#include "clusterview.h"
#include "spikewaveview.h"
#include "ratetimeline.h"
#include "somanetcodec.h"
#include "sourcesettingswin.h" 

// widget list types
typedef boost::ptr_vector<SpikeWaveView> pSpikeWaveViewVect_t; 
typedef boost::ptr_vector<SpikeWaveView> pClusterViewVect_t; 

// helper
void printEvent(Event_t event); 

class TSpikeWin : public Gtk::Window
{
public:
  explicit TSpikeWin(NetworkInterface * network, datasource_t src);

  virtual ~TSpikeWin();

  void setTime(rtime_t t);
  void loadExistingSpikes(const std::vector<TSpike_t> & spikes); 

protected:
  virtual bool on_idle();

protected:
  
  
  NetworkInterface * pNetwork_; 
  datasource_t dsrc_; 

  GLSPVect_t *  spvect_; 

  GLSPVectpList_t spVectpList_; 

  float spVectDuration_; 

  // member widgets:

  Gtk::Table clusterTable_; 
  Gtk::Table spikeWaveTable_; 
  Gtk::VBox spikeWaveVBox_, clusterViewVBox_; 
  Gtk::HBox mainHBox_; 
  Gtk::HBox timeLineHBox_; 

  // 
  
  ClusterView clusterViewXY_;
  ClusterView clusterViewXA_;
  ClusterView clusterViewXB_;
  ClusterView clusterViewYA_;
  ClusterView clusterViewYB_;
  ClusterView clusterViewAB_;

  SpikeWaveView spikeWaveViewX_;
  SpikeWaveView spikeWaveViewY_;
  SpikeWaveView spikeWaveViewA_;
  SpikeWaveView spikeWaveViewB_;
  
  RateTimeline rateTimeline_; 

  // action menu item 
  Glib::RefPtr<Gtk::ActionGroup> refActionGroup_; 
  Glib::RefPtr<Gtk::UIManager> refUIManager_; 
  
  Gtk::Menu * pMenuPopup_; 

  // properties


  // append data functions
  

  // update clusters
  void updateClusterView(bool, float, float); 

  void liveToggle(); 

  // timeline manipulation
  rtime_t currentTime_; 
  Gtk::ToggleButton liveButton_; 
  SomaNetworkCodec somaNetworkCodec_; 

  // property editor

  SourceSettingsWin sourceSettingsWin_;   
  // callbacks
  void timeUpdateCallback(somatime_t); 
  void sourceStateChangeCalback(int, TSpikeChannelState); 
  void newTSpikeCallback(const TSpike_t &); 
  void setupMenus(); 

  bool on_button_press_event(GdkEventButton* event); 

  // functions
  void on_action_quit(); 
  void on_action_source_settings(void);


};

#endif // TSPIKEWIN_H
