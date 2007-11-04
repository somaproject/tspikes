#ifndef COMBOSETTING_H
#define COMBOSETTING_H

#include <gtkmm.h>
#include "somanetcodec.h" 
#include "sourcesettingswin.h"

/* 
This is the base class for enumerable settings. 

*/


typedef std::map<int, std::string> settingmap_t; 

class ComboSetting : public Gtk::ComboBoxText
{
 public:
  ComboSetting(SomaNetworkCodec * nc, chanset_t chanset, settingmap_t sm); 
  
 private:

  SomaNetworkCodec * pnc_; 
  chanset_t chanset_; 
  gainmap_t settingMap_; 
  
  void stateChangeCallback(int, TSpikeChannelState); 
  void updateSetting(); 
  void setDirty(bool value); 
  void on_combo_changed(void); 
  void on_realize(void); 
  virtual int getSettingFromState(const TSpikeChannelState &);
  virtual TSpikeChannelState  setSettingInState(TSpikeChannelState, int); 

}; 
  


#endif // COMBOSETTING_H
