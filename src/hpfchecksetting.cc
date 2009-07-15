#include "hpfchecksetting.h"

HPFCheckSetting::HPFCheckSetting(pSomaNetworkCodec_t nc, chanset_t chanset) :
  pnc_(nc), 
  chanset_(chanset), 
  blockSignal_(false)
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

    updateSetting(); 
  }

}

void HPFCheckSetting::setDirty(bool value)
{
  set_inconsistent(value); 

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
  
  // block the signal handler from firing
  blockSignal_ = true; 
  int hpfsetting = -1; 
  bool allSame = true; 
  for (chanset_t::iterator c = chanset_.begin(); c != chanset_.end(); ++c)
    {
      bool newhpf = pnc_->getChannelState(*c).hpf; 

      if (hpfsetting == -1) {
	hpfsetting = newhpf ; 
      } else {
	if (newhpf != hpfsetting) {
	  allSame = false; 
 	}
	hpfsetting = newhpf; 
      }
      
    }
  
  if (allSame) {
    set_active(hpfsetting); 
    setDirty(false); 
  } else {
     // intersection is zero; must have conflict; 
    set_inconsistent(true);
    
  }



  blockSignal_ = false; 

}

void HPFCheckSetting::on_button_clicked(void)
{
  if (blockSignal_) 
    return; 

  // first we dirty the widget
  setDirty(true); 
  // then for each channel we send the updated hpf status
  
  bool newhpf; 
  if (get_active()) {
    newhpf = true; 
  } else {
    newhpf = false; 
  } 

  for (chanset_t::iterator c = chanset_.begin(); c != chanset_.end(); ++c)
    {
      
      TSpikeChannelState state = pnc_->getChannelState(*c); 
      if (state.hpf != newhpf ) {
 	state.hpf = newhpf; 
 	pnc_->setChannelState(*c, state); 
      }
    }
  

}
