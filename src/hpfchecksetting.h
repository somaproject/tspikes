#ifndef HPFCHECKSETTING_H
#define HPFCHECKSETTING_H

#include <gtkmm.h>
#include "somanetcodec.h" 
#include "sourcesettingswin.h"



class HPFCheckSetting : public Gtk::CheckButton
{
 public:
  HPFCheckSetting(SomaNetworkCodec * nc, chanset_t chanset); 
  
 private:

  SomaNetworkCodec * pnc_; 
  chanset_t chanset_; 
  
  
  void stateChangeCallback(int, TSpikeChannelState); 
  void updateSetting(); 
  void setDirty(bool value); 
  void on_button_clicked(void); 
  void on_realize(void); 

}; 
  


#endif 
