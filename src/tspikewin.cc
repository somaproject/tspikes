#include "tspikewin.h"
#include "glspikes.h"
#include <boost/assign.hpp>

void printEvent(Event_t event)
{
  printf("Event cmd = %2.2X src = %2.2X\n", event.cmd, event.src); 
  for (int i = 0; i < 5; i++) {
    printf("Data word %d: %4.4X\n", i, event.data[i]); 
  }

}

TSpikeWin::TSpikeWin(pNetworkInterface_t pNetwork, 
		     datasource_t src,
		     somatime_t expStartTime_soma
		     ) : 

  pNetwork_(pNetwork), 
  
  spVectpList_(), 

  clusterTable_(2, 3), 
  spikeWaveTable_(2, 2), 
  spikeWaveVBox_(false, 0), 
  clusterViewVBox_(false, 0), 
  mainHBox_(false, 0), 

  clusterViewXY_(spVectpList_, VIEW12), 
  clusterViewXA_(spVectpList_, VIEW13), 
  clusterViewXB_(spVectpList_, VIEW14), 
  clusterViewYA_(spVectpList_, VIEW23), 
  clusterViewYB_(spVectpList_, VIEW24), 
  clusterViewAB_(spVectpList_, VIEW34), 
  
  spikeWaveViewX_(CHANX), 
  spikeWaveViewY_(CHANY), 
  spikeWaveViewA_(CHANA), 
  spikeWaveViewB_(CHANB),

  spVectorStartTime_(0.0),
  liveButton_("Live"), 
  somaNetworkCodec_(pNetwork_, src), 
  sourceSettingsWin_(&somaNetworkCodec_), 
  pMenuPopup_(0), 
  dsrc_(src), 
  offsetTime_(0.0), 
  spikeCount_(0), 
  expStartTime_(expStartTime_soma)
{
  //
  // Top-level window.
  //
  
  set_title("Tetrode Spike Viewer");

  set_reallocate_redraws(true);
  
  reltime_t rt = abstimeToRelTime(spVectorStartTime_, offsetTime_); 
  spVectpList_.insert(rt, new GLSPVect_t);
  //boost::assign::ptr_map_insert<GLSPVect_t>(spVectpList_)(abstimeToRelTime(spVectorStartTime_, offsetTime_)); 
  add(mainHBox_); 
  
  int clusterWidth = 165; 
  clusterViewXY_.set_size_request(clusterWidth, clusterWidth);
  clusterViewXA_.set_size_request(clusterWidth, clusterWidth);
  clusterViewXB_.set_size_request(clusterWidth, clusterWidth);
  clusterViewYA_.set_size_request(clusterWidth, clusterWidth);
  clusterViewYB_.set_size_request(clusterWidth, clusterWidth);
  clusterViewAB_.set_size_request(clusterWidth, clusterWidth);

  
  clusterTable_.attach(clusterViewXY_, 0, 1, 0, 1);
  clusterTable_.attach(clusterViewXA_, 1, 2, 0, 1);
  clusterTable_.attach(clusterViewXB_, 2, 3, 0, 1);
  clusterTable_.attach(clusterViewYA_, 0, 1, 1, 2);
  clusterTable_.attach(clusterViewYB_, 1, 2, 1, 2);
  clusterTable_.attach(clusterViewAB_, 2, 3, 1, 2);

  clusterTable_.set_border_width(1.0); 
  clusterTable_.set_row_spacings(1.0); 
  clusterTable_.set_col_spacings(1.0); 

  rateTimeline_.set_size_request(clusterWidth*3, 60); 
  
  clusterViewVBox_.pack_start(clusterTable_) ;
  clusterViewVBox_.pack_start(timeLineHBox_); 
  timeLineHBox_.pack_start(rateTimeline_); 
  timeLineHBox_.pack_start(liveButton_); 
  
  spikeWaveViewX_.set_size_request(150, clusterWidth); 
  spikeWaveViewY_.set_size_request(150, clusterWidth); 
  spikeWaveViewA_.set_size_request(150, clusterWidth); 
  spikeWaveViewB_.set_size_request(150, clusterWidth); 

  spikeWaveTable_.attach(spikeWaveViewX_, 0, 1, 0, 1); 
  spikeWaveTable_.attach(spikeWaveViewY_, 1, 2, 0, 1); 
  spikeWaveTable_.attach(spikeWaveViewA_, 0, 1, 1, 2); 
  spikeWaveTable_.attach(spikeWaveViewB_, 1, 2, 1, 2); 

  spikeWaveTable_.set_border_width(1.0); 
  spikeWaveTable_.set_row_spacings(1.0); 
  spikeWaveTable_.set_col_spacings(1.0); 
  spikeWaveVBox_.pack_start(spikeWaveTable_); 
  
  mainHBox_.pack_start(spikeWaveVBox_); 
  mainHBox_.pack_start(clusterViewVBox_);   
  //
  // Show window.
  //

  float decay = 0.05; 
  clusterViewXY_.setView(spVectpList_.begin(), 
			 spVectpList_.end(), 
			 decay, LOG); 
  clusterViewXY_.setViewingWindow(0, 0, float(150e-6), float(150e-6));

  clusterViewXA_.setView(spVectpList_.begin(), 
			 spVectpList_.end(), 
			 decay, LOG); 
  clusterViewXA_.setViewingWindow(0, 0, float(150e-6), float(150e-6));

  clusterViewXB_.setView(spVectpList_.begin(), 
			 spVectpList_.end(), 
			 decay, LOG); 
  clusterViewXB_.setViewingWindow(0, 0, float(150e-6), float(150e-6));

  clusterViewYA_.setView(spVectpList_.begin(), 
			 spVectpList_.end(), 
			 decay, LOG); 
  clusterViewYA_.setViewingWindow(0, 0, float(150e-6), float(150e-6));

  clusterViewYB_.setView(spVectpList_.begin(), 
			 spVectpList_.end(), 
			 decay, LOG); 
  clusterViewYB_.setViewingWindow(0, 0, float(150e-6), float(150e-6));

  clusterViewAB_.setView(spVectpList_.begin(), 
			 spVectpList_.end(), 
			 decay, LOG); 
  clusterViewAB_.setViewingWindow(0, 0, float(150e-6), float(150e-6));


  // now we wire up all of the views together so zooms are sync'd
  clusterViewXY_.xViewChangeSignal().connect(
					      sigc::mem_fun(clusterViewXA_, 
							    &ClusterView::setXView)); 
  clusterViewXY_.xViewChangeSignal().connect(
					      sigc::mem_fun(clusterViewXB_, 
							    &ClusterView::setXView)); 

  clusterViewXY_.yViewChangeSignal().connect(
					      sigc::mem_fun(clusterViewYA_, 
							    &ClusterView::setXView)); 

  clusterViewXY_.yViewChangeSignal().connect(
					      sigc::mem_fun(clusterViewYB_, 
							    &ClusterView::setXView)); 

  
  spikeWaveViewX_.setAmplitudeView(-100e-6, 300e-6); 
  spikeWaveViewY_.setAmplitudeView(-100e-6, 300e-6); 
  spikeWaveViewA_.setAmplitudeView(-100e-6, 300e-6); 
  spikeWaveViewB_.setAmplitudeView(-100e-6, 300e-6); 
  

  spikeWaveViewX_.amplitudeViewChangedSignal().connect(
						       sigc::mem_fun(spikeWaveViewY_, 
								     &SpikeWaveView::setAmplitudeView)); 
  spikeWaveViewX_.amplitudeViewChangedSignal().connect(
						       sigc::mem_fun(spikeWaveViewA_, 
								     &SpikeWaveView::setAmplitudeView)); 
  spikeWaveViewX_.amplitudeViewChangedSignal().connect(
						       sigc::mem_fun(spikeWaveViewB_, 
								     &SpikeWaveView::setAmplitudeView)); 


  spikeWaveViewY_.amplitudeViewChangedSignal().connect(
						       sigc::mem_fun(spikeWaveViewX_, 
								     &SpikeWaveView::setAmplitudeView)); 
  spikeWaveViewY_.amplitudeViewChangedSignal().connect(
						       sigc::mem_fun(spikeWaveViewA_, 
								     &SpikeWaveView::setAmplitudeView)); 
  spikeWaveViewY_.amplitudeViewChangedSignal().connect(
						       sigc::mem_fun(spikeWaveViewB_, 
								     &SpikeWaveView::setAmplitudeView)); 

  spikeWaveViewA_.amplitudeViewChangedSignal().connect(
						       sigc::mem_fun(spikeWaveViewX_, 
								     &SpikeWaveView::setAmplitudeView)); 
  spikeWaveViewA_.amplitudeViewChangedSignal().connect(
						       sigc::mem_fun(spikeWaveViewY_, 
								     &SpikeWaveView::setAmplitudeView)); 
  spikeWaveViewA_.amplitudeViewChangedSignal().connect(
						       sigc::mem_fun(spikeWaveViewB_, 
								     &SpikeWaveView::setAmplitudeView)); 

  spikeWaveViewB_.amplitudeViewChangedSignal().connect(
						       sigc::mem_fun(spikeWaveViewX_, 
								     &SpikeWaveView::setAmplitudeView)); 
  spikeWaveViewB_.amplitudeViewChangedSignal().connect(
						       sigc::mem_fun(spikeWaveViewY_, 
								     &SpikeWaveView::setAmplitudeView)); 
  spikeWaveViewB_.amplitudeViewChangedSignal().connect(
						       sigc::mem_fun(spikeWaveViewA_, 
								     &SpikeWaveView::setAmplitudeView)); 




  liveButton_.set_active(true); 

  show_all();

  rateTimeline_.viewSignal().connect(sigc::mem_fun(*this, 
						   &TSpikeWin::updateClusterView)); 
  
  liveButton_.signal_toggled().connect(sigc::mem_fun(*this,
						     &TSpikeWin::liveToggle)); 

  
  /// data recovery callback hookup
  somaNetworkCodec_.signalSourceStateChange().connect(sigc::mem_fun(*this, 
								    &TSpikeWin::sourceStateChangeCalback)); 
  
  somaNetworkCodec_.signalTimeUpdate().connect(sigc::mem_fun(*this, 
							     &TSpikeWin::timeUpdateCallback)); 

  somaNetworkCodec_.signalNewTSpike().connect(sigc::mem_fun(*this, 
							    &TSpikeWin::newTSpikeCallback)); 


  set_decorated(false);

  
  setupMenus(); 


  
  // now query state
  somaNetworkCodec_.refreshStateCache(); 


}

void TSpikeWin::setupMenus()
{

  // First we create "Actions", these are things that we want to "do" from our
  // menu 
  refActionGroup_ = Gtk::ActionGroup::create();

  refActionGroup_->add(Gtk::Action::create("Quit", "Quit"), 
		       sigc::mem_fun(*this, &TSpikeWin::on_action_quit)); 
  refActionGroup_->add(Gtk::Action::create("ResetViews", "Reset All Views"), 
		       sigc::mem_fun(*this, &TSpikeWin::on_action_reset_views)); 

  refActionGroup_->add(Gtk::Action::create("ResetData", "Reset All Data History"), 
		       sigc::mem_fun(*this, &TSpikeWin::on_action_reset_data)); 

  refActionGroup_->add(Gtk::Action::create("SourceSettings", "Change Source Settings"), 
		       sigc::mem_fun(*this, &TSpikeWin::on_action_source_settings)); 
  
  refActionGroup_->add(Gtk::Action::create("About", "About TSpikes")); 
  
  




  refUIManager_ = Gtk::UIManager::create();

  Glib::ustring ui_info =
    "<ui>"
    "  <popup name='PopupMenu'>"
    "    <menuitem action='SourceSettings'/>"
    "    <menuitem action='ResetViews'/>"
    "    <menuitem action='ResetData'/>"
    "    <menuitem action='About'/>"
    "    <menuitem action='Quit'/>"
    "  </popup>"
    "</ui>";

  refUIManager_->insert_action_group(refActionGroup_);
  add_accel_group(refUIManager_->get_accel_group());
  refUIManager_->add_ui_from_string(ui_info); 
  


  pMenuPopup_ = dynamic_cast<Gtk::Menu*>(refUIManager_->get_widget("/PopupMenu")); 

  add_events(Gdk::VISIBILITY_NOTIFY_MASK | 
	     Gdk::BUTTON_PRESS_MASK );
  signal_button_press_event().connect(sigc::mem_fun(*this, 
						    &TSpikeWin::on_button_press_event)); 


}

TSpikeWin::~TSpikeWin()
{

}

bool TSpikeWin::on_idle()

{

  clusterViewXY_.invalidate(); 
  clusterViewXA_.invalidate(); 
  clusterViewXB_.invalidate(); 
  clusterViewYA_.invalidate(); 
  clusterViewYB_.invalidate(); 
  clusterViewAB_.invalidate(); 

  spikeWaveViewX_.invalidate(); 
  spikeWaveViewY_.invalidate(); 
  spikeWaveViewA_.invalidate(); 
  spikeWaveViewB_.invalidate(); 

  return true;
}


void TSpikeWin::setTime(somatime_t  stime)
{
  
  abstime_t t= somatimeToAbsTime(stime); 

  // compute reltime
  reltime_t reltime = t - offsetTime_; 

  if (reltime - spVectorStartTime_  > SPVECTDURATION )
    {
      spVectpList_.insert(reltime, new GLSPVect_t); 
      
      spVectorStartTime_ = reltime; 

    }
  
  if (reltime - lastRateTime_ > RATEUPDATE ) 
    {
      rateval_t rv = double(spikeCount_ - lastRateSpikeCount_)/(reltime - lastRateTime_);  
      RatePoint_t rp; 
      rp.rate = rv; 
      rp.time = reltime; 
      rateTimeline_.appendRate(rp); 
      lastRateTime_ = reltime; 
      lastRateSpikeCount_ = spikeCount_; 
    }
  
  abstime_t hours = trunc(t / (60*60)); 
  abstime_t mins = trunc((t - (hours*60)) / 60); 
  abstime_t secs = round(( t - hours * 60 * 60  - mins * 60)); 
  boost::format timeformat("Live \n(%d:%02d:%02d)\n%d"); 
  timeformat % hours % mins % secs % stime; 
  liveButton_.set_label(boost::str(timeformat)); 

}


void TSpikeWin::updateClusterView(bool isLive, reltime_t activePos, float decayRate)
{
  // decayRate is a rate, luminance/alpha drops per unit time

  // compute time range
  reltime_t t2 = activePos; 
  reltime_t t1; 
  if (decayRate > 0 ) {
    reltime_t winsize = 1 / decayRate; 
    t1 = t2 - winsize; 
  } else {
    t1 = spVectpList_.begin()->first; 
  }
  
  // now find iterators
  GLSPVectMap_t::iterator t1i, t2i; 

  // -----------------------------------------------------------
  // get lower bound
  // -----------------------------------------------------------
  t1i =  spVectpList_.lower_bound(t1); 

  if (t1i == spVectpList_.end())
    {
      // there is now element lower than t1

      // DO SOMETHING
      
    } 
  else if (t1i == spVectpList_.begin())
    {
      // the first element, don't dec
    } 
  else 
    {
      // normal, we want to go one further to get a superset of the times
      t1i--; 
    }

  // -----------------------------------------------------------
  // get upper bound
  // -----------------------------------------------------------
  t2i =  spVectpList_.lower_bound(t2); 

  if (t2i == spVectpList_.end())
    {
      // there is now element lower than t1 - that's okay, we want the full
      // range

    } 
  else 
    {
      // normal, we want to go one further to get a superset of the times

      t2i++; 
    }

  
  // now update internal iterators
  clusterViewXY_.setView(t1i, t2i, 
			 decayRate, LOG); 

  clusterViewXA_.setView(t1i, t2i, 
			 decayRate, LOG); 

  clusterViewXB_.setView(t1i, t2i, 
			 decayRate, LOG); 

  clusterViewYA_.setView(t1i, t2i, 
			 decayRate, LOG); 

  clusterViewYB_.setView(t1i, t2i, 
			 decayRate, LOG); 

  clusterViewAB_.setView(t1i, t2i, 
			 decayRate, LOG); 

  clusterViewXY_.invalidate(); 
  clusterViewXA_.invalidate(); 
  clusterViewXB_.invalidate(); 
  clusterViewYA_.invalidate(); 
  clusterViewYB_.invalidate(); 
  clusterViewAB_.invalidate(); 

}


void TSpikeWin::loadExistingSpikes(const std::vector<TSpike_t> & spikes)
{
  std::vector<TSpike_t>::const_iterator pts; 
  for (pts = spikes.begin(); pts != spikes.end(); pts++)
    {
      
      //rtime_t t = float(pts->time) / 10e3; 
      //setTime(t);

    }
}



void TSpikeWin::liveToggle()
{

  rateTimeline_.setLive(liveButton_.get_active()); 
  spikeWaveViewX_.setLive(liveButton_.get_active()); 
  spikeWaveViewY_.setLive(liveButton_.get_active()); 
  spikeWaveViewA_.setLive(liveButton_.get_active()); 
  spikeWaveViewB_.setLive(liveButton_.get_active()); 
  

}

void TSpikeWin::timeUpdateCallback(somatime_t stime)
{

  setTime(stime); 
}

void TSpikeWin::sourceStateChangeCalback(int chan, TSpikeChannelState state)
{

  switch(chan) {
  case 0:
    spikeWaveViewX_.updateState(state); 

    clusterViewXY_.updateState(true, state); 
    clusterViewXA_.updateState(true, state); 
    clusterViewXB_.updateState(true, state); 
    break; 

  case 1:
    spikeWaveViewY_.updateState(state); 

    clusterViewXY_.updateState(false, state);
    clusterViewYA_.updateState(true, state); 
    clusterViewYB_.updateState(true, state); 
    break;

  case 2:
    spikeWaveViewA_.updateState(state); 

    clusterViewXA_.updateState(false, state);
    clusterViewYA_.updateState(false, state); 
    clusterViewAB_.updateState(true, state); 
    break;

  case 3:
    spikeWaveViewB_.updateState(state); 

    clusterViewXB_.updateState(false, state);
    clusterViewYB_.updateState(false, state); 
    clusterViewAB_.updateState(false, state); 
    break;

  }
  
}

void TSpikeWin::newTSpikeCallback(const TSpike_t & ts)
{
  std::vector<GLSpikeWave_t> gls = splitSpikeToGLSpikeWaves(ts, offsetTime_); 
  
  
  spikeWaveViewX_.newSpikeWave(gls[0]); 
  spikeWaveViewY_.newSpikeWave(gls[1]); 
  spikeWaveViewA_.newSpikeWave(gls[2]); 
  spikeWaveViewB_.newSpikeWave(gls[3]); 
  
  GLSpikePoint_t sp = convertTSpikeToGLSpike(ts, offsetTime_); 

  appendTSpikeToSPL(sp, &spVectpList_); 
  spikeCount_++; 

  clusterViewXY_.invalidate(); 
  clusterViewXA_.invalidate(); 
  clusterViewXB_.invalidate(); 
  clusterViewYA_.invalidate(); 
  clusterViewYB_.invalidate(); 
  clusterViewAB_.invalidate(); 



}

bool TSpikeWin::on_button_press_event(GdkEventButton* event)
{
  if( (event->type == GDK_BUTTON_PRESS) &&
      (event->button == 3) )
  {
    pMenuPopup_->popup(event->button, event->time);

    return true; 
  } else if (event->type == GDK_2BUTTON_PRESS) { 
    std::cout << "Double click!" << std::endl;
    set_decorated(!get_decorated()); 

    return true; 
  }
  else
    return false;
}

void TSpikeWin::on_action_quit(void)
{
  std::cout << "Quitting..." << std::endl; 
  hide(); 

}

void TSpikeWin::on_action_source_settings(void)
{
  sourceSettingsWin_.show(); 
  
}

void TSpikeWin::on_action_reset_data()
{
  spVectpList_.clear(); 
  reltime_t rt = abstimeToRelTime(spVectorStartTime_, offsetTime_); 
  spVectpList_.insert(rt, new GLSPVect_t);

  //  spVectpList_.insert((reltime_t)0.0, new GLSPVect_t);

  rateTimeline_.resetData(); 
  // reset offset time

  spikeWaveViewX_.resetData(); 
  spikeWaveViewY_.resetData(); 
  spikeWaveViewA_.resetData(); 
  spikeWaveViewB_.resetData();

  
  clusterViewXY_.resetData(); 
  clusterViewXA_.resetData(); 
  clusterViewXB_.resetData();
  clusterViewYA_.resetData(); 
  clusterViewYB_.resetData(); 
  clusterViewAB_.resetData();
  
}

void TSpikeWin::on_action_reset_views()
{
  
  spikeWaveViewX_.resetView(); 
  spikeWaveViewY_.resetView(); 
  spikeWaveViewA_.resetView(); 
  spikeWaveViewB_.resetView();

  clusterViewXY_.resetView(); 
  clusterViewXA_.resetView(); 
  clusterViewXB_.resetView();
  clusterViewYA_.resetView(); 
  clusterViewYB_.resetView(); 
  clusterViewAB_.resetView();
  

}
