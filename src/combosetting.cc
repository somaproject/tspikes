#include "combosetting.h"

ComboSetting::ComboSetting(SomaNetworkCodec * nc, chanset_t chanset, 
			   settingmap_t sm) :
  pnc_(nc), 
  chanset_(chanset), 
  settingMap_(sm)
{

  for (settingmap_t::iterator i = settingMap_.begin(); 
       i != settingMap_.end(); ++i)
    {
      insert_text(i->first, i->second); 
    }
  
  // register for update
  pnc_->signalSourceStateChange().connect(sigc::mem_fun(*this, 
 							&ComboSetting::stateChangeCallback)); 
  signal_changed().connect(sigc::mem_fun(*this,
					 &ComboSetting::on_combo_changed) );
  
  //updateSetting(); 
}

void ComboSetting::stateChangeCallback(int chan, 
					   TSpikeChannelState newstate)
{

  if (chanset_.find(chan) != chanset_.end() ) {

    updateSetting(); 
  }

}

void ComboSetting::setDirty(bool value)
{
  set_sensitive(!value); 

}

void ComboSetting::on_realize()
{

  Gtk::ComboBoxText::on_realize(); 
  updateSetting(); 
}

void ComboSetting::updateSetting()
{
  // Update our current setting with whatever the current
  // state of the network codec is, for the channels we care about
  
  int setting = -1; 
  bool allSame = true; 
  for (chanset_t::iterator c = chanset_.begin(); c != chanset_.end(); ++c)
    {
      int newsetting = getSettingFromState(pnc_->getChannelState(*c)); 

      if (setting == -1) {
	setting = newsetting ; 
      } else {
	
	if (newsetting != setting) {
	  allSame = false; 
	}
	setting = newsetting; 
      }
	
    }
  if (allSame) {

    set_active_text(settingMap_[setting]); 
  } else {
    // intersection is zero; must have conflict; 
    set_active_text(""); 
  }

  setDirty(false); 

}

void ComboSetting::on_combo_changed(void)
{

  // first we dirty the widget
  setDirty(true); 

  // then for each channel we send the updated setting
  int newsetting = -1; 
  for (settingmap_t::iterator g = settingMap_.begin(); 
       g != settingMap_.end(); g++) {
    if (g->second == get_active_text()) {
      newsetting = g->first;
    }
  }
  if (newsetting < 0) {
    return; 
  }
    
  for (chanset_t::iterator c = chanset_.begin(); c != chanset_.end(); ++c)
    {
      
      TSpikeChannelState state = pnc_->getChannelState(*c); 
      if (getSettingFromState(state) != newsetting ) {
	state = setSettingInState(state, newsetting); 
	
	pnc_->setChannelState(*c, state); 
      }
    }
  

}


int ComboSetting::getSettingFromState(const TSpikeChannelState & state)
{
  std::cerr << "error : should never call ComboSetting::getSettingFromState()" << std::endl; 
  return 0; 
}

TSpikeChannelState ComboSetting::setSettingInState(TSpikeChannelState state, 
						       int setting)
{
  std::cerr << "error : should never call ComboSetting::setSettingInState()" << std::endl; 


  return state; 

}
