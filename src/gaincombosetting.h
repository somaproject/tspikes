#ifndef GAINCOMBOSETTING_H
#define GAINCOMBOSETTING_H

#include <gtkmm.h>
#include "somanetcodec.h" 
#include "sourcesettingswin.h"



typedef std::map<int, std::string> gainmap_t; 
class GainComboSetting : public Gtk::ComboBoxText
{
 public:
  GainComboSetting(SomaNetworkCodec * nc, chanset_t chanset); 
  
 private:

  SomaNetworkCodec * pnc_; 
  chanset_t chanset_; 
  gainmap_t gainMap_; 
  
  void stateChangeCallback(int, TSpikeChannelState); 
  void updateSetting(); 
  void setDirty(bool value); 
  void on_combo_changed(void); 
  void on_realize(void); 

}; 
  


#endif 
