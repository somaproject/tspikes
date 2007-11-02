#include "gaincombosetting.h"

GainComboSetting::GainComboSetting(SomaNetworkCodec * nc, chanset_t chanset) :
  pnc_(nc), 
  chanset_(chanset)
{
  gainMap_[0] = "0"; 
  gainMap_[100] = "100"; 
  gainMap_[200] = "200"; 
  gainMap_[500] = "500"; 
  gainMap_[1000] = "1000"; 
  gainMap_[2000] = "2000"; 
  gainMap_[5000] = "5000"; 
  gainMap_[10000] = "10000"; 

  for (gainmap_t::iterator i = gainMap_.begin(); 
       i != gainMap_.end(); ++i)
    {
      insert_text(i->first, i->second); 
    }
  
  // register for update
  pnc_->signalSourceStateChange().connect(sigc::mem_fun(*this, 
 							&GainComboSetting::stateChangeCallback)); 
  signal_changed().connect(sigc::mem_fun(*this,
					 &GainComboSetting::on_combo_changed) );
  
  updateSetting(); 
}

void GainComboSetting::stateChangeCallback(int chan, 
					   TSpikeChannelState newstate)
{

  if (chanset_.find(chan) != chanset_.end() ) {
    std::cout << "For us" << std::endl; 
    updateSetting(); 
  }

}

void GainComboSetting::setDirty(bool value)
{
  set_sensitive(!value); 

}

void GainComboSetting::on_realize()
{
  std::cout << "GainComboSetting::on_realize()" << std::endl; 
  Gtk::ComboBoxText::on_realize(); 
  updateSetting(); 
}

void GainComboSetting::updateSetting()
{
  // Update our current setting with whatever the current
  // state of the network codec is, for the channels we care about
  
  int gain = -1; 
  bool allSame = true; 
  for (chanset_t::iterator c = chanset_.begin(); c != chanset_.end(); ++c)
    {
      int newgain = pnc_->getChannelState(*c).gain; 

      if (gain == -1) {
	gain = newgain ; 
      } else {
	
	if (newgain != gain) {
	  allSame = false; 
	}
	gain = newgain; 
      }
	
    }
  if (allSame) {
    std::cout << "Setting gain to " << gain << std::endl;
    set_active_text(gainMap_[gain]); 
  } else {
    // intersection is zero; must have conflict; 
    set_active_text(""); 
  }

  setDirty(false); 

}

void GainComboSetting::on_combo_changed(void)
{
  // send the actual signal 
  std::cout << "On Combo Changed" << std::endl; 
  // first we dirty the widget
  setDirty(true); 
  // then for each channel we send the updated gain
  
  
  int newgain = -1; 
  for (gainmap_t::iterator g = gainMap_.begin(); 
       g != gainMap_.end(); g++) {
    if (g->second == get_active_text()) {
      newgain = g->first;
    }
  }
  if (newgain < 0) {
    return; 
  }
    
  for (chanset_t::iterator c = chanset_.begin(); c != chanset_.end(); ++c)
    {
      
      TSpikeChannelState state = pnc_->getChannelState(*c); 
      if (state.gain != newgain ) {
	state.gain = newgain; 
	
	pnc_->setChannelState(*c, state); 
      }
    }
  

}
