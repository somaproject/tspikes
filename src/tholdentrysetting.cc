#include "tholdentrysetting.h"
#include "voltage.h"

TholdEntrySetting::TholdEntrySetting(SomaNetworkCodec * nc, chanset_t chanset) :
  pnc_(nc), 
  chanset_(chanset)

{

  // register for update
  pnc_->signalSourceStateChange().connect(sigc::mem_fun(*this, 
 							&TholdEntrySetting::stateChangeCallback)); 
  
  //updateSetting(); 
}

void TholdEntrySetting::stateChangeCallback(int chan, 
					   TSpikeChannelState newstate)
{

  if (chanset_.find(chan) != chanset_.end() ) {
    updateSetting(); 
  }

}

void TholdEntrySetting::setDirty(bool value)
{
  set_sensitive(!value); 

}

void TholdEntrySetting::on_realize()
{

  Gtk::Entry::on_realize(); 
  updateSetting(); 
}

void TholdEntrySetting::updateSetting()
{
  // Update our current setting with whatever the current
  // state of the network codec is, for the channels we care about
  
  int setting = -1; 
  bool allSame = true, firstPass = true; 
  for (chanset_t::iterator c = chanset_.begin(); c != chanset_.end(); ++c)
    {
      int newsetting = getSettingFromState(pnc_->getChannelState(*c)); 
      
      if (firstPass ) {
 	setting = newsetting ; 
	firstPass = false; 
      } else {
	
 	if (newsetting != setting) {
 	  allSame = false; 
 	}
 	setting = newsetting; 
      }
      
    }
  if (allSame) {

    std::string valstr = Voltage(setting).str(); 
    set_text(valstr); 
    
  } else {
    // intersection is zero; must have conflict; 
    set_text(""); 
  }

  setDirty(false); 

}

void TholdEntrySetting::on_activate(void)
{

  try {
    Voltage V(get_text()); 
    
    // first we dirty the widget
    setDirty(true); 
    
    
    // then for each channel we send the updated setting
    int32_t newsetting = V.to_nv(); 
    for (chanset_t::iterator c = chanset_.begin(); c != chanset_.end(); ++c)
      {
	
	TSpikeChannelState state = pnc_->getChannelState(*c); 
	if (state.threshold != newsetting ) {
	  state.threshold = newsetting; 
	  pnc_->setChannelState(*c, state); 
	}
      }
    
  } catch (ConversionError e) {
    std::cerr << "Invalid entry" << std::endl; 
    
  }

  

}


int TholdEntrySetting::getSettingFromState(const TSpikeChannelState & state)
{

  return state.threshold; 
}

TSpikeChannelState TholdEntrySetting::setSettingInState(TSpikeChannelState state, 
						       int setting)
{

  return state; 

}
