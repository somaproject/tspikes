#ifndef THOLDENTRYSETTING_H
#define THOLDENTRYSETTING_H

#include <gtkmm.h>
#include "somanetcodec.h" 
#include "sourcesettingswin.h"

/* 
This lets us enter the threshold

*/


typedef std::map<int, std::string> settingmap_t; 

class TholdEntrySetting : public Gtk::Entry
{
 public:
  TholdEntrySetting(pSomaNetworkCodec_t  nc, chanset_t chanset); 

  void updateSetting();   
 private:

  pSomaNetworkCodec_t pnc_; 
  chanset_t chanset_; 
  
  void stateChangeCallback(int, TSpikeChannelState); 

  void setDirty(bool value); 
  void on_activate(void); 
  void on_realize(void); 

  int getSettingFromState(const TSpikeChannelState &); 
  TSpikeChannelState  setSettingInState(TSpikeChannelState, int); 
  
}; 
  


#endif // THOLDENTRYSETTING_H
