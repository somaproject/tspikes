#ifndef FILTERCOMBOSETTING_H
#define FILTERCOMBOSETTING_H

#include <gtkmm.h>
#include "somanetcodec.h" 
#include "sourcesettingswin.h"
#include "combosetting.h" 


class FilterComboSetting : public ComboSetting
{
 public:
  FilterComboSetting(SomaNetworkCodec * nc, chanset_t chanset, 
		   settingmap_t sm); 
  
 private:
  int getSettingFromState(const TSpikeChannelState &);
  TSpikeChannelState  setSettingInState(TSpikeChannelState, int); 
  

}; 
  


#endif 
