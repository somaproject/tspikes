#ifndef GAINCOMBOSETTING_H
#define GAINCOMBOSETTING_H

#include <gtkmm.h>
#include "somanetcodec.h" 
#include "sourcesettingswin.h"
#include "combosetting.h" 


class GainComboSetting : public ComboSetting
{
 public:
  GainComboSetting(pSomaNetworkCodec_t  nc, chanset_t chanset, 
		   settingmap_t sm); 
  
 private:
  int getSettingFromState(const TSpikeChannelState &);
  TSpikeChannelState  setSettingInState(TSpikeChannelState, int); 
  

}; 
  


#endif 
