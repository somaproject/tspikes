#include "hpfchecksetting.h"

HPFCheckSetting::HPFCheckSetting(SomaNetworkCodec * nc, chanset_t chanset) :
  pnc_(nc), 
  chanset_(chanset)
{
  
  // register for update
  pnc_->signalSourceStateChange().connect(sigc::mem_fun(*this, 
 							&HPFCheckSetting::stateChangeCallback)); 
  signal_clicked().connect(sigc::mem_fun(*this,
					 &HPFCheckSetting::on_button_clicked) );
  
  updateSetting(); 
}

void HPFCheckSetting::stateChangeCallback(int chan, 
					   TSpikeChannelState newstate)
{

  if (chanset_.find(chan) != chanset_.end() ) {
    std::cout << "For us" << std::endl; 
    updateSetting(); 
  }

}

void HPFCheckSetting::setDirty(bool value)
{
  set_sensitive(!value); 

}

void HPFCheckSetting::on_realize()
{
  Gtk::CheckButton::on_realize(); 
  updateSetting(); 
}

void HPFCheckSetting::updateSetting()
{
  // Update our current setting with whatever the current
  // state of the network codec is, for the channels we care about
  
//   int gain = -1; 
//   bool allSame = true; 
//   for (chanset_t::iterator c = chanset_.begin(); c != chanset_.end(); ++c)
//     {
//       int newgain = pnc_->getChannelState(*c).gain; 

//       if (gain == -1) {
// 	gain = newgain ; 
//       } else {
	
// 	if (newgain != gain) {
// 	  allSame = false; 
// 	}
// 	gain = newgain; 
//       }
	
//     }
//   if (allSame) {
//     std::cout << "Setting gain to " << gain << std::endl;
//     set_active_text(gainMap_[gain]); 
//   } else {
//     // intersection is zero; must have conflict; 
//     set_active_text(""); 
//   }

//   setDirty(false); 

}

void HPFCheckSetting::on_button_clicked(void)
{

  // first we dirty the widget
  setDirty(true); 
  // then for each channel we send the updated gain
  
  
//   int newgain = -1; 
//   for (gainmap_t::iterator g = gainMap_.begin(); 
//        g != gainMap_.end(); g++) {
//     if (g->second == get_active_text()) {
//       newgain = g->first;
//     }
//   }
//   if (newgain < 0) {
//     return; 
//   }
    
//   for (chanset_t::iterator c = chanset_.begin(); c != chanset_.end(); ++c)
//     {
      
//       TSpikeChannelState state = pnc_->getChannelState(*c); 
//       if (state.gain != newgain ) {
// 	state.gain = newgain; 
	
// 	pnc_->setChannelState(*c, state); 
//       }
//     }
  

}
